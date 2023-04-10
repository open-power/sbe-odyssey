/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_status/pmic_status_tool_utils.C $ */
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
/// @file pmic_status_tool_utils.H
/// @brief Input processing for pmic_status lab tool
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_status/pmic_status_tool_utils.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <pmic_common_lab.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace status
{

// Declarations/Definitions to please the compiler
constexpr mss::lab::input::traits status_input_processing::ID;
constexpr mss::lab::input::traits status_input_processing::CLEAR;
constexpr mss::lab::input::traits status_input_processing::TELEMETRY;
constexpr mss::lab::input::traits status_input_processing::POWER;
constexpr mss::lab::input::traits status_input_processing::TEMPERATURE;
constexpr mss::lab::input::traits status_input_processing::PMIC_CURRENT_RAIL;
constexpr mss::lab::input::traits status_input_processing::CURRENT_WARNING;
constexpr mss::lab::input::traits status_input_processing::CURRENT_LIMITER;
constexpr mss::lab::input::traits status_input_processing::HELP1;
constexpr mss::lab::input::traits status_input_processing::HELP2;
constexpr mss::lab::input::traits status_input_processing::SINGLE;

///
/// @brief Fields definitions for status_input_processing class
///
const std::vector<std::pair<mss::lab::input::traits, std::vector<std::string>>> status_input_processing::FIELDS =
{
    // ID, is an enum and therefore has options to show. The rest are not enums and their descriptions
    // are defined in pmic_status_tool_utils.H
    {status_input_processing::ID, {"ALL", "PMIC0", "PMIC1"}},
    {status_input_processing::CLEAR, {}},
    {status_input_processing::TELEMETRY, {}},
    {status_input_processing::POWER, {}},
    {status_input_processing::TEMPERATURE, {}},
    {status_input_processing::PMIC_CURRENT_RAIL, {"SWA", "SWB", "SWC", "SWD"}},
    {status_input_processing::CURRENT_WARNING, {"For use with -pmic_current_rail= option"}},
    {status_input_processing::CURRENT_LIMITER, {"For use with -pmic_current_rail= option"}},
    {status_input_processing::SINGLE, {}},
    {status_input_processing::HELP1, {}},
    {status_input_processing::HELP2, {}}
};

///
/// @brief mappings from text to mss::lab::pmic::status::id enum
///
template <>
const std::map<std::string, mss::lab::pmic::id>
mss::lab::enum_traits<mss::lab::pmic::id>::iv_string_to_enum_map
{
    {"PMIC0", mss::lab::pmic::id::PMIC0},
    {"PMIC1", mss::lab::pmic::id::PMIC1},
    {"ALL", mss::lab::pmic::id::ALL}};

template <>
const char* mss::lab::enum_traits<mss::lab::pmic::id>::iv_type = "mss::lab::pmic::status::id";

///
/// @brief mappings from text to mss::lab::pmic::status::rail_current enum
///
template<>
const std::map<std::string, mss::lab::pmic::status::rail_current>
mss::lab::enum_traits<mss::lab::pmic::status::rail_current>::iv_string_to_enum_map
{
    {"SWA", mss::lab::pmic::status::rail_current::SWA},
    {"SWB", mss::lab::pmic::status::rail_current::SWB},
    {"SWC", mss::lab::pmic::status::rail_current::SWC},
    {"SWD", mss::lab::pmic::status::rail_current::SWD}};

template <>
const char* mss::lab::enum_traits<mss::lab::pmic::status::rail_current>::iv_type =
    "mss::lab::pmic::status::rail_current";


} // namespace status
} // namespace pmic
} // namespace lab
} // namespace mss
