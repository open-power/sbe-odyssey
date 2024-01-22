/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/ras/sbeCmdCheckMemoryDevice.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "chipop_struct.H"
#include "plat_hwp_data_stream.H"
#include "globals.H"
#include "rasUtils.H"
#include "imagemap.H"
#include "heap.H"

using namespace fapi2;

/*
 * @brief getNumOfBitsSetInByte : function would check and return number
 *                                of bits set in byte
 *
 * @param[in] i_byte : input byte value
 *
 * @return numOfBitsSet
 */
static
uint8_t getNumOfBitsSetInByte(uint8_t i_byte)
{
    uint8_t l_count;

    for(l_count = 0; i_byte > 0; i_byte >>= 1)
    {
        l_count += (i_byte & 1);
    }

    return l_count;
}

uint32_t sbeCmdCheckMemoryDevice(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeCmdCheckMemoryDevice "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeRespGenHdr_t l_hdr;
    l_hdr.init();
    sbeFifoType l_fifoType;
    struct memCheckCmdMsg_t l_memCheckCmdMsg;
    struct memDeviceStatusDetails_t *l_memDeviceStatusBufferArea = NULL;
    chipOpParam_t* l_configStr = (struct chipOpParam*)i_pArg;
    l_fifoType = static_cast<sbeFifoType>(l_configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",l_fifoType);

    do
    {
        // Will attempt to dequeue for the memory check and
        // expected EOT entry at the end
        uint32_t l_len2dequeue = sizeof(l_memCheckCmdMsg)/sizeof(uint32_t);
        l_fifoRc = sbeUpFifoDeq_mult (l_len2dequeue, (uint32_t *)&l_memCheckCmdMsg,
                                      true, false, l_fifoType);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_fifoRc);

        // Get params
        SBE_INFO(SBE_FUNC "Scope:[0x%02x] Side:[0x%02x] DeviceId:[0x%02x]",
                 l_memCheckCmdMsg.scope, l_memCheckCmdMsg.side, l_memCheckCmdMsg.devId);

        // Check for validity of input params
        l_rc = l_memCheckCmdMsg.checkInputParamsValid();
        if (l_rc)
        {
            SBE_ERROR(SBE_FUNC "checkInputParamsValid failed. RC[0x%08x]", l_rc);
            l_hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
            break;
        }

        // Get the sides count to scrub on
        auto l_sideCnt = getNumOfBitsSetInByte(l_memCheckCmdMsg.side);

        // Get the device count to scrub on
        auto l_devCnt = getNumOfBitsSetInByte(l_memCheckCmdMsg.devId);

        // Get the number of entries to send in response buffer
        auto l_numOfScrubEntries = l_sideCnt * l_devCnt;

        // Get the structure allocated to collect status of devices suppported per side
        uint32_t l_memScrubBufferArea = sizeof(struct memDeviceStatusDetails_t) * l_numOfScrubEntries;
        l_memDeviceStatusBufferArea =
            (struct memDeviceStatusDetails_t *)Heap::get_instance().scratch_alloc(l_memScrubBufferArea);
        if(l_memDeviceStatusBufferArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed", l_memScrubBufferArea);
            l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
            l_hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc );
            break;
        }

        // Get memory scrubbed data
        l_rc = getMemoryScrubData(l_memCheckCmdMsg,
                                  l_memDeviceStatusBufferArea,
                                  l_hdr);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to get memory scrub data, RC[0x%08x]",
                      l_rc);
            l_hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
            break;
        }

        fapi2::sbefifo_hwp_data_ostream ostream(l_fifoType);
        l_fifoRc = ostream.put(((l_memScrubBufferArea) / sizeof(uint32_t)),
                               (uint32_t*)l_memDeviceStatusBufferArea);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to send memory device check chip-op response RC[0x%08x]",
                      l_fifoRc);
            break;
        }
    } while(false);

    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(l_hdr, NULL, l_fifoType);
        if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      " Failed to send response header for checkMemoryDevice " \
                      " RC[0x%08x]", l_rc);
        }
    }

    //Free the scratch area
    if (l_memDeviceStatusBufferArea != NULL)
    {
        Heap::get_instance().scratch_free(l_memDeviceStatusBufferArea);
    }

    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;
    #undef SBE_FUNC
}

