/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/imgcustomize.C $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include <imgcustomize.H>
#include "sbeutil.H"
#include "errorcodes.H"
#include "measurementregs.H"
#include "sbetrace.H"
#include "poz_ppe.H"
#include "odysseylink.H"
#include "globals.H"
#include "sbeglobals.H"
#include "sberegaccess.H"
#include "pakwrapper.H"
#include "filenames.H"
#include "fapi2.H"
#include "sbestreampaktohwp.H"

using namespace fapi2;

void sbeSecurityCheckWrap(void)
{
    //Read the SROM measurement control register and validate if boot complete bit is set
    secureBootCtrlSettings_t sromSecureBootCtrlSettings;
    sromSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_24);
    SBE_INFO(SBE_FUNC "SROM Secure Boot Control Measurement Reg Value: 0x%08x",
            sromSecureBootCtrlSettings.secureBootControl);
    if(sromSecureBootCtrlSettings.bootComplete != 0x1)
    {
        SBE_ERROR(SBE_FUNC "SROM Boot Complete bit not set.");
        SBE::updateErrorCodeAndHalt(BOOT_RC_SROM_COMPLETE_BIT_NOT_SET_IN_RUNTIME);
    }

    //Read the Boot Loader measurement control register and validate if boot complete bit is set
    secureBootCtrlSettings_t bldrSecureBootCtrlSettings;
    bldrSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_25);
    SBE_INFO(SBE_FUNC "BLDR Secure Boot Control Measurement Reg Value: 0x%08x",
            bldrSecureBootCtrlSettings.secureBootControl);
    if(bldrSecureBootCtrlSettings.bootComplete != 0x1)
    {
        SBE_ERROR(SBE_FUNC "BLDR Boot Complete bit not set.");
        SBE::updateErrorCodeAndHalt(BOOT_RC_BLDR_COMPLETE_BIT_NOT_SET_IN_RUNTIME);
    }
}

void sbePakSearchStartOffset(void)
{
    // Get the partition start offset
    sbe_local_LFR lfrReg;
    PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);

    g_partitionOffset = getAbsPartitionAddr(lfrReg.boot_selection);
    g_partitionSize = NOR_SIDE_SIZE;


    SBE_INFO(SBE_FUNC "Partition offset is : 0x%08x, "
                        "Partition size is : 0x%08x, "
                        "Partition selected is : 0x%02x",
                        g_partitionOffset, g_partitionSize,
                        (uint8_t)lfrReg.boot_selection);
}

ReturnCode sbeRuntimePopulateMetadataWrap(uint32_t i_metadata_ptr)
{
    #define SBE_FUNC " sbeRuntimePopulateMetadataWrap "
    SBE_ENTER(SBE_FUNC);

    // Variable declaration
    uint8_t *l_scratchArea = nullptr;

    do
    {
        // Update commit-Id, build-date and build-tag from sbeBuildInfo.bin
        uint8_t l_runSide = SbeRegAccess::theSbeRegAccess().getBootSelection();
        uint32_t l_sideStartAddr = getAbsPartitionAddr(l_runSide);

        PakWrapper pak((void *)l_sideStartAddr, (void*)(l_sideStartAddr + NOR_SIDE_SIZE));
        uint32_t *l_filePtr = NULL, l_fileSize = 0;
        uint32_t l_rc = (uint32_t) pak.get_image_start_ptr_and_size(sbe_build_info_fname,
                                                                    &l_filePtr, &l_fileSize);

        PLAT_FAPI_ASSERT( (l_rc == SBE_SEC_OPERATION_SUCCESSFUL) && (l_fileSize != 0),
                           POZ_PAK_OPERATION_FAILED().set_PAK_RC(l_rc),
                           "get_image_start_ptr_and_size failed [%s], size: %d, RC[0x%08x]",
                           sbe_build_info_fname, l_fileSize, l_rc);

        l_scratchArea = (uint8_t *)Heap::get_instance().scratch_alloc(l_fileSize);
        PLAT_FAPI_ASSERT( (l_scratchArea != nullptr),
                           POZ_SCRATCH_ALLOC_FAILED().set_REQUIRED_SPACE(l_fileSize).
                           set_AVAILABLE_SPACE(Heap::get_instance().getFreeHeapSize()),
                           "scratch allocation request for [%d] bytes failed ", l_fileSize);

        uint32_t l_size = 0;
        sha3_t l_digest = {0};
        l_rc = (uint32_t) pak.read_file(sbe_build_info_fname, l_scratchArea, l_fileSize, &l_digest, &l_size);
        PLAT_FAPI_ASSERT( (l_rc == SBE_SEC_OPERATION_SUCCESSFUL),
                           POZ_PAK_OPERATION_FAILED().set_PAK_RC(l_rc),
                           "Failed to read image file [%s]. RC=0x%08X",
                            sbe_build_info_fname, l_rc);

         PLAT_FAPI_ASSERT( (l_fileSize == l_size),
                           POZ_PAK_OPERATION_FAILED().set_PAK_RC(SBE_SEC_IMAGE_SIZE_MISMATCH),
                           "Failed to read, Expected size[%d] actual size[%d]",
                            l_fileSize, l_size);

        // checking the image hash and validate
        // checking the image hash and validate
        SBE_FAPI_TRY(current_err, check_file_hash_and_validate(sbe_build_info_fname, l_digest),
                        SBE_FUNC " File hash valiadtion failed RC: [0x%08X]", current_err);
        // Get commit-Id
        memcpy((uint8_t *)(i_metadata_ptr + OFFSET_COMMIT_ID_IN_METADATA),
                (uint8_t *)l_filePtr, BUILD_DATE_N_COMMIT_ID_MAX_LEN_BYTE);

        // Get build-date
        memcpy((uint8_t *)(i_metadata_ptr + OFFSET_BUILD_DATE_IN_METADATA),
            (uint8_t *)(l_filePtr+1), BUILD_DATE_N_COMMIT_ID_MAX_LEN_BYTE);

        // Get build-tag
        memcpy((uint8_t *)(i_metadata_ptr + OFFSET_BUILD_TAG_IN_METADATA),
            (uint8_t *)(l_filePtr+2), BUILD_TAG_MAX_LENGTH_BYTE);


    }while (false);

    SBE_EXIT(SBE_FUNC);

fapi_try_exit:
    // free allocated scratch area
    Heap::get_instance().scratch_free(l_scratchArea);
    return fapi2::current_err;
    #undef SBE_FUNC
}