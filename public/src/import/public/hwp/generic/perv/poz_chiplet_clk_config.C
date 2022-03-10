/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_clk_config.C $ */
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
/// @file  poz_chiplet_clk_config.C
///
/// @brief Enable clocks for chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "poz_chiplet_clk_config.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_misc.H"
#include "poz_perv_utils.H"
#include <target_filters.H>
#include <p11_scom_perv.H>

SCOMT_PERV_USE_NET_CTRL0;

using namespace fapi2;
using namespace scomt::perv;

enum POZ_CHIPLET_CLK_CONFIG_Private_Constants
{
};

ReturnCode poz_chiplet_clk_config(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    NET_CTRL0_t NET_CTRL0;
    MulticastGroup l_mc_group;

    FAPI_INF("Entering ...");
    FAPI_TRY(get_hotplug_mc_group(i_target, l_mc_group));

    {
        // Initializing chiplets inside a new scope to prevent issues with FAPI_TRY
        auto l_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(l_mc_group);

        FAPI_INF("Set up chiplet clock muxing (TBD)");
        // TBD, not needed on Tap

        FAPI_INF("Enable chiplet clocks");
        NET_CTRL0 = 0;
        NET_CTRL0.set_CLK_ASYNC_RESET(1);
        NET_CTRL0.set_LVLTRANS_FENCE(1);
        FAPI_TRY(NET_CTRL0.putScom_AND(l_chiplets_mc));

        NET_CTRL0 = 0;
        NET_CTRL0.set_PLLFORCE_OUT_EN(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
