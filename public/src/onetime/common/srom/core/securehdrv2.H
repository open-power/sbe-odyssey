/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/securehdrv2.H $           */
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
 * @brief This file describes the format of Version 2 Secure Header
 *
 */

#ifndef SECURE_HDR_V2_H
#define SECURE_HDR_V2_H

#include <sha3.H>
#include <ecdsa.H>
#include <dilithium.H>

#define ECID_SIZE 16   //16 bytes of ECID

#define PREFIX_HDR 1
#define SW_HDR     2

/**
 *  @brief Indicates container header section versions
 */
enum CONTAINER_SECTION_VERSION : uint16_t
{
    CONTAINER_VERSION = 0x0002,
    HEADER_VERSION    = 0x0002,
};

/**
 *  @brief Indicates sizes of container header fields
 */
enum CONTAINER_FIELD_SIZE : size_t
{
    SW_HDR_COMP_ID_SIZE_BYTES = 8,
};

/**
 *  @brief Unique identifer for the hash algorithm to use
 */
enum ROM_HASH_ALGORITHM : uint8_t
{
    HASH_ALG_SHA2_512 = 0x01,
    HASH_ALG_SHA3_512 = 0x02,
};

/**
 *  @brief Unique identifier for the signature algorithm to use
 */
enum ROM_SIGNATURE_ALGORITHM : uint8_t
{
    SIG_ALG_ECDSA521           = 0x01,
    SIG_ALG_ECDSA521_DILITHIUM = 0x02,
};

/**
 * @brief Prefix and SW/FW version header
 *
 */
typedef struct
{
  uint16_t     version;     // (1: see versions above)
  uint8_t      hash_alg;    // (1: SHA-512, 2: SHA3-512)
  uint8_t      sig_alg;     // (1: SHA-512,ECDSA-521, 2: SHA3-512,ECDSA 521/Dilithium r2 8/7))
}__attribute__((packed)) ROM_version_raw;

/**
 * @brief Hardware Header format
 *
 */
typedef struct
{
  uint32_t        magic_number;    // (17082011)
  uint16_t        version;         // (1: see versions above)
  uint64_t        container_size;  // filled by caller
  uint8_t         reserved[6];
  ecc_key_t       hw_pkey_a;
  dilithium_key_t hw_pkey_d;
  uint64_t        prefix; // prefix header place holder
  // followed by sw header (if not special prefix)
  // followed by optional unprotected payload data
}__attribute__((packed)) ROM_container_raw;

/**
 * @brief Prefix Header Format
 *
 */
typedef struct
{
  ROM_version_raw ver_alg;
  uint64_t        reserved;
  uint32_t        flags;
  uint8_t         sw_key_count;
  uint64_t        payload_size;
  sha3_t          payload_hash;
  uint8_t         ecid[ECID_SIZE];
  uint8_t         reserved1[3];
  // followed by prefix data (sig,keys) key raw
}__attribute__((packed)) ROM_prefix_header_raw;

#define PREFIX_HEADER_SIZE(_p) (sizeof(ROM_prefix_header_raw))

/**
 * @brief Prefix Header HW Signatures and SW/FW PUB Keys
 *
 */
typedef struct
{
  ecc_signature_t        hw_sig_a;
  dilithium_signature_t  hw_sig_d;
  ecc_key_t              sw_pkey_p;
  dilithium_key_t        sw_pkey_s;
}__attribute__((packed)) ROM_prefix_data_raw;

/**
 * @brief SW/FW Header format
 *
 */
typedef struct
{
  ROM_version_raw ver_alg;
  uint64_t        reserved;
  char            component_id[SW_HDR_COMP_ID_SIZE_BYTES];
  uint32_t        flags;
  uint8_t         fw_secure_version;
  uint64_t        payload_size_protected;
  uint64_t        payload_size_unprotected;
  sha3_t          payload_hash_protected;
  uint8_t         ecid[ECID_SIZE];
  uint8_t         reserved1[7];
  // followed by sw sig raw
}__attribute__((packed)) ROM_sw_header_raw;

#define SW_HEADER_SIZE(_p) (sizeof(ROM_sw_header_raw))

/**
 * @brief SW/FW Header Signatures
 *
 */
typedef struct
{
  ecc_signature_t       sw_sig_p;
  dilithium_signature_t sw_sig_s;
  // followed by zero's padding to 10K
  // followed by protected sw payload_data
  // followed by unprotected sw payload_text
}__attribute__((packed)) ROM_sw_sig_raw;

/**
 * @brief Secure Header Verification Response
 *
 */
typedef enum { ROM_DONE, ROM_FAILED } ROM_response;

// Consts used for container header validation
const uint32_t ROM_MAGIC_NUMBER = 0x17082011;
const uint16_t ROM_VERSION = 2;
const uint8_t  ROM_HASH_ALG = 2;
const uint8_t  ROM_SIG_ALG = 2;
const uint8_t  SW_KEY_COUNT = 2;
const size_t   SECURE_HEADER_SIZE = 15368;
// The payload should contain minimum a hash,size of the file name and file name
// Hence it can never be less that 65 bytes
const size_t   MINIMUM_SECURE_HEADER_PAYLOAD_SIZE = 65;

/**
 * @brief HW Secure Boot Security Flags
 *
 */
enum HW_SB_FLAGS
{
    PWR_OPENPWR          = 0x80000000,
    PWR_ENTERPRISE       = 0x40000000,
    PWR_IBM_FCODE        = 0x20000000,
    PWR_AIX              = 0x10000000,
    Z                    = 0x08000000,
    PWR_VENDOR_FCODE     = 0x00100000,
    SBE_SB_DISABLE_CHECK = 0x00080000,
    PWR_KEY_TRANSITION   = 0x00000001

};

#endif //----SECURE_HDR_V2_H----
