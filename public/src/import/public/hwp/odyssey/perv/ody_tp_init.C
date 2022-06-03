/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_init.C $     */
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
/// @brief
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SPPE
//------------------------------------------------------------------------------

#include "ody_tp_init.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_misc.H"
#include <multicast_defs.H>
#include <target_filters.H>
#include <p11_scom_perv.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;

using namespace fapi2;
using namespace scomt::perv;

enum ODY_TP_INIT_Private_Constants
{
};

ReturnCode ody_tp_init(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    auto l_mc_allgood = i_target.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD);
    static const hang_pulse_t SETUP_HANG_COUNTERS_PERV[] = {{0, 16, 0}, {1, 1, 0}, {2, 1, 0}, {5, 6, 0}, {6, 7, 0, 1}} ;
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;

    // TODO : Set up TOD error routing, error mask via scan inits
    // TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    FAPI_INF("Entering ...");
    FAPI_DBG("Drop GLOBAL_EP_RESET_DC.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_GLOBAL_EP_RESET_DC(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Start using PCB network");
    FAPI_TRY(mod_switch_pcbmux(i_target, mux::PCB2PCB));

    FAPI_INF("Set up static multicast groups");
    FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL));
    FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL));

    FAPI_INF("Set up chiplet hang pulses");
    FAPI_TRY(mod_hangpulse_setup(l_mc_allgood, 1, SETUP_HANG_COUNTERS_PERV));

    FAPI_INF("Miscellaneous TP setup");
    FAPI_TRY(mod_poz_tp_init_common(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
