/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_common_stopclocks.H $ */
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
/// @file  poz_common_stopclocks.H
/// @brief Common module for stopclocks
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:HB
//------------------------------------------------------------------------------


#ifndef _POZ_COMMON_STOPCLOCKS_H_
#define _POZ_COMMON_STOPCLOCKS_H_
#define btos(x) ((x)?"TRUE":"FALSE")

#include <fapi2.H>

/// @brief Check whether stopping chiplet clocks possible
/// @param[in]     i_target                       Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]     stop_chiplet_clocks_possible   True if stop chiplet clocks is possible
/// @param[in]     stop_tp_clocks_possible        True if TP chiplet is clock stoppable
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_stopclocks_pre_check(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target,
    bool& stop_chiplet_clocks_possible,
    bool& stop_tp_clocks_possible);

/// @brief Stopping SBE region clocks of TP chiplet using CBS
///
/// @param[in]     i_target            Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_common_sbe_stopclocks(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target);

/// @brief Checking for CBS request complete
///
/// @param[in]     i_target            Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_common_poll_cbs_cmd_complete(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target);

/// @brief Check if the chiplet is accessible for vital access
/// can be used for all chiplets except TP
///
/// @param[in]     i_target    Reference to TARGET_TYPE_PERV
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_common_stopclocks_chiplet_accessible(
    const fapi2::Target<fapi2::TARGET_TYPE_PERV>& i_target_chiplet);

/// @brief Stop vital clocks
/// @param[in]     i_target    Reference to TARGET_TYPE_ANY_POZ_CHIP
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_vital_stopclocks(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target);

/// @brief Stopclocks for chiplets
/// -- Check if chiplet is accessible, raise chiplet fence, call mod_start_stop_clocks,
/// -- Set vital fence and flushmode
///
/// @param[in]     i_target_chiplet   Reference to TARGET_TYPE_PERV target
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_chiplet_stopclocks(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target_chiplet);

/// @brief Setup a multicast group from accessible chiplets and stop chiplet clocks with multicast target
///
/// @param[in]     i_target          Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]     i_target_filter   Target filter containing chiplets to stop clocks
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_stop_chiplet_clocks_with_multiacst(
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target, fapi2::TargetFilter i_target_filter);
#endif
