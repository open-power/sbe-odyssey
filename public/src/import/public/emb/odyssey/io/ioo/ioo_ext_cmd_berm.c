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
// *! *** IBM Confidential ***
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

// Format
// [00:04] 05b : Lane
// [05   ] 01b : X Out Of Range
// [06   ] 01b : Y Out of Range
// [07:11] 05b : X Offset
// [12:16] 05b : Timer Loops
// [17:31] 15b : Errors
#define BERM_LANE_MASK 0xF8000000
#define BERM_X_OUT_OF_RANGE_MASK 0x04000000
#define BERM_Y_OUT_OF_RANGE_MASK 0x02000000
#define BERM_X_OFFSET_MASK 0x01F00000
#define BERM_TIMER_MASK 0x000F8000
#define BERM_ERROR_MASK 0x00007FFF

#define BERM_LANE_SHIFT 27
#define BERM_X_OUT_OF_RANGE_SHIFT 26
#define BERM_Y_OUT_OF_RANGE_SHIFT 25
#define BERM_X_OFFSET_SHIFT 20
#define BERM_TIMER_SHIFT 15
#define BERM_ERRORS_SHIFT 0

extern uint32_t _debug_log_start __attribute__((section("debuglog")));
volatile uint32_t* g_logspace = &_debug_log_start;

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
int ioo_ext_cmd_berm(t_gcr_addr* gcr_addr, const uint32_t i_lane_mask_rx)
{

    const int32_t c_num_lanes = get_num_rx_physical_lanes();
    int32_t l_thread = get_gcr_addr_thread(gcr_addr);
    int32_t l_ber_timer_sel = 15;
    int32_t l_rx_berpl_count = 0;
    int32_t l_lane = 0;
    int32_t l_count = 0;
    uint32_t l_done_mask = 0x00;
    uint32_t l_xskip_mask = 0x00;
    uint32_t l_yskip_mask = 0x00;
    t_init_cal_mode l_cal_mode = C_AXO_CAL;


    t_bank l_bank = (mem_pg_field_get(rx_berm_bank) == 0) ? bank_a : bank_b;
    int32_t l_timer_depth = 0;
    int32_t l_dac_offset = TwosCompToInt(mem_pg_field_get(rx_berm_voffset), rx_berm_voffset_width);
    int32_t l_pr_offset = TwosCompToInt(mem_pg_field_get(rx_berm_hoffset), rx_berm_hoffset_width);
    int32_t l_pattern_sel = mem_pg_field_get(rx_berm_pattern_sel);

    int32_t l_depth = mem_pg_field_get(rx_berm_depth);

    if (l_depth == 0)
    {
        l_timer_depth = 20;
    }
    else if (l_depth == 1)
    {
        l_timer_depth = 15;
    }
    else
    {
        l_timer_depth = 10;
    }

    // Step 0: Setup the Debug Log Space
    // Set the entries to max so our data does not get overwritten
    img_field_put(ppe_debug_log_num, 127);

    // Clear the Debug Log (Note the ptr is defined as 32b sizes which is why we are indexing to 255)
    for (; l_count < 32; ++l_count)
    {
        g_logspace[(l_thread * 32) + l_count] = 0;
    }

    // Step 1: Power On, Bank Sync, Enable BER, Apply Offsets
    set_gcr_addr_lane(gcr_addr, 0);
    put_ptr_field(gcr_addr, rx_ber_en, 1, fast_write); // Per-Group

    mem_pg_field_put(rx_recal_abort_0_15, (i_lane_mask_rx >> 16));
    mem_pg_field_put(rx_recal_abort_16_23, (i_lane_mask_rx >> (16 - rx_recal_abort_16_23_width)));

    clear_all_cal_lane_sel(
        gcr_addr); // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.

    for (l_lane = 0; l_lane < c_num_lanes; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);
        g_logspace[(l_thread * 32) + l_lane] |= (l_lane << BERM_LANE_SHIFT) & BERM_LANE_MASK;

        put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
        put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, l_lane, read_modify_write);

        // Power On the Alt Bank
        alt_bank_psave_clear_and_wait(gcr_addr);

        t_bank l_current_main_bank  = (get_ptr_field(gcr_addr, rx_bank_sel_a) == 0) ? bank_b : bank_a;

        // If we want to test the current main bank, we have to sync the alt bank before
        //   we switch over, or else we will cause errors downstream on the dl
        if (l_bank == l_current_main_bank)
        {
            if (l_bank == bank_a)
            {
                eo_bank_sync(gcr_addr, bank_b, l_cal_mode);
            }
            else
            {
                eo_bank_sync(gcr_addr, bank_a, l_cal_mode);
            }
        }

        // Set the correct bank under test, so we do not corrupt main data
        set_cal_bank(gcr_addr, l_bank);


        if (l_bank == bank_a)
        {
            //put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_align_cdr_b_lcl, fast_write);
            put_ptr_field(gcr_addr, rx_pr_external_mode_a, 1, read_modify_write);
        }
        else
        {
            // bank_b
            //put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl_align, fast_write);
            put_ptr_field(gcr_addr, rx_pr_external_mode_b, 1, read_modify_write);
        }

        // Do I need to do another bank sync after we set external mode?
        eo_bank_sync(gcr_addr, l_bank, l_cal_mode);


        // Enable BER
        // put_ptr_field(gcr_addr, rx_berpl_count_en, 1, read_modify_write);
        // put_ptr_field(gcr_addr, rx_berpl_exp_data_sel, 0, read_modify_write);
        // put_ptr_field(gcr_addr, rx_berpl_mask_mode, 0, read_modify_write);
        // put_ptr_field(gcr_addr, rx_berpl_lane_invert, 0, read_modify_write);
        put_ptr_field(gcr_addr, rx_berpl_count_en, 1, fast_write); // does the above scoms in a single write
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

        // Apply DAC Offset
        apply_rx_dac_offset(gcr_addr, data_only, l_bank, l_dac_offset);

        if (get_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky))
        {
            g_logspace[(l_thread * 32) + l_lane] |= BERM_Y_OUT_OF_RANGE_MASK;
            l_yskip_mask |= (0x80000000 >> l_lane);
        }

        // Apply PR Offset
        // - If the offset is out of range, we will apply zero offset and mark the lane
        g_logspace[(l_thread * 32) + l_lane] |= (l_pr_offset << BERM_X_OFFSET_SHIFT) & BERM_X_OFFSET_MASK;
        int32_t rc = apply_pr_offset(gcr_addr, l_bank, l_pr_offset);

        if (rc || get_ptr_field(gcr_addr, rx_mini_pr_step_rollover_sticky))
        {
            g_logspace[(l_thread * 32) + l_lane] |= BERM_X_OUT_OF_RANGE_MASK;
            l_xskip_mask |= (0x80000000 >> l_lane);
        }

        put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
    }

    // Step 2: Collect the BER Data
    // - The amount of dwell time is configurable based on the depth
    l_done_mask |= l_xskip_mask | l_yskip_mask;
    l_ber_timer_sel = 14;

    for (l_count = 0; l_count <= l_timer_depth; ++l_count)
    {
        // Skip collectin BER measurements if none of the lanes are valid
        if (l_done_mask == i_lane_mask_rx)
        {
            break;
        }

        set_gcr_addr_lane(gcr_addr, 0);
        put_ptr_field(gcr_addr, rx_ber_timer_sel, l_ber_timer_sel, read_modify_write);
        put_ptr_field(gcr_addr, rx_ber_reset, 1, fast_write);

        // Poll for Timer Completion
        while (get_ptr_field(gcr_addr, rx_ber_timer_running) != 0)
        {
            io_sleep(l_thread);
        }

        // Collect and Analyze the data
        for (l_lane = 0; l_lane < c_num_lanes; ++l_lane)
        {
            if ((i_lane_mask_rx & ~l_done_mask & (0x80000000 >> l_lane)) == 0x0)
            {
                continue;
            }

            set_gcr_addr_lane(gcr_addr, l_lane);
            l_rx_berpl_count = get_ptr_field(gcr_addr, rx_berpl_count);

            // If the counter is saturated, we do not have an accurate measurement,
            //   so we will not add the data to the log space
            if (l_rx_berpl_count == 0xFFF)
            {
                l_done_mask |= (0x80000000 >> l_lane);
                continue;
            }

            // Add Errors and Bits Samples to log space
            g_logspace[(l_thread * 32) + l_lane] += l_rx_berpl_count;
            g_logspace[(l_thread * 32) + l_lane] = (g_logspace[(l_thread * 32) + l_lane] & ~BERM_TIMER_MASK) | ((
                    l_count << BERM_TIMER_SHIFT) & BERM_TIMER_MASK);

            // With > 100 errors, the data sample will be statistically accurate, no need to collect more data
            if ((g_logspace[(l_thread * 32) + l_lane] & BERM_ERROR_MASK) > 100)
            {
                l_done_mask |= (0x80000000 >> l_lane);
            }
        }

        if (l_ber_timer_sel > 0)
        {
            --l_ber_timer_sel;
        }
    }

    // Step 3: Cleanup
    // l_pr_data_offset_vec[0] = 0;
    // l_pr_data_offset_vec[1] = 0;
    for (l_lane = 0; l_lane < c_num_lanes; ++l_lane)
    {
        if ((i_lane_mask_rx & (0x80000000 >> l_lane)) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, l_lane);

        put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
        put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, l_lane, read_modify_write);

        put_ptr_field(gcr_addr, rx_berpl_count_en, 0, read_modify_write); //pl

        // Remove Dac Offset
        if ((l_yskip_mask & (0x80000000 >> l_lane)) == 0x0)
        {
            int l_poff_adj = -TwosCompToInt(get_ptr_field(gcr_addr, rx_poff_adj), rx_poff_adj_width);
            apply_rx_dac_offset(gcr_addr, data_only, l_bank, l_poff_adj);
        }

        // Remove PR Offset on lanes that we have successfully applied the x offset to
        // - If x was out of range, then we never applied the offset in the first place.
        if ((l_xskip_mask & (0x80000000 >> l_lane)) == 0x0)
        {
            uint32_t l_pr_step_adjust = get_ptr(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr,
                                                rx_mini_pr_step_data_edge_adj_full_reg_startbit, rx_mini_pr_step_data_edge_adj_full_reg_endbit);
            int32_t l_data_offset = -TwosCompToInt(((l_pr_step_adjust & rx_mini_pr_step_data_adj_mask) >>
                                                    rx_mini_pr_step_data_adj_shift), rx_mini_pr_step_data_adj_width);
            int32_t l_edge_offset = -TwosCompToInt(((l_pr_step_adjust & rx_mini_pr_step_edge_adj_mask) >>
                                                    rx_mini_pr_step_edge_adj_shift), rx_mini_pr_step_edge_adj_width);

            pr_stepper_run(gcr_addr, l_bank, l_data_offset, l_edge_offset);
        }

        put_ptr_field(gcr_addr, rx_pr_external_mode_a, 0, read_modify_write);
        put_ptr_field(gcr_addr, rx_pr_external_mode_b, 0, read_modify_write);

        // Power Down the Alternate Bank
        int disable_bank_powerdown = mem_pg_field_get(rx_disable_bank_pdwn);

        if (!disable_bank_powerdown)
        {
            put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
        }

        put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
    }

    set_gcr_addr_lane(gcr_addr, 0);

    return rc_no_error;
}
