/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_scratch_regs_utils.H $ */
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
/// @file  ody_scratch_regs_utils.H
/// @brief Project specific utility functions to support Odyssey scratch
///        register setup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

#ifndef __PPE__
///
/// @brief Read Host attribute/platform state to determine desired OCMB PLL bucket
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
/// @param[out]  o_pll_bucket              Target PLL bucket setting
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode ody_scratch_regs_get_pll_bucket(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    fapi2::ATTR_OCMB_PLL_BUCKET_Type& o_pll_bucket);
#endif

///
/// @brief Lookup frequencies associated with a given PLL bucket
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
/// @param[in]   i_pll_bucket              PLL bucket setting
/// @param[out]  o_freq_grid_mhz           Chip grid freqeuncy (MHz)
/// @param[out]  o_freq_link_mhz           OMI link frequency (MHz)
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode ody_scratch_regs_get_pll_freqs(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const fapi2::ATTR_OCMB_PLL_BUCKET_Type i_pll_bucket,
    uint32_t& o_freq_grid_mhz,
    uint32_t& o_freq_link_mhz);

/// @brief Set platform specific multicast setup attributes
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode ody_scratch_regs_setup_plat_multicast_attrs(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);
