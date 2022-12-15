/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_init.C $     */
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
/// @file  ody_tp_init.C
/// @brief Install pervasive-specific chiplet personalization
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_tp_init.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_misc.H>
#include <multicast_defs.H>
#include <target_filters.H>
#include <ody_scom_perv.H>

using namespace fapi2;
using namespace scomt::perv;

SCOMT_PERV_USE_CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_ROOT_CTRL0;

enum ODY_TP_INIT_Private_Constants
{
};

ReturnCode ody_tp_init(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    // The pre divider settings depend on the PLL bucket. Generally,
    // base_period = (1 / PLL speed) * 4 * (pre_divider + 1)
    // We want a base_period close to 16.666ns so that the PPE hang pulses (which have an internal division by 2)
    // run at 33.3333 ns.
    //
    //           PLL bucket     0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15
    //  PLL frequency [MHz]  1600 2000 2400 2400 ???? ???? ???? ???? ???? ???? ???? ???? ???? ????  267 2133
    //          pre_divider     5    7    9    9    9    9    9    9    9    9    9    9    9    9    0    8
    //     base period [ns]    30   32 33.3 33.3 ???? ???? ???? ???? ???? ???? ???? ???? ???? ????   30 33.8
    static const uint8_t pre_dividers[16] = { 5, 7, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 8 };
    static const hang_pulse_t HANG_COUNTERS_ALL[] = {{0, 14, 0}, {1, 1, 0}, {2, 1, 0}, {5, 4, 0}, {6, 5, 0, 1}} ;

    auto l_mc_allgood = i_target.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD);
    CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;
    ATTR_OCMB_PLL_BUCKET_Type l_pll_bucket;

    // TODO : Set up TOD error routing, error mask via scan inits
    // TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    FAPI_INF("Entering ...");

    FAPI_TRY(FAPI_ATTR_GET(ATTR_OCMB_PLL_BUCKET, i_target, l_pll_bucket));

    FAPI_DBG("Drop GLOBAL_EP_RESET.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_GLOBAL_EP_RESET(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Start using PCB network");
    FAPI_TRY(mod_switch_pcbmux(i_target, mux::PCB2PCB));

    FAPI_DBG("Drop OOB Mux.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_OOB_MUX(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Set up static multicast groups");
    FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL));
    FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL));

    FAPI_INF("Set up chiplet hang pulses (PLL bucket = %d, pre divider = %d)", l_pll_bucket, pre_dividers[l_pll_bucket]);
    FAPI_TRY(mod_hangpulse_setup(l_mc_allgood, pre_dividers[l_pll_bucket], HANG_COUNTERS_ALL));

    FAPI_INF("Miscellaneous TP setup");
    FAPI_TRY(mod_poz_tp_init_common(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
