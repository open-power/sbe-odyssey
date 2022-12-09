/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbecodeupdate.H $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
#ifndef SBE_CODE_UPDATE_H
#define SBE_CODE_UPDATE_H

#include "sbe_sp_intf.H"
#include "sha3.H"

#define MAX_PARTITION_TABLE_ENTRY      6
#define BUILD_TAG_CHAR_MAX_LENGTH      20
#define PARTITION_TABLE_NAME_MAX_CHAR  16
#define PARTITION_TABLE_TITLE_LEN      4

/**
 * @brief - Used to decide no of updatable image section count.
 *        - If the updatable image section count is changed then,
 *          we should add appropriate image section type in the
 *          CodeLevelsRespMsg_t::iv_updateableImagesInfo member,
 *          refer constructor.
 */
#define UPDATABLE_IMG_SECTION_CNT 4

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
             *
             * @param[in] i_imgType used to pass the type of updatable image.
             */
            explicit ImageUpdate_t(const CU_IMAGES i_imgType) :
                iv_reserved1(0),
                iv_imageType(i_imgType),
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
         * @brief 4 bytes is reserved for future needs.
         *        Also, its helps to align the size of ImageUpdate_t
         *        in multiple of 8 bytes.
         */
        uint32_t iv_reserved1;

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
            // TODO: JIRA: PFSBE-6
            // Value must be assign based on no of chip-ops that are supported
            // by the SBE FW by using chip-op json instead of hard code value.
            // Note, it should be in words like p10.
            iv_noOfCapabilities(23),
            // TODO: JIRA: PFSBE-6
            // Value must be assign based on no of sub-images that are supported
            // in the specific SBE FW project.
            iv_noOfSubImages(10),
            iv_reserved1(0),
            // If a new image section type added in below list then,
            // we should update in UPDATABLE_IMG_SECTION_CNT macro as well.
            iv_updateableImagesInfo{ImageUpdate_t(CU_IMAGES::BOOTLOADER),
                ImageUpdate_t(CU_IMAGES::RUNTIME),
                ImageUpdate_t(CU_IMAGES::BMC_OVRD),
                ImageUpdate_t(CU_IMAGES::HOST_OVRD)}
        {}
    };

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

    /*
     * @brief map for image::partition name in partition table
     */
    typedef struct
    {
        CU_IMAGES imageNum;
        char      imageName[PARTITION_TABLE_NAME_MAX_CHAR];
    } imgMap_t;

    //TODO:JIRA-PFSBE-302 Remove cust.pak
    static const imgMap_t g_imgMap[] = {
        {CU_IMAGES::BOOTLOADER, "boot"},
        {CU_IMAGES::RUNTIME,    "rt"},
        {CU_IMAGES::BMC_OVRD,   "bmc"},
        {CU_IMAGES::HOST_OVRD,  "host"}
    };

    ///////////////////////////////////////
    /////// Partition table format ////////
    ///////////////////////////////////////
    // Header: 8 bytes
    // 4 bytes - Magic word (PTBL)
    // 2 bytes - Format version
    // 2 bytes - Number of entries
    //
    // For each section entry: 24 bytes
    // 16 bytes - Section name
    // 04 bytes - Section offset
    // 04 bytes - Section max size
    //
    // Last section entry:
    // section size = minus 8 bytes to include trace and metadata pointer
    /*
     * @brief Structure for partition entry
     */
    typedef struct __attribute__((__packed__))
    {
        char     partitionName[PARTITION_TABLE_NAME_MAX_CHAR];
        uint32_t partitionStartAddr;
        uint32_t partitionSize;
    }partitionEntry_t;

    /*
     * @brief Structure for partition table
     */
    typedef struct __attribute__((__packed__))
    {
        char     partitionTitle[PARTITION_TABLE_TITLE_LEN];
        uint16_t partitionVersion;
        uint16_t numOfEntries;
        CU::partitionEntry_t partitionEntry[MAX_PARTITION_TABLE_ENTRY];
    }partitionTable_t;

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

/*@brief - sbeSyncPartition : Implement SBE Sync Partition Chipop (0xAE03)
 *
 * @param[in] - i_pArg Buffer to be passed to the function
 *
 * @return - RC from the FIFO access utility
 */
uint32_t sbeSyncPartition(uint8_t *i_pArg);

/*@brief - Get image hash for an image in NOR
 *
 * @param[in]  -const CU_IMAGES i_imageType :i_imageType passed to the function
 * @param[in]  -uint8_t i_Partition : passed to the function
 * @param[out] -uint8_t* o_hashArrayPtr :64-bytes image hash
 *
 * @return - success/ fail
 */
uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_Partition,
                      uint8_t* o_hashArrayPtr);

/**
 * @brief getPakEntryFromPartitionTable : Get pak file entry from partition table.
 *        This info can come either from partition side in nor or from image pak
 *        file containing partition table. The function returns the absolute
 *        starting address (incl. partition start address) and size of image pak
 *        file as per the partition table in nor or passed in pak file.
 *
 * @param[in]  uint8_t i_partition    : running/ non-running parition info in nor
 * @param[in]  CU_IMAGES i_imageType : enum IMAGES
 * @param[in]  void *i_pakStartAddr   : pointer to image pak start address if pak
 *                                      file containing partition table is passed
 * @param[out] partitionEntry_t *o_pakEntry : returns absolute address with size
 *
 * @return    RC success/fail
 */
uint32_t getPakEntryFromPartitionTable(const uint8_t i_partition,
                                       const CU_IMAGES i_imageType,
                                       void *i_pakStartAddr,
                                       CU::partitionEntry_t *o_pakEntry);
#endif //SBE_CODE_UPDATE_H
