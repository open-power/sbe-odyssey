/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/getcapabilitiesutils.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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

#include "filenames.H"
#include "metadata.H"
#include "globals.H"
#include "odysseylink.H"
#include "pakwrapper.H"
#include "sbe_sp_intf.H"
#include "heap.H"
#include "imagemap.H"
#include "getcapabilitiesutils.H"

uint32_t fillImagesDetails(GetCapabilityResp_t &o_capMsg)
{
    #define SBE_FUNC " fillImagesDetails "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Iterating the loop to get all the images properties.
        for (uint8_t l_img = 0;
             l_img < (sizeof(g_getCapabilitiesImages) /
                      sizeof(g_getCapabilitiesImages[0]));
             l_img++)
        {
            uint32_t l_identifier = 0, l_timeStamp = 0, l_commitId = 0;

            switch (g_getCapabilitiesImages[l_img].imageNum)
            {
                case CAPABILITY_IMAGES::SROM:
                    {
                        // Update the image type as SROM.
                        o_capMsg.iv_imageInfo[l_img].iv_imageType =
                                                    CAPABILITY_IMAGES::SROM;

                        // Calling function to get commitID
                        l_rc = getCommitId((uint8_t*)(SROM_ORIGIN +
                                                      SROM_VECTOR_SIZE),
                                                      l_commitId);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " failed to get commitId of an "
                                               " imageType[%d], at Offset[0x%08x] "
                                               " RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                (SROM_ORIGIN + SROM_VECTOR_SIZE), l_rc);
                            break;
                        }
                        // Update the commit id.
                        o_capMsg.iv_imageInfo[l_img].iv_identifier = l_commitId;

                        //TODO: JIRA: PFSBE-415: Fill SROM build timestamp.
                    }
                    break;

                case CAPABILITY_IMAGES::BOOTLOADER:
                    {
                        l_timeStamp = 0;
                        l_commitId = 0;

                        // Update the image type as BOOTLOADER
                        o_capMsg.iv_imageInfo[l_img].iv_imageType =
                                                CAPABILITY_IMAGES::BOOTLOADER;

                        PakWrapper pak((void *)g_partitionOffset,
                                       (void *)(g_partitionOffset + g_partitionSize));

                        uint32_t* l_filePtr = NULL;
                        l_rc = pak.get_image_start_ptr_and_size(bldr_file_name,
                                                                &l_filePtr);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " Pak get image start pointer Failed "
                                               " imageType[%d], RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                l_rc);
                            break;
                        }

                        // start offset to get commitId and timeStamp
                        uint8_t *l_startOffset =  ((uint8_t*)l_filePtr + VECTOR_SIZE);

                        // Calling function to get commitID
                        l_rc = getCommitId(l_startOffset, l_commitId);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " failed to get commitId of an "
                                               " imageType[%d], at Offset[%p] "
                                               " RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                l_startOffset, l_rc);
                            break;
                        }
                        // Update the commit id
                        o_capMsg.iv_imageInfo[l_img].iv_identifier = l_commitId;

                         // Calling function to get time stamp
                        l_rc = getTimeStamp(l_startOffset, l_timeStamp);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " failed to get timeStamp of an "
                                               " imageType[%d], at Offset[%p] "
                                               " RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                l_startOffset, l_rc);
                            break;
                        }
                        // Update the time stamp.
                        o_capMsg.iv_imageInfo[l_img].iv_buildTime = l_timeStamp;
                    }
                    break;

                case CAPABILITY_IMAGES::RUNTIME:
                    {
                        l_timeStamp = 0;
                        l_commitId = 0;

                        // Update the image type as RUNTIME.
                        o_capMsg.iv_imageInfo[l_img].iv_imageType =
                                                    CAPABILITY_IMAGES::RUNTIME;

                        // Calling function to get commit id
                        l_rc = getCommitId((uint8_t*)(SRAM_ORIGIN +
                                                      VECTOR_SIZE),
                                                      l_commitId);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " failed to get commitId of an "
                                               " imageType[%d], at Offset[0x%08x] "
                                               " RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                (SRAM_ORIGIN + VECTOR_SIZE), l_rc);
                            break;
                        }
                        // Update the commit id.
                        o_capMsg.iv_imageInfo[l_img].iv_identifier = l_commitId;

                        // Calling function to get time stamp
                        l_rc = getTimeStamp((uint8_t*)(SRAM_ORIGIN +
                                                       VECTOR_SIZE),
                                                       l_timeStamp);
                        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            SBE_ERROR(SBE_FUNC " failed to get timeStamp of an "
                                               " imageType[%d], at Offset[0x%08x] "
                                               " RC[0x%08x] ",
                                                o_capMsg.iv_imageInfo[l_img].iv_imageType,
                                                (SRAM_ORIGIN + VECTOR_SIZE), l_rc);
                            break;
                        }
                        // Update the time stamp.
                        o_capMsg.iv_imageInfo[l_img].iv_buildTime = l_timeStamp;
                    }
                    break;

                case CAPABILITY_IMAGES::BMC_OVRD:
                    GET_IMAGE_DETAILS_FROM_INFO_TXT(bmc_info_file_name, CAPABILITY_IMAGES::BMC_OVRD,\
                                                    o_capMsg.iv_imageInfo[l_img], l_identifier, l_timeStamp, l_rc);
                    break;

                case CAPABILITY_IMAGES::HOST_OVRD:
                    GET_IMAGE_DETAILS_FROM_INFO_TXT(host_info_file_name, CAPABILITY_IMAGES::HOST_OVRD,\
                                                    o_capMsg.iv_imageInfo[l_img], l_identifier, l_timeStamp, l_rc);
                    break;

                case CAPABILITY_IMAGES::EKB:
                    GET_IMAGE_DETAILS_FROM_INFO_TXT(ekb_info_file_name, CAPABILITY_IMAGES::EKB,\
                                                    o_capMsg.iv_imageInfo[l_img], l_identifier, l_timeStamp, l_rc);
                    break;

                default:
                    // Update the image type as invalid.
                    l_rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
                    SBE_ERROR(SBE_FUNC "Invalid Image type[%d]",
                              o_capMsg.iv_imageInfo[l_img].iv_imageType);
                    break;
            }
            SBE_INFO(SBE_FUNC "ImageType[%d], TimeStamp[0x%08x], Identifier[0x%08x]",
                               o_capMsg.iv_imageInfo[l_img].iv_imageType,
                               o_capMsg.iv_imageInfo[l_img].iv_buildTime,
                               o_capMsg.iv_imageInfo[l_img].iv_identifier);
        }
    } while (false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


void fillCapabilitiesDetails(uint32_t *o_capability)
{
    o_capability[GENERIC_CAPABILTITY_START_IDX] =
                                                HWP_FFDC_COLLECTION_SUPPPORTED |
                                                SBE_FFDC_COLLECTION_SUPPPORTED |
                                                FIFO_RESET_SUPPPORTED |
                                                PIPES_SUPPPORTED ;

    o_capability[IPL_CAPABILITY_START_IDX] =
                                            EXECUTE_ISTEP_SUPPPORTED |
                                            EXECUTE_HWP_SUPPPORTED;

    o_capability[SCOM_CAPABILITY_START_IDX] =
                                            GET_SCOM_SUPPPORTED |
                                            PUT_SCOM_SUPPPORTED ;

    o_capability[RING_CAPABILITY_START_IDX] =
                                            GET_RING_SUPPPORTED |
                                            PUT_RING_SUPPPORTED |
                                            PUT_RING_FROM_IMAGE_SUPPPORTED ;

    o_capability[ARRAY_CAPABILITY_START_IDX] =
                                            CONTROL_FAST_ARRAY_SUPPPORTED |
                                            CONTROL_TRACE_ARRAY_SUPPPORTED ;

    o_capability[GENERIC_CHIPOP_CAPABILITY_START_IDX] =
                                                    GET_SBE_FFDC_SUPPPORTED |
                                                    GET_CAPABILITIES_SUPPORTED;

    o_capability[STOP_CLOCKS_CAPABILITY_START_IDX] = STOP_CLOCKS_SUPPORTED;

    o_capability[ATTRIBUTE_CAPABILITY_START_IDX] = GET_RAWATTR_DUMP_SUPPORTED |
                                                   LIST_ATTR_SUPPORTED |
                                                   UPDATE_ATTR_SUPPORTED ;

    o_capability[CODEUPDATE_CAPABILITY_START_IDX] = GET_CODE_LEVELS_SUPPORTED |
                                                    UPDATE_IMAGE_SUPPORTED |
                                                    SYNC_SIDE_SUPPORTED;
}

