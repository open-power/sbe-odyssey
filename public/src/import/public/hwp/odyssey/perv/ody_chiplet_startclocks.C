/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_startclocks.C $ */
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
/// @file  ody_chiplet_startclocks.C
/// @brief Align and start chiplet clocks, drop chiplet fence
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_chiplet_startclocks.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_chiplet_startclocks.H>
#include <ody_scom_perv.H>
#include <target_filters.H>

using namespace fapi2;
using namespace cc;
using namespace scomt::perv;

SCOMT_PERV_USE_CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_PERV_CTRL0;
typedef CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0_t;

SCOMT_PERV_USE_TCMC_CPLT_CONF1;
SCOMT_PERV_USE_TCMC_SYNC_CONFIG;

enum ODY_CHIPLET_STARTCLOCKS_Private_Constants
{
    DELAY_1us = 1000, // unit is nano seconds
    SIM_CYCLE_DELAY = 1000, // unit is sim cycles
    ODY_MC_PUB_PRIM = ODY_MC_PUB0 | ODY_MC_PUB1 | ODY_MC_PRIM0 | ODY_MC_PRIM1,
    ODY_MC_ALL_BUT_ATPG = ODY_MC_PUB_PRIM | ODY_MC_PERV | ODY_MC_IOO | ODY_MC_CORE | ODY_MC_CFG | ODY_MC_DFI,
};

ReturnCode ody_chiplet_startclocks(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    PERV_CTRL0_t PERV_CTRL0;
    TCMC_CPLT_CONF1_t  CPLT_CONF1;
    TCMC_SYNC_CONFIG_t SYNC_CONFIG;
    auto l_mc_NO_TP = i_target.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_INF("Drop TP chiplet fence");
    PERV_CTRL0 = 0;
    PERV_CTRL0.set_PERV_CHIPLET_FENCE(1);
    FAPI_TRY(PERV_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Deassert ATPGMODE_PUBMAC (asserted in chiplet_reset)");
    CPLT_CONF1 = 0;
    CPLT_CONF1.setBit<16>();
    FAPI_TRY(CPLT_CONF1.putScom_CLEAR(l_mc_NO_TP));

    FAPI_INF("Reset DDR PHY");
    FAPI_TRY(CPLT_CONF1.getScom(l_mc_NO_TP));
    CPLT_CONF1.clearBit<6>();
    CPLT_CONF1.clearBit<7>();
    CPLT_CONF1.setBit<2>();
    CPLT_CONF1.setBit<3>();
    CPLT_CONF1.setBit<10>();
    CPLT_CONF1.setBit<11>();
    CPLT_CONF1.clearBit<14>();
    CPLT_CONF1.clearBit<15>();
    FAPI_TRY(CPLT_CONF1.putScom(l_mc_NO_TP));

    FAPI_INF("Align chiplet since we're about to start some clocks");
    FAPI_TRY(SYNC_CONFIG.getScom(l_mc_NO_TP));
    SYNC_CONFIG.set_SYNC_PULSE_INPUT_DIS(1);
    FAPI_TRY(SYNC_CONFIG.putScom(l_mc_NO_TP));

    FAPI_TRY(mod_align_regions(l_mc_NO_TP, REGION_ALL));

    FAPI_INF("Start PHY clocks to make the functional reset propagate.");
    FAPI_INF("The PHY won't be fully cleaned up at this point so we don't drop fences just yet.");
    FAPI_TRY(mod_start_stop_clocks(l_mc_NO_TP, ODY_MC_PUB_PRIM, CLOCK_TYPE_ALL, true, false));

    FAPI_INF("Assert DDR PHY PWROKIN to complete PHY reset sequence");
    CPLT_CONF1 = 0;
    CPLT_CONF1.setBit<6>();
    CPLT_CONF1.setBit<7>();
    FAPI_TRY(CPLT_CONF1.putScom_SET(l_mc_NO_TP));

    FAPI_INF("Give the PHY some time to reset - 64 clock cycles ain't much but let's be safe");
    FAPI_TRY(delay(DELAY_1us, SIM_CYCLE_DELAY));

    // At this point the DDR PHY is in reset state; leave it to the memory code to take it out of there
    FAPI_INF("Start remaining clocks (and drop region fences)");
    FAPI_TRY(poz_chiplet_startclocks(l_mc_NO_TP, ODY_MC_ALL_BUT_ATPG));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
