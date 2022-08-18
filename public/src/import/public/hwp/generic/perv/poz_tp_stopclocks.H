/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_tp_stopclocks.H $ */
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
/// @file  poz_tp_stopclocks.H
/// @brief Stop clocks for TP chiplet
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <poz_common_stopclocks.H>

/// @brief Stop clocks for TP chiplet
///
/// @param[in]     i_target            Reference to TARGET_TYPE_ANY_POZ_CHIP target
/// @param[in]     i_stopclocks_flags  Stop clocks flags
/// @param[in]     i_pcb_accessible    True if PCB can be accessed
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_tp_stopclocks(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const StopClocksFlags i_stopclocks_flags,
    bool i_pcb_accessible);

/// @brief Stopclocks for all regions of TP chiplet except sbe
///
/// @param[in]     i_target       Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_tp_stopclocks_all_regions_except_sbe(
    const  fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target);

/// @brief Stopclocks for sbe region
///
/// @param[in]     i_target       Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_sbe_region_stopclocks(
    const  fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target);
