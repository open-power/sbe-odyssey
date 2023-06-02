/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_esd.c $        */
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
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_esd.c
// *! TITLE       :
// *! DESCRIPTION : Run ESD Path Offset check
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// mwh23041400 |mwh     | Add set_rxbist_fail_lane issue 292951
// mwh22111100 |mwh     | Add in fail log, fail bit, and reg code for compare
// vbr22111000 |vbr     | General cleanup and code size reductions
// jac22061700 |jac     | Initial Rev A1yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "io_init_and_reset.h"
#include "ioo_common.h"
#include "eo_esd.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"

// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
#define EO_ESD_DBG_LVL 3


// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.
#define num_servo_ops 4
static uint16_t servo_ops_esd_test_a[num_servo_ops] = { c_loff_ae_n000, c_loff_ae_e000, c_loff_ae_s000, c_loff_ae_w000 };
static uint16_t servo_ops_esd_test_b[num_servo_ops] = { c_loff_be_n000, c_loff_be_e000, c_loff_be_s000, c_loff_be_w000 };


// Assumption checking
PK_STATIC_ASSERT(rx_a_lte_gain_zero_alias_width == 6);
PK_STATIC_ASSERT(rx_b_lte_gain_zero_alias_width == 6);
PK_STATIC_ASSERT(rx_a_lte_gain_startbit < rx_a_lte_zero_startbit);
PK_STATIC_ASSERT(rx_b_lte_gain_startbit < rx_b_lte_zero_startbit);
PK_STATIC_ASSERT(rx_a_lte_gain_zero_alias_width == (rx_a_lte_zero_endbit - rx_a_lte_gain_startbit + 1));
PK_STATIC_ASSERT(rx_b_lte_gain_zero_alias_width == (rx_b_lte_zero_endbit - rx_b_lte_gain_startbit + 1));


///////////////
// ESD Test
///////////////
int eo_esd_test(t_gcr_addr* gcr_addr, t_bank test_bank)
{
    set_debug_state(0xA100); // DEBUG ESD Test Start

    /////////////////////////////
    // Configuration
    /////////////////////////////

    // Disable the DL clock
    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0, read_modify_write);

    // Make sure the ALT bank is powered up (main bank will be powered because this is only run on powered lanes)
    // Psave returns after powering the bank (do not wait for CDR lock)
    put_ptr_field(gcr_addr, rx_psave_cdrlock_mode_sel, 0b11, read_modify_write); //pl
    alt_bank_psave_clear_and_wait(gcr_addr);

    // Set the Cal (Alt) bank
    set_cal_bank(gcr_addr, test_bank);

    // Select the cal lane
    clear_all_cal_lane_sel(
        gcr_addr); // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.
    put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
    int lane = get_gcr_addr_lane(gcr_addr);
    put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, lane, read_modify_write);

    // Set Circuit Controls
    // Test is destructive: these settings are not restored
    put_ptr_field(gcr_addr, rx_bist_en_dc,            0b0,      read_modify_write);
    put_ptr_field(gcr_addr, rx_a_lte_gain_zero_alias, 0b111000, read_modify_write);
    put_ptr_field(gcr_addr, rx_b_lte_gain_zero_alias, 0b111000, read_modify_write);
    put_ptr_field(gcr_addr, rx_a_ctle_gain,           0b1111,   read_modify_write);
    put_ptr_field(gcr_addr, rx_b_ctle_gain,           0b1111,   read_modify_write);

    // Configure the servo op for regular mode (not live edge offset), but do not enable the fence
    put_ptr_field(gcr_addr, rx_loff_livedge_mode, 0b0, read_modify_write);

    // Select the servo ops and dac latches
    uint16_t* servo_ops;
    t_bank main_bank;

    if (test_bank == bank_a)
    {
        servo_ops = servo_ops_esd_test_a;
        main_bank = bank_b;
    }
    else     //bank_b
    {
        servo_ops = servo_ops_esd_test_b;
        main_bank = bank_a;
    }


    /////////////////////////////
    // Test loop
    /////////////////////////////
    // Loop 0: Run servo ops on ALT bank with both banks powered (Dual Bank)
    // Loop 1: Run servo ops on ALT bank with only ALT powered (Single Bank)
    int fail = 0;
    int result_sum_loop[2];
    int fail_loop[2];
    int single;

    for (single = 0; single <= 1; single++)
    {
        // Leave main bank powered in loop 0, and power it down in loop 1
        if (single)
        {
            set_debug_state(0xA101); // DEBUG ESD Run Servo Loop1
            sequence_rx_bank_controls(gcr_addr, main_bank, power_off);
        }

        // Run the servo ops and get the results
        int32_t servo_results[num_servo_ops];
        run_servo_ops_and_get_results(gcr_addr, c_servo_queue_general, num_servo_ops, servo_ops, servo_results);

        // Calculate the sum of the offset results
        int result_sum = servo_results[0] + servo_results[1] + servo_results[2] + servo_results[3];
        result_sum_loop[single] = result_sum;

        // Check for a loop failure (Absolute Limit of the offset results)
        uint32_t avgPerBankAbsoluteLimit = mem_pg_field_get(rx_esd_abs_val_check);
        uint32_t AbsoluteLimit = 4 * avgPerBankAbsoluteLimit;

        if ( abs(result_sum) > AbsoluteLimit )
        {
            fail = 1;
            fail_loop[single] = 1;
        }
        else
        {
            fail_loop[single] = 0;
        }
    } //for 0..1


    /////////////////////////////
    // Cleanup
    /////////////////////////////

    // Clear cal lane sel
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);

    // Power on the main bank
    sequence_rx_bank_controls(gcr_addr, main_bank, power_on);

    // Power off the alt bank
    put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);


    /////////////////////////////
    // Error Checking/Reporting
    /////////////////////////////

    if (fail_loop[0])   // Loop 0: Dual Bank
    {
        set_debug_state(0xA108); // DEBUG ESD FailDual
    }

    if (fail_loop[1])   // Loop 1: Single Bank
    {
        set_debug_state(0xA118); // DEBUG ESD FailSignle
    }

    // Check for a difference between results when the main bank is powered vs not powered
    uint32_t avgPerBankShiftLimit = mem_pg_field_get(rx_esd_shift_val_check);
    uint32_t ShiftLimit = 4 * avgPerBankShiftLimit;

    if ( abs(result_sum_loop[0] - result_sum_loop[1]) > ShiftLimit )
    {
        set_debug_state(0xA107); // DEBUG ESD FailShift
        fail = 1;
    }

    if (fail)
    {
        // Set the ESD Fail
        mem_pl_bit_set(rx_esd_fail, lane);

        //set Fir Error and lane
        set_rxbist_fail_lane(gcr_addr);//add for issue 292951
        // Set the FIRs and PPE Error Status
        //set_fir(fir_code_dft_error | fir_code_bad_lane_warning);

        // Log the dual offset and single offset
        ADD_LOG(DEBUG_BIST_ESD_FAIL, gcr_addr, result_sum_loop[0]); // Dual Bank Offset
        ADD_LOG(DEBUG_BIST_ESD_FAIL, gcr_addr, result_sum_loop[1]); // Single Bank Offset
    }

    set_debug_state(0xA1FF); // DEBUG ESD Test End
    return (fail ? error_code : pass_code);
} //eo_esd_test
