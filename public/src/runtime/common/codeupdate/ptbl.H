/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/ptbl.H $                 */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
#ifndef __PTBL_H
#define __PTBL_H

#include "imagemap.H"

#define PARTITION_TABLE_TITLE_LEN      4

// CU: Code Update
namespace CU
{
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
        char     partitionName[PARTITION_ENTRY_NAME_MAX_CHAR];
        uint32_t partitionStartOffset;
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


/*
 * @brief getPakEntryFromPartitionTable : Get pak file entry from partition table.
 *        This info can come either from partition table in NOR or from image pak
 *        file containing partition table in pibmem. The function returns the absolute
 *        starting address (incl. partition start address) and size of image pak
 *        file as per the partition table in nor or passed in pak file.
 *
 * @param[in]  i_sideNumber : running/ non-running side number in device
 * @param[in]  i_imageType : enum IMAGES
 * @param[in]  i_pakStartAddr : pointer to incoming image pak file in case
 *             of bootloader and NULL if to be read from NOR
 * @param[inout] io_codeUpdateCtrlStruct_t : structure for code-update params
 *
 * @return    RC success/fail
 */
uint32_t getPakEntryFromPartitionTable(const uint8_t i_sideNumber,
                                       const CU_IMAGES i_imageType,
                                       void *i_pakStartAddr,
                                       codeUpdateCtrlStruct_t &io_codeUpdateReqStruct);

/*
 * @brief validatePartitionTable : Validate partition table if incoming image
 *                                 is bootloader pak file
 *
 * @param[in] i_pakStartAddr : pointer to buffer containing incoming pak file
 * @param[in] i_codeUpdateCtrlStruct_t : structure for code-update params
 *
 * @return rc
 */
uint32_t validatePartitionTable(void *i_pakStartAddr,
                                codeUpdateCtrlStruct_t &i_codeUpdateReqStruct);

#endif //PTBL_H
