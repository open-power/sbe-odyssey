/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_common_lab.H $ */
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
/// @file pmic_common_lab.H
/// @brief Common declarations for PMIC lab input processing
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#ifndef __PMIC_COMMON_LAB_H__
#define __PMIC_COMMON_LAB_H__

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lab/mss_lab_input_processing.H>

namespace mss
{
namespace lab
{
namespace pmic
{

///
/// @brief IDs of PMICS to poll/bias
///
enum id
{
    PMIC0 = 0,
    PMIC1 = 1,
    ALL = 2,
};

///
/// @brief Forward declaration of mappings from text to mss::pmic::id enum
///
template<> const std::map<std::string, mss::lab::pmic::id>
mss::lab::enum_traits<mss::lab::pmic::id>::iv_string_to_enum_map;

template<> const char* mss::lab::enum_traits<mss::lab::pmic::id>::iv_type;

///
/// @brief Get the current rail voltage of a JEDEC PMIC
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to read from
/// @param[out] o_current_rail_voltage voltage calculated for rail
/// @param[out] o_range_selection range selection of that voltage
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error
/// @note not templated as the arguments may differ for other chips (if we ever use others)
///
fapi2::ReturnCode get_current_rail_voltage(const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::pmic::rail i_rail,
        uint32_t& o_current_rail_voltage,
        uint8_t& o_range_selection);

} // lab
} // pmic
} // mss

#endif
