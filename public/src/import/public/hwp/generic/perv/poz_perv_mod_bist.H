/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_bist.H $ */
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
/// @file  poz_perv_mod_bist.H
/// @brief Prototype declarations for poz_bist module functions
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <poz_perv_common_params.H>
#include <poz_bist.H>

using namespace cc;

/// @brief Poll for and exit upon OPCG_DONE, BIST_HALT, or (optionally) ABIST_DONE.
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_poll_abist_done         Inspect ABIST_DONE alongside the other bits
/// @param[in]   i_assert_abist_done       Assert ABIST_DONE is set at the end of the procedure
/// @param[in]   i_max_polls               Max number of polls to issue (>0x7FFFFFFF for infinite)
/// @param[in]   i_poll_delay_hw           Delay in terms of nS between polls on hardware environment
/// @param[in]   i_poll_delay_sim          Delay in terms of simcycles between polls on simulation environment
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_bist_poll(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > & i_target,
    bool i_poll_abist_done = true,
    bool i_assert_abist_done = true,
    uint32_t i_max_polls = 400,
    uint32_t i_poll_delay_hw = 200000,
    uint32_t i_poll_delay_sim = 1120000);

/// @brief BIST SCOM reg cleanup module.
/// --Zero OPCG_REG0/1/2, SCAN_REGION_TYPE, CLK_REGION, BIST, and OPCG_CAPT1/2/3 registers
/// --Restore specific bits of CPLT_CTRL0, CPLT_CTRL1, NET_CTRL0, and BH_SCRATCH_REG registers
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_bist_reg_cleanup(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target);

/// @brief LBIST setup module.
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_params                  Reference to the bist_params struct
/// @param[in]   i_ctrl_chiplets           Mask of chiplets to set up for control mode (if 0, don't configure)
/// @param[in]   i_lbist_sequence          Enumerated value of single sequence to configure
/// @param[in]   i_lbist_weight            Enumerated value of single weight to configure
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_lbist_setup(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    const bist_params& i_params,
    const uint64_t i_ctrl_chiplets = 0,
    const uint16_t i_lbist_sequence = 0x8000,
    const uint16_t i_lbist_weight = 0x8000);
