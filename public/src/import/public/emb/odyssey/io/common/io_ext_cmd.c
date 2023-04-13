/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_ext_cmd.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_ext_cmd.c
// *! TITLE       :
// *! DESCRIPTION : Common Functions for IO External Command (ext_cmd) Interface
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23031601 |vbr     | EWM 301316: Don't clear done/fail when mode bit is set (let FW do it)
// vbr22120300 |vbr     | Shifted some sleeps
// mbs22083000 |mbs     | PSL comment updates
// vbr22061500 |vbr     | Add external command fail reporting
// vbr22060200 |vbr     | Add lab commands, support more than 16 commands
// vbr22060100 |vbr     | Check ext_cmd_power_banks_sel in rx lane power_off/on
// vbr22031700 |vbr     | Improve efficiency of run_ext_cmd loop
// vbr22011201 |vbr     | Skip lanes that are bad (IOT)
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21101100 |vbr     | Added more sleeps to tx fifo init
// vbr21072600 |vbr     | Added clearing of mem_regs back into pl reset
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21030400 |vbr     | Split ext_cmd_lanes into rx and tx
// vbr21020800 |vbr     | Moved EO code
// vbr21020400 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"

#ifdef IOO
    #include "ioo_common.h"
#endif
#ifdef IOT
    #include "iot_common.h"
#endif

#include "io_ext_cmd.h"
#include "io_logger.h"

#include "io_init_and_reset.h"
#include "eo_wrappers.h"
#include "eo_main.h"

#include "io_lab_code.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
#define EXT_CMD_DBG_LVL 3


/////////////////////////////////////////////////////////////////////////////////
// Run External Commands from API Registers
// This function assumes there are only 16 (not 32) possible external commands
/////////////////////////////////////////////////////////////////////////////////
void run_external_commands(t_gcr_addr* io_gcr_addr)
{
    uint32_t l_cmd_req  = (fw_field_get(ext_cmd_req_00_15) << 16) |
                          (fw_field_get(ext_cmd_req_16_31) <<  0);
    uint32_t l_cmd_done = (fw_field_get(ext_cmd_done_00_15) << 16) |
                          (fw_field_get(ext_cmd_done_16_31) <<  0);

    // Clear the _done and _fail in response to _req=0
    if (l_cmd_req == 0x0)
    {
        // PSL ext_cmd_status_clear_mode
        if (fw_field_get(ext_cmd_status_clear_mode) == 0)
        {
            fw_field_put(ext_cmd_done_00_15, 0);
            fw_field_put(ext_cmd_done_16_31, 0);
            fw_field_put(ext_cmd_fail_00_15, 0);
            fw_field_put(ext_cmd_fail_16_31, 0);
        }
    }
    else if (l_cmd_req ^ l_cmd_done)
    {
        // Process new commands
        //ADD_LOG(DEBUG_EXT_CMD, io_gcr_addr, l_cmd_req);
        uint32_t l_cmd_fail     = (fw_field_get(ext_cmd_fail_00_15) << 16) |
                                  (fw_field_get(ext_cmd_fail_16_31) <<  0);
        uint32_t l_lane_mask_rx = (fw_field_get(ext_cmd_lanes_rx_00_15) << 16) |
                                  (fw_field_get(ext_cmd_lanes_rx_16_31) <<  0);
        uint32_t l_lane_mask_tx = (fw_field_get(ext_cmd_lanes_tx_00_15) << 16) |
                                  (fw_field_get(ext_cmd_lanes_tx_16_31) <<  0);

        // Run commands in priority order until all are completed
        uint32_t l_new_cmd_req = l_cmd_req & ~l_cmd_done;

        while (l_new_cmd_req)
        {
            // Determine the highest priority uncompleted command
            uint32_t l_cmd_num = __builtin_clz(l_new_cmd_req); //count leading zeros

            // Execute the external command function
            int status = io_ext_cmd_table[l_cmd_num](io_gcr_addr, l_lane_mask_rx, l_lane_mask_tx);
            uint32_t l_cmd_mask = 0x80000000 >> l_cmd_num;

            // Write the Fail Bit if the command returned status
            if (status)
            {
                l_cmd_fail |= l_cmd_mask;
                fw_field_put(ext_cmd_fail_00_15, ((l_cmd_fail & 0xFFFF0000) >> 16));
                fw_field_put(ext_cmd_fail_16_31, ((l_cmd_fail & 0x0000FFFF) >>  0));
            }

            // Write the Command Done Bit
            l_cmd_done |= l_cmd_mask;
            fw_field_put(ext_cmd_done_00_15, ((l_cmd_done & 0xFFFF0000) >> 16));
            fw_field_put(ext_cmd_done_16_31, ((l_cmd_done & 0x0000FFFF) >>  0));

            // Update command req vec
            l_new_cmd_req = l_new_cmd_req & ~l_cmd_done;
        } //while new_cmd_req
    } //proces commands

    return;
} //run_external_command


///////////////////////////
// Default / NOP command
// Sets a FIR if called
///////////////////////////
int cmd_nop(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0A);
    ADD_LOG(DEBUG_BAD_EXT_CMD, io_gcr_addr, 0x0);
    set_fir(fir_code_fatal_error);
    return rc_error;
} //cmd_nop


///////////////////////////
// Helper Functions
///////////////////////////
void track_and_adjust_group_power(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx,
                                  const int new_power_state)
{
    // Load Current Lane State
    uint32_t l_tx_lanes_power_on = (mem_pg_field_get(tx_lanes_pon_00_15) << 16) |
                                   (mem_pg_field_get(tx_lanes_pon_16_23) << (16 - tx_lanes_pon_16_23_width));
    uint32_t l_rx_lanes_power_on = (mem_pg_field_get(rx_lanes_pon_00_15) << 16) |
                                   (mem_pg_field_get(rx_lanes_pon_16_23) << (16 - rx_lanes_pon_16_23_width));

    // Adjustments based on new state
    bool state_changed = false;

    // PSL new_power_state
    if (new_power_state == 1)
    {
        ////////////////////////
        // Powering Off Lanes
        ////////////////////////

        // Group Power On If Needed
        // - Check if there are any lanes currently powered on
        // - Check if the current command is powering on any lanes
        if (l_tx_lanes_power_on == 0x0 && i_lane_mask_tx > 0x0)
        {
            io_group_power_on_tx(io_gcr_addr);
            state_changed = true;
        }

        if (l_rx_lanes_power_on == 0x0 && i_lane_mask_rx > 0x0)
        {
            io_group_power_on_rx(io_gcr_addr);
            state_changed = true;
        }

        // Update Lane State
        l_tx_lanes_power_on |= i_lane_mask_tx;
        l_rx_lanes_power_on |= i_lane_mask_rx;
    }
    else     //new_power_state == 0
    {
        ////////////////////////
        // Powering Off Lanes
        ////////////////////////

        // Get RX banks select configuration (ignored on IOT)
#ifdef IOO
        int banks_sel = fw_field_get(ext_cmd_power_banks_sel);
#else
        int banks_sel = both_banks;
#endif

        // Update Lane State
        l_tx_lanes_power_on &= ~i_lane_mask_tx;

        if (banks_sel == both_banks)
        {
            l_rx_lanes_power_on &= ~i_lane_mask_rx;
        }

        // Group Power Off If Needed
        // - Check if any lanes are still powered on
        // PSL group_power_off_tx
        if (l_tx_lanes_power_on == 0x0)
        {
            io_group_power_off_tx(io_gcr_addr);
            state_changed = true;
        }

        // PSL group_power_off_rx
        if (l_rx_lanes_power_on == 0x0)
        {
            io_group_power_off_rx(io_gcr_addr);
            state_changed = true;
        }
    }

    // Store New Lane State
    mem_pg_field_put(tx_lanes_pon_00_15, (l_tx_lanes_power_on >> 16) & 0xFFFF);
    mem_pg_field_put(tx_lanes_pon_16_23, (l_tx_lanes_power_on >>  8) & 0x00FF);
    mem_pg_field_put(rx_lanes_pon_00_15, (l_rx_lanes_power_on >> 16) & 0xFFFF);
    mem_pg_field_put(rx_lanes_pon_16_23, (l_rx_lanes_power_on >>  8) & 0x00FF);

    // PSL state_changed
    if(state_changed)
    {
        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }
} //track_and_adjust_group_power


///////////////////////////
// Common Handlers
///////////////////////////

// Calls HW REG INIT Per-Group
int cmd_hw_reg_init_pg(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD01, EXT_CMD_DBG_LVL);
    io_hw_reg_init(io_gcr_addr);  //sleeps at end
    return rc_no_error;
} //cmd_hw_reg_init_pg


// Calls I/O Reset Per-Lane
int cmd_ioreset_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD02, EXT_CMD_DBG_LVL);

    // Reset lanes
    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift_tx = i_lane_mask_tx;
    uint32_t i_lane_shift_rx = i_lane_mask_rx;
    uint32_t l_num_lanes = max( get_num_rx_lane_slices(), get_num_tx_lane_slices() );

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift_tx = i_lane_shift_tx << 1, i_lane_shift_rx = i_lane_shift_rx << 1)
    {
        if ( (i_lane_shift_tx | i_lane_shift_rx) & 0x80000000 )
        {
            set_gcr_addr_lane(io_gcr_addr, l_lane);

            // PSL io_reset_lane_tx
            if (i_lane_shift_tx & 0x80000000)
            {
                status |= io_reset_lane_tx(io_gcr_addr);    //sleeps at end
            }

            // PSL io_reset_lane_rx
            if (i_lane_shift_rx & 0x80000000)
            {
                status |= io_reset_lane_rx(io_gcr_addr);    //sleeps at end
            }
        }
    }  //for

    // Update lanes_pon_* and power off group (if needed) since ioreset turns off the power to lanes
    track_and_adjust_group_power(io_gcr_addr, i_lane_mask_rx, i_lane_mask_tx, 0); //0 = power_off

    return status;
} //cmd_ioreset_pl


// Calls Lane Power On Per-Lane
int cmd_power_on_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD03, EXT_CMD_DBG_LVL);

    // Get RX banks select configuration (ignored on IOT)
#ifdef IOO
    int banks_sel = fw_field_get(ext_cmd_power_banks_sel);
#else
    int banks_sel = both_banks;
#endif

    // Update lanes_pon_* and power on group (if needed)
    track_and_adjust_group_power(io_gcr_addr, i_lane_mask_rx, i_lane_mask_tx, 1); //1 = power_on

    // Power on specific lanes
    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift_tx = i_lane_mask_tx;
    uint32_t i_lane_shift_rx = i_lane_mask_rx;
    uint32_t l_num_lanes = max( get_num_rx_lane_slices(), get_num_tx_lane_slices() );

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift_tx = i_lane_shift_tx << 1, i_lane_shift_rx = i_lane_shift_rx << 1)
    {
        if ( (i_lane_shift_tx | i_lane_shift_rx) & 0x80000000 )
        {
            set_gcr_addr_lane(io_gcr_addr, l_lane);

            // PSL io_lane_power_on_tx
            if ((i_lane_shift_tx & 0x80000000) && !get_tx_lane_bad(l_lane))
            {
                status |= io_lane_power_on_tx(io_gcr_addr);  //sleeps at end
            }

            // PSL io_lane_power_on_rx
            if ((i_lane_shift_rx & 0x80000000) && !get_rx_lane_bad(l_lane))
            {
                status |= io_lane_power_on_rx(io_gcr_addr, banks_sel, true); // Power on and set dl_clk_en to 1 (HW508366)
                io_sleep(get_gcr_addr_thread(io_gcr_addr));
            }
        }
    } //for

    return status;
} //cmd_power_on_pl


// Calls Lane Power Off Per-Lane
int cmd_power_off_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD04, EXT_CMD_DBG_LVL);

    // Get RX banks select configuration (ignored on IOT)
#ifdef IOO
    int banks_sel = fw_field_get(ext_cmd_power_banks_sel);
#else
    int banks_sel = both_banks;
#endif

    // Power off specific lanes
    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift_tx = i_lane_mask_tx;
    uint32_t i_lane_shift_rx = i_lane_mask_rx;
    uint32_t l_num_lanes = max( get_num_rx_lane_slices(), get_num_tx_lane_slices() );

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift_tx = i_lane_shift_tx << 1, i_lane_shift_rx = i_lane_shift_rx << 1)
    {
        if ( (i_lane_shift_tx | i_lane_shift_rx) & 0x80000000 )
        {
            set_gcr_addr_lane(io_gcr_addr, l_lane);

            // PSL io_lane_power_off_tx
            if (i_lane_shift_tx & 0x80000000)
            {
                status |= io_lane_power_off_tx(io_gcr_addr);    //sleeps at end
            }

            // PSL io_lane_power_off_rx
            if (i_lane_shift_rx & 0x80000000)
            {
                status |= io_lane_power_off_rx(io_gcr_addr, banks_sel);    //sleeps at end
            }
        }
    } //for

    // Update lanes_pon_* and power off group (if needed)
    track_and_adjust_group_power(io_gcr_addr, i_lane_mask_rx, i_lane_mask_tx, 0); //0 = power_off

    return status;
} //cmd_power_off_pl


// Calls Tx Fifo Initialization Per-Lane
int cmd_tx_fifo_init_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD05, EXT_CMD_DBG_LVL);
    set_gcr_addr_reg_id(io_gcr_addr, tx_group);

    const int c_sleep_rate = 2;
    uint32_t l_count = 0;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_tx;
    uint32_t l_num_lanes = get_num_tx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        // PSL tx_lane_masked
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        // PSL tx_lane_bad
        if (get_tx_lane_bad(l_lane))
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        tx_fifo_init(io_gcr_addr);
        l_count++;

        if ((l_count % c_sleep_rate) == 0)
        {
            io_sleep(get_gcr_addr_thread(io_gcr_addr));
        }
    }

    if ((l_count % c_sleep_rate) != 0)
    {
        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }

    set_gcr_addr_reg_id(io_gcr_addr, rx_group);
    return rc_no_error;
} //cmd_tx_fifo_init_pl


// Calls Rx/Tx DC Calibration Per-Lane (and powers up the lane)
int cmd_dccal_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD06, EXT_CMD_DBG_LVL);

#ifdef IOO
    mem_pg_field_put(rx_dc_enable_loff_ab_alias, 0b11); // Both banks
#endif

    // Update lanes_pon_* and power on group (if needed)
    track_and_adjust_group_power(io_gcr_addr, i_lane_mask_rx, i_lane_mask_tx, 1); //1 = power_on

    // Run DC Cal on specific lanes
    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift_tx = i_lane_mask_tx;
    uint32_t i_lane_shift_rx = i_lane_mask_rx;
    uint32_t l_num_lanes = max( get_num_rx_lane_slices(), get_num_tx_lane_slices() );

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift_tx = i_lane_shift_tx << 1, i_lane_shift_rx = i_lane_shift_rx << 1)
    {
        // PSL lane_masked
        if ( ((i_lane_shift_tx | i_lane_shift_rx) & 0x80000000) == 0x0 )
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        mem_pl_bit_set(rx_lane_busy, l_lane);

        // PSL rx_lane_good
        if ((i_lane_shift_rx & 0x80000000) && !get_rx_lane_bad(l_lane))
        {
            status |= io_lane_power_on_rx(io_gcr_addr, both_banks, false); // Power on but leave dl_clk_en untouched (HW508366)
            io_sleep(get_gcr_addr_thread(io_gcr_addr));
            status |= eo_main_dccal_rx(io_gcr_addr);
        }

        // PSL tx_lane_good
        if ((i_lane_shift_tx & 0x80000000) && !get_tx_lane_bad(l_lane))
        {
            status |= io_lane_power_on_tx(io_gcr_addr);  //sleeps at end
            status |= eo_main_dccal_tx(io_gcr_addr);
        }

        mem_pl_bit_clr(rx_lane_busy, l_lane);
        mem_pl_bit_set(rx_dccal_done, l_lane);

        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }//for

    return status;
} //cmd_dccal_pl


// Calls Rx Train Per-Lane
int cmd_train_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD07, EXT_CMD_DBG_LVL);

#ifdef IOO
    mem_pg_field_put(rx_eo_phase_select_0_2, 0b111); // Run all phases
#endif

    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_rx;
    uint32_t l_num_lanes = get_num_rx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        // PSL rx_lane_masked
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        // PSL rx_lane_bad
        if (get_rx_lane_bad(l_lane))
        {
            continue;
        }

        status |= run_initial_training(io_gcr_addr, l_lane); //sleeps at end
        mem_pl_bit_set(rx_cmd_init_done, l_lane);
    } //for

    return status;
} //cmd_train_pl


// Calls Recalibration Per-Lane
int cmd_recal_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD08, EXT_CMD_DBG_LVL);

    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_rx;
    uint32_t l_num_lanes = get_num_rx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        // PSL rx_lane_masked
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        // PSL rx_lane_bad
        if (get_rx_lane_bad(l_lane))
        {
            continue;
        }

        status |= run_recalibration(io_gcr_addr, l_lane); //sleeps at end
    }

    return status;
} //cmd_recal_pl


// Calls Lab Code (Per-Group Code)
int cmd_lab_code_pg(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD11, EXT_CMD_DBG_LVL);

    int status = io_lab_code_pg(io_gcr_addr);

    return status;
} //cmd_lab_code_pg


// Calls Lab Code (Per-Lane Code)
int cmd_lab_code_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD10, EXT_CMD_DBG_LVL);

    int status = rc_no_error;
    uint32_t l_lane = 0;
    uint32_t i_lane_shift_tx = i_lane_mask_tx;
    uint32_t i_lane_shift_rx = i_lane_mask_rx;
    uint32_t l_num_lanes = max( get_num_rx_lane_slices(), get_num_tx_lane_slices() );

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift_tx = i_lane_shift_tx << 1, i_lane_shift_rx = i_lane_shift_rx << 1)
    {
        if ( ((i_lane_shift_tx | i_lane_shift_rx) & 0x80000000) == 0x0 )
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);

        if (i_lane_shift_rx & 0x80000000)
        {
            status |= io_lab_code_pl_rx(io_gcr_addr);
        }

        if (i_lane_shift_tx & 0x80000000)
        {
            status |= io_lab_code_pl_tx(io_gcr_addr);
        }

        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }//for

    return status;
} //cmd_lab_code_pl
