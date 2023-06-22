/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_reset.C $ */
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
    HEARTBEAT_POLL_COUNT = 20,    // count to wait for chiplet heartbeat
    HEARTBEAT_SIM_CYCLE_DELAY = 70000, // unit is cycles
    HEARTBEAT_HW_NS_DELAY = 10000, // unit is nano seconds
};

ReturnCode poz_chiplet_reset(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                             const uint8_t i_chiplet_delays[64],
                             const uint8_t i_sync_pulse_delay,
                             const poz_chiplet_reset_phases i_phases)
{
    NET_CTRL0_t NET_CTRL0;
    SYNC_CONFIG_t SYNC_CONFIG;
    OPCG_ALIGN_t OPCG_ALIGN;
    CPLT_CTRL2_t CPLT_CTRL2;
    CPLT_CTRL3_t CPLT_CTRL3;
    HEARTBEAT_REG_t HEARTBEAT_REG;
    buffer<uint32_t> l_attr_pg;
    buffer<uint64_t> l_data64;
    uint32_t l_poll_count;
    uint8_t  l_sync_pulse_delay;

    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_BITX > l_chiplets_bitx;
    std::vector<Target<TARGET_TYPE_PERV>> l_chiplets_uc;

    FAPI_INF("Entering ...");
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, &l_chiplets_uc, MCGROUP_ALL));
    l_chiplets_bitx = l_chiplets_mc;

    if (i_phases & PRE_SCAN0)
    {
        FAPI_INF("Enable and reset chiplets");
        NET_CTRL0 = 0;
        NET_CTRL0.set_PCB_EP_RESET(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));

        NET_CTRL0 = 0;
        NET_CTRL0.set_PCB_EP_RESET(1);
        FAPI_TRY(NET_CTRL0.putScom_CLEAR(l_chiplets_mc));

        NET_CTRL0 = 0;
        NET_CTRL0.set_CHIPLET_EN(1);
        NET_CTRL0.setBit<23>();   // SRAM_ENABLE
        FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));

        l_poll_count = HEARTBEAT_POLL_COUNT;

        while (l_poll_count != 0)
        {
            FAPI_INF("Reading HEARTBEAT_REG register to check HEARTBEAT_DEAD");
            FAPI_TRY(HEARTBEAT_REG.getScom(l_chiplets_bitx));

            if (!HEARTBEAT_REG)
            {
                break;
            }

            FAPI_TRY(fapi2::delay(HEARTBEAT_HW_NS_DELAY, HEARTBEAT_SIM_CYCLE_DELAY));
            --l_poll_count;
        }

        FAPI_DBG("HEARTBEAT Poll Count : %d", l_poll_count);

        FAPI_ASSERT(l_poll_count > 0,
                    fapi2::POZ_HEARTBEAT_NOT_SET_ERR()
                    .set_PERV_HEARTBEAT_REG(HEARTBEAT_REG)
                    .set_POLL_COUNT(HEARTBEAT_POLL_COUNT)
                    .set_PROC_TARGET(i_target),
                    "ERROR:HEARTBEAT NOT RUNNING");

        FAPI_INF("Set up clock controllers with sync pulse delay of %d cycles", i_sync_pulse_delay);
        SYNC_CONFIG = 0;
        l_sync_pulse_delay = (i_sync_pulse_delay == 8) ? 0 : i_sync_pulse_delay - 1;
        SYNC_CONFIG.set_SYNC_PULSE_DELAY(l_sync_pulse_delay);
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
    }

    if (i_phases & SCAN0_AND_UP)
    {
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
