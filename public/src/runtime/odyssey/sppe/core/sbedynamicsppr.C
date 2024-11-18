/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbedynamicsppr.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024,2025                        */
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

#include "sbedynamicsppr.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_handler.H"
#include "plat_hwp_data_stream.H"
#include "ody_row_repair.H"

using namespace fapi2;


uint32_t sbeDynamicSPPR(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeDynamicSPPR "
    SBE_ENTER(SBE_FUNC);

    ReturnCode l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    uint64_t address = 0;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

    sbefifo_hwp_data_istream istream(type);

    // Handles reading out the FIFO information and running the HWP
    do
    {
        const uint32_t len = 2;
        fifoRc = istream.get(len, (uint32_t *)&address);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
        SBE_EXEC_HWP(l_rc, mss::ody::row_repair::sppe_dynamic_row_repair, l_ocmb_chip, fapi2::buffer<uint64_t>(address));
        if(l_rc)
        {
            SBE_ERROR(SBE_FUNC "HWP failed with RC : [%08X]", l_rc);
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, SBE_SEC_HWP_FAILURE);
            break;
        }
    }while(0);

    // Handles sending the response header
    // Note: this is done separately than the HWP loop so the response header is always sent
    do
    {
        // Build the response header packet.
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);
        fifoRc = sbeDsSendRespHdr(hdr, &ffdc, type);
        // will let command processor routine handle the failure.

    }while(0);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
