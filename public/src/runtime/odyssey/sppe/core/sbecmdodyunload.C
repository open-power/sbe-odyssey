/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdodyunload.C $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
#include "sbecmdodyunload.H"
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
#include "plat_hwp_data_stream.H"
#include <stdint.h>

#include <ody_omi_unload.H>

using namespace fapi2;

/////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeHssOdyUnload(uint8_t *i_pArg)
{
#define SBE_FUNC " sbeHssOdyUnload "
    SBE_ENTER(SBE_FUNC);

    constexpr uint32_t c_chipunit_mask = 0;

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    uint32_t l_params[64];
    bool needsDrain = true;

    Target<SBE_ROOT_CHIP_TYPE> tgt = g_platTarget->plat_getChipTarget();

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);
        needsDrain = false;

        fapi2::sbefifo_hwp_data_ostream l_ostream(type);
        hwp_bit_ostream l_bit_os(l_ostream);
        // Call the wrapper
        fapi2::ReturnCode fapi_rc = fapi2::FAPI2_RC_SUCCESS;
        SBE_EXEC_HWP( fapi_rc, ody_omi_unload, tgt, c_chipunit_mask, l_bit_os);
        if (fapi_rc != fapi2::FAPI2_RC_SUCCESS)
        {
            hdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE, SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
            ffdc.setRc(fapi_rc);
            break;
        }
    } while(false);

    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        if (needsDrain)
        {
            uint32_t len = 0;
            uint32_t rc = sbeUpFifoDeq_mult(len, l_params, true, true, type);
            if (rc != SBE_SEC_OPERATION_SUCCESSFUL) // FIFO access issue
            {
                SBE_ERROR(SBE_FUNC "FIFO EOT flush failed, rc[0x%X]", rc);
            }
        }

        hdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
    }
    do
    {
        SBE_TRY(sbeDsSendRespHdr(hdr, &ffdc, type));
    } while (0);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC

}
