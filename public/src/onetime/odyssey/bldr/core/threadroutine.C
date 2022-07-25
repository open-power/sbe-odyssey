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
#include "p11_ppe_pc.H"
#include "cmnglobals.H"

#define RUN_TIME_SH_COMPONENT_ID 0x52554E5F54494D45ull  //RUN_TIME

//We are assuming hash list can never go more than 2k
uint8_t hashList[HASH_LIST_SIZE] __attribute__((section (".hash_list"))) = {0x00};

enum load_image_flags {
    LIF_IS_PAK = 1,
    LIF_ALLOW_ABSENT = 2,
};

/// @brief Wrapper that loads an image from PNOR into SRAM and checks its hash
///
/// This function will make sure that the bootloader does not get overwritten
/// by the loaded file.
/// It will halt with an error code on any unmasked error,
/// so there is no return value.
///
/// @param[in]    i_pak                       PakWrapper targeted to PNOR
/// @param[in]    i_fname                     Name of file to load
/// @param[inout] io_load_offset              Address to load file to; will be advanced
///                                           to the end of the file after loading
/// @param[in]    i_fileHashCalculationEnable If true file hash is calculated
//                                            and compared with the hash list
//                                            value else skipped
/// @param[in]    i_flags                     Flags to control operation details
void load_image(PakWrapper &i_pak, const char *i_fname, uint32_t &io_load_offset,
        bool i_fileHashCalculationEnable = 1 , uint32_t i_flags = 0)
{
    sha3_t digest;
    uint32_t size = 0;
    uint32_t size_available = BOOTLOADER_ORIGIN - io_load_offset;

    ARC_RET_t pakRc = i_pak.read_file(i_fname, (void *)io_load_offset, size_available,
                                        (i_fileHashCalculationEnable ? &digest : NULL), &size);
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

    if(i_fileHashCalculationEnable)
    {
        auto hashListRc = SBE::check_file_hash(i_fname, digest, hashList);
        if(hashListRc != SBE::HASH_COMPARE_PASS)
        {
            SBE_ERROR(SBE_FUNC "Failed to verify payload hash");
            SBE::updateErrorCodeAndHalt(FILE_RC_PAYLOAD_HASH_VERIFICATION + hashListRc);
        }
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

    do{
        sha3_t digest;
        hwKeysHashMsv_t hwKeysHashMsv;
        uint32_t hwKeysHashMsvFileSize = 0x00;

        // Input for Secure Hdr verification
        shvReq_t shvReq;

        //Secure Container
        uint8_t *secureContainer = (uint8_t *)SROM_BLDR_PAK_SCRATCH_START;

        // Rc for pak
        ARC_RET_t pakRc = ARC_INVALID_PARAMS;

        //LFR Reg
        sbe_local_LFR lfrReg;

        // BLDR secure boot control settings
        secureBootCtrlSettings_t bldrSecureBootCtrlSettings;

        //Read LFR Reg
        PPE_LVD(scomt::ppe_pc::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_LFR_READ);

        // Get the partition start offset
        uint32_t partitionStartAddress = getAbsPartitionAddr(lfrReg.boot_selection);
        SBE_INFO(SBE_FUNC "Partition start Address is : 0x%08x, Partition selected is : 0x%02x",
                    partitionStartAddress,(uint8_t)lfrReg.boot_selection);

        PakWrapper pak((void *)partitionStartAddress);

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
        // If HRESET read previous measurement's for secure boot control settings
        if(!(SBE::isHreset()))
        {
            bldrSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_24);

            UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_READING_SROM_SETTINGS_FROM_MEASUREMENT);

            //We need to keep all settings same as SROM except for MSV and matching MSV check
            //Lets update MSV thats read from PNOR
            bldrSecureBootCtrlSettings.msv = hwKeysHashMsv.msv;

            // Matching MSV check does not apply for Runtime secure hdr
            bldrSecureBootCtrlSettings.enforceMatchingMSV = 0x0;

            //Check if SROM boot complete bit is set. If not halt
            if(bldrSecureBootCtrlSettings.bootComplete != 0x01)
            {
                SBE_ERROR(SBE_FUNC "SROM Boot complete bit 0x%01x", (uint8_t)bldrSecureBootCtrlSettings.bootComplete);
                SBE::updateErrorCodeAndHalt(BOOT_RC_SROM_COMPLETE_BIT_NOT_SET_IN_BLDR);
            }

            //Clear out the boot loader boot complete bit.
            //It will be set once measurement regs are updated with result
            bldrSecureBootCtrlSettings.bootComplete = 0x0;
        }
        else
        {
            bldrSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_25);

            UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_READING_BLDR_SETTINGS_FROM_MEAS_HRESET);

            //Check if BLDR boot complete bit is set in HRESET path. If not halt
            if(bldrSecureBootCtrlSettings.bootComplete != 0x01)
            {
                SBE_ERROR(SBE_FUNC "Boot Loader Boot complete bit 0x%01x",
                                    (bool)bldrSecureBootCtrlSettings.bootComplete);
                SBE::updateErrorCodeAndHalt(BOOT_RC_BLDR_COMPLETE_BIT_NOT_SET_IN_HRESET);
            }
        }

        SBE_INFO(SBE_FUNC "Boot Loader Secure Boot Control Measurement Reg Value: 0x%08x",
                 bldrSecureBootCtrlSettings.secureBootControl);

        //Fill in the secure header request control data
        shvReq.controlData.secureBootVerificationEnforcement = bldrSecureBootCtrlSettings.secureBootVerificationEnforcement;
        shvReq.controlData.ecdsaCheck = bldrSecureBootCtrlSettings.ecdsaSigCheckEnable;
        shvReq.controlData.dilithiumCheck = bldrSecureBootCtrlSettings.dilithiumSigCheckEnable;
        shvReq.controlData.hwKeyHashCheck = bldrSecureBootCtrlSettings.hwKeyHashVerificationEnable;
        shvReq.controlData.ecidCheck = bldrSecureBootCtrlSettings.ecidCheckEnable;
        shvReq.controlData.matchingMSVCheck = bldrSecureBootCtrlSettings.enforceMatchingMSV;
        shvReq.msv = bldrSecureBootCtrlSettings.msv;

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

        // Verify the Secure Header
        SBE_INFO(SBE_FUNC "Verifying Secure Header....");
        (*gl_srom_fn_verifySecureHdr)(&shvReq, &shvRsp);

        SBE_INFO(SBE_FUNC "Prefix Hdr Flags 0x%08x",  shvRsp.flag);

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
        else if(shvRsp.statusCode == NO_ERROR || !(shvReq.controlData.secureBootVerificationEnforcement))
        {
            // calculate sha3-512 (hash of runtime hw keys | hash of runtime fw keys | hash runtime hash list)
            sha3_t digest;
            SBE::hash_block(shvRsp.sha3.data, sizeof(shvRsp.sha3.data), &digest);
            memcpy(measurememtHash.sha3TruncatedHash, digest, SHA3_TRUNCATED_SIZE);
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_HASH_CALCULATION_OF_FINAL_RESULT);

        // Update the above result into Measurement reg's
        // Incase of HRESET dont update results into measurement instead compare the results
        // with previous measurements
        int hresetMeasurementResult = 0x0;
        if(!(SBE::isHreset()))
        {
            measurememtHash.putSha3TruncatedHash(MEASUREMENT_REG_12,MEASUREMENT_REG_13,
                                                 MEASUREMENT_REG_14,MEASUREMENT_REG_15,
                                                 MEASUREMENT_REG_16,MEASUREMENT_REG_17,
                                                 MEASUREMENT_REG_18,MEASUREMENT_REG_19,
                                                 MEASUREMENT_REG_20,MEASUREMENT_REG_21,
                                                 MEASUREMENT_REG_22,MEASUREMENT_REG_23);
        }
        else
        {
            // Read the previous measurement reg's and compare result, if mismatch halt ppe
            truncatedHashMeasurement_t previousMeasurementHash;
            previousMeasurementHash.getSha3TruncatedHash(MEASUREMENT_REG_12,MEASUREMENT_REG_13,
                                                         MEASUREMENT_REG_14,MEASUREMENT_REG_15,
                                                         MEASUREMENT_REG_16,MEASUREMENT_REG_17,
                                                         MEASUREMENT_REG_18,MEASUREMENT_REG_19,
                                                         MEASUREMENT_REG_20,MEASUREMENT_REG_21,
                                                         MEASUREMENT_REG_22,MEASUREMENT_REG_23);

            hresetMeasurementResult =
                previousMeasurementHash.compareTruncatedHash(measurememtHash.sha3TruncatedHash);
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_HASH_WRITE_OR_READ_INTO_MEASUREMENT_REG);

        //Set the boot complete bit to indicate bldr measurements have be written
        //into measurement regs
        if(!(SBE::isHreset()))
        {
            bldrSecureBootCtrlSettings.bootComplete = 0x1;

            SBE_INFO(SBE_FUNC "BLDR Secure Boot Control Measurement Reg Value: 0x%08x",
                    bldrSecureBootCtrlSettings.secureBootControl);
            bldrSecureBootCtrlSettings.putSecureBootCtrlSettings(MEASUREMENT_REG_25);

            UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_WRITING_SECURE_BOOT_SETTINGS);
        }

        // Write Status code into scratch and halt incase of failure if enforcement is enabled
        // if enforcement is disabled dont halt
        SBE_INFO(SBE_FUNC "Secure Header Verification status code is [0x%02X]", shvRsp.statusCode);
        SBE::updateErrorCode(shvRsp.statusCode);
        if(shvReq.controlData.secureBootVerificationEnforcement && shvRsp.statusCode != NO_ERROR)
        {
            SBE_INFO(SBE_FUNC "Enforcement Enabled." );
            SBE_ERROR(SBE_FUNC "Halting PPE...");
            pk_halt();
        }

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_STATUS_CODE_WRITE_INTO_SCRATCH);

        if(shvReq.controlData.secureBootVerificationEnforcement && hresetMeasurementResult != 0x0)
        {
            SBE_INFO(SBE_FUNC "Enforcement Enabled. HRESET Path.." );
            SBE_ERROR(SBE_FUNC "Previous measurement results dont match with current results" );
            SBE::updateErrorCodeAndHalt(BOOT_RC_SPPE_MEASUREMENT_MISMATCH_IN_HRESET);
        }

        //Check if scom filtering and invalid addr check need's to be enabled or disabled
        //based on imprint mode/production mode and scratch settings
        do{
            if((shvRsp.flag & IMPRINT_MODE) && (!(SBE::isHreset())))
            {
                // Read mbx16 to check if mbx11 is valid
                mbx16_t mbx16 = {0};
                getscom_abs(scomt::perv::FSXCOMP_FSXLOG_SCRATCH_REGISTER_16_RW, &mbx16.iv_mbx16);
                SBE_INFO(SBE_FUNC "MBX16  [0x%08X 0x%08X] ", SBE::higher32BWord(mbx16.iv_mbx16),
                            SBE::lower32BWord(mbx16.iv_mbx16));

                UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_MBX16_REG_READ);

                // Read mbx11 scratch
                mbx11_t mbx11 = {0};
                if(mbx16.iv_mbx11Valid != 0x1)
                {
                    SBE_INFO(SBE_FUNC "Scratch valid bit not set for MBX11 .Defaulting mbx11 to 0x00");
                    break;
                }
                else
                {
                    getscom_abs(scomt::perv::FSXCOMP_FSXLOG_SCRATCH_REGISTER_11_RW, &mbx11.iv_mbx11);
                    SBE_INFO(SBE_FUNC "MBX11  [0x%08X 0x%08X] ", SBE::higher32BWord(mbx11.iv_mbx11),
                                SBE::lower32BWord(mbx11.iv_mbx11));
                }

                UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_MBX11_REG_READ);

                //LFR Reg
                sbe_local_LFR lfrReg;

                if(mbx11.iv_disableScomFiltering)
                {
                    SBE_INFO(SBE_FUNC "Disabling SCOM Filtering in Imprint/Lab Mode" );
                    lfrReg.disable_scom_filtering = 0x1;
                }

                if(mbx11.iv_disableInvalidScomAddrCheck)
                {
                    SBE_INFO(SBE_FUNC "Disabling Imvlid Scom Address Check in Imprint/Lab Mode" );
                    lfrReg.disable_invalid_scom_addr_check = 0x1;
                }

                //Update LFR Reg. Update LFR W_OR
                PPE_STVD(scomt::ppe_pc::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_PPE1, lfrReg);

                UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_BLDR_LFR_WRITE);
            }
        }while(false);

        // Check for hash list version.
        // 1st byte of hash list is version
        if(*(hashList) != SROM_HASH_LIST_SUPPORTED_VERSION)
        {
            SBE_ERROR(SBE_FUNC "Unsupported hash list version 0x%02x", *hashList);
            SBE::updateErrorCodeAndHalt(BOOT_RC_INVALID_HASH_LIST_VERSION);
        }

        // Check for hash list hash algo.
        // 2nd byte of hash list is hash algo
        if(*(hashList + 1) != SROM_HASH_LIST_SUPPORTED_HASH_ALGO)
        {
            SBE_ERROR(SBE_FUNC "Unsupported hash list hash algorithm 0x%02x", *(uint8_t *)(hashList + 1));
            SBE::updateErrorCodeAndHalt(BOOT_RC_INVALID_HASH_LIST_HASH_ALGO);
        }

        SBE_INFO("Loading SPPE main binary");
        uint32_t load_offset = SRAM_ORIGIN;
        load_image(pak, sppe_bin_fname, load_offset,
                    bldrSecureBootCtrlSettings.fileHashCalculationEnable);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_SPPE_BINARY_LOAD);

        SBE_INFO("Loading SPPE embedded archive");
        load_image(pak, sppe_pak_fname, load_offset,
                    bldrSecureBootCtrlSettings.fileHashCalculationEnable, LIF_IS_PAK);

        UPDATE_BLDR_SBE_PROGRESS_CODE(COMPLETED_SPPE_PAK_LOAD);

        SBE_INFO("Loading optional VPD archive");
        load_image(pak, vpd_pak_fname, load_offset,
                    bldrSecureBootCtrlSettings.fileHashCalculationEnable, LIF_IS_PAK | LIF_ALLOW_ABSENT);

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
