/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_lbist.C $       */
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
/// @file  ody_lbist.C
/// @brief Execute Logic Built In Self Test
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Mike Hamilton (mikehami@us.ibm.com)
//------------------------------------------------------------------------------

#include <ody_lbist.H>
#include <poz_perv_common_params.H>
#include <poz_bist.H>
#include <poz_perv_mod_chiplet_clocking.H>

using namespace fapi2;

static const bist_params ody_lbist_params =
{
    BIST_PARAMS_CURRENT_VERSION,                    ///< BIST_PARAMS_VERSION

    bist_params::SCAN0                  |           ///< 0x0200
    bist_params::ARRAYINIT              |           ///< 0x0100
    bist_params::RING_SETUP             |           ///< 0x0080
    bist_params::RING_PATCH             |           ///< 0x0040
    bist_params::REG_SETUP              |           ///< 0x0020
    bist_params::GO                     |           ///< 0x0010
    bist_params::POLL                   |           ///< 0x0008
    bist_params::REG_CLEANUP            |           ///< 0x0004
    bist_params::COMPARE,                           ///< 0x0002

    bist_params::FAST_DIAGNOSTICS       |           ///< 0x40000000
    bist_params::CHIPLET_FENCE_ACTIVE   |           ///< 0x02000000
    bist_params::INT_MODE               |           ///< 0x00800000
    bist_params::SCAN0_ARY_FILL,                    ///< 0x00400000

    /// 0x0 for chiplets to let hotplug decide which chiplets to BIST
    0x0000000000000000,                             ///< chiplets
    0x0000000000000000,                             ///< uc_go_chiplets
    0x100000,                                       ///< opcg_count
    0x80,                                           ///< idle_count
    0,                                              ///< linear_stagger
    0,                                              ///< zigzag_stagger
    0x77701186FE407080,                             ///< opcg_align
    0x4010000000000008,                             ///< lbist_opcg1
    {0},                                            ///< opcg_count_adjust
    100000,                                         ///< max_polls
    200000,                                         ///< poll_delay_hw
    1120000,                                        ///< poll_delay_sim
    cc::SCAN_TYPE_ALL,                              ///< scan0_types

    /// lbist_scan_types
    cc::SCAN_TYPE_NOT_RTG & ~cc::SCAN_TYPE_CMSK,    ///< 0xDCD

    /// base_regions
    cc::ODY_MC_PERV |                               ///< REGION(0)
    cc::ODY_MC_IOO  |                               ///< REGION(1)
    cc::ODY_MC_CORE |                               ///< REGION(2)
    cc::ODY_MC_CFG  |                               ///< REGION(3)
    cc::ODY_MC_DFI  |                               ///< REGION(4)
    cc::ODY_MC_PUB0 |                               ///< REGION(5)
    cc::ODY_MC_PUB1,                                ///< REGION(6)

    0,                                              ///< padding
    {0},                                            ///< chiplets_regions
    0x1000,                                         ///< outer_loop_mask
    0x0400,                                         ///< inner_loop_mask
    "system",                                       ///< program
    "none",                                         ///< ring_patch
};

enum ODY_LBIST_Private_Constants
{
};

ReturnCode ody_lbist(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");

    bist_return ody_lbist_return;
    auto l_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_TRY(poz_bist(i_target, ody_lbist_params, ody_lbist_return));

    // Post-BIST scan0 for IML with BIST
    FAPI_TRY(mod_scan0(l_chiplets_mc, cc::REGION_ALL));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
