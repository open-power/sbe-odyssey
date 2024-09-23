/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdcodeupdateall.C $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
#include "sbecmdcodeupdate.H"
#include "imgcustomize.H"
#include "sbeupdateimage.H"
#include "codeupdateutils.H"
#include "chipop_struct.H"
#include "sbeutil.H"
#include "plat_hwp_data_stream.H"
#include "ptbl.H"

using namespace fapi2;

// Break if both the sides has failed to update.
#define CHECK_UPDATE_IMG_CTRL_STRUCTURE_AND_BREAK_IF_BOTH_SIDES_FAILED(updateImgCtrlStruct) \
    if ((updateImgCtrlStruct.goldenStruct.returnCodeSide[0] != SBE_SEC_OPERATION_SUCCESSFUL) && \
        (updateImgCtrlStruct.goldenStruct.returnCodeSide[1] != SBE_SEC_OPERATION_SUCCESSFUL)) \
    { \
        break; \
    }

/*
 * Handle FAPI error by-
 * Loging the error, updating the side return code,
 * committing the ffdc and clearing the current error status
 */
#define HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_writeWordsLen, l_errorLog, l_sideStatus) \
    SBE_ERROR(l_errorLog " device at side %d failed, Addr:0x%08X Size:0x%08X", \
             l_side, \
             getSideCurrentOffset(l_updateImgCtrlStruct, l_side), \
             WORD_TO_BYTES(l_writeWordsLen)); \
    l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] = l_sideStatus; \
    logFatalError(l_fapiRc); \
    CLEAR_FAPI2_CURRENT_ERROR(); \

/*
 * @brief getSideCurrentOffset: API is used to send current offset of side under execution.
 *
 * @param[in] i_codeUpdateReqStruct struct used to contain start addresses of sides
 * @param[in] i_side side under execution for which current offset is seeked
 *
 * @return current offset of side under execution
 */
static
uint32_t getSideCurrentOffset(codeUpdateCtrlStruct_t &i_codeUpdateReqStruct, uint8_t i_side)
{
    #define SBE_FUNC " getSideCurrentOffset "
    SBE_ENTER(SBE_FUNC);
    uint32_t sideCurrentOffset = 0;
    if(i_side == 0)
    {
        sideCurrentOffset = i_codeUpdateReqStruct.imageStartAddr;
    }
    else if (i_side == 1)
    {
        sideCurrentOffset = (i_codeUpdateReqStruct.imageStartAddr +
                             i_codeUpdateReqStruct.storageDevStruct.storageDevSideSize);
    }
    else
    {
        // Side other than 0 & 1 not expected so raise assert
        SBE_ERROR(SBE_FUNC "Invalid side: [0x%02x]", i_side);
        assert(false);
    }
    SBE_EXIT(SBE_FUNC);
    return sideCurrentOffset;
    #undef SBE_FUNC
}

/*
 * @brief checkUpdateImageStatus : Check on image update status on side-0 & 1
 *                                 from golden side
 *
 * @param[in] i_codeUpdateCtrlStruct struct used to contains various
 *            parameters needed for code-update
 *
 * @return rc
 */
static
uint32_t checkUpdateImageStatus(codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " checkUpdateImageStatus "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc  = SBE_SEC_OPERATION_SUCCESSFUL;

    uint32_t l_rc0 = i_codeUpdateCtrlStruct.goldenStruct.returnCodeSide[0];
    uint32_t l_rc1 = i_codeUpdateCtrlStruct.goldenStruct.returnCodeSide[1];

    if ((l_rc0 != SBE_SEC_OPERATION_SUCCESSFUL) && (l_rc1 != SBE_SEC_OPERATION_SUCCESSFUL))
    {
        l_rc = SBE_SEC_CU_BOTH_SIDES_UPDATE_FAILED;
        SBE_ERROR(SBE_FUNC "chip-op to both sides failed");
    }
    else if ((l_rc0 == SBE_SEC_OPERATION_SUCCESSFUL) && (l_rc1 != SBE_SEC_OPERATION_SUCCESSFUL))
    {
        l_rc = SBE_SEC_CU_SIDE_1_UPDATE_FAILED;
        SBE_ERROR(SBE_FUNC "chip-op to side-1 failed");
    }
    else if ((l_rc0 != SBE_SEC_OPERATION_SUCCESSFUL) && (l_rc1 == SBE_SEC_OPERATION_SUCCESSFUL))
    {
        l_rc = SBE_SEC_CU_SIDE_0_UPDATE_FAILED;
        SBE_ERROR(SBE_FUNC "chip-op to side-0 failed");
    }
    else
    {
        SBE_INFO(SBE_FUNC "chip-op to both sides successful");
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

/*
 * @brief updateImageGolden : Get image updated from golden to both side-0/1
 *
 * @param[in] updateImageCmdMsg_t msg : structure for update image
 * @param[in] sbeFifoType type : fifo type
 * @param[out] uint32_t fifoRc : rc for fifo access failure
 * @param[out] bool ackEOT : flag for EOT received or not
 *
 * @return    rc
 */
static
uint32_t updateImageGolden(const CU::updateImageCmdMsg_t *i_msg,
                           const sbeFifoType i_type,
                           uint32_t &o_fifoRc,
                           bool &o_ackEOT)
{
    #define SBE_FUNC " updateImageGolden "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;

    // Code-update structure for update image
    codeUpdateCtrlStruct_t l_updateImgCtrlStruct __attribute__ ((aligned (8)));
    // Buffer pointer
    uint32_t *l_imgBufScratchArea = NULL;
    spi::AbstractMemoryDevice *l_memHandle[2] = {NULL,NULL};
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
            l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
            break;
        }

        // Get memory device handle for side 0 and side 1
        for(uint8_t l_side=0; l_side<2; l_side++)
        {
            l_rc = createMemoryDevice(l_updateImgCtrlStruct.nonRunSideIndex,
                                    l_updateImgCtrlStruct.storageDevStruct.memId,
                                    true, true, l_memHandle[l_side]);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC " createMemoryDevice for side %d unsuccessful. RC[0x%08x] ",
                          l_side, l_rc);
                l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] = l_rc;
            }
        }

        // If both side operation is failed break and come out
        CHECK_UPDATE_IMG_CTRL_STRUCTURE_AND_BREAK_IF_BOTH_SIDES_FAILED(l_updateImgCtrlStruct);

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
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(o_fifoRc);

            // check for EOT ack set above
            if (l_eotFlag)
            {
                o_ackEOT = true;
            }

            // For first pass/loop do:
            // 1. Check signature of image
            // 2. Validate partition table:
            //      if image type is bootloader else if the image is updateable
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
                        break;
                    }
                }
                else
                {
                    l_rc = validateImageType((CU_IMAGES)(i_msg->imageType));
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "validateImageType unsuccessful. RC[0x%08x]", l_rc);
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
                    SBE_ERROR(SBE_FUNC "getImageEntryFromPartitionTable unsuccessful. RC[0x%08x]",
                             l_rc);
                    break;
                }

                // 4. Validate incoming image size
                l_rc = validateImageSize(l_updateImgCtrlStruct);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "validateImageSize unsuccessful. RC[0x%08x]", l_rc);
                    break;
                }

                // 5. Start on write_begin on both side-0 and 1

                for(uint8_t l_side=0; l_side<2; l_side++)
                {
                    uint32_t l_sideAddress = getSideCurrentOffset(l_updateImgCtrlStruct, l_side);
                    if(l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] == SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_INFO(SBE_FUNC "Write begin Side %d .....",l_side);
                        l_fapiRc = l_memHandle[l_side]->write_begin(l_sideAddress,
                                        WORD_TO_BYTES(l_updateImgCtrlStruct.imageSizeInWords));
                        if(l_fapiRc != FAPI2_RC_SUCCESS)
                        {
                            HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_updateImgCtrlStruct.imageSizeInWords,
                                SBE_FUNC "Write begin to",
                                SBE_SEC_CU_WRITE_BEGIN_IMAGE_FAILURE);
                        }
                    }
                }

                // If both side operation is failed break and come out
                CHECK_UPDATE_IMG_CTRL_STRUCTURE_AND_BREAK_IF_BOTH_SIDES_FAILED(l_updateImgCtrlStruct);

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
                    // Clean-up by calling write functions to clear error states and
                    // restoring the tail end of a partially written erase block so
                    // it's crucial to finish the write operation to ensure we don't lose
                    // data outside of the partition we're writing
                    for(uint8_t l_side=0; l_side<2; l_side++)
                    {
                        if(l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] == SBE_SEC_OPERATION_SUCCESSFUL)
                        {
                            l_fapiRc = l_memHandle[l_side]->write_end();
                            if(l_fapiRc != FAPI2_RC_SUCCESS){
                                HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_writeWordsLen,
                                 SBE_FUNC "Clearing Error States and Restoring the Tail End of",
                                 SBE_SEC_CU_WRITE_END_IMAGE_FAILURE);
                            }
                        }
                    }
                    break;
                }
            }

            // Writing Data on Side 0 and side 1.......
            for(uint8_t l_side=0; l_side<2; l_side++)
            {
                if(l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] == SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_INFO(SBE_FUNC "Write data side %d ....",l_side);
                    l_fapiRc = l_memHandle[l_side]->write_data(l_imgBufScratchArea,
                                        WORD_TO_BYTES(l_writeWordsLen));
                    if(l_fapiRc != FAPI2_RC_SUCCESS)
                    {
                        HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_writeWordsLen,
                                SBE_FUNC "Write data to ",
                                SBE_SEC_CU_WRITE_IMAGE_FAILURE);

                        // Explanation as per LN:336-339
                        l_fapiRc = l_memHandle[l_side]->write_end();
                        if(l_fapiRc != FAPI2_RC_SUCCESS){
                            HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_writeWordsLen,
                                SBE_FUNC "Clearing Error States and Restoring the Tail End of",
                                SBE_SEC_CU_WRITE_END_IMAGE_FAILURE);
                        }
                    }
                }
            }

            // If both side operation is failed break and come out
            CHECK_UPDATE_IMG_CTRL_STRUCTURE_AND_BREAK_IF_BOTH_SIDES_FAILED(l_updateImgCtrlStruct);

            if (l_eotFlag == true)
            {
                for(uint8_t l_side=0; l_side<2; l_side++)
                {
                    if(l_updateImgCtrlStruct.goldenStruct.returnCodeSide[l_side] == SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_INFO(SBE_FUNC "Write end side %d .....",l_side);
                        l_fapiRc = l_memHandle[l_side]->write_end();
                        if(l_fapiRc != FAPI2_RC_SUCCESS){
                            HANDLE_FAPI_ERROR(l_fapiRc, l_updateImgCtrlStruct, l_side, l_writeWordsLen,
                                 SBE_FUNC "Write end to",
                                 SBE_SEC_CU_WRITE_END_IMAGE_FAILURE);
                        }
                    }
                }

            // If both side operation is failed break and come out
            CHECK_UPDATE_IMG_CTRL_STRUCTURE_AND_BREAK_IF_BOTH_SIDES_FAILED(l_updateImgCtrlStruct);

            }

            l_updateImgCtrlStruct.imageStartAddr += WORD_TO_BYTES(l_writeWordsLen);
        } //end of for-loop

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(o_fifoRc);

        if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
        {
            l_rc = checkUpdateImageStatus(l_updateImgCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "checkUpdateImageStatus unsuccessful. RC[0x%08x]", l_rc);
                break;
            }

            // update image successful
            SBE_INFO(SBE_FUNC "chip-op successful");
        }
    }
    while(false);

    //Free the scratch area
    Heap::get_instance().scratch_free(l_imgBufScratchArea);
    for(uint8_t l_side=0; l_side<2; l_side++)
    {
        if (l_memHandle[l_side] != NULL)
        {
            freeMemoryDevice(l_memHandle[l_side]);
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

////////////////////////////////////////////////////////
//////// Update Image Chip-op //////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeUpdateImageAll (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeUpdateImageAll "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    CU::updateImageCmdMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    bool ackEOT = false;

    do
    {
        uint8_t l_runningSide = 0, l_nonrunningSide = 0;
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
                 msg.imageType, WORD_TO_BYTES(msg.imageSizeInWords));

        // Check for validity of input parameters
        rc = msg.checkInputParamsValid();
        if (rc)
        {
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, rc );
            break;
        }

        // Get booted side
        getSideInfo(l_runningSide, l_nonrunningSide);

        if (l_runningSide == GOLDEN_SIDE_INDEX)
        {
            // call update image chip-op function (golden)
            rc = updateImageGolden(&msg, type, fifoRc, ackEOT);
            if (rc)
            {
                hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, rc );
            }
        }
        else
        {
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
        }
    } while(false);

    do
    {
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

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
