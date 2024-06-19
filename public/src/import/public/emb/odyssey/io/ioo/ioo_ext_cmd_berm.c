/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_ext_cmd_berm.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_lab_code.c
// *! TITLE       :
// *! DESCRIPTION : Lab specific code
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr22060200 |vbr     | Initial Rev (space holder)
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ppe_img_reg_const_pkg.h"
#include "ppe_fw_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "ioo_common.h"
#include "eo_bank_sync.h"

#define BERM_CONTROL_KILL     0x8000
#define BERM_CONTROL_STOP     0x4000
#define BERM_CONTROL_BANK     0x2000
#define BERM_CONTROL_DEPTH    0x0003

#define BERM_THREAD_CONTROL_START    0x8000

#define BERM_STATUS_DONE      0x8000
#define BERM_STATUS_RUNNING   0x4000


#define SPACE_A 4
#define SPACE_B 32




// Format
// [00:03] 04b : Timer Loops
// [04:15] 12b : Errors
#define BERM_TIMER_MASK 0xF000
#define BERM_ERROR_MASK 0x0FFF

#define BERM_TIMER_SHIFT 12
#define BERM_ERRORS_SHIFT 0

extern uint16_t _debug_log_start __attribute__((section("debuglog")));
uint16_t* g_logspace = &_debug_log_start;

int pr_stepper_run(t_gcr_addr* gcr_addr, const t_bank bank, const int32_t i_data_offset, const int32_t i_edge_offset)
{

    uint32_t l_pr_step_command = (rx_mini_pr_step_a_ns_data_run_mask | rx_mini_pr_step_a_ns_edge_run_mask) |
                                 (rx_mini_pr_step_a_ew_data_run_mask | rx_mini_pr_step_a_ew_edge_run_mask);

    if (bank != bank_a)
    {
        l_pr_step_command = (rx_mini_pr_step_b_ns_data_run_mask | rx_mini_pr_step_b_ns_edge_run_mask) |
                            (rx_mini_pr_step_b_ew_data_run_mask | rx_mini_pr_step_b_ew_edge_run_mask);
    }

    uint32_t l_edge_offset_field = IntToTwosComp(i_edge_offset, rx_mini_pr_step_edge_adj_width);
    uint32_t l_data_offset_field = IntToTwosComp(i_data_offset, rx_mini_pr_step_data_adj_width);
    uint32_t l_pr_step_adjust = l_data_offset_field << rx_mini_pr_step_data_adj_shift | l_edge_offset_field;

    put_ptr_fast(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr, rx_mini_pr_step_data_edge_adj_full_reg_endbit,
                 l_pr_step_adjust);

    // Writing this sets the run bits and clears the done bits
    put_ptr_fast(gcr_addr, rx_mini_pr_step_run_done_full_reg_addr, rx_mini_pr_step_run_done_full_reg_endbit,
                 l_pr_step_command);

    uint32_t l_done = 0;

    do
    {
        l_done = get_ptr(gcr_addr, rx_mini_pr_step_run_done_full_reg_addr, rx_mini_pr_step_run_done_full_reg_startbit,
                         rx_mini_pr_step_run_done_full_reg_endbit);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }
    while ((l_done & (rx_mini_pr_step_a_done_alias_mask | rx_mini_pr_step_b_done_alias_mask)) == 0);

    return 0;
}


int apply_pr_offset(t_gcr_addr* gcr_addr, const t_bank bank, const int i_total_offset)
{
    int l_max_data_offset = 0;
    int l_data_offset = 0;
    int l_edge_offset = 0;
    int pr_vals[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}

    read_active_pr(gcr_addr, bank, pr_vals);

    if (i_total_offset > 0) // Positive Offset
    {
        l_max_data_offset = 31 - max(pr_vals[prDns_i], pr_vals[prDew_i]);

        if (i_total_offset <= l_max_data_offset)
        {
            l_data_offset = i_total_offset;
        }
        else
        {
            l_data_offset = l_max_data_offset;
            l_edge_offset = - (i_total_offset - l_data_offset);

            if ((min(pr_vals[prEns_i], pr_vals[prEew_i]) + l_edge_offset) < 0)
            {
                return 1;
            }
        }
    }
    else if (i_total_offset < 0)   // Negative Offset
    {
        l_max_data_offset = 0 - min(pr_vals[prDns_i], pr_vals[prDew_i]);

        if (i_total_offset >= l_max_data_offset)
        {
            l_data_offset = i_total_offset;
        }
        else
        {
            l_data_offset = l_max_data_offset;
            l_edge_offset = - (i_total_offset - l_data_offset);

            if ((max(pr_vals[prEns_i], pr_vals[prEew_i]) + l_edge_offset) > 31)
            {
                return 1;
            }
        }
    }

    return pr_stepper_run(gcr_addr, bank, l_data_offset, l_edge_offset);
}

/**
 * @brief Runs a Bit Error rate Measurement on a given X/Y location
 * @param[inout] gcr_addr         Target Information
 * @param[in   ] i_lane_mask_rx   Rx Lane Mask
 * @retval void
 */
int ioo_berm_init(t_gcr_addr* gcr_addr,
                  const uint32_t i_lane_mask_rx,
                  const t_bank i_bank,
                  uint16_t* io_logspace)
{
    uint16_t l_lane = 0;
    uint16_t l_count = 0;
    t_init_cal_mode l_cal_mode = C_AXO_CAL;


    int32_t l_pattern_sel = 0;

    // Step 0: Setup the Debug Log Space
    // Set the entries to max so our data does not get overwritten
    img_field_put(ppe_debug_log_num, 127);

    // Clear the Debug Log (Note the ptr is defined as 16b sizes which is why we are indexing to 255)
    for (; l_count < 72; ++l_count)
    {
        io_logspace[l_count] = 0x0;
    }

    // Step 1: Power On, Bank Sync, Enable BER, Apply Offsets
    set_gcr_addr_lane(gcr_addr, 0);
    put_ptr_field(gcr_addr, rx_ber_en, 1, fast_write); // Per-Group

    mem_pg_field_put(rx_recal_abort_0_15, (i_lane_mask_rx >> 16));
    mem_pg_field_put(rx_recal_abort_16_23, (i_lane_mask_rx >> (16 - rx_recal_abort_16_23_width)));

    clear_all_cal_lane_sel(
        gcr_addr); // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.

    for (l_lane = 0; l_lane < 32; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);

        // put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
        // put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, l_lane, read_modify_write);

        alt_bank_psave_clear_and_wait(gcr_addr);

        // If we want to test the current main bank, we have to sync the alt bank before
        //   we switch over, or else we will cause errors downstream on the dl
        eo_bank_sync(gcr_addr, (get_ptr_field(gcr_addr, rx_bank_sel_a) == 1) ? bank_b : bank_a, l_cal_mode);

        // Set the correct bank under test, so we do not corrupt main data
        set_cal_bank(gcr_addr, i_bank);

        int bit_lock_done_clr_regval = bank_to_bitfield_ab_mask(i_bank);
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, bit_lock_done_clr_regval, fast_write);


        if (i_bank == bank_a)
        {
            put_ptr_field(gcr_addr, rx_pr_external_mode_a, 1, read_modify_write);
        }
        else
        {
            // bank_b
            put_ptr_field(gcr_addr, rx_pr_external_mode_b, 1, read_modify_write);
        }


        // Enable BER
        put_ptr_field(gcr_addr, rx_berpl_count_en, 1, read_modify_write);
        put_ptr_field(gcr_addr, rx_berpl_exp_data_sel, 0, read_modify_write);
        // put_ptr_field(gcr_addr, rx_berpl_mask_mode, 0, read_modify_write);
        // put_ptr_field(gcr_addr, rx_berpl_lane_invert, 0, read_modify_write);
        //put_ptr_field(gcr_addr, rx_berpl_count_en, 1, fast_write); // does the above scoms in a single write
        put_ptr_field(gcr_addr, rx_berpl_pattern_sel, l_pattern_sel, read_modify_write);
        put_ptr_field(gcr_addr, rx_berpl_sat_thresh, 0xFFF, fast_write);

        if( l_pattern_sel )
        {
            put_ptr_field(gcr_addr, rx_berpl_exp_data_sel, 1, read_modify_write);
            put_ptr_field(gcr_addr, rx_berpl_prbs_seed_mode, 0, read_modify_write);
            put_ptr_field(gcr_addr, rx_berpl_prbs_seed_mode, 1, read_modify_write);
            uint32_t l_timeout = 0;

            while (get_ptr_field(gcr_addr, rx_berpl_prbs_seed_done) == 0 && l_timeout < 100)
            {
                ++l_timeout;
            }
        }

        // Clear the Stickys
        put_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky_clr, 1, read_modify_write);
        put_ptr_field(gcr_addr, rx_mini_pr_step_rollover_sticky_clr, 1, read_modify_write);

        // put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
    }

    return rc_no_error;
}

int ioo_berm_run(t_gcr_addr* gcr_addr,
                 const uint32_t i_lane_mask_rx,
                 const t_bank i_bank,
                 const int32_t l_pr_offset,
                 const int32_t l_dac_offset,
                 uint16_t* i_control,
                 uint16_t* io_data)
{
    uint16_t l_lane = 0;
    uint16_t l_count = 0;
    int32_t l_ber_timer_sel = 15;
    uint16_t l_rx_berpl_count = 0;
    uint32_t l_done_mask = 0x00;
    uint32_t l_accurate_mask = 0x00;
    uint32_t l_xskip_mask = 0x00;
    uint32_t l_yskip_mask = 0x00;
    int16_t l_max_depth = 0;
    int32_t l_depth = i_control[0] & BERM_CONTROL_DEPTH;

    if (l_depth == 0)
    {
        l_max_depth = 15;
    }
    else if (l_depth == 1)
    {
        l_max_depth = 8;
    }
    else
    {
        l_max_depth = 5;
    }

    for (l_count = 0; l_count < 24; ++l_count)
    {
        io_data[l_count] = 0x0;
    }


    for (l_lane = 0; l_lane < 32; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);

        apply_rx_dac_offset(gcr_addr, data_only, i_bank, l_dac_offset);

        if (get_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky))
        {
            l_yskip_mask |= (0x80000000 >> l_lane);
        }

        int32_t rc = apply_pr_offset(gcr_addr, i_bank, l_pr_offset);

        if (rc || get_ptr_field(gcr_addr, rx_mini_pr_step_rollover_sticky))
        {
            l_xskip_mask |= (0x80000000 >> l_lane);
        }
    }


    // Step 2: Collect the BER Data
    // - The amount of dwell time is configurable based on the depth
    l_done_mask |= l_xskip_mask | l_yskip_mask;
    l_ber_timer_sel = 14;

    for (l_count = 0; l_count <= l_max_depth; ++l_count)
    {
        // Skip collectin BER measurements if none of the lanes are valid
        if ((l_done_mask | l_accurate_mask) == i_lane_mask_rx)
        {
            break;
        }

        set_gcr_addr_lane(gcr_addr, 0);
        put_ptr_field(gcr_addr, rx_ber_timer_sel, l_ber_timer_sel, read_modify_write);
        put_ptr_field(gcr_addr, rx_ber_reset, 1, fast_write);

        // Poll for Timer Completion
        while (get_ptr_field(gcr_addr, rx_ber_timer_running) != 0 && (i_control[0] & BERM_CONTROL_STOP) == 0)
        {
            if (l_count > 2)
            {
                io_sleep(get_gcr_addr_thread(gcr_addr));
            }
        }

        if (i_control[0] & BERM_CONTROL_STOP)
        {
            break;
        }

        // Collect and Analyze the data
        for (l_lane = 0; l_lane < 32; ++l_lane)
        {
            if ((i_lane_mask_rx & ~l_done_mask & (0x80000000 >> l_lane)) == 0x0)
            {
                continue;
            }

            set_gcr_addr_lane(gcr_addr, l_lane);
            l_rx_berpl_count = get_ptr_field(gcr_addr, rx_berpl_count);

            // - If the counter is saturated, we do not have an accurate measurement, (saturated = 0xFFF, 4096)
            //   so we will not add the data to the log space
            if ((l_rx_berpl_count + (io_data[l_lane] & BERM_ERROR_MASK)) >= 0xFFF)
            {
                l_done_mask |= (0x80000000 >> l_lane);
            }
            else
            {
                // Add Errors and Bits Samples to log space
                io_data[l_lane] = (((l_count + 1) << BERM_TIMER_SHIFT) & BERM_TIMER_MASK) | ((io_data[l_lane] + l_rx_berpl_count) &
                                  BERM_ERROR_MASK);
            }

            // With > 100 errors, the data sample will be statistically accurate, no need to collect more data
            if ((io_data[l_lane] & BERM_ERROR_MASK) > 100)
            {
                l_accurate_mask |= (0x80000000 >> l_lane);
            }
        }

        if (l_ber_timer_sel > 0)
        {
            l_ber_timer_sel -= 2;
        }
    }

    for (l_lane = 0; l_lane < 32; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);

        if ((l_yskip_mask & (0x80000000 >> l_lane)) == 0x0)
        {
            int l_poff_adj = -TwosCompToInt(get_ptr_field(gcr_addr, rx_poff_adj), rx_poff_adj_width);
            apply_rx_dac_offset(gcr_addr, data_only, i_bank, l_poff_adj);
        }

        if ((l_xskip_mask & (0x80000000 >> l_lane)) == 0x0)
        {
            uint32_t l_pr_step_adjust = get_ptr(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr,
                                                rx_mini_pr_step_data_edge_adj_full_reg_startbit, rx_mini_pr_step_data_edge_adj_full_reg_endbit);
            int32_t l_data_offset = -TwosCompToInt(((l_pr_step_adjust & rx_mini_pr_step_data_adj_mask) >>
                                                    rx_mini_pr_step_data_adj_shift), rx_mini_pr_step_data_adj_width);
            int32_t l_edge_offset = -TwosCompToInt(((l_pr_step_adjust & rx_mini_pr_step_edge_adj_mask) >>
                                                    rx_mini_pr_step_edge_adj_shift), rx_mini_pr_step_edge_adj_width);

            pr_stepper_run(gcr_addr, i_bank, l_data_offset, l_edge_offset);
        }
    }

    set_gcr_addr_lane(gcr_addr, 0);
    return rc_no_error;
}

int ioo_berm_cleanup(t_gcr_addr* gcr_addr, const uint32_t i_lane_mask_rx)
{
    uint16_t l_lane = 0;

    // Step 3: Cleanup
    set_gcr_addr_lane(gcr_addr, 0);
    put_ptr_field(gcr_addr, rx_ber_en, 0, fast_write); // Per-Group

    for (l_lane = 0; l_lane < 32; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);

        // put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
        // put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, l_lane, read_modify_write);

        put_ptr_field(gcr_addr, rx_berpl_count_en, 0, read_modify_write); //pl

        put_ptr_field(gcr_addr, rx_pr_external_mode_a, 0, read_modify_write);
        put_ptr_field(gcr_addr, rx_pr_external_mode_b, 0, read_modify_write);

        // Power Down the Alternate Bank
        int disable_bank_powerdown = mem_pg_field_get(rx_disable_bank_pdwn);

        if (!disable_bank_powerdown)
        {
            put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
        }

        // put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel

        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);
    }

    set_gcr_addr_lane(gcr_addr, 0);

    return rc_no_error;
}



/**
 * @brief Runs a Bit Error rate Measurement on a given X/Y location
 * @param[inout] gcr_addr         Target Information
 * @param[in   ] i_lane_mask_rx   Rx Lane Mask
 * @retval void
 */
int ioo_ext_cmd_berm(t_gcr_addr* gcr_addr, const uint32_t i_lane_mask_rx)
{
    int32_t l_thread = get_gcr_addr_thread(gcr_addr);
    int32_t l_dac_offset = 0;
    int32_t l_pr_offset = 0;
    int32_t l_space = 0;
    t_bank l_bank = (g_logspace[0] & BERM_CONTROL_BANK) ? bank_a : bank_b;
    uint16_t* l_thread_space = &g_logspace[(l_thread * 72) + SPACE_A];
    uint16_t l_space_control = 0;

    ioo_berm_init(gcr_addr, i_lane_mask_rx, l_bank, &g_logspace[(l_thread * 72)]);
    g_logspace[(l_thread * 72) + SPACE_A + 1] = BERM_STATUS_DONE;
    g_logspace[(l_thread * 72) + SPACE_B + 1] = BERM_STATUS_DONE;



    while ((g_logspace[0] & BERM_CONTROL_KILL) == 0)
    {
        if (l_thread_space[0] & BERM_THREAD_CONTROL_START)
        {
            l_space_control = l_thread_space[0];
            l_pr_offset = TwosCompToInt((l_space_control >> 8) & 0x7F, 7);
            l_dac_offset = TwosCompToInt(l_space_control & 0xFF, 8);
            l_thread_space[0] = 0;
            ioo_berm_run(gcr_addr,
                         i_lane_mask_rx,
                         l_bank,
                         l_pr_offset,
                         l_dac_offset,
                         &g_logspace[0],
                         &l_thread_space[2]);
            l_thread_space[1] = l_space_control;
        }
        else
        {
            io_sleep(l_thread);
        }

        l_space = (l_space ^ 0x1 ) & 0x1;

        if(l_space)
        {
            l_thread_space = &g_logspace[(l_thread * 72) + SPACE_B];
        }
        else
        {
            l_thread_space = &g_logspace[(l_thread * 72) + SPACE_A];
        }
    }

    ioo_berm_cleanup(gcr_addr, i_lane_mask_rx);
    return rc_no_error;
}

// Layout (16bits per entry, 512 entries total in the debug space)
// [000]: Thread 0  Control        :: [0] Kill, [1] Stop-Op, [2] Bank, [14:15] Depth
// [001]: Thread 0  None           ::
// [002]: Thread 0  None           ::
// [003]: Thread 0  None           ::

// [004]: Thread 0 A Control X/Y Reg     :: [0]: Start, [1:7] X-Value, [8:15] Y-Value -- PPE Clears these when starting
// [005]: Thread 0 A Status X/Y Reg      :: [0]: Done, [1:7] X-Value, [8:15] Y-Value -- User Clears these when starting
// [006]: Thread 0 A Lane 0 Data         :: [0:3] Timer, [4:15] Errors
// ...................................................................
// [030]: Thread 0 A Lane 23 Data        :: [0:3] Timer, [4:15] Errors

// [032]: Thread 0 B Control X/Y Reg     :: [0]: Start, [1:7] X-Value, [8:15] Y-Value -- PPE Clears these when starting
// [033]: Thread 0 B Status X/Y Reg      :: [0]: Done, [1:7] X-Value, [8:15] Y-Value -- User Clears these when starting
// [034]: Thread 0 B Lane 0 Data         :: [0:3] Timer, [4:15] Errors
// ...................................................................
// [058]: Thread 0 B Lane 23 Data        :: [0:3] Timer, [4:15] Errors

// [072]: Thread 1 Control               :: [0]: Start, [1:7] X-Value, [8:15] Y-Value -- PPE Clears these when starting
