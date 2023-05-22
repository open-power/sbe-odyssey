/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/getcapabilities/sbeCmdGetCapabilities.C $ */
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

#include "chipop_struct.H"
#include "plat_hwp_data_stream.H"
#include "sbeCmdGetCapabilities.H"
#include "getcapabilitiesutils.H"

uint32_t sbeCmdGetCapabilities(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeCmdGetCapability "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeRespGenHdr_t l_hdr;
    l_hdr.init();
    sbeResponseFfdc_t l_ffdc;
    sbeFifoType l_fifoType;
    GetCapabilityResp_t l_capRespMsg;
    chipOpParam_t* l_configStr = (struct chipOpParam*)i_pArg;
    l_fifoType = static_cast<sbeFifoType>(l_configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",l_fifoType);

    do
    {
        // Input params are not expected for GetCapability chip-op.
        // so len2dequeue will be zero
        uint32_t l_len2dequeue = 0;
        l_fifoRc = sbeUpFifoDeq_mult (l_len2dequeue, NULL,
                                      true, false, l_fifoType);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_fifoRc);

        // TODO: JIRA: PFSBE-408
        // Need to fill tag for SBE and EKB.

        // Filling image information that is used in the running SBE firmware
        l_rc = fillImagesDetails(l_capRespMsg);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to fill images information, RC[0x%08x]",
                      l_rc);
        }

        // Filling capabilities details
        fillCapabilitiesDetails(l_capRespMsg.iv_capability);

        fapi2::sbefifo_hwp_data_ostream ostream(l_fifoType);
        l_fifoRc = ostream.put((sizeof(l_capRespMsg) / sizeof(uint32_t)),
                               (uint32_t*)&l_capRespMsg);
        if (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      "Failed to send GetCapability chip-op response RC[0x%08x]",
                      l_fifoRc);
            break;
        }
    } while(false);

    if (l_fifoRc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(l_hdr, &l_ffdc, l_fifoType);
        if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC \
                      " Failed to send response header for getCapabilities " \
                      " RC[0x%08x]", l_rc);
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;
    #undef SBE_FUNC
}
