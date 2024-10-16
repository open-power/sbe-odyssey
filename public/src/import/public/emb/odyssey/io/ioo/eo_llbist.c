/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_llbist.c $     */
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
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_llbist.c
// *! TITLE       :
// *! DESCRIPTION : Run the ber check logic after we have run init and recal
// *!             :
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
// mwh23091900 |mwh     | EWM 310460 add in change so that Lab team can see what kind error it is.
// mwh23060700 |mwh     | Issue 306377 Zm thread vs lane swapping fix
// jjb23052300 |jjb     | Issue 305590 drive zero on rxdatavalid in pcie mode at end of llbist.
// jjb23052200 |jjb     | Issue 305590 drive rxdatavalid high in pcie mode llbist to enable PCS
// mwh23050300 |mwh     | Issue 304210 put dl clk enable in loop set all lanes
// mwh23040500 |mwh     | Issue 300849 add in check see if DL clock are on
// jjb22120700 |jjb     | Added Enabling/Disabling dl_clock
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22061500 |vbr     | Added returning of fail status for ext commands
// mwh21051700 |mwh     | Removed nv form reset
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// mwh20110300 |mwh     | Fix issue with fail flag, was getting cleared here HW552097
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// cws20011400 |cws     | Added Debug Logs
// mwh19111500 |mwh     | Increase the time we are checking to 2 microsec.
// bja19082100 |bja     | Set rx_fail_flag on fail
// bja19081900 |bja     | Set FIR on fail
// bja19081400 |bja     | Remove while loops. Use longer spins instead.
// mwh19040900 |mwh     | Initial Rev 51yy debug state
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "eo_llbist.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"


// most common for loop shorthand
#define FOR_LESS(i, stop) for(i = 0; i < stop; ++i)
// check if mask has the given lane selected. Big endian.
#define LANE_MASKED(mask, lane) (((mask << lane) & 0x80000000) == 0x0)


//NOTE:  Exceptation is that the rx_dl_clk_en is on when running bist.
//       It need to be on during init training so that a 1 will be in
//       the stick error latch in the DL. IF rx_dl_clk_en is not on
//       than LLBIST will not run correctly.

int eo_llbist(t_gcr_addr* gcr_addr, const uint32_t lane_mask)
{
    //start eo_llbist
    set_debug_state(0x51E0); // DEBUG
    int status = rc_no_error;
    int done_status = 0;

    // Make sure IO_DL_CLOCK is enabled, required to run LLBIST
    const uint32_t l_num_lanes = get_num_rx_lane_slices();
    int orig_gcr_lane = get_gcr_addr_lane(gcr_addr);
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    int lane = 0;
    FOR_LESS(lane, l_num_lanes)
    {
        // PSL lane_masked
        if LANE_MASKED(lane_mask, lane)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, lane);
        put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write);//pl
    }//end for
    set_gcr_addr_lane(gcr_addr, orig_gcr_lane);//back to per group

    //Grab error see if DL has clock on
    int error = get_ptr_field(gcr_addr, rx_pb_io_iobist_prbs_error);//pg

    //clock in DL or something is not right
    if (!error )
    {
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_BIST_LL_TEST_FAIL_DL, gcr_addr, 0);
        status = error_code;
        goto function_exit;
    }

    //turn on seed mode and turn off rx_berpl_timer_run_slow
    put_ptr_field(gcr_addr, rx_io_pb_iobist_reset, 0b1, read_modify_write);

    //Wait for prbs 15 to sink
    set_debug_state(0x51E1, 3);
    io_spin_us(2);//2 micro-sec

    //turn off seed mode and turn on rx_berpl_timer_run_slow
    put_ptr_field(gcr_addr, rx_io_pb_iobist_reset, 0b0, read_modify_write);

    //let comparison run
    set_debug_state(0x51E2, 3);
    io_spin_us(2);//1 micro-sec

    error = get_ptr_field(gcr_addr, rx_pb_io_iobist_prbs_error);//pg

    if ( error )
    {
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_BIST_LL_TEST_FAIL, gcr_addr, 0);
        status = error_code;
    }

    //Set done bit only if we have run the test, if we jump to function_exit
    //because of DL not working than done will not be set, but error will.
    done_status = 1;

function_exit:
    mem_pg_field_put(rx_linklayer_done, done_status );//pg
    // Read the number of threads from the img_regs. The IO threads have IDs [0, io_threads-1].
    //fix issue of lanes being across diffrent DL's, issue 306377
    int io_threads = img_field_get(ppe_num_threads);
    // Loop through and check each thread
    int thread;
    int thread_count_done;

    do
    {
        thread_count_done = 0; //reset thread_count_done

        for (thread = 0; thread < io_threads; thread++)
        {
            int llbist_done = mem_regs_u16_base_get(pg_base_addr(rx_linklayer_done_addr, thread), rx_linklayer_done_mask,
                                                    rx_linklayer_done_shift);//pg

            if (llbist_done)
            {
                thread_count_done = thread_count_done + 1;
            }
        } //for(thread)

        io_sleep(get_gcr_addr_thread(gcr_addr));//switch threads
    }
    while (io_threads != thread_count_done); //exit once all threads llbist done are high

    mem_pg_field_put(rx_linklayer_fail, error);//pg
    error |= mem_pg_field_get(rx_fail_flag);
    mem_pg_field_put(rx_fail_flag, error);

    // Disable IO_DL_CLOCK when finished LLBIST
    FOR_LESS(lane, l_num_lanes)
    {
        // PSL lane_masked
        if LANE_MASKED(lane_mask, lane)
        {
            continue;
        }

        set_gcr_addr_lane(gcr_addr, lane);
        put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0, read_modify_write);

        // PSL pcie_mode_llbist_rxdatavalid_disable
        if (l_pcie_mode)
        {
            put_ptr_fast(gcr_addr, pipe_fence_cntl4_pl_addr, 15, 0x0000); // clear PIPE Fence Output Value for RXDATAVALID low
        }
    }//end for
    set_gcr_addr_lane(gcr_addr, orig_gcr_lane);//back to per group

    set_debug_state(0x51E3); // DEBUG
    return status;
}//end eo_llbist
