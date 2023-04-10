/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/tests/pmic_bias_tool_ut.C $ */
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
/// @file pmic_bias_tool_ut.C
/// @brief Unit tests for pmic bias input processing API and helper functions
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <cstdarg>
#include <catch.hpp>
#include <pmic_bias/pmic_bias_tool_utils.H>
#include <pmic_bias/pmic_bias_utils.H>
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
#include <generic/memory/lib/utils/mss_math.H>
#include <mss_pmic_attribute_getters.H>
#include <mss_pmic_attribute_setters.H>
#include <pmic_common_lab.H>

namespace mss
{
namespace test
{
///
/// @brief Unit test cases for pmic_bias
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "pmic bias", "[pmic_bias_tool]")
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
    GIVEN("Test mss::lab::pmic::bias::setting in lab processing API")
    {
        std::vector<std::pair<std::string, mss::lab::pmic::bias::setting>> l_conversion =
        {
            {"SWA_VOLT", mss::lab::pmic::bias::setting::SWA_VOLT},
            {"SWB_VOLT", mss::lab::pmic::bias::setting::SWB_VOLT},
            {"SWC_VOLT", mss::lab::pmic::bias::setting::SWC_VOLT},
            {"SWD_VOLT", mss::lab::pmic::bias::setting::SWD_VOLT}
        };
        auto l_value = mss::lab::pmic::bias::setting::SWA_VOLT;

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

    // UNIT
    GIVEN("Test mss::lab::pmic::bias::unit in lab processing API")
    {
        std::vector<std::pair<std::string, mss::lab::pmic::bias::unit>> l_conversion =
        {
            {"PERCENT", mss::lab::pmic::bias::unit::PERCENT},
            {"VALUE", mss::lab::pmic::bias::unit::VALUE}
        };
        auto l_value = mss::lab::pmic::bias::unit::PERCENT;

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
        mss::lab::pmic::bias::inputs l_inputs;
        mss::lab::pmic::bias::inputs l_expected;

        // Checks help - we should exit with an error, no changes to inputs
        {
            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"-h",
                };
                REQUIRE(mss::lab::pmic::bias::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }

            {
                int l_argc = 1;
                char* l_argv[] =
                {
                    (char*)"--help",
                };
                REQUIRE(mss::lab::pmic::bias::process_inputs(l_argc, l_argv, l_inputs));
                REQUIRE(l_inputs == l_expected);
            }
        }

        // Checks no inputs - we should fail because mode, type, and address are required
        {
            int l_argc = 0;
            char* l_argv[] =
                {};
            REQUIRE(mss::lab::pmic::bias::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Checks bad ranged inputs and bad enum inputs
        {
            constexpr int NUM_ARGS = 3;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-m=err",
                (char*)"-t=err",
                (char*)"-a=123G", // not hex

            };

            REQUIRE(mss::lab::pmic::bias::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }

        // Set new expected value
        l_expected.iv_id = mss::lab::pmic::id::PMIC1;
        l_expected.iv_setting = mss::lab::pmic::bias::setting::SWB_VOLT;
        l_expected.iv_amount = 5.0;
        l_expected.iv_unit = mss::lab::pmic::bias::unit::PERCENT;
        l_expected.iv_force = true;

        // Checks a sample input
        {
            constexpr int NUM_ARGS = 5;
            int l_argc = NUM_ARGS;
            char* l_argv[] =
            {
                (char*)"-d=PMIC1",
                (char*)"-s=SWB_VOLT",
                (char*)"-v=5",
                (char*)"-u=PERCENT",
                (char*)"-f",
            };

            REQUIRE_FALSE(mss::lab::pmic::bias::process_inputs(l_argc, l_argv, l_inputs));
            REQUIRE(l_inputs == l_expected);
        }
    }

    GIVEN("Checks help screen")
    {
        {
            const std::vector<std::string> l_examples =
            {
                "-d=PMIC0 -s=SWA_VOLT -v=-7.5 -u=PERCENT # Bias SWA_VOLT of PMIC0 -7.5%",
                "-d=PMIC1 -s=SWB_VOLT -v=0.9 -u=VALUE    # Bias SWB_VOLT of PMIC1 to 0.9V",
                "-h # Runs the help screen"
            };
            // Test help screen
            const auto l_lines = mss::lab::input::generate_help_screen_helper("pmic_bias_tool",
                                 mss::lab::pmic::bias::bias_input_processing::FIELDS, l_examples);

            const std::vector<std::string> l_expected_help =
            {
                "wrapper: pmic_bias_tool.exe",
                "Git commit ID: " + GIT_COMMIT_ID,
                "",
                "Input format:",
                "-parameter  description | input type | REQUIRED/Default: $value | optional value check",
                "",
                "Inputs:",
                "-d=     PMIC Device/ID to Bias | enum | REQUIRED",
                "        PMIC0",
                "        PMIC1",
                "        ALL",
                "-s=     Setting to bias | enum | REQUIRED",
                "        SWA_VOLT",
                "        SWB_VOLT",
                "        SWC_VOLT",
                "        SWD_VOLT",
                "-v=     Value/Percent to set | float | REQUIRED",
                "-u=     Unit to use for bias (VALUE in Volts) | enum | REQUIRED",
                "        VALUE",
                "        PERCENT",
                "-f      Force, ignore 10% change limit | flag | Default: false",
                "",
                "Examples:",
                "    pmic_bias_tool.exe -d=PMIC0 -s=SWA_VOLT -v=-7.5 -u=PERCENT # Bias SWA_VOLT of PMIC0 -7.5%",
                "    pmic_bias_tool.exe -d=PMIC1 -s=SWB_VOLT -v=0.9 -u=VALUE    # Bias SWB_VOLT of PMIC1 to 0.9V",
                "    pmic_bias_tool.exe -h # Runs the help screen"
            };

            REQUIRE(l_lines == l_expected_help);
        }
    }

    GIVEN("Test rounding functions")
    {
        FAPI_INF("Tests mss::round_to_nearest_multiple")
        {
            uint64_t l_expected = 1000;
            uint64_t l_value = 1002;
            uint64_t l_round_amount = 5;

            REQUIRE(mss::round_to_nearest_multiple(l_value, l_round_amount) == l_expected);

            l_value = 1003;
            l_expected = 1005;

            REQUIRE(mss::round_to_nearest_multiple(l_value, l_round_amount) == l_expected);
        }
        FAPI_INF("Test pmic_bias_utils lab::round_rail_target_voltage")
        {
            uint32_t l_expected = 1205;
            uint32_t l_value = 1206;

            REQUIRE(mss::lab::pmic::bias::round_rail_target_voltage(l_value) == l_expected);

            l_expected = 1210;
            l_value = 1208;

            REQUIRE(mss::lab::pmic::bias::round_rail_target_voltage(l_value) == l_expected);

            l_expected = 1205;
            l_value = 1205;

            REQUIRE(mss::lab::pmic::bias::round_rail_target_voltage(l_value) == l_expected);
        }
    }

    GIVEN("Checks JEDEC-compliant helper functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
#ifdef SUET_WORKING_NOW

            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
            constexpr auto RANGE_1 = CONSTS::RANGE_1;
            constexpr auto RANGE_0 = CONSTS::RANGE_0;

            // for each pmic
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                // arbitrary value
                static constexpr uint8_t TEST_VOLT_SETTING_BITMAP = 0b0010000;

                // for each rail
                for (uint8_t l_rail = mss::pmic::rail::SWA; l_rail <= mss::pmic::rail::SWD; ++ l_rail)
                {
                    // Hold on to current values
                    fapi2::buffer<uint8_t> l_current_voltage_setting;
                    fapi2::buffer<uint8_t> l_current_voltage_range;

                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(
                                      l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_current_voltage_setting));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(
                                      l_pmic, REGS::R2B, l_current_voltage_range));

                    FAPI_INF("Test a sample value on range 0")
                    {
                        fapi2::buffer<uint8_t> l_target_voltage_setting;
                        l_target_voltage_setting.insertFromRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>
                        (TEST_VOLT_SETTING_BITMAP);

                        fapi2::buffer<uint8_t> l_target_voltage_range(l_current_voltage_range);

                        // Force range 0
                        l_target_voltage_range.clearBit(mss::pmic::VOLT_RANGE_FLDS[l_rail]);

                        // Write these back to registers
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write(
                                          l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_target_voltage_setting));
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(
                                          l_pmic, REGS::R2B, l_target_voltage_range));

                        // Get back
                        uint32_t l_voltage_resp = 0;
                        uint8_t l_range_select_resp = 0;
                        REQUIRE_FALSE(mss::lab::pmic::get_current_rail_voltage(l_pmic,
                                      static_cast<mss::pmic::rail>(l_rail), l_voltage_resp, l_range_select_resp));

                        // Test valid response
                        REQUIRE(l_range_select_resp == RANGE_0);

                        // Convert voltage back to a bitmap and make sure it matches
                        REQUIRE(l_voltage_resp ==
                                (mss::pmic::VOLT_RANGE_MINS[l_rail][l_range_select_resp] + (CONSTS::VOLT_STEP *
                                        TEST_VOLT_SETTING_BITMAP)));
                    }

                    FAPI_INF("Test a sample value on range 1")
                    {
                        fapi2::buffer<uint8_t> l_target_voltage_setting;
                        l_target_voltage_setting.insertFromRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>
                        (TEST_VOLT_SETTING_BITMAP);

                        fapi2::buffer<uint8_t> l_target_voltage_range(l_current_voltage_range);

                        // Force range 1
                        l_target_voltage_range.setBit(mss::pmic::VOLT_RANGE_FLDS[l_rail]);

                        // Write these back to registers
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write(
                                          l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_target_voltage_setting));
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(
                                          l_pmic, REGS::R2B, l_target_voltage_range));

                        // Get back
                        uint32_t l_voltage_resp = 0;
                        uint8_t l_range_select_resp = 0;
                        REQUIRE_FALSE(mss::lab::pmic::get_current_rail_voltage(l_pmic,
                                      static_cast<mss::pmic::rail>(l_rail), l_voltage_resp, l_range_select_resp));

                        // Test valid response
                        REQUIRE(l_range_select_resp == RANGE_1);

                        // Convert voltage back to a bitmap and make sure it matches
                        REQUIRE(l_voltage_resp ==
                                (mss::pmic::VOLT_RANGE_MINS[l_rail][l_range_select_resp] + (CONSTS::VOLT_STEP *
                                        TEST_VOLT_SETTING_BITMAP)));
                    }

                    // Put back old values
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(
                                      l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_current_voltage_setting));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(
                                      l_pmic, REGS::R2B, l_current_voltage_range));
                }

                for (uint8_t l_rail = mss::pmic::rail::SWA; l_rail <= mss::pmic::rail::SWC; ++ l_rail)
                {
                    FAPI_INF("Test fail due to voltage range out of range");
                    {
                        // arbitrary rail
                        static constexpr uint8_t OUT_OF_RANGE = 2;
                        static constexpr uint32_t TEST_VOLTAGE = 1200;

                        REQUIRE(mss::lab::pmic::bias::set_new_rail_voltage(l_pmic, mss::pmic::rail::SWA, TEST_VOLTAGE, OUT_OF_RANGE));
                    }

                    FAPI_INF("Write a sample voltage on range 0");
                    {
                        // Hold on to current values
                        fapi2::buffer<uint8_t> l_current_voltage_setting;
                        fapi2::buffer<uint8_t> l_current_voltage_range;

                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(
                                          l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_current_voltage_setting));
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(
                                          l_pmic, REGS::R2B, l_current_voltage_range));

                        // Test on range0: 1.2V, HEX: 50
                        static constexpr uint32_t TEST_VOLTAGE = 1200;
                        static constexpr uint8_t EXPECTED_BITMAP_SWA = 0x50;

                        REQUIRE_FALSE(mss::lab::pmic::bias::set_new_rail_voltage(l_pmic,
                                      static_cast<mss::pmic::rail>(l_rail), TEST_VOLTAGE, CONSTS::RANGE_0));

                        // Check the bitmap first (manually)
                        fapi2::buffer<uint8_t> l_voltage_bitmap_buffer;
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_voltage_bitmap_buffer));

                        // Extract out the voltage reg result
                        uint8_t l_voltage_reg_result = 0;
                        l_voltage_bitmap_buffer.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>
                        (l_voltage_reg_result);

                        REQUIRE(EXPECTED_BITMAP_SWA == l_voltage_reg_result);

                        uint32_t l_voltage_resp = 0;
                        uint8_t l_range_select_resp = 0;

                        // Check the voltage value matches as well as the range bit
                        REQUIRE_FALSE(mss::lab::pmic::get_current_rail_voltage(l_pmic,
                                      static_cast<mss::pmic::rail>(l_rail), l_voltage_resp, l_range_select_resp));

                        REQUIRE(l_voltage_resp == TEST_VOLTAGE);
                        REQUIRE(l_range_select_resp == RANGE_0);

                        // Put back old values
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write(
                                          l_pmic, mss::pmic::VOLT_SETTING_ACTIVE_REGS[l_rail], l_current_voltage_setting));
                        REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(
                                          l_pmic, REGS::R2B, l_current_voltage_range));
                    }
                } // for each rail
            } // for each pmic

            FAPI_INF("Test running the full pmic_bias procedure")
            {
                const auto l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

                // Let's use SWA PMIC0, SWC for PMIC1

                // Save off attributes
                uint8_t l_saved_pmic0_swa_voltage = 0;
                uint8_t l_saved_pmic1_swc_voltage = 0;
                REQUIRE_FALSE(mss::attr::get_pmic0_swa_voltage_setting(i_ocmb_target, l_saved_pmic0_swa_voltage));
                REQUIRE_FALSE(mss::attr::get_pmic1_swc_voltage_setting(i_ocmb_target, l_saved_pmic1_swc_voltage));

                // First let's put in a value (0x04 == 820mV, can be calculated via any pmic spec)

                static constexpr uint8_t TEST_VOLT_1 = 0x04;
                static constexpr uint8_t TEST_VOLT_2 = 0x1E;

                REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_setting(i_ocmb_target, TEST_VOLT_1));

                // SWC (0x1E == 950mV, can be calculated via any pmic spec)
                REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_setting(i_ocmb_target, TEST_VOLT_2));

                // Now let's inject this into SWA for PMIC0
                mss::lab::pmic::id l_id = mss::lab::pmic::id::PMIC0;
                mss::lab::pmic::bias::setting l_setting = mss::lab::pmic::bias::setting::SWA_VOLT;
                float l_amount = 0;
                mss::lab::pmic::bias::unit l_unit = mss::lab::pmic::bias::unit::PERCENT;
                bool l_force = true;

                // Since we're biasing by 0%, we should expect the voltage to match
                REQUIRE_FALSE(mss::lab::pmic::bias_chip(i_ocmb_target, l_id, l_setting, l_amount, l_unit, l_force));

                // Let's do the same for PMIC1 SWC
                {
                    l_id = mss::lab::pmic::id::PMIC1;
                    l_setting = mss::lab::pmic::bias::setting::SWC_VOLT;
                    l_amount = 0;
                    l_unit = mss::lab::pmic::bias::unit::PERCENT;
                    l_force = true;
                    REQUIRE_FALSE(mss::lab::pmic::bias_chip(i_ocmb_target, l_id, l_setting, l_amount, l_unit, l_force));
                }

                for (const auto& l_pmic : l_pmics)
                {
                    if ((mss::index(l_pmic) % CONSTS::NUM_PRIMARY_PMICS) == mss::pmic::id::PMIC0)
                    {
                        fapi2::buffer<uint8_t> l_volt_buffer;
                        // PMIC 0 SWA
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R21_SWA_VOLTAGE_SETTING, l_volt_buffer));

                        uint8_t l_voltage_bitmap = 0;
                        l_volt_buffer.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

                        REQUIRE(l_voltage_bitmap == TEST_VOLT_1);
                    }
                    else
                    {
                        // PMIC 1 SWC
                        fapi2::buffer<uint8_t> l_volt_buffer;
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R25_SWC_VOLTAGE_SETTING, l_volt_buffer));

                        uint8_t l_voltage_bitmap = 0;
                        l_volt_buffer.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

                        REQUIRE(l_voltage_bitmap == TEST_VOLT_2);
                    }
                }

                // Now that that works, let's test that trying to go over 10% change will cause a fail
                // We will test this with VALUE mode such that it has to parse the attribute
                {
                    l_id = mss::lab::pmic::id::PMIC0;
                    l_setting = mss::lab::pmic::bias::setting::SWA_VOLT;
                    l_amount = 1.4;
                    l_unit = mss::lab::pmic::bias::unit::VALUE;
                    l_force = false;
                    REQUIRE(uint64_t(mss::lab::pmic::bias_chip(i_ocmb_target, l_id, l_setting, l_amount, l_unit, l_force))
                            == uint64_t(fapi2::FAPI2_RC_FALSE));
                }

                // Now let's try some percentage changes and make sure they are relative to the attribute
                {
                    l_id = mss::lab::pmic::id::PMIC0;
                    l_setting = mss::lab::pmic::bias::setting::SWA_VOLT;
                    l_amount = 8;
                    l_unit = mss::lab::pmic::bias::unit::PERCENT;
                    l_force = false;
                    REQUIRE_FALSE(mss::lab::pmic::bias_chip(i_ocmb_target, l_id, l_setting, l_amount, l_unit, l_force));

                    l_id = mss::lab::pmic::id::PMIC1;
                    l_setting = mss::lab::pmic::bias::setting::SWC_VOLT;
                    l_amount = 5;
                    l_unit = mss::lab::pmic::bias::unit::PERCENT;
                    l_force = false;
                    REQUIRE_FALSE(mss::lab::pmic::bias_chip(i_ocmb_target, l_id, l_setting, l_amount, l_unit, l_force));
                }

                // Check the results
                for (const auto& l_pmic : l_pmics)
                {
                    if ((mss::index(l_pmic) % CONSTS::NUM_PRIMARY_PMICS) == mss::pmic::id::PMIC0)
                    {
                        // TEST_VOLT_1 = 820mV
                        // 820mV * 1.08 = 885.6 == 885mV (closest increment of 5mV)
                        // 885 - 800 = 85 offset from base voltage
                        // 85 / 5mV step = 17
                        // 17d --> 0x11 <-- What we want to see at the end

                        static constexpr uint8_t EXPECTED_NEW_PMIC0_SWA = 0x11;
                        fapi2::buffer<uint8_t> l_volt_buffer;
                        // PMIC 0 SWA
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R21_SWA_VOLTAGE_SETTING, l_volt_buffer));

                        uint8_t l_voltage_bitmap = 0;
                        l_volt_buffer.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

                        REQUIRE(l_voltage_bitmap == EXPECTED_NEW_PMIC0_SWA);
                    }
                    else
                    {
                        // TEST_VOLT_0 = 950mV
                        // 950mV * 1.05 = 997.5.6 == 995 (closest increment of 5mV, .5 dropped)
                        // 995 - 800 = 195 offset from base voltage
                        // 195 / 5mV step = 39
                        // 39d --> 0x27 <-- What we want to see at the end

                        static constexpr uint8_t EXPECTED_NEW_PMIC1_SWC = 0x27;
                        fapi2::buffer<uint8_t> l_volt_buffer;
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R25_SWC_VOLTAGE_SETTING, l_volt_buffer));

                        uint8_t l_voltage_bitmap = 0;
                        l_volt_buffer.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

                        REQUIRE(l_voltage_bitmap == EXPECTED_NEW_PMIC1_SWC);
                    }
                }


                // Let's read out those regs and check that they match

                // Put back attributes
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_setting(i_ocmb_target, l_saved_pmic0_swa_voltage));
                REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_setting(i_ocmb_target, l_saved_pmic1_swc_voltage));
            }
#endif
            return 0;
        }); // for each OCMB
    } // section

    GIVEN("Checks procedure exit due to unset rail attributes")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

            // for each pmic
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                if ((mss::index(l_pmic) % CONSTS::NUM_PRIMARY_PMICS) == mss::pmic::id::PMIC0)
                {
                    // PMIC 0

                    // Set up an attribute with 0's
                    uint8_t l_saved_swa_voltage = 0;
                    uint8_t l_saved_swa_range = 0;
                    int8_t l_saved_swa_offset = 0;
                    int8_t l_saved_efd_swa_offset = 0;

                    // Just to call the function, not checked
                    uint32_t l_swa_volt = 0;
                    uint8_t l_swa_range = 0;

                    REQUIRE_FALSE(mss::attr::get_pmic0_swa_voltage_setting(i_ocmb_target, l_saved_swa_voltage));
                    REQUIRE_FALSE(mss::attr::get_pmic0_swa_voltage_range_select(i_ocmb_target, l_saved_swa_range));
                    REQUIRE_FALSE(mss::attr::get_pmic0_swa_voltage_offset(i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_saved_efd_swa_offset));

                    // Inject some 0's
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_setting(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_range_select(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_offset(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_efd_pmic0_swa_voltage_offset(i_ocmb_target, 0));


                    // Should fail, we've tested in above tests that this will pass with set attributes
                    REQUIRE(mss::lab::pmic::bias::get_nominal_rail_voltage(i_ocmb_target, l_pmic, mss::pmic::rail::SWA, l_swa_volt,
                            l_swa_range));

                    // Put back attrs
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_setting(i_ocmb_target, l_saved_swa_voltage));
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_range_select(i_ocmb_target, l_saved_swa_range));
                    REQUIRE_FALSE(mss::attr::set_pmic0_swa_voltage_offset(i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_saved_efd_swa_offset));

                }
                else
                {
                    // PMIC 1

                    // Set up an attribute with 0's
                    uint8_t l_saved_swc_voltage = 0;
                    uint8_t l_saved_swc_range = 0;
                    int8_t l_saved_swc_offset = 0;
                    int8_t l_saved_efd_swc_offset = 0;

                    // Just to call the function, not checked
                    uint32_t l_swc_volt = 0;
                    uint8_t l_swc_range = 0;

                    REQUIRE_FALSE(mss::attr::get_pmic1_swc_voltage_setting(i_ocmb_target, l_saved_swc_voltage));
                    REQUIRE_FALSE(mss::attr::get_pmic1_swc_voltage_range_select(i_ocmb_target, l_saved_swc_range));
                    REQUIRE_FALSE(mss::attr::get_pmic1_swc_voltage_offset(i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_pmic1_swc_voltage_offset(i_ocmb_target, l_saved_efd_swc_offset));

                    // Inject some 0's
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_setting(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_range_select(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_offset(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_efd_pmic1_swc_voltage_offset(i_ocmb_target, 0));

                    // Should fail, we've tested in above tests that this will pass with set attributes
                    REQUIRE(mss::lab::pmic::bias::get_nominal_rail_voltage(i_ocmb_target, l_pmic, mss::pmic::rail::SWC, l_swc_volt,
                            l_swc_range));

                    // Put back attrs
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_setting(i_ocmb_target, l_saved_swc_voltage));
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_range_select(i_ocmb_target, l_saved_swc_range));
                    REQUIRE_FALSE(mss::attr::set_pmic1_swc_voltage_offset(i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_pmic1_swc_voltage_offset(i_ocmb_target, l_saved_efd_swc_offset));
                }

            }

            return 0;
        });
    }

}// scenario
}// test
}// mss
