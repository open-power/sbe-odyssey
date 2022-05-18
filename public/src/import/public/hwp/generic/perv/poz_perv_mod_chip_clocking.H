/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chip_clocking.H $ */
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
/// @file  poz_perv_mod_chip_clocking.H
/// @brief Chip PLL lock checking
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <poz_perv_common_params.H>

using namespace pll;

/// @brief Chip PLL lock checking
///
/// @param[in]   i_target                  Reference to target
/// @param[in]   i_pll_mask                Select pll_mask bits
///
/// @return  FAPI2_RC_SUCCESS if success, else error code
fapi2::ReturnCode mod_poll_pll_lock(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV |  fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > &i_target,
    pll_lock_bits i_pll_mask);

/// @brief   Poll for pll lock using FSI2PIB status register for cases where the PCB is not up yet.
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_ANY_POZ
/// @param[in]   i_pll_mask                Select pll_mask bits
///
/// @return  FAPI2_RC_SUCCESS if success, else error code
fapi2::ReturnCode mod_poll_pll_lock_fsi2pib(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target,
    pll_lock_bits i_pll_mask);
