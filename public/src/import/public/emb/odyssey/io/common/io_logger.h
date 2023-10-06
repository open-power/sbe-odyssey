/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_logger.h $  */
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
// *! FILENAME    : io_logger.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Chris Steffen       Email: cwsteffen@us.ibm.com
// *! BACKUP NAME :
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
// -----------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh23091900 |mwh     | Added DEBUG_BIST_TDR_FAIL
// mwh23091900 |mwh     | Added DEBUG_RX_BIST_LL_TEST_FAIL_DL EWM 310460 replaced lte since was unused
// jfg23030100 |jfg     | Added new log type for IOT Deskew TOO_MUCH and HANDSHAKE_END
// mwh22111100 |mwh     | Added and removed some enter ESD add LTE remvoed
// jfg22100700 |jfg     | Added new log type for IOT TX BIST DCC MIN/MAX fails
// vbr22032800 |vbr     | Added new log types for IOT
// mwh22032500 |mwh     | Added log type DEBUG_BIST_TXIDLE_FAIL
// jfg22031400 |jfg     | Add DEBUG_PPE_INIT_OVER_LIMIT
// vbr22011300 |vbr     | Added more new Tbus debug logs
// jjb21011200 |jjb     | replaced *bank_sync* with *sigdet*
// vbr21063000 |vbr     | Added new debug logs for some tbus handshake errors
// vbr21060200 |vbr     | Re-assigned the servo queue not empty log types
// vbr21051300 |vbr     | Added new debug log for thread active time
// vbr21033100 |vbr     | Added new debug log for pipe interface
// vbr21020901 |vbr     | Added new debug log for io_wait escape
// vbr21011901 |vbr     | Added new log for debugging thread_locked errors
// vbr20111800 |vbr     | HW552111: Added log type for timebase error (stuck/rollover)
// mwh20071400 |mwh     | Added log type DEBUG_RX_DAC_TEST_FAIL
// vbr20061101 |vbr     | Added log type for bank sync warning
// gap20032600 |gap     | Inserted DEBUG_BIST_TX_ZCAL_FAIL
// mwh20022500 |mwh     | Added log type DEBUG_BIST_TXSEG
// vbr20021300 |vbr     | Added log type for eye height fail
// cws20011000 |cws     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef __IO_LOGGER_H__
#define __IO_LOGGER_H__

//#define ADD_LOG(i_type, io_gcr_addr, i_data) _add_log(i_type, io_gcr_addr, i_data)
//#define ADD_LOG_NO_GCR_ADR(i_type, i_data) _add_log_no_gcr_addr(i_type, i_data)

#define ADD_LOG(...) ADD_LOG_IMPL(LOG_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

// These defines will return the number of arguments
#define LOG_VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define LOG_VA_NARGS(...) LOG_VA_NARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

// Uses the found number of arguments to call the correct pound define
#define ADD_LOG_IMPL2(count, ...) ADD_LOG ## count (__VA_ARGS__)
#define ADD_LOG_IMPL(count, ...) ADD_LOG_IMPL2(count, __VA_ARGS__)

// Defines based upon the argument count.
#define ADD_LOG2 _add_log_no_gcr_addr
#define ADD_LOG3 _add_log

/// DEBUG LOG TYPES START
#define DEBUG_MASK                              0x7F
#define DEBUG_NONE                              0x00
#define DEBUG_EXT_CMD                           0x01
#define DEBUG_BAD_EXT_CMD                       0x02
#define DEBUG_RECAL_ABORT                       0x03
#define DEBUG_BIST_VGA_GAIN_FAIL                0x04
#define DEBUG_BIST_CTLE_PEAK1_FAIL              0x05
#define DEBUG_BIST_CTLE_PEAK2_FAIL              0x06
#define DEBUG_BIST_ESD_FAIL                     0x07
#define DEBUG_RX_BIST_LL_TEST_FAIL_DL           0x08
#define DEBUG_BIST_QPA_FAIL                     0x09
#define DEBUG_BIST_TX_DCC_I_FAIL                0x0A
#define DEBUG_BIST_TX_DCC_Q_FAIL                0x0B
#define DEBUG_BIST_TX_DCC_IQ_FAIL               0x0C
#define DEBUG_BIST_TX_LS_FAIL                   0x0D
#define DEBUG_BIST_TX_HS_FAIL                   0x0E

#define DEBUG_TX_ZCAL_LIMIT                     0x0F
#define DEBUG_DCCAL_SERVO_QUEUE_NOT_EMPTY       0x10
#define DEBUG_INITCAL_SERVO_QUEUE_NOT_EMPTY     0x11
#define DEBUG_RECAL_SERVO_QUEUE_NOT_EMPTY       0x12
#define DEBUG_RX_CDR_LOCK_TIMEOUT               0x17
#define DEBUG_RX_EOFF_PRE_LOFF_LIMIT            0x18
#define DEBUG_RX_EOFF_LOFF_FAIL                 0x19
#define DEBUG_RX_EOFF_EOFF_FAIL                 0x1A
#define DEBUG_RX_EOFF_POFF_FAIL                 0x1B
#define DEBUG_RX_QPA_CDR_NOT_LOCKED             0x1C
#define DEBUG_RX_CTLE_SERVO_LIMIT               0x1D
#define DEBUG_RX_SIGDET_FAIL                    0x1E
#define DEBUG_RX_DFE_AP_ZERO_FAIL               0x1F
#define DEBUG_RX_DFE_NEG_CLK_ADJ_FAIL           0x20
#define DEBUG_RX_DFE_H1_LIMIT                   0x21
#define DEBUG_RX_DFE_DAC_LIMIT                  0x22
#define DEBUG_RX_DFE_NO_CONVERGANCE             0x23
#define DEBUG_RX_BIST_LL_TEST_FAIL              0x24
#define DEBUG_RX_EYE_HEIGHT_FAIL                0x25
#define DEBUG_BIST_TXSEG                        0x26
#define DEBUG_MAN_SERVO_DATAPIPE_FAIL           0x27
#define DEBUG_MAN_SERVO_TIMEOUT_FAIL            0x28
#define DEBUG_MAN_SERVO_OP                      0x29
#define DEBUG_MAN_SERVO_DATA                    0x2A
#define DEBUG_MAN_SERVO_DATA1                   0x2B
#define DEBUG_MAN_SERVO_DATA2                   0x2C
#define DEBUG_MAN_SERVO_MASK                    0x2D
#define DEBUG_MAN_SERVO_PATTERN                 0x2E
#define DEBUG_VGA_GAIN                          0x2F
#define DEBUG_RX_DAC_TEST_FAIL                  0x30
#define DEBUG_TIMEBASE_ERROR                    0x31
#define DEBUG_THREAD_LOCK_ERROR                 0x32
#define DEBUG_IO_WAIT_ESCAPE                    0x33
#define DEBUG_BAD_PIPE_CMD                      0x34
#define DEBUG_THREAD_ACTIVE_TIME_ERROR          0x35
#define DEBUG_BIST_TX_ZCAL_FAIL                 0x36
#define DEBUG_RX_BANK_SYNC_WARN                 0x37
#define DEBUG_PPE_INIT_OVER_TIME                0x38
#define DEBUG_GEN12_BANK_SYNC_FAIL              0x39

#define DEBUG_INIT_SETUP_ERROR                  0x40
#define DEBUG_INIT_CLEANUP_ERROR                0x41
#define DEBUG_RECAL_CLEANUP_ERROR               0x42
#define DEBUG_INIT_OR_DESKEW_NOT_RUN_ERROR      0x43
#define DEBUG_REPAIR_SLS_TIMEOUT_ERROR          0x44
#define DEBUG_BIST_TXDETRX_FAIL                 0x45
#define DEBUG_BIST_TXIDLE_FAIL                  0x46
#define DEBUG_SLS_NOP_TIMEOUT_ERROR             0x47
#define DEBUG_PRBS_TIMEOUT_ERROR                0x48
#define DEBUG_RECAL_SETUP_ERROR                 0x49
#define DEBUG_FUNC_DATA_TIMEOUT_ERROR           0x4A
#define DEBUG_BIST_TX_DCC_MIN_FAIL              0x4B
#define DEBUG_BIST_TX_DCC_MAX_FAIL              0x4C
#define DEBUG_DESKEW_HANDSHAKE_END_FAILURE      0x4D
#define DEBUG_DESKEW_TOO_MUCH_SKEW_FATAL        0x4E
#define DEBUG_BIST_TDR_FAIL                     0x4F

/// DEBUG LOG TYPES END

#include "io_lib.h"


/**
 * @brief Logging structure (8 Bytes)
 */
typedef struct struct_log
{
    unsigned int timestamp : 32; // Timestamp bits 48-16 (104days - 2ms)
    unsigned int trained   :  1; // (0): Pre/During Training, (1): Post Training
    unsigned int type      :  7; // Example: Warning DFE Wants to move by +6
    unsigned int unit      :  2; // 00-03
    unsigned int lane      :  5; // 00-31
    unsigned int bank      :  1; // 00-01
    unsigned int data      : 16; // Debug Data
} __attribute__((packed)) t_log;


/**
 * @brief Add Logging data to the DEBUG SECTION
 * @param[in] i_type      Type of Debug Log
 * @param[in] i_gcr_addr  GCR Target
 * @param[in] i_data      16 bits of User Defined Data
 * @return void
 */
void _add_log(const uint8_t i_type,  t_gcr_addr* io_gcr_addr, const uint16_t i_data);

/**
 * @brief Add Logging data to the DEBUG SECTION
 * @param[in] i_type      Type of Debug Log
 * @param[in] i_data      16 bits of User Defined Data
 * @return void
 */
static inline void _add_log_no_gcr_addr(const uint8_t i_type, const uint16_t i_data)
{
    _add_log(i_type, NULL, i_data);
}

#endif // __IO_LOGGER_H__
