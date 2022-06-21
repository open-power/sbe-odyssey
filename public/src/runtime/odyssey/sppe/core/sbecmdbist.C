/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdbist.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include "sbecmdbist.H"
#include "sbefifo.H"
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbescom.H"
#include "sbeutil.H"
#include "sbeFifoMsgUtils.H"
#include "plat_hw_access.H"
#include "plat_target.H"
#include "ffdc.H"
#include "sbeglobals.H"
#include "chipop_struct.H"
#include <poz_bist.H>
#include <stdint.h>

using namespace fapi2;

/////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeBist (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeBist "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeBistReqMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    Target<SBE_ROOT_CHIP_TYPE> tgt =  g_platTarget->plat_getChipTarget();

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        uint32_t  len2dequeue  = sizeof(msg)/sizeof(uint32_t);
        l_rc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, true, false, type);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        fapi2::ReturnCode fapi_rc = poz_bist(tgt, msg.params);
        if (fapi_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = SBE_SEC_HWP_FAILURE;
        }

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
       // will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
