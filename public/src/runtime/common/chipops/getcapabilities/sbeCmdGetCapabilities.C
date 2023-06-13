/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/sbeCmdGetCapabilities.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "chipop_struct.H"
#include "plat_hwp_data_stream.H"
#include "globals.H"
#include "getcapabilitiesutils.H"
#include "metadata.H"
#include "pakwrapper.H"
#include "imagemap.H"
#include "sbe_build_info.H"

#define INFO_TXT_ENTRY_OPEN_DELIMITER   0x5B       // "["
#define INFO_TXT_ENTRY_END_DELIMITER    0x5D       // "]"
#define INFO_TXT_ENTRY_FIELD_SEPARATOR  0x2C       // ","

#define INFO_TXT_FORMAT_MAX_FIELD            5
#define INFO_TXT_VERSION_LENGTH              5 // Version format: vX.YZ whereX,Y,Z=[0-9]
#define INFO_TXT_IMAGE_IDENTIFIER_MAX_LENGTH 8 // Identifier as this can be commit-id or version no.
#define INFO_TXT_BUILDDATE_LENGTH            8
#define INFO_TXT_TAG_MAX_LENGTH              20

// Info.txt format:Len = 52
// [<5 chars version>,<5 chars image type>,<8 chars commit-Id>,<8 chars build date>,<20 chars tag>]
#define INFO_TXT_FORMAT_MAX_LENGTH                  \
        (1 + INFO_TXT_VERSION_LENGTH +              \
         1 + INFO_TXT_IMAGE_NAME_MAX_LENGTH +       \
         1 + INFO_TXT_IMAGE_IDENTIFIER_MAX_LENGTH + \
         1 + INFO_TXT_BUILDDATE_LENGTH +            \
         1 + INFO_TXT_TAG_MAX_LENGTH + 1)


uint32_t sbeCmdGetCapabilities(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeCmdGetCapability "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeRespGenHdr_t l_hdr;
    l_hdr.init();
    sbeResponseFfdc_t l_ffdc;
    sbeFifoType l_fifoType;
    GetCapabilityResp_t l_capRespMsg;
    chipOpParam_t* l_configStr = (struct chipOpParam*)i_pArg;
    l_fifoType = static_cast<sbeFifoType>(l_configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",l_fifoType);

    do
    {
        // Input params are not expected for GetCapability chip-op.
        // so len2dequeue will be zero
        uint32_t l_len2dequeue = 0;
        l_fifoRc = sbeUpFifoDeq_mult (l_len2dequeue, NULL,
                                      true, false, l_fifoType);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_fifoRc);

        // Fill-in SBE tag.
        // For EKB tag which is part of info.txt file, would be
        // filled as part of function fillImagesDetails.
        // The return code if not successful, will not be send out
        // to the host in the response data as this chip-op is never
        // expected to fail
        l_rc = fillSbeTagDetails(l_capRespMsg);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to fill SBE tag information, RC[0x%08x]",
                      l_rc);
        }

        // Filling image information that is used in the running SBE firmware.
        // The return code if not successful, will not be send out to the host
        // in the response data as this chip-op is never expected to fail
        l_rc = fillImagesDetails(l_capRespMsg);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to fill images information, RC[0x%08x]",
                      l_rc);
        }

        // Filling capabilities details
        fillCapabilitiesDetails(l_capRespMsg.iv_capability);

        fapi2::sbefifo_hwp_data_ostream ostream(l_fifoType);
        l_fifoRc = ostream.put((sizeof(l_capRespMsg) / sizeof(uint32_t)),
                               (uint32_t*)&l_capRespMsg);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to send GetCapability chip-op response RC[0x%08x]",
                      l_fifoRc);
            break;
        }
    } while(false);

    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(l_hdr, &l_ffdc, l_fifoType);
        if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      " Failed to send response header for getCapabilities " \
                      " RC[0x%08x]", l_rc);
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;
    #undef SBE_FUNC
}


uint32_t getTimeStamp(const uint8_t *i_meta_start,
                      uint32_t &o_timeStamp)
{
    #define SBE_FUNC " getTimeStamp "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // Get image metadata pointer.
        auto ptrLDAStruct = GET_META_DAT((uint8_t*)i_meta_start);
        if (ptrLDAStruct == NULL)
        {
            l_rc = SBE_SEC_GIT_TIME_STAMP_FAILURE;
            SBE_ERROR(SBE_FUNC "Failed to get GIT timeStamp at Addr:[%p] ",
                                i_meta_start);
            break;
        }
        o_timeStamp = ptrLDAStruct->timeStamp;

    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t getCommitId(const uint8_t *i_meta_start,
                     uint32_t &o_commitID)
{
    #define SBE_FUNC " getCommitId "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // Get image metadata pointer.
        auto ptrGITStruct = GET_META_GIT((uint8_t*)i_meta_start);
        if (ptrGITStruct == NULL)
        {
            l_rc = SBE_SEC_GIT_COMMIT_ID_FAILURE;
            SBE_ERROR(SBE_FUNC "Failed to get GIT commitId at Addr:[%p] ",
                                i_meta_start);
            break;
        }
        o_commitID = ptrGITStruct->commitId;

    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is Used to get version info of the format supported in info.txt
 *
 * @param[in] i_startPtr pointer to the beginning location of version in pibmem
 * @param[in] i_endPtr   pointer to the last location of version in pibmem
 * @param[out] o_majorVersion major version info of format in info.txt
 * @param[out] o_minorVersion minor version info of format in info.txt
 *
 * @return  RC
 */
static uint32_t getVersionFromInfoTxt(const uint8_t *i_startPtr,
                                      const uint8_t *i_endPtr,
                                      uint8_t &o_majorVersion,
                                      uint8_t &o_minorVersion)
{
    #define SBE_FUNC " getVersionFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint8_t l_size = i_endPtr - i_startPtr;

        if (l_size != INFO_TXT_VERSION_LENGTH)
        {
            // If version length it not correct then return with
            // default value for metadata
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Version should be of [%d] characters."\
                    " Rx [%d] characters as follows...",
                    INFO_TXT_VERSION_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        if ((i_startPtr[0] != 0x76) || (i_startPtr[2] != 0x2E)) // 'v' = 0x76; '.' = 0x2E
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Version format not correct."\
                               " RetVal:[0x%02x 0x%02x]", i_startPtr[0], i_startPtr[2]);
            break;
        }

        // Get major version
        l_rc = SBE::alphaNumericToHex(i_startPtr+1,
                                      1,
                                      sizeof(o_majorVersion),
                                      &o_majorVersion);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "alphaNumericToHex failed with RC[0x%08x]", l_rc);
            break;
        }

        // Get minor version
        l_rc = SBE::alphaNumericToHex(i_startPtr+3,
                                      2,
                                      sizeof(o_minorVersion),
                                      &o_minorVersion);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "alphaNumericToHex failed with RC[0x%08x]", l_rc);
            break;
        }

        SBE_INFO(SBE_FUNC "Version: Major[0x%02x] Minor[0x%02x]",
                          o_majorVersion, o_minorVersion);
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is Used to validate image type specified in info.txt
 *
 * @param[in] i_startPtr pointer to the beginning location of image type in pibmem
 * @param[in] i_endPtr   pointer to the last location of image type in pibmem
 * @param[in] i_capImg  enum CAPABILITY_IMAGES of images supported
 *
 * @return  RC
 */
static uint32_t validateImageTypeFromInfoTxt(const uint8_t *i_startPtr,
                                             const uint8_t *i_endPtr,
                                             const CAPABILITY_IMAGES i_capImg)
{
    #define SBE_FUNC " validateImageTypeFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint8_t l_size = i_endPtr - i_startPtr;

        // Validate image name max length if exceeds exit with default
        // values for metadata
        if (l_size > INFO_TXT_IMAGE_NAME_MAX_LENGTH)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Image name length greater than max [%d] characters."\
                    " Rx [%d] characters as follows...",
                    INFO_TXT_IMAGE_NAME_MAX_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        // Get capability image name corresponding to image number from map.
        // Image is always expected to be found as its internal to the code
        // and not provided as inputs from host
        char l_imgName[INFO_TXT_IMAGE_NAME_MAX_LENGTH] = {0}; // image name as per capability map
        for(uint8_t l_id=0;
            l_id < sizeof(g_getCapabilitiesImages)/sizeof(g_getCapabilitiesImages[0]);
            l_id++)
        {
            if (g_getCapabilitiesImages[l_id].imageNum == i_capImg)
            {
                strncpy(l_imgName,
                        g_getCapabilitiesImages[l_id].imageName,
                        sizeof(g_getCapabilitiesImages[l_id].imageName));
                break;
            }
        }

        // Match for image name- if it doesn't match exit with default values for metadata
        if (memcmp(i_startPtr, l_imgName, strlen(l_imgName)))
        {
            l_rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
            SBE_ERROR(SBE_FUNC "Expected image [%d] not found", i_capImg);
            PRINT_STRING_DATA("Expected: ", l_imgName, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        SBE_INFO(SBE_FUNC "Image [%d] found", i_capImg);
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is Used to get identifier (commit-Id/ version)  of image specified in info.txt
 *
 * @param[in] i_startPtr pointer to the beginning location of identifier in pibmem
 * @param[in] i_endPtr   pointer to the last location of identifier in pibmem
 * @param[in] i_capImg  enum CAPABILITY_IMAGES of images supported
 * @param[out] o_commitId commit-Id of image as per info.txt
 *
 * @return  RC
 */
static uint32_t getIdentifierFromInfoTxt(const uint8_t *i_startPtr,
                                         const uint8_t *i_endPtr,
                                         const CAPABILITY_IMAGES i_capImg,
                                         uint32_t &o_identifier)
{
    #define SBE_FUNC " getIdentifierFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint8_t l_size = i_endPtr - i_startPtr;

        // Validate identifier length if exceeds max length
        // go with default value for metadata
        if (l_size > INFO_TXT_IMAGE_IDENTIFIER_MAX_LENGTH)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Length of identifier must not be"\
                    "more than [%d] [Rx:%d]. Received values..."\
                    , INFO_TXT_IMAGE_IDENTIFIER_MAX_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        if ((i_capImg == CAPABILITY_IMAGES::EKB) && (l_size != 8))
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Length of commitId must be" \
                    " [%d] characters length [Rx:%d]." \
                    " Received values...", INFO_TXT_IMAGE_IDENTIFIER_MAX_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        l_rc = SBE::alphaNumericToHex(i_startPtr,
                                      l_size,
                                      sizeof(o_identifier),
                                      (uint8_t *)&o_identifier);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "alphaNumericToHex failed with RC[0x%08x]", l_rc);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            o_identifier = 0;
            break;
        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is Used to get build date of image specified in info.txt
 *
 * @param[in] i_startPtr pointer to the beginning location of build date in pibmem
 * @param[in] i_endPtr   pointer to the last location of build date in pibmem
 * @param[out] o_buildDat build data of image as per info.txt
 *
 * @return  RC
 */
static uint32_t getBuildDateFromInfoTxt(const uint8_t *i_startPtr,
                                        const uint8_t *i_endPtr,
                                        uint32_t &o_buildDate)
{
    #define SBE_FUNC " getBuildDateFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint8_t l_size = i_endPtr - i_startPtr;

        // Validate build date length if exceeds max length
        // go with default value for metadata
        if (l_size != INFO_TXT_BUILDDATE_LENGTH)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Length of build date must be"\
                    "equal to [%d] [Rx:%d]. Received values..."\
                    , INFO_TXT_BUILDDATE_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        l_rc = SBE::alphaNumericToHex(i_startPtr,
                                      l_size,
                                      sizeof(o_buildDate),
                                      (uint8_t *)&o_buildDate);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "alphaNumericToHex failed with RC[0x%08x]", l_rc);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            o_buildDate = 0;
            break;
        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is used to get tag of image specified in info.txt
 *
 * @param[in] i_startPtr pointer to the beginning location of tag in pibmem
 * @param[in] i_endPtr   pointer to the last location of tag in pibmem
 * @param[out] o_tag     tag of image as per info.txt
 *
 * @return  RC
 */
static uint32_t getTagFromInfoTxt(const uint8_t *i_startPtr,
                                  const uint8_t *i_endPtr,
                                  char (&o_tag)[INFO_TXT_TAG_MAX_LENGTH])
{
    #define SBE_FUNC " getTagFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint8_t l_size = i_endPtr - i_startPtr;

        // Validate tag length if exceeds max length go with
        // default value for metadata
        if (l_size > INFO_TXT_TAG_MAX_LENGTH)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Length of tag must not be"\
                    "more than [%d] [Rx:%d]. Received values..."\
                    , INFO_TXT_TAG_MAX_LENGTH, l_size);
            PRINT_STRING_DATA("Received: ", i_startPtr, l_size);
            break;
        }

        // Copy tag
        strncpy(o_tag, (char *)i_startPtr, l_size);
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief This API is Used to get metadata of image specified in info.txt
 *
 * @param[in] i_capImg  enum CAPABILITY_IMAGES of images supported
 * @param[in] i_fileStartAddr pointer to the beginning location of info.txt in pibmem
 * @param[in] i_fileSize   size of info.txt file
 * @param[out] o_commitId commit-Id of image as per info.txt
 * @param[out] o_buildDate build date of image as per info.txt
 * @param[out] o_tag tag of image as per info.txt
 *
 * @return  RC
 */
static uint32_t getMetadataFromInfoTxt(const CAPABILITY_IMAGES i_capImg,
                                       uint8_t *i_fileStartAddr,
                                       const uint32_t i_fileSize,
                                       uint32_t &o_identifier,
                                       uint32_t &o_buildDate,
                                       char (&o_tag)[INFO_TXT_TAG_MAX_LENGTH])
{
    #define SBE_FUNC " getMetadataFromInfoTxt "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    uint8_t *l_offset = NULL;
    uint8_t *l_infoTxtEntryNextFieldStartAddr = NULL;
    uint8_t l_currentEntryFieldCnt = 0;
    uint8_t *l_fileEndAddr = i_fileStartAddr + i_fileSize;

    // Info.txt file format:
    // [<Version   : vX.YZ XYZ=[0-9] 5 chars>
    //  <Image     : ekb|bmc|host 5 chars max>
    //  <Identifier: Commid-Id or version 8 chars max>
    //  <BuildDate : YYYYMMDD 8 chars>
    //  <Tag       : git tag 20 chars max>
    // ]
    // The control would break out of this function and go with default metadata values
    // if any of the below conditions are met:
    // 1. Info.txt file size is greater than format size defined
    // 2. Entry open delimiter not found
    // 3. Any field is empty
    // 4. Field size is more than defined
    // 5. Version format and its value doesn't match to expected value
    // 6. Image type is not correct
    // 7. Identifier size is not correct based on image type

    do
    {
        // Check for file size not exceeding the info.txt format max length
        if (i_fileSize > INFO_TXT_FORMAT_MAX_LENGTH)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Info.txt contents [size:%d] is more than the max "\
                               "format length [%d]", i_fileSize, INFO_TXT_FORMAT_MAX_LENGTH);
            break;
        }

        // Start with open delimiter "[" if not found exit with default values for metadata
        if (*i_fileStartAddr != INFO_TXT_ENTRY_OPEN_DELIMITER)
        {
            l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
            SBE_ERROR(SBE_FUNC "Invalid file format as entry open delimiter not "\
                               "found at filestartAdd[%p] and offset[%p]",
                               i_fileStartAddr, l_offset);
            break;
        }

        // Loop to go over the info.txt file contents in the pibmem
        for (l_offset = i_fileStartAddr + 1; l_offset < l_fileEndAddr; l_offset++)
        {
            // Next to check for end delimiter "]" or separator ","
            // for each field in info.txt file
            if ((*l_offset == INFO_TXT_ENTRY_FIELD_SEPARATOR) ||
                (*l_offset == INFO_TXT_ENTRY_END_DELIMITER))
            {
                // This check if NULL would mean an entry/field is empty
                // which is not correct as per design. Any field if not
                // populated would be entered as NA i.e. not applicable
                if (l_infoTxtEntryNextFieldStartAddr == NULL)
                {
                    l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
                    SBE_ERROR(SBE_FUNC "No field in info.txt can be empty");
                    break;
                }
                // Any field not populated should be marked as "NA".
                // Before starting to process each field value
                // check if value is NA and if found jump to next field
                if ((l_offset - l_infoTxtEntryNextFieldStartAddr) == 2)
                {
                    if (!memcmp((void *)l_infoTxtEntryNextFieldStartAddr, "NA", 2))
                    {
                        l_infoTxtEntryNextFieldStartAddr = NULL;
                        l_currentEntryFieldCnt++;
                        continue;
                    }
                }

                ////// Version //////
                if (l_currentEntryFieldCnt == 0)
                {
                    uint8_t l_majorVersion = 0, l_minorVersion = 0;

                    l_rc = getVersionFromInfoTxt(l_infoTxtEntryNextFieldStartAddr,
                                                 l_offset,
                                                 l_majorVersion,
                                                 l_minorVersion);
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "getVersionFromInfoTxt failed with RC[0x%08x]", l_rc);
                        break;
                    }

                    // Compare version with the expected version of info.txt
                    // incase of mismatch exit with default values for metadata
                    if ((l_majorVersion != INFO_TXT_MAJOR_VERSION) ||
                        (l_minorVersion != INFO_TXT_MINOR_VERSION))
                    {
                        l_rc = SBE_SEC_INFO_TXT_FORMAT_VERSION_MISMATCH;
                        SBE_ERROR(SBE_FUNC "Info.txt version mismatch for image[%d]",
                                           i_capImg);
                        SBE_ERROR(SBE_FUNC "Expected[v%d.%02x] Received[v%d.%02x]",
                                           INFO_TXT_MAJOR_VERSION, INFO_TXT_MINOR_VERSION,
                                           l_majorVersion, l_minorVersion);
                        break;
                    }
                }

                ////// Image type //////
                if (l_currentEntryFieldCnt == 1)
                {
                    l_rc = validateImageTypeFromInfoTxt(l_infoTxtEntryNextFieldStartAddr,
                                                        l_offset,
                                                        i_capImg);
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "validateImageTypeFromInfoTxt failed with RC[0x%08x]", l_rc);
                        break;
                    }
                }

                ////// Identifier - commid-Id/ version //////
                if (l_currentEntryFieldCnt == 2)
                {
                    l_rc = getIdentifierFromInfoTxt(l_infoTxtEntryNextFieldStartAddr,
                                                    l_offset,
                                                    i_capImg,
                                                    o_identifier);

                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "getIdentifierFromInfoTxt failed with RC[0x%08x]", l_rc);
                        break;
                    }
                }

                ////// Build date //////
                if(l_currentEntryFieldCnt == 3)
                {
                    l_rc = getBuildDateFromInfoTxt(l_infoTxtEntryNextFieldStartAddr,
                                                   l_offset,
                                                   o_buildDate);

                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "getBuildDateFromInfoTxt failed with RC[0x%08x]", l_rc);
                        break;
                    }
                }

                ////// Tag //////
                if (l_currentEntryFieldCnt == 4)
                {
                    l_rc = getTagFromInfoTxt(l_infoTxtEntryNextFieldStartAddr,
                                             l_offset,
                                             o_tag);

                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "getTagFromInfoTxt failed with RC[0x%08x]", l_rc);
                        break;
                    }
                }

                // If separator found move to next field
                if ((*l_offset == INFO_TXT_ENTRY_FIELD_SEPARATOR) &&
                    (l_currentEntryFieldCnt == (INFO_TXT_FORMAT_MAX_FIELD - 1)))
                {
                    l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
                    SBE_ERROR(SBE_FUNC "Extra field found in info.txt");
                    break;
                }

                // If end delimiter found and fields count exactly matches to the
                // number of field then success else exit
                if ((*l_offset == INFO_TXT_ENTRY_END_DELIMITER) &&
                    (l_currentEntryFieldCnt == (INFO_TXT_FORMAT_MAX_FIELD - 1)))
                {
                    SBE_INFO(SBE_FUNC "Successfully fetched metadata from info.txt");
                    break;
                }

                if ((*l_offset == INFO_TXT_ENTRY_END_DELIMITER) &&
                         (l_currentEntryFieldCnt < (INFO_TXT_FORMAT_MAX_FIELD - 1)))
                {
                    l_rc = SBE_SEC_INFO_TXT_FORMAT_INVALID;
                    SBE_ERROR(SBE_FUNC "Delimiter found early than expected. All values may not be correctly populated");
                    break;
                }

                l_infoTxtEntryNextFieldStartAddr = NULL;
                l_currentEntryFieldCnt++;
                continue;
            }

            if (l_infoTxtEntryNextFieldStartAddr == NULL)
            {
                l_infoTxtEntryNextFieldStartAddr = l_offset;
            }
        } // end of for loop

        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t loadAndParseInfoTxt(const char *i_fileName,
                             const CAPABILITY_IMAGES i_capImg,
                             uint32_t &o_identifier,
                             uint32_t &o_buildDate,
                             char (&o_tag)[INFO_TXT_TAG_MAX_LENGTH])
{
    #define SBE_FUNC " loadAndParseInfoTxt "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_fileSize = 0;
    uint32_t* l_filePtr = NULL;
    uint8_t *l_scratchArea = NULL;

    do
    {
        PakWrapper pak((void *)g_partitionOffset,
                       (void *)(g_partitionOffset + g_partitionSize));

        l_rc = pak.get_image_start_ptr_and_size(i_fileName, &l_filePtr, &l_fileSize);
        if (l_rc != ARC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR("Get_image_start ptr Failed with RC[0x%08X]" \
                      " fileName[%s] fileStart addr[%p] fileSize[0x%08x]",
                      l_rc, i_fileName, l_filePtr, l_fileSize);
            break;
        }
        SBE_INFO(SBE_FUNC "StartOffset:[0x%08x] Size of the binary[0x%08x]",
                 l_filePtr, l_fileSize);

        if (l_fileSize > 0)
        {
            l_scratchArea = (uint8_t *)Heap::get_instance().scratch_alloc(l_fileSize);
            if(l_scratchArea == NULL)
            {
                l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
                SBE_ERROR(SBE_FUNC " Allocation of file size[0x%08x]failed " \
                                   " RC[0x%08x]  fileName[%s]  fileSize[0x%08x]",
                                    l_rc, i_fileName, l_fileSize);
                break;
            }

            uint32_t l_size = 0;
            l_rc = pak.read_file(i_fileName, l_scratchArea, l_fileSize, NULL, &l_size);
            if( l_rc != ARC_OPERATION_SUCCESSFUL )
            {
                SBE_ERROR(SBE_FUNC "Failed to read file" \
                                   " RC[0x%08x]  fileName[%s]  fileSize[0x%08x]",
                                    l_rc, i_fileName, l_fileSize);
                break;
            }

            if (l_fileSize != l_size)
            {
                l_rc = SBE_SEC_CU_FILE_SIZE_NOT_MATCHING;
                SBE_ERROR(SBE_FUNC "Failed to read expected file size of " \
                                    "file[%s] Expected size[0x%08x]" \
                                    "actual size[0x%08x] ",
                                    i_fileName, l_fileSize, l_size);
                break;
            }

            // Get metadata for the image
            l_rc = getMetadataFromInfoTxt(i_capImg,
                                          l_scratchArea, l_size,
                                          o_identifier, o_buildDate, o_tag);
            if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "getMetadataFromInfoTxt failed."\
                                   " RC[0x%08x] FileSize[0x%08x] Identifier[0x%08x]"\
                                   " BuildDate[0x%08x]", l_rc, l_size, o_identifier,
                                   o_buildDate);
                break;
            }
        }
        else
        {
            l_rc = SBE_SEC_FILE_SIZE_IS_ZERO;
            SBE_ERROR(SBE_FUNC "File size [%d] is zero", l_fileSize);
            break;
        }
    } while (false);

    //Free the scratch area
    Heap::get_instance().scratch_free(l_scratchArea);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t fillSbeTagDetails(GetCapabilityResp_t &o_capMsg)
{
    #define SBE_FUNC " fillSbeTagDetails "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Get SBE build tag from the generated sbe_build_info.H
        const char *l_tag = SBE_BUILD_TAG;

        // Check for tag length
        if (sizeof(SBE_BUILD_TAG) > sizeof(o_capMsg.iv_sbeFwReleaseTag))
        {
            l_rc = SBE_SEC_INVALID_LENGTH_PASSED;
            SBE_ERROR(SBE_FUNC "SBE tag length [%d] exceeds max length [%d]",
                               sizeof(SBE_BUILD_TAG), BUILD_TAG_CHAR_MAX_LENGTH);
            break;
        }

        // Copy the tag into response structure member - sbeFwReleaseTag
        memcpy(o_capMsg.iv_sbeFwReleaseTag, (uint8_t *)l_tag, sizeof(SBE_BUILD_TAG));
    } while (false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
