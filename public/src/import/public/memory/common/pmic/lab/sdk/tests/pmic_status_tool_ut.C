/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/tests/pmic_status_tool_ut.C $ */
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
/// @file pmic_status_tool_ut.C
/// @brief Unit tests for pmic status input processing API and helper functions
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <cstdarg>
#include <catch.hpp>
#include <pmic_status/pmic_status_tool_utils.H>
#include <pmic_status/pmic_status_utils.H>
#include <generic/memory/tests/target_fixture.H>
#include <generic/memory/lab/mss_lab_input_processing.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/mss_git_data.H>
#include <lib/utils/pmic_consts.H>
#include <generic/memory/lib/utils/find.H>
#include <lib/i2c/i2c_pmic.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>

namespace mss
{
namespace test
{
///
/// @brief Unit test cases for pmic_status_tool
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "pmic status", "[pmic_status_tool]")
{
    // Test input processing API
    // ID
    GIVEN("Test mss::lab::pmic::id in lab processing API")
    {
        std::vector<std::pair<std::string, mss::lab::pmic::id>> l_conversion =
        {
            {"PMIC0", mss::lab::pmic::id::PMIC0},
            {"PMIC1", mss::lab::pmic::id::PMIC1},
            {"ALL", mss::lab::pmic::id::ALL}
        };
        auto l_value = mss::lab::pmic::id::PMIC0;

        // tests non error path
        for(const auto& i : l_conversion)
        {
            // tests enum generation
            REQUIRE_FALSE(mss::lab::generate_enum(i.first, l_value));
            REQUIRE(l_value == i.second);

            // tests string generation
            REQUIRE(i.first == mss::lab::generate_str(i.second));
            REQUIRE(i.first == mss::lab::generate_str(l_value));
        }

        // tests error path
        REQUIRE(mss::lab::generate_enum("ZER", l_value));
    }

    // Test
    GIVEN("Checks input processing")
    {
        mss::lab::pmic::status::inputs l_inputs;
        mss::lab::pmic::status::inputs l_expected;

        // Checks help - we should exit with success, no changes to inputs
        {
            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"-h",
                };
                REQUIRE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }

            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"--help",
                };
                REQUIRE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }
        }

        // Checks no inputs - we should be OK because ID has a default value of ALL
        {
            int l_argc = 0;
            char* l_argv[] =
                {};
            REQUIRE_FALSE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Checks bad enum input
        {
            constexpr int NUM_ARGS = 1;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=err",
            };

            REQUIRE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Set new expected value
        l_expected.iv_id = mss::lab::pmic::id::PMIC1;
        l_expected.iv_clear = true;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 2;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC1",
                (char*)"-clear",
            };

            REQUIRE_FALSE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        l_expected.iv_single = true;
        l_expected.iv_telemetry = true;
        l_expected.iv_power = true;
        l_expected.iv_temperature = true;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 5;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC1",
                (char*)"-single",
                (char*)"-telemetry",
                (char*)"-power",
                (char*)"-temperature",
            };

            REQUIRE_FALSE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Set new expected value
        l_inputs.iv_single = false;
        l_inputs.iv_telemetry = false;
        l_inputs.iv_power = false;
        l_inputs.iv_temperature = false;
        l_inputs.iv_clear = false;
        l_expected.iv_id = mss::lab::pmic::id::PMIC0;
        l_expected.iv_clear = false;
        l_expected.iv_telemetry = false;
        l_expected.iv_power = false;
        l_expected.iv_temperature = false;
        l_expected.iv_single = false;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 1;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC0"
            };

            REQUIRE_FALSE(mss::lab::pmic::status::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }
    }

    GIVEN("Checks help screen")
    {
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
                "-h                      # Runs the help screen"
            };
            // Test help screen
            const auto l_lines = mss::lab::input::generate_help_screen_helper("pmic_status_tool",
                                 mss::lab::pmic::status::status_input_processing::FIELDS, l_examples);

            const std::vector<std::string> l_expected_help =
            {
                "wrapper: pmic_status_tool.exe",
                "Git commit ID: " + GIT_COMMIT_ID,
                "",
                "Input format:",
                "-parameter  description | input type | REQUIRED/Default: $value | optional value check",
                "",
                "Inputs:",
                "-d=                     PMIC Device/ID to Poll | enum | Default: ALL",
                "                        ALL",
                "                        PMIC0",
                "                        PMIC1",
                "-clear                  clear the status bits | flag | Default: false",
                "-telemetry              get/set telemetry data/settings (current, power, voltage, temperature) | flag | Default: false",
                "-power                  get PMIC output current and voltage setpoints | flag | Default: false",
                "-temperature            get PMIC temperatures | flag | Default: false",
                "-pmic_current_rail=     PMIC output to use for current limits | enum | Default: NONE",
                "                        SWA",
                "                        SWB",
                "                        SWC",
                "                        SWD",
                "-current_warning=       Value (mA) for the high current consumption warning threshold | decimal | Default:  | value < 7876",
                "                        For use with -pmic_current_rail= option",
                "-current_limiter=       Value (mA) for the current limiter warning threshold | decimal | Default:  | value < 5501",
                "                        For use with -pmic_current_rail= option",
                "-single                 Run on only the first valid OCMB target, then exit | flag | Default: false",
                "-h                      help screen flag | flag | Default: false",
                "--help                  help screen flag | flag | Default: false",
                "",
                "Examples:",
                "    pmic_status_tool.exe                         # No arguments: Display data for all PMICs",
                "    pmic_status_tool.exe -d=PMIC1 -p0            # Act only on OCMB p0 PMIC1",
                "    pmic_status_tool.exe -clear                  # Clear status bits on all PMICs",
                "    pmic_status_tool.exe -telemetry              # Get/Set PMIC telemetry data (current, power, voltage, temperature)",
                "    pmic_status_tool.exe -power                  # Get PMIC output current and voltage setpoints",
                "    pmic_status_tool.exe -temperature            # Get PMIC temperatures",
                "    pmic_status_tool.exe -pmic_current_rail=SWA  # Specify PMIC output SWA to set the current limit threshold",
                "    pmic_status_tool.exe -current_warning=3000   # Use a high current consumption warning threshold value of 3000 mA",
                "    pmic_status_tool.exe -current_limiter=4000   # Use a current limiter warning threshold value of 4000 mA",
                "    pmic_status_tool.exe -h                      # Runs the help screen"
            };

            REQUIRE(l_lines == l_expected_help);
        }
    }

    GIVEN("Checks calculation of current from sample bitmap")
    {
        fapi2::buffer<uint8_t> l_test_buffer;

        // A few examples
        l_test_buffer.setBit<7>(); // Last bit, so we should result in 0 + 125*(1) = 125
        REQUIRE(125 == mss::lab::pmic::status::calculate_current_from_bitmap(l_test_buffer));

        l_test_buffer.setBit<5>(); // 0 + 125*(5) = 621
        REQUIRE(625 == mss::lab::pmic::status::calculate_current_from_bitmap(l_test_buffer));

        l_test_buffer.setBit<3>(); // 0 + 125*(21) = 2625
        REQUIRE(2625 == mss::lab::pmic::status::calculate_current_from_bitmap(l_test_buffer));

    } // section

    GIVEN("Test get vendor string from vendor ID helper function")
    {
        auto l_result = mss::lab::pmic::status::get_vendor_from_reg_helper(mss::pmic::vendor::TI_SHORT);
        REQUIRE(l_result == "TI");

        l_result = mss::lab::pmic::status::get_vendor_from_reg_helper(mss::pmic::vendor::IDT_SHORT);
        REQUIRE(l_result == "IDT");

        l_result = mss::lab::pmic::status::get_vendor_from_reg_helper(0xFF); // Arbitrary invalid value
        REQUIRE(l_result == "Unknown");

    } // GIVEN

}// scenario
}// test
}// mss
