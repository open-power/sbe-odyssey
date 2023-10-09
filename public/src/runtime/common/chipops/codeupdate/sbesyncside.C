/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/sbesyncside.C $  */
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

#include "poz_sbe_spi_cmd.H"
#include "ppe42_string.h"
#include "sbeFifoMsgUtils.H"
#include "codeupdateutils.H"
#include "sbecmdcodeupdate.H"
#include "sbeupdateimage.H"
#include "ptbl.H"
#include "sbesyncside.H"
#include "heap.H"
#include "archive.H"

fapi2::ReturnCode syncSide(const sbeSyncReqMsg_t *i_msg,
                           sbeRespGenHdr_t *o_hdr)
{
   #define SBE_FUNC " syncSide "
   SBE_ENTER(SBE_FUNC);
   ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
   fapi2::current_err = FAPI2_RC_SUCCESS;

   do
   {
      //declaration of variable
      bool l_isBootLoaderSynced = false;
      bool l_isSyncFailed = false;

      // control structure for sync
      codeUpdateCtrlStruct_t l_syncSideCtrlStruct;

      // Get code-update parameters populated
      getCodeUpdateParams(l_syncSideCtrlStruct);

      //side information
      SBE_INFO(SBE_FUNC " runSide:[%d] nonRunSide:[%d]",
                        l_syncSideCtrlStruct.runSideIndex,
                        l_syncSideCtrlStruct.nonRunSideIndex);

      // Bootloader image type must be at first index in the syncable images list
      // because if bootloader is synced (there may be chance that partition table
      // is modified) then we need to sync all other images without checking
      // the respective image hash.
      static_assert(CU::g_expectedImgPkgMap[0].imageNum == CU_IMAGES::BOOTLOADER,
                   "First element must be BootLoader in the g_expectedImgPkgMap"
                   "as per sync chip-op algorithm");

      for (uint8_t l_img = 0; l_img < EXPECTED_IMG_SECTION_CNT ; l_img++)
      {
         bool  l_isImgHashMatched = false;

         // Get incoming chip-op params updated
         l_syncSideCtrlStruct.imageType = (uint16_t)CU::g_expectedImgPkgMap[l_img].imageNum;

         //When force sync is set no need to compare the image hash
         //corresponding to each image type on both sides instead force copy
         //the images to the other side. When force sync is not set and if
         //bootlaoder image (always first image in list not applicable for
         //other images in list) mismatches then copy over all the remaining
         //images as-is on the other side without comparing the image hash.

         if (!i_msg->forceSync)
         {
            //Validate image hash for a given image across both sides in device
            //and do sync in case of mismatch
            if(!l_isBootLoaderSynced)
            {
               l_isImgHashMatched = checkImageHashMismatch(l_syncSideCtrlStruct,o_hdr);
               if ((o_hdr->secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL))
               {
                  //in case checkImageHashMismatch is failed , making this flag true
                  // to break the do while code
                  l_isSyncFailed = true;
                  break;
               }
            }
         }

         SBE_INFO(SBE_FUNC "force sync[%d],l_isImgHashMatched[%d]" \
                           "l_isBootLoaderSynced[%d]",i_msg->forceSync ,
                           l_isImgHashMatched, l_isBootLoaderSynced );

         if (!l_isImgHashMatched)
         {
            // calling API to sync from Running to NonRunning Side
            l_fapiRc = syncImage(l_syncSideCtrlStruct, o_hdr);

            //doing error handling of the API(syncImage)
            if ((o_hdr->secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL) ||
               (l_fapiRc != FAPI2_RC_SUCCESS))
            {
               SBE_ERROR(SBE_FUNC \
                           "Failed to sync imageType[%d] from " \
                           " runSide[%d] to nonRunSide[%d] ",
                           CU::g_expectedImgPkgMap[l_img].imageNum,
                           l_syncSideCtrlStruct.runSideIndex,
                           l_syncSideCtrlStruct.nonRunSideIndex);
               //in case sync is failed , making this flag true
               // to break the do while code
               l_isSyncFailed = true;
               break;
            }
            else if (CU::g_expectedImgPkgMap[l_img].imageNum == CU_IMAGES::BOOTLOADER)
            {
               // incase of bootloader, making this flag true
               // to avoid comparision for other image like(runtime,host,bmc)
               l_isBootLoaderSynced = true;
            }
         }
      }

      // simply breaking the do-while loop when the flag is set
      if(l_isSyncFailed)
      {
         break;
      }

   } while (false);

   SBE_EXIT(SBE_FUNC);
   return l_fapiRc;
   #undef SBE_FUNC

}

fapi2::ReturnCode syncImage(codeUpdateCtrlStruct_t &i_syncSideCtrlStruct,
                            sbeRespGenHdr_t *o_hdr)
{
   #define SBE_FUNC " syncImage "
   SBE_ENTER(SBE_FUNC);

   uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
   ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;

   //buffer pointer
   uint32_t *l_imgBufScratchArea = NULL;
   spi::AbstractMemoryDevice *l_memHandle = NULL;

   do
   {
      // Get the partition start offset and size from running side
      l_rc = getPakEntryFromPartitionTable(i_syncSideCtrlStruct.runSideIndex,
                                           (CU_IMAGES)i_syncSideCtrlStruct.imageType,
                                           NULL,
                                           i_syncSideCtrlStruct);
      if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
      {
         SBE_ERROR(SBE_FUNC "getPakEntryFromPartitionTable unsuccessful " \
                            "runSide[%d] imageTyepe[%d] RC[0x%08x]",
                            i_syncSideCtrlStruct.runSideIndex,
                            i_syncSideCtrlStruct.imageType, l_rc);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
         break;
      }

      //runside image start address
      uint32_t l_runSideImageStartAddr = i_syncSideCtrlStruct.imageStartAddr;

      // Declaring variable to get pad size
      uint32_t l_paddedSize = 0;
      void *l_padStart = NULL;

      // Get the side start offset for non-run side
      uint32_t l_nonRunSideStartAddress = 0;
      getSideAddress(i_syncSideCtrlStruct.nonRunSideIndex, l_nonRunSideStartAddress);

      // Get the side start offset for run side
      uint32_t l_runSideStartAddress = 0;
      getSideAddress(i_syncSideCtrlStruct.runSideIndex, l_runSideStartAddress);

      // archive file start address and max size(archive limit)
      FileArchive pakPadSize((uint32_t *)l_runSideImageStartAddr,
                             (uint32_t *)(l_runSideStartAddress +
                             i_syncSideCtrlStruct.storageDevStruct.storageDevSideSize));
      l_rc = pakPadSize.locate_padding((void*&)l_padStart, l_paddedSize);
      if (l_rc != ARC_OPERATION_SUCCESSFUL)
      {
         SBE_ERROR("padSize is not found of an image[%d]"
                    "of address [0x%08x]",i_syncSideCtrlStruct.imageType,
                    i_syncSideCtrlStruct.imageStartAddr);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
         break;
      }
      SBE_INFO(" padStart[%p] ,padSize[0x%08x]",(uint8_t*)l_padStart, l_paddedSize);

      //Getting actual image size, this size is going to sync.
      //Actual image size will be subtraction of image start address
      //from return padStart address
      uint32_t l_actualImageSize = (uint32_t)((uint8_t*)l_padStart - l_runSideImageStartAddr);

      // validate actual image + pad size must not exceed the image max size
      if((l_actualImageSize + l_paddedSize) > i_syncSideCtrlStruct.imageSizeMax)
      {
         SBE_ERROR(" Invalid pad size value " \
                     " runSideImageStartAdd[0x%08x] imageMaxSize[0x%08x] " \
                     " actualImageSize[0x%08x] padStart[%p]",
                     l_runSideImageStartAddr, i_syncSideCtrlStruct.imageSizeMax,
                     l_actualImageSize, (uint8_t*)l_padStart);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_CU_INVALID_PAD_SIZE);
         break;
      }

      //nonRunSide image start address
      uint32_t l_nonRunSideImageStartAddr = ((l_runSideImageStartAddr - l_runSideStartAddress )+
                                             l_nonRunSideStartAddress);

      //converting 24 bit as nor is 16MB 24-bits used to address
      //it to pass on to the spi
      l_runSideImageStartAddr  -=
                  i_syncSideCtrlStruct.storageDevStruct.storageDevBaseAddress;

      l_nonRunSideImageStartAddr -=
               i_syncSideCtrlStruct.storageDevStruct.storageDevBaseAddress;

      SBE_INFO(SBE_FUNC "imageType[%d] , runside image startAdd:[0x%08x]" \
                        " nonRunside image startAdd:[0x%08x]" \
                        " Actual imageSize:[0x%08x]",
                        i_syncSideCtrlStruct.imageType,
                        l_runSideImageStartAddr, l_nonRunSideImageStartAddr,
                        l_actualImageSize);

      //nonRunSide image start address and image size for erase and write
      i_syncSideCtrlStruct.imageStartAddr = l_nonRunSideImageStartAddr;
      i_syncSideCtrlStruct.imageSizeInWords = BYTES_TO_WORDS(l_actualImageSize);

      // Allocate buffer of bufferSize
      l_imgBufScratchArea =
                  (uint32_t *)Heap::get_instance().scratch_alloc(
                  i_syncSideCtrlStruct.storageDevStruct.maxBufferSize);
      if(l_imgBufScratchArea == NULL)
      {
         SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed",
                     i_syncSideCtrlStruct.storageDevStruct.maxBufferSize);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_HEAP_BUFFER_ALLOC_FAILED );
         break;
      }

      // Get memory device handle
      l_rc = createMemoryDevice(i_syncSideCtrlStruct.runSideIndex,
                                i_syncSideCtrlStruct.storageDevStruct.memId,
                                true, true, l_memHandle);
      if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
      {
         SBE_ERROR(SBE_FUNC " createMemoryDevice unsuccessful. RC[0x%08x] ", l_rc);
         o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
         break;
      }

      // local variable declaration
      uint32_t l_imgOffset = 0;
      uint32_t l_readWriteLen = 0;
      bool l_preDataReadFlag = true;

      for(uint32_t l_len = l_actualImageSize; l_len > 0;
            l_len -= l_readWriteLen, l_imgOffset += l_readWriteLen)
      {
         l_readWriteLen = (l_len > i_syncSideCtrlStruct.storageDevStruct.maxBufferSize ?
                           i_syncSideCtrlStruct.storageDevStruct.maxBufferSize : l_len);

         //Using  memset() to force all bytes of ImgBufScratchArea to zero
         memset(l_imgBufScratchArea,0,
                i_syncSideCtrlStruct.storageDevStruct.maxBufferSize);

         if (l_preDataReadFlag == true)
         {
            SBE_INFO(SBE_FUNC "Write begin.....");
            l_fapiRc = l_memHandle->write_begin(i_syncSideCtrlStruct.imageStartAddr,
                                                WORD_TO_BYTES(i_syncSideCtrlStruct.imageSizeInWords));
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
               SBE_ERROR(SBE_FUNC "Write begin to device failed, Addr:0x%08X Size:0x%08X",\
                         i_syncSideCtrlStruct.imageStartAddr,\
                         WORD_TO_BYTES(i_syncSideCtrlStruct.imageSizeInWords));

               o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                 SBE_SEC_CU_WRITE_BEGIN_IMAGE_FAILURE );
               break;
            }
            l_preDataReadFlag = false;
         }

         SBE_INFO(SBE_FUNC "Read data...");
         l_fapiRc = l_memHandle->read(l_runSideImageStartAddr + l_imgOffset,
                                      l_readWriteLen,
                                      l_imgBufScratchArea);
         if(l_fapiRc != FAPI2_RC_SUCCESS)
         {
            SBE_ERROR(SBE_FUNC "Read data from device failed, Addr:0x%08X Size:0x%08X",\
                      l_runSideImageStartAddr + l_imgOffset,\
                      l_readWriteLen);

            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                              SBE_SEC_CU_READ_DATA_IMAGE_FAILURE );

            // Clean-up by calling write functions to clear error states and
            // restoring the tail end of a partially written erase block so
            // it's crucial to finish the write operation to ensure we don't lose
            // data outside of the partition we're writing
            l_memHandle->write_end();
            break;
         }

         SBE_INFO(SBE_FUNC "Write data...");
         l_fapiRc = l_memHandle->write_data(l_imgBufScratchArea, l_readWriteLen);
         if(l_fapiRc != FAPI2_RC_SUCCESS)
         {
            SBE_ERROR(SBE_FUNC "Write data to device failed, Addr:0x%08X Size:0x%08X",\
                      i_syncSideCtrlStruct.imageStartAddr,\
                      l_readWriteLen);

            o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                              SBE_SEC_CU_WRITE_DATA_IMAGE_FAILURE );

            // Explanation as per LN:318-321
            l_memHandle->write_end();
            break;
         }

         if (l_len == l_readWriteLen)
         {
            SBE_INFO(SBE_FUNC "Write end...");
            l_fapiRc = l_memHandle->write_end();
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
               SBE_ERROR(SBE_FUNC "Write end to device failed, Addr:0x%08X Size:0x%08X",\
                         i_syncSideCtrlStruct.imageStartAddr,\
                         l_readWriteLen);

               o_hdr->setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                 SBE_SEC_CU_WRITE_END_IMAGE_FAILURE );
               break;
            }
         }

         i_syncSideCtrlStruct.imageStartAddr += l_readWriteLen;
      }

   } while (false);

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
