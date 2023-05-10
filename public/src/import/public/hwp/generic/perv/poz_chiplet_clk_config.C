/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_clk_config.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
/// @brief Enable clocks for chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_clk_config.H>
#include <poz_chiplet_clk_config_regs.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_misc.H>
#include <poz_perv_utils.H>
#include <target_filters.H>

using namespace fapi2;

enum POZ_CHIPLET_CLK_CONFIG_Private_Constants
{
};

ReturnCode poz_chiplet_clk_config(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    chiplet_mux_setup_FP_t i_mux_setup)
{
    NET_CTRL0_t NET_CTRL0;
    CPLT_CTRL0_t CPLT_CTRL0;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    uint64_t l_chiplet_mask;

    FAPI_INF("Entering ...");
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, NULL, MCGROUP_ALL));
    l_chiplet_mask = get_mc_group_members(l_chiplets_mc);

    FAPI_DBG("Inhibit PLAT flush on Pervasive chiplet");
    // Some pervasive PLATs are needed to facilitate synchronization
    // between chiplets, so make sure they're not flushed
    // (this is important for hotplug and won't hurt in IPL)
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_FLUSHMODE_INH(1);
    FAPI_TRY(CPLT_CTRL0.putScom_SET(get_tp_chiplet_target(i_target)));

    FAPI_INF("Set up chiplet clock muxing");
    FAPI_TRY(i_mux_setup(i_target, l_chiplet_mask));

    FAPI_INF("Enable chiplet clocks");
    NET_CTRL0 = 0;
    NET_CTRL0.set_CLK_ASYNC_RESET(1);
    NET_CTRL0.set_LVLTRANS_FENCE(1);
    FAPI_TRY(NET_CTRL0.putScom_CLEAR(l_chiplets_mc));

    NET_CTRL0 = 0;
    NET_CTRL0.set_PLL_FORCE_OUT_EN(1);
    FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
