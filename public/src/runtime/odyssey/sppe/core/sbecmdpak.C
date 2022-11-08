/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdpak.C $            */
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
#include "heap.C"
#include "sbecmdpak.H"
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbescom.H"
#include "sbeutil.H"
#include "sbeFifoMsgUtils.H"
#include "plat_target.H"
#include "ffdc.H"
#include "heap.H"
#include "chipop_struct.H"

using namespace fapi2;

/////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbePakPush (uint8_t *i_pArg)
{
    #define SBE_FUNC " pakPush "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbePakPushMsgHdr_t l_reqMsg = sbePakPushMsgHdr_t();
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Dequeue size to load
        uint32_t len2dequeue = sizeof(l_reqMsg)/sizeof(uint32_t);
        // EoT not expected
        l_rc = sbeUpFifoDeq_mult(len2dequeue, (uint32_t *)&l_reqMsg, false, false, type);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        l_rc = Heap::get_instance().pushPakStack(type, l_reqMsg.size);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // Push the final heap midline addr into downstream FIFO
        uint32_t len2enqueue = 1;
        uint32_t res_midline = reinterpret_cast<uint32_t>(Heap::get_instance().getHeapMidlinePtr());
        l_rc = sbeDownFifoEnq_mult (len2enqueue, &res_midline, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(respHdr, &ffdc, type);
       // will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
/////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbePakPop (uint8_t *i_pArg)
{
    #define SBE_FUNC " pakPop "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbePakPopMsgHdr_t l_reqMsg;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);
        // Dequeue pop count (number of paks to pop)
        uint32_t len2dequeue = sizeof(l_reqMsg)/sizeof(uint32_t);
        // EoT expected
        l_rc = sbeUpFifoDeq_mult(len2dequeue, (uint32_t *)&l_reqMsg, true, false, type);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        l_rc = Heap::get_instance().popPakStack(l_reqMsg.pop_count);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // Push the final heap midline addr into downstream FIFO
        uint32_t len2enqueue = 1;
        uint32_t res_midline = reinterpret_cast<uint32_t>(Heap::get_instance().getHeapMidlinePtr());
        l_rc = sbeDownFifoEnq_mult (len2enqueue, &res_midline, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(respHdr, &ffdc, type);
       // will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
