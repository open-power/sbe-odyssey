/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbescom.C $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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

#include <stdint.h>
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "plat_hw_access.H"
#include "plat_target.H"
#include "sbescom.H"
#include "ffdc.H"

using namespace fapi2;

void checkIndirectAndDoScom( const bool i_isRead,
                             const uint64_t i_addr,
                             uint64_t & io_data,
                             sbeRespGenHdr_t *const o_hdr,
                             sbeResponseFfdc_t *const o_ffdc,
                             sbeFifoType i_fifoType,
                             uint64_t i_mask )
{

    #define SBE_FUNC " checkIndirectAndDoScom "
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    Target<SBE_ROOT_CHIP_TYPE> l_hndl =  g_platTarget->plat_getChipTarget();
    rc = platcheckIndirectAndDoScom(i_isRead, &l_hndl, i_addr, io_data, fapiRc);

    if (fapiRc != FAPI2_RC_SUCCESS)
    {
        o_hdr->setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                         SBE_SEC_PCB_PIB_ERR);
        if(o_ffdc) o_ffdc->setRc(fapiRc);
    }
    if (rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        o_hdr->setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                         rc);
    }
    SBE_DEBUG(SBE_FUNC "Scom data is 0x%08X_%08X", ((io_data >> 32) & 0xFFFFFFFF), (io_data & 0xFFFFFFFF));
    SBE_DEBUG(SBE_FUNC "fapiRc:%u o_hdr->secondaryStatus:0x%08X", fapiRc, o_hdr->secondaryStatus());
    #undef SBE_FUNC
}
