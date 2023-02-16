/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdlistattr.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "sbecmdlistattr.H"
#include "sbe_sp_intf.H"
#include "sbeglobals.H"
#include "heap.H"
#include "attribute_table.H"

uint32_t sbeCmdListAttr(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeCmdListAttr "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_len = 0;
    uint32_t l_fifoRc =0;
    sbeFifoType type;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    void *l_outBuffer = nullptr;
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        l_rc = sbeUpFifoDeq_mult(l_len, NULL, true, false, type);
        // If FIFO access failure
        if(l_rc)
        {
            l_fifoRc = l_rc;
            SBE_ERROR(SBE_FUNC"l_fifoRc upfifo : 0x%08x", l_fifoRc);
            break;
        }
        SBE_INFO(SBE_FUNC "Fifo Type is:[%02X]",type);
        uint32_t attrListSize = fapi2::ATTR::ListResponseBuffer::getExpectedHeapSize();
        l_outBuffer = Heap::get_instance().scratch_alloc(attrListSize);
        SBE_INFO(SBE_FUNC " output buffer = 0x%08x", l_outBuffer);
        if (l_outBuffer == nullptr)
        {
            SBE_ERROR(SBE_FUNC"scratch allocation request for [%d] bytes failed.", attrListSize);
            l_rc=SBE_SEC_HEAP_SPACE_FULL_FAILURE;
            break;
        }
        l_rc = fapi2::ATTR::listAttribute(l_outBuffer);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        uint32_t len2enqueue = attrListSize / 4;
        SBE_INFO(SBE_FUNC "len2Enqueue = 0x%08x", len2enqueue);
        l_fifoRc = sbeDownFifoEnq_mult (len2enqueue, (uint32_t *)l_outBuffer, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_fifoRc);

    } while(false);

    // scratch_free() will check if the input pointer
    // is nullptr before calling free
    Heap::get_instance().scratch_free(l_outBuffer);
    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
    }
    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        l_fifoRc = sbeDsSendRespHdr(respHdr, NULL, type);
        if ( l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC"Attribute List chip-op send response "
                        "header failed.RC=0x%08X",l_fifoRc);
        }
    }
    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;
    #undef SBE_FUNC

}