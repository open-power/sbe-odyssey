/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_status/pmic_status_tool_utils.H $ */
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

#ifndef __PMIC_STATUS_TOOL_UTILS_H__
#define __PMIC_STATUS_TOOL_UTILS_H__

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_status/pmic_status_utils.H>
#include <generic/memory/lab/mss_lab_input_processing.H>
#include <pmic_common_lab.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace status
{

///
/// @brief Struct for wrapper inputs
///
struct inputs
{
    mss::lab::pmic::id iv_id = mss::lab::pmic::id::ALL;
    bool iv_clear = false;
    bool iv_telemetry = false;
    bool iv_power = false;
    bool iv_temperature = false;
    bool iv_single = false;
    mss::lab::pmic::status::rail_current iv_pmic_current_rail = mss::lab::pmic::status::rail_current::NONE;
    uint16_t iv_current_warning = 0;
    uint16_t iv_current_limiter = 0;

    ///
    /// @brief Equals comparison operator
    /// @param[in] i_rhs right hand side to compare against
    /// @return true iff everything is equal
    ///
    bool operator==(const inputs& i_rhs) const
    {
        return (iv_id == i_rhs.iv_id) &&
               (iv_telemetry == i_rhs.iv_telemetry) &&
               (iv_power == i_rhs.iv_power) &&
               (iv_temperature == i_rhs.iv_temperature) &&
               (iv_pmic_current_rail == i_rhs.iv_pmic_current_rail) &&
               (iv_current_warning == i_rhs.iv_current_warning) &&
               (iv_current_limiter == i_rhs.iv_current_limiter) &&
               (iv_clear == i_rhs.iv_clear) &&
               (iv_single == i_rhs.iv_single);
    }
};

///
/// @brief Forward declaration of mappings from text to mss::lab::pmic::id enum
///
template <>
const std::map<std::string, mss::lab::pmic::id>
mss::lab::enum_traits<mss::lab::pmic::id>::iv_string_to_enum_map;

template <>
const char* mss::lab::enum_traits<mss::lab::pmic::id>::iv_type;

///
/// @brief mappings from text to mss::lab::pmic::status::rail_current enum
///
template<>
const std::map<std::string, mss::lab::pmic::status::rail_current>
mss::lab::enum_traits<mss::lab::pmic::status::rail_current>::iv_string_to_enum_map;

template <>
const char* mss::lab::enum_traits<mss::lab::pmic::status::rail_current>::iv_type;


///
/// @brief class for pmic_status wrapper input processing
///
class status_input_processing
{
    public:
        static constexpr mss::lab::input::traits ID
        {
            mss::lab::input::process_type::ENUM,
            mss::lab::input::required::OPTIONAL,
            "-d=",
            "PMIC Device/ID to Poll", "ALL"};

        static constexpr mss::lab::input::traits CLEAR
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-clear",
            "clear the status bits",
            "false"};

        static constexpr mss::lab::input::traits TELEMETRY
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-telemetry",
            "get/set telemetry data/settings (current, power, voltage, temperature)",
            "false"};

        static constexpr mss::lab::input::traits POWER
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-power",
            "get PMIC output current and voltage setpoints",
            "false"};

        static constexpr mss::lab::input::traits TEMPERATURE
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-temperature",
            "get PMIC temperatures",
            "false"};

        static constexpr mss::lab::input::traits PMIC_CURRENT_RAIL
        {
            mss::lab::input::process_type::ENUM,
            mss::lab::input::required::OPTIONAL,
            "-pmic_current_rail=",
            "PMIC output to use for current limits",
            "NONE"};

        static constexpr mss::lab::input::traits CURRENT_WARNING =
            mss::lab::input::create_traits<mss::lab::input::process_type::DEC,
            mss::lab::input::required::OPTIONAL,
            mss::lab::input::compare::LESS>(
                "-current_warning=",
                "Value (mA) for the high current consumption warning threshold",
                "",
                (mss::lab::pmic::status::CURRENT_WARNING_MAX_VALUE + 1));

        static constexpr mss::lab::input::traits CURRENT_LIMITER =
            mss::lab::input::create_traits<mss::lab::input::process_type::DEC,
            mss::lab::input::required::OPTIONAL,
            mss::lab::input::compare::LESS>(
                "-current_limiter=",
                "Value (mA) for the current limiter warning threshold",
                "",
                (mss::lab::pmic::status::CURRENT_LIMITER_MAX_VALUE + 1));

        static constexpr mss::lab::input::traits SINGLE
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-single",
            "Run on only the first valid OCMB target, then exit",
            "false"};

        static constexpr mss::lab::input::traits HELP1
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-h",
            "help screen flag",
            "false"};
        static constexpr mss::lab::input::traits HELP2
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "--help",
            "help screen flag",
            "false"};

        static const std::vector<std::pair<mss::lab::input::traits, std::vector<std::string>>> FIELDS;

}; // class

///
/// @brief Prints help information
///
inline static void help()
{
    const std::vector<std::string> l_examples =
    {
        "                        # No arguments: Display data for all PMICs",
        "-d=PMIC1 -p0            # Act only on OCMB p0 PMIC1",
        "-clear                  # Clear status bits on all PMICs",
        "-telemetry              # Get/Set PMIC telemetry data (current, power, voltage, temperature)",
        "-power                  # Get PMIC output current and voltage setpoints",
        "-temperature            # Get PMIC temperatures",
        "-pmic_current_rail=SWA  # Specify PMIC output SWA to set the current limit threshold",
        "-current_warning=3000   # Use a high current consumption warning threshold value of 3000 mA",
        "-current_limiter=4000   # Use a current limiter warning threshold value of 4000 mA",
        "-h                      # Runs the help screen",
    };

    mss::lab::input::print_help_screen("pmic_status_tool", status_input_processing::FIELDS, l_examples);
    return;
}

///
/// @brief process input arguments
///
/// @param[in,out] io_argc argc
/// @param[in,out] io_argv argv
/// @param[in,out] io_inputs struct of input data
/// @return fapi2::ReturnCode
///
inline static fapi2::ReturnCode process_inputs(int& io_argc, char* io_argv[], inputs& io_inputs)
{
    bool l_help = false;
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // Checks for the help screen first
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::HELP1>(io_argc, io_argv, l_help));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::HELP2>(io_argc, io_argv, l_help));

    // If we get here, run the help screen
    FAPI_ASSERT(!l_help,
                fapi2::MSS_LAB_HELP_SCREEN_CALLED(),
                "Help screen requested!");

    // Process the rest of the inputs
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::ID>(io_argc, io_argv, io_inputs.iv_id));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::CLEAR>(io_argc, io_argv, io_inputs.iv_clear));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::TELEMETRY>(io_argc, io_argv, io_inputs.iv_telemetry));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::POWER>(io_argc, io_argv, io_inputs.iv_power));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::TEMPERATURE>(io_argc, io_argv,
             io_inputs.iv_temperature));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::PMIC_CURRENT_RAIL>(io_argc, io_argv,
             io_inputs.iv_pmic_current_rail));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::CURRENT_WARNING>(io_argc, io_argv,
             io_inputs.iv_current_warning));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::CURRENT_LIMITER>(io_argc, io_argv,
             io_inputs.iv_current_limiter));
    FAPI_TRY(mss::lab::input::process_input<status_input_processing::SINGLE>(io_argc, io_argv, io_inputs.iv_single));


    // check for any invalid input combinations
    FAPI_TRY(((io_inputs.iv_pmic_current_rail != mss::lab::pmic::status::rail_current::NONE) &&
              ((io_inputs.iv_current_warning == 0) && (io_inputs.iv_current_limiter == 0)) ?
              fapi2::FAPI2_RC_INVALID_PARAMETER : fapi2::FAPI2_RC_SUCCESS),
             "Must use non-zero value for -current_warning= or -current_warning= when using -pmic_current_rail=");
    FAPI_TRY(((io_inputs.iv_pmic_current_rail == mss::lab::pmic::status::rail_current::NONE) &&
              ((io_inputs.iv_current_warning > 0) || (io_inputs.iv_current_limiter > 0)) ?
              fapi2::FAPI2_RC_INVALID_PARAMETER : fapi2::FAPI2_RC_SUCCESS),
             "Must specify -pmic_current_rail= when using -current_warning= or -current_warning=");

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    printf("Printing help and exiting!\n");
    mss::lab::pmic::status::help();
    return fapi2::current_err;
}

} // namespace status
} // namespace pmic
} // namespace lab
} // namespace mss
#endif
