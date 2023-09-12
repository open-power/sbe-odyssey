/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_nest_enable_io.C $ */
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
/// @file  poz_nest_enable_io.C
/// @brief Enable chip/nest drivers and receivers
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_nest_enable_io.H>
#include <poz_nest_enable_io_regs.H>
#include <poz_perv_common_params.H>
#include <poz_perv_utils.H>
#include <target_filters.H>

using namespace fapi2;

enum POZ_NEST_ENABLE_IO_Private_Constants
{
};

ReturnCode poz_nest_enable_io(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    ROOT_CTRL1_t ROOT_CTRL1;
    NET_CTRL0_t NET_CTRL0;

    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;

    FAPI_INF("Entering poz_nest_enable_io...");
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, NULL, MCGROUP_ALL));

    FAPI_DBG("Allow PHB control");
    ROOT_CTRL1 = 0;
    ROOT_CTRL1.set_GLOBAL_PERST_OVERRIDE(1);
    FAPI_TRY(ROOT_CTRL1.putScom_CLEAR(i_target));

    FAPI_INF("Enable TP drivers");
    ROOT_CTRL1 = 0;
    ROOT_CTRL1.set_TP_RI_DC_N(1);
    ROOT_CTRL1.set_TP_DI1_DC_N(1);
    ROOT_CTRL1.set_TP_DI2_DC_N(1);
    FAPI_TRY(ROOT_CTRL1.putScom_SET(i_target));

    FAPI_INF("Chiplet receiver enable, Chiplet driver enable.");
    NET_CTRL0 = 0;
    NET_CTRL0.set_DCTRL(1);
    NET_CTRL0.set_RCTRL(1);
    NET_CTRL0.set_RCTRL2(1);
    NET_CTRL0.setBit<30>(); // PHY power on
    FAPI_TRY(NET_CTRL0.putScom_SET(l_chiplets_mc));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
