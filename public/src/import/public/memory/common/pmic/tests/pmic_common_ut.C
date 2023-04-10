/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/pmic_common_ut.C $ */
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
/// @file pmic_common_ut.C
/// @brief Unit tests for common PMIC operations
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
#include <lib/i2c/i2c_pmic.H>
#include <lib/utils/pmic_consts.H>
#include <lib/utils/pmic_common_utils.H>
#include <generic/memory/lib/data_engine/data_engine_utils.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>

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
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "PMIC common", "[pmic_common]")
{
    SECTION("Check offset calculation")
    {
        using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
        uint8_t l_offset = 3;
        uint8_t l_direction = CONSTS::OFFSET_MINUS;
        int8_t l_expected = -3;

        REQUIRE(l_expected == mss::pmic::convert_to_signed_offset(l_offset, l_direction));

        l_offset = 25;
        l_direction = CONSTS::OFFSET_PLUS;
        l_expected = 25;

        REQUIRE(l_expected == mss::pmic::convert_to_signed_offset(l_offset, l_direction));

        // This function will not be tested for out of bounds data:
        // The offset is calculated from 7 bits of an SPD byte so there
        // should be no way to overflow (see note in function)
    }

    SECTION("Test check VIN_BULK threshold helper")
    {
        // Voltage readout register is read-only in simics, so we will test the helper
        bool l_is_ti = true;
        // Arbitrary revision for TI, since we don't care
        uint8_t l_rev = 0x30;
        // Min threshold bitmap. Stick some extra bits on the end to test the masking
        uint8_t l_min_threshold = 0b00111111;

        // 001XXXXX = 9500mV
        REQUIRE(9500 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));

        // 110XXXXX = 4250
        l_min_threshold = 0b11011001;
        REQUIRE(4250 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));

        // Rev 0x30 < 0x33 (REV C1), so we should see 4000mV flat
        l_is_ti = false;
        REQUIRE(4000 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));
        l_min_threshold = 0b10111001;
        REQUIRE(4000 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));

        // C1 Part, 101XXXXX = 5500
        l_rev = 0x33;
        REQUIRE(5500 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));

        // 100XXXXX = 6500
        l_min_threshold = 0b10001001;
        REQUIRE(6500 == mss::pmic::get_minimum_vin_bulk_threshold_helper(l_min_threshold, l_is_ti, l_rev));
    }

    SECTION("Test pmic::status::status_regs struct helper")
    {
        using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;

        mss::pmic::status::status_regs l_saved_values = {0};

        mss::pmic::status::status_reg_save_helper(REGS::R04, 0x04, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R05, 0x05, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R06, 0x06, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R08, 0x08, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R09, 0x09, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R0A, 0x0A, l_saved_values);
        mss::pmic::status::status_reg_save_helper(REGS::R0B, 0x0B, l_saved_values);
        // test an invalid register for completeness
        mss::pmic::status::status_reg_save_helper(REGS::R0C, 0x0C, l_saved_values);

        REQUIRE(l_saved_values.iv_r04 == 0x04);
        REQUIRE(l_saved_values.iv_r05 == 0x05);
        REQUIRE(l_saved_values.iv_r06 == 0x06);
        REQUIRE(l_saved_values.iv_r08 == 0x08);
        REQUIRE(l_saved_values.iv_r09 == 0x09);
        REQUIRE(l_saved_values.iv_r0A == 0x0A);
        REQUIRE(l_saved_values.iv_r0B == 0x0B);
    }

#ifdef SUET_WORKING_NOW

    SECTION("Checks JEDEC-compliant helper functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                FAPI_INF("Test Lock and Unlock Vendor Region Functions");
                {
                    REQUIRE_FALSE(mss::pmic::unlock_vendor_region(l_pmic));

                    // Test with and without return codes
                    fapi2::ReturnCode l_test_code = fapi2::RC_PMIC_ORDER_OUT_OF_RANGE; // arbitrary return code that could occur

                    // Make sure that we do get back the return code we give it
                    REQUIRE(uint64_t(l_test_code) == uint64_t(mss::pmic::lock_vendor_region(l_pmic, l_test_code)));

                    // Make sure we get back success when given success
                    REQUIRE_FALSE(mss::pmic::lock_vendor_region(l_pmic, fapi2::FAPI2_RC_SUCCESS));
                }
            }

            return 0;
        });
    }

#endif
}
} //test
}// mss
