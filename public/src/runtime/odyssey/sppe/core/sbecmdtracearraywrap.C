/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdtracearraywrap.C $ */
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

#include "sbecmdtracearray.H"
#include "ody_gettracearray.H"

using namespace fapi2;

// HWP call support
ody_gettracearray_FP_t ody_sbe_tracearray_hwp = &ody_gettracearray;


ReturnCode sbeControlTraceArrayCmd_t::executeHwp(
                                    fapi2::sbefifo_hwp_data_ostream& o_stream)
{
    uint32_t fapiRc = FAPI2_RC_SUCCESS;

    /* Execute odyssey HWP */
    Target<SBE_ROOT_CHIP_TYPE>l_fapiTarget = g_platTarget->plat_getChipTarget();

    // Execute Trace Array HWP
    SBE_EXEC_HWP( fapiRc, ody_sbe_tracearray_hwp,l_fapiTarget,
                  sbeControlTraceArrayCmd_t::getArg(), o_stream) ;
    return fapiRc;
}