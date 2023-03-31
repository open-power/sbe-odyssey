/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic_ddr5/tests/poweric_i2c_addr_get_ut.C $ */
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
/// @file poweric_i2c_addr_get_ut.C
/// @brief Unit tests for poweric_i2c_addr_get.C
///
// *HWP HWP Owner: Sneha Kadam <sneha.kadam1@ibm.com>
// *HWP FW Owner: Dan Crowell <dcrowell@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: CI

#include <cstdarg>
#include <fapi2.H>
#include <catch.hpp>
#include <generic/memory/tests/target_fixture.H>

#include <chips/ocmb/common/spd_access/poweric_i2c_addr_get.H>

namespace mss
{
namespace test
{

///
/// @brief Unit test cases for Power IC data engine
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "Test Power IC i2c address getter", "[poweric_i2c_addr_get]")
{
    GIVEN("Testing Power IC i2c address getter")
    {
        // Arbitrary values
        constexpr uint8_t SAMPLE_I2C_ADDR0 = 0xCE;
        constexpr uint8_t SAMPLE_I2C_ADDR1 = 0xF0;
        constexpr uint8_t SAMPLE_I2C_ADDR2 = 0x52;
        constexpr uint8_t SAMPLE_I2C_ADDR3 = 0xB3;

        constexpr uint8_t SAMPLE_I2C_ADDR_DDR4 = 0;

        // Arbitrary length
        uint8_t l_fake_spd[450] = {0};

        FAPI_INF("Test Power IC i2c DDR4 address");

        // force to DDR4 offsets
        l_fake_spd[DDIMM_SPD::DDR_BYTE] = DDIMM_SPD::MEM_DDR4;
        l_fake_spd[DDIMM_SPD::DIMM_TYPE_BYTE] = DDIMM_SPD::MEM_DDIMM;
        l_fake_spd[DDIMM_SPD::DDR4_DIMM_SIZE_BYTE] = DDIMM_SPD::MEM_4U;

        // Test the good case, and an out of bounds case
        REQUIRE(SAMPLE_I2C_ADDR_DDR4 == get_poweric_i2c_addr(&l_fake_spd[0], 0));
        REQUIRE(0 == get_poweric_i2c_addr(&l_fake_spd[0], 4));

        // Test an unacceptable SPD.
        l_fake_spd[DDIMM_SPD::DDR4_DIMM_SIZE_BYTE] = 0;
        REQUIRE(0 == get_poweric_i2c_addr(&l_fake_spd[0], 0));

        // Test for DDR5
        FAPI_INF("Test Power IC i2c DDR5 address");

        l_fake_spd[DDIMM_SPD::DDR_BYTE] = DDIMM_SPD::MEM_DDR5;
        l_fake_spd[DDIMM_SPD::DIMM_TYPE_BYTE] = DDIMM_SPD::MEM_DDIMM;
        l_fake_spd[DDIMM_SPD::DDR5_DIMM_SIZE_BYTE] = DDIMM_SPD::MEM_4U;

        l_fake_spd[SPD_POWERIC0_I2C_BYTE] = SAMPLE_I2C_ADDR0;
        l_fake_spd[SPD_POWERIC1_I2C_BYTE] = SAMPLE_I2C_ADDR1;
        l_fake_spd[SPD_POWERIC2_I2C_BYTE] = SAMPLE_I2C_ADDR2;
        l_fake_spd[SPD_POWERIC3_I2C_BYTE] = SAMPLE_I2C_ADDR3;

        REQUIRE(SAMPLE_I2C_ADDR0 == get_poweric_i2c_addr(&l_fake_spd[0], POWERIC0));
        REQUIRE(SAMPLE_I2C_ADDR1 == get_poweric_i2c_addr(&l_fake_spd[0], POWERIC1));
        REQUIRE(SAMPLE_I2C_ADDR2 == get_poweric_i2c_addr(&l_fake_spd[0], POWERIC2));
        REQUIRE(SAMPLE_I2C_ADDR3 == get_poweric_i2c_addr(&l_fake_spd[0], POWERIC3));

        // Try a few out of range values (should get 0)
        REQUIRE(0 == get_poweric_i2c_addr(&l_fake_spd[0], 4));
        REQUIRE(0 == get_poweric_i2c_addr(&l_fake_spd[0], 9));

    }
}
} // test
} // mss
