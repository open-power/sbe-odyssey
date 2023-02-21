/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_dcc_main_servo.h $ */
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
// *! FILENAME    : tx_dcc_main_servo.h
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
// gap23012500 |gap     | Issue 293532 log when tune bit hits min or max bound
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
#ifndef _TX_DCC_MAIN_SERVO_H_
#define _TX_DCC_MAIN_SERVO_H_

// Use this to set debug_state levels for testing to enable deep debug
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk,
// deep debug info will be generated; the current value of IO_DEBUG_LEVEL is 2
// You will may need to decrease $free_space_limit in ppe/obj/analyze_size.pl
// TX_DCC_MAIN_SERVO_DBG_LVL is a related define in *.c
#define TX_DCC_MAIN_SERVO_DEEP_DBG_LVL 3

#include <stdbool.h>
#include "tx_dcc_tune_constants.h"

#define tx_dcc_main_wait_tune_us_c  1 /* time from tune bit change to first cal edge                */



// types of servo ops; _I must be first in IOO for consistent logging with IOT
#ifdef IOO
typedef enum
{
    SERVOOP_I,
    SERVOOP_Q,
    SERVOOP_IQ
} t_servoop;
#endif

#ifdef IOT
typedef enum
{
    SERVOOP_I,
} t_servoop;
#endif

// result of accumulated samples
typedef enum
{
    COMP_RESULT_P_GT_N,
    COMP_RESULT_P_NEAR_N,
    COMP_RESULT_P_LT_N,
} t_comp_result;


// Run Duty cycle servo
void tx_dcc_main_servo(t_gcr_addr* gcr_addr_i, uint32_t step_size_i, int32_t dir_i, t_servoop op_i, int32_t min_tune_i,
                       int32_t max_tune_i, uint32_t min_samples_i, int32_t ratio_thresh_i);

t_comp_result tx_dcc_main_compare_result(t_gcr_addr* gcr_addr_i, uint32_t min_samples_i, int32_t ratio_thresh_i);

////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
// Some functions and macros to help in debugging.
// These are light weight but the code size and performance hit can add up,
// so allow for a compiler option to enable/disable based on debug level.
////////////////////////////////////////////////////////////////////////////////////////////
#if TX_DCC_MAIN_SERVO_DEEP_DBG_LVL > IO_DEBUG_LEVEL
    #define set_tx_dcc_debug(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif // TX_DCC_MAIN_SERVO_DEEP_DBG_LVL > IO_DEBUG_LEVEL

#endif //_TX_DCC_MAIN_SERVO_H_
