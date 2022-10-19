/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_vga_pathoffset.c $ */
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
// *! FILENAME    : eo_vga_pathoffset.c
// *! TITLE       :
// *! DESCRIPTION : Take path offset gotten by vga and update dac's
// *!
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
// mwh22070622 |mwh     | fixed issue 284226 done bit not being set.
// mwh22060700 |mwh     | fixed issue 281498 now A020 to A022
// vbr22012801 |vbr     | Use common functions for DAC accelerator
// mwh21101900 |mwh     | Updated code to keep the path offset value found poff_avg
// mwh21008160 |mwh     | Removed epoff check and moved to own func - called in eo_main.c
// mwh21060400 |mwh     | add in hysteresis
// mwh21060300 |mwh     | add in way todo Gen 3 in recal capture the N000 DC value
// mwh21051900 |mwh     | Update to have delta path offset
// mwh21050300 |mwh     | Removing DAC Accelerator errors check since we want speed.(see PR for more detials)
// mwh21041600 |mwh     | Initial Rev A0yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "servo_ops.h"
#include "ioo_common.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"
#include "eo_vga_pathoffset.h"

//eo_vga_pathoffset(gcr_addr, bank_a, &saved_Amax, recal, first_loop_iteration);
int eo_vga_pathoffset(t_gcr_addr* gcr_addr, t_bank bank, int saved_Amax_poff , bool recal, bool first_recal,
                      bool pcie_gen1_cal, bool pcie_gen2_cal, bool pcie_gen3_cal)
{
    //start eo_vga_pathoffset.c
    set_debug_state(0xA020); // DEBUG - APPLY PATH OFFSET

    int Amax_poff;
    int  data_before_n, data_n_dac;
    int lane = get_gcr_addr_lane(gcr_addr);

//-------------------------------------------------------------------------------
    // PSL bank_a
    if (bank == bank_a )
    {
        //bank A is alt B is main
        mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//ppe pl
        data_n_dac = (get_ptr(gcr_addr, rx_ad_latch_dac_n000_addr, rx_ad_latch_dac_n000_startbit,
                              rx_ad_latch_dac_n000_endbit));//pl

        // PSL bank_a_gen1to3
        if (pcie_gen1_cal || pcie_gen2_cal || (pcie_gen3_cal && !recal))//init for Gen1,2,3 and only Gen1 and Gen2 in recal
        {
            mem_pl_field_put(rx_a_before_loff_n000, lane, (data_n_dac));   //ppe pl
        }

    }//bank A is alt B is main
    else
    {
        //bank B is alt A is main
        mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//ppe pl
        data_n_dac = (get_ptr(gcr_addr, rx_bd_latch_dac_n000_addr, rx_bd_latch_dac_n000_startbit,
                              rx_bd_latch_dac_n000_endbit));//pl

        // PSL bank_b_gen1to3
        if (pcie_gen1_cal || pcie_gen2_cal || (pcie_gen3_cal && !recal))//init for Gen1,2,3 and only Gen1 and Gen2 in recal
        {
            mem_pl_field_put(rx_b_before_loff_n000, lane, (data_n_dac));   //ppe pl
        }
    }//bank B is alt A is main

    //Gen3 will use value gotten in init always since we run DFE in Gen 3

    if (bank == bank_a )
    {
        data_before_n = LatchDacToInt(mem_pl_field_get(rx_a_before_loff_n000, lane)); //pl
    }
    else
    {
        data_before_n = LatchDacToInt(mem_pl_field_get(rx_b_before_loff_n000, lane));
    }//pl

    // poff = vga_after - data_n_fence get the delta value for path offset
    // does not included the fenced offset value.
    Amax_poff = saved_Amax_poff - data_before_n;


//------------------------------------------------------------------------------------
    //this code is updating the Data Dac's with path offset. Either bank A or Bank B Dacs

    bool restore = false;

    // PSL recal
    if (recal)
    {
        //begin if recal
        int ppe_eoff_edge_hysteresis_int =  mem_pg_field_get(ppe_eoff_edge_hysteresis);//ppe pg

        if ( (abs(Amax_poff)) <= ppe_eoff_edge_hysteresis_int)
        {
            restore = true;    // Restore setting did not change enough
        }
    }//end if recal

    //for getting rel path offset need add previous value or if enough runs go 0
    if (restore)
    {
        set_debug_state(0xA021);
    }
    else
    {
        eo_update_poff_avg( gcr_addr, Amax_poff, bank, lane);
    }

    // PSL restore
    if (!restore)  //update registers init and recal
    {
        // PSL restore_gen1to3
        if (((first_recal) && (pcie_gen3_cal)) || (pcie_gen1_cal) || (pcie_gen2_cal))
        {
            //first gen3 and first/recal for gen1 and gen2
            apply_rx_data_and_edge_dac_offset(gcr_addr, bank, Amax_poff);
        }
        else
        {
            //begin recal gen3
            apply_rx_edge_dac_offset(gcr_addr, bank, Amax_poff);
        }//end recal
    }//end update registers


    //TODO replace removed: Check loff data latches before is +-64
    //ADD_LOG(DEBUG_RX_EOFF_PRE_LOFF_LIMIT, gcr_addr, loff_before);


    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_eoff_done, lane, 0b1);   //ppe pl
    }
    else
    {
        mem_pl_field_put(rx_b_eoff_done, lane, 0b1);
    }

    //------------------------------------------------------------------------------------

    set_debug_state(0xA022);
    return pass_code;
}//end eo_vga_pathoffset.c
