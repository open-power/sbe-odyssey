/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/errorcodes.H $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
 * @brief This file contains failure status codes incase of secure header verification failure/
 *        pak read/boot errors.
 *        The status code will be updated in scratch reg.
 *        These codes are common for SROM and Boot Loader
 *
 * This file is included by assembly code, so please keep the
 * contents to #defines only!
 *
 */

#ifndef STATUS_CODES_H
#define STATUS_CODES_H

// 4bits --> Fail Area Indicator
// 8bits --> Error code

//Area Indicator
//0x0 --> Reserved
//0x1 --> ASM Code (SPI Init etc) Fails
//0x2 --> HWP Code Fails
//0x3 --> Boot Code Fails
//0x4 --> Secure Header Verification Fails
//0x5 --> File Errors Fails
//0x6 .. 0xf --> Unused

// Area 0x1 --> ASM Code (SPI Init etc) Fails
// 0x100..0x10f - errors locating command tables
#define BOOT_RC_MISSING_MAIN_FSM    0x101
#define BOOT_RC_INVALID_MAIN_FSM    0x102
#define BOOT_RC_INVALID_CUST_FSM    0x103
#define BOOT_RC_INVALID_PAK_MAGIC   0x104
#define BOOT_RC_INVALID_FN_SIZE     0x105
#define BOOT_RC_FILE_COMPRESSED     0x106

// 0x110..0x11f - errors during command table execution
#define BOOT_RC_INSTRUCTION_OVERRUN 0x110
#define BOOT_RC_VALUE_OVERRUN       0x111
#define BOOT_RC_INVALID_CALL        0x112
#define BOOT_RC_INVALID_BRANCH      0x113
#define BOOT_RC_ADDRESS_BLOCKED     0x114
#define BOOT_RC_SCAN_TYPE_BLOCKED   0x115

// Area 0x2 --> HWP Code Fails
// TODO: #include <asm_hwp_error_codes.H> from EKB

//Area 0x3 --> Boot Code Fails
// 0x10 to 0x1f are used for SROM Initial Boot Checks
#define BOOT_RC_INVALID_ECDSA_FUSE_CHECK_CNFG 0x310
#define BOOT_RC_INVALID_DILITHIUM_FUSE_CHECK_CNFG 0x311
#define BOOT_RC_SROM_COMPLETE_BIT_NOT_SET 0x312

//Area 0x4 --> Secure Header Verification Fails
// 0x00 to 0xf (These are not error codes)
#define NO_ERROR 0x400
#define SB_ENFORCEMENT_DISABLED 0x40f   // This is set to indicate Enforcement was disabled

// 0x10 to 0x1f are used for Secure Header Verification: Sanity Checks
#define SHV_RC_CONTAINER_SIZE_CHECK 0x410
#define SHV_RC_MINIMUM_PAYLOAD_SIZE_CHECK 0x411
#define SHV_RC_MINIMUM_SCRATCH_SIZE_CHECK 0x412
#define SHV_RC_SCRATCH_START_OFFSET_CHECK 0x413

// 0x20 to 0x2f are used for Secure Header Verification: Trivial Checks
#define SHV_RC_MAGIC_NUMBER_TEST 0x420
#define SHV_RC_CONTAINER_VERSION_TEST 0x421
#define SHV_RC_CONTAINER_SIZE_TEST 0x422
#define SHV_RC_HW_KEY_HASH_TEST 0x423

// 0x30 to 0x3f are used for Secure Header Verification: Prefix Header checks
#define SHV_RC_PREFIX_HDR_VER_TEST 0x430
#define SHV_RC_PREFIX_HASH_ALGO_TEST 0x431
#define SHV_RC_PREFIX_SIG_ALGO_TEST 0x432
#define SHV_RC_PREFIX_HEADER_SZ_TEST 0x433
#define SHV_RC_HW_ECDSA_SIG_TEST 0x434
#define SHV_RC_HW_DILITHIUM_SIG_TEST 0x435
#define SHV_RC_PREFIX_ECID_TEST 0x436
#define SHV_RC_PREFIX_HASH_TEST 0x437
#define SHV_RC_SW_KEY_PROTECTION_TEST 0x438
#define SHV_RC_SW_KEY_INVALID_COUNT 0x439
#define SHV_RC_PREFIX_PAYLD_SZ_TEST 0x43A

// 0x40 to 0x4f are used for Secure Header Verification: FW/SW Header checks
#define SHV_RC_SW_HDR_VER_TEST 0x440
#define SHV_RC_SW_HASH_ALGO_TEST 0x441
#define SHV_RC_SW_SIG_ALGO_TEST 0x442
#define SHV_RC_SW_HEADER_SZ_TEST 0x443
#define SHV_RC_SW_PROTECTED_PAYLD_SZ_TEST 0x444
#define SHV_RC_SW_UNPROTECTED_PAYLD_SZ_TEST 0x445
#define SHV_RC_COMPONENT_ID_TEST 0x446
#define SHV_RC_SECURE_VERSION_TEST 0x447
#define SHV_RC_SW_ECDSA_SIG_TEST 0x448
#define SHV_RC_SW_DILITHIUM_SIG_TEST 0x449
#define SHV_RC_SW_ECID_TEST 0x44A
#define SHV_RC_SW_HASH_TEST 0x44B

// Area 0x5 --> File Errors Fails
//
// 0x10 to 0xf are used for secure header file error
// In case of any pak read fail, the below base error value is added
// with the actual pak rc provided in:
// public/src/common/pakwrapper/archive_config.H (ARCHIVE_RETURN_CODES)
#define FILE_RC_SECURE_HEADER_FILE_READ_BASE_ERROR 0x510

// 0x20 to 0x2f are used for hash list file error
// In case of any pak read fail, the below base error value is added
// with the actual pak rc provided in:
// public/src/common/pakwrapper/archive_config.H (ARCHIVE_RETURN_CODES)
#define FILE_RC_HASH_LIST_FILE_READ_BASE_ERROR 0x520

// 0x30 to 0x3f are used for HW Keys Hash and MSV file
// In case of any pak read fail, the below base error value is added
// with the actual pak rc provided in:
// public/src/common/pakwrapper/archive_config.H (ARCHIVE_RETURN_CODES)
#define FILE_RC_HW_KEYS_HASH_MSV_FILE_READ_BASE_ERROR 0x530

// 0x40 to 0x4f are used for payload file error(bootloader.bin/runtime.bin)
// In case of any pak read fail, the below base error value is added
// with the actual pak rc provided in:
// public/src/common/pakwrapper/archive_config.H (ARCHIVE_RETURN_CODES)
#define FILE_RC_PAYLOAD_FILE_READ_BASE_ERROR 0x540

// 0x50 to 0x5f are used for payload hash verification with hash list
// The below base error value is added with the actual pak rc provided in:
// public/src/common/utils/sbeutil.H (HASH_LIST_RETURN_CODES)
#define FILE_RC_PAYLOAD_HASH_VERIFICATION 0x550

#endif
