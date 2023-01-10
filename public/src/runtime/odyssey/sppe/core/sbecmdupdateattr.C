/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdupdateattr.C $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#include "sbeFifoMsgUtils.H"
#include "chipop_struct.H"
#include "sbecmdupdateattr.H"
#include "sbe_sp_intf.H"
#include "sbeglobals.H"
#include "heap.H"
#include "attribute_override.H"

uint32_t sbeUpdateAttr(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeUpdateAttr "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_len = 0;
    sbeFifoType type;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    void *l_inBuffer = nullptr;
    void *l_outBuffer = nullptr;
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);

        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Get the length of payload
        // Length is not part of chipop. So take length from total length
        l_len = SBE_GLOBAL->sbeFifoCmdHdr.len -
                        sizeof(SBE_GLOBAL->sbeFifoCmdHdr)/sizeof(uint32_t);

        SBE_INFO(SBE_FUNC"Expected length of the payload(in words) : %d", l_len);

        // The size of the buffer to be allocated in bytes.
        // For that, multiply the number of words by 4.
        l_inBuffer = Heap::get_instance().scratch_alloc(l_len*4);
        if (l_inBuffer == nullptr)
        {
            SBE_ERROR(SBE_FUNC"scratch allocation request for [%d] bytes failed.", (l_len*4));
            l_rc=SBE_SEC_HEAP_SPACE_FULL_FAILURE;
            break;
        }

        l_rc = sbeUpFifoDeq_mult (l_len, (uint32_t *)l_inBuffer, true, false, type);

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        l_outBuffer = Heap::get_instance().scratch_alloc(l_len*4);
        if (l_outBuffer == nullptr)
        {
            SBE_ERROR(SBE_FUNC"scratch allocation request for [%d] bytes failed.", (l_len*4));
            l_rc=SBE_SEC_HEAP_SPACE_FULL_FAILURE;
            break;
        }

        uint32_t  l_respPackSize = 0;
        l_rc = fapi2::ATTR::applyOverride(l_inBuffer,l_outBuffer,l_respPackSize);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        uint32_t len2enqueue = l_respPackSize / 4;
        l_rc = sbeDownFifoEnq_mult (len2enqueue, (uint32_t *)l_outBuffer, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

    } while(false);

    // scratch_free() will check if the input pointer
    // is nullptr before calling free
    Heap::get_instance().scratch_free(l_outBuffer);

    Heap::get_instance().scratch_free(l_inBuffer);


    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
    }
    l_rc = sbeDsSendRespHdr(respHdr, NULL, type);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
