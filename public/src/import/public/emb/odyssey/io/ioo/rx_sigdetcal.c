/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/rx_sigdetcal.c $  */
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
// *! (C) Copyright International Business Machines Corp. 2023
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : rx_sigdetcal.c
// *! TITLE       :
// *! DESCRIPTION : Receiver Signal Detect Circuit BIST test PPE code.
// *!             :
// *!
// *! OWNER NAME  : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// jjb23080400 |jjb     | Initial Code
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

//------------------------------------------------------------------------------
// RX Sigdetect Calibration off_dm search function
//------------------------------------------------------------------------------

uint32_t rx_sigdetcal_search(t_gcr_addr* gcr_addr, uint32_t rx_sigdet_test_init_amp_value)
{
    put_ptr_field(gcr_addr, rx_sigdet_test_init_amp, rx_sigdet_test_init_amp_value,
                  read_modify_write); // set initial amplitude, will immediately switch to opposite amplitude
    uint32_t l_off_dm = 32;                                                 // Start search from 32
    uint32_t l_off_dm_search_done = 0;
    uint32_t l_rx_sdout_sample = 0;
    uint32_t l_off_dm_amp = 0;

    while (l_off_dm_search_done == 0)
    {
        put_ptr_field(gcr_addr, rx_off_dm, l_off_dm, read_modify_write);      // Update rx_off_dm
        put_ptr_field(gcr_addr, rx_sigdet_test_start, 1, read_modify_write);  // Start rx_sigdet bist
        io_sleep(get_gcr_addr_thread(gcr_addr));                              // Allow sigdet bist to complete
        put_ptr_field(gcr_addr, rx_sigdet_test_start, 0, read_modify_write);  // Clear rx_sigdet bist
        l_rx_sdout_sample = get_ptr_field(gcr_addr, rx_sigdet_test_sample2);  // Get sdout sample at end of bist

        if ((l_off_dm < 64) && (l_rx_sdout_sample == 0))                      // inc off_dm if sdout is low
        {
            l_off_dm ++;
        }
        else if ((l_off_dm < 64) && (l_rx_sdout_sample == 1))                 // save off_dm when sdout=1
        {
            l_off_dm_amp = l_off_dm;
            l_off_dm_search_done = 1;
        }
        else                                                                  // off_dm saturated
        {
            l_off_dm_amp = 63;
            l_off_dm_search_done = 1;
        }
    }

    return l_off_dm_amp;                                                    // return result
}// end rx_sigdetcal_search

//------------------------------------------------------------------------------
// RX Sigdetect Calibration
//------------------------------------------------------------------------------

void rx_sigdetcal(t_gcr_addr* gcr_addr)
{
    //start rx_sigdetcal
    set_debug_state(0xB000); // Start of RXSIGDET Calibration

    uint32_t initial_sigdet_pd_state = get_ptr_field(gcr_addr, rx_sigdet_pd); // Store Initial State of sigdet power down

    put_ptr_field(gcr_addr, rx_sigdet_pd, 0b0 , read_modify_write); // Power Up Sigdet
    io_spin(150); // Wait for sigdet power up to settle

    put_ptr_field(gcr_addr, rx_sigdet_test_en, 1, read_modify_write);      // set sigdet test enable high
    put_ptr_field(gcr_addr, rx_sigdet_test_clk_div, 1, read_modify_write); // set clk div to 500MHz
    put_ptr_field(gcr_addr, rx_sigdet_test_trans, 0, read_modify_write);   // switch to low amplitude immediately

    uint32_t l_off_dm_low_amp = rx_sigdetcal_search(gcr_addr, 1); // Search low amplitude off_dm
    uint32_t l_off_dm_hi_amp  = rx_sigdetcal_search(gcr_addr, 0); // Search hi amplitude off_dm

    uint32_t l_off_dm = (l_off_dm_low_amp + l_off_dm_hi_amp) * 3 / 8 + 8; // Calculate calibrated rx_off_dm

    put_ptr_field(gcr_addr, rx_off_dm, l_off_dm, read_modify_write);  // Update calibrated rx_off_dm
    put_ptr_field(gcr_addr, rx_sigdet_test_en, 0, read_modify_write); // Clear sigdet test enable
    put_ptr_field(gcr_addr, rx_sigdet_pd, initial_sigdet_pd_state,
                  read_modify_write); // Restore initial state of sigdet power down control
    set_debug_state(0xB001); // RXSIGDET Calibration Complete
}//end rx_sigdetcal
