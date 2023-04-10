/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_bias/pmic_bias_tool_utils.H $ */
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
/// @file pmic_bias_tool_utils.H
/// @brief Input processing for pmic_bias lab tool
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#ifndef __PMIC_BIAS_TOOL_UTILS_H__
#define __PMIC_BIAS_TOOL_UTILS_H__

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <generic/memory/lab/mss_lab_input_processing.H>
#include <pmic_common_lab.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace bias
{

//-------- Wrapper fields/traits ---------//

///
/// @brief Struct for wrapper inputs
///
struct inputs
{
    mss::lab::pmic::id iv_id                    = mss::lab::pmic::id::PMIC0;
    mss::lab::pmic::bias::setting iv_setting    = mss::lab::pmic::bias::setting::SWA_VOLT;
    float iv_amount                             = 0;
    mss::lab::pmic::bias::unit iv_unit          = mss::lab::pmic::bias::unit::PERCENT;
    bool iv_force                               = false;

    ///
    /// @brief Equals comparison operator
    /// @param[in] i_rhs right hand side to compare against
    /// @return true iff everything is equal
    ///
    bool operator==(const inputs& i_rhs) const
    {
        return iv_id            == i_rhs.iv_id &&
               iv_setting       == i_rhs.iv_setting &&
               iv_amount        == i_rhs.iv_amount &&
               iv_unit          == i_rhs.iv_unit &&
               iv_force         == i_rhs.iv_force;
    }
};

///
/// @brief class for pmic_bias wrapper input processing
///
class bias_input_processing
{
    public:
        static constexpr mss::lab::input::traits ID
        {
            mss::lab::input::process_type::ENUM,
            mss::lab::input::required::REQUIRED,
            "-d=",
            "PMIC Device/ID to Bias", ""}; // empty default required

        static constexpr mss::lab::input::traits SETTING
        {
            mss::lab::input::process_type::ENUM,
            mss::lab::input::required::REQUIRED,
            "-s=",
            "Setting to bias", ""}; // empty default required

        static constexpr mss::lab::input::traits AMOUNT
        {
            mss::lab::input::process_type::FLOAT,
            mss::lab::input::required::REQUIRED,
            "-v=",
            "Value/Percent to set", ""}; // empty default required

        static constexpr mss::lab::input::traits UNIT
        {
            mss::lab::input::process_type::ENUM,
            mss::lab::input::required::REQUIRED,
            "-u=",
            "Unit to use for bias (VALUE in Volts)", ""};

        static constexpr mss::lab::input::traits FORCE
        {
            mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL,
            "-f",
            "Force, ignore 10% change limit",
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

};// class

///
/// @brief Prints help information
///
inline static void help()
{
    const std::vector<std::string> l_examples =
    {
        "-d=PMIC0 -s=SWA_VOLT -v=-7.5 -u=PERCENT # Bias SWA_VOLT of PMIC0 -7.5%",
        "-d=PMIC1 -s=SWB_VOLT -v=0.9 -u=VALUE    # Bias SWB_VOLT of PMIC1 to 0.9V",
        "-h # Runs the help screen",
    };

    mss::lab::input::print_help_screen("pmic_bias_tool", bias_input_processing::FIELDS, l_examples);
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
inline static fapi2::ReturnCode process_inputs( int& io_argc, char* io_argv[], inputs& io_inputs)
{
    bool l_help = false;
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // Checks for the help screen first
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::HELP1>(io_argc, io_argv, l_help));
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::HELP2>(io_argc, io_argv, l_help));

    // If we get here, run the help screen
    FAPI_ASSERT(!l_help,
                fapi2::MSS_LAB_HELP_SCREEN_CALLED(),
                "Help screen requested!");

    // Process the rest of the inputs
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::ID>(io_argc, io_argv, io_inputs.iv_id));
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::SETTING>(io_argc, io_argv, io_inputs.iv_setting));
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::AMOUNT>(io_argc, io_argv, io_inputs.iv_amount));
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::UNIT>(io_argc, io_argv, io_inputs.iv_unit));
    FAPI_TRY(mss::lab::input::process_input<bias_input_processing::FORCE>(io_argc, io_argv, io_inputs.iv_force));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    printf("Printing help and exiting!\n");
    mss::lab::pmic::bias::help();
    return fapi2::current_err;
}

} // bias
} // pmic
} // lab
} // mss

#endif
