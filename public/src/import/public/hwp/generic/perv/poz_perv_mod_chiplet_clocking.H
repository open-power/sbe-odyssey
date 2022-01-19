/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chiplet_clocking.H $ */
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
/// @file  poz_perv_mod_chiplet_clocking.H
///
/// @brief  prototype declarations for modules abist_start/poll/cleanup, scan0
///                                            start_stop_clocks, align_regions
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include "poz_perv_common_params.H"

using namespace cc;

/// @brief ABIST start module.
/// --Switch dual clocked arrays to ABIST clock domain
/// --Setup BIST regions, clocking.
/// --Configure idle count in OPCG_REG1
/// --Configure loop count in OPCG_REG0 and start OPCG.
///
/// @param[in]   i_target          Reference to TARGET_TYPE_PERV
/// @param[in]   i_clock_regions   select clock regions
/// @param[in]   i_runn_cycles     loop count value to set OPCG run-N mode
/// @param[in]   i_abist_start_at  idle count value to set in OPCG_REG1
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_abist_start(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_clock_regions,
    uint32_t i_runn_cycles = 0x42FFF,
    uint64_t i_abist_start_at = 0xF0);


/// @brief   Poll OPCG done bit to check for run-N completeness.
/// @param[in]   i_target          Reference to TARGET_TYPE_PERV
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_abist_poll(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > & i_target);

/// @brief  ABIST cleanup module.
/// --Clear OPCG_REG0, CLK_REGION, CPLT_CTRL0 & BIST registers
/// --Clear ABSTCLK_MUXSEL_DC
/// --Clear TC_SRAM_ABIST_MODE_DC
///
/// @param[in]   i_target                   Reference to TARGET_TYPE_PERV
/// @param[in]   i_clear_sram_abist_mode    clear SRAM ABIST mode
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_abist_cleanup(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    bool i_clear_sram_abist_mode = false);

/// @brief   SCAN0 module.
/// @param[in]   i_target          Reference to TARGET_TYPE_PERV
/// @param[in]   i_clock_regions   select clock regions
/// @param[in]   i_scan_types      select scan types
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_scan0(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_clock_regions,
    uint16_t i_scan_types = SCAN_TYPE_NOT_GTR);

/// @brief   clock start/stop module.
/// @param[in]   i_target          Reference to TARGET_TYPE_PERV
/// @param[in]   i_clock_regions   select clock regions
/// @param[in]   i_clock_types     select clock types SL/NSL/ARY/ALL
/// @param[in]   i_start_not_stop  clock start or stop value.
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_start_stop_clocks(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_OR > & i_target,
    uint16_t i_clock_regions,
    uint16_t i_clock_types = CLOCK_TYPE_ALL,
    bool i_start_not_stop = true);
