/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/pmic_n_mode_detect_ut.C $ */
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
/// @file pmic_telemetry_ut.C
/// @brief Unit tests for PMIC telemetry
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: CI

#include <fapi2.H>
#include <catch.hpp>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/tests/target_fixture.H>
#include <pmic_n_mode_detect.H>
#include <wrapper/pmic_n_mode_detect_wrap.H>

namespace mss
{

namespace test
{

///
/// @brief Unit test cases for PMIC common utilities
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "PMIC N Mode Detect", "[pmic_n_mode_detect]")
{
    SECTION("Checks PMIC Telemetry functions")
    {
        FAPI_INF("Test struct packing/sending API");

        runtime_n_mode_telem_info l_sample;
        runtime_n_mode_telem_info l_rcv;
        wrapper_ostream l_stream = wrapper_ostream();

        // Populate with sample values. These are mostly arbitrary
        l_sample.iv_revision = 0x01;
        l_sample.iv_aggregate_error = aggregate_state::N_MODE;
        l_sample.iv_pmic1_errors = 0;
        l_sample.iv_pmic2_errors = 1;
        l_sample.iv_pmic3_errors = 2;
        l_sample.iv_pmic4_errors = 3;

        l_sample.iv_telemetry_data.iv_gpio1_port_state = 0x55;
        l_sample.iv_telemetry_data.iv_gpio1_port_state = 0xCC;

        l_sample.iv_telemetry_data.iv_pmic1.iv_r08 = 0xA1;
        l_sample.iv_telemetry_data.iv_pmic1.iv_r09 = 0x2B;
        l_sample.iv_telemetry_data.iv_pmic1.iv_r0a = 0xC3;
        l_sample.iv_telemetry_data.iv_pmic1.iv_r0b = 0x4D;
        l_sample.iv_telemetry_data.iv_pmic1.iv_swa_current_mA = 0xABCD;
        l_sample.iv_telemetry_data.iv_pmic1.iv_swb_current_mA = 0xDEF1;
        l_sample.iv_telemetry_data.iv_pmic1.iv_swc_current_mA = 0x1234;
        l_sample.iv_telemetry_data.iv_pmic1.iv_swd_current_mA = 0x5678;
        l_sample.iv_telemetry_data.iv_pmic1.iv_vin_bulk_mV = 0x9ABC;
        l_sample.iv_telemetry_data.iv_pmic1.iv_temp_c = 0xDEF1;
        l_sample.iv_telemetry_data.iv_pmic1.iv_r33 = 0x56;

        l_sample.iv_telemetry_data.iv_pmic2.iv_r08 = 0x12;
        l_sample.iv_telemetry_data.iv_pmic2.iv_r09 = 0x34;
        l_sample.iv_telemetry_data.iv_pmic2.iv_r0a = 0x56;
        l_sample.iv_telemetry_data.iv_pmic2.iv_r0b = 0x78;
        l_sample.iv_telemetry_data.iv_pmic2.iv_swa_current_mA = 0x0123;
        l_sample.iv_telemetry_data.iv_pmic2.iv_swb_current_mA = 0x8765;
        l_sample.iv_telemetry_data.iv_pmic2.iv_swc_current_mA = 0xBCBC;
        l_sample.iv_telemetry_data.iv_pmic2.iv_swd_current_mA = 0xDEAD;
        l_sample.iv_telemetry_data.iv_pmic2.iv_vin_bulk_mV = 0x0987;
        l_sample.iv_telemetry_data.iv_pmic2.iv_temp_c = 0x7655;
        l_sample.iv_telemetry_data.iv_pmic2.iv_r33 = 0x01;

        l_sample.iv_telemetry_data.iv_pmic3.iv_r08 = 0xA3;
        l_sample.iv_telemetry_data.iv_pmic3.iv_r09 = 0x5B;
        l_sample.iv_telemetry_data.iv_pmic3.iv_r0a = 0xC9;
        l_sample.iv_telemetry_data.iv_pmic3.iv_r0b = 0x0D;
        l_sample.iv_telemetry_data.iv_pmic3.iv_swa_current_mA = 0xABED;
        l_sample.iv_telemetry_data.iv_pmic3.iv_swb_current_mA = 0xEEF1;
        l_sample.iv_telemetry_data.iv_pmic3.iv_swc_current_mA = 0x12E4;
        l_sample.iv_telemetry_data.iv_pmic3.iv_swd_current_mA = 0x567E;
        l_sample.iv_telemetry_data.iv_pmic3.iv_vin_bulk_mV = 0x9AEC;
        l_sample.iv_telemetry_data.iv_pmic3.iv_temp_c = 0xEEF1;
        l_sample.iv_telemetry_data.iv_pmic3.iv_r33 = 0x5E;

        l_sample.iv_telemetry_data.iv_pmic4.iv_r08 = 0x1B;
        l_sample.iv_telemetry_data.iv_pmic4.iv_r09 = 0xB4;
        l_sample.iv_telemetry_data.iv_pmic4.iv_r0a = 0x5B;
        l_sample.iv_telemetry_data.iv_pmic4.iv_r0b = 0xB8;
        l_sample.iv_telemetry_data.iv_pmic4.iv_swa_current_mA = 0x0B23;
        l_sample.iv_telemetry_data.iv_pmic4.iv_swb_current_mA = 0x876B;
        l_sample.iv_telemetry_data.iv_pmic4.iv_swc_current_mA = 0xBCBE;
        l_sample.iv_telemetry_data.iv_pmic4.iv_swd_current_mA = 0xDEBD;
        l_sample.iv_telemetry_data.iv_pmic4.iv_vin_bulk_mV = 0x09B7;
        l_sample.iv_telemetry_data.iv_pmic4.iv_temp_c = 0x7BB5;
        l_sample.iv_telemetry_data.iv_pmic4.iv_r33 = 0xB1;

        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch1_mV = 0x8907;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch2_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch3_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch4_mV = 0xABCD;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch5_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch6_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_recent_ch7_mV = 0xDEDE;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch1_mV = 0xFEED;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch2_mV = 0xB0B0;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch3_mV = 0xD3AD;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch4_mV = 0xBEEF;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch5_mV = 0xBEAD;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch6_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_max_ch7_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch1_mV = 0xEEDE;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch2_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch3_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch4_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch5_mV = 0x0938;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch6_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc1.iv_min_ch7_mV = 0x4960;

        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch1_mV = 0x8907;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch2_mV = 0x4968;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch3_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch4_mV = 0xFEFE;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch5_mV = 0xFFFF;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch6_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_recent_ch7_mV = 0xDEDE;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch1_mV = 0xFEED;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch2_mV = 0xB0B0;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch3_mV = 0xD3AD;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch4_mV = 0xBEEF;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch5_mV = 0x0F6E;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch6_mV = 0x1011;
        l_sample.iv_telemetry_data.iv_adc2.iv_max_ch7_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch0_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch1_mV = 0xEEDE;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch2_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch3_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch4_mV = 0x9ABC;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch5_mV = 0x0938;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch6_mV = 0x1234;
        l_sample.iv_telemetry_data.iv_adc2.iv_min_ch7_mV = 0x4960;

        send_struct(l_sample, l_stream);

        // Get back the struct, make sure they match
        l_rcv = l_stream.get_struct();
        REQUIRE(memcmp(&l_rcv, &l_sample, sizeof(l_rcv)) == MEMCMP_EQUAL);
    }

    SECTION("Checks GPIO PMIC state for each pair function")
    {
        {
            FAPI_INF("Test for PMIC PAIR 0");
            aggregate_state l_output_state = GI2C_I2C_FAIL;
            fapi2::buffer<uint8_t> l_failed_pmics_1;
            fapi2::buffer<uint8_t> l_failed_pmics_2;

            get_gpio_pmic_state<PAIR0>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == GI2C_I2C_FAIL);

            l_output_state = N_PLUS_1;
            get_gpio_pmic_state<PAIR0>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_PLUS_1);

            l_failed_pmics_1.setBit<PAIR0>();
            get_gpio_pmic_state<PAIR0>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_MODE);

            l_failed_pmics_1.clearBit<PAIR0>();
            l_failed_pmics_2.setBit<PAIR0>();
            get_gpio_pmic_state<PAIR0>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_MODE);

            l_failed_pmics_1.setBit<PAIR0>();
            get_gpio_pmic_state<PAIR0>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == LOST);
        }

        {
            FAPI_INF("Test for PMIC PAIR 1");
            aggregate_state l_output_state = GI2C_I2C_FAIL;
            fapi2::buffer<uint8_t> l_failed_pmics_1;
            fapi2::buffer<uint8_t> l_failed_pmics_2;

            get_gpio_pmic_state<PAIR1>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == GI2C_I2C_FAIL);

            l_output_state = N_PLUS_1;
            get_gpio_pmic_state<PAIR1>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_PLUS_1);

            l_failed_pmics_1.setBit<PAIR1>();
            get_gpio_pmic_state<PAIR1>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_MODE);

            l_failed_pmics_1.clearBit<PAIR1>();
            l_failed_pmics_2.setBit<PAIR1>();
            get_gpio_pmic_state<PAIR1>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == N_MODE);

            l_failed_pmics_1.setBit<PAIR1>();
            get_gpio_pmic_state<PAIR1>(l_output_state, l_failed_pmics_1, l_failed_pmics_2);
            REQUIRE(l_output_state == LOST);
        }
    }
}
} //test
}// mss
