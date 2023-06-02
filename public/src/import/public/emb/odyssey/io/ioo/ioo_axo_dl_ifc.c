/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_axo_dl_ifc.c $ */
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
// *! FILENAME    : ioo_axo_dl_ifc.c
// *! TITLE       :
// *! DESCRIPTION : Functions for handling the AXO DL Interface
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// jjb22121400 |jjb     | Issue 296218 : updated all set_debug_state values to 0x001? to uniquify them
// mbs22082601 |mbs     | Updated with PSL comments
// vbr21092200 |vbr     | Use wo pulse for psave_req_alt
// vbr21081800 |vbr     | Story 237887: Check for psave_req=1 and run_lane=0 to power off alt. Issue 244463: Sleep more often when checking DL signals.
// vbr21061000 |vbr     | Moved clearing of recal_abort_sticky to eo_wrappers
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21050700 |vbr     | HW563765: Removed check of rx_any_init_req_or_reset
// vbr21042100 |vbr     | Made the rx_any_init_req_or_reset check more efficient in dl_recal
// vbr21020800 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_axo_dl_ifc.h"
#include "io_logger.h"
#include "eo_wrappers.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


/**
 * @brief Service DL Initialize Request
 * @param[inout] io_gcr_addr        Target Information
 * @param[in   ] i_num_lanes     Total Number of Lanes
 * @retval void
 */
void dl_init_req(t_gcr_addr* io_gcr_addr, const uint32_t i_num_lanes)
{
    //set_debug_state(0x0010);
    uint32_t l_rx_psave_req_mask = (get_ptr_field(io_gcr_addr, rx_psave_req_dl_0_15_sts)  << 16) |
                                   (get_ptr_field(io_gcr_addr, rx_psave_req_dl_16_23_sts) << (16 - rx_psave_req_dl_16_23_sts_width));
    uint32_t l_run_lane_mask = (get_ptr_field(io_gcr_addr, rx_dl_phy_run_lane_0_15)  << 16) |
                               (get_ptr_field(io_gcr_addr, rx_dl_phy_run_lane_16_23) << (16 - rx_dl_phy_run_lane_16_23_width));
    uint32_t l_lane          = 0;

    for (; l_lane < i_num_lanes; ++l_lane)
    {
        set_gcr_addr_lane(io_gcr_addr, l_lane);

        uint32_t l_init_done = mem_pl_field_get(rx_init_done, l_lane);
        uint32_t l_dl_init_req = ((0x80000000 >> l_lane) & l_run_lane_mask);

        if (l_dl_init_req && !l_init_done)
        {
            // PSL_EN_NEXT
            io_sleep(get_gcr_addr_thread(io_gcr_addr));
            //mem_pg_field_put(rx_eo_phase_select_0_2, 0b111); // Run all phases (default setting for AXO)
            run_initial_training(io_gcr_addr, l_lane);  //sleeps at end
            put_ptr_field(io_gcr_addr, rx_phy_dl_init_done_set, 0b1, fast_write);
        }
        else if (!l_dl_init_req && l_init_done && !mem_pl_field_get(rx_cmd_init_done, l_lane))
        {
            // PSL put_init_done
            put_ptr_field(io_gcr_addr, rx_phy_dl_init_done_clr, 0b1, fast_write);
            mem_pl_bit_clr(rx_init_done, l_lane);
        }

        // Lane is unused (run_lane==0)
        //PSL unused_lane
        if (!l_dl_init_req)
        {
            set_recal_or_unused(l_lane);

            // Power down ALT bank if DL is requesting DL power save
            //PSL unused_lane_psave_req
            if ((0x80000000 >> l_lane) & l_rx_psave_req_mask)
            {
                put_ptr_field(io_gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
            }
        }

        if ((l_lane % 2) == 1)
        {
            io_sleep(get_gcr_addr_thread(io_gcr_addr));
        }
    } //for lane

    //set_debug_state(0x0011);
    return;
}//dl_init_req


/**
 * @brief Gets DL Recalibration Request Vector
 * @param[inout] io_gcr_addr     Target Information
 * @retval void
 */
uint32_t get_dl_recal_req_vec(t_gcr_addr* io_gcr_addr)
{
    return (get_ptr_field(io_gcr_addr, rx_dl_phy_recal_req_0_15)  << 16) |
           (get_ptr_field(io_gcr_addr, rx_dl_phy_recal_req_16_23) << (16 - rx_dl_phy_recal_req_16_23_width));
}


/**
 * @brief Calls Service DL Recalibration Request
 * @param[inout] io_gcr_addr     Target Information
 * @param[in   ] i_num_lanes     Total Number of Lanes
 * @param[in   ] i_recal_req_vec DL Recalibration Request Vector
 * @retval void
 */
void dl_recal_req(t_gcr_addr* io_gcr_addr, const uint32_t i_num_lanes, const uint32_t i_recal_req_vec)
{
    //set_debug_state(0x0012);

    // Check if need to run INIT on another lane
    // HW563765 - Don't check rx_any_init_req_or_reset due to preset bad lane
    int l_any_init_req_or_reset = 0; //get_ptr_field(io_gcr_addr, rx_any_init_req_or_reset);

    uint32_t l_lane = 0;

    for (; l_lane < i_num_lanes; ++l_lane)
    {
        // Set the current lane in the io_gcr_addr
        set_gcr_addr_lane(io_gcr_addr, l_lane);

        if ((0x80000000 >> l_lane) & i_recal_req_vec)
        {
            bool send_recal_done = false;

            if (!mem_pl_field_get(rx_init_done, l_lane))
            {
                mem_pl_field_put(rx_recal_before_init, l_lane, 1);
                send_recal_done = true;
            }
            else if (!l_any_init_req_or_reset)   // Prioritize INIT
            {
                // PSL_EN_NEXT
                run_recalibration(io_gcr_addr, l_lane); //sleeps at end
                //l_any_init_req_or_reset = get_ptr_field(io_gcr_addr, rx_any_init_req_or_reset); //Check again for INIT on another lane
                send_recal_done = true;
            }

            // HW537933 - Complete the recal handshake with the DL, even if run_lane=0 and we don't actually recal.
            // This helps flush out reset time issues with the DL recal request signals.
            if ( send_recal_done )
            {
                set_debug_state(0x0013); // DEBUG - Recal Done Handshake
                put_ptr_field(io_gcr_addr, rx_phy_dl_recal_done_set, 0b1, fast_write); // strobe bit

                while ((get_ptr_field(io_gcr_addr, rx_dl_phy_recal_req) ||
                        get_ptr_field(io_gcr_addr, rx_dl_phy_recal_abort)) &&
                       !fw_field_get(fw_stop_thread))
                {
                    io_sleep(get_gcr_addr_thread(io_gcr_addr));
                };

                set_debug_state(0x0014); // DEBUG - Recal Done Handshake Complete

                put_ptr_field(io_gcr_addr, rx_phy_dl_recal_done_clr, 0b1, fast_write); // strobe bit

                // Moved to run_recalibration(): Clear the recal_abort sticky bit now that recal_abort should be unasserted (in both external and internal recal modes)
                //put_ptr_field(io_gcr_addr, rx_dl_phy_recal_abort_sticky_clr, 0b1, fast_write); // strobe bit
            }

        }
    } //for lane

    //set_debug_state(0x0015);
    return;
} //dl_recal_req
