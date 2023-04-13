/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_vclq_checks.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_vclq_checks.c
// *! TITLE       : RX BIST Fail Checking
// *! DESCRIPTION : Check that value are in correct range, used in DFT and system
// *!             :
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh23033000 |mwh     | EWM 300841 put back debug statement to support lab.
// mwh23011300 |mwh     | moved to lib.c since used by iot and ioo void set_rxbist_fail_lane
// mwh22111100 |mwh     | Removed code checking for LTE gain and zero
// mbs22082601 |mbs     | Updated with PSL comments
// mwh21008110 |mwh     | To support removing of gcr  rx_a/b_lane_fail_0_15_16_23 and moving to using rx_lane_fail_0_15,16_23
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// mwh20101300 |mwh     | After talking with Chris and Jim moved logger for eoff back to eoff, lane scom get set up here
//                                       | done in ioo_thread
// mwh20100900 |mwh     | HW549417: Move the add_log to inside the if fail so only logs if there a fail
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// bja20012300 |bja     | Move setting of per group fail mask to function
// mwh20012820 |mwh     | There is no min max for phase data -- HW516933
// cws20011400 |cws     | Added Debug Logs
// bja19100300 |bja     | enable peak2 checking
// bja19081400 |bja     | improve code speed, size, and clarity
// bja19081401 |bja     | set per-group fail regs
// mwh19043019 |mwh     | add in set_fir(fir_code_dft_error);
// mwh19042919 |mwh     | add in scom vec write for done on lanes for bist
// jfg19043001 |jfg     | Manually merge divergent main
// vbr19041500 |vbr     | Updated register names
// mwh19041719 |mwh     | add in peak2 code but it is commited out until merge with test_march_26_model_cns
// mwh19040119 |mwh     | removed the rx_fail_flag code -- code size hit and taking out reduce sim
// mwh19011101 |mwh     | Took out quad phase fail = 0 reset
// mwh19011100 |mwh     | Add ()  because the || could could cause compare to wrong thing
// mwh18112800 |mwh     | Initial Rev 51yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"

#include "ppe_img_reg_const_pkg.h"
#include "ppe_fw_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"

#include "io_config.h"

#include "eo_vclq_checks.h"
#include "io_logger.h"

//checking clte gain, ctle peak, lte gain, lte zero, qpa
void eo_vclq_checks(t_gcr_addr* gcr_addr, t_bank bank, int bist_check_en)
{
    set_debug_state(0x5100); // start vclq checks

    const uint16_t rx_ctle_gain_check_en  = (bist_check_en & rx_ctle_gain_check_en_mask); //0x8000
    const uint16_t rx_ctle_peak1_check_en = (bist_check_en & rx_ctle_peak1_check_en_mask); //0x0800
    const uint16_t rx_ctle_peak2_check_en = (bist_check_en & rx_ctle_peak2_check_en_mask); //0x0400
    //const uint16_t rx_lte_gain_check_en   =(bist_check_en & rx_lte_gain_check_en_mask);//0x0200
    //const uint16_t rx_lte_zero_check_en   =(bist_check_en & rx_lte_zero_check_en_mask);//0x0100
    const uint16_t rx_quad_phase_check_en = (bist_check_en & rx_quad_phase_check_en_mask); //0x0040


    int lane = get_gcr_addr_lane(gcr_addr);

    /////////////////////////////////////////////////////////
    // Check fail conditions and set fail registers
    /////////////////////////////////////////////////////////
    {
        // start scope limiting - A

        uint8_t fail = 0; // 0 = pass, 1 = fail

        // create 3*uint32_t arrays to retain register access information
        // store access information to speed up
        mk_ptr_ary(rx_ctle_gain);
        mk_ptr_ary(rx_ctle_peak1);
        mk_ptr_ary(rx_ctle_peak2);
        //mk_ptr_ary(rx_lte_gain);
        //mk_ptr_ary(rx_lte_zero);
        //mk_ptr_ary(rx_quad_ph_adj_nsd);
        mk_ptr_ary(rx_quad_ph_adj_nse);
        //mk_ptr_ary(rx_quad_ph_adj_ewd);
        mk_ptr_ary(rx_quad_ph_adj_ewe);

        // fill arrays with bank-dependent register values
        // PSL bist_checks_bank_a
        if (bank == bank_a)
        {
            //set_debug_state(0x5150); // bank a
            asn_ptr_ary(rx_ctle_gain,       rx_a_ctle_gain);
            asn_ptr_ary(rx_ctle_peak1,      rx_a_ctle_peak1);
            asn_ptr_ary(rx_ctle_peak2,      rx_a_ctle_peak2);
            //asn_ptr_ary(rx_lte_gain,        rx_a_lte_gain);
            //asn_ptr_ary(rx_lte_zero,        rx_a_lte_zero);
            //asn_ptr_ary(rx_quad_ph_adj_nsd, rx_a_pr_ns_data);
            asn_ptr_ary(rx_quad_ph_adj_nse, rx_a_pr_ns_edge);
            //asn_ptr_ary(rx_quad_ph_adj_ewd, rx_a_pr_ew_data);
            asn_ptr_ary(rx_quad_ph_adj_ewe, rx_a_pr_ew_edge);
        }
        else
        {
            //set_debug_state(0x5151); // bank b
            asn_ptr_ary(rx_ctle_gain,       rx_b_ctle_gain);
            asn_ptr_ary(rx_ctle_peak1,      rx_b_ctle_peak1);
            asn_ptr_ary(rx_ctle_peak2,      rx_b_ctle_peak2);
            //asn_ptr_ary(rx_lte_gain,        rx_b_lte_gain);
            //asn_ptr_ary(rx_lte_zero,        rx_b_lte_zero);
            //asn_ptr_ary(rx_quad_ph_adj_nsd, rx_b_pr_ns_data);
            asn_ptr_ary(rx_quad_ph_adj_nse, rx_b_pr_ns_edge);
            //asn_ptr_ary(rx_quad_ph_adj_ewd, rx_b_pr_ew_data);
            asn_ptr_ary(rx_quad_ph_adj_ewe, rx_b_pr_ew_edge);
        }

        //Check ctle gain (vga) is not to low or to high
        // PSL bist_gain_check
        if( rx_ctle_gain_check_en)
        {
            //set_debug_state(0x5152); // checking ctle gain

            // declare in this scope to reduce stack size
            int rx_ctle_gain_int = get_ptr_ary(gcr_addr, rx_ctle_gain);

            //evaluate consecutively to reduce memory accesses, but increases coverage
            //can handle min/max cases separately (e.g. fail bits or debug state)
            // PSL bist_gain_check_lt_min
            if ( rx_ctle_gain_int < mem_pg_field_get(rx_ctle_gain_min_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_VGA_GAIN_FAIL, gcr_addr, rx_ctle_gain_int);
                set_debug_state(0x5153); // EWM300841: Don't  Optimize out the debug state
            }
            // PSL bist_gain_check_gt_max
            else if ( rx_ctle_gain_int > mem_pg_field_get(rx_ctle_gain_max_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_VGA_GAIN_FAIL, gcr_addr, rx_ctle_gain_int);
                set_debug_state(0x5154); // EWM300841: Don't  Optimize out the debug state
            }

            //set the fail to register
            mem_pl_field_put(rx_ctle_gain_fail, lane, fail);
            fail = 0;
        }


        //Check ctle peak is not to low or to high
        // PSL bist_peak1_check
        if(rx_ctle_peak1_check_en)
        {
            //set_debug_state(0x5156); // checking peak1

            int rx_ctle_peak1_int = get_ptr_ary(gcr_addr, rx_ctle_peak1);

            // PSL bist_peak1_check_lt_min
            if ( rx_ctle_peak1_int < mem_pg_field_get(rx_ctle_peak1_min_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_CTLE_PEAK1_FAIL, gcr_addr, rx_ctle_peak1_int);
                set_debug_state(0x5157); // EWM300841: Don't  Optimize out the debug state
            }
            // PSL bist_peak1_check_gt_max
            else if ( rx_ctle_peak1_int >  mem_pg_field_get(rx_ctle_peak1_max_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_CTLE_PEAK1_FAIL, gcr_addr, rx_ctle_peak1_int);
                set_debug_state(0x5158); // EWM300841: Don't  Optimize out the debug state
            }

            mem_pl_field_put(rx_ctle_peak1_fail, lane, fail );
            fail = 0;
        }

        //Check ctle peak is not to low or to high
        // PSL bist_peak2_check
        if(rx_ctle_peak2_check_en)
        {
            //set_debug_state(0x515A); // checking peak1

            int rx_ctle_peak2_int = get_ptr_ary(gcr_addr, rx_ctle_peak2);

            // PSL bist_peak2_check_lt_min
            if ( rx_ctle_peak2_int < mem_pg_field_get(rx_ctle_peak2_min_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_CTLE_PEAK2_FAIL, gcr_addr, rx_ctle_peak2_int);
                set_debug_state(0x515B); // EWM300841: Don't  Optimize out the debug state
            }
            // PSL bist_peak2_check_gt_max
            else if ( rx_ctle_peak2_int >  mem_pg_field_get(rx_ctle_peak2_max_check) )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_CTLE_PEAK2_FAIL, gcr_addr, rx_ctle_peak2_int);
                set_debug_state(0x515C); // EWM300841: Don't  Optimize out the debug state
            }

            mem_pl_field_put(rx_ctle_peak2_fail, lane, fail );
            fail = 0;
        }

        //Check lte gain is not to low or to high
        // PSL bist_lte_gain_check
        //if( rx_lte_gain_check_en) {
        //    //set_debug_state(0x515E); // checking lte gain
        //
        //    int rx_lte_gain_int = get_ptr_ary(gcr_addr, rx_lte_gain);
        //
        //    // PSL bist_lte_gain_check_lt_min
        //    if ( rx_lte_gain_int < mem_pg_field_get(rx_lte_gain_min_check) ) {
        //        fail = 1;
        //        ADD_LOG(DEBUG_BIST_LTE_GAIN_FAIL, gcr_addr, rx_lte_gain_int);
        //        set_debug_state(0x516F);
        //    }
        //    // PSL bist_lte_gain_check_gt_max
        //    else if ( rx_lte_gain_int >  mem_pg_field_get(rx_lte_gain_max_check) ) {
        //         fail = 1;
        //        ADD_LOG(DEBUG_BIST_LTE_GAIN_FAIL, gcr_addr, rx_lte_gain_int);
        //        set_debug_state(0x5160);
        //    }
        //    else {
        //        fail = 0;
        //        set_debug_state(0x5161);
        //    }
        //
        //
        //    mem_pl_field_put(rx_lte_gain_fail, lane, fail );
        // }

        //check rx_lte_zero values start
        // PSL bist_lte_zero_check
        //if(rx_lte_zero_check_en) {
        //    //set_debug_state(0x5162); // checking lte gain
        //    int rx_lte_zero_int = get_ptr_ary(gcr_addr, rx_lte_zero);
        //
        //    // PSL bist_lte_zero_check_lt_min
        //    if ( rx_lte_zero_int  < mem_pg_field_get(rx_lte_zero_min_check) ) {
        //        fail = 1;
        //        ADD_LOG(DEBUG_BIST_LTE_ZERO_FAIL, gcr_addr, rx_lte_zero_int);
        //        set_debug_state(0x5163);
        //    }
        //    // PSL bist_lte_zero_check_gt_max
        //    else if ( rx_lte_zero_int >  mem_pg_field_get(rx_lte_zero_max_check) ) {
        //        fail = 1;
        //        ADD_LOG(DEBUG_BIST_LTE_ZERO_FAIL, gcr_addr, rx_lte_zero_int);
        //        set_debug_state(0x5164);
        //    }
        //    else {
        //        fail = 0;
        //        set_debug_state(0x5165);
        //    }
        //
        //
        //      mem_pl_field_put(rx_lte_zero_fail, lane, fail );
        // }

        //check quad phase adj is not to low or to high start
        // PSL bist_quad_phase_check
        if(rx_quad_phase_check_en)
        {
            //set_debug_state(0x5166); // checking qpa

            //int rx_quad_ph_adj_nsd_int = get_ptr_ary(gcr_addr, rx_quad_ph_adj_nsd);
            int rx_quad_ph_adj_nse_int = get_ptr_ary(gcr_addr, rx_quad_ph_adj_nse);
            //int rx_quad_ph_adj_ewd_int = get_ptr_ary(gcr_addr, rx_quad_ph_adj_ewd);
            int rx_quad_ph_adj_ewe_int = get_ptr_ary(gcr_addr, rx_quad_ph_adj_ewe);
            int check_quad_ph_adj_max = mem_pg_field_get(rx_quad_ph_adj_max_check);
            int check_quad_ph_adj_min = mem_pg_field_get(rx_quad_ph_adj_min_check);

            //if ( rx_quad_ph_adj_nsd_int <  check_quad_ph_adj_min ) {
            //    fail = 1;
            //    set_debug_state(0x5167);
            //}
            //else if ( rx_quad_ph_adj_nsd_int >  check_quad_ph_adj_max ) {
            //    fail = 1;
            //    set_debug_state(0x5168);
            //}
            // PSL bist_quad_phase_check_nse_lt_min
            if ( rx_quad_ph_adj_nse_int <  check_quad_ph_adj_min )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_QPA_FAIL, gcr_addr, 0x0);
                set_debug_state(0x5169); // EWM300841: Don't  Optimize out the debug state
            }
            // PSL bist_quad_phase_check_nse_gt_max
            else if ( rx_quad_ph_adj_nse_int >  check_quad_ph_adj_max )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_QPA_FAIL, gcr_addr, 0x0);
                set_debug_state(0x516A); // EWM300841: Don't  Optimize out the debug state
            }
            //else if ( rx_quad_ph_adj_ewd_int <  check_quad_ph_adj_min ) {
            //    fail = 1;
            //    set_debug_state(0x516B);
            //}
            //else if ( rx_quad_ph_adj_ewd_int >  check_quad_ph_adj_max ) {
            //    fail = 1;
            //    set_debug_state(0x516C);
            //}
            // PSL bist_quad_phase_check_ewe_lt_min
            else if ( rx_quad_ph_adj_ewe_int <  check_quad_ph_adj_min )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_QPA_FAIL, gcr_addr, 0x0);
                set_debug_state(0x516D); // EWM300841: Don't  Optimize out the debug state
            }
            // PSL bist_quad_phase_check_ewe_gt_max
            else if ( rx_quad_ph_adj_ewe_int >  check_quad_ph_adj_max )
            {
                fail = 1;
                ADD_LOG(DEBUG_BIST_QPA_FAIL, gcr_addr, 0x0);
                set_debug_state(0x516E); // EWM300841: Don't  Optimize out the debug state
            }

            mem_pl_field_put(rx_quad_phase_fail, lane, fail);
            fail = 0;
        }

    } // end scope limiting - A
    /////////////////////////////////////////////////////////
    // Write appropriate per-group fail bit if any step failed
    /////////////////////////////////////////////////////////
    {
        // start  scope limiting - B

        mk_ptr_ary(pg_done_reg);
        uint8_t lane_bit_offset;

        // set register access values according to bank and lane number
        // PSL lane_lt_16
        if (lane < 16)
        {
            lane_bit_offset = 15;

            if (bank == bank_a)
            {
                asn_ptr_ary( pg_done_reg, rx_a_lane_done_0_15 );
            }
            else
            {
                asn_ptr_ary( pg_done_reg, rx_b_lane_done_0_15 );
            }
        }
        else
        {
            lane_bit_offset = 23;

            if (bank == bank_a)
            {
                asn_ptr_ary( pg_done_reg, rx_a_lane_done_16_23 );
            }
            else
            {
                asn_ptr_ary( pg_done_reg, rx_b_lane_done_16_23 );
            }
        }

        // shift the lane bit to the appropriate spot for the reg selected
        uint32_t lane_mask = 1 << ( lane_bit_offset - lane );

        // setting done
        // read in the current reg values
        uint32_t pg_reg_val = get_ptr_ary( gcr_addr, pg_done_reg );
        // or in the lane bit
        pg_reg_val = ( pg_reg_val | lane_mask );
        // write the new value to pg done reg
        put_ptr_ary( gcr_addr, pg_done_reg, pg_reg_val, read_modify_write );

        // only write the pg fails if any per-step fail is set
        if ( mem_pl_field_get(rx_step_fail_alias, lane) != 0 )
        {
            // setting fail
            set_debug_state(0x5171, 3);
            //set_rxbist_fail_lane( gcr_addr, bank );
            set_rxbist_fail_lane( gcr_addr);
        }
    } // end scope limiting - B


    set_debug_state(0x5101); // end vclq checks
}//vclq_checks()
