/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_ctle.c $       */
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
// *! FILENAME    : eo_ctle.c
// *! TITLE       :
// *! DESCRIPTION : Eyeopt flow to calibrate peaking
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : John Gullickson     Email: gullicks@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23031400 |vbr     | EWM 299933: Use filter_depth=9 and bo_time=31 for peak2
// vbr23031500 |vbr     | Added new peak1/2 cal force enables to allow for lab experimenting.
// vbr23030800 |vbr     | No longer run peak2 in Gen5 or AXO (>=32Gbps); ignore peak1/2 cal disables.
// jjb22122000 |jjb     | qualified setting rx_a/b_ctle_peak1/2_done with !peak1/2_disable
// jjb22102600 |jjb     | Issue 292931: added io_sleep to abort check in 0x6080
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22032300 |vbr     | Issue 273572: Allow disabling of starting servo ops from 0 to aid in debugging
// vbr22022500 |vbr     | Revamped peak hysteresis to used stored bit.
// vbr21100600 |vbr     | Added averaging of peak results in recal.
// vbr21092200 |vbr     | Use shared functions for setting servo status error enable.
// vbr21091300 |vbr     | Back to a double pass in recal when both peaks are enabled.
// vbr21090800 |vbr     | Clean up / updates for P11 (remove fine mode).
// vbr21081200 |vbr     | Correctly set number of servo ops in rough mode. Set h_sel to H2 for peak2 and Gen3/4.
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// vbr21040700 |vbr     | Added cal_mode input and peak1/2_en which results
// jfg21040500 |jfg     | EWM228237 Restrict peak clearing to initial training only
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr20050400 |vbr     | Don't get servo results in rough mode unless copying from A to B
// vbr20050100 |vbr     | Added copying A to B in rough-only mode (as well as setting ctle_a done), combined register accesses
// vbr20043000 |vbr     | Removed averaging of peak2 in rough mode; re-arranged servo queue status check and switched to common function
//-------------|P11 ----|-------------------------------------------------------
// mwh20012100 |mwh     | Add in code to set rx_ctle_peak1/2_fail if servo op has issue
// vbr20101200 |vbr     | HW544452/HW539450: Don't clear peak1/2 if it is disabled.
// mwh20100900 |mwh     | HW549165:Only the eo_vcql should be setting fail also need be 0 since need set outside the loop
// vbr20090900 |vbr     | HW544452: Added config for disabling peak1 or peak2 cal. Also removed averaging of peak2 results in rough mode.
// vbr20080600 |vbr     | HW539450: Start first_run/rough servos from 0 since ZFE breaks down when peak is extremely high.
// jfg20021901 |jfg     | HW521060: Abort scenario on first-run rough pass exposed failure to restore peak2 due to mis-use of pk
// jfg20032401 |jfg     | HW526927  Wrong conditional || vs &&
// jfg20032400 |jfg     | HW526927  hysteresis_en is not sensitive to first_run as intended.
// jfg20030900 |jfg     | HW525009 add a rough_only mode to set initial coarse peak 1&2
// cws20011400 |cws     | Added Debug Logs
// jfg19092300 |jfg     | PR661 fix bug in error conditional for servo error
// jfg19091100 |jfg     | HW503164 refactoring flow to include a coarse catch-up and then separate peak servos for an initial pass
// jfg19082200 |jfg     | regression coverege identified ctle done write was missing for bankA
// jfg19071900 |jfg     | HW492082 Add peak2 servos
// jfg19052002 |jfg     | REAPPLY: Fix ctle_mode conditional which tried to cast an int into bool and was not working (nor flagged as error)
// jfg19050101 |jfg     | change peak_done to peak1_done by Mike Harper's request
// vbr19041500 |vbr     | Updated register names
// mwh19041719 |mwh     | change peak to peak1 -- bist stuff
// jfg19022700 |jfg     | Add result hysteresis and restore, remove static config
// vbr19021800 |vbr     | Added peak changed output for use in VGA-CTLE loop.
// vbr19012200 |vbr     | Updated function calls
// vbr19011701 |vbr     | Re-arrange servo error and abort handling to restore on both.
// vbr19011700 |vbr     | HW451171: Queue up all 4 servo ops at once.
// vbr19011600 |vbr     | HW473413: Do not use CDR quad mode tracking.
// mwh19011100 |mwh     | Add () because the || could could cause compare to wrong thing
// vbr18111400 |vbr     | Updated calls to set_fir.
// vbr18101100 |vbr     | Updated to avoid possibility of alignment exceptions in result sort (HW468390)
// vbr18101000 |vbr     | Disable servo_status for result min/max
// jfg18081301 |jfg     | Fix CQD math expression for pow typo
// jfg16060600 |jfg     | Replace ctle codes on servo_code_b array with b versions
// jfg16051000 |jfg     | Fix debug_state values
// jfg16042300 |jfg     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_ctle.h"
#include "io_logger.h"

#include "ppe_com_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "io_config.h"


// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value of IO_DEBUG_LEVEL is 2
#define EO_CTLE_DBG_LVL 3


// Asumption checking
PK_STATIC_ASSERT(rx_a_ctle_gain_peak_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_b_ctle_gain_peak_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_a_ctle_gain_startbit  == rx_b_ctle_gain_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak1_startbit == rx_b_ctle_peak1_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak2_startbit == rx_b_ctle_peak2_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak1_hyst_width == 4);
PK_STATIC_ASSERT(rx_a_ctle_peak2_hyst_width == 4);
PK_STATIC_ASSERT(rx_b_ctle_peak1_hyst_width == 4);
PK_STATIC_ASSERT(rx_b_ctle_peak2_hyst_width == 4);
PK_STATIC_ASSERT(rx_a_ctle_peak1_peak2_hyst_alias_width == 8);
PK_STATIC_ASSERT(rx_b_ctle_peak1_peak2_hyst_alias_width == 8);
PK_STATIC_ASSERT(rx_a_ctle_peak2_hyst_startbit == rx_a_ctle_peak1_hyst_startbit + 4);
PK_STATIC_ASSERT(rx_b_ctle_peak2_hyst_startbit == rx_b_ctle_peak1_hyst_startbit + 4);
PK_STATIC_ASSERT(rx_ctle_peak_hyst_settings_full_reg_width == 16);
PK_STATIC_ASSERT(rx_ctle_force_peak1_peak2_cal_enable_width == 3);


////////////////////////////////////////////////////////////////////////////
// Hysteresis Helper Function
////////////////////////////////////////////////////////////////////////////
void ctle_hysteresis(int i_hyst_delta_mode, int i_hyst_limit, int i_old_val, int* io_new_val, int* io_hyst_cnt)
{
    int l_hyst_cnt  = *io_hyst_cnt;
    int l_new_val   = *io_new_val;

    // Calculate the new hysteresis counter
    int l_delta = l_new_val - i_old_val;

    // PSL delta_eq_0
    if (l_delta == 0)
    {
        // Count towards zero if no change in value
        // PSL delta_eq_0_hyst_cnt_lt_0
        if (l_hyst_cnt < 0)
        {
            l_hyst_cnt = l_hyst_cnt + 1;
            // PSL delta_eq_0_hyst_cnt_gt_0
        }
        else if(l_hyst_cnt > 0)
        {
            l_hyst_cnt = l_hyst_cnt - 1;
        }

        // PSL hyst_delta_mode
    }
    else if (i_hyst_delta_mode || (abs(l_delta) > 1))
    {
        // Count by delta
        //   hyst_delta_mode=1: allow any delta
        //   hyst_delta_mode=0: allow delta >= 2 only (not +/-1)
        l_hyst_cnt = l_hyst_cnt + l_delta;
    }

    // Adjust the value and reset the hysteresis counter if the counter rolled over
    // PSL hyst_cnt_gt_hyst_limit
    if (l_hyst_cnt > i_hyst_limit)
    {
        l_hyst_cnt = 0;
        l_new_val  = i_old_val + 1;
        // PSL hyst_cnt_lt_neg_hyst_limit
    }
    else if (l_hyst_cnt < -i_hyst_limit)
    {
        l_hyst_cnt = 0;
        l_new_val  = i_old_val - 1;
    }
    else
    {
        l_new_val  = i_old_val;
    }

    // Return results
    *io_hyst_cnt = l_hyst_cnt;
    *io_new_val  = l_new_val;
} //ctle_hysteresis


////////////////////////////////////////////////////////////////////////////
// IOO CTLE ZFE METHOD
////////////////////////////////////////////////////////////////////////////
static uint16_t servo_ops_ctle_a[2] = { c_ctle_peak1_ae_n000, c_ctle_peak2_ae_n000 };
static uint16_t servo_ops_ctle_b[2] = { c_ctle_peak1_be_n000, c_ctle_peak2_be_n000 };

int eo_ctle(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode, t_bank cal_bank, bool copy_to_main, bool start_at_zero,
            bool recal, bool hysteresis_en, bool* peak_changed)
{
    set_debug_state(0x6000); // DEBUG - CTLE Start (IOO)
    int lane = get_gcr_addr_lane(gcr_addr);

    // Peak1 correlation bit defaults to 0 in the RegDef.
    // Peak2 correlation bit is set to 0 in hw_reg_init.
    //put_ptr_field(gcr_addr, rx_ctle_peak2_h_sel, 1, read_modify_write); // Gen5: H3
    //put_ptr_field(gcr_addr, rx_ctle_peak2_h_sel, 0, read_modify_write); // Gen3, Gen4: H2

    // Determine configuration on which sub-steps are enabled and return if both are disabled for some reason.
    // Default Config:
    //  AXO:  Peak1 only at >=32Gbps, Peak2 only at <32Gbps.
    //  PCIe: Peak1 only at Gen5, Peak2 only at Gen4.
    // ppe_data_rate: 000: 2.5Gbps; 001: 5.0Gbps; 010: 8.0Gbps; 011: 16.0Gbps; 100: 21.3Gbps; 101: 25.6Gbps; 110: 32.0Gbps; 111: 38.4Gbps"
    bool peak1_en = ((cal_mode == C_AXO_CAL) && (mem_pg_field_get(ppe_data_rate) >= 6)) || (cal_mode == C_PCIE_GEN5_CAL);
    bool peak2_en = !peak1_en;
    int force_peak1_peak2_enable = mem_pg_field_get(rx_ctle_force_peak1_peak2_cal_enable);

    if (force_peak1_peak2_enable == 0b100)
    {
        return rc_no_error;    // Both Forced Disabled
    }

    if (force_peak1_peak2_enable & 0b100)
    {
        peak1_en = (force_peak1_peak2_enable & 0b110);
        peak2_en = (force_peak1_peak2_enable & 0b101);
    }

    bool peak1_disable = !peak1_en;
    bool peak2_disable = !peak2_en;

    // Start servos at peak2 (index=1) when peak1 is disabled
    int servo_array_start = peak1_disable ? 1 : 0;

    // Only need to run 1 of the servo ops if peak1 or peak2 is disabled.
    // Otherwise run 2 servo ops (peak1/2 single pass).
    int num_servo_ops = (peak1_disable || peak2_disable) ? 1 : 2;

    // EWM 299933: Use filter_depth=9 and bo_time=31 for peak2 (run in PCIe Gen4, AXO <32Gbps)
    // Do not update the settings when using the lab debug enable overrides.
    bool update_servo_settings = peak2_en && ((force_peak1_peak2_enable & 0b100) == 0);

    if (update_servo_settings)
    {
        put_ptr_field(gcr_addr, rx_ctle_filter_depth,  9, read_modify_write);
        put_ptr_field(gcr_addr, rx_bo_time,           31, fast_write); //rx_servo_timer_half_rate=0 only other field in register
    }

    // Select the correct servo ops
    uint16_t* servo_ops;

    // PSL bank_a
    if (cal_bank == bank_a)
    {
        servo_ops = servo_ops_ctle_a;
    }
    else     //bank_b
    {
        servo_ops = servo_ops_ctle_b;
    }

    // Issue 273572: Allow disabling of starting servo ops from 0 to aid in debugging
    bool start_servos_at_zero = start_at_zero && (mem_pg_field_get(rx_ctle_start_at_zero_disable) == 0);

    // Read the initial register value for the cases where it is needed
    int initial_gain_peak_regval = 0;

    // PSL start0_or_recal_or_hyst
    if (start_servos_at_zero || recal || hysteresis_en)
    {
        // PSL start0_or_recal_or_hyst_bank_a
        if (cal_bank == bank_a)
        {
            initial_gain_peak_regval = get_ptr_field(gcr_addr, rx_a_ctle_gain_peak_full_reg_alias);
        }
        else     //bank_b
        {
            initial_gain_peak_regval = get_ptr_field(gcr_addr, rx_b_ctle_gain_peak_full_reg_alias);
        }
    }

    // HW539450: When peaking is excessively high, ZFE may not vote properly to bring peaking down.
    // Main concerns are TXEQ on PCIe (and any equivalent for AXO) and PCIe speed changes.
    // Don't want to try running ZFE from high settings left over from a previous TXEQ or speed.
    // Initial training is re-run in both of those cases so only need to start from 0 on first pass of peaking in INIT.
    // PSL start0
    if (start_servos_at_zero)
    {
        // Only clear for the enabled controls since the others may be preset
        int gain_peak_regval = initial_gain_peak_regval;

        if (!peak1_disable)
        {
            gain_peak_regval = gain_peak_regval & ~rx_a_ctle_peak1_mask;
        }

        if (!peak2_disable)
        {
            gain_peak_regval = gain_peak_regval & ~rx_a_ctle_peak2_mask;
        }

        // PSL start0_bank_a
        if (cal_bank == bank_a)
        {
            put_ptr_field(gcr_addr, rx_a_ctle_gain_peak_full_reg_alias, gain_peak_regval, fast_write);
        }
        else     //bank_b
        {
            put_ptr_field(gcr_addr, rx_b_ctle_gain_peak_full_reg_alias, gain_peak_regval, fast_write);
        }
    }

    // Disable servo status for result at min/max
    servo_errors_disable_only_result_at_min_or_max(gcr_addr);

    // Run the servo ops and check recal abort status.
    // The run_servo_op function checks for servo_status errors and returns warning_code/rc_warning on an error.
    set_debug_state(0x6010); // DEBUG - CTLE Run Servo ops
    int status = run_servo_ops_with_results_disabled(gcr_addr, c_servo_queue_general, num_servo_ops,
                 &servo_ops[servo_array_start]);
    status |= check_rx_abort(gcr_addr);

    // Re-enable servo status for result at min/max
    servo_errors_enable_all(gcr_addr);

    // If changed, restore servo settings to defaults (filter_depth=8 and bo_time=14)
    if (update_servo_settings)
    {
        put_ptr_field(gcr_addr, rx_ctle_filter_depth,  8, read_modify_write);
        put_ptr_field(gcr_addr, rx_bo_time,           14, fast_write); //rx_servo_timer_half_rate=0 only other field in register
    }

    // A recal abort (only checked in recal by check_rx_abort) causes a restore
    bool restore = false;

    // PSL recal_abort
    if (status & (abort_error_code | abort_clean_code))
    {
        set_debug_state(0x6080); //DEBUG - CTLE Recal Abort Condition
        restore = true;
        io_sleep(get_gcr_addr_thread(gcr_addr)); // added to resolve pcie thread time exceeded
    }

    // Servo error sets rx_ctle_peak1/2_fail. In recal, it also causes a restore; in init, we keep the result.
    // PSL bad_lane_warning
    if (status & rc_warning )
    {
        set_debug_state(0x60DD); // DEBUG - CTLE Servo Error
        mem_pl_bit_set(rx_ctle_peak1_fail, lane);
        mem_pl_bit_set(rx_ctle_peak2_fail, lane);

        if (recal)
        {
            restore = true;
        }
    }

    // If not already restoring original values, read the results and check for hysteresis and copy to main as needed
    if (!restore)
    {
        // Results can be read directly from the register
        int peak1_peak2_regval;

        if (cal_bank == bank_a)
        {
            peak1_peak2_regval = get_ptr_field(gcr_addr, rx_a_ctle_peak1_peak2_alias);
        }
        else     //bank_b
        {
            peak1_peak2_regval = get_ptr_field(gcr_addr, rx_b_ctle_peak1_peak2_alias);
        }

        // Indicate if the peak values changed (this is set to false later on a restore)
        int new_peak1 = bitfield_get((peak1_peak2_regval << rx_a_ctle_peak1_peak2_alias_shift), rx_a_ctle_peak1_mask,
                                     rx_a_ctle_peak1_shift);
        int new_peak2 = bitfield_get((peak1_peak2_regval << rx_a_ctle_peak1_peak2_alias_shift), rx_a_ctle_peak2_mask,
                                     rx_a_ctle_peak2_shift);
        int old_peak1 = bitfield_get(initial_gain_peak_regval, rx_a_ctle_peak1_mask, rx_a_ctle_peak1_shift);
        int old_peak2 = bitfield_get(initial_gain_peak_regval, rx_a_ctle_peak2_mask, rx_a_ctle_peak2_shift);
        *peak_changed = (new_peak1 != old_peak1) || (new_peak2 != old_peak2);

        // Apply hysteresis to peak1 and peak2 individually
        // PSL hyst_en
        if (hysteresis_en)
        {
            int peak1_peak2_hyst;

            // PSL hyst_en_bank_a
            if (cal_bank == bank_a)
            {
                peak1_peak2_hyst = mem_pl_field_get(rx_a_ctle_peak1_peak2_hyst_alias, lane);
            }
            else     //bank_b
            {
                peak1_peak2_hyst = mem_pl_field_get(rx_b_ctle_peak1_peak2_hyst_alias, lane);
            }

            int peak1_hyst = TwosCompToInt((peak1_peak2_hyst & 0xF0) >> rx_a_ctle_peak2_hyst_width, rx_a_ctle_peak1_hyst_width);
            int peak2_hyst = TwosCompToInt((peak1_peak2_hyst & 0x0F), rx_a_ctle_peak2_hyst_width);

            // Calculate the new peak values and hysteresis count
            int ctle_hyst_settings_regval = mem_pg_field_get(rx_ctle_peak_hyst_settings_full_reg);

            int peak1_hyst_delta_mode = bitfield_get(ctle_hyst_settings_regval, rx_ctle_peak1_hyst_delta_mode_mask,
                                        rx_ctle_peak1_hyst_delta_mode_shift);
            int peak1_hyst_limit      = bitfield_get(ctle_hyst_settings_regval, rx_ctle_peak1_hyst_limit_mask,
                                        rx_ctle_peak1_hyst_limit_shift);
            ctle_hysteresis(peak1_hyst_delta_mode, peak1_hyst_limit, old_peak1, &new_peak1, &peak1_hyst);

            int peak2_hyst_delta_mode = bitfield_get(ctle_hyst_settings_regval, rx_ctle_peak2_hyst_delta_mode_mask,
                                        rx_ctle_peak2_hyst_delta_mode_shift);
            int peak2_hyst_limit      = bitfield_get(ctle_hyst_settings_regval, rx_ctle_peak2_hyst_limit_mask,
                                        rx_ctle_peak2_hyst_limit_shift);
            ctle_hysteresis(peak2_hyst_delta_mode, peak2_hyst_limit, old_peak2, &new_peak2, &peak2_hyst);

            // Save the hysteresis count
            peak1_peak2_hyst = ((peak1_hyst & 0xF) << rx_a_ctle_peak2_hyst_width) | (peak2_hyst & 0xF);

            if (cal_bank == bank_a)
            {
                mem_pl_field_put(rx_a_ctle_peak1_peak2_hyst_alias, lane, peak1_peak2_hyst);
            }
            else     //bank_b
            {
                mem_pl_field_put(rx_b_ctle_peak1_peak2_hyst_alias, lane, peak1_peak2_hyst);
            }

            // Process the final results
            // PSL hyst_peak1_peak2_equal
            if ( (new_peak1 == old_peak1) && (new_peak2 == old_peak2) )
            {
                // If neither peak1 nor peak2 changed after applying hysteresis, restore the previous results
                restore = true;
            }
            else
            {
                // When at least one of peak1 or peak2 changed after applying hysteresis,
                // write the new values to the the registers
                set_debug_state(0x60AB, EO_CTLE_DBG_LVL); // DEBUG - CTLE Hysteresis Exceeded

                peak1_peak2_regval = (new_peak1 << (rx_a_ctle_peak1_shift - rx_a_ctle_peak1_peak2_alias_shift)) |
                                     (new_peak2 << (rx_a_ctle_peak2_shift - rx_a_ctle_peak1_peak2_alias_shift));

                // PSL hyst_peak1_peak2_ne_bank_a
                if (cal_bank == bank_a)
                {
                    put_ptr_field(gcr_addr, rx_a_ctle_peak1_peak2_alias, peak1_peak2_regval, read_modify_write);
                }
                else     //bank_b
                {
                    put_ptr_field(gcr_addr, rx_b_ctle_peak1_peak2_alias, peak1_peak2_regval, read_modify_write);
                }
            }
        }
        else     //!hysteresis_en
        {
            // Clear hysteresis bits when hysteresis is not enabled
            // PSL nohyst_bank_a
            if (cal_bank == bank_a)
            {
                mem_pl_field_put(rx_a_ctle_peak1_peak2_hyst_alias, lane, 0);
            }
            else
            {
                mem_pl_field_put(rx_b_ctle_peak1_peak2_hyst_alias, lane, 0);
            }
        } //hysteresis_en

        // Only copy to main if not restoring
        if (!restore && copy_to_main)
        {
            // PSL copy_main_bank_a
            if (cal_bank == bank_a)
            {
                put_ptr_field(gcr_addr, rx_b_ctle_peak1_peak2_alias, peak1_peak2_regval, read_modify_write);
            }
            else     //bank_b
            {
                put_ptr_field(gcr_addr, rx_a_ctle_peak1_peak2_alias, peak1_peak2_regval, read_modify_write);
            }
        }
    }//if (!restore)

    // Restore the original results if required
    // PSL restore
    if (restore)
    {
        set_debug_state(0x60AA, EO_CTLE_DBG_LVL); // DEBUG - CTLE Restore

        // PSL restore_bank_a
        if (cal_bank == bank_a)
        {
            put_ptr_field(gcr_addr, rx_a_ctle_gain_peak_full_reg_alias, initial_gain_peak_regval, fast_write);
        }
        else     //bank_b
        {
            put_ptr_field(gcr_addr, rx_b_ctle_gain_peak_full_reg_alias, initial_gain_peak_regval, fast_write);
        }

        // Indicate the peak value did not change
        *peak_changed = false;
    }

    // Set BIST done bits
    // PSL set_done_bank_a
    if (cal_bank == bank_a)
    {
        if (!peak1_disable)
        {
            mem_pl_bit_set(rx_a_ctle_peak1_done, lane);
        }

        if (!peak2_disable)
        {
            mem_pl_bit_set(rx_a_ctle_peak2_done, lane);
        }
    }
    else     //bank_b
    {
        if (!peak1_disable)
        {
            mem_pl_bit_set(rx_b_ctle_peak1_done, lane);
        }

        if (!peak2_disable)
        {
            mem_pl_bit_set(rx_b_ctle_peak2_done, lane);
        }
    }

    // Return Status
    set_debug_state(0x60FF); // DEBUG - CTLE Complete
    return status;
} //eo_ctle
