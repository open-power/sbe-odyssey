/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_common.H $   */
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
/// @file  pz_rcs_common.H
/// @brief Common functions for RCS(Redundant clock switch)
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Oliver Morlok (marquaro@in.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
using namespace fapi2;

/// @brief Switch RCS oscillators
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_sw_switch(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target);

/// @brief Validate RCS changes
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_verify_clean_state(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target,
                                  const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select);

/// @brief Clear RCS errors and check them
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_check_errors(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target,
                            fapi2::ATTR_CP_REFCLOCK_SELECT_Type i_refclock_select);

// TODO
ReturnCode rcs_lock_fplla( const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target);
// TODO
ReturnCode rcs_lock_fpllb( const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target);
