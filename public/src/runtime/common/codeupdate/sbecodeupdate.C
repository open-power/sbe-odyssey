/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbecodeupdate.C $        */
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
#include "sbecmdupdateimage.H"
#include "sbecodeupdate.H"
#include "codeupdateutils.H"
#include "chipop_struct.H"
#include "sbe_sp_intf.H"
#include "pakwrapper.H"
#include "filenames.H"
#include "sbeutil.H"
#include "sbe_sp_intf.H"
#include "plat_hwp_data_stream.H"
#include "target.H"

using namespace fapi2;

const uint32_t partition_table_magic_word = 0x5054424C; //"PTBL"

////////////////////////////////////////////////////////
//////// Get Code Levels Chip-op ///////////////////////
////////////////////////////////////////////////////////
uint32_t sbeGetCodeLevels (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetCodeLevels "
    SBE_ENTER(SBE_FUNC);

    // Use only for FIFO utility API
    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* l_configStr((struct chipOpParam*)i_pArg);
    sbeFifoType l_fifoType(static_cast<sbeFifoType>(l_configStr->fifoType));
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]", l_fifoType);

    sbeRespGenHdr_t l_hdr;
    l_hdr.init();
    do
    {
        // Input params are not expected for GetCodeLevels chip-op.
        // But, we need to read EOT from the upstream fifo to acknowledge
        // the chip-op. (expect EOT and don't flush)
        uint32_t l_ipLen = 0;
        l_fifoRc = sbeUpFifoDeq_mult(l_ipLen, NULL, true, false, l_fifoType);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Don't set the secondary rc for the FIFO error since we won't
            // send the response header for the FIFO error.
            // Refer below at the last in this function to get more details.
            SBE_ERROR(SBE_FUNC \
                      "Failed to ack EOT for GetCodeLevels chip-op, RC[0x%08x]",
                      l_fifoRc);
            break;
        }

        uint8_t l_runningPart;
        uint8_t l_nonRunningPart; // unused now
        getPartitionInfo(l_runningPart, l_nonRunningPart);

        // Use only for secondary rc to send in the response header
        uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
        CU::CodeLevelsRespMsg_t l_codeLevelsRespMsg;
        for (uint8_t i = 0; i < UPDATABLE_IMG_SECTION_CNT; i++)
        {
            l_rc = getImageHash(l_codeLevelsRespMsg.iv_updateableImagesInfo[i].
                                    iv_imageType,
                                l_runningPart,
                                l_codeLevelsRespMsg.iv_updateableImagesInfo[i].
                                    iv_imageHashSHA3_512);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC \
                          "Failed to get image hash, "
                          "RC[0x%08x] RunningSide[%d] ImageType[%d]",
                          l_rc, l_runningPart, l_codeLevelsRespMsg.
                             iv_updateableImagesInfo[i].iv_imageType);
                l_hdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
                break;
            }
        }
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }

        fapi2::sbefifo_hwp_data_ostream ostream(l_fifoType);
        l_fifoRc = ostream.put((sizeof(l_codeLevelsRespMsg) / sizeof(uint32_t)),
                               (uint32_t*)&l_codeLevelsRespMsg);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Don't set the secondary rc for the FIFO error since we won't
            // send the response header for the FIFO error.
            // Refer below at the last in this function to get more details.
            SBE_ERROR(SBE_FUNC \
                      "Failed to send GetCodeLevelsRespMsg, RC[0x%08x]",
                      l_fifoRc);
            break;
        }
    } while(false);

    /**
     * @note TODO: JIRA: PFSBE-303
     *       - Don't send the response header for the FIFO error, just return
     *         FIFO error code and the command processor will go into infinite
     *         loop and the caller will hit a timeout and halt the SBE,
     *         collect SBE dumps, and does the necessary actions (Restart SBE/
     *         HRESET/ReIPL) to fix the FIFO error.
     *       - The chip-op backend assume any error from FIFO utils API
     *         as a FIFO error. Currently, no rules to decide which rc is a FIFO
     *         error because the logic spreaded across common code infra
     *         so it need to change to simplify the chip-op backend code
     *         responsibility.
     */
    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // FIXME Currently, No FAPI ffdc in this chip-op implementation.
        l_fifoRc = sbeDsSendRespHdr(l_hdr, NULL, l_fifoType);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Nothing can be done here, just add a trace and return rc.
            SBE_ERROR(SBE_FUNC \
                      "Failed to send response header for GetCodeLevels "
                      "chip-op, RC[0x%08x]", l_fifoRc);
        }
    }

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
    return l_fifoRc;
}

////////////////////////////////////////////////////////
//////// Update Image Chip-op //////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeUpdateImage (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeUpdateImage "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    updateImageCmdMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    bool ackEOT = false;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Will attempt to dequeue two entries for the update image
        // plus the expected EOT entry at the end
        uint32_t len2dequeue = sizeof(msg)/sizeof(uint32_t);
        fifoRc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, false, false, type);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        // Get image type and its size to update in nor
        SBE_INFO(SBE_FUNC "ImageType: 0x%04x , ImageSize(bytes): 0x%08x",
                 msg.imageType, IN_BYTES(msg.imageSizeInWords));

        // Check for validity of image type
        if (msg.imageType != (uint16_t)CU_IMAGES::BOOTLOADER &&
            msg.imageType != (uint16_t)CU_IMAGES::RUNTIME &&
            msg.imageType != (uint16_t)CU_IMAGES::BMC_OVRD &&
            msg.imageType != (uint16_t)CU_IMAGES::HOST_OVRD)
        {
            // TODO:PFSBE-311 for using END_OF_IMG_LIST
            if (msg.imageType == (uint16_t)CU_IMAGES::IMG_TYPE_INVALID ||
                msg.imageType >= (uint16_t)CU_IMAGES::END_OF_IMG_LIST)
            {
                SBE_ERROR(SBE_FUNC "Image type invalid: [0x%04x]", msg.imageType);
                hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_CU_INVALID_IMAGE_TYPE );
                break;
            }
        }

        // Check for validity of image size
        if (msg.imageSizeInWords == 0)
        {
            SBE_ERROR(SBE_FUNC "Invalid image size zero received");
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_CU_INVALID_IMAGE_SIZE );
            break;
        }
        else if (IN_BYTES(msg.imageSizeInWords) % 8 != 0)
        {
            SBE_ERROR(SBE_FUNC "Image size [0x%08x] not 8 byte aligned", msg.imageSizeInWords);
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_CU_IMAGE_SIZE_NOT_8BYTE_ALIGNED );
            break;
        }

        // call update image chip-op function
        fapiRc = updateImage(&msg, type, &hdr, fifoRc, ackEOT);
        if (hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            if(fapiRc != FAPI2_RC_SUCCESS)
            {
                ffdc.setRc(fapiRc);
            }
            break;
        }
        else
        {
            if(fapiRc != FAPI2_RC_SUCCESS)
            {
                ffdc.setRc(fapiRc);
                hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_CU_UPDATE_IMAGE_FAILURE );
                break;
            }
        }
    } while(false);

    do
    {
        if ((hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL) &&
            (ackEOT == false))
        {
            // flush out fifo
            uint32_t len2dequeue = 0;
            fifoRc = sbeUpFifoDeq_mult (len2dequeue, NULL, true, true, type);
        }
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);
        // Build the response header packet
        fifoRc = sbeDsSendRespHdr(hdr, &ffdc, type);
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return fifoRc;
#undef SBE_FUNC
}

////////////////////////////////////////////////////////
//////// Sync Partition Chip-op ////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeSyncPartition (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeSyncPartition "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    //TODO:PFSBE-21 to be uncommented during implementation
    //uint32_t fapiRc = fapi2::FAPI2_RC_SUCCESS;
    //syncPartitionRespMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

#if 0   //TODO: to be enabled during implementation
        //No attempt to read FIFO as no input params expected
        // call sync partition function
        fapiRc = syncPartition(&msg, type);
        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = CU_RC_SYNC_PARTITION_HWP_FAILURE;
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
            ffdc.setRc(l_rc);
            break;
        }
#endif
    } while(false);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC"Failed. rc[0x%X]",l_rc);
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_Partition,
                      uint8_t* o_hashArrayPtr)
{
    #define SBE_FUNC " getImageHash "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ARC_RET_t l_pakRC = ARC_OPERATION_SUCCESSFUL;
    uint32_t l_size = 0;
    uint32_t l_partitionStartAddress = 0;

    do
    {
        //To get partition start address
        getPartitionAddress(i_Partition,l_partitionStartAddress);
        PakWrapper pak((void *)l_partitionStartAddress, (void *)(l_partitionStartAddress + NOR_PARTITION_SIZE));

        switch (i_imageType)
        {
            case CU_IMAGES::BOOTLOADER:
                l_pakRC = pak.read_file(bldr_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::RUNTIME:
                l_pakRC = pak.read_file(runtime_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::BMC_OVRD:
                l_pakRC = pak.read_file(bmc_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::HOST_OVRD:
                l_pakRC = pak.read_file(host_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            default:
                l_rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
                SBE_ERROR(SBE_FUNC " Invalid Image  Passed by caller image: %d ",
                                     i_imageType);
                break;
        }

        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        if (l_pakRC != ARC_OPERATION_SUCCESSFUL)
        {
            l_rc = SBE_SEC_CU_FILE_IMAGE_HASH_READ_ERROR;
            SBE_ERROR(SBE_FUNC " ImageType: %d " \
                      "Failed to read image hash Rc:%d",i_imageType,l_pakRC);
            break;
        }

        if (SHA3_DIGEST_LENGTH != l_size)
        {
            l_rc = SBE_SEC_CU_IMAGE_HASH_SIZE_MISMATCH;
            SBE_ERROR(SBE_FUNC "Failed to read expected hash size of image:%d" \
                               "Expected size: %d,actual size: %d ",
                                i_imageType,SHA3_DIGEST_LENGTH,l_size);
            break;
        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t getPakEntryFromPartitionTable(const uint8_t i_partition,
                                       const CU_IMAGES i_imageType,
                                       void *i_pakStartAddr,
                                       uint32_t i_pakBufSize,
                                       CU::partitionEntry_t *o_pakEntry)
{
    #define SBE_FUNC " getPakEntryFromPartitionTable "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    char l_sectionPakName[PARTITION_TABLE_NAME_MAX_CHAR] = {NULL};
    CU::partitionTable_t *l_partitionTable = NULL;

    do
    {
        uint32_t l_partitionStartAddress = 0;
        uint8_t  l_numOfEntries = 0;
        uint32_t l_fileSize = 0;

        // Get the partition start offset
        getPartitionAddress(i_partition, l_partitionStartAddress);

        // Check for valid partition start address
        if (l_partitionStartAddress == 0)
        {
            SBE_ERROR("Invalid partition id %d. Returned partition start address is zero",
                       i_partition);
            l_rc = SBE_SEC_CU_INVALID_PARTITION_ID_PASSED;
            break;
        }

        // Init pakwrapper with either partition start address or
        // pak file start address. For pak start address valid partition
        // id to be passed as well
        uint32_t l_pakStart = (i_pakStartAddr == NULL ? l_partitionStartAddress : (uint32_t)i_pakStartAddr);
        uint32_t l_pakMaxSize = (i_pakStartAddr == NULL ? NOR_PARTITION_SIZE : (uint32_t)i_pakBufSize);
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
        if (partition_table_magic_word != *(uint32_t *)(l_partitionTable->partitionTitle))
        {
            SBE_ERROR(SBE_FUNC "Partition magic word mismatch."\
                      "Expected:[0x%08x] Original:[0x%08x]."\
                      "Partition table start addr:[0x%08x]",
                      partition_table_magic_word,
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
                strcpy(l_sectionPakName,
                       CU::g_imgMap[l_id].imageName);
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
                strcpy(o_pakEntry->partitionName,
                       l_partitionTable->partitionEntry[l_id].partitionName);
                // Get absolute address for image start address in NOR
                o_pakEntry->partitionStartAddr =
                    l_partitionTable->partitionEntry[l_id].partitionStartAddr +
                    l_partitionStartAddress;
                o_pakEntry->partitionSize      =
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

        SBE_INFO(SBE_FUNC "PSN:[0x%08x] PSA:[0x%08x] PS:[0x%08x]",\
                 *(uint32_t *)o_pakEntry->partitionName,
                 o_pakEntry->partitionStartAddr,
                 o_pakEntry->partitionSize);
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
