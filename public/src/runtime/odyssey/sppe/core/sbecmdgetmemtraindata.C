/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdgetmemtraindata.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "sbecmdgetmemtraindata.H"
#include "sbeFifoMsgUtils.H"
#include "plat_hwp_data_stream.H"
#include "chipop_handler.H"
#include "globals.H"
#include "heap.H"
#include "odysseylink.H"

using namespace fapi2;

uint32_t sbeGetMemTrainData(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetMemTrainData "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeGetMemTrainDataMsgHdr_t reqMsg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

    fapi2::sbefifo_hwp_data_istream istream(type);
    fapi2::sbefifo_hwp_data_ostream ostream(type);

    do
    {
        // Get the sbeGetMemTrainDataMsgHdr_t message.
        uint32_t len2dequeue  = sizeof(reqMsg)/sizeof(uint32_t);
        fifoRc = istream.get(len2dequeue, (uint32_t *)&reqMsg, true, false);

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        // Validate the params.
        if(!reqMsg.isValidOperations())
        {
            SBE_ERROR(SBE_FUNC "Operation is not correct. Operation is: [%02X]",
                                (uint32_t)reqMsg.operation);
            hdr.setStatus( SBE_PRI_INVALID_DATA,
                           SBE_SEC_INVALID_PARAMS);
            break;
        }

        // Check if the memory train data is available in the SRAM.
        if(!g_draminitOffset)
        {
            SBE_ERROR(SBE_FUNC "There is no memory training data. g_draminitOffset is: [%08X]",
                               g_draminitOffset);
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_NO_MEM_TRAIN_DATA);
            break;
        }

        SBE_INFO(SBE_FUNC "Operation is: [%02X] g_draminitOffset is: [0x%08X]",
                           (uint32_t)reqMsg.operation, g_draminitOffset);

        // Stream the train data.
        SBE_INFO(SBE_FUNC "Stream the train data");
        if(reqMsg.operation == MEMTRAINDUMP)
        {
            // Data is available and now stream it.
            fifoRc = ostream.put((SPPE_MEM_TRAINING_DATA_SIZE / 4), (uint32_t *)g_draminitOffset);

            // If FIFO access failure
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);
            Heap::get_instance().scratch_free((uint32_t *)g_draminitOffset);
            g_draminitOffset = NULL;

        }

        // Purge the train data.
        SBE_INFO(SBE_FUNC "Purge the train data");
        if(reqMsg.operation == MEMTRAINPURGE)
        {
            // De allocate the scratch space.
            Heap::get_instance().scratch_free((uint32_t *)g_draminitOffset);
            g_draminitOffset = NULL;

        }
    }while(0);
    do
    {
        // Build the response header packet.
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);
        fifoRc = sbeDsSendRespHdr(hdr, &ffdc, type);
        // will let command processor routine handle the failure.
    }while(0);

    SBE_EXIT(SBE_FUNC);
    return fifoRc;
    #undef SBE_FUNC
}
