/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/gpio_adc_i2c_addr_get_ut.C $ */
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
/// @file gpio_adc_i2c_addr_get_ut.C
/// @brief Unit tests for gpio_adc_i2c_addr_get.C
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP FW Owner: Dan Crowell <dcrowell@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: CI

#include <cstdarg>
#include <fapi2.H>
#include <catch.hpp>
#include <generic/memory/tests/target_fixture.H>

#include <chips/ocmb/common/spd_access/gpio_adc_i2c_addr_get.H>

namespace mss
{
namespace test
{

///
/// @brief Unit test cases for GPIO/ADC i2c address getters
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "Test GPIO/ADC i2c address getter", "[gpio_adc_i2c_addr_get]")
{
    GIVEN("Testing GPIO/ADC i2c address getter")
    {
        // Constants, addresses from the 4U DDIMM spec
        constexpr uint8_t ADC1  = 0x20;
        constexpr uint8_t ADC2  = 0x2E;
        constexpr uint8_t GPIO0 = 0x70;
        constexpr uint8_t GPIO1 = 0x7E;

        // Constants, addresses from the 4U DDR5 DDIMM spec
        constexpr uint16_t ADC_DDR5 = 286;
        // Sample address for DDR5 ADC
        constexpr uint8_t ADC_DDR5_ADDR = 0x12;

        // Arbitrary SPD blob to work with
        uint8_t fake_spd[300] = {0};

        FAPI_INF("Test DDR4 GPIO/ADC i2c address getter");

        // Setup an acceptable SPD. get_gpio_adc_i2c_addr requires DDR4 4U DDIMM to return a valid address
        fake_spd[DDIMM_SPD::DDR_BYTE] = DDIMM_SPD::MEM_DDR4;
        fake_spd[DDIMM_SPD::DIMM_TYPE_BYTE] = DDIMM_SPD::MEM_DDIMM;
        fake_spd[DDIMM_SPD::DDR4_DIMM_SIZE_BYTE] = DDIMM_SPD::MEM_4U;

        // Test the 4 good cases, and an out of bounds case
        REQUIRE(ADC1 == get_gpio_adc_i2c_addr(&fake_spd[0], 0));
        REQUIRE(ADC2 == get_gpio_adc_i2c_addr(&fake_spd[0], 1));
        REQUIRE(GPIO0 == get_gpio_adc_i2c_addr(&fake_spd[0], 2));
        REQUIRE(GPIO1 == get_gpio_adc_i2c_addr(&fake_spd[0], 3));
        REQUIRE(0 == get_gpio_adc_i2c_addr(&fake_spd[0], 4));

        // Test an unacceptable SPD.
        fake_spd[DDIMM_SPD::DDR4_DIMM_SIZE_BYTE] = 0;
        REQUIRE(0 == get_gpio_adc_i2c_addr(&fake_spd[0], 3));

        FAPI_INF("Test DDR5 GPIO/ADC i2c address getter");

        // Setup an acceptable SPD. get_gpio_adc_i2c_addr requires DDR4 4U DDIMM to return a valid address
        fake_spd[DDIMM_SPD::DDR_BYTE] = DDIMM_SPD::MEM_DDR5;
        fake_spd[DDIMM_SPD::DIMM_TYPE_BYTE] = DDIMM_SPD::MEM_DDIMM;
        fake_spd[DDIMM_SPD::DDR5_DIMM_SIZE_BYTE] = DDIMM_SPD::MEM_4U;

        fake_spd[ADC_DDR5] = ADC_DDR5_ADDR;

        // Test the good case, and an out of bounds case
        REQUIRE(ADC_DDR5_ADDR == get_gpio_adc_i2c_addr(&fake_spd[0], 0));
        REQUIRE(0 == get_gpio_adc_i2c_addr(&fake_spd[0], 4));
    }
}
} // test
} // mss
