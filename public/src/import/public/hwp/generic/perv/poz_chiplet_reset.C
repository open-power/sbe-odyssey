/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_reset.C $ */
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
/// @file  poz_chiplet_reset.C
/// @brief Enable and reset chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_reset.H>
#include <poz_chiplet_reset_regs.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_misc.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_utils.H>

using namespace fapi2;

enum POZ_CHIPLET_RESET_Private_Constants
{
    PGOOD_REGIONS_STARTBIT = 4,
    PGOOD_REGIONS_LENGTH = 15,
    PGOOD_REGIONS_OFFSET = 12,
};

ReturnCode poz_chiplet_reset(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, const uint8_t i_chiplet_delays[64])
{
    NET_CTRL0_t NET_CTRL0;
    SYNC_CONFIG_t SYNC_CONFIG;
    OPCG_ALIGN_t OPCG_ALIGN;
    CPLT_CTRL2_t CPLT_CTRL2;
    CPLT_CTRL3_t CPLT_CTRL3;
    buffer<uint32_t> l_attr_pg;
    buffer<uint64_t> l_data64;
    MulticastGroup l_mc_group;

    FAPI_INF("Entering ...");
    FAPI_TRY(get_hotplug_mc_group(i_target, l_mc_group));

    {
        // Initializing chiplets inside a new scope to prevent issues with FAPI_TRY
        auto l_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(l_mc_group);
        auto l_chiplets_uc = l_chiplets_mc.getChildren<TARGET_TYPE_PERV>();

        FAPI_INF("Enable and reset chiplets");
        NET_CTRL0 = 0;
        NET_CTRL0.set_PCB_EP_RESET(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));

        FAPI_TRY(NET_CTRL0.getScom(l_chiplets_mc));
        NET_CTRL0.set_PCB_EP_RESET(0);
        NET_CTRL0.set_CHIPLET_ENABLE(1);
        FAPI_TRY(NET_CTRL0.putScom(l_chiplets_mc));

        FAPI_INF("Set up clock controllers (decimal 9 -> 10 cycles) ");
        SYNC_CONFIG = 0;
        SYNC_CONFIG.set_SYNC_PULSE_DELAY(0b1001);
        FAPI_TRY(SYNC_CONFIG.putScom(l_chiplets_mc));

        FAPI_INF("Set up per-chiplet OPCG delays");

        for (auto& targ : l_chiplets_uc)
        {
            OPCG_ALIGN = 0;
            OPCG_ALIGN.set_INOP_ALIGN(7);
            OPCG_ALIGN.set_INOP_WAIT(0);
            OPCG_ALIGN.set_SCAN_RATIO(3);
            OPCG_ALIGN.set_OPCG_WAIT_CYCLES(0x30 - 4 * i_chiplet_delays[targ.getChipletNumber()]);
            FAPI_TRY(OPCG_ALIGN.putScom(targ));
        }

        FAPI_DBG("scan0 all clock regions, scan types GPTR, TIME, REPR");
        FAPI_TRY(mod_scan0(l_chiplets_mc, cc::REGION_ALL, cc::SCAN_TYPE_RTG));

        FAPI_DBG("scan0 all clock regions, scan types except GPTR, TIME, REPR");
        FAPI_TRY(mod_scan0(l_chiplets_mc, cc::REGION_ALL, cc::SCAN_TYPE_NOT_RTG));

        FAPI_INF("Transfer partial good attributes into region PGOOD and PSCOM enable registers");

        for (auto& targ : l_chiplets_uc)
        {
            FAPI_TRY(FAPI_ATTR_GET(ATTR_PG, targ, l_attr_pg));
            l_attr_pg.invert();
            l_data64.flush<0>();
            l_data64.insert< PGOOD_REGIONS_STARTBIT, PGOOD_REGIONS_LENGTH, PGOOD_REGIONS_OFFSET >(l_attr_pg);
            CPLT_CTRL2 = l_data64();
            FAPI_TRY(CPLT_CTRL2.putScom(targ));
            CPLT_CTRL3 = l_data64();
            FAPI_TRY(CPLT_CTRL3.putScom(targ));
        }
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
