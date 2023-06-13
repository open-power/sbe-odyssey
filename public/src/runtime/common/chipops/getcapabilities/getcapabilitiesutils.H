/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/getcapabilitiesutils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef GETCAPABILITIES_UTILS_H
#define GETCAPABILITIES_UTILS_H

#include "sbeCmdGetCapabilities.H"

#define GET_IMAGE_DETAILS_FROM_INFO_TXT(i_imageFileName, i_imageType, i_imageInfo,       \
                                        o_identifier, o_buildDate, o_tag, o_rc)          \
    i_imageInfo.iv_imageType = i_imageType;                                              \
    o_identifier = 0; o_buildDate = 0;                                                   \
    o_rc = loadAndParseInfoTxt(i_imageFileName, i_imageType, o_identifier,               \
                               o_buildDate, o_tag);                                      \
    if (o_rc != SBE_SEC_OPERATION_SUCCESSFUL)                                            \
    {                                                                                    \
        SBE_ERROR(SBE_FUNC " loadAndParseInfoTxt failed for image [%d] from [%s]. "      \
                           "RC[0x%08x]", i_imageType, i_imageFileName, o_rc);            \
        break;                                                                           \
    }                                                                                    \
    i_imageInfo.iv_identifier = o_identifier;                                            \
    i_imageInfo.iv_buildTime = o_buildDate;

// Structure map for maintaining image name corresponding to image number.
// Its usage is project specific defined by global variable - g_getCapabilitiesImages
#define INFO_TXT_IMAGE_NAME_MAX_LENGTH       5
typedef struct
{
    CAPABILITY_IMAGES imageNum;
    char              imageName[INFO_TXT_IMAGE_NAME_MAX_LENGTH];
} capImgMap_t;

/**
 * @brief Used to get images details to send in the GetCapabilities response
 *
 * @param[out] o_capMsg The getcapability response buffer
 *
 * @return  RC
 */
uint32_t fillImagesDetails(GetCapabilityResp_t &o_capMsg);

/**
 * @brief Used to get capabilities details to send in the GetCapabilities
 *        response
 *
 * @param[out] o_capability The pointer to capability array
 *
 * @return void
 */
void fillCapabilitiesDetails(uint32_t* o_capability);

#endif
