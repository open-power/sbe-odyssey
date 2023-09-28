/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_abist.C $       */
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
/// @file  ody_abist.C
/// @brief Execute Array Built In Self Test
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include "ody_abist.H"
#include "poz_bist.H"
#include <poz_perv_common_params.H>

using namespace fapi2;

static const bist_params ody_abist_params =
{
    BIST_PARAMS_CURRENT_VERSION,        ///< BIST_PARAMS_VERSION

    bist_params::SCAN0              |   ///< 0x0200
    bist_params::RING_SETUP         |   ///< 0x0080
    bist_params::RING_PATCH         |   ///< 0x0040
    bist_params::REG_SETUP          |   ///< 0x0020
    bist_params::GO                 |   ///< 0x0010
    bist_params::POLL               |   ///< 0x0008
    bist_params::REG_CLEANUP        |   ///< 0x0004
    bist_params::COMPARE,               ///< 0x0002

    bist_params::ABIST_NOT_LBIST    |   ///< 0x80000000
    bist_params::POLL_ABIST_DONE    |   ///< 0x08000000
    bist_params::ASSERT_ABIST_DONE  |   ///< 0x04000000
    bist_params::SCAN0_ARY_FILL     |   ///< 0x00400000
    bist_params::SYS_BIST_PAK,          ///< 0x00200000

    /// 0x0 for chiplets to let hotplug decide which chiplets to BIST
    0x0000000000000000,                 ///< chiplets
    0x0000000000000000,                 ///< uc_go_chiplets
    135000000,                          ///< opcg_count
    0x20000,                            ///< idle_count
    0,                                  ///< linear_stagger
    0,                                  ///< zigzag_stagger
    0,                                  ///< opcg_align
    0,                                  ///< lbist_opcg1
    {0},                                ///< opcg_count_adjust
    10000,                              ///< max_polls
    200000,                             ///< poll_delay_hw
    1120000,                            ///< poll_delay_sim
    cc::SCAN_TYPE_ALL,                  ///< scan0_types
    0,                                  ///< lbist_scan_types
    cc::REGION_ALL,                     ///< base_regions
    0,                                  ///< padding
    {0},                                ///< chiplets_regions
    0x8000,                             ///< outer_loop_mask
    0x8000,                             ///< inner_loop_mask
    "istep",                            ///< program
    "none",                             ///< ring_patch
};

enum ODY_ABIST_Private_Constants
{
};

ReturnCode ody_abist(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");

    ReturnCode l_rc;
    bist_diags l_bist_diags = {0};

    // Dummy variables to support poz_bist API, but unused in istep mode
    hwp_data_unit dbuff[128 * 4]; // SCA TODO: implement a filestream instead??
    hwp_array_ostream l_stream(dbuff, (sizeof(dbuff) / sizeof((dbuff)[0])));

    FAPI_EXEC_HWP(l_rc, poz_bist, i_target, ody_abist_params, l_bist_diags, l_stream, BIST_CHIPOP_ID);
    FAPI_TRY(l_rc);

    if (l_bist_diags.completed_stages & ody_abist_params.bist_stages::COMPARE)
    {
        for (uint8_t chiplet_id = 0; chiplet_id < 64; chiplet_id++)
        {
            if (l_bist_diags.failing_regions[chiplet_id])
            {
                FAPI_ERR("Nonzero scan compare fail count");
            }
        }
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
