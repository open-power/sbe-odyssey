/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_edge_amax.c $  */
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
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_edge_amax.c
// *! TITLE       :
// *! DESCRIPTION : Run amax to cal out poff and latch offset drift
// *!             :
// *!
// *! Owner NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr23040800 |vbr     | Added early exit on abort/servo error. Various fixes and code size savings.
// mwh23030800 |mwh     | Add in _hysteresis and bist stuff
// vbr23030700 |vbr     | Change to offset calc ((Ap+An)/2) from Amax cal ((Ap-An)/2)
// vbr23030600 |vbr     | Added debug states and call to setup servo config
// mwh23030300 |mwh     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"

#include "io_logger.h"
#include "ioo_common.h"
#include "eo_edge_amax.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

// Assumption Checking
PK_STATIC_ASSERT(rx_ae_latch_dac_e_addr == rx_ae_latch_dac_n_addr + 1);
PK_STATIC_ASSERT(rx_ae_latch_dac_s_addr == rx_ae_latch_dac_n_addr + 2);
PK_STATIC_ASSERT(rx_ae_latch_dac_w_addr == rx_ae_latch_dac_n_addr + 3);
PK_STATIC_ASSERT(rx_be_latch_dac_e_addr == rx_be_latch_dac_n_addr + 1);
PK_STATIC_ASSERT(rx_be_latch_dac_s_addr == rx_be_latch_dac_n_addr + 2);
PK_STATIC_ASSERT(rx_be_latch_dac_w_addr == rx_be_latch_dac_n_addr + 3);

// These will actually be used as biased Apxxxxx and Anxxxxx servos to find Amax directly
#define c_ap_xxx1x_ae_n000 amp_servo_op(0b00010, 0b00010, c_latch_ae_n000)
#define c_an_xxx0x_ae_n000 amp_servo_op(0b00010, 0b00000, c_latch_ae_n000)
#define c_ap_xxx1x_ae_e000 amp_servo_op(0b00010, 0b00010, c_latch_ae_e000)
#define c_an_xxx0x_ae_e000 amp_servo_op(0b00010, 0b00000, c_latch_ae_e000)
#define c_ap_xxx1x_ae_s000 amp_servo_op(0b00010, 0b00010, c_latch_ae_s000)
#define c_an_xxx0x_ae_s000 amp_servo_op(0b00010, 0b00000, c_latch_ae_s000)
#define c_ap_xxx1x_ae_w000 amp_servo_op(0b00010, 0b00010, c_latch_ae_w000)
#define c_an_xxx0x_ae_w000 amp_servo_op(0b00010, 0b00000, c_latch_ae_w000)

#define c_ap_xxx1x_be_n000 amp_servo_op(0b00010, 0b00010, c_latch_be_n000)
#define c_an_xxx0x_be_n000 amp_servo_op(0b00010, 0b00000, c_latch_be_n000)
#define c_ap_xxx1x_be_e000 amp_servo_op(0b00010, 0b00010, c_latch_be_e000)
#define c_an_xxx0x_be_e000 amp_servo_op(0b00010, 0b00000, c_latch_be_e000)
#define c_ap_xxx1x_be_s000 amp_servo_op(0b00010, 0b00010, c_latch_be_s000)
#define c_an_xxx0x_be_s000 amp_servo_op(0b00010, 0b00000, c_latch_be_s000)
#define c_ap_xxx1x_be_w000 amp_servo_op(0b00010, 0b00010, c_latch_be_w000)
#define c_an_xxx0x_be_w000 amp_servo_op(0b00010, 0b00000, c_latch_be_w000)


// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.
#define num_servo_ops 8
#define answer_size 4

// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.
static uint16_t servo_ops_edge_a [num_servo_ops] = { c_ap_xxx1x_ae_n000, c_an_xxx0x_ae_n000, c_ap_xxx1x_ae_e000, c_an_xxx0x_ae_e000,
                                                     c_ap_xxx1x_ae_s000, c_an_xxx0x_ae_s000, c_ap_xxx1x_ae_w000, c_an_xxx0x_ae_w000
                                                   };

static uint16_t servo_ops_edge_b [num_servo_ops] = { c_ap_xxx1x_be_n000, c_an_xxx0x_be_n000, c_ap_xxx1x_be_e000, c_an_xxx0x_be_e000,
                                                     c_ap_xxx1x_be_s000, c_an_xxx0x_be_s000, c_ap_xxx1x_be_w000, c_an_xxx0x_be_w000
                                                   };




int eo_edge_amax(t_gcr_addr* gcr_addr, t_bank bank)
{
    //start eo_edge_amax
    set_debug_state(0xA030); // DEBUG - EOFF AMAX Start

    // This only runs in recal with hysteresis enabled
    const bool hyst_en = true;

    // Set up Apxxxxx and Anxxxxx servos with biased filters to find Amax.
    // This has the advantage of being bank independent and more peaking independent.
    rx_eo_amp_servo_setup(gcr_addr, SERVO_SETUP_VGA);

    int lane;
    int i;
    int dac_addr;
    int dac_start_addr;
    uint16_t* servo_ops;
    int32_t servo_results[num_servo_ops];

    // PSL bank_a
    if (bank == bank_a)   //bank A is alt B is main
    {
        servo_ops = servo_ops_edge_a;
        dac_start_addr = rx_ae_latch_dac_n_alias_addr;
    }
    else     //bank B is alt A is main
    {
        servo_ops = servo_ops_edge_b;
        dac_start_addr = rx_be_latch_dac_n_alias_addr;
    }

    // Issue 296947 Workaround (Not really needed since this does not run on Odyssey)
    //int dac_addr_adjust = get_latch_dac_addr_adjust();
    //dac_start_addr += dac_addr_adjust;


    // Run Servo Ops
    int status = run_servo_ops_and_get_results(gcr_addr, c_servo_queue_amp_meas, num_servo_ops, servo_ops, servo_results);
    status |= check_rx_abort(gcr_addr);

    if (status)
    {
        goto FUNC_EXIT;
    }

    // Offset = (Ap + An) / 2
    int32_t answer[answer_size];
    answer[0] = ((servo_results[0] + servo_results[1]) >> 1);
    answer[1] = ((servo_results[2] + servo_results[3]) >> 1);
    answer[2] = ((servo_results[4] + servo_results[5]) >> 1);
    answer[3] = ((servo_results[6] + servo_results[7]) >> 1);

    // Hysteresis
    bool update_dac[answer_size];

    if (hyst_en)
    {
        //set_debug_state(0xA039); // DEBUG - Edge Amax Hysteresis
        int before_edge_dacs[answer_size];

        //get the previous value of what edge latch were set too.
        dac_addr = dac_start_addr;

        for (i = 0; i < answer_size; i++, dac_addr++)  //begin
        {
            before_edge_dacs[i] = LatchDacToInt(get_ptr(gcr_addr, dac_addr, rx_ae_latch_dac_n_startbit, rx_ae_latch_dac_n_endbit));
        }//end

        int ppe_eoff_edge_hysteresis_int = mem_pg_field_get(ppe_eoff_edge_hysteresis);//ppe pg

        for (i = 0; i < answer_size; i++)
        {
            answer[i] = eo_get_weight_ave(answer[i], before_edge_dacs[i]); //getting weighted round average
            int poff_delta = answer[i] - before_edge_dacs[i];
            update_dac[i] = ( abs(poff_delta) > ppe_eoff_edge_hysteresis_int );
        }
    }
    else     //!hyst_en
    {
        // Update DAC with result when hysteresis disabled
        //update_dac[0] = true;
        //update_dac[1] = true;
        //update_dac[2] = true;
        //update_dac[3] = true;
    }

    // For each edge DAC, Write the weighted average or leave at the original value
    dac_addr = dac_start_addr;

    for (i = 0; i < answer_size; i++, dac_addr++)
    {
        // Update the DAC if hysteresis is disabled or if hysteresis indicated an update is needed
        if (!hyst_en || update_dac[i])
        {
            int dac_val = IntToLatchDac(answer[i]);
            put_ptr_fast(gcr_addr, dac_addr, rx_ad_latch_dac_n000_endbit, dac_val);
        }
    }


FUNC_EXIT:
    lane = get_gcr_addr_lane(gcr_addr);

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

    set_debug_state(0xA03F); // DEBUG - EOFF AMAX End
    return status;
}//end eo_edge_amax
