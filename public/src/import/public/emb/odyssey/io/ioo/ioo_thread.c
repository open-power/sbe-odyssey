/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_thread.c $    */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_thread.c
// *! TITLE       :
// *! DESCRIPTION : IOO Thread Loop
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr22082300 |vbr     | Add thread time stress mode for testing PCIe
// vbr22031500 |vbr     | Issue 265958: move reading of some config into main loop
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21033100 |vbr     | Added PIPE command processing
// vbr21020800 |vbr     | Moved DL and EO code
// vbr21020400 |vbr     | Moved ext_cmd code
// vbr21012500 |vbr     | Updated for changed mem_regs
// vbr21012100 |vbr     | New pcie_mode config and RX/TX lane num split
// vbr21011300 |vbr     | New ext_cmd reg names to allow for more commands and now clear done when req=0
// vbr20121100 |vbr     | Removed P10 dd1 code
// vbr20120200 |vbr     | Removed zcal circuit code
// vbr20030200 |vbr     | Added hooks for handling PCIe training / PIPE signals
//-------------|P11     |-------------------------------------------------------
// vbr21011900 |vbr     | Updated thread_lock check to avoid possibility of thread_loop_count incrementing exactly 64K times
// mwh20092100 |mwh     | Add in the if statement for dac test code so will not run on DD1 or even if cmd is given
// mbs20072800 |mbs     | HW537933 - Updated dl_recal_req to fix issue with sending recal done erroneously after init_req is high
// mbs20072700 |mbs     | HW537933 - Updated dl_recal_req to complete recal handshake even when run_lane is 0
// gap20021100 |gap     | Added tx_zcal_bist in cmd_tx_bist_tests_pl
// vbr20021100 |vbr     | HW522731: smarter lane cal copy based on flywheel lane.
// bja20020500 |bja     | Use new tx_fifo_init() in cmd_tx_fifo_init_pl()
// bja20020300 |bja     | Set load/unload regs during fifo init
// gap20020600 |gap     | Added tx_seg_test in cmd_tx_bist_tests_pl
// bja20011700 |bja     | Address tx group in cmd_bist_final()
// bja20011501 |bja     | Remove set_cal_bank from cmd_bist_final
// bja20011500 |bja     | Add lane_mask to eo_rxbist_ber
// vbr20012700 |vbr     | HW520939: lane cal copying implemented for command interface
// cws20011400 |cws     | Added Debug Logs
// bja20011000 |bja     | Sleep while waiting for legacy zcal to finish
// cws20010900 |cws     | Added zcal state machine funcationality
// cws20000700 |cws     | Added Bist Status Fields
// cws19121100 |cws     | Added Tx Bist, Bist Link Layer and Phase Rotator Commands
// vbr19111500 |vbr     | Initial implementation of debug levels
// vbr19121101 |vbr     | HW516179: Re-implemented setting of rx_recal_run_or_unused on lanes where INIT has not been run
// gap19111200 |gap     | Added call to tx_ffe and tx_zcal_tdr
// mbs19111300 |mbs     | Update lanes_pon in cmd_ioreset_pl (HW504083)
// mbs19111100 |mbs     | Update lanes_pon in cmd_dccal_pl (HW504083)
// mbs19111100 |mbs     | Set dl_clk_en high in cmd_power_on_pl (HW508366)
// vbr19100300 |vbr     | Changing duplicate debug states.
// vbr19092000 |vbr     | HW506463: Added back in writing to fw_thread_stopped.
// cws19051300 |cws     | Changed API to Software
// mwh19042600 |mwh     | HW490057 Change order of excution put "lane power up" after DCcal -- meeting
// vbr19040200 |vbr     | HW487712: Remove ucontroller_test from primary image.
// vbr18121400 |vbr     | HW474927: Add config to enable/disable lane copy in init.
// vbr16021600 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_thread.h"
#include "io_logger.h"
#include "io_ext_cmd.h"
#include "eo_wrappers.h"
#include "ioo_axo_dl_ifc.h"
#include "ioo_pipe_ifc.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
#define IOO_THREAD_DBG_LVL 3

//#if IO_DEBUG_LEVEL == 0
//  #define FW_DEBUG(i_data) {}
//#else
//  #define FW_DEBUG(i_data) { fw_regs_u16[fw_addr(fw_debug_addr)] = (i_data); }
//#endif


/**
 * @brief Polling loop for each thread
 * @param[in] arg   Contains thread ID
 * @retval void
 */
void ioo_thread(void* arg)
{
    // Parse input parameters
    const int* l_config = (int*)arg;
    const int l_thread = l_config[0]; // config parameter 0 - the thread_id

    // Set the pointers for mem_regs and fw_regs to this thread's section
    set_pointers(l_thread);

#if IO_DEBUG_LEVEL >= 1
    // Debug info on the current thread running
    img_field_put(ppe_current_thread, l_thread);
#endif

    // Read bus_id from fw_regs - must match the hardware rx_bus_id & tx_bus_id (they must be the same).
    const int l_bus_id = fw_field_get(fw_gcr_bus_id);

    // Form gcr_addr structure
    t_gcr_addr l_gcr_addr;
    set_gcr_addr(&l_gcr_addr, l_thread, l_bus_id, rx_group, 0); // RX lane 0

    uint32_t l_stop_thread = 0;

    do
    {
        // Do not run loop  while fw_stop_thread is set; this may be done when in an error condition and need to reset.
        // Also, mirror the setting as the status/handshake.
        l_stop_thread = fw_field_get(fw_stop_thread);
        fw_field_put(fw_thread_stopped, l_stop_thread);

        if (l_stop_thread)
        {
            set_debug_state(0x00FF); // DEBUG - Thread Stopped

            if (mem_pg_field_get(ppe_thread_time_stress_mode))
            {
                // Stress threading inactive time by forcing wait in stopped threads.
                // 14.4+ us thread active time = 12us busy wait + 2.4us thread switch
                io_spin_us(12);
            }
        }
        else     //!fw_stop_thread
        {
            set_debug_state(0x0001); // DEBUG - Thread Loop Start

            // Read config from fw_regs
            const uint32_t l_num_lanes_rx  = get_num_rx_physical_lanes();
            const bool     l_pcie_mode     = fw_field_get(fw_pcie_mode);

            // Checks if any new command requests need to be run
            run_external_commands(&l_gcr_addr);

            if (!l_pcie_mode)   // AXO DL Interface
            {
                // DL Intial Training
                set_debug_state(0x0002, IOO_THREAD_DBG_LVL); // DEBUG - Thread DL Initial Training
                dl_init_req(&l_gcr_addr, l_num_lanes_rx);

                // DL Recalibration Request
                set_debug_state(0x0003, IOO_THREAD_DBG_LVL); // DEBUG - Thread DL Recal
                uint32_t l_recal_req_vec = get_dl_recal_req_vec(&l_gcr_addr);

                if (l_recal_req_vec)
                {
                    dl_recal_req(&l_gcr_addr, l_num_lanes_rx, l_recal_req_vec);
                }
            }
            else     // PIPE Interface (PCIe)
            {
                // Run highest priority PIPE command on each lane
                set_debug_state(0x0004, IOO_THREAD_DBG_LVL); // DEBUG - Thread PIPE Commands
                run_pipe_commands(&l_gcr_addr, l_num_lanes_rx);
            }

            // Auto recal.  Also checks for some status clears.
            set_debug_state(0x0005, IOO_THREAD_DBG_LVL); // DEBUG - Thread Auto Recal
            auto_recal(&l_gcr_addr, l_num_lanes_rx);

            set_debug_state(0x000F); // DEBUG - Thread Loop End
        } //!fw_stop_thread


        // Increment the thread loop count (for both active and stopped)
        uint16_t l_thread_loop_cnt = mem_pg_field_get(ppe_thread_loop_count);
        l_thread_loop_cnt = l_thread_loop_cnt + 1;

        if (l_thread_loop_cnt == 0)
        {
            l_thread_loop_cnt = 1;
        }

        mem_pg_field_put(ppe_thread_loop_count, l_thread_loop_cnt);

        // Yield to other threads at least once per thread loop
        io_sleep(get_gcr_addr_thread(&l_gcr_addr));
    }
    while(1);

} //ioo_thread

// Assumption Checking
PK_STATIC_ASSERT(ppe_thread_loop_count_width == 16);
