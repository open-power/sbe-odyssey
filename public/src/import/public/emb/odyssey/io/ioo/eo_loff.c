/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_loff.c $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
// *! FILENAME    : eo_loff.c
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
// ------------|--------|-------------------------------------------------------
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22061500 |vbr     | Added returning of fail status
// vbr21101200 |vbr     | Added saving of data latch dac values in pcie mode
// vbr21092200 |vbr     | Use write only pulse for live edge mode bit
// mwh21060800 |mwh     | Had extra {} did seem to infer with code and move done
// mwh21060700 |mwh     | Moved loff before check to here out of eo_eoff.c
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// mwh21040100 |mwh     | Removed rx_eo_loff_servo_setup eoff has it own registers now
// vbr21012200 |vbr     | Use rx_eo_loff_servo_setup for setting servo config
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr20043000 |vbr     | Switched to common function for servo queues empty check
//-------------|P11 ----|-------------------------------------------------------
// mbs20073000 |mbs     | LAB - Remove override of rx_loff_timeout (now one value in io_init_and_reset.c)
// cws20011400 |cws     | Added Debug Logs
// mbs19072500 |mbs     | Moved live_edgeoff_mode out of loff_setting_ovr_enb umbrella
// mwh19051700 |mwh     | HW492097 changed inc/dec for change in step size
// mwh19040200 |mwh     | add a put of rx_loff_hyst_start to 5, to reset for loff
// mwh19020601 |mwh     | Combined abort and hyst
// mwh19020600 |mwh     | Moved bank sel to eo_main for loff
// mwh19011100 |mwh     | Add () to the || could could cause compare to wrong thing
// vbr18111400 |vbr     | Updated calls to set_fir.
// mwh18080700 |mwh     | Add in if statement so that we will not write the filter, inc/dec or timeout regs
// mwh18070500 |mwh     | Add in default filter and inc/dec to use with loff, because live edge offset
// vbr17111000 |vbr     | Updated function name to be more represntative of the step.
// mwh17110600 |mwh     | Add in if else for setting fence to 0 for A bank or B bank
// mwh17101800 |mwh     | Add in fix assigned at review of code
// mwh17092900 |mwh     | Add in fir write if queue are not empty
// vbr17092800 |vbr     | Abort no longer returned by run_servo_ops.
// mwh17090900 |mwh     | Add all latch offset servo ops into array
//-------------|--------|-------------------------------------------------------
// vbr17090500 |vbr     | Updated servo op constants.
// vbr17081400 |vbr     | Removed edge bank as a separately calibrated thing.
//-------------|--------|-------------------------------------------------------
// vbr17062901 |vbr     | Switched from PathOffsetToInt to TwosCompToInt.
// vbr17062900 |vbr     | Removed thread input from various functions.
// vbr17052300 |vbr     | Switched to new servo ops and using globals for them.
// vbr17052200 |vbr     | Commented out initial loff since no plans for it at this time.
// vbr17051800 |vbr     | Remove IOF code.
// vbr17042500 |vbr     | Added IOO thread support
// vbr17041800 |vbr     | Switched to using the servo op queue and disabled live-data edge offset.
// vbr17022200 |vbr     | Path offset is a mem_reg now.
// vbr16072900 |vbr     | Optimizations for code size
// vbr16062700 |vbr     | Switch to gcr_addr functions
// vbr16052400 |vbr     | Added set_debug_state
// vbr16031800 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_loff.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"


// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.

static uint16_t servo_ops_loff_a[36] = { c_loff_ad_n000,  c_loff_ad_n001, c_loff_ad_n010, c_loff_ad_n011, c_loff_ad_n100, c_loff_ad_n101, c_loff_ad_n110, c_loff_ad_n111,
                                         c_loff_ad_e000,  c_loff_ad_e001, c_loff_ad_e010, c_loff_ad_e011, c_loff_ad_e100, c_loff_ad_e101, c_loff_ad_e110, c_loff_ad_e111,
                                         c_loff_ad_s000,  c_loff_ad_s001, c_loff_ad_s010, c_loff_ad_s011, c_loff_ad_s100, c_loff_ad_s101, c_loff_ad_s110, c_loff_ad_s111,
                                         c_loff_ad_w000,  c_loff_ad_w001, c_loff_ad_w010, c_loff_ad_w011, c_loff_ad_w100, c_loff_ad_w101, c_loff_ad_w110, c_loff_ad_w111,
                                         c_loff_ae_n000, c_loff_ae_e000, c_loff_ae_s000, c_loff_ae_w000
                                       };


static uint16_t servo_ops_loff_b[36] = { c_loff_bd_n000,  c_loff_bd_n001, c_loff_bd_n010, c_loff_bd_n011, c_loff_bd_n100, c_loff_bd_n101, c_loff_bd_n110, c_loff_bd_n111,
                                         c_loff_bd_e000,  c_loff_bd_e001, c_loff_bd_e010, c_loff_bd_e011, c_loff_bd_e100, c_loff_bd_e101, c_loff_bd_e110, c_loff_bd_e111,
                                         c_loff_bd_s000,  c_loff_bd_s001, c_loff_bd_s010, c_loff_bd_s011, c_loff_bd_s100, c_loff_bd_s101, c_loff_bd_s110, c_loff_bd_s111,
                                         c_loff_bd_w000,  c_loff_bd_w001, c_loff_bd_w010, c_loff_bd_w011, c_loff_bd_w100, c_loff_bd_w101, c_loff_bd_w110, c_loff_bd_w111,
                                         c_loff_be_n000, c_loff_be_e000, c_loff_be_s000, c_loff_be_w000
                                       };






static inline int loff_check(t_gcr_addr* gcr_addr, int lane, uint32_t l_dac_addr, uint32_t end_address,
                             int check_latchoff_min, int check_latchoff_max)
{
    int status = pass_code;
    int loff_before;

    for (; l_dac_addr <= end_address; ++l_dac_addr)
    {
        //begin1
        loff_before = LatchDacToInt(get_ptr(gcr_addr, l_dac_addr, rx_ad_latch_dac_n000_startbit, rx_ad_latch_dac_n000_endbit)) ;

        if ((loff_before >  check_latchoff_max) || (loff_before < check_latchoff_min))
        {
            //begin3
            status = error_code;
            mem_pl_field_put(rx_latch_offset_fail, lane, 0b1);//ppe pl
            set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
            ADD_LOG(DEBUG_RX_EOFF_PRE_LOFF_LIMIT, gcr_addr, loff_before);
        }//end3
    }//end1

    return status;
}


// Latch Offset (fenced)
int eo_loff_fenced(t_gcr_addr* gcr_addr, t_bank bank)
{
    //start eo_loff
    set_debug_state(0x4000); // DEBUG

    // int abort_status, abort_status_d, abort_status_e;
    // Servo op based on bank
    unsigned int num_servo_ops;
    uint16_t* servo_ops;
    int lane = get_gcr_addr_lane(gcr_addr);

    // setting back to dccal latch offsetmode
    //rx_eo_loff_servo_setup(gcr_addr, SERVO_SETUP_LOFF);

    //eoff and loff have own gcr registers and this gcr selcet mux in logic
    //0 = loff gcr registers  1= eoff gcr registers
    put_ptr_field(gcr_addr, rx_loff_livedge_mode_clr, 0b1, fast_write);

    //setting fence for latch offset(data and edge done by same commmand) and alt and main
    // PSL bank_a
    if (bank == bank_a )
    {
        //bank A is alt B is main
        put_ptr_field(gcr_addr, rx_a_fence_en , 0b1, read_modify_write);
        servo_ops = servo_ops_loff_a;
    }//bank A is alt B is main
    else
    {
        //bank B is alt A is main
        put_ptr_field(gcr_addr, rx_b_fence_en , 0b1, read_modify_write);
        servo_ops = servo_ops_loff_b;
    }//bank B is alt A is main


    set_debug_state(0x4001);// DEBUG
    num_servo_ops = 36;

    //Run all the servo latch offset ops
    int status = run_servo_ops_with_results_disabled(gcr_addr, c_servo_queue_general, num_servo_ops, servo_ops);

    //Setting fence back to default value after servo ops
    if (bank == bank_a )
    {
        put_ptr_field(gcr_addr, rx_a_fence_en , 0b0, read_modify_write);
    }
    else
    {
        put_ptr_field(gcr_addr, rx_b_fence_en , 0b0, read_modify_write);
    }

    // In PCIe mode, save the data latch dac values
    int pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if (pcie_mode)
    {
        set_debug_state(0x4003); // DEBUG - Save Data Latch DAC values
        save_rx_data_latch_dac_values(gcr_addr, bank);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    //Begin rxbist checking of latch offset values. Depedning on bank what address get chosen
    //edge ae=20-22 be=23-26 ad=27-58 bd=59-90
    int rx_latchoff_check_en_int  = get_ptr(gcr_addr, rx_latchoff_check_en_addr, rx_latchoff_check_en_startbit,
                                            rx_latchoff_check_en_endbit);//pg
    uint32_t l_dac_addr_e;
    uint32_t end_address_e;
    uint32_t l_dac_addr_d;
    uint32_t end_address_d;

    if(rx_latchoff_check_en_int)
    {
        //begin0
        int check_latchoff_min =  TwosCompToInt(mem_pg_field_get(rx_latchoff_min_check), rx_latchoff_min_check_width); //ppe pg
        int check_latchoff_max =  TwosCompToInt(mem_pg_field_get(rx_latchoff_max_check), rx_latchoff_max_check_width); //ppe pg

        // PSL check_en_bank_a
        if (bank == bank_a) //begin
        {
            l_dac_addr_e =  rx_ae_latch_dac_n_addr;
            end_address_e = rx_ae_latch_dac_w_addr;
            l_dac_addr_d =  rx_ad_latch_dac_n000_addr;
            end_address_d = rx_ad_latch_dac_w000_addr;
            status |= loff_check(gcr_addr, lane, l_dac_addr_e, end_address_e, check_latchoff_min, check_latchoff_max);
            status |= loff_check(gcr_addr, lane, l_dac_addr_d, end_address_d, check_latchoff_min, check_latchoff_max);
        }
        else
        {
            l_dac_addr_e =  rx_be_latch_dac_n_addr;
            end_address_e = rx_be_latch_dac_w_addr;
            l_dac_addr_d =  rx_bd_latch_dac_n000_addr;
            end_address_d = rx_bd_latch_dac_w000_addr;
            status |= loff_check(gcr_addr, lane, l_dac_addr_e, end_address_e, check_latchoff_min, check_latchoff_max);
            status |= loff_check(gcr_addr, lane, l_dac_addr_d, end_address_d, check_latchoff_min, check_latchoff_max);
        }//end
    }//end0

    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_latch_offset_done, lane, 0b1);   //ppe pl
    }
    else
    {
        mem_pl_field_put(rx_b_latch_offset_done, lane, 0b1);
    }

    set_debug_state(0x4002); // DEBUG
    return status;
}//end eo_loff_fenced
