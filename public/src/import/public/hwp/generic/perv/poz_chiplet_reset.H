/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_reset.H $ */
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
/// @file  poz_chiplet_reset.H
/// @brief Enable and reset chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

enum poz_chiplet_reset_phases : uint8_t
{
    PRE_SCAN0 = 0x80,
    SCAN0_AND_UP = 0x40,
    ALL = 0xFF,
};

typedef fapi2::ReturnCode (*poz_chiplet_reset_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
        const uint8_t i_chiplet_delays[64],
        const poz_chiplet_reset_phases i_phases);

/// @brief Enable and reset chiplets
/// -- Enable and reset chiplets, setup clock controllers & OPCG delays
/// -- do SCAN0 on all regions, transfer PGOOD attr into CPLT_CTRL[2,3]
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]   i_chiplet_delays[64]      Array of per-chiplet delays to be programmed in OPCG_WAIT_CYCLES
/// @param[in]   i_sync_pulse_delay        Delay incoming sync pulse by the specified number of cycles (2 <= i_sync_pulse_delay < 16) with the default being 8
/// @param[in]   i_phases                  Option to split chiplet reset in 2 halves (PRE_SCAN0 / SCAN0_AND_UP) or do the combination (ALL)
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_chiplet_reset(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                                        const uint8_t i_chiplet_delays[64],
                                        const uint8_t i_sync_pulse_delay = 8,
                                        const poz_chiplet_reset_phases i_phases = ALL);
}
