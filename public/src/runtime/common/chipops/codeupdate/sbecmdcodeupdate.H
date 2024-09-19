/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/sbecmdcodeupdate.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2024                        */
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
#ifndef SBE_CMD_CODE_UPDATE_H
#define SBE_CMD_CODE_UPDATE_H

#include "sbe_sp_intf.H"
#include "sha3.H"
#include "imagemap.H"
#include "sbeSpMsg.H"
#include "sbeCmdGetCapabilities.H"

// CU: Code Update
namespace CU
{
    /**
     * @brief Structure for GetCodeLevels chip-op response
     */
    struct __attribute__((packed, aligned(8))) CodeLevelsRespMsg_t
    {
        /**
         * @brief Structure for images that are updatable
         */
        struct __attribute__((packed, aligned(8))) ImageUpdate_t
        {
            /**
             * @brief 2 bytes is reserved for future needs.
             *        Also, its helps to align with the FIFO a word (aka 4bytes)
             */
            uint16_t iv_reserved1;

            /**
             * @brief Used to maintain the updatable image section type.
             */
            CU_IMAGES iv_imageType;

            /**
             * @brief 4 bytes is reserved for future needs.
             *        Also, its helps to align the size of ImageUpdate_t
             *        in multiple of 8 bytes.
             */
            uint32_t iv_reserved2;

            /**
             * @brief Used to maintain the hash of updatable image section.
             */
            sha3_t iv_imageHashSHA3_512;

            /**
             * @brief Explicit constructor to initialize its members.
             */
            explicit ImageUpdate_t() :
                iv_reserved1(0),
                iv_imageType(CU_IMAGES::IMG_TYPE_INVALID),
                iv_reserved2(0),
                iv_imageHashSHA3_512()
            {}
        };

        /**
         * @brief - No of chip-ops that supported by the SBE FW.
         *        - It is used to decide the buffer size to collect
         *          capabilities details from the GetCapabilities chip-op.
         */
        uint16_t iv_noOfCapabilities;

        /**
         * @brief - No of sub-images that supported by the SBE FW.
         *        - It is used to decide the buffer size to collect
         *          sub-images details from the GetCapabilities chip-op.
         */
        uint16_t iv_noOfSubImages;

        /**
         * @brief Used to display SBE FW major version
         */
        uint16_t iv_versionMajor;

        /**
         * @brief Used to display SBE FW minor version
         */
        uint16_t iv_versionMinor;

        /**
         * @brief Buffer to respond image (aka section i.e bldr, rt, bmc,
         *        and host) hash and image type respectively.
         */
        ImageUpdate_t iv_updateableImagesInfo[UPDATABLE_IMG_SECTION_CNT];

        /**
         * @brief Used to override default constructor to initialize
         *        its members with the expected value to frame the response.
         */
        CodeLevelsRespMsg_t() :
            iv_noOfCapabilities(GETCAPABILITIES_NO_OF_CAPABILITIES),
            iv_noOfSubImages(GETCAPABILITIES_NO_OF_IMAGES),
            // Major version must be update as per poz specs
            iv_versionMajor(SBE_FW_MAJOR_VERSION),
            // Minor version must be update as per poz specs
            iv_versionMinor(SBE_FW_MINOR_VERSION),
            iv_updateableImagesInfo()
        {}
    };

    /*
     * @brief structure for update image chip-op
     */
    typedef struct __attribute__((__packed__))
    {
        uint16_t reserved;
        uint16_t imageType;
        uint32_t imageSizeInWords;

        /*
         * @brief Validate the input parameters
         *
         * @param[in] image type
         * @param[in] image size
         *
         * @return rc
         */
        uint32_t checkInputParamsValid()
        {
            uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;

            do
            {
                // Check for validity of image type
                // As we continue to maintain commonality of images across platforms
                // (unless broken) will just check for validity of image being valid
                // and during update image respective platform can raise error if
                // not supported
                if (imageType == (uint16_t)CU_IMAGES::IMG_TYPE_INVALID ||
                    imageType >= (uint16_t)CU_IMAGES::END_OF_CU_IMAGES)
                {
                    rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
                    break;
                }

                // Check for validity of image size
                if (imageSizeInWords == 0)
                {
                    rc = SBE_SEC_CU_INVALID_IMAGE_SIZE;
                    break;
                }
                else if (WORD_TO_BYTES(imageSizeInWords) % 8 != 0)
                {
                    rc = SBE_SEC_CU_IMAGE_SIZE_NOT_8BYTE_ALIGNED;
                    break;
                }
            }while(false);

            return rc;
        }

    }updateImageCmdMsg_t;


    /*
     * @brief Structure for image properties
     */
    typedef struct __attribute__((__packed__))
    {
        CU_IMAGES  imageType;
        uint32_t   buildTime; // Format- YYYYMMDD
        uint32_t   commidId;
        char       buildTag[BUILD_TAG_CHAR_MAX_LENGTH];
        uint32_t   reserved[2];
    }imageProperties_t;

} // namespace CU


/*@brief - sbeGetCodeLevels : Implement SBE Get Code Levels Chipop (0xAE01)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeGetCodeLevels(uint8_t *i_pArg);

/*@brief - sbeUpdateImage : Implement SBE Update Image Chipop (0xAE02)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeUpdateImage(uint8_t *i_pArg);

/*@brief - sbeSyncSide : Implement SBE sync chip-op (0xAE03)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeSyncSide(uint8_t *i_pArg);

/**
 * @brief checkImageHashMismatch : comparing the image hash of an image from
 *                                 runSide to nonRunSide
 *
 * @param[in]  - i_syncSideCtrlStruct struct used to contains various
 *               parameters needed for sync chip-op
 * @param[out] - o_hdr: Response header object
 *
 * @return:      false - for image hash mismatch
 *               true - for image match
 */
bool checkImageHashMismatch(codeUpdateCtrlStruct_t &i_syncSideCtrlStruct,
                            sbeRespGenHdr_t *o_hdr);

#endif //SBE_CMD_CODE_UPDATE_H
