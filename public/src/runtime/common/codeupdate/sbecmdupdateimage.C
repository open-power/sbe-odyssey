/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/sbecmdupdateimage.C $    */
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
#include "filenames.H"
#include "poz_sbe_spi_cmd.H"
#include "sbecodeupdate.H"
#include "codeupdateutils.H"
#include "sbecmdupdateimage.H"
#include "heap.H"
#include "archive.H"

/////////////////////////////////////////////////////
// Update image chip-op flow
// -------------------------
// 1.  Validate incoming image type and its size
// 2.  Get non-running partition info (which side to update image)
// 3.  If incoming image is other than bootloader:
// 3.1 Get the image start offset & max size from parition table in
//     non-running partition in nor
// 3.2 Validate the incoming image size against image max size
// 4.  Get max space available in heap. Make sure minimum 1KB space available
// 5.  Allocate buffer from heap(per step-4).Start receiving image data from fifo
// 6.  If incoming image is bootloader:
// 6.1 Get all images sizes from incoming partition table in buffer
// 6.2 Check for total images sizes not exceeding 4MB
// 6.3 Get booloader start offset & max size from incoming partition table in data
// 6.4 Validate incoming bootloader size against image max size in incoming
//     paritition table
// 7.  Erase sectors/blocks in nor as per incoming image size
// 8.  Perform write incoming image file into non-running partition in nor
// 9.  Repeat steps 5 to 7 till image is fully written in nor
//     Step 6 executed only once
// 10. Write pak pad/end towards the end of image written in nor
/////////////////////////////////////////////////////
fapi2::ReturnCode updateImage(const updateImageCmdMsg_t *i_msg,
                              const sbeFifoType i_type,
                              sbeRespGenHdr_t *o_hdr,
                              uint32_t &o_fifoRc,
                              bool &o_ackEOT)
{
    #define SBE_FUNC " updateImage "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;

    // Set ackEOT to false
    o_ackEOT = false;
    // Variable for running and non-running partition in nor
    uint8_t l_nonRunPartIndex, l_runPartIndex;
    // structure for partition entry
    CU::partitionEntry_t l_partitionEntry;
    // Buffer used to collect data from fifo
    uint32_t *ImgBufScratchArea __attribute__ ((aligned (8))) = {0};
    // Variable to get image.pak details from partition table
    uint32_t l_imageStartAddr = 0;
    uint32_t l_imageSizeMax  = 0;
    uint32_t l_writeWordsLen = 0;
    bool l_eraseDone = false;
    uint32_t maxSpiWriteInWords = 0;

    // This is for testing bootloader update image
    bool l_checkBootUpdateFlag = false;

    do
    {
        // Get partition info in nor
        getPartitionInfo(l_runPartIndex, l_nonRunPartIndex);

        // From partition table in non-running side get the image start
        // address and size for images other than bootloader as for bootloader
        // we need to check for incoming partition table before updating the
        // bootloader image into non-running side
        if (i_msg->imageType != (uint16_t)CU_IMAGES::BOOTLOADER)
        {
            // Get image start addr in nor
            l_rc = getPakEntryFromPartitionTable(l_nonRunPartIndex,
                                                 (CU_IMAGES)(i_msg->imageType),
                                                 NULL,
                                                 0,
                                                 &l_partitionEntry);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "Partition read unsuccessful. RC[0x%08x]", l_rc);
                o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
                break;
            }
            l_imageSizeMax    = l_partitionEntry.partitionSize;
            l_imageStartAddr  = l_partitionEntry.partitionStartAddr;

            // As nor is 16MB 24-bits used to address it to pass on to the spi
            // driver for various operations - read/write/erase
            // TODO:PFSBE-311 project specific macros
            l_imageStartAddr &= 0x00FFFFFF;

            // Check for image size received before starting to write into nor
            if (IN_BYTES(i_msg->imageSizeInWords) > l_partitionEntry.partitionSize)
            {
                SBE_ERROR(SBE_FUNC "Image size passed [0x%08X] is more than size"\
                          "put up in partition table [0x%08X] ",
                          IN_BYTES(i_msg->imageSizeInWords), l_partitionEntry.partitionSize);
                o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                  SBE_SEC_CU_IMAGE_SIZE_MORE_THAN_MAX_SIZE );
                break;
            }

            SBE_INFO(SBE_FUNC "ImageStartAddr:0x%08X Size(max):0x%08X",
                    l_imageStartAddr, l_imageSizeMax);
        }

        // Initialize SPI controller to get PIB SPI base address
        SpiControlHandle spiHandle(g_platTarget->plat_getChipTarget(), 0);

        // Get max free scratch area for receiving image data
        size_t getMaxBufSize = Heap::get_instance().getFreeHeapSize();
        if (getMaxBufSize < IN_BYTES(MIN_SPI_WRITE_IN_WORDS))
        {
            SBE_ERROR(SBE_FUNC "Mininum heap space of 0x%08x not available. MaxSize[0x%zx]",
                      IN_BYTES(MIN_SPI_WRITE_IN_WORDS), getMaxBufSize);
            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                              SBE_SEC_HEAP_SPACE_FULL_FAILURE );
            break;
        }
        else if (getMaxBufSize % WORD_ALIGNED)
        {
            getMaxBufSize -= (getMaxBufSize % WORD_ALIGNED);
        }

        SBE_INFO(SBE_FUNC "Buffer available(bytes):[0x%zX]",getMaxBufSize);

        // Get max spi write data buffer
        maxSpiWriteInWords = (getMaxBufSize/WORD_LENGTH);

        // Allocate buffer size
        ImgBufScratchArea =
                    (uint32_t*)Heap::get_instance().scratch_alloc(getMaxBufSize);
        if(ImgBufScratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed",
                      getMaxBufSize);
            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                              SBE_SEC_HEAP_SPACE_FULL_FAILURE );
            break;
        }

        // The received image would be updated in chunks of maxSpiWriteInWords
        // or less depending on the image size in a loop
        // Note: The start address and size for each image in partition table
        // is logical and not calculated with ECC byte included so while
        // calculating the effective address/size to write/read into nor
        // the value need to multiplied by 9 and divided by 8 respectively
        for (uint32_t l_len = i_msg->imageSizeInWords; l_len > 0; l_len -= l_writeWordsLen)
        {
            l_writeWordsLen = (l_len / maxSpiWriteInWords > 0 ? maxSpiWriteInWords : l_len);

            // Will attempt to dequeue entries based on the size passed above plus
            // the expected EOT entry at the end
            // NOTE: Will change to SBE_DEBUG as needed later
            SBE_INFO(SBE_FUNC "Loop: WriteLen(bytes):[0x%08X] withECC(bytes):[0x%08X]",
                     (l_writeWordsLen*sizeof(uint32_t)),WITH_ECC(l_writeWordsLen*sizeof(uint32_t)));

            // For next write to fifo to get next set of data
            bool l_setEOT = false;
            if (l_len == l_writeWordsLen)
            {
                // last set of data with EOT set
                l_setEOT = true;
            }

            o_fifoRc = sbeUpFifoDeq_mult (l_writeWordsLen, (uint32_t *)ImgBufScratchArea,
                                      l_setEOT, false, i_type);

            // If FIFO access failure
            // TODO: JIRA: PFSBE-303
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(o_fifoRc);

            // check for EOT
            if (l_setEOT)
            {
                o_ackEOT = true;
            }

            // For bootloader image we need to check for incoming
            // partition table and subsequently verifying the combined size for
            // all image incl. bootloader file before updating the bootloader
            // image into non-running side
            if (i_msg->imageType == (uint16_t)CU_IMAGES::BOOTLOADER && !l_checkBootUpdateFlag)
            {
                uint32_t l_sumOfSectionEntriesSize = 0;
                // TODO:PFSBE-311 for using END_OF_IMG_LIST
                for (uint8_t imgId = 1; imgId < (uint8_t)CU_IMAGES::END_OF_IMG_LIST; imgId++)
                {
                    // Get each image type start address from the partition table
                    // in the incoming pak file
                    l_rc = getPakEntryFromPartitionTable(l_nonRunPartIndex,
                                                         (CU_IMAGES)imgId,
                                                         (void *)ImgBufScratchArea,
                                                         l_writeWordsLen,
                                                         &l_partitionEntry);

                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "Partition read unsuccessful. RC[0x%08x]", l_rc);
                        o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
                        break;
                    }

                    l_sumOfSectionEntriesSize += l_partitionEntry.partitionSize;

                    // Get bootloader start addr in nor
                    if (imgId == (uint16_t)CU_IMAGES::BOOTLOADER)
                    {
                        l_imageSizeMax    = l_partitionEntry.partitionSize;
                        l_imageStartAddr  = (l_partitionEntry.partitionStartAddr & 0x00FFFFFF);
                    }
                }

                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    break;
                }

                if (l_sumOfSectionEntriesSize > NOR_PARTITION_SIZE)
                {
                    SBE_ERROR(SBE_FUNC "Sum of section entries exceeds NOR side max size "\
                              "[0x%08x]. Sum of sections: [0x%08x]",\
                              NOR_PARTITION_SIZE, l_sumOfSectionEntriesSize);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_EXCEEDS_MAX_SIDE_IMAGE_SIZE );
                    break;
                }

                // Check for image size before starting to write into nor
                if (IN_BYTES(i_msg->imageSizeInWords) > l_imageSizeMax)
                {
                    SBE_ERROR(SBE_FUNC "Image size passed [0x%08X] is more than size"\
                              "put up in partition table [0x%08X] ",
                              i_msg->imageSizeInWords, l_imageSizeMax);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_IMAGE_SIZE_MORE_THAN_MAX_SIZE );
                    break;
                }

                SBE_INFO(SBE_FUNC "ImageStartAddr(wo ECC):[0x%08X] Size(max wo ECC):[0x%08X]",
                        l_imageStartAddr, l_imageSizeMax);

                // Make sure flag is set
                l_checkBootUpdateFlag = true;
            }

            // Before starting to write into nor, make sure to erase entire
            // requisite number of sectors/blocks as per the image size
            // to be updated.
            // Note: If incoming image is less than the previous one the partition
            // may contain junk data beyond the latest updated image
            if (!l_eraseDone)
            {
                // Note: both start and end address for erase operation
                // must be 4K aligned with ECC byte included
                uint32_t l_eraseStartAddr = WITH_ECC(l_imageStartAddr) &
                                            NOR_FLASH_SECTOR_BOUNDARY_ALIGN;
                uint32_t l_eraseEndAddr   = WITH_ECC(l_imageStartAddr + IN_BYTES(i_msg->imageSizeInWords) - 1);
                if ((l_eraseEndAddr & NOR_FLASH_SECTOR_BOUNDARY_CHECK_MASK) != 0)
                {
                    l_eraseEndAddr &= NOR_FLASH_SECTOR_BOUNDARY_ALIGN;
                    l_eraseEndAddr = l_eraseEndAddr + NOR_FLASH_SECTOR_SIZE - 1;
                }
                else
                {
                    l_eraseEndAddr -= 1;
                }

                SBE_INFO(SBE_FUNC "EraseStartAddr(with ECC):[0x%08X] EraseEndAddr(with ECC):[0x%08X]",
                        l_eraseStartAddr, l_eraseEndAddr);

                l_fapiRc = spi_erase_and_no_preserve(spiHandle,
                                                   l_eraseStartAddr,
                                                   l_eraseEndAddr);
                if(l_fapiRc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC "Error: erase opreration failed. "\
                              "Start address:[0x%08x] End address:[0x%08X]. RC[0x%08X]",
                              l_eraseStartAddr, l_eraseEndAddr, l_fapiRc);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_UPDATE_IMAGE_FAILURE );
                    break;
                }

                // Make sure flag is set
                l_eraseDone = true;
            }

            SBE_DEBUG(SBE_FUNC "Addr:0x%08X Size(bytes):0x%08X",
                      WITH_ECC(l_imageStartAddr), IN_BYTES(l_writeWordsLen));

            if (l_setEOT)
            {
                // Incoming pak file should contain the pak end marker and its size
                // towards the last 8-bytes in the incoming file so check for same
                uint32_t l_checkForPakEndNSize =
                         (uint32_t)ImgBufScratchArea + IN_BYTES(l_writeWordsLen) - sizeof(PakFileHeaderCore);
                if (*(uint32_t *)(l_checkForPakEndNSize) != PAK_END)
                {
                    SBE_ERROR(SBE_FUNC "Incoming pak file doesn't contain PAK_END. Value[0x%08x]",
                              *(uint32_t *)l_checkForPakEndNSize);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_PAK_END_MARKER_NOT_FOUND );
                    break;
                }

                if (*(uint32_t *)(l_checkForPakEndNSize + 4) != IN_BYTES(i_msg->imageSizeInWords))
                {
                    SBE_ERROR(SBE_FUNC "Incoming pak file size [0x%08x] doesn't match its file size [0x%08x]",
                              IN_BYTES(i_msg->imageSizeInWords), *(uint32_t *)(l_checkForPakEndNSize + 4));
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_FILE_SIZE_NOT_MATCHING );
                    break;
                }

                // Before updating the pak pad, check in the incoming pak file (with pak end & size included)
                // if its size matching the nor partition side. If yes, append PAK_END or PAK_PAD towards the
                // end of the image in nor
                if ((l_imageStartAddr + IN_BYTES(l_writeWordsLen)) ==
                        (uint32_t)(NOR_PARTITION_SIZE * (l_nonRunPartIndex + 1)))
                {
                    *(uint32_t *)l_checkForPakEndNSize = PAK_END;
                    *(uint32_t *)(l_checkForPakEndNSize + 4) = NOR_PARTITION_SIZE;
                }
                else
                {
                    uint32_t l_remainingSpace = (l_imageSizeMax - IN_BYTES(i_msg->imageSizeInWords));
                    *(uint32_t *)l_checkForPakEndNSize = PAK_PAD;
                    *(uint32_t *)(l_checkForPakEndNSize + 4) = l_remainingSpace;
                }

                SBE_INFO(SBE_FUNC "Pak marker:[0x%08x] Size:[0x%08x] ",\
                         *(uint32_t *)l_checkForPakEndNSize, *(uint32_t *)(l_checkForPakEndNSize + 4));
            }

            SBE_INFO(SBE_FUNC "WriteStartAddr(wo ECC):[0x%08X] WriteWordsLen(wo ECC):[0x%08X]",
                     l_imageStartAddr, IN_BYTES(l_writeWordsLen));

            // Perform SPI write
            l_fapiRc = spi_write_ecc(spiHandle,
                                     l_imageStartAddr,
                                     IN_BYTES(l_writeWordsLen),
                                     (uint8_t *)ImgBufScratchArea,
                                     true);

            // SPI write failed
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "SPI write with ecc failed, Addr:0x%08X Size:0x%08X"\
                          "Error:0x%016llX", WITH_ECC(l_imageStartAddr), IN_BYTES(l_writeWordsLen),\
                          (uint64_t)l_fapiRc);
                o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                  SBE_SEC_CU_UPDATE_IMAGE_FAILURE );
                break;
            }

            l_imageStartAddr += IN_BYTES(l_writeWordsLen);
        } //for (uint32_t l_len = i_msg->imageSizeInWords; l_len > 0; l_len -= l_writeWordsLen)

        if ((l_fapiRc == FAPI2_RC_SUCCESS) &&
            (o_hdr->secondaryStatus() == SBE_SEC_OPERATION_SUCCESSFUL))
        {
            // update image successful
            SBE_INFO(SBE_FUNC "chip-op successful");
        }
    }
    while(false);

    //Free the scratch area
    Heap::get_instance().scratch_free(ImgBufScratchArea);

    SBE_EXIT(SBE_FUNC);
    return l_fapiRc;
    #undef SBE_FUNC
}
