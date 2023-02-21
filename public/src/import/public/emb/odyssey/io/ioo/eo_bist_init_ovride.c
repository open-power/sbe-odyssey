/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_bist_init_ovride.c $ */
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
// *! FILENAME    : eo_bist_init_ovride.c
// *! TITLE       : n/a
// *! DESCRIPTION : Reset the done ppe register for each eye opt step RX
// *!             : A Bank and B Bank
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
// vbr23011000 |vbr     | Issue 297222: Moved disabling of thread active time to start of hw_reg_init
// jjb23010400 |jjb     | Removed unnecessary register read when setting bist enables
// jjb22113000 |jjb     | Updated dac_test_check_en and rx_esd_check_en query code
// mwh22111100 |mwh     | Updated for ESD
// mbs22082601 |mbs     | Updated with PSL comments
// mwh22092140 |mwh     | Change rx_min_recal_cnt to 1 for bist issue_290239
// vbr22072100 |vbr     | Moved thread_active_time disable from mem_regs to img_regs
// mwh22072100 |mwh     | Change 1 to 0b1 for active thread and bist in progress
// mwh22071800 |mwh     | Add in rx_enable_auto_recal_0_15=0x0 disable auto-recal for bist
// mwh22040800 |mwh     | add in tx_bist_txdetidle_en_alias idle bist and txdet bist and rx_sigdet_test_en
// jjb21011200 |jjb     | replaced *bank_sync* with *sigdet*
// mwh22011900 |mwh     |- removed logic related to tc_iobist_start_test_0 and iobist_mode_dc
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21082500 |vbr     | Explicitly disable the thread active time check in BIST mode
// vbr21072900 |vbr     | Clear rx_dfe_full_mode so actually test all quadrants in 2 recals (1 for each bank) during BIST
// vbr21012100 |vbr     | Split number of RX/TX lanes
// vbr21030800 |vbr     | HW560154: Prevent overwritting of rx_bist_cir settings (freq_adj, atten); allow dac_test to be enabled/disabled individually.
// mwh20073000 |mwh     | Change the rx_check_en_alias FFFE to FFFF so dac test is included
// mwh20022701 |mwh     | Removed extra powerdown write and comminted out the write of step since they are default to all on
// mwh20022700 |mwh     | Put in fix for CQ523188 and CQ526617 -- put sleep in loop, fix shared loop issue
// mwh20022600 |mwh     | Removed extra bit form rx_eo_step_cntl_opt_alias
// mwh19121100 |mwh     | For new call of bist add in rx_disable_bank_pdwn = 1
// mwh19121000 |mwh     | removed rx_enable_auto_recal -- this will be replaces by usning external recal command.
// bja19082200 |bja     | add bits to rx_bist_cir_alias
// bja19081900 |bja     | Switch addr to tx_group before setting tx_bist_en
// cws19053000 |cws     | Removed hw_reg_init, dccal, run_lane calls
// mwh19052200 |mwh     | updated rx_bist_cir_alias
// mwh19042600 |mwh     | Initial Rev 51yy debug state
// jfg19050101 |jfg     | change peak_done to peak1_done by Mike Harper's request
// vbr19041500 |vbr     | Updated register names
// mwh19041719 |mwh     | change peak to peak1
// mwh19040119 |mwh     | removed the rx_fail_flag code -- code size hit and taking out reduce sim
// mwh19012100 |mwh     | Add in way to start rxbist with shared test
// mwh19011100 |mwh     | Initial Rev 51yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

#include "eo_bist_init_ovride.h"


// Assumption Checking
PK_STATIC_ASSERT(rx_check_en_alias_width == 16);
PK_STATIC_ASSERT(rx_check_en_alias_addr == rx_dac_test_check_en_addr);
PK_STATIC_ASSERT(rx_dac_test_check_en_width == 1);
PK_STATIC_ASSERT(rx_dac_test_check_en_startbit == 15);


//checking clte gain, ctle peak, lte gain, lte zero, qpa
void eo_bist_init_ovride(t_gcr_addr* gcr_addr)
{
    //start eo_rxbist_init_or_override.c

    //assume manu and system do a scan flush reset
    mem_pg_field_put(bist_in_progress, 0b1);

    // Don't care about thread active times in BIST
    // Moved to hw_reg_init: img_field_put(ppe_disable_thread_active_time_check, 0b1);

    int lane = 0;//for the "for loop" used on pl registers
    int bist_num_lanes_rx = get_num_rx_lane_slices();//getting max number of lanes per theard
    int bist_num_lanes_tx = get_num_tx_lane_slices();//getting max number of lanes per theard


    //enabling all rx bist checks except for dac_test and ESD_test which are enabled/disabled by tester pattern (HW560154)
    int rx_check_en_alias_value = 0xFDFE | get_ptr_field(gcr_addr, rx_check_en_alias);
    put_ptr_field(gcr_addr, rx_check_en_alias, rx_check_en_alias_value, fast_write); //pg

    //change rx_min_recal_cnt to 1 for bist issue_290239 -- need only 1 recal for bist
    mem_pg_field_put(rx_min_recal_cnt, 0b0001); //pg ppe register


    // switch to tx address
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    //enabling all tx bist checks
    put_ptr_field(gcr_addr, tx_bist_en_alias, 0b111, read_modify_write ); //pg


    for (lane = 0; lane <  bist_num_lanes_tx; lane++)
    {
        //begin for
        set_gcr_addr_lane(gcr_addr, lane);
        put_ptr_field(gcr_addr, tx_bist_txdetidle_en_alias, 0b11, read_modify_write ); //pl
    }//end for


    // switch gcr address to rx
    set_gcr_addr_reg_id(gcr_addr, rx_group);

    //turn on pervasive_capt see what pervasive signs are set scan only observ
    put_ptr_field(gcr_addr, rx_pervasive_capt, 0b1, read_modify_write ); //pg

    //trun off alt bank powerdown
    mem_pg_field_put(rx_disable_bank_pdwn, 0b1);//pg

    //turning on all init steps -- removed since default to all on
    //mem_pg_field_put(rx_eo_step_cntl_opt_alias,0b111111111);//pg

    //turning on all recal steps -- removed since default to all on
    //mem_pg_field_put(rx_rc_step_cntl_opt_alias,0b111111111);//pg

    // Set rx_dfe_full_mode=0 so that DFE Full runs on all quadrants in each recal for BIST
    mem_pg_field_put(rx_dfe_full_mode, 0b0);

    //Disabling auto recal since using the recal command
    mem_pg_field_put(rx_enable_auto_recal_0_15, 0x0000);


    for (lane = 0; lane <  bist_num_lanes_rx; lane++)
    {
        //begin for
        //turn on circuit components for bist
        // Bist circuit settings are done elsewhere (HW548766, HW560154)
        set_gcr_addr_lane(gcr_addr, lane);
        put_ptr_field(gcr_addr, rx_bist_en_dc, 0b1, read_modify_write); //pl
        put_ptr_field(gcr_addr, rx_sigdet_test_en, 0b1, read_modify_write); //pl

        //doing lane power up
        //mem_pl_field_put(io_power_up_lane_req,lane,0b1);//pl

        //enabling init and recal with two below
        //mem_pl_field_put(rx_enable_auto_recal,lane,0b1);//pl
        //mem_pl_field_put(rx_run_lane,lane,0b1);//pl
    }//end for

    //system vs manu setting specific stuff

    set_gcr_addr_lane(gcr_addr, 0);
    int system_vs_tester = get_ptr_field(gcr_addr, system_manu_sel);//pg

    if (system_vs_tester)
    {
        //setting change specific to manu/dft
    }
    else
    {
        //setting change specific to system
    }

    mem_pg_field_put(bist_in_hold_loop, 1);
    //while loop for syncclk mux -- must be set 0 -- system should fly through this
    int rx_syncclk_muxsel_dc_int = get_ptr_field(gcr_addr, rx_syncclk_muxsel_dc); //pg rox

    // PSL syncclk_muxsel
    while (rx_syncclk_muxsel_dc_int == 1)
    {
        //begin while
        rx_syncclk_muxsel_dc_int = get_ptr_field(gcr_addr, rx_syncclk_muxsel_dc); //pg rox
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }//end


    //Shared test pin is tied to 1 for IO that are not shared at chip level
    //Shared_test = 0 = on ( IO are shared for test)
    //Shared_test = 1 = off ( IO are not shared for test)
    //tc_bist_shared_ovrd =  rx_ctl_cntl5_pg, bit 1 (start at 0)
    //tc_bist_code_go =  rx_ctl_cntl5_pg, bit 3 (start at 0)
    //bist_in_hold_loop =  rx_ppe_stat28_pg, bit 1  (start at 0)


    //while loop for shared test pin must be set if running txbist
    int tc_shared_pin_dc_int = 0;
    int tc_bist_shared_ovrd_int = 0;
    int tc_bist_code_go_int = 0;
    int l_run_bist = 0;

    // PSL run_bist_loop
    do
    {
        io_sleep(get_gcr_addr_thread(gcr_addr));
        tc_shared_pin_dc_int = get_ptr_field(gcr_addr, tc_shared_pin_dc); //pg rox
        tc_bist_shared_ovrd_int = get_ptr_field(gcr_addr, tc_bist_shared_ovrd); //pg
        tc_bist_code_go_int = get_ptr_field(gcr_addr, tc_bist_code_go); //pg

        l_run_bist = ((tc_shared_pin_dc_int | tc_bist_shared_ovrd_int) & tc_bist_code_go_int) & 0x1;
    }
    while (l_run_bist == 0);

    mem_pg_field_put(bist_in_hold_loop, 0);

    //clearing the work reg so we do not repeat the bist run
    lcl_put(scom_ppe_work1_lcl_addr, scom_ppe_work1_width, 0x00000000);

}//end  eo_bist_init_ovride.c






//mem_pl_field_put(rx_ctle_peak1_fail, lane, 0b1 )
//read_modify_write)
//reseting all the dones
//mem_pl_field_put(rx_a_ctle_gain_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_ctle_gain_done, lane, 0b0); //loop so get reset in loop also
//
//mem_pl_field_put(rx_a_latch_offset_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_latch_offset_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_ctle_peak1_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_ctle_peak1_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_lte_gain_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_lte_gain_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_lte_freq_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_lte_freq_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_sigdet_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_sigdet_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_quad_phase_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_quad_phase_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_dfe_h1_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_dfe_h1_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_dfe_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_dfe_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_ddc_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_ddc_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_ber_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_ber_done, lane, 0b0);//only reset here
//
//mem_pg_field_put(rx_linklayer_done, 0b0);//only reset here

//Reset fail bits
//mem_pl_field_put(rx_ctle_gain_fail   , lane, 0b0);
//mem_pl_field_put(rx_latch_offset_fail, lane, 0b0);
//mem_pl_field_put(rx_eoff_fail        , lane, 0b0);
//mem_pl_field_put(rx_ctle_peak1_fail  , lane, 0b0);
//mem_pl_field_put(rx_lte_gain_fail    , lane, 0b0);
//mem_pl_field_put(rx_lte_freq_fail    , lane, 0b0);
//mem_pl_field_put(rx_sigdet_fail      , lane, 0b0);
//mem_pl_field_put(rx_quad_phase_fail  , lane, 0b0);
//mem_pl_field_put(rx_dfe_h1_fail      , lane, 0b0);
//mem_pl_field_put(rx_dfe_fail         , lane, 0b0);
//mem_pl_field_put(rx_ddc_fail         , lane, 0b0);
//mem_pl_field_put(rx_ber_fail         , lane, 0b0);
//mem_pg_field_put(rx_linklayer_fail         , 0b0);



//The if statement below is to be used if you want different settings
//system vs tester (wafer or module)
//
//int system_vs_tester = get_ptr_field(gcr_addr, system_manu_sel );
//int shared_pin_start = get_ptr_field(gcr_addr, tc_shared_pin_dc);
//int tc_bist_shared_ovrd = get_ptr_field(gcr_addr, tc_bist_shared_ovrd);
//int tc_bist_opcg_go_ovrd = get_ptr_field(gcr_addr, tc_bist_opcg_go_ovrd);
//
//  if (system_vs_tester)
//    {//begin system_vs_tester
//
//      while ((!shared_pin_start || ( tc_bist_shared_ovrd)) && (tc_opcg_iobist_go || ( tc_bist_opcg_go_ovrd)))
//        {//begin while
//         tc_bist_shared_ovrd = get_ptr_field(gcr_addr,tc_bist_shared_ovrd );
//         tc_bist_opcg_go_ovrd = get_ptr_field(gcr_addr,tc_bist_opcg_go_ovrd );
//  }//end while
//
//        put_ptr_field(gcr_addr, tc_bist_shared_ovrd  ,0b1, read_modify_write);
//        put_ptr_field(gcr_addr, tc_bist_opcg_go_ovrd ,0b1, read_modify_write);
//
//     }//end system_vs_tester





//scom register that will get set at end
//Not going reset here, since dependend on above
//Add code just in case

//put_ptr_field(gcr_addr,rx_a_lane_fail_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_a_lane_fail_16_23 ,0b00000000         , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_fail_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_fail_16_23 ,0b00000000         , fast_write);

//put_ptr_field(gcr_addr,rx_a_lane_done_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_a_lane_done_16_23 ,0b00000000         , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_done_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_done_16_23 ,0b00000000         , fast_write);

//put_ptr_field(gcr_addr,rx_section_fail      ,0b0000000000000000 , fast_write);

//put_ptr_field(gcr_addr,tc_opcg_iobist_go       ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_io_pb_iobist_reset   ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_a_bist_en            ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_b_bist_en            ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_bistclk_en           ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_a_prbs15_adj         ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_b_prbs15_adj         ,0b0   , read_modify_write);

//put_ptr_field(gcr_addr,rx_check_en_alias,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_ctle_gain_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_latchoff_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_eoff_check_en        ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_eoff_poff_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_ctle_peak_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_lte_gain_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_lte_freq_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_sigdet_check_en      ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_quad_phase_check_en  ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_dfe_h1_check_en      ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_dfe_check_en         ,0b0, read_modify_write);
//put_ptr_field  //mem_pl_field_put(rx_ctle_peak1_fail, lane, 0b1 )
//read_modify_write)
//reseting all the dones
//mem_pl_field_put(rx_a_ctle_gain_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_ctle_gain_done, lane, 0b0); //loop so get reset in loop also
//
//mem_pl_field_put(rx_a_latch_offset_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_latch_offset_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_ctle_peak1_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_ctle_peak1_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_lte_gain_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_lte_gain_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_lte_freq_done, lane, 0b0);//loop so get reset in loop also
//mem_pl_field_put(rx_b_lte_freq_done, lane, 0b0);//loop so get reset in loop also
//
//mem_pl_field_put(rx_a_sigdet_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_sigdet_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_quad_phase_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_quad_phase_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_dfe_h1_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_dfe_h1_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_dfe_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_dfe_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_ddc_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_ddc_done, lane, 0b0);//only reset here
//
//mem_pl_field_put(rx_a_ber_done, lane, 0b0);//only reset here
//mem_pl_field_put(rx_b_ber_done, lane, 0b0);//only reset here
//
//mem_pg_field_put(rx_linklayer_done, 0b0);//only reset here

//Reset fail bits
//mem_pl_field_put(rx_ctle_gain_fail   , lane, 0b0);
//mem_pl_field_put(rx_latch_offset_fail, lane, 0b0);
//mem_pl_field_put(rx_eoff_fail        , lane, 0b0);
//mem_pl_field_put(rx_ctle_peak1_fail  , lane, 0b0);
//mem_pl_field_put(rx_lte_gain_fail    , lane, 0b0);
//mem_pl_field_put(rx_lte_freq_fail    , lane, 0b0);
//mem_pl_field_put(rx_sigdet_fail      , lane, 0b0);
//mem_pl_field_put(rx_quad_phase_fail  , lane, 0b0);
//mem_pl_field_put(rx_dfe_h1_fail      , lane, 0b0);
//mem_pl_field_put(rx_dfe_fail         , lane, 0b0);
//mem_pl_field_put(rx_ddc_fail         , lane, 0b0);
//mem_pl_field_put(rx_ber_fail         , lane, 0b0);
//mem_pg_field_put(rx_linklayer_fail         , 0b0);


//The if statement below is to be used if you want different settings
//system vs tester (wafer or module)
//
//int system_vs_tester = get_ptr_field(gcr_addr, system_manu_sel );
//int shared_pin_start = get_ptr_field(gcr_addr, tc_shared_pin_dc);
//int tc_bist_shared_ovrd = get_ptr_field(gcr_addr, tc_bist_shared_ovrd);
//int tc_bist_opcg_go_ovrd = get_ptr_field(gcr_addr, tc_bist_opcg_go_ovrd);
//int tc_opcg_iobist_go = get_ptr_field(gcr_addr, tc_opcg_iobist_go);
//
//  if (system_vs_tester)
//    {//begin system_vs_tester
//
//      while ((!shared_pin_start || ( tc_bist_shared_ovrd)) && (tc_opcg_iobist_go || ( tc_bist_opcg_go_ovrd)))
//        {//begin while
//         tc_bist_shared_ovrd = get_ptr_field(gcr_addr,tc_bist_shared_ovrd );
//         tc_bist_opcg_go_ovrd = get_ptr_field(gcr_addr,tc_bist_opcg_go_ovrd );
//  }//end while
//
//        put_ptr_field(gcr_addr, tc_bist_shared_ovrd  ,0b1, read_modify_write);
//        put_ptr_field(gcr_addr, tc_bist_opcg_go_ovrd ,0b1, read_modify_write);
//
//     }//end system_vs_tester





//scom register that will get set at end
//Not going reset here, since dependend on above
//Add code just in case

//put_ptr_field(gcr_addr,rx_a_lane_fail_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_a_lane_fail_16_23 ,0b00000000         , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_fail_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_fail_16_23 ,0b00000000         , fast_write);

//put_ptr_field(gcr_addr,rx_a_lane_done_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_a_lane_done_16_23 ,0b00000000         , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_done_0_15  ,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_b_lane_done_16_23 ,0b00000000         , fast_write);

//put_ptr_field(gcr_addr,rx_section_fail      ,0b0000000000000000 , fast_write);

//put_ptr_field(gcr_addr,tc_opcg_iobist_go       ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_io_pb_iobist_reset   ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_a_bist_en            ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_b_bist_en            ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_bistclk_en           ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_a_prbs15_adj         ,0b0   , read_modify_write);
//put_ptr_field(gcr_addr,rx_b_prbs15_adj         ,0b0   , read_modify_write);

//put_ptr_field(gcr_addr,rx_check_en_alias,0b0000000000000000 , fast_write);
//put_ptr_field(gcr_addr,rx_ctle_gain_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_latchoff_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_eoff_check_en        ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_eoff_poff_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_ctle_peak_check_en   ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_lte_gain_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_lte_zero_check_en    ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_sigdet_check_en      ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_quad_phase_check_en  ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_dfe_h1_check_en      ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_dfe_check_en         ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_ddc_check_en         ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_ber_check_en         ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_link_layer_check_en  ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,system_dft_check_sel    ,0b0, read_modify_write);

//put_ptr_field(gcr_addr,rx_ber_check_en         ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,rx_link_layer_check_en  ,0b0, read_modify_write);
//put_ptr_field(gcr_addr,system_dft_check_sel    ,0b0, read_modify_write);
