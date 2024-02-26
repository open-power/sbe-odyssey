/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_add.H $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
/// @file  pz_rcs_add.H
/// @brief Add oscillator to RCS(Redundant clock switch) path
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Kevin Duffy (kjduffy@us.ibm.com)
// *HWP FW Maintainer   : Oliver Morlok (marquaro@in.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

/// @brief Add oscillator to RCS(Redundant clock switch) path
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
//DECLARE_HWP(pz_rcs_add,
//            const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP |
//            fapi2::TARGET_TYPE_HUB_CHIP > &i_target);
//

// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*pz_rcs_add_FP_t)(
    const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP | fapi2::TARGET_TYPE_HUB_CHIP > &);

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
//
extern "C" {

/// @brief Add oscillator to RCS(Redundant clock switch) path
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode pz_rcs_add(
        const fapi2::Target < fapi2::TARGET_TYPE_PROC_CHIP | fapi2::TARGET_TYPE_HUB_CHIP > & i_target);

} //extern "C"
