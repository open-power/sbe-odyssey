/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/rx_sigdetbist_test.c $ */
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
// *! FILENAME    : rx_sigdetbist_test.c
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
// mbs22082601 |mbs     | Updated with PSL comments
// jjb22081200 |jjb     | Issue 287309 : increased rx_sigdet_test_stat_alias to 16 bits
// jjb22080200 |jjb     | removed rx_b_sigdet_done
// vbr22061500 |vbr     | Added returning of fail status, only set FIRs on fail
// jjb22011000 |jjb     | Initial Code
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"

//------------------------------------------------------------------------------
// Runs 1 RX Sigdet BIST Test
//------------------------------------------------------------------------------

static uint32_t rx_sigdetbist_test1(t_gcr_addr* gcr_addr, uint32_t cntl, uint32_t stat, uint32_t test)
{
    uint32_t result = 0;
    put_ptr_field(gcr_addr, rx_sigdet_test_cntl_alias, cntl, fast_write);
    put_ptr_field(gcr_addr, rx_sigdet_test_stat_alias, stat, fast_write);
    io_spin(150);
    uint32_t l_rx_sigdet_test_stat_alias = get_ptr_field(gcr_addr, rx_sigdet_test_stat_alias);

    // PSL not_test_stat
    if (!(l_rx_sigdet_test_stat_alias & rx_sigdet_test_start_mask))   // set test1 done bit if start is zero
    {
        result = test; // set 5th LSB
    }

    // PSL test_stat
    if (l_rx_sigdet_test_stat_alias & rx_sigdet_test_fail_mask)   // set test1 fail bit
    {
        result = result | (test >> 4); // set 1st LSB
    }

    return result;
}//end rx_sigdetbist_test1

//------------------------------------------------------------------------------
// Runs 4 RX Sigdet BIST Tests
//------------------------------------------------------------------------------

int rx_sigdetbist_test(t_gcr_addr* gcr_addr)
{
    //start rx_sigdetbist_test
    set_debug_state(0x51F0, 3); // DEBUG : Start of RXSIGDET BIST
    uint32_t result = 0;
    uint32_t fail_result = 0;
    uint32_t done_result = 0;

    // Store Initial State of sigdet power down
    uint32_t initial_sigdet_pd_state = get_ptr_field(gcr_addr, rx_sigdet_pd);

    // Power Up Sigdet
    put_ptr_field(gcr_addr, rx_sigdet_pd, 0b0 , read_modify_write);
    io_spin(150);

    // Test 1 : Static Low Amplitude Test
    set_debug_state(0x51F1, 3); // DEBUG : Starting RXSIGDET BIST Test 1
    result = rx_sigdetbist_test1(gcr_addr, 0b110001000000000, 0b1111000000000000, 0x0080);

    // Test 2 : Static High Amplitude Test
    set_debug_state(0x51F2, 3); // DEBUG : Starting RXSIGDET BIST Test 2
    result = result | rx_sigdetbist_test1(gcr_addr, 0b110000000000000, 0b1001000000000000, 0x0040);

    // Test 3 : Dynamic Low to High Amplitude Test
    set_debug_state(0x51F3, 3); // DEBUG : Starting RXSIGDET BIST Test 3
    result = result | rx_sigdetbist_test1(gcr_addr, 0b110000111110000, 0b1101000000000000, 0x0020);

    // Test 4 : Dynamic High to Low Amplitude Test
    set_debug_state(0x51F4, 3); // DEBUG : Starting RXSIGDET BIST Test 4
    result = result | rx_sigdetbist_test1(gcr_addr, 0b110001111110000, 0b1011000000000000, 0x0010);

    // Turn off RX Sigdet Test Enable
    set_debug_state(0x51F5, 3); // DEBUG : Turn Off RXSIGDET Test Enable
    put_ptr_field(gcr_addr, rx_sigdet_test_cntl_alias, 0b000000000000000, fast_write);

    // Report Results
    set_debug_state(0x51F6, 3); // DEBUG : Reporting RXSIGDET BIST Results

    // PSL fail_result
    if (result & 0x000F)   // 1st LSB nibble of result are per test fail bits. Set fail_result if any tests fail.
    {
        fail_result = 1;
    }

    // PSL done_result
    if ((result & 0x00F0) ==
        0x00F0)   // 2nd LSB nibble of result are per test done bits. Set done_result only if all test are done.
    {
        done_result = 1;
    }

    int lane = get_gcr_addr_lane(gcr_addr);
    mem_pl_field_put(rx_sigdet_fail, lane, fail_result);
    mem_pl_field_put(rx_a_sigdet_done, lane,
                     done_result); // update bank a done bit even though there is only a single sigdet

    if (fail_result)
    {
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_SIGDET_FAIL, gcr_addr, result);
    }

    // Restore initial state of sigdet power down control
    put_ptr_field(gcr_addr, rx_sigdet_pd, initial_sigdet_pd_state , read_modify_write);
    set_debug_state(0x51F7, 3); // DEBUG : RXSIGDET BIST Complete

    return (fail_result ? error_code : pass_code);
}//end rx_sigdetbist_test
