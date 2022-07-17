/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/odyssey/bldr/core/progresscode.H $         */
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

#ifndef _PROGRESSCODE_H_
#define _PROGRESSCODE_H_

/**
 * @brief This file contains all progress code's for odyssey Boot Loader that will be updated in
 *        sbe messaging register
 *
 */

//Before Secure Header Verification Call
#define EABI_ENTER                                            0x01
#define EABI_EXIT                                             0x02
#define ENTERED_BLDR_MAIN                                     0x03
#define COMPLETED_PK_INIT                                     0x04
#define COMPLETED_THREAD_CREATION                             0x05
#define COMPLETED_BLDR_LFR_READ                               0x06
#define COMPLETED_BLDR_LOADING_SECURE_HEADER_INTO_PIB         0x07
#define COMPLETED_BLDR_LOADING_HASH_LIST_INTO_PIB             0x08
#define COMPLETED_BLDR_LOADING_HW_KEYS_HASH_MSV_INTO_PIB      0x09
#define COMPLETED_BLDR_READING_SROM_SETTINGS_FROM_MEASUREMENT 0x0A
#define COMPLETED_BLDR_READING_BLDR_SETTINGS_FROM_MEAS_HRESET 0x0B
#define COMPLETED_BLDR_READING_ECID_FROM_OTPROM               0x0C

//Secure Header Verification Call start
#define SHV_PROGRESS_CODE_START_OFFSET                        0x0D

//After Secure Header Verification Call
#define COMPLETED_HASH_CALCULATION_OF_FINAL_RESULT            0x2E
#define COMPLETED_HASH_WRITE_OR_READ_INTO_MEASUREMENT_REG     0x2F
#define COMPLETED_BLDR_WRITING_SECURE_BOOT_SETTINGS           0x30
#define COMPLETED_STATUS_CODE_WRITE_INTO_SCRATCH              0x31
#define COMPLETED_BLDR_MBX16_REG_READ                         0x32
#define COMPLETED_BLDR_MBX11_REG_READ                         0x33
#define COMPLETED_BLDR_LFR_WRITE                              0x34
#define COMPLETED_SPPE_BINARY_LOAD                            0x35
#define COMPLETED_SPPE_PAK_LOAD                               0x36
#define COMPLETED_VPD_PAK_LOAD                                0x37
#define COMPLETED_BLDR                                        0x3E

#endif /* _PROGRESSCODE_H_ */
