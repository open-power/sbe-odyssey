/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_startclocks.H $ */
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
/// @file  poz_chiplet_startclocks.H
///
/// @brief startclocks sequence for chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include "poz_perv_common_params.H"

typedef fapi2::ReturnCode (*poz_chiplet_startclocks_FP_t)(const fapi2::Target < fapi2::TARGET_TYPE_PERV |
        fapi2::TARGET_TYPE_MULTICAST > &, uint16_t);

/// @brief
/// -- switch ABIST&SYNC CLK muxes to functional state, Disable listen to sync
/// -- align chiplets, start chiplet clocks, Put PLATs into flush mode, Drop chiplet fence
///
/// @param[in]   i_target          Reference to TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST
/// @param[in]   i_clock_regions   select clock regions (default REGION_ALL_BUT_PLL)
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_chiplet_startclocks(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > &i_target,
        uint16_t i_clock_regions = cc::REGION_ALL_BUT_PLL);
}
