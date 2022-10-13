/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_bist_mux_switch.C $ */
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
/// @file  ody_tp_bist_mux_switch.C
/// @brief Switch the mux for IML BIST
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_tp_bist_mux_switch.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_mod_misc.H>
#include <target_filters.H>
#include <poz_perv_utils.H>

using namespace fapi2;
using namespace cc;

enum ODY_TP_BIST_MUX_SWITCH_Private_Constants
{
};

ReturnCode ody_tp_bist_mux_switch(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");

    fapi2::Target<fapi2::TARGET_TYPE_PERV> l_tpchiplet = get_tp_chiplet_target(i_target);

    FAPI_INF("Start using I2C2PCB network");
    FAPI_TRY(mod_switch_pcbmux_cfam(i_target, mux::I2C2PCB))

    FAPI_INF("Calling mod_start_stop_clocks for SBE regions ...");
    FAPI_TRY(mod_start_stop_clocks(l_tpchiplet, ODY_PERV_SBE, CLOCK_TYPE_ALL, false))

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
