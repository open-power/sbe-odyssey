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

// CU: Code Update
namespace CU
{
    /*
     * @brief Structure for images that are updatable
     */
    typedef struct
    {
        CU_IMAGES imageType;
        // hash of entire pak file (incl header minus image.hash file)
        sha3_t   imageHashSHA3;
    }imageUpdate_t;

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
        char       imageName[PARTITION_TABLE_NAME_MAX_CHAR];
    } imgMap_t;

    //TODO:JIRA-PFSBE-302 Remove cust.pak
    static const imgMap_t g_imgMap[] = {
        {CU_IMAGES::BOOTLOADER, "boot"},
        {CU_IMAGES::RUNTIME,    "rt"},
        {CU_IMAGES::BMC_OVRD,   "bmc"},
        {CU_IMAGES::HOST_OVRD,  "host"},
        {CU_IMAGES::RSVD,       "rsvd"},
        {CU_IMAGES::CUST,       "cust"}
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
