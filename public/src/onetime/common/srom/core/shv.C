/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/shv.C $                   */
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
 * @brief This file contains the SBE Secure Boot Verification Code
 *
 **/

#include "shv.H"
#include "sbetrace.H"
#include "ecdsa.H"
#include "dilithium.H"
#include "sha3.H"
#include "ppe42_string.h"
#include "sbeutil.H"
#include "errorcodes.H"
#include "scratch.H"
#include "shvprogresscode.H"
#include "p11_scom_perv_cfam.H"
#include "ppe42_scom.h"

#define VERIFY_FAILED(_c) { if(shvRsp->statusCode == NO_ERROR) \
                                shvRsp->statusCode=(_c); \
                            if(shvReq->controlData.secureBootVerificationEnforcement) \
                                return ROM_FAILED; } \

//Lets add the progress code start offset before we write into messaging register
#define UPDATE_SBE_PROGRESS_CODE(value) \
        SBE::updateProgressCode((value + shvReq->shvProgressCodeStartOffset)); \

#define MINIMUM_DILITHIUM_SCRATCH_SIZE (150 * 1024)

/**
 * @brief Validate prefix & SW/FW version, hash and signature algo version
 *
 * @param ver_alg pointer to version algo details
 * @param hdr_type Prefix or SW/FW Header
 * @return Status code in case of failure, 0 in case of success
 */
static uint32_t valid_ver_alg(ROM_version_raw* ver_alg, uint8_t hdr_type)
{
    #define SBE_FUNC " valid_ver_alg "
    SBE_ENTER(SBE_FUNC);

    uint32_t status;

    //Validate header version
    SBE_INFO("Hdr: Version : %d", SBE::get16(&ver_alg->version));
    if(SBE::get16(&ver_alg->version) != HEADER_VERSION)
    {
        SBE_ERROR(SBE_FUNC "FAILED: bad header version");
        status = (hdr_type == PREFIX_HDR) ? SHV_RC_PREFIX_HDR_VER_TEST : SHV_RC_SW_HDR_VER_TEST;
        return status;
    }

    //Validate header hash algo version
    SBE_INFO("Hdr: hash algo : %d", SBE::get8(&ver_alg->hash_alg));
    if(SBE::get8(&ver_alg->hash_alg) != HASH_ALG_SHA3_512)
    {
        SBE_ERROR(SBE_FUNC "FAILED: bad hash algorithm version");
        status = (hdr_type == PREFIX_HDR) ? SHV_RC_PREFIX_HASH_ALGO_TEST : SHV_RC_SW_HASH_ALGO_TEST;
        return status;
    }

    //Validate header sign algo version
    SBE_INFO("Hdr: Sign Algo : %d", SBE::get8(&ver_alg->sig_alg));
    if(SBE::get8(&ver_alg->sig_alg) != SIG_ALG_ECDSA521_DILITHIUM)
    {
        SBE_ERROR(SBE_FUNC "FAILED: bad signature algorithm version");
        status = (hdr_type == PREFIX_HDR) ? SHV_RC_PREFIX_SIG_ALGO_TEST : SHV_RC_SW_SIG_ALGO_TEST;
        return status;
    }

    SBE_EXIT(SBE_FUNC);
    return 0;
    #undef SBE_FUNC
}

static int dilithium_wrap(const unsigned char *sig,
                   const unsigned char *msg,
                   const unsigned char *pub,
                   const uint32_t scratchStart,
                   const uint32_t scratchSize)
{
    /*
     * Dilithium signature verification requires a truckload of stack space (and then some!!!!),
     * so we use a dynamically allocated block of scratch space for the stack.
     */

    // Mandatory macro inclusion for scratch use
    scratch sbeScratch(scratchStart, scratchSize);

    // Prepare the Dilithium stack
    polyvec_max *scratch = (polyvec_max *)sbeScratch.scratch_alloc(sizeof(polyvec_max) * 11);
    polyvec_max *w1      = scratch;
    polyvec_max *mat01   = w1 + 1;
    polyvec_max *mat234  = mat01 + 2;
    polyvec_max *mat567  = mat234 + 3;
    polyvec_max *z       = mat567 + 3;
    polyvec_max *h       = z + 1;

    if (w1 == NULL || mat01 == NULL || mat234 == NULL || mat567 == NULL || z == NULL || h == NULL) {
      SBE_INFO("Couldn't reserve enough stack space!!!!");
      return -1;
    } else {
      SBE_INFO("No problemo with stack!!!");
    }

    // Run the signature verification
    int retval = ref_verify2(sig, DILITHIUM_SIG_SIZE,
                 msg, SHA3_DIGEST_LENGTH, //mbytes,
                 pub, DILITHIUM_PUB_KEY_SIZE,
                 w1, mat01, mat234, mat567, z, h);

    SBE_INFO("RetVal: %d", retval);

    sbeScratch.scratch_free((void *)scratch);

    retval = !!(retval > 0); //..1.. ..1..

    return retval;
}

/**
 * @brief Verify Secure container.
 *
 * @param *shvReq Secure Header Verification Request struct
 *
 * @param *shvRsp Secure Header Verification Response struct
 *
 * @return Secure container verification response(ROM_DONE/POM_FAILED)
 */
static ROM_response ROM_verify(shvReq_t *shvReq, shvRsp_t *shvRsp)
{
    #define SBE_FUNC " ROM_verify "
    SBE_ENTER(SBE_FUNC);

    ROM_container_raw* container = (ROM_container_raw *)shvReq->containerStartOffset;
    ROM_prefix_header_raw* prefix;
    ROM_prefix_data_raw* hw_data;
    ROM_sw_header_raw* header;
    ROM_sw_sig_raw* sw_sig;
    sha3_t digest;
    //NOTE: Keep the array size 8 byte aligned to overcome sram allignment issues.
    //2468 bytes is MAX hash we calculate and hence buffer size is 2472 bytes.
    //i.e ECDSA PUB Key + Dilithium PUB Key
    //NOTE: The same buffer is used for hash list as well.
    //Currently considering hash list wont grow more than 2472
    const uint32_t hashDataBuffSize = sizeof(uint64_t) * 309;
    uint8_t hashDataBuff[hashDataBuffSize]  __attribute__ ((aligned(8))) = {0x00};
    uint64_t size;
    uint32_t hdrStatusChecks = 0;

    /**************************************** Trivial Checks ***************************************/

    //Validate Magic Number
    SBE_INFO("Magic number: 0x%X", SBE::get32(&container->magic_number));
    if(SBE::get32(&container->magic_number) != ROM_MAGIC_NUMBER)
    {
        SBE_ERROR (SBE_FUNC "FAILED : bad container magic number");
        VERIFY_FAILED(SHV_RC_MAGIC_NUMBER_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_MAGIC_CHECK);

    //Validate Container Version
    SBE_INFO("Container Version: 0x%X", SBE::get16(&container->version));
    if(SBE::get16(&container->version) != CONTAINER_VERSION)
    {
        SBE_ERROR (SBE_FUNC "FAILED : bad container version");
        VERIFY_FAILED(SHV_RC_CONTAINER_VERSION_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_CONTAINER_VER_CHECK);

    //Validate Container Size
    SBE_INFO("Container Size: 0x%X", SBE::get64(&container->container_size));
    if(SBE::get64(&container->container_size) != (shvReq->containerSize + shvReq->hashListSize))
    {
        SBE_ERROR (SBE_FUNC "FAILED : bad container size");
        VERIFY_FAILED(SHV_RC_CONTAINER_SIZE_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SH_CONTAINER_SIZE_CHECK);

    //Process HW Keys and verify HW keys Hash
    if(shvReq->controlData.hwKeyHashCheck)
    {
        memcpy(hashDataBuff, &container->hw_pkey_a, (sizeof(ecc_key_t) + sizeof(dilithium_key_t)));
        sha3_Hash(hashDataBuff, (sizeof(ecc_key_t) + sizeof(dilithium_key_t)), &digest);
        //Return the calculated SHA3-512 HW Key Hash.
        memcpy(shvRsp->sha3.hwKeyHash,digest,SHA3_DIGEST_LENGTH);

        UPDATE_SBE_PROGRESS_CODE(COMPLETED_HW_KEY_HASH_CALCULATION);

        if(memcmp(digest, shvReq->pubKeyHashSet1, SHA3_DIGEST_LENGTH))
        {
            SBE_INFO(SBE_FUNC "HW Key Hash Set1 Invalid. Trying with HW Key Hash Set2");
            if(memcmp(digest, shvReq->pubKeyHashSet2, SHA3_DIGEST_LENGTH))
            {
                SBE_INFO(SBE_FUNC "HW Key Hash Set1 & Set2 Invalid");
                SBE_ERROR (SBE_FUNC "FAILED : Invalid HW key's");
                VERIFY_FAILED(SHV_RC_HW_KEY_HASH_TEST);
            }
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_HW_KEY_HASH_CHECK);

    /**************************************************************************************************/

    /**************************************** Prefix Hdr Checks ***************************************/

    // process prefix header
    prefix = (ROM_prefix_header_raw*)&container->prefix;
    // test for valid header version, hash & signature algorithms (sanity check)
    hdrStatusChecks = valid_ver_alg(&prefix->ver_alg, PREFIX_HDR);
    if(hdrStatusChecks)
    {
        SBE_ERROR(SBE_FUNC "FAILED : bad prefix header version or hash/sig algo's");
        VERIFY_FAILED(hdrStatusChecks);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_HDR_CHECK);

    // test for valid prefix header signatures (all)
    hw_data = (ROM_prefix_data_raw*)(prefix->reserved1 + 3);

    // Validate the PREFIX_HEADER_SIZE fits in our hashDataBuff
    if(hashDataBuffSize < PREFIX_HEADER_SIZE(prefix))
    {
        VERIFY_FAILED(SHV_RC_PREFIX_HEADER_SZ_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_HDR_SIZE_CHECK);

    //Calculate Hash of prefix header
    if(shvReq->controlData.ecdsaCheck || shvReq->controlData.dilithiumCheck)
    {
        memcpy(hashDataBuff, prefix, PREFIX_HEADER_SIZE(prefix));
        sha3_Hash(hashDataBuff, PREFIX_HEADER_SIZE(prefix), &digest);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_HDR_HASH_CALCULATION);

    //Verify HW signature A (ECDSA521)
    if(shvReq->controlData.ecdsaCheck)
    {
        if(ec_verify(container->hw_pkey_a, digest, hw_data->hw_sig_a) < 1)
        {
            SBE_ERROR(SBE_FUNC "FAILED : Invalid HW signature A, ECDSA521");
            VERIFY_FAILED(SHV_RC_HW_ECDSA_SIG_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_HW_SIG_A_CHECK);

    //Verify HW signature D (Dilithium)
    if(shvReq->controlData.dilithiumCheck)
    {
        if(! (dilithium_wrap(hw_data->hw_sig_d,
                        digest,
                        container->hw_pkey_d,
                        shvReq->scratchStart,
                        shvReq->scratchSize)))
        {
            SBE_ERROR(SBE_FUNC "FAILED : Invalid HW signature D, Dilithium");
            VERIFY_FAILED(SHV_RC_HW_DILITHIUM_SIG_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_HW_SIG_D_CHECK);

    SBE_INFO("Prefix Hdr: Reserved : %d", SBE::get64(&prefix->reserved));
    SBE_INFO("Prefix Hdr: flags : %X", SBE::get32(&prefix->flags));
    //Return the Prefix Hdr flag
    shvRsp->flag = SBE::get32(&prefix->flags);

    // test for machine specific matching ecid
    if(shvReq->controlData.ecidCheck)
    {
        if(memcmp(prefix->ecid,shvReq->hwEcid,ECID_SIZE))
        {
            SBE_ERROR(SBE_FUNC "FAILED : unauthorized prefix ecid");
            VERIFY_FAILED(SHV_RC_PREFIX_ECID_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_ECID_CHECK);

    // test for valid prefix payload hash
    SBE_INFO("Prefix Hdr: Payload Size: %d", SBE::get64(&prefix->payload_size));
    size = SBE::get64(&prefix->payload_size);

    // Validate the prefix payload fits in our hashDataBuff
    if (hashDataBuffSize < size)
    {
        VERIFY_FAILED(SHV_RC_PREFIX_PAYLD_SZ_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_PAYLD_SZ_CHECK);

    memcpy(hashDataBuff, &hw_data->sw_pkey_p, size);
    sha3_Hash(hashDataBuff, size, &digest);
    //Return the calculated SHA3-512 FW/SW Key Hash.
    memcpy(shvRsp->sha3.fwKeyHash,digest,SHA3_DIGEST_LENGTH);
    memcpy(hashDataBuff, &prefix->payload_hash, SHA3_DIGEST_LENGTH);

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_PAYLOAD_HASH_CALCULATION);

    if(memcmp(&hashDataBuff, digest, sizeof(sha3_t)))
    {
        SBE_ERROR(SBE_FUNC "FAILED : invalid prefix payload hash");
        VERIFY_FAILED(SHV_RC_PREFIX_HASH_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PREFIX_PAYLOAD_HASH_CHECK);

    // test for valid sw key count
    SBE_INFO("Prefix Hdr: SW Key Count: %d", SBE::get8(&prefix->sw_key_count));
    if (SBE::get8(&prefix->sw_key_count) != SW_KEY_COUNT)
    {
        SBE_ERROR(SBE_FUNC "FAILED : sw key count not equal to 2");
        VERIFY_FAILED(SHV_RC_SW_KEY_INVALID_COUNT);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_KEY_COUNT_CHECK);

    // finish procesing prefix header
    // test for protection of all sw key material (sanity check)
    if(size != (sizeof(ecc_key_t) + sizeof(dilithium_key_t)))
    {
        SBE_ERROR(SBE_FUNC "FAILED : incomplete sw key protection in prefix header");
        VERIFY_FAILED(SHV_RC_SW_KEY_PROTECTION_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_KEY_PROTECTION_CHECK);

    /**************************************************************************************************/

    /**************************************** SW/FW Hdr Checks ***************************************/

    // start processing sw header
    header = (ROM_sw_header_raw*)(hw_data->sw_pkey_s + sizeof(dilithium_key_t));

    // test for fw secure version
    SBE_INFO("SW Hdr: Secure Version: %d", SBE::get8(&header->fw_secure_version));
    if(shvReq->controlData.matchingMSVCheck)
    {
        if(SBE::get8(&header->fw_secure_version) != shvReq->msv)
        {
            SBE_ERROR(SBE_FUNC "FAILED : bad container fw secure version");
            VERIFY_FAILED(SHV_RC_SECURE_VERSION_TEST);
        }
    }
    else
    {
        if(SBE::get8(&header->fw_secure_version) < shvReq->msv)
        {
            SBE_ERROR(SBE_FUNC "FAILED : bad container fw secure version");
            VERIFY_FAILED(SHV_RC_SECURE_VERSION_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_FW_SECURE_VERSION_CHECK);

    // test for valid header version, hash & signature algorithms (sanity check)
    hdrStatusChecks = valid_ver_alg(&header->ver_alg, SW_HDR);
    if(hdrStatusChecks)
    {
        SBE_ERROR(SBE_FUNC "FAILED : bad sw header version or hash/sign algo's");
        VERIFY_FAILED(hdrStatusChecks);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_HEADER_CHECK);

    // test for machine specific matching ecid
    if(shvReq->controlData.ecidCheck)
    {
        if(memcmp(header->ecid,shvReq->swEcid,ECID_SIZE))
        {
            SBE_ERROR(SBE_FUNC "FAILED : unauthorized SW ecid");
            VERIFY_FAILED(SHV_RC_SW_ECID_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_ECID_CHECK);

    sw_sig = (ROM_sw_sig_raw*) (header->reserved1 + 7);

    //Calculate Hash of SW/FW header
    if(shvReq->controlData.ecdsaCheck || shvReq->controlData.dilithiumCheck)
    {
        memcpy(hashDataBuff, header, SW_HEADER_SIZE(header));
        sha3_Hash(hashDataBuff, SW_HEADER_SIZE(header), &digest);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_HDR_HASH_CALCULATION);

    // test for valid sw header signatures (all)
    //Verify SW signature P (ECDSA521)
    if(shvReq->controlData.ecdsaCheck)
    {
        if(ec_verify(hw_data->sw_pkey_p, digest, sw_sig->sw_sig_p) < 1)
        {
            SBE_ERROR(SBE_FUNC "FAILED : Invalid SW signature P, ECDSA521");
            VERIFY_FAILED(SHV_RC_SW_ECDSA_SIG_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_SIG_P_CHECK);

    //Verify SW signature S (Dilithium)
    if(shvReq->controlData.dilithiumCheck)
    {
        if(! (dilithium_wrap(sw_sig->sw_sig_s,
                        digest,
                        hw_data->sw_pkey_s,
                        shvReq->scratchStart,
                        shvReq->scratchSize)))
        {
            SBE_ERROR(SBE_FUNC "FAILED : Invalid SW signature S, Dilithium");
            VERIFY_FAILED(SHV_RC_SW_DILITHIUM_SIG_TEST);
        }
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SW_SIG_S_CHECK);

    // test for valid component ID
    if(SBE::get64(&header->component_id) != shvReq->componentId)
    {
        SBE_ERROR(SBE_FUNC "FAILED : invalid component ID ");
        VERIFY_FAILED(SHV_RC_COMPONENT_ID_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_COMPONENT_ID_CHECK);

    // Compare proctected payload size
    SBE_INFO("SW/FW Hdr: Payload Size Protected: %d", SBE::get64(&header->payload_size_protected));
    if(SBE::get64(&header->payload_size_protected) != shvReq->hashListSize)
    {
        SBE_ERROR(SBE_FUNC "FAILED : Invalid protcted payload section size exp:%d, act:%d",
                   SBE::get64(&header->payload_size_protected),shvReq->hashListSize);
        VERIFY_FAILED(SHV_RC_SW_PROTECTED_PAYLD_SZ_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PROTECTED_PAYLOAD_SIZE_CHECK);

    // Compare un proctected payload size
    SBE_INFO("SW/FW Hdr: Payload Size Un-Protected: %d", SBE::get64(&header->payload_size_unprotected));
    if(SBE::get64(&header->payload_size_unprotected) != shvReq->unproctectedPayloadSize)
    {
        SBE_ERROR(SBE_FUNC "FAILED : Invalid un-protcted payload section size exp:%d, act:%d",
                   SBE::get64(&header->payload_size_unprotected),shvReq->unproctectedPayloadSize);
        VERIFY_FAILED(SHV_RC_SW_UNPROTECTED_PAYLD_SZ_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_UNPROTECTED_PAYLOAD_SIZE_CHECK);

    // Validate hash of hash list(payload).
    //Return the calculated SHA3-512 Payload Hash.
    //We are just returing the hash calculated during pak read, which was passed
    //as a input
    memcpy(shvRsp->sha3.payloadHash, shvReq->payloadHash, sizeof(sha3_t));

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PAYLOAD_HASH_CALCULATION);

    memcpy(hashDataBuff, &header->payload_hash_protected, SHA3_DIGEST_LENGTH);
    if(memcmp(&hashDataBuff, shvReq->payloadHash, SHA3_DIGEST_LENGTH))
    {
        SBE_ERROR(SBE_FUNC "FAILED : invalid sw payload hash");
        VERIFY_FAILED(SHV_RC_SW_HASH_TEST);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_PAYLOAD_HASH_CHECK);

    /**************************************************************************************************/

    SBE_INFO("Secure HDR Verified");

    SBE_EXIT(SBE_FUNC);
    return ROM_DONE;
    #undef SBE_FUNC
}

ROM_response verifySecureHdr(shvReq_t *shvReq, shvRsp_t *shvRsp)
{
    #define SBE_FUNC " verifySecureHdr "
    SBE_ENTER(SBE_FUNC);

    ROM_response status = ROM_DONE;

    // Get the current SBE Messaging Register value
    getscom_abs(scomt::perv::FSXCOMP_FSXLOG_SB_MSG, &shvReq->sbeMsgRegValue);
    SBE_INFO(SBE_FUNC "SBE Messaging Reg value [0x%08X 0x%08X]" ,SBE::higher32BWord(shvReq->sbeMsgRegValue),
                    SBE::lower32BWord(shvReq->sbeMsgRegValue));

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_MSG_REG_FETCH);

    // Lets do some sanity checks before we verify the container
    if(shvReq->containerSize != SECURE_HEADER_SIZE)
    {
        SBE_INFO(SBE_FUNC "Sanity Check: Invalid Container size");
        VERIFY_FAILED(SHV_RC_CONTAINER_SIZE_CHECK);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_CONTAINER_SIZE_CHECK);

    if(shvReq->hashListSize < MINIMUM_SECURE_HEADER_PAYLOAD_SIZE)
    {
        SBE_INFO(SBE_FUNC "Sanity Check: Invalid payload size");
        VERIFY_FAILED(SHV_RC_MINIMUM_PAYLOAD_SIZE_CHECK);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_HASH_LIST_SIZE_CHECK);

    //If Dilithium check is enabled, check if minimum scratch size is requested
    if(shvReq->controlData.dilithiumCheck && (shvReq->scratchSize < MINIMUM_DILITHIUM_SCRATCH_SIZE))
    {
        SBE_INFO(SBE_FUNC "Sanity Check: Not enough scratch size.Minimum scratch size required %d ",
                 MINIMUM_DILITHIUM_SCRATCH_SIZE);
        VERIFY_FAILED(SHV_RC_MINIMUM_SCRATCH_SIZE_CHECK);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SCRATCH_SIZE_CHECK);

    //If Dilithium check is enabled, check if scratch start offest is not NULL
    SBE_INFO(SBE_FUNC "scratch start is %08x ", shvReq->scratchStart);
    if(shvReq->controlData.dilithiumCheck && (shvReq->scratchStart == 0x00))
    {
        SBE_INFO(SBE_FUNC "Sanity Check: Scratch start offset is NULL");
        VERIFY_FAILED(SHV_RC_SCRATCH_START_OFFSET_CHECK);
    }

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SCRATCH_START_OFFSET_CHECK);

    //Verify the Secure Header
    status = ROM_verify(shvReq, shvRsp);

    UPDATE_SBE_PROGRESS_CODE(COMPLETED_SECURE_HDR_VERIFICATION);

    SBE_EXIT(SBE_FUNC);
    return status;
    #undef SBE_FUNC
}
