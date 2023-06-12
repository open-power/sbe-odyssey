/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_pll_setup.C $ */
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
/// @file  poz_chiplet_pll_setup.C
/// @brief Start chiplet PLLs & Check for PLL lock
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_pll_setup.H>
#include <poz_chiplet_pll_setup_regs.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_chip_clocking.H>
#include <poz_perv_utils.H>
#include <target_filters.H>

using namespace fapi2;

enum POZ_CHIPLET_PLL_SETUP_Private_Constants
{
};

ReturnCode poz_chiplet_pll_setup(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    fapi2::MulticastGroup i_mcgroup)
{
    ATTR_IO_TANK_PLL_BYPASS_Type l_attr_io_tank_pll_bypass;
    CPLT_CTRL1_t CPLT_CTRL1;
    NET_CTRL0_t NET_CTRL0;
    PCB_RESPONDER_CONFIG_t PCB_RESPONDER_CONFIG;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    std::vector<Target<TARGET_TYPE_PERV>> l_chiplets_uc;

    FAPI_INF("Entering ...");
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, &l_chiplets_uc, i_mcgroup));

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IO_TANK_PLL_BYPASS, i_target, l_attr_io_tank_pll_bypass));

    if (!l_attr_io_tank_pll_bypass)
    {

        FAPI_INF("Drop PLL region fences");
        CPLT_CTRL1 = 0;
        CPLT_CTRL1.set_REGION13_FENCE(1);
        FAPI_TRY(CPLT_CTRL1.putScom_CLEAR(l_chiplets_mc));

        //with all chiplets except TP via multicast:
        FAPI_INF("Start chiplet PLLs");
        NET_CTRL0 = 0;
        NET_CTRL0.set_PLL_TEST_EN(1);
        FAPI_TRY(NET_CTRL0.putScom_CLEAR(l_chiplets_mc));

        NET_CTRL0 = 0;
        NET_CTRL0.set_PLL_RESET(1);
        FAPI_TRY(NET_CTRL0.putScom_CLEAR(l_chiplets_mc));

        FAPI_INF("Check for PLL lock");
        FAPI_TRY(mod_poll_pll_lock(l_chiplets_mc, pll::ALL_PLLS));

        FAPI_INF("Release PLL bypass");
        NET_CTRL0 = 0;
        NET_CTRL0.set_PLL_BYPASS(1);
        FAPI_TRY(NET_CTRL0.putScom_CLEAR(l_chiplets_mc));

        FAPI_INF("Enable PLL unlock error reporting");

        for (auto& targ : l_chiplets_uc)
        {
            FAPI_TRY(PCB_RESPONDER_CONFIG.getScom(targ));
            PCB_RESPONDER_CONFIG.set_CFG_MASK_PLL_ERRS(0);
            FAPI_TRY(PCB_RESPONDER_CONFIG.putScom(targ))
        }
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
