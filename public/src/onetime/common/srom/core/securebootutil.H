/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/core/securebootutil.H $        */
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

#ifndef __SECURE_BOOT_UTIL_H
#define __SECURE_BOOT_UTIL_H

#include "stdint.h"
#include "ppe42_scom.h"
#include "securehdrv2.H"

#define SHA3_TRUNCATED_SIZE 48

typedef uint8_t sha3truncated_t[SHA3_TRUNCATED_SIZE];

/**
 * @brief Union to store sha3 truncated hash that needs to be written into
 *        measurement reg's
 *
 */
typedef union truncatedHashMeasurement
{
    sha3truncated_t sha3TruncatedHash = {0};

    struct
    {
        uint32_t hash[SHA3_TRUNCATED_SIZE/sizeof(uint32_t)];
    }hashValMeasurementWrite;

    /**
     * @brief Write the SHA3 Truncated Hash value into Measurement register's
     *
     * @param regAddr1 Register address to write truncated hash[0-3]
     * @param regAddr2 Register address to write truncated hash[4-7]
     * @param regAddr3 Register address to write truncated hash[8-11]
     * @param regAddr4 Register address to write truncated hash[12-15]
     * @param regAddr5 Register address to write truncated hash[16-19]
     * @param regAddr6 Register address to write truncated hash[20-23]
     * @param regAddr7 Register address to write truncated hash[24-27]
     * @param regAddr8 Register address to write truncated hash[28-31]
     * @param regAddr9 Register address to write truncated hash[32-35]
     * @param regAddr10 Register address to write truncated hash[36-39]
     * @param regAddr11 Register address to write truncated hash[40-43]
     * @param regAddr12 Register address to write truncated hash[44-47]
     */
    void putSha3TruncatedHash(uint32_t regAddr1,uint32_t regAddr2,
                              uint32_t regAddr3,uint32_t regAddr4,
                              uint32_t regAddr5,uint32_t regAddr6,
                              uint32_t regAddr7,uint32_t regAddr8,
                              uint32_t regAddr9,uint32_t regAddr10,
                              uint32_t regAddr11,uint32_t regAddr12)
    {
        putscom_abs(regAddr1, ((uint64_t)hashValMeasurementWrite.hash[0] << 32));
        putscom_abs(regAddr2, ((uint64_t)hashValMeasurementWrite.hash[1] << 32));
        putscom_abs(regAddr3, ((uint64_t)hashValMeasurementWrite.hash[2] << 32));
        putscom_abs(regAddr4, ((uint64_t)hashValMeasurementWrite.hash[3] << 32));
        putscom_abs(regAddr5, ((uint64_t)hashValMeasurementWrite.hash[4] << 32));
        putscom_abs(regAddr6, ((uint64_t)hashValMeasurementWrite.hash[5] << 32));
        putscom_abs(regAddr7, ((uint64_t)hashValMeasurementWrite.hash[6] << 32));
        putscom_abs(regAddr8, ((uint64_t)hashValMeasurementWrite.hash[7] << 32));
        putscom_abs(regAddr9, ((uint64_t)hashValMeasurementWrite.hash[8] << 32));
        putscom_abs(regAddr10, ((uint64_t)hashValMeasurementWrite.hash[9] << 32));
        putscom_abs(regAddr11, ((uint64_t)hashValMeasurementWrite.hash[10] << 32));
        putscom_abs(regAddr12, ((uint64_t)hashValMeasurementWrite.hash[11] << 32));
    }
}truncatedHashMeasurement_t;

/**
 * @brief Union to store sha3 hash read from OTPROM reg's
 *
 */
typedef union sha3HashOtprom
{
    sha3_t sha3Hash = {0};

    struct
    {
        uint64_t hash[SHA3_DIGEST_LENGTH/sizeof(uint64_t)];
    }hashValOtpromRead;

    /**
     * @brief Get the SHA3 Hash value from OTPROM register's
     *
     * @param regAddr1 Register address storing SHA3  hash[0-7]
     * @param regAddr2 Register address storing SHA3  hash[8-15]
     * @param regAddr3 Register address storing SHA3  hash[16-23]
     * @param regAddr4 Register address storing SHA3  hash[24-31]
     * @param regAddr5 Register address storing SHA3  hash[32-39]
     * @param regAddr6 Register address storing SHA3  hash[40-47]
     * @param regAddr7 Register address storing SHA3  hash[48-55]
     * @param regAddr8 Register address storing SHA3  hash[56-63]
     */
    void getSha3Hash(uint32_t regAddr1,uint32_t regAddr2,
                     uint32_t regAddr3,uint32_t regAddr4,
                     uint32_t regAddr5,uint32_t regAddr6,
                     uint32_t regAddr7,uint32_t regAddr8)
    {
        getscom_abs(regAddr1, &hashValOtpromRead.hash[0]);
        getscom_abs(regAddr2, &hashValOtpromRead.hash[1]);
        getscom_abs(regAddr3, &hashValOtpromRead.hash[2]);
        getscom_abs(regAddr4, &hashValOtpromRead.hash[3]);
        getscom_abs(regAddr5, &hashValOtpromRead.hash[4]);
        getscom_abs(regAddr6, &hashValOtpromRead.hash[5]);
        getscom_abs(regAddr7, &hashValOtpromRead.hash[6]);
        getscom_abs(regAddr8, &hashValOtpromRead.hash[7]);
    }

}sha3HashOtprom_t;

/**
 * @brief Union to read the stored ECID from OTPROM Reg's
 *
 */
typedef union ecid
{
    uint8_t ecid[ECID_SIZE] = {0};
    struct
    {
        uint64_t ecid[ECID_SIZE/sizeof(uint64_t)];
    }ecidOtpromRead;

    /**
     * @brief Get the Ecid object
     *
     * @param regAddr1 ECID0
     * @param regAddr2 ECID1
     */
    void getEcid(uint32_t regAddr1, uint32_t regAddr2)
    {
        getscom_abs(regAddr1, &ecidOtpromRead.ecid[0]);
        getscom_abs(regAddr2, &ecidOtpromRead.ecid[1]);
    }
}ecid_t;

enum signaturAlgoCheck
{
    DISABLE_SIGNATURE_ALGO,
    ENABLE_SIGNATURE_ALGO,
    INVALID_SIGNATURE_CNFG         // Halt PPE
};

/**
 * @brief Funtion to check whether to enable or disable signature algorithm
 *        based on OTPROM and MBX Secure Boot control data.
 *
 * @param signatureEnableScratchVal Secure Boot control data to enable signature algo from MBX scratch
 * @param signatureEnableFuseVal    Secure Boot control data to enable signature algo from OTPROM
 * @param signatureDisableFuseVal   Secure Boot control data to disable signature algo from OTPROM
 * @return uint8_t Enable/Disable/Invalid (signaturAlgoCheck)
 */
uint8_t getSignatureAlgoCheck(bool signatureEnableScratchVal,
                              bool signatureEnableFuseVal,
                              bool signatureDisableFuseVal);

/**
 * @brief Function to get the Minimum secure version basis OTPROM Fuse bits
 *
 * @param msvFuseBank0 Minimum Secure Version OTPROM Fuse Bank 0
 * @param msvFuseBank1 Minimum Secure Version OTPROM Fuse Bank 1
 * @param msvFuseBank2 Minimum Secure Version OTPROM Fuse Bank 2
 * @return uint8_t Minimum Secure Version
 */
uint8_t getMSV(uint64_t msvFuseBank0, uint64_t msvFuseBank1,
               uint64_t msvFuseBank2);

/**
 * @brief Print SHA3 result of truncated value(48B)
 *
 * @param result Pointer to SHA3 Truncated Result
 */
void SHA3TruncatedDump(sha3truncated_t *result);

#endif /***** __SECURE_BOOT_UTIL_H *********/
