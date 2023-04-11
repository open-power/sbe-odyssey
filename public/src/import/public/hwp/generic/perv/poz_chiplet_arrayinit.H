/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_arrayinit.H $ */
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
/// @file  poz_chiplet_arrayinit.H
/// @brief Arrayinit all chiplets except TP
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <poz_perv_common_params.H>

/// @brief Arrayinit all chiplets except TP
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]   i_clock_regions           Select clock regions
/// @param[in]   i_runn_cycles             Loop count value to set OPCG run-N mode (>0x7FFFFFFFFFF for infinite)
/// @param[in]   i_do_scan0                Perform scan0 after arrayinit
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_chiplet_arrayinit(
        const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
        const cc::clock_region i_clock_regions = cc::REGION_ALL_BUT_PLL,
        const uint64_t i_runn_cycles = 0x42FFF,
        const bool i_do_scan0 = true);
}
