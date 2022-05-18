/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_unused_psave.C $ */
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
//------------------------------------------------------------------------------
/// @file  poz_chiplet_unused_psave.C
/// @brief Place non-functional chiplets into low power state
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_unused_psave.H>
#include <poz_chiplet_unused_psave_regs.H>
#include <poz_perv_common_params.H>

using namespace fapi2;


enum POZ_CHIPLET_UNUSED_PSAVE_Private_Constants
{
};

ReturnCode poz_chiplet_unused_psave(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    NET_CTRL0_t NET_CTRL0;
    auto l_chiplets_uc = i_target.getChildren<TARGET_TYPE_PERV>(TARGET_STATE_PRESENT);

    FAPI_INF("Entering ...");
    FAPI_DBG("put all non-functional chiplets into a state of minimal power usage");

    for (auto& targ : l_chiplets_uc)
    {
        if (!targ.isFunctional())
        {
            FAPI_TRY(NET_CTRL0.getScom(targ));
            NET_CTRL0.set_CHIPLET_ENABLE(0);
            NET_CTRL0.set_PCB_EP_RESET(1);
            NET_CTRL0.set_TP_FENCE_PCB(1);
            FAPI_TRY(NET_CTRL0.putScom(targ));

            NET_CTRL0.set_PLLFORCE_OUT_EN(0);
            NET_CTRL0.set_CLK_ASYNC_RESET(1);
            NET_CTRL0.set_LVLTRANS_FENCE(1);
            FAPI_TRY(NET_CTRL0.putScom(targ));
        }
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
