/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_scratch_regs_utils.C $ */
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
/// @file  ody_scratch_regs_utils.C
/// @brief Project specific utility functions to support Odyssey scratch
///        register setup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_scratch_regs_utils.H>
#include <ody_frequency_buckets.H>

#ifndef __PPE__
fapi2::ReturnCode ody_scratch_regs_get_pll_bucket(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    fapi2::ATTR_OCMB_PLL_BUCKET_Type& o_pll_bucket)
{
    FAPI_DBG("Start");
    fapi2::ATTR_IS_SIMULATION_Type l_is_simulation;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IS_SIMULATION, fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(), l_is_simulation));

    if (l_is_simulation)
    {
        // sim setting reserved for VBU usage
        o_pll_bucket = 14;
    }
    else
    {
        fapi2::Target<fapi2::TARGET_TYPE_OMI> l_omi_target;
        fapi2::Target<fapi2::TARGET_TYPE_MC> l_mc_target;
        fapi2::Target<fapi2::TARGET_TYPE_HUB_CHIP> l_host_target;
        fapi2::ATTR_FREQ_MC_MHZ_Type l_attr_freq_mc_mhz;
        bool l_match_found = false;
        o_pll_bucket = 0;

        FAPI_TRY(i_target.getOtherEnd(l_omi_target));
        l_mc_target = l_omi_target.getParent<fapi2::TARGET_TYPE_MC>();
        l_host_target = l_omi_target.getParent<fapi2::TARGET_TYPE_HUB_CHIP>();

        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_FREQ_MC_MHZ, l_mc_target, l_attr_freq_mc_mhz),
                 "Error from FAPI_ATTR_GET (ATTR_FREQ_MC_MHZ)");

        for (auto l_ocmb_bucket = 0; l_ocmb_bucket < ODY_MAX_PLL_BUCKETS; l_ocmb_bucket++)
        {
            auto l_ocmb_bucket_descriptor = ODY_PLL_BUCKETS[l_ocmb_bucket];

            if (l_ocmb_bucket_descriptor.freq_grid_mhz == l_attr_freq_mc_mhz)
            {
                o_pll_bucket = l_ocmb_bucket;
                l_match_found = true;
                break;
            }
        }

        FAPI_ASSERT(l_match_found,
                    fapi2::ODY_SCRATCH_REGS_UTILS_LOOKUP_ERR()
                    .set_TARGET_CHIP(i_target)
                    .set_HOST_TARGET(l_host_target)
                    .set_HOST_FREQ_GRID_MHZ(l_attr_freq_mc_mhz),
                    "Requested Ody frequency (%d MHz) not found in supported frequency bucket list!",
                    l_attr_freq_mc_mhz);
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
#endif

fapi2::ReturnCode ody_scratch_regs_setup_plat_multicast_attrs(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    // nothing specific needed in this branch
    return fapi2::FAPI2_RC_SUCCESS;
}
