/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_perv_mod_chip_clocking.H $ */
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
/// @file  pz_perv_mod_chip_clocking.H
///
/// @brief clock test module for PZ chips to check whether the ref clock is valid.
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include "poz_perv_common_params.H"

/// @brief       clock test module for PZ chips to check whether the ref clock is valid.
/// @param[in]   i_target          Reference to TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP
/// @param[in]   i_use_cfam_path   option to select between CFAM or SCOM access
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode mod_clock_test(
    const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP | fapi2::TARGET_TYPE_HUB_CHIP > & i_target,
    bool i_use_cfam_path);
