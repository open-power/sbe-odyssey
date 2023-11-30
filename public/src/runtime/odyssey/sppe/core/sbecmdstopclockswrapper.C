/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdstopclockswrapper.C $ */
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

#include "sbecmdstopclocks.H"
#include "sbeFifoMsgUtils.H"
#include "ody_stopclocks.H"
#include "poz_common_stopclocks_flags.H"


using namespace fapi2;

#define CHIPLET_NUMBER_MC   8u

sbeSecondaryResponse sbeStopClocksReqMsgHdr_t::validateParams(void)
{
    sbeSecondaryResponse rc = SBE_SEC_INVALID_TARGET_TYPE_PASSED;

    if ((iv_logTargetType == LOG_TARGET_TYPE_OCMB_CHIP) ||
        (iv_logTargetType == LOG_TARGET_TYPE_PERV &&
         iv_instanceId == CHIPLET_NUMBER_MC) )
    {
        plat_target_handle_t tgtHndl;
        rc = g_platTarget->getSbePlatTargetHandle(iv_logTargetType, iv_instanceId, tgtHndl);
    }
    return rc;
}


ReturnCode sbeStopClocksReqMsgHdr_t::executeHwp( void )
{
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    // For OCMB and MC target stop the MC region clock
    // Sbe can't stopclock the TP, Vital, SBE target.
    const StopClocksFlags i_stopClockFlag = STOP_REGION_MC;

    /* Execute odyssey HWP */
    Target<SBE_ROOT_CHIP_TYPE> l_fapiTarget =  g_platTarget->plat_getChipTarget();
    SBE_EXEC_HWP(fapiRc, ody_stopclocks, l_fapiTarget, i_stopClockFlag);
    return fapiRc;
}