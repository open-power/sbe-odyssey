/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/sbecmdcodeupdate.C $ */
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
#include "sbecmdcodeupdate.H"
#include "sbeupdateimage.H"
#include "ptbl.H"
#include "codeupdateutils.H"
#include "chipop_struct.H"
#include "pakwrapper.H"
#include "filenames.H"
#include "sbeutil.H"
#include "plat_hwp_data_stream.H"
#include "sbesyncside.H"


uint32_t sbeGetCodeLevels (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetCodeLevels "
    SBE_ENTER(SBE_FUNC);

    // Use only for FIFO utility API
    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* l_configStr((struct chipOpParam*)i_pArg);
    sbeFifoType l_fifoType(static_cast<sbeFifoType>(l_configStr->fifoType));
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]", l_fifoType);

    // Request structure for code-update params needed
    codeUpdateCtrlStruct_t l_codeUpdateCtrlStruct;

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

        // Get code-update parameters populated
        getCodeUpdateParams(l_codeUpdateCtrlStruct);

        uint8_t l_runningSide;
        uint8_t l_nonRunningSide;
        getSideInfo(l_runningSide, l_nonRunningSide);

        // Use only for secondary rc to send in the response header
        uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
        CU::CodeLevelsRespMsg_t l_codeLevelsRespMsg;
        for (uint8_t i = 0; i < UPDATABLE_IMG_SECTION_CNT; i++)
        {
            l_codeLevelsRespMsg.iv_updateableImagesInfo[i].iv_imageType =
                                            (CU_IMAGES)CU::g_updatableImgPkgMap[i].imageNum;
            l_rc = getImageHash(l_codeLevelsRespMsg.iv_updateableImagesInfo[i].
                                    iv_imageType,
                                l_runningSide,
                                l_codeUpdateCtrlStruct,
                                l_codeLevelsRespMsg.iv_updateableImagesInfo[i].
                                    iv_imageHashSHA3_512);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC \
                          "Failed to get image hash, "
                          "RC[0x%08x] RunningSide[%d] ImageType[%d]",
                          l_rc, l_runningSide, l_codeLevelsRespMsg.
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
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    CU::updateImageCmdMsg_t msg;
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
                 msg.imageType, WORD_TO_BYTES(msg.imageSizeInWords));

        // Check for validity of input parameters
        rc = msg.checkInputParamsValid();
        if (rc)
        {
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, rc );
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

uint32_t sbeSyncSide (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeSyncSide "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_fiforc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    sbeSyncReqMsg_t l_msg;
    sbeRespGenHdr_t l_hdr;
    l_hdr.init();
    sbeResponseFfdc_t l_ffdc;
    sbeFifoType l_type;
    do
    {
        chipOpParam_t* l_configStr = (struct chipOpParam*)i_pArg;
        l_type = static_cast<sbeFifoType>(l_configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",l_type);

        // Will attempt to dequeue one entry for the sync chip-op
        uint32_t l_len2dequeue = sizeof(l_msg)/sizeof(uint32_t);
        l_fiforc = sbeUpFifoDeq_mult (l_len2dequeue, (uint32_t *)&l_msg,
                                      true, false, l_type);

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_fiforc);

        // call sync chip-op function
        l_fapiRc = syncSide(&l_msg, &l_hdr);
        if (l_hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
                l_ffdc.setRc(l_fapiRc);
            }
            break;
        }
        else if(l_fapiRc != FAPI2_RC_SUCCESS)
        {
            l_ffdc.setRc(l_fapiRc);
            l_hdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                            SBE_SEC_CU_SYNC_CHIPOP_FAILURE);
            break;
        }

    } while(false); // end of do-while

    if (l_fiforc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_fiforc = sbeDsSendRespHdr(l_hdr, &l_ffdc, l_type);
        if (l_fiforc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      " Failed to send response header for sync chip-op " \
                      " RC[0x%08x]", l_fiforc);
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_fiforc;
    #undef SBE_FUNC

}



bool checkImageHashMismatch(codeUpdateCtrlStruct_t &i_syncSideCtrlStruct,
                            sbeRespGenHdr_t *o_hdr)
{
   #define SBE_FUNC " checkImageHashMismatch "
   SBE_ENTER(SBE_FUNC);

   uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
   bool isImageHashMatch = false;

   do
   {
      // declaration of running side image hash
      uint8_t  l_runSideImgHash[SHA3_DIGEST_LENGTH] = {0};

      // declaration of non-running side image hash
      uint8_t  l_nonRunSideImgHash[SHA3_DIGEST_LENGTH] = {0};

      // Get running side image hash of an image
      l_rc = getImageHash((CU_IMAGES)i_syncSideCtrlStruct.imageType,
                          i_syncSideCtrlStruct.runSideIndex,
                          i_syncSideCtrlStruct,
                          l_runSideImgHash);
      // Doing error handling of get image Hash from runSide
      if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
      {
         SBE_ERROR(SBE_FUNC \
                  "Failed to get image hash to image type [%d]" \
                  "for runSide[%d] RC[0x%08x]",
                  i_syncSideCtrlStruct.imageType,
                  i_syncSideCtrlStruct.runSideIndex,l_rc);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,l_rc);
         break;
      }

      // Get non-running side image hash of an image
      l_rc = getImageHash((CU_IMAGES)i_syncSideCtrlStruct.imageType,
                          i_syncSideCtrlStruct.nonRunSideIndex,
                          i_syncSideCtrlStruct,
                          l_nonRunSideImgHash);
      // Doing error handling of get image Hash from nonRunSide
      if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
      {
         SBE_ERROR(SBE_FUNC \
                  "Failed to get image hash to image type [%d]" \
                  "for nonRunSide[%d] RC[0x%08x]",
                  i_syncSideCtrlStruct.imageType,
                  i_syncSideCtrlStruct.nonRunSideIndex,l_rc);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,l_rc);
         break;
      }

      // comparing image hash of sides (from runSide to nonRunSide)
      // then return the status of compared image hash
      if(memcmp(l_runSideImgHash,l_nonRunSideImgHash,SHA3_DIGEST_LENGTH) == 0)
      {
         isImageHashMatch = true;
      }

   } while(false);

   SBE_EXIT(SBE_FUNC);
   return  isImageHashMatch;
   #undef SBE_FUNC

}

fapi2::ReturnCode performEraseInDevice(SpiControlHandle& i_handle,
                                       codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " performEraseInDevice "

    // Note: both start and end address for erase operation
    // must be 4K aligned with ECC byte included
    // Note: Would need to have alignment check for start address but it may
    // not work for bootloader today as its not aligned. As we dont expect any
    // frequent changes in partition table in Odyssey (also its frozen) so we
    // can defer to check for alignment and raise error if any, for later.
    // TODO:JIRA:PFSBE-387
    uint32_t eraseStartAddress = WITH_ECC(i_codeUpdateCtrlStruct.imageStartAddr) &
                                 i_codeUpdateCtrlStruct.storageDevStruct.storageSectorBoundaryAlign;

    // Erase end address may not be aligned to the sector boundary as its dependent
    // on the incoming image size. Erase API accept the last byte of the sector as the
    // end address so finding the last byte of the sector contaning this. This logic
    // would not cross to the next image as the image always starts at sector boundary
    uint32_t eraseEndAddress = WITH_ECC(i_codeUpdateCtrlStruct.imageStartAddr +
                               WORD_TO_BYTES(i_codeUpdateCtrlStruct.imageSizeInWords) - 1);

    if (((eraseEndAddress + 1 ) & i_codeUpdateCtrlStruct.storageDevStruct.storageSubSectorCheckMask) != 0)
    {
        // Find the start address of this sector
        eraseEndAddress &= i_codeUpdateCtrlStruct.storageDevStruct.storageSectorBoundaryAlign;
        // Find the start address of the next sector
        eraseEndAddress = eraseEndAddress + i_codeUpdateCtrlStruct.storageDevStruct.storageSubSectorSize;
        // Find the last address of this sector
        eraseEndAddress -= 1;
    }

    SBE_INFO(SBE_FUNC "Perform erase opreration: Start address:[0x%08x] End address:[0x%08X].",
                      eraseStartAddress, eraseEndAddress);

    return deviceErase(i_handle, eraseStartAddress, eraseEndAddress);
    #undef SBE_FUNC
}


fapi2::ReturnCode performWriteInDevice(SpiControlHandle& i_handle,
                                       codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct,
                                       uint32_t i_writeLength,
                                       void *i_bufferAddr,
                                       bool i_ecc)
{
    return deviceWrite(i_handle,
                       i_codeUpdateCtrlStruct.imageStartAddr,
                       i_writeLength,
                       (void *)i_bufferAddr,
                       i_ecc);
}

fapi2::ReturnCode performReadFromDevice(SpiControlHandle& i_handle,
                                        const uint32_t i_readAddress,
                                        const uint32_t i_readLength,
                                        const SPI_ECC_CONTROL_STATUS i_eccStatus,
                                        void *o_buffer
                                       )
{
    return deviceRead(i_handle,
                      i_readAddress,
                      i_readLength,
                      i_eccStatus,
                      (void *)o_buffer
                      );
}

