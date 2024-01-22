/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/sbeupdateimage.C $ */
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
#include "filenames.H"
#include "sbeupdateimage.H"
#include "heap.H"
#include "archive.H"
#include "assert.h"
#include "ptbl.H"
#include "imagemap.H"

using namespace fapi2;

fapi2::ReturnCode updateImage(const CU::updateImageCmdMsg_t *i_msg,
                              const sbeFifoType i_type,
                              sbeRespGenHdr_t *o_hdr,
                              uint32_t &o_fifoRc,
                              bool &o_ackEOT)
{
    #define SBE_FUNC " updateImage "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    fapi2::current_err = FAPI2_RC_SUCCESS;

    // Request structure for update image
    codeUpdateCtrlStruct_t l_updateImgCtrlStruct __attribute__ ((aligned (8)));
    // Buffer pointer
    uint32_t *l_imgBufScratchArea = NULL;
    spi::AbstractMemoryDevice *l_memHandle = NULL;
    // Set ackEOT to false
    o_ackEOT = false;

    do
    {
        // Get incoming chip-op params updated
        l_updateImgCtrlStruct.imageType = i_msg->imageType;
        l_updateImgCtrlStruct.imageSizeInWords = i_msg->imageSizeInWords;

        // Get code-update struct parameters populated with values from plat
        getCodeUpdateParams(l_updateImgCtrlStruct);

        // Allocate buffer size
        l_imgBufScratchArea =
                    (uint32_t *)Heap::get_instance().scratch_alloc(
                            l_updateImgCtrlStruct.storageDevStruct.maxBufferSize);
        if(l_imgBufScratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed",
                      l_updateImgCtrlStruct.storageDevStruct.maxBufferSize);
            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                              SBE_SEC_HEAP_BUFFER_ALLOC_FAILED );
            break;
        }

        // Get memory device handle
        l_rc = createMemoryDevice(l_updateImgCtrlStruct.nonRunSideIndex,
                                  l_updateImgCtrlStruct.storageDevStruct.memId,
                                  true, true, l_memHandle);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC " createMemoryDevice unsuccessful. RC[0x%08x] ", l_rc);
            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
            break;
        }

        // The received image would be updated in chunks of maxSpiWriteInWords
        // or less depending on the image size in a loop
        uint32_t l_writeWordsLen = 0;
        uint32_t l_maxSpiWriteInWords = BYTES_TO_WORDS(l_updateImgCtrlStruct.storageDevStruct.maxBufferSize);
        bool l_preCheckFlag = true;
        bool l_eotFlag  = false;
        for (uint32_t l_len = i_msg->imageSizeInWords; l_len > 0; l_len -= l_writeWordsLen)
        {
            l_writeWordsLen = (l_len > l_maxSpiWriteInWords ? l_maxSpiWriteInWords : l_len);

            // Will attempt to dequeue entries based on the size passed above plus
            // the expected EOT entry at the end
            SBE_INFO(SBE_FUNC "Loop: WriteLen(bytes):[0x%08X] withECC(bytes):[0x%08X]",
                     l_writeWordsLen*sizeof(uint32_t), WITH_ECC(l_writeWordsLen*sizeof(uint32_t)));

            // For next write to fifo to get next set of data
            if (l_len == l_writeWordsLen)
            {
                // last set of data with EOT set
                l_eotFlag = true;
            }

            // Dequeing fifo along with EOT ack, set to true in last loop
            // false otherwise
            o_fifoRc = sbeUpFifoDeq_mult (l_writeWordsLen, (uint32_t *)l_imgBufScratchArea,
                                          l_eotFlag, false, i_type);

            // If FIFO access failure
            // TODO: JIRA: PFSBE-303
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(o_fifoRc);

            // check for EOT ack set above
            if (l_eotFlag)
            {
                o_ackEOT = true;
            }

            // For first pass/loop do:
            // 1. Check signature of image
            // 2. Validate partition table if image type is bootloader else if the image is updateable
            // 3. Get image detail from partition table
            // 4. Validate incoming image size
            // 5. Start on write_begin
            if (l_preCheckFlag == true)
            {
	        // 1. Verify image signature
                l_rc = checkSignature((CU_IMAGES)(i_msg->imageType),
                                      (void *)l_imgBufScratchArea,
                                      l_updateImgCtrlStruct);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "checkSignature unsuccessful. RC[0x%08x]", l_rc);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
                    break;
                }

                // 2. If image type is bootloader validate incoming partition table
                if (i_msg->imageType == (uint16_t)CU_IMAGES::BOOTLOADER)
                {
                    l_rc = validatePartitionTable((void *)l_imgBufScratchArea,
                                                  l_updateImgCtrlStruct);
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "validatePartitionTable unsuccessful. RC[0x%08x]", l_rc);
                        o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
                        break;
                    }
                } else {
		    l_rc = validateImageType((CU_IMAGES)(i_msg->imageType));
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "validateImageType unsuccessful. RC[0x%08x]", l_rc);
                        o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
                        break;
                    }

                }

                // 3. Get incoming image info data from partition table
                l_rc = getImageEntryFromPartitionTable(l_updateImgCtrlStruct.nonRunSideIndex,
                                                       (CU_IMAGES)(i_msg->imageType),
                                                       (void *)l_imgBufScratchArea,
                                                       l_updateImgCtrlStruct);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "getImageEntryFromPartitionTable unsuccessful. RC[0x%08x]", l_rc);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
                    break;
                }

                // 4. Validate incoming image size
                l_rc = validateImageSize(l_updateImgCtrlStruct);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "validateImageSize unsuccessful. RC[0x%08x]", l_rc);
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
                    break;
                }

                // 5. Start on write_begin
                SBE_INFO(SBE_FUNC "Write begin.....");
                l_fapiRc = l_memHandle->write_begin(l_updateImgCtrlStruct.imageStartAddr,
                                                   WORD_TO_BYTES(l_updateImgCtrlStruct.imageSizeInWords));
                if(l_fapiRc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC "Write begin to device failed, Addr:0x%08X Size:0x%08X",\
                              l_updateImgCtrlStruct.imageStartAddr,\
                              WORD_TO_BYTES(l_updateImgCtrlStruct.imageSizeInWords));

                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_WRITE_BEGIN_IMAGE_FAILURE );
                    break;
                }
                l_preCheckFlag = false;
            }

            if (l_eotFlag == true)
            {
                // Incoming pak file should contain the pak end marker and its size
                // towards the last 8-bytes in the incoming file so check for same.
                // Note: Calling this function before write_data so that data buffer
                // is updated with correct pak header
                l_rc = checkNUpdatePakMarkerNSize(l_imgBufScratchArea,
                                                  l_writeWordsLen,
                                                  l_updateImgCtrlStruct);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,l_rc );

                    // Clean-up by calling write functions to clear error states and
                    // restoring the tail end of a partially written erase block so
                    // it's crucial to finish the write operation to ensure we don't lose
                    // data outside of the partition we're writing
                    l_memHandle->write_end();
                    break;
                }
            }

            SBE_INFO(SBE_FUNC "Write data...");
            l_fapiRc = l_memHandle->write_data(l_imgBufScratchArea, WORD_TO_BYTES(l_writeWordsLen));
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "Write data to device failed, Addr:0x%08X Size:0x%08X",\
                          l_updateImgCtrlStruct.imageStartAddr,\
                          WORD_TO_BYTES(l_writeWordsLen));

                o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                  SBE_SEC_CU_WRITE_DATA_IMAGE_FAILURE );

                // Explanation as per LN:204-207
                l_memHandle->write_end();
                break;
            }

            if (l_eotFlag == true)
            {
                SBE_INFO(SBE_FUNC "Write end...");
                l_fapiRc = l_memHandle->write_end();
                if(l_fapiRc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC "Write end to device failed, Addr:0x%08X Size:0x%08X",\
                              l_updateImgCtrlStruct.imageStartAddr,\
                              WORD_TO_BYTES(l_writeWordsLen));

                    o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                      SBE_SEC_CU_WRITE_END_IMAGE_FAILURE );
                    break;
                }
            }

            l_updateImgCtrlStruct.imageStartAddr += WORD_TO_BYTES(l_writeWordsLen);
        } //end of for-loop

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(o_fifoRc);

        if ((l_fapiRc == FAPI2_RC_SUCCESS) &&
            (o_hdr->secondaryStatus() == SBE_SEC_OPERATION_SUCCESSFUL))
        {
            // update image successful
            SBE_INFO(SBE_FUNC "chip-op successful");
        }
    }
    while(false);

    //Free the scratch area
    Heap::get_instance().scratch_free(l_imgBufScratchArea);
    if (l_memHandle != NULL)
    {
        freeMemoryDevice(l_memHandle);
    }

    SBE_EXIT(SBE_FUNC);
    return l_fapiRc;
    #undef SBE_FUNC
}


uint32_t getImageEntryFromPartitionTable(const uint8_t i_sideNumber,
                                         const CU_IMAGES i_imageType,
                                         void *i_pakStartAddrInPibMem,
                                         codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " getImageEntryFromPartitionTable "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    l_rc = getPakEntryFromPartitionTable(
              i_sideNumber,
              i_imageType,
              (i_imageType == CU_IMAGES::BOOTLOADER ? i_pakStartAddrInPibMem : NULL),
              io_codeUpdateCtrlStruct);

    if (l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Get updated storage device image address
        io_codeUpdateCtrlStruct.imageStartAddr -=
            io_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress;

        SBE_INFO(SBE_FUNC "StartAddr:[0x%08x] StartAddr(ECC):[0x%08x]",io_codeUpdateCtrlStruct.imageStartAddr,
                 WITH_ECC(io_codeUpdateCtrlStruct.imageStartAddr));
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t validateImageSize(codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " validateImageSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    if (i_codeUpdateCtrlStruct.imageSizeInWords >
            i_codeUpdateCtrlStruct.imageSizeMax)
    {
        SBE_ERROR(SBE_FUNC "Image size passed [0x%08X] is more than size"\
                  "put up in partition table [0x%08X] ",
                  i_codeUpdateCtrlStruct.imageSizeInWords,
                  i_codeUpdateCtrlStruct.imageSizeMax);
        l_rc = SBE_SEC_CU_IMAGE_SIZE_MORE_THAN_MAX_SIZE;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t validateImageType(const CU_IMAGES i_imageType)
{
    #define SBE_FUNC " validateImageType "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    bool updateableImage = false;
    for (uint32_t i = 0; i < UPDATABLE_IMG_SECTION_CNT; i++)
    {
        if (CU::g_updatableImgList[i] == i_imageType)
        {
            updateableImage = true;
        }
    }
    if (!updateableImage)
    {
        SBE_ERROR(SBE_FUNC "Image type passed [0x%08X] is not in the list of updateable images ",
                  i_imageType);
        l_rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t checkImagePakMarker(void *i_buffer, uint32_t i_writeWordsLength)
{
    #define SBE_FUNC " checkImagePakMarker "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    // Incoming pak file should contain the pak end marker and its size
    // towards the last 8-bytes in the incoming file, check for marker here
    uint32_t l_checkForPakEndMarker =
        (uint32_t)i_buffer + WORD_TO_BYTES(i_writeWordsLength) - sizeof(PakFileHeaderCore);

    if(((PakFileHeaderCore*)l_checkForPakEndMarker)->iv_magic != PAK_END)
    {
        SBE_ERROR(SBE_FUNC "Incoming pak file doesn't contain PAK_END. Value[0x%08x]"\
                 "Offset[0x%08x]", ((PakFileHeaderCore*)l_checkForPakEndMarker)->iv_magic,
                 WORD_TO_BYTES(i_writeWordsLength));
        l_rc = SBE_SEC_CU_PAK_END_MARKER_NOT_FOUND;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t checkImagePakSize(void *i_buffer,
                           uint32_t i_writeWordsLength,
                           codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkImagePakSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    // Incoming pak file contain its size towards the last 4-bytes
    uint32_t l_checkForPakSize =
        (uint32_t)i_buffer + WORD_TO_BYTES(i_writeWordsLength) - sizeof(PakFileHeaderCore);

    if(((PakFileHeaderCore*)l_checkForPakSize)->iv_padsize !=
            WORD_TO_BYTES(io_codeUpdateCtrlStruct.imageSizeInWords))
    {
        SBE_ERROR(SBE_FUNC "Incoming pak file size [0x%08x] doesn't match its file size[0x%08x]",
                  WORD_TO_BYTES(io_codeUpdateCtrlStruct.imageSizeInWords),
                  ((PakFileHeaderCore*)l_checkForPakSize)->iv_padsize);
        l_rc = SBE_SEC_CU_FILE_SIZE_NOT_MATCHING;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t updateImagePakMarkerNSize(void *i_buffer,
                                   uint32_t i_writeWordsLength,
                                   codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " updateImagePakMarkerNSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    uint32_t l_endPakMarkerOffset =
        (uint32_t)i_buffer + WORD_TO_BYTES(i_writeWordsLength) - sizeof(PakFileHeaderCore);

    // check in the incoming pak file (with pak end & size included)
    // if its size matching the storage side size. If yes, append PAK_END
    // or else PAK_PAD towards the end of the image in storage device
    if ((i_codeUpdateCtrlStruct.imageStartAddr + WORD_TO_BYTES(i_writeWordsLength)) ==
            (uint32_t)(i_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize *
            (i_codeUpdateCtrlStruct.nonRunSideIndex + 1)))
    {
        ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_magic = PAK_END;
        ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_padsize =
            i_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize;
    }
    else
    {
        uint32_t l_remainingSpace = (i_codeUpdateCtrlStruct.imageSizeMax -
                                     WORD_TO_BYTES(i_codeUpdateCtrlStruct.imageSizeInWords));
        ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_magic = PAK_PAD;
        ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_padsize =
            l_remainingSpace;
    }

    SBE_INFO(SBE_FUNC "Pak marker:[0x%08x] Size:[0x%08x] ",\
            ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_magic,
            ((PakFileHeaderCore*)l_endPakMarkerOffset)->iv_padsize);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t checkNUpdatePakMarkerNSize(void *i_buffer,
                                    uint32_t i_writeWordsLength,
                                    codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkNUpdatePakMarkerNSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    l_rc = checkImagePakMarker(i_buffer, i_writeWordsLength);
    if (l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        l_rc = checkImagePakSize(i_buffer, i_writeWordsLength, i_codeUpdateCtrlStruct);
        if (l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
        {
            l_rc = updateImagePakMarkerNSize(i_buffer, i_writeWordsLength, i_codeUpdateCtrlStruct);
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
