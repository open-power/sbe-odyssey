/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_setup_ref_clock.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
/// @file pz_setup_ref_clock.H
///
/// @brief shared code for setup_ref_clock procedure
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

typedef fapi2::ReturnCode (*pz_setup_ref_clock_FP_t)(const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP |
        fapi2::TARGET_TYPE_HUB_CHIP > &);

/// @brief shared code for setup_ref_clock procedure
/// -- Disable Write Protection for Root/Perv Control registers
/// -- Set RCS control signals to CFAM reset values
/// -- Setup refclock receiver termination & Unprotect inputs to RCS sense register
///
/// @param[in]   i_target              Reference to TARGET_TYPE_HUB_CHIP or TARGET_TYPE_PROC_CHIP
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode pz_setup_ref_clock(const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP |
                                         fapi2::TARGET_TYPE_HUB_CHIP > & i_target);
}
