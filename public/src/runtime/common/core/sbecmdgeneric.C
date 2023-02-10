/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbecmdgeneric.C $              */
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
#include "sbecmdgeneric.H"
#include "cmnglobals.H"
#include "ppe42_string.h"
#include "sbetrace.H"
#include "poz_scom_perv_cfam.H"
#include "poz_ppe.H"
#include "mbxscratch.H"
#include "errorcodes.H"
#include "sbefifo.H"
#include "chipop_struct.H"
#include "sbeFifoMsgUtils.H"


////////////////////////////////////////////////////////
//////// Get Capability Chip-op ////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeGetCapability (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeGetCapability "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    //TODO:PFSBE-6 to be uncommented during implementation
    //uint32_t fapiRc = fapi2::FAPI2_RC_SUCCESS;
    //getCapabilityRespMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

#if 0   //TODO: to be enabled during implementation
        //No attempt to read FIFO as no input params expected
        // call get capability function
        fapiRc = getCapability(&msg, type);
        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = CU_RC_GET_CAPABILITY_HWP_FAILURE;
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
            ffdc.setRc(l_rc);
            break;
        }
#endif
    } while(false);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC"Failed. rc[0x%X]",l_rc);
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

