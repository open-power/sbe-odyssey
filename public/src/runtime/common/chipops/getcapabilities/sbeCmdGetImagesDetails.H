/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/sbeCmdGetImagesDetails.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#pragma once

#include "sbeutil.H"
#include "sbe_sp_intf.H"
#include "capabilities.H"

/**
 * - No of images count to get images details.
 * - It is used to decide the maximum number of images details
 *   for the GetCapabilities and GetCodeLevels chip-ops.
 */
static constexpr uint16_t GETCAPABILITIES_NO_OF_IMAGES = 6;

/**
 * @brief Structure for GetCapabilities image details
 */
struct __attribute__((packed)) ImageDetails_t
{
    /**
     * @brief 2 bytes is reserved for future needs.
     *        Also, its helps to align with the FIFO a word (aka 4bytes)
     */
    uint16_t iv_reserved1;

    /**
     * @brief Used to maintain the image type
     */
    CU_IMAGES iv_imageType; // uint16_t

    /**
     * @brief Used to maintain the time stamp of an image
     *        Format- YYYYMMDD
     */
    uint32_t iv_buildTime;

    /**
     * @brief Used to maintain the commit id or the data version id
     *        depend on the image type.
     */
    uint32_t iv_identifier;

    /**
     * @brief Explicit constructor to initialize its members.
     */
    explicit ImageDetails_t() :
        iv_reserved1(0),
        iv_imageType(CU_IMAGES::IMG_TYPE_INVALID),
        iv_buildTime(0),
        iv_identifier(0)
    {}
};

/**
 * @brief Structure for GetImagesDetails response data
 */
struct __attribute__((packed)) GetImagesDetailsResp_t
{
    /**
     * @brief Used for the SBE FW major number
     */
    uint16_t iv_versionMajor;

    /**
     * @brief Used for the SBE FW minor number
     */
    uint16_t iv_versionMinor;

    /**
     * @brief Used for the SBE FW tag
     */
    uint8_t iv_sbeFwReleaseTag[BUILD_TAG_CHAR_MAX_LENGTH];

    /**
     * @brief Used for the EKB FW tag
     */
    uint8_t iv_ekbFwReleaseTag[BUILD_TAG_CHAR_MAX_LENGTH];

    /**
     * @brief Number of image infos in iv_imageInfo
     */
    uint32_t iv_numImageInfos;

    /**
     * @brief Used for the images details (that used in the running SBE FW)
     */
    ImageDetails_t iv_imageInfo[GETCAPABILITIES_NO_OF_IMAGES];


    /**
     * @brief Explicit constructor to initialize its members.
     */
    explicit GetImagesDetailsResp_t():
        iv_versionMajor(SBE_FW_MAJOR_VERSION),
        iv_versionMinor(SBE_FW_MINOR_VERSION),
        iv_sbeFwReleaseTag(),
        iv_ekbFwReleaseTag(),
        iv_numImageInfos(GETCAPABILITIES_NO_OF_IMAGES),
        iv_imageInfo()
    {}
};


/**
 * @brief Implementation of the GetImagesDetails Chipop (0xA803)
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return the RC from the FIFO access utility
 */
uint32_t sbeCmdGetImagesDetails(uint8_t *i_pArg);
