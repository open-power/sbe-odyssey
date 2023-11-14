/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/sbeCmdGetCapabilities.H $ */
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

#ifndef SBE_CMD_GETCAPABILITIES_H
#define SBE_CMD_GETCAPABILITIES_H

#include "sbeutil.H"
#include "sbe_sp_intf.H"

/**
 * - No of capabilities that supported by the SBE FW.
 * - It is used to decide the maximum number of capabilities details
 *   for the GetCapabilities and GetCodeLevels chip-ops.
 *   whenever any chip-op is added,the GETCAPABILITIES_NO_OF_CAPABILITIES
 *   must be updatable.
 */
static const uint16_t GETCAPABILITIES_NO_OF_CAPABILITIES = 9;

/**
 * - No of images count to get images details.
 * - It is used to decide the maximum number of images details
 *   for the GetCapabilities and GetCodeLevels chip-ops.
 */
static const uint16_t GETCAPABILITIES_NO_OF_IMAGES = 6;

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
 * @brief Structure for GetCapabilities response data
 */
struct __attribute__((packed)) GetCapabilityResp_t
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
     * @brief Used for the images details (that used in the running SBE FW)
     */
    ImageDetails_t iv_imageInfo[GETCAPABILITIES_NO_OF_IMAGES];

    /**
     * @brief Used for the capabilities details that contains supported
     *        chip-ops list in the running SBE FW.
     */
    uint32_t iv_capability[GETCAPABILITIES_NO_OF_CAPABILITIES];

    /**
     * @brief Explicit constructor to initialize its members.
     */
    explicit GetCapabilityResp_t():
        iv_versionMajor(SBE_FW_MAJOR_VERSION),
        iv_versionMinor(SBE_FW_MINOR_VERSION),
        iv_sbeFwReleaseTag(),
        iv_ekbFwReleaseTag(),
        iv_imageInfo(),
        iv_capability()
    {}
};

/**
 * @brief Implementation of the GetCapabilities Chipop (0xA802)
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return the RC from the FIFO access utility
 */
uint32_t sbeCmdGetCapabilities(uint8_t *i_pArg);

/**
 * @brief used to get timestamp from given offset
 *
 * @param[in] i_meta_start : address to get time stamp
 * @param[out] o_timeStamp : we will get time stamp as a output
 *                           from given address
 *
 * @return  RC
 */
uint32_t getTimeStamp(const uint8_t *i_meta_start,
                      uint32_t &o_timeStamp);

/**
 * @brief used to get commitId from given offset
 *
 * @param[in] i_meta_start : address to get commitId
 * @param[out] o_commitID : we will get commitId as a output from given address
 *
 * @return  RC
 */
uint32_t getCommitId(const uint8_t *i_meta_start,
                     uint32_t &o_commitID);
/*
* @brief This API is used to retreive information from the info.txt
*
* @param[in] i_fileName: file name corresponding to image type
* @param[in] i_capImg: enum CU_IMAGES of image type supported
* @param[out] o_identifier : commitId/version of image type from the info.txt
* @param[out] o_timeStamp : time stamp of image built from the info.txt
* @param[out] o_tag : git tag of image
*
* @return rc
*/
uint32_t loadAndParseInfoTxt(const char *i_fileName,
                             const CU_IMAGES i_capImg,
                             uint32_t &o_identifier,
                             uint32_t &o_timeStamp,
                             char (&o_tag)[BUILD_TAG_CHAR_MAX_LENGTH]);

/**
 * @brief This API is used to get tag detail of SBE image to send
 *        in the GetCapabilities response
 *
 * @param[out] o_capMsg The getcapability response buffer
 *
 * @return  RC
 */
uint32_t fillSbeTagDetails(GetCapabilityResp_t &o_capMsg);

#endif // SBE_CMD_GETCAPABILITIES_H
