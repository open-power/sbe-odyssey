/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/shvprogresscode.H $       */
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

/**
 * @brief This file contains all progress code offset for secure header verification code that will be updated in
 *        sbe messaging register.
 *        This code will be called in SROM as well as boot loader.
 *        The actual progress code in messaging register is addition of
 *        base progress code + offset
 *
 */

#ifndef _SHVPROGRESSCODE_H_
#define _SHVPROGRESSCODE_H_

//SROM and Boot Loader common Progress code
#define COMPLETED_MSG_REG_FETCH 0x0
#define COMPLETED_CONTAINER_SIZE_CHECK 0x1
#define COMPLETED_HASH_LIST_SIZE_CHECK 0x2
#define COMPLETED_SCRATCH_SIZE_CHECK 0x3
#define COMPLETED_SCRATCH_START_OFFSET_CHECK 0x4
#define COMPLETED_MAGIC_CHECK 0x5
#define COMPLETED_CONTAINER_VER_CHECK 0x6
#define COMPLETED_HW_KEY_HASH_CALCULATION 0x7
#define COMPLETED_HW_KEY_HASH_CHECK 0x8
#define COMPLETED_PREFIX_HDR_CHECK 0x9
#define COMPLETED_PREFIX_HDR_SIZE_CHECK 0xA
#define COMPLETED_PREFIX_HDR_HASH_CALCULATION 0xB
#define COMPLETED_HW_SIG_A_CHECK 0xC
#define COMPLETED_HW_SIG_D_CHECK 0xD
#define COMPLETED_PREFIX_ECID_CHECK 0xE
#define COMPLETED_PREFIX_PAYLD_SZ_CHECK 0xF
#define COMPLETED_PREFIX_PAYLOAD_HASH_CALCULATION 0x10
#define COMPLETED_PREFIX_PAYLOAD_HASH_CHECK 0x11
#define COMPLETED_SW_KEY_COUNT_CHECK 0x12
#define COMPLETED_SW_KEY_PROTECTION_CHECK 0x13
#define COMPLETED_FW_SECURE_VERSION_CHECK 0x14
#define COMPLETED_SW_HEADER_CHECK 0x15
#define COMPLETED_SW_ECID_CHECK 0x16
#define COMPLETED_SW_HDR_HASH_CALCULATION 0x17
#define COMPLETED_SW_SIG_P_CHECK 0x18
#define COMPLETED_SW_SIG_S_CHECK 0x19
#define COMPLETED_COMPONENT_ID_CHECK 0x1A
#define COMPLETED_PROTECTED_PAYLOAD_SIZE_CHECK 0x1B
#define COMPLETED_UNPROTECTED_PAYLOAD_SIZE_CHECK 0x1C
#define COMPLETED_PAYLOAD_HASH_CALCULATION 0x1D
#define COMPLETED_PAYLOAD_HASH_CHECK 0x1E
#define COMPLETED_SECURE_HDR_VERIFICATION 0x1F

#endif /* _SHVPROGRESSCODE_H_ */
