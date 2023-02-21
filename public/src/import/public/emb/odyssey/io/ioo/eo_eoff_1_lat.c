/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_eoff_1_lat.c $ */
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
// *! FILENAME    : eo_eoff_1_lat.c
// *! TITLE       :
// *! DESCRIPTION : Run Latch Offset - with Integrator/CTLE Fenced -- data and edge
// *!             : only run during DC calibration
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
// vbr23010400 |vbr     | Issue 296947: Adjusted latch_dac accesses for different addresses on Odyssey vs P11/ZMetis
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22012801 |vbr     | Use common functions for DAC accelerator
// mwh21101900 |mwh     | Updated code to keep the path offset value found poff_avg
// vbr21092200 |vbr     | Use write only pulse for live edge mode bit
// mwh21008230 |mwh     | per Mike S removed the Check for DAC Accelerator errors code
// mwh21081900 |mwh     | vga loop input not need clean up
// mwh21008160 |mwh     | Removed epoff check and moved to own func - called in eo_main.c
// mwh21060800 |mwh     | Removed dccal check
// vbr21060201 |vbr     | Moved reading of bist registers
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// mwh21040100 |mwh     | Removed rx_eo_loff_servo_setup eoff has it own registers now
// vbr21012200 |vbr     | Use rx_eo_loff_servo_setup for setting servo config
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr20043000 |vbr     | Switched to common function for servo queues empty check
// vbr20050500 |vbr     | Use IntToTwosComp() and update reg name
// vbr20040900 |vbr     | Added wait on DAC accelerator done and checking of DAC accelerator error
// vbr20040600 |vbr     | Switched to using DAC accelerator for data latches in INIT
//-------------|P11     |-------------------------------------------------------
// mwh20111100 |mwh     | HW550299 -- changes make eoff more statibly
// mwh20101600 |mwh     | Change way clear is happen so we retain fails better.
// mwh20101300 |mwh     | After talking with Chris and Jim logger is back in this file, also scom set in vclq
// mwh20101200 |mwh     | Put loff fail clear into if to gate form clearing
// mwh20070700 |mwh     | Change DEBUG_RX_EOFF_EOFF_FAIL to DEBUG_RX_EOFF_POFF_FAIL for CQ528211 delta is for poff set
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// cws20011400 |cws     | Added Debug Logs
// mwh20116000 |mwh     | HW512908 increased first stage filter to 3 and reduced first stage inc/dec to 2
// mwh19112000 |mwh     | Fix issue were we were going into a init only check, in recal hw512120
// bja19081900 |bja     | Set loff_fail var when fail conditions are met in !recal section
// mbs19072500 |mbs     | Added loff_setting_ovr_enb
// mwh19051700 |mwh     | HW492097 Change inc/dec for loff and eoff for change in step 4.6
// vbr19051400 |vbr     | HW491892: Change VDAC from 9-bit SM to 8-bit twos_comp
// mwh19043000 |mwh     | add set_fir(fir_code_dft_error);
// mwh19012100 |mwh     | Add rx_bist fail flag
// mwh19032100 |mwh     | Add in vga_loop_count so that we do not do the latch offset check more than once
// mwh19022600 |mwh     | Fix signed mag issue adn bank sel
// mwh19022500 |mwh     | Chagned poff_avg to sm 2^5 and 1 bit signed
// mwh19020500 |mwh     | Change timeout to 5 issue was something else
// mwh19020500 |mwh     | Change timeout to 6 for eoff since it triggering 5 to 6, out 70 runs 13 see timeout
// mwh19002060 |vbr     | Combined recal abort with hysteresis, and reduce logic
// vbr19012200 |vbr     | Added recal abort and servo op error handling
// mwh19020500 |mwh     | Add in hysteresis for recal only, and edge latch only
// mwh19011100 |mwh     | Add () to the || could could cause compare to wrong thing
// mwh18112800 |mwh     | Updated code to add poff to dac's and add in checks
// vbr18121000 |vbr     | Updated debug state for consistency.
// vbr18120500 |vbr     | Increased timeout from 4 to 5.
// vbr18111400 |vbr     | Updated calls to set_fir.
// mwh18070500 |mwh     | Initial Rev A0yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_eoff_1_lat.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"
#include "eo_eoff.h"

// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.
#define num_servo_ops 1
#define c_loff_e_n000_op 0
static uint16_t servo_ops_eoff_a[num_servo_ops] = { c_loff_ae_n000};
static uint16_t servo_ops_eoff_b[num_servo_ops] = { c_loff_be_n000};


int eo_eoff_1_lat(t_gcr_addr* gcr_addr,  bool recal,  t_bank bank, bool vote_sel)
{
    //start eo_eoff
    set_debug_state(0xA00C); // DEBUG

    // Servo op based on bank
    uint16_t* servo_ops;

    int lane = get_gcr_addr_lane(gcr_addr);

    int edge_n_dac;

    int edge_before_n;
    int edge_after_n;

    //int eoff_fail = 0;
    //int rx_eoff_fail_a = 0;
    //int rx_eoff_poff_fail_a = 0;


    int status;
    int dac_addr;

    //set_debug_state(0xA001);
    {
        //run servo start

        //put_ptr_field(gcr_addr, rx_loff_livedge_mode_set, 0b1, fast_write);

        // transition0 and  edge = 0  just edge = 1
        //rx_eloff_alias_live_data_vote rx_loff_livedge_mode_set, rx_eoff_data_src, rx_eoff_vote_sel
        // PSL vote_sel
        if (vote_sel == 1)
        {
            put_ptr_field(gcr_addr, rx_eloff_alias_live_data_vote , 0b111, fast_write);
        }
        else
        {
            put_ptr_field(gcr_addr, rx_eloff_alias_live_data_vote , 0b110, fast_write);
        }


        // PSL bank_a
        if (bank == bank_a )
        {
            //bank A is alt B is main
            mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//ppe pl
            dac_addr = rx_ae_latch_dac_n_alias_addr;
            servo_ops = servo_ops_eoff_a;
        }//bank A is alt B is main
        else
        {
            //bank B is alt A is main
            mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//ppe pl
            dac_addr = rx_be_latch_dac_n_alias_addr;
            servo_ops = servo_ops_eoff_b;
        }//bank B is alt A is main

        // Issue 296947 Workaround
        int dac_addr_adjust = get_latch_dac_addr_adjust();
        dac_addr += dac_addr_adjust;

        edge_n_dac = get_ptr(gcr_addr, dac_addr, rx_ae_latch_dac_n_startbit, rx_ae_latch_dac_n_endbit);//pl
        edge_before_n = LatchDacToInt(edge_n_dac);//pl

        set_debug_state(0xA00E);// DEBUG - EOFF Run Servo Ops

        //Run all the servo latch offset ops and returns 2 comps
        int32_t servo_results[num_servo_ops];
        status = run_servo_ops_and_get_results(gcr_addr, c_servo_queue_general, num_servo_ops, servo_ops, servo_results);
        status |= check_rx_abort(gcr_addr);

        edge_after_n = servo_results[c_loff_e_n000_op];
        set_debug_state(0xA00F); // DEBUG
    }//run servo end

    int poff_n =  (edge_after_n - edge_before_n );

//for getting rel path offset need add previous value or if enough runs goes to 0
    //(gcr_addr, recal, bank_a);

    //for getting rel path offset need add previous value or if enough runs go 0
    eo_update_poff_avg( gcr_addr, poff_n, bank, lane);

    //------------------------------------------------------------------------------------
    //this code is updating the Data Dac's with path offset. Either bank A or Bank B Dacs
    //no copy of a to b allowed
    //Do not updated edge latch since poff is already in it
    // PSL recal
    if (!recal)
    {
        //bank
        set_debug_state(0xA010); // DEBUG - APPLY PATH OFFSET
        //This put the edge dac used find pathoffset back to before value
        //so the dac accel can be used write all edge latches
        put_ptr_fast(gcr_addr, dac_addr, rx_ae_latch_dac_n_endbit, edge_n_dac);

        apply_rx_data_and_edge_dac_offset(gcr_addr, bank, poff_n);


        //TODO replace removed: Check loff data latches before is +-64
        //ADD_LOG(DEBUG_RX_EOFF_PRE_LOFF_LIMIT, gcr_addr, loff_before);
    }//end bank



    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_eoff_done, lane, 0b1);   //ppe pl
    }
    else
    {
        mem_pl_field_put(rx_b_eoff_done, lane, 0b1);
    }

    set_debug_state(0xA014); // DEBUG

    //for bist if there is a servo error this get set -- help with debug
    // PSL set_fail
    if (status & rc_warning )
    {
        mem_pl_field_put(rx_eoff_fail, lane, 0b1);    //ppe pl
        set_debug_state(0xA015);
    }


    return status;
}//end eo_eoff
