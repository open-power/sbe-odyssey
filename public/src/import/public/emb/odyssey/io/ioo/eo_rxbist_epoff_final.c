/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_rxbist_epoff_final.c $ */
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
// *! FILENAME    : eo_rxbist_epoff_final.c
// *! TITLE       :
// *! DESCRIPTION : Rxbist check to check max and min value of edge dac after dccal and path offset was add
// *!             : rx_eoff_min_check and rx_eoff_max_check
// *!             : This funciton is used by all 3 different poff offset ways. eoff.c, eo_vga_pathoffset.c and eo_eoff_1_lat.c
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
// mwh21008160 |mwh     | initial -- replaced code that was in eoff.c and bring out for all 3 types
//------------------------------------------------------------------------------


#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_eoff.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"

void eo_rxbist_epoff_final(t_gcr_addr* gcr_addr, t_bank bank)
{
    //start eo_rxbist_epoff_final
    set_debug_state(0xA018);//start

    int lane = get_gcr_addr_lane(gcr_addr);
    int edge_after_n, edge_after_e, edge_after_s, edge_after_w;

    //not a stick since fail is only last time it was run (vga loop)
    int rx_eoff_fail_int = 0;//pl


    // PSL bank_a
    if (bank == bank_a )  //abank values
    {
        edge_after_n = LatchDacToInt(get_ptr(gcr_addr, rx_ae_latch_dac_n_addr, rx_ae_latch_dac_n_startbit,
                                             rx_ae_latch_dac_n_endbit));//pl
        edge_after_e = LatchDacToInt(get_ptr(gcr_addr, rx_ae_latch_dac_e_addr, rx_ae_latch_dac_e_startbit,
                                             rx_ae_latch_dac_e_endbit));//pl
        edge_after_s = LatchDacToInt(get_ptr(gcr_addr, rx_ae_latch_dac_s_addr, rx_ae_latch_dac_s_startbit,
                                             rx_ae_latch_dac_s_endbit));//pl
        edge_after_w = LatchDacToInt(get_ptr(gcr_addr, rx_ae_latch_dac_w_addr, rx_ae_latch_dac_w_startbit,
                                             rx_ae_latch_dac_w_endbit));//pl
    }
    else    //bbank values
    {
        edge_after_n = LatchDacToInt(get_ptr(gcr_addr, rx_be_latch_dac_n_addr, rx_be_latch_dac_n_startbit,
                                             rx_be_latch_dac_n_endbit));//pl
        edge_after_e = LatchDacToInt(get_ptr(gcr_addr, rx_be_latch_dac_e_addr, rx_be_latch_dac_e_startbit,
                                             rx_be_latch_dac_e_endbit));//pl
        edge_after_s = LatchDacToInt(get_ptr(gcr_addr, rx_be_latch_dac_s_addr, rx_be_latch_dac_s_startbit,
                                             rx_be_latch_dac_s_endbit));//pl
        edge_after_w = LatchDacToInt(get_ptr(gcr_addr, rx_be_latch_dac_w_addr, rx_be_latch_dac_w_startbit,
                                             rx_be_latch_dac_w_endbit));//pl
    }//end else


    int check_eoff_min           =  TwosCompToInt(mem_pg_field_get(rx_eoff_min_check), rx_eoff_min_check_width); //ppe pg
    int check_eoff_max           =  TwosCompToInt(mem_pg_field_get(rx_eoff_max_check), rx_eoff_max_check_width); //ppe pg

    if ((edge_after_n < check_eoff_min)  || (edge_after_n > check_eoff_max))
    {
        rx_eoff_fail_int = 1;
    }

    if ((edge_after_e < check_eoff_min)  || (edge_after_e > check_eoff_max))
    {
        rx_eoff_fail_int = 1;
    }

    if ((edge_after_s < check_eoff_min)  || (edge_after_s > check_eoff_max))
    {
        rx_eoff_fail_int = 1;
    }

    if ((edge_after_w < check_eoff_min)  || (edge_after_w > check_eoff_max))
    {
        rx_eoff_fail_int = 1;
    }

    if (rx_eoff_fail_int == 1) //being if
    {
        mem_pl_field_put(rx_eoff_fail, lane, rx_eoff_fail_int);
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_EOFF_EOFF_FAIL, gcr_addr, 0x0);
    }//end if (this is pl)

    set_debug_state(0xA019);//end
}//end eo_rxbist_epoff_final
