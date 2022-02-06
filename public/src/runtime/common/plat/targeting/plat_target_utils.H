/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_utils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2022                        */
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
/**
 * @file plat_target_util.H
 * @brief platform utility definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_TARGET_UTIL__
#define __FAPI2_PLAT_TARGET_UTIL__


//
// Platform Utility functions..
//
namespace fapi2
{
    ///
    /// @brief Function to print the G_targets vector
    ///
    void plat_PrintTargets();

    ///
    /// @brief Function to initialize the G_targets vector based on partial good
    ///        targets
    /// @return FAPI2_RC_SUCCESS if okay
    ///
    ReturnCode plat_TargetsInit();

    ///
    /// @brief Function to get the proc chip target initialize the G_targets vector
    /// @return proc target
    ///
    Target<TARGET_TYPE_PROC_CHIP> plat_getChipTarget();

}
#endif
