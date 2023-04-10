/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_update/pmic_update_utils.H $ */
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
///
/// @file pmic_update_utils.H
/// @brief Update util function and helper functions
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#ifndef __PMIC_UPDATE_UTILS_H__
#define __PMIC_UPDATE_UTILS_H__

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_common_lab.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace update
{

///
/// @brief Vendor region settings to modify
///
enum setting
{
    SWA_VOLT = 0,
    SWB_VOLT = 1,
    SWC_VOLT = 2,
    SWD_VOLT = 3,
    SWA_RANGE = 4, // IDT Only
    SWB_RANGE = 5, // IDT Only
    SWC_RANGE = 6, // IDT Only
    SWD_RANGE = 7,
};

///
/// @brief Poll for completion of vendor region burn
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS when burn successful, or FAPI2_RC_FALSE if unsuccessful
///
fapi2::ReturnCode poll_for_burn_completion(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Checks that the range modification of SWA,SWB,SWC is not ocurring on an invalid pmic
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_ti boolean true if TI pmic
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS unless invalid TI range change, in which case FAPI2_RC_FALSE
///
fapi2::ReturnCode check_ti_pmic(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target, bool& o_ti);

///
/// @brief Set the provided voltage on the current range
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (in V)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_voltage_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                     const mss::pmic::rail i_rail,
                                     const float i_value);

///
/// @brief Calls helper function to set voltage on the desired rail, and then poll for completion
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (in V)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_voltage(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                              const mss::pmic::rail i_rail,
                              const float i_value);

///
/// @brief Set the provided range on the provided rail in the PMIC vendor region
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (0 or 1. Will be truncated to integer)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_range_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                   const mss::pmic::rail i_rail,
                                   const float i_value);

///
/// @brief Calls helper function to set desired range, and then poll for completion
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (0 or 1. Will be truncated to integer)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_range(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                            const mss::pmic::rail i_rail,
                            const float i_value);

} // namespace update

// ----------------------------------
//          Update Functions
// ----------------------------------

///
/// @brief Helper function to update PMIC
///
/// @param[in] i_pmic_target PMIC target to perform update on
/// @param[in] i_setting SETTING to modify
/// @param[in] i_value value to update with
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode update_helper(
    const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::lab::pmic::update::setting i_setting,
    const float i_value);

///
/// @brief PMIC update function to modify vendor region
///
/// @param[in] i_ocmb_target OCMB target with PMIC children to update
/// @param[in] i_id ID of pmic (PMIC0, PMIC1)
/// @param[in] i_setting SETTING to modify
/// @param[in] i_value value to update with
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode update_chip(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const mss::lab::pmic::id i_id,
    const mss::lab::pmic::update::setting i_setting,
    const float i_value);

} // pmic
} // lab
} // mss
#endif
