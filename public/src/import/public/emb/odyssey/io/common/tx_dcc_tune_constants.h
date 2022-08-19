/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_dcc_tune_constants.h $ */
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
#ifndef _TX_DCC_TUNE_CONSTANTS_H_
#define _TX_DCC_TUNE_CONSTANTS_H_

#include "ppe_com_reg_const_pkg.h"

#define TX_DCC_I_TUNE_WIDTH_5NM  tx_dcc_i_tune_width
#define TX_DCC_Q_TUNE_WIDTH_5NM  tx_dcc_q_tune_width
#define TX_DCC_IQ_TUNE_WIDTH_5NM tx_dcc_iq_tune_width
#define TX_DCC_I_TUNE_WIDTH_7NM  6
#define TX_DCC_Q_TUNE_WIDTH_7NM  6
#define TX_DCC_IQ_TUNE_WIDTH_7NM 5

/* In the offset encoding scheme, If PPE wants to write tune to 0,
 * it must add this value to achieve the circuit encoding for 0.
 * Example: PPE represents 0 as binary 00000. Circuit represents it as 01111. */
#define TX_DCC_IQ_TUNE_OFFSET_7NM  15
#define TX_DCC_I_TUNE_OFFSET_7NM    0
#define TX_DCC_Q_TUNE_OFFSET_7NM    0
#define TX_DCC_IQ_TUNE_OFFSET_5NM  31
#define TX_DCC_I_TUNE_OFFSET_5NM  128
#define TX_DCC_Q_TUNE_OFFSET_5NM  128

#define SET_TX_TUNE_CONSTANTS \
    unsigned int tx_dcc_i_tune_width_const; \
    unsigned int tx_dcc_q_tune_width_const; \
    unsigned int tx_dcc_iq_tune_width_const; \
    int tx_dcc_i_tune_offset_const; \
    int tx_dcc_q_tune_offset_const; \
    int tx_dcc_iq_tune_offset_const; \
    if ( is_odyssey() ) { \
        tx_dcc_i_tune_width_const   = TX_DCC_I_TUNE_WIDTH_7NM; \
        tx_dcc_q_tune_width_const   = TX_DCC_Q_TUNE_WIDTH_7NM; \
        tx_dcc_iq_tune_width_const  = TX_DCC_IQ_TUNE_WIDTH_7NM; \
        tx_dcc_i_tune_offset_const  = TX_DCC_I_TUNE_OFFSET_7NM; \
        tx_dcc_q_tune_offset_const  = TX_DCC_Q_TUNE_OFFSET_7NM; \
        tx_dcc_iq_tune_offset_const = TX_DCC_IQ_TUNE_OFFSET_7NM; \
    } else { \
        tx_dcc_i_tune_width_const   = TX_DCC_I_TUNE_WIDTH_5NM; \
        tx_dcc_q_tune_width_const   = TX_DCC_Q_TUNE_WIDTH_5NM; \
        tx_dcc_iq_tune_width_const  = TX_DCC_IQ_TUNE_WIDTH_5NM; \
        tx_dcc_i_tune_offset_const  = TX_DCC_I_TUNE_OFFSET_5NM; \
        tx_dcc_q_tune_offset_const  = TX_DCC_Q_TUNE_OFFSET_5NM; \
        tx_dcc_iq_tune_offset_const = TX_DCC_IQ_TUNE_OFFSET_5NM; \
    } \

#define tx_dcc_main_max_i_7nm   31
#define tx_dcc_main_min_i_7nm  -32
#define tx_dcc_main_max_q_7nm   31
#define tx_dcc_main_min_q_7nm  -32
#define tx_dcc_main_max_iq_7nm  16
#define tx_dcc_main_min_iq_7nm -15

#define tx_dcc_main_max_i_5nm   127
#define tx_dcc_main_min_i_5nm  -128
#define tx_dcc_main_max_q_5nm   127
#define tx_dcc_main_min_q_5nm  -128
#define tx_dcc_main_max_iq_5nm   32
#define tx_dcc_main_min_iq_5nm  -31

#define SET_TUNE_LIMITS \
    int tx_dcc_main_max_i_c;\
    int tx_dcc_main_max_q_c; \
    int tx_dcc_main_max_iq_c; \
    int tx_dcc_main_min_i_c; \
    int tx_dcc_main_min_q_c; \
    int tx_dcc_main_min_iq_c; \
    if ( is_odyssey() ) { \
        tx_dcc_main_max_i_c  = tx_dcc_main_max_i_7nm ;\
        tx_dcc_main_max_q_c  = tx_dcc_main_max_q_7nm ; \
        tx_dcc_main_max_iq_c = tx_dcc_main_max_iq_7nm; \
        tx_dcc_main_min_i_c  = tx_dcc_main_min_i_7nm ; \
        tx_dcc_main_min_q_c  = tx_dcc_main_min_q_7nm ; \
        tx_dcc_main_min_iq_c = tx_dcc_main_min_iq_7nm; \
    } else { \
        tx_dcc_main_max_i_c  = tx_dcc_main_max_i_5nm ;\
        tx_dcc_main_max_q_c  = tx_dcc_main_max_q_5nm ; \
        tx_dcc_main_max_iq_c = tx_dcc_main_max_iq_5nm; \
        tx_dcc_main_min_i_c  = tx_dcc_main_min_i_5nm ; \
        tx_dcc_main_min_q_c  = tx_dcc_main_min_q_5nm ; \
        tx_dcc_main_min_iq_c = tx_dcc_main_min_iq_5nm; \
    } \

#endif
