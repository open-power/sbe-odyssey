/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/ras/rasUtils.C $            */
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

#include "sbe_sp_intf.H"
#include "imagemap.H"
#include "rasUtils.H"
#include "pakwrapper.H"
#include "filenames.H"
#include "ptbl.H"

using namespace fapi2;
#define MAX_BUFFER_SIZE 0x1000 // 4KB

/*
 * @brief scrubMemoryForAddressNSize : Scrub region in memory for ecc errors
 *
 * @param[in] i_memHandle         : handle to memory device
 * @param[in] i_bufferScratchArea : pointer to buffer in scratch for spi read.
 * @param[in] i_sideStartAddress  : scrub side start address
 * @param[in] i_sideMaxSize.      : scrub side max size
 *
 * @return rc
 */
uint32_t scrubMemoryForAddressNSize(spi::AbstractMemoryDevice *&i_memHandle,
                                    uint32_t *i_bufferScratchArea,
                                    const uint32_t i_sideStartAddress,
                                    const uint32_t i_sideMaxSize)
{
    #define SBE_FUNC " scrubMemoryForAddressNSize "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;

    do
    {
        uint32_t l_offset = 0;
        uint32_t l_readLen = 0;

        for(uint32_t l_len = i_sideMaxSize; l_len > 0;
                l_len -= l_readLen, l_offset += l_readLen)
        {
            l_readLen = (l_len > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : l_len);

            l_fapiRc = i_memHandle->read(i_sideStartAddress + l_offset,
                                         l_readLen,
                                         i_bufferScratchArea);
            if(l_fapiRc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "Read data from device failed, Addr:0x%08X Size:0x%08X",\
                          i_sideStartAddress + l_offset, l_readLen);
                l_rc = SBE_SEC_CU_READ_DATA_IMAGE_FAILURE;

                // Commit the ffdc
                logError(l_fapiRc, fapi2::FAPI2_ERRL_SEV_RECOVERED);
                CLEAR_FAPI2_CURRENT_ERROR();
                break;
            }
        }
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

/*
 * @brief scrubMemoryImageOnly : Scrub image only region in memory for ecc errors
 *
 * @param[in]    i_memHandle         : handle to memory device
 * @param[in]    i_bufferScratchArea : pointer to buffer in scratch for spi read.
 * @param[in]    i_side              : side to scrub
 * @param[inout] io_scrubMemCtrlStruct struct used to get/set/use various
 *               context needed for memory scrub operations
 *
 * @return rc
 */
uint32_t scrubMemoryImageOnly(spi::AbstractMemoryDevice *&i_memHandle,
                              uint32_t *i_bufferScratchArea,
                              uint8_t i_side,
                              codeUpdateCtrlStruct_t &io_scrubMemCtrlStruct)
{
    #define SBE_FUNC " scrubMemoryImageOnly "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Get side number in memory: 0,1,2
        uint8_t l_side = i_side / 2;

        for (uint8_t l_img = 0; l_img < EXPECTED_IMG_SECTION_CNT; l_img++)
        {
            io_scrubMemCtrlStruct.imageType = (uint16_t)CU::g_expectedImgPkgMap[l_img].imageNum;

            // Get the partition start offset and size for the image from side passed
            l_rc = getPakEntryFromPartitionTable(l_side,
                                                 (CU_IMAGES)io_scrubMemCtrlStruct.imageType,
                                                 NULL,
                                                 io_scrubMemCtrlStruct);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR(SBE_FUNC "getPakEntryFromPartitionTable unsuccessful " \
                                   "Side[%d] imageTyepe[%d] RC[0x%08x]",
                                   l_side, io_scrubMemCtrlStruct.imageType, l_rc);
                break;
            }

            // Declaring variable to get pad size
            uint32_t l_paddedSize = 0;
            void *l_padStart = NULL;

            // Get the side start offset
            uint32_t l_sideStartAddress = 0;
            getSideAddress(l_side, l_sideStartAddress);

            // archive file start address and max size(archive limit)
            FileArchive pakPadSize((uint32_t *)io_scrubMemCtrlStruct.imageStartAddr,
                                   (uint32_t *)(l_sideStartAddress +
                                   io_scrubMemCtrlStruct.storageDevStruct.storageDevSideSize));
            l_rc = pakPadSize.locate_padding((void*&)l_padStart, l_paddedSize);
            if (l_rc != ARC_OPERATION_SUCCESSFUL)
            {
                SBE_ERROR("padSize is not found of an image[%d]"
                          "of address [0x%08x]",io_scrubMemCtrlStruct.imageType,
                          io_scrubMemCtrlStruct.imageStartAddr);
                break;
            }
            SBE_INFO(SBE_FUNC " padStart[%p], padSize[0x%08x]", (uint8_t*)l_padStart, l_paddedSize);

            //Getting actual image size, this size is going to be scrubbed.
            //Actual image size will be subtraction of image start address
            //from return padStart address
            uint32_t l_actualImageSize = (uint32_t)((uint8_t*)l_padStart - io_scrubMemCtrlStruct.imageStartAddr);

            //converting 24 bit as nor is 16MB 24-bits used to address
            //it to pass on to the spi
            io_scrubMemCtrlStruct.imageStartAddr  -=
                        io_scrubMemCtrlStruct.storageDevStruct.storageDevBaseAddress;

            SBE_INFO(SBE_FUNC "Image:[0x%02x] StartAddress:[0x%08x] Size:[0x%08x]",
                               l_img, io_scrubMemCtrlStruct.imageStartAddr, l_actualImageSize);

            l_rc = scrubMemoryForAddressNSize(i_memHandle, i_bufferScratchArea,
                                              io_scrubMemCtrlStruct.imageStartAddr, l_actualImageSize);
            if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                break;
            }
        }
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t getMemoryScrubData(const struct memCheckCmdMsg_t i_memCheckCmdMsg,
                            struct memDeviceStatusDetails_t *&o_memDeviceStatus,
                            sbeRespGenHdr_t &o_hdr)
{
    #define SBE_FUNC " getMemoryScrubData "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t *l_imgBufScratchArea = NULL;
    spi::AbstractMemoryDevice *l_memHandle = NULL;
    RASSPIPort *l_portHandle = NULL;

    do
    {
        uint8_t l_runSideIndex, l_nonRunSideIndex;
        // Get runnning & non-running side info
        getSideInfo(l_runSideIndex, l_nonRunSideIndex);
        SBE_INFO(SBE_FUNC "run:[%d] non-run:[%d]", l_runSideIndex, l_nonRunSideIndex);

        // control structure for memory scrub
        codeUpdateCtrlStruct_t l_scrubMemCtrlStruct;

        // Get memory scrub parameters populated
        getCodeUpdateParams(l_scrubMemCtrlStruct);

        // Allocate buffer size
        l_imgBufScratchArea =
                    (uint32_t *)Heap::get_instance().scratch_alloc(MAX_BUFFER_SIZE);
        if(l_imgBufScratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed", MAX_BUFFER_SIZE);
            l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
            o_hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
            break;
        }

        // Iterating the loop to get all the device status
        uint8_t l_scrubEntryCnt = 0;

        // Iterate over sides
        for (uint8_t l_side = 1; l_side <= END_OF_SIDE_LIST; l_side <<= 1)
        {
            if (!(l_side & i_memCheckCmdMsg.side))
            {
                continue;
            }

            // Iterate over device per side
            for (uint8_t l_dev = 1; l_dev <= END_OF_DEVICE_LIST; l_dev <<= 1)
            {
                if (!(l_dev & i_memCheckCmdMsg.devId))
                {
                    continue;
                }

                uint32_t l_sideStartAddress = 0;
                uint32_t l_sideMaxSize = 0;

                // Get the side->device start offset to scrub on
                l_rc = getSideStartAddressAndSize(l_side, l_dev, l_sideStartAddress, l_sideMaxSize);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC " getSideStartAddressAndSize unsuccessful. RC[0x%08x] ", l_rc);
                    o_memDeviceStatus[l_scrubEntryCnt].side = l_side;
                    o_memDeviceStatus[l_scrubEntryCnt].devId = l_dev;
                    o_memDeviceStatus[l_scrubEntryCnt].status = (uint16_t)l_rc;
                    l_scrubEntryCnt++;
                    continue;
                }

                // Get memory device handle
                l_rc = createMemoryDeviceRAS(l_runSideIndex,
                                             l_scrubMemCtrlStruct.storageDevStruct.memId,
                                             l_memHandle, l_portHandle);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC " createMemoryDeviceRAS unsuccessful. RC[0x%08x] ", l_rc);
                    o_memDeviceStatus[l_scrubEntryCnt].side = l_side;
                    o_memDeviceStatus[l_scrubEntryCnt].devId = l_dev;
                    o_memDeviceStatus[l_scrubEntryCnt].status = (uint16_t)l_rc;
                    l_scrubEntryCnt++;
                    continue;
                }

                if (i_memCheckCmdMsg.scope == SCOPE_FULL_SIDE)
                {
                    SBE_INFO(SBE_FUNC "Scope: Full side: Side:[0x%02x] Device:[0x%02x] Address:[0x%08x] Size:[0x%08x]",
                                       l_side, l_dev, l_sideStartAddress, l_sideMaxSize);
                    l_rc = scrubMemoryForAddressNSize(l_memHandle, l_imgBufScratchArea,
                                                      l_sideStartAddress, l_sideMaxSize);
                }
                else if (i_memCheckCmdMsg.scope == SCOPE_IMAGE_ONLY)
                {
                    SBE_INFO(SBE_FUNC "Scope: Image only: Side:[0x%02x] Device:[0x%02x] Address:[0x%08x]",
                                       l_side, l_dev, l_sideStartAddress);
                    l_rc = scrubMemoryImageOnly(l_memHandle, l_imgBufScratchArea, l_side, l_scrubMemCtrlStruct);
                }

                // Get ecc status for the memory side read
                o_memDeviceStatus[l_scrubEntryCnt].side = l_side;
                o_memDeviceStatus[l_scrubEntryCnt].devId = l_dev;
                o_memDeviceStatus[l_scrubEntryCnt].status = (uint16_t)l_rc;
                o_memDeviceStatus[l_scrubEntryCnt].numOfCE = l_portHandle->iv_ioMemDeviceStatus.numOfCE;
                o_memDeviceStatus[l_scrubEntryCnt].numOfUE = l_portHandle->iv_ioMemDeviceStatus.numOfUE;
                l_scrubEntryCnt++;

                if (l_memHandle != NULL)
                {
                    freeMemoryDeviceRAS(l_memHandle);
                }

                // Reset the rc value for next iteration if any
                l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
            }
        }
    } while (false);

   //Free the scratch area
   Heap::get_instance().scratch_free(l_imgBufScratchArea);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

