/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_dcc_main.h $ */
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
// *! FILENAME    : tx_dcc_main.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// bja21101800 |bja     | Move DCC constants to a new header file
// mwh21092300 |mwh     | Moved to common to and removed out of tx_dcc_main.c to support iot and ioo
// gap20100800 |gap     | Backout repmux for P11
// gap20082500 |gap     | HW542315 correct repeating pattern when in half-width mode
// gap20050200 |gap     | Add main and pad dcc error propagation
// gap20010900 |gap     | Removed put_ptr_field_twoscomp define, no longer used
// vbr19111500 |vbr     | Initial implementation of debug levels
// gap19091000 |gap     | Change rx_dcc_debug to tx_dcc_debug HW503432
// mbs19090500 |mbs     | Updated tx_dcc_main_min_samples mem_reg for sim speedup
// gap19073000 |gap     | Updated to use modified dcdet circuitry and associated state machine
// gap19061300 |gap     | Added wait time for auto zero
// gap19030600 |gap     | Redefined dcc_min_iq_c and dcc_max_iq_c to values used in code
// gap19022800 |gap     | Corrected dcc_max_iq_c value
// vbr18081500 |vbr     | Removed return code constants replicated from ioo_common.h
// gap18043000 |gap     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _TX_DCC_MAIN_H_
#define _TX_DCC_MAIN_H_

#include "tx_dcc_tune_constants.h"

#define tx_dcc_main_dir_i_c  1 /* 1 increments tune_i  when p_lt_n, -1 decrements it */
#define tx_dcc_main_dir_q_c  1 /* 1 increments tune_q  when p_lt_n, -1 decrements it */
#define tx_dcc_main_dir_iq_c 1 /* 1 increments tune_iq when p_lt_n, -1 decrements it */

#define tx_dcc_main_max_step_i_c 4    /* maximum  i_tune step size used, must be power of 2 */
#define tx_dcc_main_max_step_q_c 4    /* maximum  q_tune step size used, must be power of 2 */
#define tx_dcc_main_max_step_iq_c 4   /* maximum iq_tune step size used, must be power of 2 */

#define tx_dcc_main_wait_tune_us_c  1 /* time from tune bit change to first cal edge                */

#define tx_dcc_main_ratio_thresh_c 2  /* one vote must be 2^this value times the other to move      */

// Run Duty cycle initialzation
int tx_dcc_main_init(t_gcr_addr* gcr_addr_i);

////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
// Some functions and macros to help in debugging.
// These are light weight but the code size and performance hit can add up,
// so allow for a compiler option to enable/disable based on debug level.
////////////////////////////////////////////////////////////////////////////////////////////
#if IO_DEBUG_LEVEL < 3
    #define set_tx_dcc_debug(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif //IO_DEBUG_LEVEL
#endif //_TX_DCC_MAIN_H_
