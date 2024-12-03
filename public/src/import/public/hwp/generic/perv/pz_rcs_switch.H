/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_switch.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024,2025                        */
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
/// @file  pz_rcs_switch.H
/// @brief Switch oscillator RCS(Redundant clock switch) path
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include "pz_rcs_common.H"

/// @brief Switch oscillator RCS(Redundant clock switch) path
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*pz_rcs_switch_FP_t)(
    const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP >&);

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
//
extern "C" {

/// @brief Switch oscillator RCS(Redundant clock switch) path
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode pz_rcs_switch(
        const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP >& i_target);

} //extern "C"
