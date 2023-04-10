/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/pmic_i2c_ut.C $ */
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
/// @file pmic_i2c_ut.C
/// @brief Unit tests for PMIC i2c
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: CI

#include <cstdarg>
#include <fapi2.H>
#include <catch.hpp>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/tests/target_fixture.H>
#include <generic/memory/lib/utils/c_str.H>
#include <lib/i2c/i2c_pmic.H>

namespace mss
{
namespace test
{

using fapi2::TARGET_TYPE_OCMB_CHIP;

///
/// @brief Unit test cases for PMIC i2C
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "pmic i2c utilities", "[i2c_pmic]")
{
#ifdef SUET_WORKING_NOW
    GIVEN("Checks PMIC i2C operations")
    {
        for_each_target([](const fapi2::Target<TARGET_TYPE_OCMB_CHIP>& i_target)
        {
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_target))
            {
                FAPI_INF("Check PMIC i2c reads/writes");
                {
                    static constexpr uint8_t TEST_VAL = 0x3C; // arbitrary data
                    static constexpr uint8_t TEST_REG = 0x35; // arbitrary RW register
                    fapi2::buffer<uint8_t> l_test_buffer(TEST_VAL);
                    fapi2::buffer<uint8_t> l_old_data;

                    // Store away old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, TEST_REG, l_old_data));

                    // Write, then read back
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(l_pmic, TEST_REG, l_test_buffer));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read (l_pmic, TEST_REG, l_test_buffer));

                    // Check for match
                    REQUIRE(uint8_t(l_test_buffer) == TEST_VAL);

                    // Put back old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(l_pmic, TEST_REG, l_old_data));
                }

                FAPI_INF("Check PMIC i2c reverse reads/writes")
                {
                    static constexpr uint8_t TEST_BIT = 2;
                    static constexpr uint8_t REVERSE_OFFSET = 7;
                    static constexpr uint8_t TEST_REG = 0x35;

                    fapi2::buffer<uint8_t> l_test_buffer(0);
                    fapi2::buffer<uint8_t> l_old_data;

                    l_test_buffer.setBit<TEST_BIT>();

                    // Store away old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, TEST_REG, l_old_data));

                    // Write, then read back
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, TEST_REG, l_test_buffer));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer (l_pmic, TEST_REG, l_test_buffer));

                    // Since we reverse on the write and then again on the read, the same bit should be set when we get it back
                    REQUIRE(l_test_buffer.getBit<TEST_BIT>());

                    // Read and check the non-reversed value
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, TEST_REG, l_test_buffer));
                    REQUIRE(l_test_buffer.getBit < REVERSE_OFFSET - TEST_BIT > ()); // bit 5 should now be set

                    // Put back old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write(l_pmic, TEST_REG, l_old_data));
                }

                FAPI_INF("Check PMIC i2c write contiguous")
                {
                    // Write 2 bytes contiguously and read them

                    static constexpr uint8_t TEST_VAL   = 0x3C; // arbitrary data
                    static constexpr uint8_t TEST_VAL_1 = 0x3F; // arbitrary data for 2nd byte
                    static constexpr uint8_t TEST_REG   = 0x35; // Starting reg

                    fapi2::buffer<uint8_t> l_test_data[] = {TEST_VAL, TEST_VAL_1};
                    fapi2::buffer<uint8_t> l_test_read[] = {0, 0};
                    fapi2::buffer<uint8_t> l_old_data[] = {0, 0};
                    fapi2::buffer<uint8_t> l_old_data_1;

                    // Store away old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, TEST_REG, l_old_data[0]));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, (TEST_REG + 1), l_old_data[1]));

                    // Write contiguous
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_contiguous(l_pmic, TEST_REG, l_test_data));

                    // Read and check the non-reversed value
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, TEST_REG, l_test_read[0]));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, (TEST_REG + 1), l_test_read[1]));
                    REQUIRE(l_test_read[0] == TEST_VAL);
                    REQUIRE(l_test_read[1] == TEST_VAL_1);

                    // Put back old data
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_contiguous(l_pmic, TEST_REG, l_old_data));
                }
            }

            return 0;
        }); //for_each
    } // GIVEN
#endif
}// scenario

} // ns test
} // ns mss
