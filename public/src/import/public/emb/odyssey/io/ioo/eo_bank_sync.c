/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_bank_sync.c $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2023
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_bank_sync.c
// *! TITLE       :
// *! DESCRIPTION : Run Bank Sync by moving one of the banks
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23082800 |vbr     | EWM 308211: Add checking of abort after Gen1/2 align wait (before ber check)
// vbr23082300 |vbr     | EWM 306166: Enable the clk phase detector for Gen 1/2
// vbr23060100 |vbr     | EWM 305986: For DD1, disable error on Gen1/2 bank sync fail; add debug log.
// vbr23051500 |vbr     | EWM 304759: Use bump_ui_mode with fw_inertia_bump=0 through all of gen1/2 bank sync.
// vbr23041100 |vbr     | Initial Rev (P11). See Github m*ster_a3 for P10 version.
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "eo_bank_sync.h"
#include "io_logger.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value of IO_DEBUG_LEVEL is 2
#define EO_BANK_SYNC_DBG_LVL 3


// Assumption checking
PK_STATIC_ASSERT(rx_berpl_cnt_en_exp_sel_alias_width == 6);
PK_STATIC_ASSERT(rx_berpl_count_en_shift     == (rx_berpl_cnt_en_exp_sel_alias_shift + 5));
PK_STATIC_ASSERT(rx_berpl_exp_data_sel_shift == (rx_berpl_cnt_en_exp_sel_alias_shift + 4));
PK_STATIC_ASSERT(rx_berpl_mask_mode_shift    == (rx_berpl_cnt_en_exp_sel_alias_shift + 0));
PK_STATIC_ASSERT(rx_ber_en_timer_sel_alias_width == 5);
PK_STATIC_ASSERT(rx_ber_en_shift        == (rx_ber_en_timer_sel_alias_shift + 4));
PK_STATIC_ASSERT(rx_ber_timer_sel_shift == (rx_ber_en_timer_sel_alias_shift + 0));
PK_STATIC_ASSERT(rx_ber_timer_sel_width == 4);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Max Sync Time, No Spread and phase_step >= 0.25 (see Workbook and EWM 289978):
//   Gen5 = 500ns, Gen4 = 1us, Gen3 = 2us, Gen2 = 4us, Gen1 = 8us
//
// Bank Sync Procedure
// For AXO/Gen3-5, can just wait 2us for the CDR align mode to synchronize the banks.
// For Gen1-2, need to follow this general procedure (EWM 302633):
//   1) Enable CDR bump UI mode and CDR bank align mode on ALT bank.
//   2) Run CDR bank align mode for 8us.
//      This could end in true alignment or false alignment (undetectable by CDR).
//   3) Perform two BER checks: one using the Bank A clock and one using the Bank B clock.
//      If both BER checks pass, the banks are aligned and the procedure finishes with the final step.
//      If either BER check fails, the banks are not align the procedure continues to the next step.
//   4) Bump 1 UI in the same direction as align mode moves (rx_pr_invalid_lock_filter_dir, default left).
//   5) Repeat from step 2 until banks are aligned in step 3.
//   6) Disable CDR bump UI mode and CDR bank align mode.
//
// Notes:
//   A) CDR Bank align mode is left enabled on the ALT bank through the entire Gen1/2 procedure.
//      This is fine since it has no effect when the banks are in sync or a bump is executing.
//   B) CDR Bump UI mode is left enabled on the ALT bank through the entire Gen1/2 procedure.
//      This is done to force fw_inertia=0 to prevent bad flywheel values with certain patterns (EWM 304759).
//      This is fine since the bump UI mode phase_step is set to >=0.25 to still meet the 8us bank align time;
//      this phase_step should be small enough to still bump correctly since there is no spread in PCIe.
//   C) The RLM clock select could be left in any state when the banks are in sync, but
//      we set it back to the sync_bank for consistency.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int eo_bank_sync(t_gcr_addr* gcr_addr, t_bank sync_bank, t_init_cal_mode cal_mode)
{
    set_debug_state(0x0B00, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Start
    int status = rc_no_error;
    bool pcie_gen1_2_cal = (cal_mode == C_PCIE_GEN1_CAL) || (cal_mode == C_PCIE_GEN2_CAL);

    // 1a) Enable CDR Bump UI Mode for Gen1/2 (EWM 304759).
    //     Enable the circuit clock phase detector as part of the CDR bank align (EWM 306166).
    //     PD is only available on ZMetis DD2+ and detects when the banks are 4 or more UI apart.
    int sel_ab_bitfield_val = bank_to_bitfield_ab_mask(sync_bank);

    if (pcie_gen1_2_cal)
    {
        put_ptr_field(gcr_addr, rx_pr_bump_ui_mode_ab_alias, sel_ab_bitfield_val, read_modify_write); //pl
        put_ptr_field(gcr_addr, rx_pr_bank_align_clk_phase_det_en, 0b1, read_modify_write); //pl
    }

    // 1b) Enable CDR Bank Align Mode
    int edge_track_cntl_val = (sync_bank == bank_a) ? cdr_a_lcl_align_cdr_b_lcl : cdr_a_lcl_cdr_b_lcl_align;
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, edge_track_cntl_val, fast_write);

    // Rest of procedure depends on cal mode
    // PSL not_gen12
    if (!pcie_gen1_2_cal)
    {
        ///////////////////////
        // AXO / PCIe Gen3-5 //
        ///////////////////////
        set_debug_state(0x0B03, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync AXO/Gen3-5

        // 1c) Wait 2 us for CDR bank align: Sleep if not PCIe RxEqEval, otherwise busy wait.
        int rx_eq_eval = mem_pg_field_get(rx_running_eq_eval);

        // PSL not_rxeqeval
        if (!rx_eq_eval)
        {
            io_wait_us(get_gcr_addr_thread(gcr_addr), 2); //sleep
        }
        else
        {
            io_spin_us(2); //busy wait, see assumptions above
        }
    }
    else
    {
        /////////////////
        // PCIe Gen1/2 //
        /////////////////
        set_debug_state(0x0B01, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Gen1-2

        // BERM Config
        //   Enable PL BERM logic clocks
        //   Select BERM comparison between banks
        //   Disable BERM mask mode
        //   Enable PG clocks for the timer
        //   Set timer to saturate at 64 slow_clock cycles (0xD = 2^6)
        //     Gen1: max_errs = (2.5Gbps / 32Gbps) * (64 slow clock) * (8 grid clock/slow clock) * (16 bits/grid clock) * (0.15) = 96 bits
        //     Gen2: max_errs = (5.0Gbps / 32Gbps) * (64 slow clock) * (8 grid clock/slow clock) * (16 bits/grid clock) * (0.15) = 192 bits
        const int c_max_errs_gen1 = 96;
        const int c_max_errs_gen2 = 192;
        put_ptr_field(gcr_addr, rx_berpl_cnt_en_exp_sel_alias, 0x20, read_modify_write); // pl
        put_ptr_field(gcr_addr, rx_ber_en_timer_sel_alias,     0x1D,
                      fast_write); // pg, nothing else in register except rx_ber_pcie_mode

        uint32_t loop_timeout = 3; // Expect no more than 3 attempts (3 aligns + 2 bumps) to get the true alignment
        bool aligned = false;

        while (!aligned && (loop_timeout > 0))
        {
            loop_timeout--;

            // 1c) Wait 8 us (sleep) for CDR bank align
            io_wait_us(get_gcr_addr_thread(gcr_addr), 8); //sleep
            status = check_rx_abort(gcr_addr);

            // PSL gen12_abort_align
            if (status)
            {
                set_debug_state(0x0B0D,
                                EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Gen1/2 Abort after Align step (1c) and before BERM step (2)
                break;
            }

            // 2) Perform Main-Alt BER check
            set_debug_state(0x0B0C, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync BERM Check
            aligned = true;
            int i = 0;

            do
            {
                // 2a) Select Bank B clock in first loop, then Bank A clock in second loop
                put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
                put_ptr_field(gcr_addr, rx_rlm_clk_sel_a, i, read_modify_write);
                i++;
                put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // turn on cal lane sel

                // 2b) BER measurement
                put_ptr_field(gcr_addr, rx_ber_reset, 0b1, fast_write); //pg, pulse to start BERM
                int berm_running;

                do
                {
                    berm_running = get_ptr_field(gcr_addr, rx_ber_timer_running);
                }
                while ( berm_running );

                // 2c) BER Check for being out of sync with either clock
                int max_errs;

                // PSL ber_gen1
                if (cal_mode == C_PCIE_GEN1_CAL)
                {
                    max_errs = c_max_errs_gen1;
                }
                else
                {
                    max_errs = c_max_errs_gen2;
                }

                int ber_count = get_ptr_field(gcr_addr, rx_berpl_count);
                aligned &= (ber_count < max_errs);
            }
            while (aligned && (i < 2));

            // PSL gen12_not_aligned
            if (!aligned && (loop_timeout > 0))
            {
                set_debug_state(0x0B0B, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Bump Back

                status = check_rx_abort(gcr_addr);

                // PSL gen12_abort_bump
                if (status)
                {
                    set_debug_state(0x0B0A,
                                    EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Gen1/2 Abort after BERM step (2) and before Bump step (3)
                    break;
                }

                // 3a) Bump UI Mode (previously enabled) with bit_lock_done set
                put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias,  sel_ab_bitfield_val, fast_write);

                // 3b) Bump 1 UI
                //     Assume bump operation is faster than register writes (don't poll on bump_in_progress)
                // PSL gen12_bump_bank_a
                if (sync_bank == bank_a)
                {
                    put_ptr_field(gcr_addr, rx_pr_bump_slsr_1ui_ab_alias, rx_pr_bump_sl_1ui_a_mask >> rx_pr_bump_slsr_1ui_ab_alias_shift,
                                  fast_write); //strobe
                }
                else
                {
                    put_ptr_field(gcr_addr, rx_pr_bump_slsr_1ui_ab_alias, rx_pr_bump_sl_1ui_b_mask >> rx_pr_bump_slsr_1ui_ab_alias_shift,
                                  fast_write); //strobe
                }

                // 3c) Clear bit_lock_done (Bump UI mode disabled later)
                put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias,  sel_ab_bitfield_val, fast_write);
            }
        } //while(!aligned)

        // Report errors if not aligned and not aborting
        // PSL bank_sync_fail
        if (!aligned && (status == rc_no_error))
        {
            ADD_LOG(DEBUG_GEN12_BANK_SYNC_FAIL, gcr_addr, 0x0);

            if (is_zm_dd1())
            {
                // EWM 305986: Abort cleanly if bank sync fails on ZMetis DD1
                status = abort_clean_code;
            }
            else
            {
                set_fir(fir_code_bad_lane_warning);
                status = rc_error;
            }
        }

        // 6a) Disable Bump UI Mode and circuit clock phase detector
        put_ptr_field(gcr_addr, rx_pr_bump_ui_mode_ab_alias, 0b00, read_modify_write); //pl
        put_ptr_field(gcr_addr, rx_pr_bank_align_clk_phase_det_en, 0b0, read_modify_write); //pl

        // 2d) Restore rlm_clk_sel to the Alt (sync) bank
        put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
        int rlm_clk_sel_val = cal_bank_to_rlmclk_sel_a(sync_bank);
        put_ptr_field(gcr_addr, rx_rlm_clk_sel_a, rlm_clk_sel_val, read_modify_write);
        put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // turn on cal lane sel

        // 4d) Clock Gate BER logic
        put_ptr_field(gcr_addr, rx_berpl_count_en, 0b0, read_modify_write); //pl
        put_ptr_field(gcr_addr, rx_ber_en,         0b0,
                      fast_write); // pg, nothing else in register except rx_ber_timer_sel and rx_ber_pcie_mode

        if ((status & abort_code) == 0)
        {
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }
    } //if(gen12)

    // 6b) Disable CDR Bank Align Mode
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);

    set_debug_state(0x0B0F, EO_BANK_SYNC_DBG_LVL); // DEBUG - Bank Sync Done

    if (!status)
    {
        status = check_rx_abort(gcr_addr);
    }

    return status;
} //eo_bank_sync
