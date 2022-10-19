/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_eoff.c $       */
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
// *! FILENAME    : eo_eoff.c
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
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22071100 |vbr     | Moved eloff_alias_live_data_vote write from eo_main to here
// vbr22012801 |vbr     | Use common functions for DAC accelerator
// vbr22012800 |vbr     | Minor code clean up and code size reduction
// mwh21101900 |mwh     | Updated code to keep the path offset value found poff_avg
// vbr21092200 |vbr     | Use write only pulse for live edge mode bit
// mwh21008230 |mwh     | per Mike S removed the Check for DAC Accelerator errors code
// mwh21008160 |mwh     | Removed epoff check and moved to own func - called in eo_main.c
// mwh21008100 |mwh     | Removed fail by bank (one bank fails all bank fails)
// mwh21007190 |mwh     | Moved functions eo_get_weight_ave and eo_round to io_lib since common across iot and ioo for me
// mwh21007140 |mwh     | Move brace in hyst code -- was not apply the correct sequence
// mwh21007090 |mwh     | Issue (243388-abort)Removed unused code and replaced vga_loop with first_recal -- vga_loop not need anymore
// mwh21006070 |mwh     | Moved before check (dccal check to loff for dacs)
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// vbr21052600 |vbr     | Moved reading of bist registers
// mwh21004220 |mwh     | Clean up set_debug_state
// mwh21040100 |mwh     | Removed rx_eo_loff_servo_setup eoff has it own registers now
// vbr21012200 |vbr     | Use rx_eo_loff_servo_setup for setting servo config
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr20043000 |vbr     | Switched to common function for servo queues empty check
// vbr20050500 |vbr     | Use IntToTwosComp() and update reg name
// vbr20040900 |vbr     | Added wait on DAC accelerator done and checking of DAC accelerator error
// vbr20040600 |vbr     | Switched to using DAC accelerator for data latches in INIT
//-------------|P11     |-------------------------------------------------------
// mwh21042900 |mwh     | HW567512 -- removed clear of rx_latch_offset_fail in vga loop 1 and 2, if Abank fails does not matter what bbank does
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

//ioo_common.h:  PCIE_GEN1_CAL = 0,
//ioo_common.h:  PCIE_GEN2_CAL = 1,
//ioo_common.h:  PCIE_GEN3_CAL = 2,
//ioo_common.h:  PCIE_GEN4_CAL = 3,
//ioo_common.h:  PCIE_GEN5_CAL = 4,

//eo_main.c bool pcie_cal      = (cal_mode != AXO_CAL);
//eo_main.c bool pcie_genX_cal = (cal_mode == PCIE_GEN5_CAL) || (cal_mode == PCIE_GEN4_CAL) || (cal_mode == PCIE_GEN3_CAL);
//ioo_common.c cal_mode = rate; // Gen1=0...Gen5=4




#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_eoff.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"


// Assumption Checking
PK_STATIC_ASSERT(rx_ae_latch_dac_e_addr == rx_ae_latch_dac_n_addr + 1);
PK_STATIC_ASSERT(rx_ae_latch_dac_s_addr == rx_ae_latch_dac_n_addr + 2);
PK_STATIC_ASSERT(rx_ae_latch_dac_w_addr == rx_ae_latch_dac_n_addr + 3);
PK_STATIC_ASSERT(rx_be_latch_dac_e_addr == rx_be_latch_dac_n_addr + 1);
PK_STATIC_ASSERT(rx_be_latch_dac_s_addr == rx_be_latch_dac_n_addr + 2);
PK_STATIC_ASSERT(rx_be_latch_dac_w_addr == rx_be_latch_dac_n_addr + 3);


// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.
#define num_servo_ops 4
#define c_loff_e_n000_op 0
#define c_loff_e_e000_op 1
#define c_loff_e_s000_op 2
#define c_loff_e_w000_op 3
static uint16_t servo_ops_eoff_a[num_servo_ops] = { c_loff_ae_n000, c_loff_ae_e000, c_loff_ae_s000, c_loff_ae_w000};
static uint16_t servo_ops_eoff_b[num_servo_ops] = { c_loff_be_n000, c_loff_be_e000, c_loff_be_s000, c_loff_be_w000};


// Latch Offset (fenced)
int eo_eoff(t_gcr_addr* gcr_addr,  bool recal, bool first_recal, t_bank bank)
{
    //start eo_eoff
    set_debug_state(0xA000); // DEBUG

    // Servo op based on bank
    uint16_t* servo_ops;

    int lane = get_gcr_addr_lane(gcr_addr);

    int edge_n_dac, edge_e_dac, edge_s_dac, edge_w_dac;

    int edge_before_n, edge_before_e, edge_before_s, edge_before_w;
    int edge_after_n, edge_after_e, edge_after_s, edge_after_w;

    // int eoff_fail = 0;
    int delta_fail = 0;

    //int rx_eoff_fail_a = 0;
    //int rx_eoff_poff_fail_a = 0;

    int status;

    //eoff and loff have own gcr registers and this gcr selcet mux in logic
    //0 = loff gcr registers  1= eoff gcr registers
    //put_ptr_field(gcr_addr, rx_loff_livedge_mode_set, 0b1, fast_write);
    put_ptr_field(gcr_addr, rx_eloff_alias_live_data_vote, 0b110, fast_write); //enable transition votes

    int dac_start_addr;

    // PSL bank_a
    if (bank == bank_a )
    {
        //bank A is alt B is main
        mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//ppe pl
        servo_ops = servo_ops_eoff_a;
        dac_start_addr = rx_ae_latch_dac_n_addr;
    }//bank A is alt B is main
    else
    {
        //bank B is alt A is main
        mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//ppe pl
        servo_ops = servo_ops_eoff_b;
        dac_start_addr = rx_be_latch_dac_n_addr;
    }//bank B is alt A is main

    // Read initial Edge DAC values
    int edge_dacs[4];
    int dac_addr = dac_start_addr;
    int i;

    for (i = 0; i < 4; i++, dac_addr++)
    {
        edge_dacs[i] = get_ptr(gcr_addr, dac_addr, rx_ae_latch_dac_n_startbit, rx_ae_latch_dac_n_endbit);
    }

    edge_n_dac = edge_dacs[0];
    edge_e_dac = edge_dacs[1];
    edge_s_dac = edge_dacs[2];
    edge_w_dac = edge_dacs[3];

    edge_before_n = LatchDacToInt(edge_n_dac);//pl
    edge_before_e = LatchDacToInt(edge_e_dac);//pl
    edge_before_s = LatchDacToInt(edge_s_dac);//pl
    edge_before_w = LatchDacToInt(edge_w_dac);//pl

    set_debug_state(0xA002);//Start servo ops

    //Run all the servo latch offset ops and returns 2 comps
    int32_t servo_results[num_servo_ops];
    status = run_servo_ops_and_get_results(gcr_addr, c_servo_queue_general, num_servo_ops, servo_ops, servo_results);
    status |= check_rx_abort(gcr_addr);

    set_debug_state(0xA003);//End servo ops

    edge_after_n = servo_results[c_loff_e_n000_op];
    edge_after_e = servo_results[c_loff_e_e000_op];
    edge_after_s = servo_results[c_loff_e_s000_op];
    edge_after_w = servo_results[c_loff_e_w000_op];

    //eoff_fence + poff = edge_after
    //poff = edge_after - eoff_fence
    //Going do averge since this is course poff
    int poff_n =  (edge_after_n - edge_before_n );
    int poff_e =  (edge_after_e - edge_before_e );
    int poff_s =  (edge_after_s - edge_before_s );
    int poff_w =  (edge_after_w - edge_before_w );
    //set_debug_state(0xA004);//asigned value to poff_nesw

    //shift by 2 is divide by 4
    int poff_avg = ( poff_n +  poff_e + poff_s + poff_w);

    // get better rounded value of (i_val / 4)
    int poff_avg_int = eo_round(poff_avg);
    //set_debug_state(0xA005);//rounded averge done

    int poff_n_delta = poff_n - poff_avg_int;
    int poff_e_delta = poff_e - poff_avg_int;
    int poff_s_delta = poff_s - poff_avg_int;
    int poff_w_delta = poff_w - poff_avg_int;
    //set_debug_state(0xA006);//assign delta values


    //------------------------------------------------------------------------------------
    //this code is updating the Data Dac's with path offset. Either bank A or Bank B Dacs
    //no copy of a to b allowed
    //Do not updated edge latch since poff is already in it
    // PSL apply_dac_offset_init
    if ((!recal) && (!status))
    {
        //bank
        set_debug_state(0xA007);//starting !recal setting of dac's

        apply_rx_data_dac_offset(gcr_addr, bank, poff_avg_int);

        //set_debug_state(0xA008);//!recal end setting dac's
    }//end bank

    //------------------------------------------------------------------------------------
    //This code will restore the value of the edge latches if the change is below what hysteresis value is given
    //default is set to 3
    //example:  before_value = 60, run servo-op after_value = 58 and the servo-op will update the edge dacs.
    //          we will overwrite the value back to before_value = 60
    //The range can be changed with ppe reg ppe_eoff_edge_hysteresis
    //using the delta between the before and after value of edge offset Dac
    //Only for recal

    int ppe_eoff_edge_hysteresis_int =  mem_pg_field_get(ppe_eoff_edge_hysteresis);//ppe pg
    bool restore_n = false;
    bool restore_e = false;
    bool restore_s = false;
    bool restore_w = false;

    //setting to edge_after_n because for first recal we will either restore or just set result of servo op.
    int weight_avr_n = edge_after_n;
    int weight_avr_e = edge_after_e;
    int weight_avr_s = edge_after_s;
    int weight_avr_w = edge_after_w;  //we do not have hysteresis in first recal

    // PSL recal
    if (recal || first_recal)
    {
        //if recal begin
        // PSL restore
        if (status)
        {
            restore_n = true;    // Restore  settings on an abort or servo error (status != 0)
            restore_e = true;
            restore_s = true;
            restore_w = true;
        }
        // PSL not_first_recal
        else if (!first_recal)
        {
            //if not abort
            set_debug_state(0xA009);//start of recal setting of dac's or restore
            weight_avr_n = eo_get_weight_ave(edge_after_n, edge_before_n); //getting weighted round average
            weight_avr_e = eo_get_weight_ave(edge_after_e, edge_before_e); //getting weighted round average
            weight_avr_s = eo_get_weight_ave(edge_after_s, edge_before_s); //getting weighted round average
            weight_avr_w = eo_get_weight_ave(edge_after_w, edge_before_w); //getting weighted round average

            poff_n = weight_avr_n - edge_before_n ;//assign poff_n value
            poff_e = weight_avr_e - edge_before_e ;//assign poff_e value
            poff_s = weight_avr_s - edge_before_s ;//assign poff_s value
            poff_w = weight_avr_w - edge_before_w ;//assign poff_w value

            //Do not need a if bank A or B because up above selection of what servo to run apply to this
            if ( (abs(poff_n)) <= ppe_eoff_edge_hysteresis_int)
            {
                restore_n = true;    // Restore setting did not change enough
            }

            if ( (abs(poff_e)) <= ppe_eoff_edge_hysteresis_int)
            {
                restore_e = true;    // Restore setting did not change enough
            }

            if ( (abs(poff_s)) <= ppe_eoff_edge_hysteresis_int)
            {
                restore_s = true;    // Restore setting did not change enough
            }

            if ( (abs(poff_w)) <= ppe_eoff_edge_hysteresis_int)
            {
                restore_w = true;    // Restore setting did not change enough
            }
        }//if not abort



        if (bank == bank_a )
        {
            //bank A is alt B is main
            if (restore_n)
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_n, edge_n_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_n, IntToLatchDac(weight_avr_n), fast_write);   //pl if not restore
            }

            if (restore_e)
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_e, edge_e_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_e, IntToLatchDac(weight_avr_e), fast_write);   //pl if not restore
            }

            if (restore_s)
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_s, edge_s_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_s, IntToLatchDac(weight_avr_s), fast_write);   //pl if not restore
            }

            if (restore_w)
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_w, edge_w_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_ae_latch_dac_w, IntToLatchDac(weight_avr_w), fast_write);   //pl if not restore
            }
        }//bank A is alt B is main
        else
        {
            //bank B is alt A is main
            if (restore_n)
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_n, edge_n_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_n, IntToLatchDac(weight_avr_n), fast_write);   //pl if not restore
            }

            if (restore_e)
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_e, edge_e_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_e, IntToLatchDac(weight_avr_e), fast_write);   //pl if not restore
            }

            if (restore_s)
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_s, edge_s_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_s, IntToLatchDac(weight_avr_s), fast_write);   //pl if not restore
            }

            if (restore_w)
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_w, edge_w_dac, fast_write);   //pl if restore
            }
            else
            {
                put_ptr_field(gcr_addr, rx_be_latch_dac_w, IntToLatchDac(weight_avr_w), fast_write);   //pl if not restore
            }
        }//bank B is alt A is main

        set_debug_state(0xA00A);//end of recal setting of dac's or restore
    }//end if recal

    //for getting rel path offset need add previous value or if enough runs go 0
    if (restore_n || restore_e || restore_s || restore_w)
    {
        set_debug_state(0xA016);
    }
    else
    {
        eo_update_poff_avg( gcr_addr, poff_avg_int, bank, lane);
    }


    //------------------------------------------------------------------------------------

    //check for poff delta should be close to 0
    int rx_eoff_poff_check_en_int =  get_ptr(gcr_addr, rx_eoff_poff_check_en_addr, rx_eoff_poff_check_en_startbit,
                                     rx_eoff_poff_check_en_endbit);//pg

    if ((rx_eoff_poff_check_en_int) && (!status))
    {
        //begin1
        int check_poff_min           =  TwosCompToInt(mem_pg_field_get(rx_epoff_min_check), rx_epoff_min_check_width); //ppe pg
        int check_poff_max           =  TwosCompToInt(mem_pg_field_get(rx_epoff_max_check), rx_epoff_max_check_width); //ppe pg

        if ((poff_n_delta < check_poff_min) || (poff_n_delta > check_poff_max))
        {
            delta_fail = 1;
        }

        if ((poff_e_delta < check_poff_min) || (poff_e_delta > check_poff_max))
        {
            delta_fail = 1;
        }

        if ((poff_s_delta < check_poff_min) || (poff_s_delta > check_poff_max))
        {
            delta_fail = 1;
        }

        if ((poff_w_delta < check_poff_min) || (poff_w_delta > check_poff_max))
        {
            delta_fail = 1;
        }
    }//end1


    //------------------------------------------------------------------------------------

    if (delta_fail == 1)
    {
        mem_pl_field_put(rx_eoff_poff_fail, lane, delta_fail);
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_EOFF_POFF_FAIL, gcr_addr, 0x0);
    }//ppe pl

    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_eoff_done, lane, 0b1);   //ppe pl
    }
    else
    {
        mem_pl_field_put(rx_b_eoff_done, lane, 0b1);
    }

    //for bist if there is a servo error this get set -- help with debug
    // PSL set_fail
    if (status & rc_warning )
    {
        mem_pl_field_put(rx_eoff_fail, lane, 0b1);   //ppe pl
    }

    set_debug_state(0xA00B);//end of eof_eoff
    return status;
}//end eo_eoff
