/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_clk_config.H $ */
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
/// @file  poz_chiplet_clk_config.H
///
/// @brief Enable clocks for chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

typedef fapi2::ReturnCode (*poz_chiplet_clk_config_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&);

/// @brief Callback function for setting up clock muxing
/// @param[in]   i_chip_target       Chip targt to set up
/// @paran[in]   i_chiplet_mask      Mask of chiplets to set up. If bit N is set, set up chiplet ID N, otherwise don't.
/// @return      usual FAPI2 return codes
typedef fapi2::ReturnCode (*chiplet_mux_setup_FP_t)(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_chip_target,
    const fapi2::buffer<uint64_t> i_chiplet_mask);

/// @brief Enable clocks for chiplets
/// @param[in]   i_target              Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]   i_mux_setup           Function pointer to a function that will set up clock muxing
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_chiplet_clk_config(
        const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
        chiplet_mux_setup_FP_t i_mux_setup);
}
