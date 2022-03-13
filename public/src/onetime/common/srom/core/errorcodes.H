/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/errorcodes.H $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
 * @brief This file contains failure status codes incase of secure header varification failure.
 *        The status code will be updated in scratch reg.
 *
 */

#ifndef STATUS_CODES_H
#define STATUS_CODES_H

//0x00 and 0xff are not Error Codes
#define COMPLETED 0x00
#define SB_ENFORCEMENT_DISABLED 0xFF

#define INVALID_ECDSA_FUSE_CHECK_CNFG 0x01
#define INVALID_DILITHIUM_FUSE_CHECK_CNFG 0x02
#define CONTAINER_SIZE_CHECK 0x03
#define MINIMUM_PAYLOAD_SIZE_CHECK 0x04
#define MINIMUM_SCRATCH_SIZE_CHECK 0x05
#define SCRATCH_START_OFFSET_CHECK 0x06

#define MAGIC_NUMBER_TEST 0x07
#define CONTAINER_VERSION_TEST 0x08
#define CONTAINER_SIZE_TEST 0x09

#define HW_KEY_HASH_TEST 0x0A

#define PREFIX_HDR_VER_TEST 0x10
#define PREFIX_HASH_ALGO_TEST 0x11
#define PREFIX_SIG_ALGO_TEST 0x12
#define PREFIX_HEADER_SZ_TEST 0x13
#define HW_ECDSA_SIG_TEST 0x14
#define HW_DILITHIUM_SIG_TEST 0x15
#define PREFIX_ECID_TEST 0x16
#define PREFIX_HASH_TEST 0x17
#define SW_KEY_PROTECTION_TEST 0x18
#define SW_KEY_INVALID_COUNT 0x19
#define PREFIX_PAYLD_SZ_TEST 0x1A

#define SW_HDR_VER_TEST 0x20
#define SW_HASH_ALGO_TEST 0x21
#define SW_SIG_ALGO_TEST 0x22
#define SW_HEADER_SZ_TEST 0x23
#define SW_PROTECTED_PAYLD_SZ_TEST 0x24
#define SW_UNPROTECTED_PAYLD_SZ_TEST 0x25
#define COMPONENT_ID_TEST 0x26
#define SECURE_VERSION_TEST 0x27
#define SW_ECDSA_SIG_TEST 0x28
#define SW_DILITHIUM_SIG_TEST 0x29
#define SW_ECID_TEST 0x2A
#define SW_HASH_TEST 0x2B

#endif
