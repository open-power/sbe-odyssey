/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/tests/pmic_update_tool_ut.C $ */
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
/// @file pmic_update_tool_ut.C
/// @brief Unit tests for pmic update input processing API and helper functions
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <cstdarg>
#include <catch.hpp>
#include <pmic_update/pmic_update_tool_utils.H>
#include <pmic_update/pmic_update_utils.H>
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
/// @brief Unit test cases for pmic_update_tool
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "pmic update", "[pmic_update_tool]")
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

    // SETTING
    GIVEN("Test mss::lab::pmic::update::setting in lab processing API")
    {
        std::vector<std::pair<std::string, mss::lab::pmic::update::setting>> l_conversion =
        {
            {"SWA_VOLT", mss::lab::pmic::update::setting::SWA_VOLT},
            {"SWB_VOLT", mss::lab::pmic::update::setting::SWB_VOLT},
            {"SWC_VOLT", mss::lab::pmic::update::setting::SWC_VOLT},
            {"SWD_VOLT", mss::lab::pmic::update::setting::SWD_VOLT},
            {"SWA_RANGE", mss::lab::pmic::update::setting::SWA_RANGE},
            {"SWB_RANGE", mss::lab::pmic::update::setting::SWB_RANGE},
            {"SWC_RANGE", mss::lab::pmic::update::setting::SWC_RANGE},
            {"SWD_RANGE", mss::lab::pmic::update::setting::SWD_RANGE}
        };
        auto l_value = mss::lab::pmic::update::setting::SWA_VOLT;

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
        mss::lab::pmic::update::inputs l_inputs;
        mss::lab::pmic::update::inputs l_expected;

        // Checks help - we should exit with an error, no changes to inputs
        {
            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"-h",
                };
                REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }

            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"--help",
                };
                REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }
        }

        // Checks no inputs - we should fail because PMIC ID, setting and value are required
        {
            int l_argc = 0;
            char* l_argv[] =
                {};
            REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
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

            REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Checks bad enum input
        {
            constexpr int NUM_ARGS = 1;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-s=err",

            };

            REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Checks bad ranged input
        {
            constexpr int NUM_ARGS = 1;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-v=err",

            };

            REQUIRE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Set new expected value
        l_expected.iv_id = mss::lab::pmic::id::PMIC1;
        l_expected.iv_setting = mss::lab::pmic::update::setting::SWB_VOLT;
        l_expected.iv_value = 5.0;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 3;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC1",
                (char*)"-s=SWB_VOLT",
                (char*)"-v=5",
            };

            REQUIRE_FALSE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Set new expected value
        l_expected.iv_id = mss::lab::pmic::id::PMIC0;
        l_expected.iv_setting = mss::lab::pmic::update::setting::SWC_RANGE;
        l_expected.iv_value = 1;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 3;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC0",
                (char*)"-s=SWC_RANGE",
                (char*)"-v=1",
            };

            REQUIRE_FALSE(mss::lab::pmic::update::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }
    }

    GIVEN("Checks help screen")
    {
        {
            const std::vector<std::string> l_examples =
            {
                "-d=PMIC0 -s=SWA_VOLT -v=1.1 # Update SWA_VOLT of PMIC0 to 1.1V on current range",
                "-d=PMIC1 -s=SWB_RANGE -v=1  # Update SWB_RANGE of PMIC1 to range 1",
                "-h # Runs the help screen"
            };
            // Test help screen
            const auto l_lines = mss::lab::input::generate_help_screen_helper("pmic_update_tool",
                                 mss::lab::pmic::update::update_input_processing::FIELDS, l_examples);

            const std::vector<std::string> l_expected_help =
            {
                "wrapper: pmic_update_tool.exe",
                "Git commit ID: " + GIT_COMMIT_ID,
                "",
                "Input format:",
                "-parameter  description | input type | REQUIRED/Default: $value | optional value check",
                "",
                "Inputs:",
                "-d=     PMIC Device/ID to Update | enum | REQUIRED",
                "        PMIC0",
                "        PMIC1",
                "        ALL",
                "-s=     Setting to change | enum | REQUIRED",
                "        SWA_VOLT, SWB_VOLT, SWC_VOLT, SWD_VOLT",
                "        SWA_RANGE, SWB_RANGE, SWC_RANGE, SWD_RANGE",
                "-v=     Value/Percent to set | float | REQUIRED",
                "",
                "Examples:",
                "    pmic_update_tool.exe -d=PMIC0 -s=SWA_VOLT -v=1.1 # Update SWA_VOLT of PMIC0 to 1.1V on current range",
                "    pmic_update_tool.exe -d=PMIC1 -s=SWB_RANGE -v=1  # Update SWB_RANGE of PMIC1 to range 1",
                "    pmic_update_tool.exe -h # Runs the help screen"
            };

            REQUIRE(l_lines == l_expected_help);
        }
    }

#ifdef SUET_WORKING_NOW
    GIVEN("Checks JEDEC-compliant helper functions")
    {
        using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
        using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            // for each pmic
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                FAPI_INF("Test set_voltage() and set_range() functions")
                {
                    // Test bitmaps, a test range, and the corresponding float that we will feed the functions
                    static constexpr uint8_t TARGET_VOLTAGE_BITMAP_A = 0b00111110;
                    static constexpr uint8_t TARGET_VOLTAGE_BITMAP_C = 0b00011110;
                    static constexpr uint8_t TARGET_VOLTAGE_BITMAP_D = 0b01101010;

                    // These are floats as this is what the input processor uses in order to take in
                    // both voltages and ranges in the same argument
                    static constexpr float TARGET_RANGE_A = CONSTS::RANGE_0;
                    static constexpr float TARGET_RANGE_C = CONSTS::RANGE_0;
                    static constexpr float TARGET_RANGE_D = CONSTS::RANGE_1;

                    static constexpr float MATCHING_VOLTAGE_A = 0.955;
                    static constexpr float MATCHING_VOLTAGE_C = 0.875;
                    static constexpr float MATCHING_VOLTAGE_D = 2.465;

                    static constexpr float RANGE_OUT_OF_RANGE = 2;
                    static constexpr float VOLTAGE_OUT_OF_RANGE = 2.900;

                    fapi2::buffer<uint8_t> l_reg_contents;

// Range register is broken in SIMICS for SWA,SWB,SWC (simics was modeled after TI)
// We can't test range 1 on those yet (should be able to soon)
#ifdef SUET_PMIC_FIXED
                    static constexpr uint8_t TARGET_VOLTAGE_BITMAP_B = 0b00110100;
                    static constexpr float TARGET_RANGE_B = CONSTS::RANGE_1;
                    static constexpr float MATCHING_VOLTAGE_B = 0.730;

                    REQUIRE_FALSE(mss::lab::pmic::update::set_range_helper(l_pmic, mss::pmic::rail::SWB, TARGET_RANGE_B));
                    REQUIRE_FALSE(mss::lab::pmic::update::set_voltage_helper(l_pmic, mss::pmic::rail::SWB, MATCHING_VOLTAGE_B));

                    l_reg_contents.flush<0>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R47_SWB_VOLTAGE_SETTING, l_reg_contents));
                    REQUIRE(l_reg_contents == TARGET_VOLTAGE_BITMAP_B);
#endif
                    // Test out of range
                    REQUIRE(mss::lab::pmic::update::set_range_helper(l_pmic, mss::pmic::rail::SWA, RANGE_OUT_OF_RANGE));
                    REQUIRE(mss::lab::pmic::update::set_voltage_helper(l_pmic, mss::pmic::rail::SWA, VOLTAGE_OUT_OF_RANGE));

                    // Test valid
                    REQUIRE_FALSE(mss::lab::pmic::update::set_range_helper(l_pmic, mss::pmic::rail::SWA, TARGET_RANGE_A));
                    REQUIRE_FALSE(mss::lab::pmic::update::set_voltage_helper(l_pmic, mss::pmic::rail::SWA, MATCHING_VOLTAGE_A));

                    REQUIRE_FALSE(mss::lab::pmic::update::set_range_helper(l_pmic, mss::pmic::rail::SWC, TARGET_RANGE_C));
                    REQUIRE_FALSE(mss::lab::pmic::update::set_voltage_helper(l_pmic, mss::pmic::rail::SWC, MATCHING_VOLTAGE_C));

                    REQUIRE_FALSE(mss::lab::pmic::update::set_range_helper(l_pmic, mss::pmic::rail::SWD, TARGET_RANGE_D));
                    REQUIRE_FALSE(mss::lab::pmic::update::set_voltage_helper(l_pmic, mss::pmic::rail::SWD, MATCHING_VOLTAGE_D));

                    // Now let's check the registers
                    l_reg_contents.flush<0>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R45_SWA_VOLTAGE_SETTING, l_reg_contents));
                    REQUIRE(l_reg_contents == TARGET_VOLTAGE_BITMAP_A);

                    l_reg_contents.flush<0>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R49_SWC_VOLTAGE_SETTING, l_reg_contents));
                    REQUIRE(l_reg_contents == TARGET_VOLTAGE_BITMAP_C);

                    l_reg_contents.flush<0>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R4B_SWD_VOLTAGE_SETTING, l_reg_contents));
                    REQUIRE(l_reg_contents == TARGET_VOLTAGE_BITMAP_D);

                    l_reg_contents.flush<0>();
                }

// R39 status register is broken in simics
// Should be fixed for next update
#ifdef SUET_PMIC_FIXED
                FAPI_INF("Test poll for burn completion");
                {
                    fapi2::buffer<uint8_t> l_code_contents;

                    // Write a zero (arbitrary invalid value
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(l_pmic, REGS::R39_COMMAND_CODES, l_code_contents)));

                    REQUIRE(mss::lab::pmic::update::poll_for_burn_completion(l_pmic));

                    // Write the valid code, should succeed
                    l_code_contents = CONSTS::BURN_COMPLETE;
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(l_pmic, REGS::R39_COMMAND_CODES, l_code_contents)));
                    REQUIRE_FALSE(mss::lab::pmic::update::poll_for_burn_completion(l_pmic));
                }
#endif
                // Not testing TI checker function as simics does not support writing to the vendor registers
                // The function implementation is trivial anyway

            } // for each pmic

            return 0;
        }); // for each OCMB
    } // section
#endif

}// scenario
}// test
}// mss
