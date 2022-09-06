/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_startclocks.C $ */
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
/// @file  poz_chiplet_startclocks.C
/// @brief Align and start chiplet clocks, drop chiplet fence
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_startclocks.H>
#include <poz_chiplet_startclocks_regs.H>
#include <poz_perv_mod_chiplet_clocking.H>

using namespace fapi2;

enum POZ_CHIPLET_STARTCLOCKS_Private_Constants
{
};

ReturnCode poz_chiplet_startclocks(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &i_target,
    uint16_t i_clock_regions)
{
    CPLT_CTRL0_t CPLT_CTRL0;
    SYNC_CONFIG_t SYNC_CONFIG;
    NET_CTRL0_t NET_CTRL0;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_COMPARE > l_mcast_cmp_target = i_target;

    FAPI_INF("Entering ...");

    FAPI_INF("Switch ABIST and sync clock muxes to functional state");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_ABSTCLK_MUXSEL(1);
    CPLT_CTRL0.set_SYNCCLK_MUXSEL(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Disable listen to sync");
    FAPI_TRY(SYNC_CONFIG.getScom(l_mcast_cmp_target));
    SYNC_CONFIG.set_SYNC_PULSE_INPUT_DIS(1);
    FAPI_TRY(SYNC_CONFIG.putScom(i_target));

    FAPI_INF("Align chiplets");
    FAPI_TRY(mod_align_regions(i_target, i_clock_regions));

    FAPI_INF("Drop chiplet fence");
    // Drop fences before starting clocks because fences are DC and might glitch
    NET_CTRL0 = 0;
    NET_CTRL0.set_FENCE_EN(1);
    FAPI_TRY(NET_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Start chiplet clocks");
    FAPI_TRY(mod_start_stop_clocks(i_target, i_clock_regions));

    FAPI_INF("Put PLATs into flush mode");
    CPLT_CTRL0 = 0;
    CPLT_CTRL0.set_FLUSHMODE_INH(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
