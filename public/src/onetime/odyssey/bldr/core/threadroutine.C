/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/odyssey/bldr/core/threadroutine.C $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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

#include "sbetrace.H"
#include "threadroutine.H"
#include "progresscode.H"
#include "measurementregs.H"
#include "shv.H"
#include "sha3.H"
#include "odysseylink.H"
#include "errorcodes.H"
#include "ppe42_string.h"
#include "mbxscratch.H"
#include "globals.H"
#include "sbeutil.H"
#include "functionaddress.h"
#include "pakwrapper.H"
#include "errorcodes.H"
#include "otpromfusemap.H"
#include "p11_scom_perv_cfam.H"
#include "odysseyfilenames.H"

#define RUN_TIME_SH_COMPONENT_ID 0x52554E5F54494D45ull  //RUN_TIME

//We are assuming hash list can never go more than 2k
uint8_t hashList[HASH_LIST_SIZE] __attribute__((section (".hash_list"))) = {0x00};

enum load_image_flags {
    LIF_IS_PAK = 1,
    LIF_ALLOW_ABSENT = 2,
};

// MBX fail status code
secureBootFailStatus_t secureBootFailStatus = {0};

/// @brief Wrapper that loads an image from PNOR into SRAM and checks its hash
///
/// This function will make sure that the bootloader does not get overwritten
/// by the loaded file.
/// It will halt with an error code on any unmasked error,
/// so there is no return value.
///
/// @param[in]    i_pak           PakWrapper targeted to PNOR
/// @param[in]    i_fname         Name of file to load
/// @param[inout] io_load_offset  Address to load file to; will be advanced
///                               to the end of the file after loading
/// @param[in]    i_flags         Flags to control operation details
void load_image(PakWrapper &i_pak, const char *i_fname, uint32_t &io_load_offset,
                uint32_t i_flags = 0)
{
    sha3_t digest;
    uint32_t size = 0;
    uint32_t size_available = BOOTLOADER_ORIGIN - io_load_offset;

    ARC_RET_t pakRc = i_pak.read_file(i_fname, (void *)io_load_offset, size_available, &digest, &size);
    if (pakRc == ARC_FILE_NOT_FOUND && (i_flags & LIF_ALLOW_ABSENT))
    {
        SBE_INFO(SBE_FUNC "Optional payload not found - skipping");
        return;
    }
    if (pakRc != ARC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC "Failed to read payload");
        SBE::updateErrorCodeAndHalt(FILE_RC_PAYLOAD_FILE_READ_BASE_ERROR + pakRc);
    }

    auto hashListRc = SBE::check_file_hash(i_fname, digest, hashList);
    if(hashListRc != SBE::HASH_COMPARE_PASS)
    {
        SBE_ERROR(SBE_FUNC "Failed to verify payload hash");
        SBE::updateErrorCodeAndHalt(FILE_RC_PAYLOAD_HASH_VERIFICATION + hashListRc);
    }

    io_load_offset += size;

    if (i_flags & LIF_IS_PAK)
    {
        // Rewind the write pointer by 8 bytes so that the next load overwrites
        // the pak end marker and appends the next pak to the current one.
        io_load_offset -= 8;
    }
}

void bldrthreadroutine(void *i_pArg)
{
    #define SBE_FUNC " bldrthreadroutine "
    SBE_ENTER(SBE_FUNC);

    SBE_INFO( SBE_FUNC "Inside Boot Loader thread");

    do{
        sha3_t digest;
        hwKeysHashMsv_t hwKeysHashMsv;
        uint32_t hwKeysHashMsvFileSize = 0x00;

        // Input for Secure Hdr verification
        shvReq_t shvReq;

        //Secure Container
        uint8_t *secureContainer = (uint8_t *)BOOTLOADER_SCRATCH_START;

        // Rc for pak
        ARC_RET_t pakRc = ARC_INVALID_PARAMS;

        // The entire NOR looks like a single pak so we can point to its beginning
        // instead of looking for a specific partition.
        // TODO: Partition start offset needs to be read from scratch reg
        PakWrapper pak((void *)NOR_PARTITION_0_OFFSET);

        //Load the secure container into pibmem
        pakRc = pak.read_file(secure_hdr_fname, secureContainer, SECURE_HEADER_SIZE, NULL, &shvReq.containerSize);
        if(pakRc != ARC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "Failed to read secure header");
            SBE::updateErrorCodeAndHalt(FILE_RC_SECURE_HEADER_FILE_READ_BASE_ERROR + pakRc);
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_LOADING_SECURE_HEADER_INTO_PIB);

        //Load the hash list into pibmem
        pakRc = pak.read_file(hash_list_fname, hashList, HASH_LIST_SIZE, &digest, &shvReq.hashListSize);
        if(pakRc != ARC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "Failed to read hash list");
            SBE::updateErrorCodeAndHalt(FILE_RC_HASH_LIST_FILE_READ_BASE_ERROR + pakRc);
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_LOADING_HASH_LIST_INTO_PIB);

        //Read the HW Keys hash set 1 and MSV from PNOR
        pakRc = pak.read_file(hw_keys_hash_fname, hwKeysHashMsv.hwKeysHashMsv, sizeof(hwKeysHashMsv.hwKeysHashMsv), NULL, &hwKeysHashMsvFileSize);
        if(pakRc != ARC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "Failed to read HW Keys Hash and MSV");
            SBE::updateErrorCodeAndHalt(FILE_RC_HW_KEYS_HASH_MSV_FILE_READ_BASE_ERROR + pakRc);
        }
        shvReq.pubKeyHashSet1 = &hwKeysHashMsv.hwKeyHash;

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_LOADING_HW_KEYS_HASH_MSV_INTO_PIB);

        shvReq.containerStartOffset = (uint32_t *)secureContainer;
        shvReq.componentId = RUN_TIME_SH_COMPONENT_ID;
        shvReq.scratchStart = SROM_BOOTLOADER_SCRATCH_START;
        shvReq.scratchSize = SROM_BOOTLOADER_SCRATCH_SIZE;
        shvReq.shvProgressCodeStartOffset = SHV_PROGRESS_CODE_START_OFFSET;
        shvReq.payloadHash = &digest;

        //Read the SROM secure boot control mesaurement register values into boot loader struct
        sbCtrlMeasurement_t bldrSbCtrlMeasurement;
        bldrSbCtrlMeasurement.getSbCtrlData(MEASUREMENT_REG_24);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_READING_SROM_SETTINGS_FROM_MEASUREMENT);

        //We need to keep all settings same as SROM except for MSV
        //Lets update MSV thats read from PNOR
        bldrSbCtrlMeasurement.msv = hwKeysHashMsv.msv;

        //Check if SROM boot complete bit is set. If not halt
        if(bldrSbCtrlMeasurement.bootComplete != 0x01)
        {
            SBE_INFO(SBE_FUNC "SROM Boot complete bit 0x%01x", (uint8_t)bldrSbCtrlMeasurement.bootComplete);
            SBE::updateErrorCodeAndHalt(BOOT_RC_SROM_COMPLETE_BIT_NOT_SET_IN_BLDR);
        }

        //Clear out the boot loader boot complete bit.
        //It will be set once measurement regs are updated with result
        bldrSbCtrlMeasurement.bootComplete = 0x0;

        SBE_INFO(SBE_FUNC "Boot Loader Secure Boot Control Measurement Reg Value: 0x%08x",
                 bldrSbCtrlMeasurement.secureBootControl);

        //Fill in the secure header request control data
        shvReq.controlData.secureBootVerificationEnforcement = bldrSbCtrlMeasurement.secureBootVerificationEnforcement;
        shvReq.controlData.ecdsaCheck = bldrSbCtrlMeasurement.ecdsaSigCheckEnable;
        shvReq.controlData.dilithiumCheck = bldrSbCtrlMeasurement.dilithiumSigCheckEnable;
        shvReq.controlData.hwKeyHashCheck = bldrSbCtrlMeasurement.hwKeyHashVerificationEnable;
        shvReq.controlData.ecidCheck = bldrSbCtrlMeasurement.ecidCheckEnable;
        shvReq.controlData.matchingMSVCheck = bldrSbCtrlMeasurement.enforceMatchingMSV;
        shvReq.msv = bldrSbCtrlMeasurement.msv;
        //TODO: Check why needed
        bldrSbCtrlMeasurement.secureModeEnable = 0;

        SBE_INFO(SBE_FUNC "Secure Boot Verification Enforcement 0x%01x",
                            shvReq.controlData.secureBootVerificationEnforcement);
        SBE_INFO(SBE_FUNC "ECDSA Check 0x%01x",shvReq.controlData.ecdsaCheck);
        SBE_INFO(SBE_FUNC "Dilithium Check 0x%01x",shvReq.controlData.dilithiumCheck);
        SBE_INFO(SBE_FUNC "HW Keys Hash Check 0x%01x",shvReq.controlData.hwKeyHashCheck);
        SBE_INFO(SBE_FUNC "ECID Check 0x%01x",shvReq.controlData.ecidCheck);
        SBE_INFO(SBE_FUNC "Matching MSV Check 0x%01x",shvReq.controlData.matchingMSVCheck);
        SBE_INFO(SBE_FUNC "MSV 0x%02x",shvReq.msv);

        // NOTE: HW ECID Field in Secure Header is not used
        // Its zeros.
        ecid_t hwEcid;
        shvReq.hwEcid = (uint32_t*)hwEcid.ecid;

        // Read SW/FW ECID from OTPROM Reg's
        ecid_t swEcid;
        swEcid.getEcid(ECID_0,ECID_1);
        shvReq.swEcid = (uint32_t*)swEcid.ecid;

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_READING_ECID_FROM_OTPROM);

        // Resopnse from secure header verification
        shvRsp_t shvRsp;

        (*gl_srom_fn_verifySecureHdr)(&shvReq, &shvRsp);

        // Update the above result into Measurement reg's
        // In case of HW Key hash/ FW key hash/ Payload hash failure we will
        // write the failing hash value into measurement reg for debug.
        // In case of Successful Secure header verification or if Secure boot
        // enforcement is disabled we calculate the hash of all 3 hash's
        truncatedHashMeasurement_t measurememtHash;
        if(shvRsp.statusCode == SHV_RC_HW_KEY_HASH_TEST)
        {
            memcpy(measurememtHash.sha3TruncatedHash, shvRsp.sha3.hwKeyHash, SHA3_TRUNCATED_SIZE);
        }
        else if(shvRsp.statusCode == SHV_RC_PREFIX_HASH_TEST)
        {
            memcpy(measurememtHash.sha3TruncatedHash, shvRsp.sha3.fwKeyHash, SHA3_TRUNCATED_SIZE);
        }
        else if(shvRsp.statusCode == SHV_RC_SW_HASH_TEST)
        {
            memcpy(measurememtHash.sha3TruncatedHash, shvRsp.sha3.payloadHash, SHA3_TRUNCATED_SIZE);
        }
        else if(shvRsp.statusCode == NO_ERROR || shvRsp.statusCode == SB_ENFORCEMENT_DISABLED)
        {
            // calculate sha3-512 (hash of boot loader hw keys | hash of boot loader fw keys | hash boot loader hash list)
            sha3_t digest;
            SBE::hash_block(shvRsp.sha3.data, sizeof(shvRsp.sha3.data), &digest);
            memcpy(measurememtHash.sha3TruncatedHash, digest, SHA3_TRUNCATED_SIZE);
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_HASH_CALCULATION_OF_FINAL_RESULT);

        measurememtHash.putSha3TruncatedHash(MEASUREMENT_REG_12,MEASUREMENT_REG_13,
                                                MEASUREMENT_REG_14,MEASUREMENT_REG_15,
                                                MEASUREMENT_REG_16,MEASUREMENT_REG_17,
                                                MEASUREMENT_REG_18,MEASUREMENT_REG_19,
                                                MEASUREMENT_REG_20,MEASUREMENT_REG_21,
                                                MEASUREMENT_REG_22,MEASUREMENT_REG_23);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_HASH_WRITE_INTO_MEASUREMENT_REG);

        // Write Status code into scratch and halt incase of failure if enforcement is enabled
        // if enforcement is disabled we write 0xff but dont halt
        SBE_INFO(SBE_FUNC "Secure Header Verification status code is [0x%02X]", shvRsp.statusCode);
        secureBootFailStatus.iv_secureHeaderFailStatusCode = shvRsp.statusCode;
        putscom_abs(scomt::perv::FSXCOMP_FSXLOG_SCRATCH_REGISTER_13_RW,secureBootFailStatus.iv_mbx13);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_STATUS_CODE_WRITE_INTO_SCRATCH);

        //Set the boot complete bit to indicate bldr measurements have be written
        //into measurement regs
        bldrSbCtrlMeasurement.bootComplete = 0x1;

        SBE_INFO(SBE_FUNC "BLDR Secure Boot Control Measurement Reg Value: 0x%08x",
                 bldrSbCtrlMeasurement.secureBootControl);
        bldrSbCtrlMeasurement.putSbCtrlData(MEASUREMENT_REG_25);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_WRITING_SECURE_BOOT_SETTINGS);

        if(shvReq.controlData.secureBootVerificationEnforcement &&
                shvRsp.statusCode != NO_ERROR && shvRsp.statusCode != SB_ENFORCEMENT_DISABLED)
        {
            SBE_ERROR(SBE_FUNC "Enforcement Enabled Halting PPE..." );
            pk_halt();
        }

        SBE_INFO("Loading SPPE main binary");
        uint32_t load_offset = SRAM_ORIGIN;
        load_image(pak, sppe_bin_fname, load_offset);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_SPPE_BINARY_LOAD);

        SBE_INFO("Loading SPPE embedded archive");
        load_image(pak, sppe_pak_fname, load_offset, LIF_IS_PAK);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_SPPE_PAK_LOAD);

        SBE_INFO("Loading optional VPD archive");
        load_image(pak, vpd_pak_fname, load_offset, LIF_IS_PAK | LIF_ALLOW_ABSENT);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_VPD_PAK_LOAD);

        // SET IVPR
        uint64_t data = (uint64_t)SRAM_ORIGIN << 32;
        PPE_STVD(0xc0000160, data);
        SBE_INFO("Launching payload");
        asm volatile ( "mtlr %0; blr" : : "r"(SRAM_ORIGIN + 0x40) : );

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR);

    }while(false);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}
