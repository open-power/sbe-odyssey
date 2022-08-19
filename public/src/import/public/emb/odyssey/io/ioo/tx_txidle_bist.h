/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_txidle_bist.h $ */
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
// *! FILENAME    : tx_txidle_bist.h
// *! TITLE       :
// *! DESCRIPTION : TX Signal Detect Circuit BIST .
// *!             :
// *!
// *! OWNER  NAME : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh22032400 |mwh     | Initial Code
//------------------------------------------------------------------------------

#ifndef _TX_TXIDLE_BIST_H_
#define _TX_TXIDLE_BIST_H_

#include <stdbool.h>
#include "ioo_common.h"

// tx_txdetrx_bist

#define expect_0  0b0
#define expect_1 0b1
#define tx_idle_mode_ovr_alias_1111 0b1111
#define tx_idle_mode_ovr_alias_1110 0b1110
#define tdr_cntl_102_clp_padp_offset 0b01100110100
#define tdr_cntl_102_clp_padn_offset 0b01100110110
#define tdr_cntl_154_clp_padp_offset 0b10011010100
#define tdr_cntl_154_clp_padn_offset 0b10011010110
#define t1_p_pad_fail_exp_1 0b00000001
#define t2_n_pad_fail_exp_1 0b00000010
#define t3_p_pad_fail_exp_0 0b00000100
#define t4_n_pad_fail_exp_0 0b00001000
#define t5_p_pad_fail_exp_1 0b00010000
#define t6_n_pad_fail_exp_1 0b00100000
#define t7_p_pad_fail_exp_0 0b01000000
#define t8_n_pad_fail_exp_0 0b10000000

uint16_t tx_idle_tests(t_gcr_addr* gcr_addr, uint16_t tx_tdr_cntl_alias_in, uint16_t  expect,
                       uint16_t tx_idle_ovr_alias, uint16_t fail_mask);

int tx_txidle_bist(t_gcr_addr* gcr_addr, int tx_bist_enable_ls, int tx_bist_enable_hs);

// share with dcc since these are not run at the same time
#if IO_DEBUG_LEVEL < 3
    #define set_tx_dcc_debug_tx_idle_bist(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug_tx_idle_bist(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif //IO_DEBUG_LEVEL
#endif // _TX_TXIDLE_BIST_H_
