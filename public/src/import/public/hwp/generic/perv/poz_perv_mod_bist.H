/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_bist.H $ */
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
/// @file  poz_perv_mod_bist.H
/// @brief Prototype declarations for modules bist_poll and bist_reg_cleanup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <poz_perv_common_params.H>

using namespace cc;

/// @brief Poll OPCG done bit to check for run-N completeness.
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_bist_poll(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > & i_target);

/// @brief BIST SCOM reg cleanup module.
/// --Clear OPCG_REG0, CLK_REGION, CPLT_CTRL0 & BIST registers
/// --Clear ABSTCLK_MUXSEL_DC
/// --Clear TC_SRAM_ABIST_MODE_DC
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_clear_sram_abist_mode   Clear SRAM ABIST mode
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_bist_reg_cleanup(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    bool i_clear_sram_abist_mode = true);
