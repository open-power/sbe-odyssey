/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmddtsread.C $        */
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
#include "sbecmddtsread.H"
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
#include "ody_dts_read.H"
#include <stdint.h>

using namespace fapi2;

uint32_t sbeDtsRead (uint8_t *i_pArg)
{
    #define SBE_FUNC " odyDtsRead "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    odyDTSReadRespHdr_t odyTempHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    fapi2::ReturnCode fapiRc;
    uint32_t len2dequeue = 0;

    Target<SBE_ROOT_CHIP_TYPE> i_target = g_platTarget->plat_getChipTarget();
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);
        l_rc = sbeUpFifoDeq_mult (len2dequeue, NULL, true, false, type);

        fapiRc = ody_dts_read(i_target, odyTempHdr.dtsTemp);

        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC"  odyDtsRead failed."
                "fapiRc:0x%04x",  fapiRc);
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_ODY_DTS_READ_FAILED);
            ffdc.setRc(fapiRc);
            break;
        }
        // Enqueue DTS return data
        uint32_t len2enqueue = sizeof(odyTempHdr)/sizeof(uint32_t);
        l_rc = sbeDownFifoEnq_mult(len2enqueue, (uint32_t*)&odyTempHdr, type);

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(respHdr, &ffdc, type);
        // Will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}