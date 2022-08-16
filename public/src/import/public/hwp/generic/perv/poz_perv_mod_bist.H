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

/// @brief Poll for and exit upon OPCG_DONE, BIST_HALT, or (optionally) ABIST_DONE.
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_poll_abist_done         Inspect ABIST_DONE alongside the other bits
/// @param[in]   i_assert_abist_done       Assert ABIST_DONE is set at the end of the procedure
/// @param[in]   i_max_polls               Max number of polls to issue (>0x7FFFFFFF for infinite)
/// @param[in]   i_hw_delay                Delay of ns between polls on hardware
/// @param[in]   i_sim_delay               Delay of simcycles between polls on sim
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_bist_poll(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > & i_target,
    bool i_poll_abist_done = true,
    bool i_assert_abist_done = true,
    uint32_t i_max_polls = 400,
    uint32_t i_hw_delay = 200000,
    uint32_t i_sim_delay = 1120000);

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

/// @brief LBIST setup module.
/// --TODO update me once functionality has been built out
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_clock_regions           Select clock regions
/// @param[in]   i_runn_cycles             Loop count value to set OPCG run-N mode (>0x7FFFFFFFFFF for infinite)
/// @param[in]   i_lbist_start_at          Idle count value to set in OPCG_REG1
/// @param[in]   i_lbist_start_stagger     Per target OPCG_REG1 stagger value
/// @param[in]   i_lbist_sequence          Enumerated value of single sequence to configure
/// @param[in]   i_lbist_weight            Enumerated value of single weight to configure
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_lbist_setup(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_clock_regions,
    uint64_t i_runn_cycles = 0x42FFF,
    uint64_t i_lbist_start_at = 0xF0,
    uint64_t i_lbist_start_stagger = 0,
    uint16_t i_lbist_sequence = 0x8000,
    uint16_t i_lbist_weight = 0x8000);
