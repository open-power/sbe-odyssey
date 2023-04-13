/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_vga_pathoffset.c $ */
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
// vbr23030600 |vbr     | Turned inputs into constants since this is now only run in PCIe Gen1/2 initial training (not recal, not Gen3).
// mwh23013000 |mwh     | Issue 298005 Poff_avg going to 0 and than a restore get done with wrong value.
// mwh23011800 |mwh     | Issue 297375 Change way we grab loff n000 value and add in poffset
// vbr23011200 |vbr     | Issue 295461: In Gen3, do not read dac value in recal (A/B) OR first_recal (B) since will have DFE applied from init.
// vbr23010400 |vbr     | Issue 296947: Adjusted latch_dac accesses for different addresses on Odyssey vs P11/ZMetis
// jjb22112200 |jjb     | Added pcie_bist_mode parameter
// mbs22082601 |mbs     | Updated with PSL comments
// mwh22070622 |mwh     | fixed issue 284226 done bit not being set.
// mwh22060700 |mwh     | fixed issue 281498 now A020 to A022
// vbr22012801 |vbr     | Use common functions for DAC accelerator
// mwh21101900 |mwh     | Updated code to keep the path offset value found poff_avg
// mwh21008160 |mwh     | Removed epoff check and moved to own func - called in eo_main.c
// mwh21060400 |mwh     | add in hysteresis
// mwh21060300 |mwh     | add in way to do Gen 3 in recal capture the N000 DC value
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
int eo_vga_pathoffset(t_gcr_addr* gcr_addr, t_bank bank, int saved_Amax_poff, bool pcie_bist_mode)
{
    //start eo_vga_pathoffset.c
    set_debug_state(0xA020); // DEBUG - APPLY PATH OFFSET

    // Former inputs that are now constants
    const bool recal = false;
    const bool first_recal = false;
    const bool pcie_gen1_cal = true;
    const bool pcie_gen2_cal = true;
    const bool pcie_gen3_cal = false;

    int Amax_poff;
    int data_before_n;
    int lane = get_gcr_addr_lane(gcr_addr);
    int dac_addr;
//-------------------------------------------------------------------------------
// Three different case that need to work for
// Init Gen 1, rate change to other Gen and stay there doing recal --> Gen 1 --> Rate change Gen 2 --> Init Gen2 than go to Recal
// Rate changes between Gen's --> Gen 3 --> Rate change Gen 2 --> Init Gen2 than go to Recal stay there
// Reset with Init Gen 1.

    // PSL bank_a
    if (bank == bank_a )
    {
        //bank A is alt B is main
        mem_pl_field_put(rx_a_eoff_done, lane, 0b0);//ppe pl
        dac_addr = rx_ad_latch_dac_n000_alias_addr;
    }//bank A is alt B is main
    else
    {
        //bank B is alt A is main
        mem_pl_field_put(rx_b_eoff_done, lane, 0b0);//ppe pl
        dac_addr = rx_bd_latch_dac_n000_alias_addr;
    }//bank B is alt A is main

    // Issue 296947 Workaround (Not really needed since this does not run on Odyssey)
    //int dac_addr_adjust = get_latch_dac_addr_adjust();
    //dac_addr += dac_addr_adjust;

    // PSL loff_before_gen1to3
    //Change back to capture dac value at all 3 Gen's has 295461 fix
    if (pcie_gen1_cal || pcie_gen2_cal || (pcie_gen3_cal && !recal && !first_recal) || (pcie_bist_mode && !recal
            && !first_recal))
    {
        //init for Gen1,2,3,pcie_bist_mode and only Gen1 and Gen2 in recal
        int data_n_dac = get_ptr(gcr_addr, dac_addr, rx_ad_latch_dac_n000_startbit, rx_ad_latch_dac_n000_endbit);//pl

        // PSL bank_a_gen1to3
        if (bank == bank_a )
        {
            mem_pl_field_put(rx_a_before_loff_n000, lane, data_n_dac); //ppe pl
        }
        else
        {
            mem_pl_field_put(rx_b_before_loff_n000, lane, data_n_dac); //ppe pl
        }
    }//end init

    //Gen3 will use value gotten in init always since we run DFE in Gen 3
    //latch offset + poff
    //Gen 1 and Gen 2 is fine
    //Gen 3 this register is only what was in gen 1 or gen 2 -- does not change for path offset delta
    if (bank == bank_a )
    {
        data_before_n = LatchDacToInt(mem_pl_field_get(rx_a_before_loff_n000, lane)); //pl
    }
    else
    {
        data_before_n = LatchDacToInt(mem_pl_field_get(rx_b_before_loff_n000, lane)); //pl
    }//pl


    // poff = vga_after - data_n_fence get the delta value for path offset
    // does not included the fenced offset value.
    Amax_poff = saved_Amax_poff - data_before_n;

    //Need this becuase of DFE is in Gen3
    //this take care change in mparm = poff but not change mparm for loff
    if (pcie_gen3_cal)
    {
        int new_before_loff = (IntToLatchDac(data_before_n + Amax_poff));

        if (bank == bank_a ) //if
        {
            mem_pl_field_put(rx_a_before_loff_n000, lane, new_before_loff); //ppe pl
        }
        else    //else
        {
            mem_pl_field_put(rx_b_before_loff_n000, lane, new_before_loff); //ppe pl
        }//end else
    }//end if Gen 3

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

    // PSL pathoffset_apply_rx_data_and_edge_dac_offset
    if ( ((!recal) && (!first_recal) && (pcie_gen1_cal || pcie_gen2_cal || pcie_bist_mode)) || // init
         ((!restore) && (pcie_gen1_cal || pcie_gen2_cal || (first_recal && pcie_gen3_cal
                         && !pcie_bist_mode))) ) // gen1,gen2 recals. 1st gen 3 recal in func mode
    {
        apply_rx_data_and_edge_dac_offset(gcr_addr, bank, Amax_poff);
    }
    else if (pcie_gen3_cal && !pcie_bist_mode) // gen3 subsequent recals
    {
        apply_rx_edge_dac_offset(gcr_addr, bank, Amax_poff);
    }

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
