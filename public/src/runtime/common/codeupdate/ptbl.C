/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/ptbl.C $                 */
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
#include "pakwrapper.H"
#include "filenames.H"
#include "ptbl.H"

const uint32_t PARTITION_TABLE_MAGIC_WORD = 0x5054424C; //"PTBL"

uint32_t getPakEntryFromPartitionTable(const uint8_t i_sideNumber,
                                       const CU_IMAGES i_imageType,
                                       void *i_pakStartAddr,
                                       codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " getPakEntryFromPartitionTable "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    char l_sectionPakName[PARTITION_ENTRY_NAME_MAX_CHAR] = {NULL};
    CU::partitionTable_t *l_partitionTable = NULL;

    do
    {
        uint32_t l_sideStartAddress = 0;
        uint8_t  l_numOfEntries = 0;
        uint32_t l_fileSize = 0;

        // Get the side start offset
        getSideAddress(i_sideNumber,
                       l_sideStartAddress);

        // Check for valid side start address
        if (l_sideStartAddress == 0)
        {
            SBE_ERROR("Invalid side id %d. Returned side start address is zero",
                       i_sideNumber);
            l_rc = SBE_SEC_CU_INVALID_SIDE_NUMBER_PASSED;
            break;
        }

        // Init pakwrapper with either side start address or
        // pak file start address. For pak start address valid side
        // id to be passed as well
        uint32_t l_pakStart = (i_pakStartAddr == NULL ?
                              l_sideStartAddress : (uint32_t)i_pakStartAddr);
        uint32_t l_pakMaxSize = (i_pakStartAddr == NULL ?
                                io_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize :
                                (uint32_t)io_codeUpdateCtrlStruct.storageDevStruct.maxBufferSize);
        PakWrapper pak((void *)l_pakStart, (void*)(l_pakStart + l_pakMaxSize));

        // Get partition table start offset
        // Note: partition table is uncompressed.
        // This API returns start ptr only if file is uncompressed
        ARC_RET_t l_pakRc = ARC_OPERATION_SUCCESSFUL;
        uint32_t *l_filePtr = NULL;
        l_pakRc = pak.get_image_start_ptr_and_size(partition_table_file_name, &l_filePtr, &l_fileSize);
        if (l_pakRc != ARC_OPERATION_SUCCESSFUL)
        {
            // TODO:JIRA-PFSBE-300 - If file is compressed then uncompress to read it
            if (l_pakRc == ARC_FUNCTIONALITY_NOT_SUPPORTED)
            {
                SBE_ERROR(" File is compressed. Expected uncompressed file. RC:[0x%08x] ",
                           l_pakRc);
                l_rc = SBE_SEC_CU_FAILED_TO_READ_COMPRESSED_FILE;
            }
            else
            {
                SBE_ERROR(" Failed to read the partition table start offset. RC:[0x%08x] ",
                           l_pakRc);
                l_rc = SBE_SEC_CU_FAILED_TO_READ_PARTITION_TABLE;
            }
            break;
        }

        // Compare returned file size of partition table
        if (l_fileSize == 0)
        {
            SBE_ERROR(SBE_FUNC "Partition table size is zero");
            l_rc = SBE_SEC_CU_PARTITION_TABLE_IS_EMPTY;
            break;
        }

        // Get partition table entry
        l_partitionTable = (CU::partitionTable_t *)l_filePtr;
        l_numOfEntries   = l_partitionTable->numOfEntries;

        SBE_DEBUG(SBE_FUNC "Number of entries in partition table:0x%02x",
                  l_partitionTable->numOfEntries);

        // Check for matching partition magic word in partition table
        // TODO:JIRA-PFSBE-301 Check for version in partition table
        if (PARTITION_TABLE_MAGIC_WORD != *(uint32_t *)(l_partitionTable->partitionTitle))
        {
            SBE_ERROR(SBE_FUNC "Partition magic word mismatch."\
                      "Expected:[0x%08x] Original:[0x%08x]."\
                      "Partition table start addr:[0x%08x]",
                      PARTITION_TABLE_MAGIC_WORD,
                      *(uint32_t *)l_partitionTable->partitionTitle,
                      l_filePtr);
            l_rc = SBE_SEC_CU_PARTITION_MAGIC_WORD_MISMATCH;
            break;
        }

        // Based on imageType, get the equivalent pak file name in partition table
        // from the mapping structure
        uint8_t l_id = 0;
        uint8_t l_maxMapImgNameEntries = sizeof(CU::g_imgMap)/sizeof(CU::g_imgMap[0]);
        for(l_id=0; l_id < l_maxMapImgNameEntries; l_id++)
        {
            // Check for mapping name as per partition table
            if (CU::g_imgMap[l_id].imageNum == i_imageType)
            {
                strncpy(l_sectionPakName,
                        CU::g_imgMap[l_id].imageName,
                        sizeof(CU::g_imgMap[l_id].imageName));
                break;
            }
        }

        if (l_id == l_maxMapImgNameEntries)
        {
            SBE_ERROR(SBE_FUNC "Image type:[0x%04x] not found in mapping"\
                               "to partiton table", i_imageType);
            l_rc = SBE_SEC_CU_PARTITION_MAP_INVALID_IMG_TYPE;
            break;
        }

        SBE_DEBUG(SBE_FUNC "Partition info index:[%d], ImageNum:[0x%04x]",\
                 l_id, (uint16_t)CU::g_imgMap[l_id].imageNum);

        // Based on pak file name from map above, search for it in the partition table
        // if found, get the image pak's starting offset and its size
        for(l_id=0; l_id < l_numOfEntries; l_id++)
        {
            // Compare section name with pak file name expected
            if (!strcmp(l_sectionPakName,
                        l_partitionTable->partitionEntry[l_id].partitionName))
            {
                // Get absolute address for image start address in device
                io_codeUpdateCtrlStruct.imageStartAddr =
                    l_partitionTable->partitionEntry[l_id].partitionStartOffset +
                    l_sideStartAddress;
                io_codeUpdateCtrlStruct.imageSizeMax =
                    l_partitionTable->partitionEntry[l_id].partitionSize;
                break;
            }
        }

        if (l_id == l_numOfEntries)
        {
            SBE_ERROR(SBE_FUNC "Image type:[0x%04x] entry not found in partition"\
                               "table. Section pak name:[0x%08x]", i_imageType,
                               *(uint32_t *)l_sectionPakName);
            l_rc = SBE_SEC_CU_IMG_NOT_FOUND_IN_PARTITION_TBL;
            break;
        }

        SBE_INFO(SBE_FUNC "PSA:[0x%08x] PS:[0x%08x]",\
                 io_codeUpdateCtrlStruct.imageStartAddr,
                 io_codeUpdateCtrlStruct.imageSizeMax);
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/*
 * @brief checkStartAddrHighNibble : Validate higher 8-bits in incoming address
 *                                   maps to correct device address
 *
 * @param[in] i_codeUpdateCtrlStruct_t : control structure for code-update params
 *
 * @return rc
 */
static
uint32_t checkStartAddrHighNibble(codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkStartAddrHighNibble "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    if ((i_codeUpdateCtrlStruct.imageStartAddr &
            i_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddressMask) !=
            i_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress)
    {
        SBE_ERROR(SBE_FUNC "Image higher 8-bits nibble address [0x%08x] not matching "\
                  "expected address [0x%08x]",i_codeUpdateCtrlStruct.imageStartAddr,
                  i_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress);
        l_rc = SBE_SEC_CU_IMAGE_HIGH_ORDER_ADDR_MISMATCH;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/*
 * @brief checkStartAddrNSizeLessSideSize : Validate for an entry in partition table
 *                                          the sum of start address and its corressponding
 *                                          size should not exceed device side size
 *
 * @param[in] i_codeUpdateCtrlStruct_t : control structure for code-update params
 *
 * @return rc
 */
static
uint32_t checkStartAddrNSizeLessSideSize(codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkStartAddrNSizeLessSideSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    //The last 8 bytes in each side of NOR contains the PAK_END (4-bytes) and
    //size 4MB (4-bytes) so the last image in partition table would always have
    //its max size less by 8 bytes.
    if (((i_codeUpdateCtrlStruct.imageStartAddr -
                i_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress) +
            i_codeUpdateCtrlStruct.imageSizeMax) >
            (uint32_t)((i_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize *
                    (i_codeUpdateCtrlStruct.nonRunSideIndex + 1)) - 8))
    {
        SBE_ERROR(SBE_FUNC "Sum of image start address [0x%08x] and its size [0x%08x] "\
                  "more than 4MB ", i_codeUpdateCtrlStruct.imageStartAddr,\
                  i_codeUpdateCtrlStruct.imageSizeMax);
        l_rc = SBE_SEC_CU_IMG_ADDR_N_SZ_EXCEEDS_SIDE_SZ;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/*
 * @brief checkNextImgStartAddr : Validate next entry start address in partition table
 *                                matches to sum of start address and its corressponding
 *                                size in the previous entry
 *
 * @param[in] i_sideStartAddr : entry start address
 * @param[in] i_codeUpdateCtrlStruct_t : control structure for code-update params
 *
 * @return rc
 */
static
uint32_t checkNextImgStartAddr(uint32_t i_sideStartAddr,
                               codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkNextImgStartAddr "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    if (i_sideStartAddr != i_codeUpdateCtrlStruct.imageStartAddr)
    {
        SBE_ERROR(SBE_FUNC "Section start address [0x%08x] not matching "\
                  "expected start address [0x%08x]",\
                  i_sideStartAddr, i_codeUpdateCtrlStruct.imageStartAddr -
                  i_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress);
        l_rc = SBE_SEC_CU_SECTION_START_ADDR_MISMATCH;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/*
 * @brief checkSumOfSectionsSize : Validate sum of all entries size in partition
 *                                 table doesn't exceed device side size
 *
 * @param[in] i_sumOfSectionSizes : sum of all sections size
 * @param[in] i_codeUpdateCtrlStruct_t : control structure for code-update params
 *
 * @return rc
 */
static
uint32_t checkSumOfSectionsSize(uint32_t i_sumOfSectionSizes,
                                codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkSumOfSectionsSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    if (i_sumOfSectionSizes > i_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize)
    {
        SBE_ERROR(SBE_FUNC "Sum of section entries exceeds device side max size "\
                  "[0x%08x]. Sum of sections: [0x%08x]",\
                  i_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize,
                  i_sumOfSectionSizes);
        l_rc = SBE_SEC_CU_PTBL_TOTAL_IMG_SIZE_INVALID;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t validatePartitionTable(void *i_pakStartAddr,
                                codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " validatePartitionTable "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_partitionStartAddr = 0;
    uint32_t l_sumOfSectionEntriesSize = 0;

    do
    {
        for (uint8_t imgId = (uint8_t)CU_IMAGES::BOOTLOADER; imgId < (uint8_t)CU_IMAGES::END_OF_IMG_LIST; imgId++)
        {
            // Get each image type start address from the partition table
            // in the incoming pak file
            l_rc = getPakEntryFromPartitionTable(i_codeUpdateCtrlStruct.nonRunSideIndex,
                                                 (CU_IMAGES)imgId,
                                                 i_pakStartAddr,
                                                 i_codeUpdateCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "Partition read unsuccessful. RC[0x%08x]", l_rc);
                break;
            }

            // First image type is bootloader
            if (imgId == (uint16_t)CU_IMAGES::BOOTLOADER)
            {
                l_partitionStartAddr = i_codeUpdateCtrlStruct.imageStartAddr;
            }

            // Check for start address higher nibble match to device base address
            l_rc = checkStartAddrHighNibble(i_codeUpdateCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "checkStartAddrHighNibble unsuccessful. RC[0x%08x]", l_rc);
                break;
            }

            // Check for sum of start address and its size dont exceeds device side size
            l_rc = checkStartAddrNSizeLessSideSize(i_codeUpdateCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "checkStartAddrNSizeLTSideSize unsuccessful. RC[0x%08x]", l_rc);
                break;
            }

            // Check for next section image start address
            l_rc = checkNextImgStartAddr(l_partitionStartAddr, i_codeUpdateCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "checkNextImgStartAddr unsuccessful. RC[0x%08x]", l_rc);
                break;
            }

            l_partitionStartAddr += i_codeUpdateCtrlStruct.imageSizeMax;

            //TODO:JIRA:PFSBE-301 Would need to validate entire partition table
            //including new entries if any, irrespective of platform supported.
            //This is to ensure partition table design and underlying format
            //and its implementation is not broken with the new changes done in
            //partition table and also to catch for early bugs if any.
            //
            //For Odyssey we can defer this validation as partition table is
            //frozen and secondly any new entry added if any, in partition table
            //would still be allowed as its not validated (unknown to plat) and
            //so it would not be blocker for code-update even if the new entry
            //is not valid
            l_sumOfSectionEntriesSize += i_codeUpdateCtrlStruct.imageSizeMax;
        }

        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }

        // Check for sum of sections size entries dont exceed device side size
        l_rc = checkSumOfSectionsSize(l_sumOfSectionEntriesSize,
                                      i_codeUpdateCtrlStruct);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "checkSumOfSectionsSize unsuccessful. RC[0x%08x]", l_rc);
        }
    }
    while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

