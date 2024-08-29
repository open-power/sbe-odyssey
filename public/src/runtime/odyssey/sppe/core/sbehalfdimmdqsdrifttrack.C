/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbehalfdimmdqsdrifttrack.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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

#include "sbehalfdimmdqsdrifttrack.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_handler.H"
#include "ody_half_dimm_dqs_track.H"

using namespace fapi2;


uint32_t sbeHalfDimmDqsDrifTrack(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeHalfDimmDqsDrifTrack "
    SBE_ENTER(SBE_FUNC);

    ReturnCode l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

    // Handles reading out the FIFO information and running the HWP
    do
    {
        uint32_t len2dequeue  = 0;
        fifoRc = sbeUpFifoDeq_mult(len2dequeue, NULL, true, false, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
        SBE_EXEC_HWP(l_rc, ody_half_dimm_dqs_track, l_ocmb_chip);
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
