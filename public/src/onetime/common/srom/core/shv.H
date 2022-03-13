/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/shv.H $                   */
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
 * @brief This file contains the Interfaces for SBE Secure Header Verification
 *
 */

#ifndef __SHV_H
#define __SHV_H

#include <stdint.h>
#include "securehdrv2.H"
#include "securebootutil.H"

/**
 * @brief Response struct of secure header verification
 *
 */
typedef struct shvRsp
{
    union
    {
        uint8_t data[SHA3_DIGEST_LENGTH * 3] = {0};
        struct
        {
            sha3_t hwKeyHash;
            sha3_t fwKeyHash;
            sha3_t payloadHash;
        };
    }sha3;

    uint32_t flag = 0x00;
    uint8_t statusCode = 0x00;
}shvRsp_t;

/**
 * @brief Request struct of secure header verification
 *
 */
typedef struct shvReq
{
    uint32_t *containerStartOffset = NULL;
    size_t containerSize = 0x00;
    uint32_t *hashListStartOffset = NULL;
    size_t hashListSize = 0x00;
    size_t unproctectedPayloadSize = 0x00;
    sha3_t *pubKeyHashSet1 = NULL;
    sha3_t *pubKeyHashSet2 = NULL;
    uint64_t componentId = 0x00;
    uint32_t *hwEcid = NULL;
    uint32_t *swEcid = NULL;
    uint64_t sbeMsgRegValue = 0x00; //Will be filled internally
    uint32_t scratchStart = 0x00;
    uint32_t scratchSize = 0x00;
    uint8_t shvProgressCodeStartOffset = 0x00;
    uint8_t msv = 0x00;
    struct
    {
        bool hwKeyHashCheck = false;
        bool ecdsaCheck = false;
        bool dilithiumCheck = false;
        bool ecidCheck = false;
        bool matchingMSVCheck = false;
        bool secureBootVerificationEnforcement = false;
    } controlData;
} shvReq_t;

/**
 * @brief Verify Secure container.
 *
 * @param *shvReq Secure Header Verification Request struct
 *
 * @param *shvRsp Secure Header Verification Response struct
 *
 * @return Secure container verification response(ROM_DONE/POM_FAILED)
 */
ROM_response verifySecureHdr(shvReq_t *shvReq, shvRsp_t *shvRsp);

#endif /* __SHV_H */
