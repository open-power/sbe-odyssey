/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_bias/pmic_bias_tool_utils.C $ */
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
/// @file pmic_bias_tool_utils.C
/// @brief Input processing for pmic_bias lab tool
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <pmic_bias/pmic_bias_tool_utils.H>
#include <generic/memory/lab/mss_lab_tools.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace bias
{

// Declarations/Definitions to please the compiler
constexpr mss::lab::input::traits bias_input_processing::ID;
constexpr mss::lab::input::traits bias_input_processing::SETTING;
constexpr mss::lab::input::traits bias_input_processing::AMOUNT;
constexpr mss::lab::input::traits bias_input_processing::UNIT;
constexpr mss::lab::input::traits bias_input_processing::FORCE;

///
/// @brief Fields definitions for bias_input_processing class
///
const std::vector<std::pair<mss::lab::input::traits, std::vector<std::string>>> bias_input_processing::FIELDS =
{
    // ID, SETTING, UNIT are enums and therefore have options to show. The rest are not enums
    {bias_input_processing::ID, {"PMIC0", "PMIC1", "ALL"}},
    {bias_input_processing::SETTING, {"SWA_VOLT", "SWB_VOLT", "SWC_VOLT", "SWD_VOLT"}},
    {bias_input_processing::AMOUNT, {}},
    {bias_input_processing::UNIT, {"VALUE", "PERCENT"}},
    {bias_input_processing::FORCE, {}},
};

///
/// @brief mappings from text to mss::lab::pmic::bias::setting enum
///
template<> const std::map<std::string, mss::lab::pmic::bias::setting>
mss::lab::enum_traits<mss::lab::pmic::bias::setting>::iv_string_to_enum_map
{
    {"SWA_VOLT", mss::lab::pmic::bias::setting::SWA_VOLT},
    {"SWB_VOLT", mss::lab::pmic::bias::setting::SWB_VOLT},
    {"SWC_VOLT", mss::lab::pmic::bias::setting::SWC_VOLT},
    {"SWD_VOLT", mss::lab::pmic::bias::setting::SWD_VOLT},
};

template<> const char* mss::lab::enum_traits<mss::lab::pmic::bias::setting>::iv_type = "mss::lab::pmic::bias::setting";

///
/// @brief mappings from text to mss::lab::pmic::bias::unit enum
///
template<> const std::map<std::string, mss::lab::pmic::bias::unit>
mss::lab::enum_traits<mss::lab::pmic::bias::unit>::iv_string_to_enum_map
{
    {"VALUE", mss::lab::pmic::bias::unit::VALUE},
    {"PERCENT", mss::lab::pmic::bias::unit::PERCENT},
};

template<> const char* mss::lab::enum_traits<mss::lab::pmic::bias::unit>::iv_type = "mss::lab::pmic::bias::unit";

} // bias
} // pmic
} // lab
} // mss
