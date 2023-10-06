/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/eo_wrappers.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_wrappers.c
// *! TITLE       :
// *! DESCRIPTION : Common functions for calling EyeOpt flows from main thread
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23091800 |vbr     | EWM 301872: Changes for periodic (staggered) auto recal
// vbr23051100 |vbr     | EWM 304487: Use ppe_pipe_margin_mode_0_3 to check for margin mode instead of a HW register read
// vbr22120600 |vbr     | Add a sleep when exiting the auto_recal loop on an abort
// vbr22111501 |vbr     | Remove sleep after initial training
// vbr22110800 |vbr     | Added skips on a recal_abort
// mbs22083000 |mbs     | PSL comment updates
// vbr22061500 |vbr     | Add external command fail reporting
// vbr22033000 |vbr     | Removed unneeded IOT recal code
// vbr22020900 |vbr     | Issue 254482: no longer call eo_main_recal 3 consecutive times on first recal
// vbr22020800 |vbr     | Added a sleep every 12 lanes to the auto_recal loop
// vbr22011300 |vbr     | Updates to run recal conditions for IOT
// vbr21120300 |vbr     | Added setting of recal_or_unused for lanes in PCIe and Tbus.
// vbr21110200 |vbr     | Skip auto_recal on lane if PCIe mode and RX electical idle or RX margining in progress
// vbr21101200 |vbr     | Issue 254483: No longer set min_recal_cnt_reached on an abort, so check abort status
// vbr21092100 |vbr     | Issue 250867: Do not auto recal on AXO lane with recal abort asserted (OpenCAPI degraded mode)
// vbr21081800 |vbr     | Minor efficiency improvement with checking for a PIPE abort in auto_recal.
// vbr21071500 |vbr     | Switched to mem_reg for repair status
// vbr21070800 |vbr     | Further filled out IOT autorecal; needs a lot more work on the sls.
// vbr21063000 |vbr     | AutoRecal: Added checking of PIPE command as abort (IOO) and started adding IOT
// vbr21061000 |vbr     | Moved clearing of recal_abort_sticky to eo_wrappers and added rx_running_recal
// vbr21060200 |vbr     | Issue 241008: Changed location of sleep after recal (between calls)
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21050700 |vbr     | HW563765: Removed check of rx_any_init_req_or_reset
// vbr21042100 |vbr     | Made the rx_any_init_req_or_reset check more efficient in auto_recal
// vbr21040200 |vbr     | run_initial_training now sets lane and reg_id in gcr_adr
// vbr21031800 |vbr     | Story 230332: Fixed IOT recal infinite loop
// vbr21020800 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"

#include "eo_wrappers.h"
#include "io_logger.h"

#include "eo_main.h"

#ifdef IOO
    #include "ioo_common.h"
#endif
#ifdef IOT
    #include "iot_common.h"
#endif

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////

// Assert Lane Mask for Recal or Unused Lanes (for handshaking with supervisor thread)
PK_STATIC_ASSERT((rx_recal_run_or_unused_0_15_addr + 1) == rx_recal_run_or_unused_16_23_addr);
PK_STATIC_ASSERT((rx_recal_run_or_unused_0_15_addr % 2) == 0);
PK_STATIC_ASSERT(rx_recal_run_or_unused_0_15_width == 16);
PK_STATIC_ASSERT(rx_recal_run_or_unused_16_23_width == 8);
PK_STATIC_ASSERT(rx_recal_run_or_unused_16_23_startbit == 0);

void set_recal_or_unused(const uint32_t i_lane)
{
    uint32_t l_data = mem_pg_u32_raw_get(rx_recal_run_or_unused_0_15_addr); //reads _0_15 + _16_23
    l_data |= (0x80000000 >> i_lane);
    mem_pg_u32_raw_put(rx_recal_run_or_unused_0_15_addr, l_data); //writes _0_15 + _16_23
} //set_recal_or_unused

// Assert Lane Mask for Auto Recal Done Lanes (for handshaking with supervisor thread)
PK_STATIC_ASSERT((rx_auto_recal_done_0_15_addr + 1) == rx_auto_recal_done_16_23_addr);
PK_STATIC_ASSERT((rx_auto_recal_done_0_15_addr % 2) == 0);
PK_STATIC_ASSERT(rx_auto_recal_done_0_15_width == 16);
PK_STATIC_ASSERT(rx_auto_recal_done_16_23_width == 8);
PK_STATIC_ASSERT(rx_auto_recal_done_16_23_startbit == 0);

void set_auto_recal_done(const uint32_t i_lane)
{
    uint32_t l_data = mem_pg_u32_raw_get(rx_auto_recal_done_0_15_addr); //reads _0_15 + _16_23
    l_data |= (0x80000000 >> i_lane);
    mem_pg_u32_raw_put(rx_auto_recal_done_0_15_addr, l_data); //writes _0_15 + _16_23
} //set_auto_recal_done


////////////////////////////////////////////////
// EO Functions
////////////////////////////////////////////////

// Runs Rx Initial Training Per-Lane
int run_initial_training(t_gcr_addr* io_gcr_addr, const uint32_t i_lane)
{
    // Set correct lane and RX reg_id
    int l_saved_reg_id = get_gcr_addr_reg_id(io_gcr_addr);
    set_gcr_addr_reg_id_lane(io_gcr_addr, rx_group, i_lane);

    mem_pl_bit_set(rx_lane_busy, i_lane);

    int status = eo_main_init(io_gcr_addr);

    mem_pl_bit_set(rx_init_done, i_lane);
    mem_pl_bit_clr(rx_lane_busy, i_lane);

    // Don't require a recal in first interval after initial cal
    set_recal_or_unused(i_lane);

    // Restore original reg_id (if not RX since already set to rx_group)
    // PSL reg_id_not_rx
    if (l_saved_reg_id != rx_group)
    {
        set_gcr_addr_reg_id(io_gcr_addr, l_saved_reg_id);
    }

    //io_sleep(get_gcr_addr_thread(io_gcr_addr));
    return status;
} //run_initial_training


// Runs Rx Recalibration Per-Lane
int run_recalibration(t_gcr_addr* io_gcr_addr, const uint32_t i_lane)
{
    set_gcr_addr_lane(io_gcr_addr, i_lane);

    mem_pl_bit_set(rx_lane_busy, i_lane);
    mem_pg_bit_set(rx_running_recal); //Enables recal_abort checking

#ifdef IOO
    // AXO: Clear any previous recal_abort - this will set again if still active
    int pcie_mode = fw_field_get(fw_pcie_mode);

    if (!pcie_mode)
    {
        put_ptr_field(io_gcr_addr, rx_dl_phy_recal_abort_sticky_clr,      0b1, fast_write); // strobe bit
        put_ptr_field(io_gcr_addr, rx_dl_phy_bump_ui_mode_req_sticky_clr, 0b1, fast_write); // strobe bit
    }

#endif

    int status = eo_main_recal(io_gcr_addr);
    io_sleep(get_gcr_addr_thread(io_gcr_addr));

    mem_pg_bit_clr(rx_running_recal);
    mem_pl_bit_clr(rx_lane_busy, i_lane);

    if ((status & abort_code) == 0)
    {
        set_recal_or_unused(i_lane); // used for recal not run checking
        set_auto_recal_done(i_lane); // set for any recal, but only used for auto recal handshaking
    }

    return status;
} //run_recalibration


// Auto-Recalibration and EO status housekeeping
PK_STATIC_ASSERT((rx_enable_auto_recal_0_15_addr + 1) == rx_enable_auto_recal_16_23_addr);
PK_STATIC_ASSERT((rx_enable_auto_recal_0_15_addr % 2) == 0);
PK_STATIC_ASSERT(rx_enable_auto_recal_0_15_width == 16);
PK_STATIC_ASSERT(rx_enable_auto_recal_16_23_width == 8);
PK_STATIC_ASSERT(rx_enable_auto_recal_16_23_startbit == 0);

int auto_recal(t_gcr_addr* io_gcr_addr, const uint32_t i_num_lanes)
{
    set_debug_state(0x0005); // DEBUG - Auto Recal

    // Status updates
    int l_clr_recal_cnt         = mem_pg_field_get(rx_clr_lane_recal_cnt);
    int l_clr_eye_height_width  = mem_pg_field_get(rx_clr_eye_height_width);

    // Clear group historical hieght/width valid when requested
    // PSL clr_eye_height_width
    if (l_clr_eye_height_width)
    {
        mem_pg_bit_clr(rx_hist_min_eye_height_valid);
        mem_pg_bit_clr(rx_hist_min_eye_width_valid);
    }

#ifdef IOO
    // Auto recal enable
    int l_auto_recal_en_0_23       = mem_pg_u32_raw_get(rx_enable_auto_recal_0_15_addr); //reads _0_15 + _16_23
    int l_auto_recal_done_0_23     = mem_pg_u32_raw_get(rx_auto_recal_done_0_15_addr); //reads _0_15 + _16_23
    int l_auto_recal_periodic_run  = mem_pg_field_get(rx_auto_recal_periodic_run);

    // For PCIe, check if there is a PIPE commands to run on any lane as a recal abort. Redundant with ioo_thread check.
    int pcie_mode = fw_field_get(fw_pcie_mode);
    //int l_pipe_ifc_recal_abort = pcie_mode ? get_ptr_field(io_gcr_addr, rx_pipe_ifc_recal_abort) : 0;

    // For AXO, check the DL recal abort (for OpenCAPI degraded mode)
    int l_axo_dl_recal_abort_0_23 = 0;

    // PSL pcie_mode
    if (!pcie_mode)
    {
        l_axo_dl_recal_abort_0_23 = (get_ptr_field(io_gcr_addr, rx_recal_abort_0_15)  << 16) |
                                    (get_ptr_field(io_gcr_addr, rx_recal_abort_16_23) << (16 - rx_recal_abort_16_23_width));
    }

    // For PCI, check the PIPE margining mode (this read will just be 0 for AXO)
    int l_pipe_margin_mode_0_3 = mem_pg_field_get(ppe_pipe_margin_mode_0_3);
#endif

    // Loop through all lanes for recal (IOO) and house keeping (IOO/IOT)
    int thread = get_gcr_addr_thread(io_gcr_addr);
    int status = rc_no_error;
    uint32_t l_lane = 0;

    for (; l_lane < i_num_lanes; ++l_lane)
    {
        set_gcr_addr_lane(io_gcr_addr, l_lane);

        // PSL lane_mod_sleep
        if ((l_lane % 8) == 7)
        {
            io_sleep(thread);
        }

        // Clear recal counter when requested
        // PSL clr_recal_cnt
        if (l_clr_recal_cnt)
        {
            mem_pl_field_put(rx_lane_recal_cnt, l_lane, 0);
        }

        // Clear lane historical eye height/width valid when requested
        // PSL clr_eye_height_width
        if (l_clr_eye_height_width)
        {
            mem_pl_bit_clr(rx_lane_hist_min_eye_height_valid, l_lane);
            mem_pl_bit_clr(rx_lane_hist_min_eye_width_valid, l_lane);
        }

#ifdef IOT

        // IOT: Mark the lane as unused if it is a bad lane (spared out) or unused spare (never recalibrated)
        // PSL lane_bad_or_unused
        if ( mem_pl_field_get(rx_lane_bad, l_lane) || ((mem_pg_field_get(rx_spare_lane_code) >= 1)
                && (mem_pg_field_get(rx_spare_lane1) == l_lane)) )
        {
            set_recal_or_unused(l_lane);
        }

#endif

#ifdef IOO
        // IOO: Mark the lane as unused if init has not been run on it (PCIe)
        int l_init_done = mem_pl_field_get(rx_init_done, l_lane);

        // PSL init_done
        if (!l_init_done)
        {
            set_recal_or_unused(l_lane);
        }

        // IOO: Run Simple Auto Recalibration if necessary and initial training has been run
        // PSL auto_recal
        if (l_init_done && ((0x80000000 >> l_lane) & l_auto_recal_en_0_23))
        {
            // Skip running a long recal if need to handle a PIPE command (redundant)
            //if (l_pipe_ifc_recal_abort) continue;

            // Issue 250867: Do not run recal if dl_phy_rx_recal_abort is asserted (OpenCAPI degraded mode)
            // PSL axo_abort
            if ((0x80000000 >> l_lane) & l_axo_dl_recal_abort_0_23)
            {
                continue;
            }

            // EWM 301872: Periodic Auto Recal
            // Freely run recal for rx_auto_recal_free_run_cnt recals.
            // Then switch to a mode where periodically each thread gets to run 1 recal per lane.
            // For PAXO, these modes can overlap - some lanes can be free running while other are on the periodic recal.
            int l_lane_periodic_recal_mode = mem_pl_field_get(rx_free_run_auto_recal_cnt_reached, l_lane);

            // PSL lane_periodic_recal_mode
            if (l_lane_periodic_recal_mode)
            {
                // Skip recal if this lane is in periodic mode, but this thread is not enabled to run
                // PSL auto_recal_periodic_run
                if (l_auto_recal_periodic_run == 0)
                {
                    continue;
                }

                // Skip recal if this lane is in periodic mode, but this lane has already run recal this period
                // PSL auto_recal_done
                if ((0x80000000 >> l_lane) & l_auto_recal_done_0_23)
                {
                    continue;
                }
            }

            // PCIe: Do not run recal under these conditions
            // PSL recal_pcie_mode
            if (pcie_mode)
            {
                // Do not run recalibration if RX margining is enabled
                // PSL recal_pcie_rxmargin
                if ((0x8 >> l_lane) & l_pipe_margin_mode_0_3)
                {
                    continue;
                }

                //if (get_ptr_field(io_gcr_addr, rx_berpl_pcie_rx_margin_start)) continue;

                // Do not run recalibration if RX is inactive (electrical idle, or no data on bus)
                set_gcr_addr_reg_id(io_gcr_addr, tx_group);
                int rx_active = get_ptr_field(io_gcr_addr, pipe_state_rxactive);
                set_gcr_addr_reg_id(io_gcr_addr, rx_group);

                if (!rx_active)
                {
                    continue;
                }
            }

            // Run recal and exit loop on a PIPE abort
            io_sleep(thread);
            status |= run_recalibration(io_gcr_addr, l_lane); //sleeps at end

            if (pcie_mode && (status & abort_code))
            {
                break; // quick escape for pipe_abort
            }

            // Check again for a pending PIPE command (redundant)
            //if (pcie_mode) { l_pipe_ifc_recal_abort = get_ptr_field(io_gcr_addr, rx_pipe_ifc_recal_abort); }
        } // if init_done && auto_recal

#endif //IOO

    } //for lane

    return status;
} //auto_recal
