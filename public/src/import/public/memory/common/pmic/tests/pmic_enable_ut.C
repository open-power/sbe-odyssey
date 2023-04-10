/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/pmic_enable_ut.C $ */
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
/// @file pmic_enable_ut.C
/// @brief Unit tests for PMIC enable operations
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
#include <lib/utils/pmic_enable_utils.H>
#include <lib/utils/pmic_common_utils.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>
#include <pmic_enable.H>
#include <mss_generic_attribute_getters.H>
#include <mss_generic_attribute_setters.H>
#include <generic/memory/lib/utils/count_dimm.H>
#include <mss_pmic_attribute_accessors_manual.H>
#include <pmic_ut_attribute_setters.H>
#include <lib/utils/pmic_enable_4u_settings.H>

namespace mss
{

namespace test
{

///
/// @brief Unit test cases for PMIC enable utilities
/// @param[in] test_fixture
/// @param[in] description
/// @param[in] tag
/// @return void
/// @note ocmb_chip_target_test_fixture is the fixture to use with this test case
///
SCENARIO_METHOD(ocmb_chip_target_test_fixture, "PMIC enable", "[pmic_enable]")
{
#ifdef SUET_WORKING_NOW
    SECTION("Checks VR_ENABLE")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
                using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

#ifdef SUET_PMIC_FIXED
                using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
                constexpr auto PROGRAMMABLE_MODE = CONSTS::PROGRAMMABLE_MODE;
#endif
                FAPI_INF("Test VR Enable Command")
                {
                    REQUIRE_FALSE(mss::pmic::start_vr_enable(l_pmic));

                    fapi2::buffer<uint8_t> l_r2f_contents;
                    fapi2::buffer<uint8_t> l_r32_contents;

                    // Check that programmable mode bit and vr enable bit were set
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R2F, l_r2f_contents));
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

#ifdef SUET_PMIC_FIXED
                    REQUIRE(l_r2f_contents.getBit<FIELDS::R2F_SECURE_MODE>() == PROGRAMMABLE_MODE);
#endif

                    REQUIRE(l_r32_contents.getBit<FIELDS::R32_VR_ENABLE>());
                }

                FAPI_INF("Test VR Enable Read")
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    l_r32_contents.setBit<FIELDS::R32_VR_ENABLE>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

                    REQUIRE_FALSE(mss::pmic::status::check_for_vr_enable(i_ocmb_target, l_pmic));

                    l_r32_contents.clearBit<FIELDS::R32_VR_ENABLE>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

                    REQUIRE(uint64_t(fapi2::RC_PMIC_NOT_ENABLED) == uint64_t(mss::pmic::status::check_for_vr_enable(i_ocmb_target,
                            l_pmic)));
                }
            }

            return 0;
        });
    }
    SECTION("Checks PMIC SPD/ATTR biasing functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            auto l_dimms = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_DIMM>(i_ocmb_target);
            auto l_pmics = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);
            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

            // PMIC0 and PMIC1 of each DIMM
            for (const auto& l_pmic : l_pmics)
            {
                // Get the PMIC ID (0 or 1)
                const mss::pmic::id l_id = static_cast<mss::pmic::id>(mss::index(l_pmic) % CONSTS::NUM_PRIMARY_PMICS);

                // Now we have the PMIC, it's ID, and the DIMM that we need for the helper functions

                FAPI_INF("Test phase combinations");
                {
                    // Save to put back later
                    uint8_t l_saved_phase_comb;
                    REQUIRE_FALSE(mss::attr::get_phase_comb[l_id](i_ocmb_target, l_saved_phase_comb));

                    // Set to single phase
                    constexpr auto SINGLE_PHASE = CONSTS::SINGLE_PHASE;
                    constexpr auto DUAL_PHASE = CONSTS::DUAL_PHASE;

                    REQUIRE_FALSE(mss::attr::set_phase_comb[l_id](i_ocmb_target, SINGLE_PHASE));

                    // Call function
                    REQUIRE_FALSE(mss::pmic::bias_with_spd_phase_comb(l_pmic, i_ocmb_target, l_id));

                    // Check register
                    fapi2::buffer<uint8_t> l_phase;
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R4F, l_phase));
                    REQUIRE(l_phase.getBit<FIELDS::SWA_SWB_PHASE_MODE_SELECT>() == SINGLE_PHASE);

                    // Now Dual Phase
                    REQUIRE_FALSE(mss::attr::set_phase_comb[l_id](i_ocmb_target, DUAL_PHASE));

                    // Call function
                    REQUIRE_FALSE(mss::pmic::bias_with_spd_phase_comb(l_pmic, i_ocmb_target, l_id));

                    // Check register
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R4F, l_phase));
                    REQUIRE(l_phase.getBit<FIELDS::SWA_SWB_PHASE_MODE_SELECT>() == DUAL_PHASE);

                    // Put back old phase comb attribute
                    REQUIRE_FALSE(mss::attr::set_phase_comb[l_id](i_ocmb_target, l_saved_phase_comb));
                }

                FAPI_INF("Test voltage range settings");
                {
                    // TK - Skipping for now as bugs in the simics PMIC model have some bits in R2B as read-only
                }

                FAPI_INF("Test startup sequence");
                {
                    // Here we go...

                    // Save off attributes to put back later
                    uint8_t l_saved_swa_order = 0;
                    uint8_t l_saved_swb_order = 0;
                    uint8_t l_saved_swc_order = 0;
                    uint8_t l_saved_swd_order = 0;

                    uint8_t l_saved_swa_delay = 0;
                    uint8_t l_saved_swb_delay = 0;
                    uint8_t l_saved_swc_delay = 0;
                    uint8_t l_saved_swd_delay = 0;

                    REQUIRE_FALSE(mss::attr::get_swa_sequence_order[l_id](i_ocmb_target, l_saved_swa_order));
                    REQUIRE_FALSE(mss::attr::get_swb_sequence_order[l_id](i_ocmb_target, l_saved_swb_order));
                    REQUIRE_FALSE(mss::attr::get_swc_sequence_order[l_id](i_ocmb_target, l_saved_swc_order));
                    REQUIRE_FALSE(mss::attr::get_swd_sequence_order[l_id](i_ocmb_target, l_saved_swd_order));

                    REQUIRE_FALSE(mss::attr::get_swa_sequence_delay[l_id](i_ocmb_target, l_saved_swa_delay));
                    REQUIRE_FALSE(mss::attr::get_swb_sequence_delay[l_id](i_ocmb_target, l_saved_swb_delay));
                    REQUIRE_FALSE(mss::attr::get_swc_sequence_delay[l_id](i_ocmb_target, l_saved_swc_delay));
                    REQUIRE_FALSE(mss::attr::get_swd_sequence_delay[l_id](i_ocmb_target, l_saved_swd_delay));

                    static constexpr uint8_t DELAY_4ms = 0b010;
                    static constexpr uint8_t DELAY_6ms = 0b011;
                    static constexpr uint8_t DELAY_8ms = 0b100;
                    static constexpr uint8_t DELAY_OUT_OF_RANGE = 0b1000;
                    static constexpr uint8_t RIGHT_ALIGN_OFFSET = 7;

                    fapi2::buffer<uint8_t> l_startup_seq_buffer;

                    {
                        // Try the out of range value to make sure that we get a warning
                        REQUIRE_FALSE(mss::attr::set_swa_sequence_order[l_id](i_ocmb_target, 1));
                        REQUIRE_FALSE(mss::attr::set_swa_sequence_delay[l_id](i_ocmb_target, DELAY_OUT_OF_RANGE));

                        REQUIRE(uint64_t(mss::pmic::bias_with_spd_startup_seq(l_pmic, i_ocmb_target, l_id)) ==
                                uint64_t(fapi2::RC_PMIC_DELAY_OUT_OF_RANGE));
                    }

                    // Try all at position 1 with delay 4ms (0b010). Not using seq. constants as they are self-explanatory
                    {
                        REQUIRE_FALSE(mss::attr::set_swa_sequence_order[l_id](i_ocmb_target, 1));
                        REQUIRE_FALSE(mss::attr::set_swb_sequence_order[l_id](i_ocmb_target, 1));
                        REQUIRE_FALSE(mss::attr::set_swc_sequence_order[l_id](i_ocmb_target, 1));
                        REQUIRE_FALSE(mss::attr::set_swd_sequence_order[l_id](i_ocmb_target, 1));

                        REQUIRE_FALSE(mss::attr::set_swa_sequence_delay[l_id](i_ocmb_target, DELAY_4ms));
                        REQUIRE_FALSE(mss::attr::set_swb_sequence_delay[l_id](i_ocmb_target, DELAY_4ms));
                        REQUIRE_FALSE(mss::attr::set_swc_sequence_delay[l_id](i_ocmb_target, DELAY_4ms));
                        REQUIRE_FALSE(mss::attr::set_swd_sequence_delay[l_id](i_ocmb_target, DELAY_4ms));

                        // Run function
                        REQUIRE_FALSE(mss::pmic::bias_with_spd_startup_seq(l_pmic, i_ocmb_target, l_id));

                        // Not reversing buffer so we can read the sequence delay without another flip
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R40_POWER_ON_SEQUENCE_CONFIG_1, l_startup_seq_buffer));

                        // Check that the delay matches and that the rails are set to be enabled
                        REQUIRE((l_startup_seq_buffer & CONSTS::MAX_DELAY_BITMAP) == DELAY_4ms);
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        // Check that the rails are not set for the next sequences
                        l_startup_seq_buffer.flush<0>();
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R41_POWER_ON_SEQUENCE_CONFIG_2, l_startup_seq_buffer));

                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        l_startup_seq_buffer.flush<0>();
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R42_POWER_ON_SEQUENCE_CONFIG_3, l_startup_seq_buffer));

                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        l_startup_seq_buffer.flush<0>();
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R43_POWER_ON_SEQUENCE_CONFIG_4, l_startup_seq_buffer));

                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());
                    }

                    // Now lets switch B and D to seq 2, C to seq 3. Delays of 4,6,8,6 respectively
                    {
                        REQUIRE_FALSE(mss::attr::set_swb_sequence_order[l_id](i_ocmb_target, 2));
                        REQUIRE_FALSE(mss::attr::set_swc_sequence_order[l_id](i_ocmb_target, 3));
                        REQUIRE_FALSE(mss::attr::set_swd_sequence_order[l_id](i_ocmb_target, 2));

                        REQUIRE_FALSE(mss::attr::set_swb_sequence_delay[l_id](i_ocmb_target, DELAY_6ms));
                        REQUIRE_FALSE(mss::attr::set_swc_sequence_delay[l_id](i_ocmb_target, DELAY_8ms));
                        REQUIRE_FALSE(mss::attr::set_swd_sequence_delay[l_id](i_ocmb_target, DELAY_6ms));

                        // Run function
                        REQUIRE_FALSE(mss::pmic::bias_with_spd_startup_seq(l_pmic, i_ocmb_target, l_id));

                        // Check R40 SEQ 1
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R40_POWER_ON_SEQUENCE_CONFIG_1, l_startup_seq_buffer));
                        REQUIRE((l_startup_seq_buffer & CONSTS::MAX_DELAY_BITMAP) == DELAY_4ms);
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());

                        // Check only A enabled
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        // Check R41 SEQ 2
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R41_POWER_ON_SEQUENCE_CONFIG_2, l_startup_seq_buffer));
                        REQUIRE((l_startup_seq_buffer & CONSTS::MAX_DELAY_BITMAP) == DELAY_6ms);
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());

                        // Check A,B,D enabled
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        // Check R42 SEQ 3
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R42_POWER_ON_SEQUENCE_CONFIG_3, l_startup_seq_buffer));
                        REQUIRE((l_startup_seq_buffer & CONSTS::MAX_DELAY_BITMAP) == DELAY_8ms);
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());

                        // Check all enabled
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());

                        // Make sure R43 SEQ 4 is not enabled (we don't care about the delay in this case)
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R43_POWER_ON_SEQUENCE_CONFIG_4, l_startup_seq_buffer));
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_ENABLE > ());

                        // Check all disabled to satisfy footnote 2 of R40-R43
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWA_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWB_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWC_ENABLE > ());
                        REQUIRE_FALSE(l_startup_seq_buffer.getBit < RIGHT_ALIGN_OFFSET - FIELDS::SEQUENCE_SWD_ENABLE > ());
                    }

                    // Check out of bounds sequence
                    {
                        REQUIRE_FALSE(mss::attr::set_swb_sequence_order[l_id](i_ocmb_target, 5));
                        REQUIRE(uint64_t(mss::pmic::bias_with_spd_startup_seq(l_pmic, i_ocmb_target, l_id))
                                == uint64_t(fapi2::RC_PMIC_ORDER_OUT_OF_RANGE));
                    }

                    // Put back attributes
                    REQUIRE_FALSE(mss::attr::set_swa_sequence_order[l_id](i_ocmb_target, l_saved_swa_order));
                    REQUIRE_FALSE(mss::attr::set_swb_sequence_order[l_id](i_ocmb_target, l_saved_swb_order));
                    REQUIRE_FALSE(mss::attr::set_swc_sequence_order[l_id](i_ocmb_target, l_saved_swc_order));
                    REQUIRE_FALSE(mss::attr::set_swd_sequence_order[l_id](i_ocmb_target, l_saved_swd_order));

                    REQUIRE_FALSE(mss::attr::set_swa_sequence_delay[l_id](i_ocmb_target, l_saved_swa_delay));
                    REQUIRE_FALSE(mss::attr::set_swb_sequence_delay[l_id](i_ocmb_target, l_saved_swb_delay));
                    REQUIRE_FALSE(mss::attr::set_swc_sequence_delay[l_id](i_ocmb_target, l_saved_swc_delay));
                    REQUIRE_FALSE(mss::attr::set_swd_sequence_delay[l_id](i_ocmb_target, l_saved_swd_delay));

                }

                FAPI_INF("Test IDT set voltage function");
                {
                    // Save off attributes to put back later
                    uint8_t l_saved_swa_voltage = 0;
                    uint8_t l_saved_swb_voltage = 0;
                    uint8_t l_saved_swc_voltage = 0;
                    uint8_t l_saved_swd_voltage = 0;

                    int8_t l_saved_swa_offset = 0;
                    int8_t l_saved_swb_offset = 0;
                    int8_t l_saved_swc_offset = 0;
                    int8_t l_saved_swd_offset = 0;

                    int8_t l_saved_efd_swa_offset = 0;
                    int8_t l_saved_efd_swb_offset = 0;
                    int8_t l_saved_efd_swc_offset = 0;
                    int8_t l_saved_efd_swd_offset = 0;

                    REQUIRE_FALSE(mss::attr::get_swa_voltage_setting[l_id](i_ocmb_target, l_saved_swa_voltage));
                    REQUIRE_FALSE(mss::attr::get_swb_voltage_setting[l_id](i_ocmb_target, l_saved_swb_voltage));
                    REQUIRE_FALSE(mss::attr::get_swc_voltage_setting[l_id](i_ocmb_target, l_saved_swc_voltage));
                    REQUIRE_FALSE(mss::attr::get_swd_voltage_setting[l_id](i_ocmb_target, l_saved_swd_voltage));

                    REQUIRE_FALSE(mss::attr::get_swa_voltage_offset[l_id](i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::get_swb_voltage_offset[l_id](i_ocmb_target, l_saved_swb_offset));
                    REQUIRE_FALSE(mss::attr::get_swc_voltage_offset[l_id](i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::get_swd_voltage_offset[l_id](i_ocmb_target, l_saved_swd_offset));

                    REQUIRE_FALSE(mss::attr::get_efd_swa_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swa_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swb_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swb_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swc_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swc_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swd_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swd_offset));

                    // Try a sample value/bitmap in range with sample offsets
                    {
                        static constexpr uint8_t SAMPLE_VOLTAGE = 0b00111100; // 1100mV or 900mV

                        static constexpr int8_t SAMPLE_SIGNED_OFFSET_1 = -3;
                        static constexpr int8_t SAMPLE_SIGNED_OFFSET_2 = -7;
                        static constexpr int8_t SAMPLE_SIGNED_OFFSET_3 = 0b101;
                        static constexpr int8_t SAMPLE_SIGNED_OFFSET_4 = 0b1100;

                        // Set some combinations
                        // SWA
                        REQUIRE_FALSE(mss::attr::set_swa_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                        REQUIRE_FALSE(mss::attr::set_swa_voltage_offset[l_id](i_ocmb_target, 0));
                        REQUIRE_FALSE(mss::attr::set_efd_swa_voltage_offset[l_id](i_ocmb_target, 0));
                        static constexpr uint8_t SWA_EXPECTED = SAMPLE_VOLTAGE;

                        // SWB
                        REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                        REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_2));
                        REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_3));
                        static constexpr uint8_t SWB_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_2 + SAMPLE_SIGNED_OFFSET_3;

                        // SWC
                        REQUIRE_FALSE(mss::attr::set_swc_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                        REQUIRE_FALSE(mss::attr::set_swc_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_1));
                        REQUIRE_FALSE(mss::attr::set_efd_swc_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_1));
                        static constexpr uint8_t SWC_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_1 + SAMPLE_SIGNED_OFFSET_1;

                        // SWD
                        REQUIRE_FALSE(mss::attr::set_swd_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                        REQUIRE_FALSE(mss::attr::set_swd_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_4));
                        REQUIRE_FALSE(mss::attr::set_efd_swd_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_2));
                        static constexpr uint8_t SWD_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_4 + SAMPLE_SIGNED_OFFSET_2;

                        // Run procedure. Should return with success
                        REQUIRE_FALSE(mss::pmic::bias_with_spd_voltages<mss::pmic::vendor::IDT>(l_pmic, i_ocmb_target, l_id));

                        // Check the registers
                        fapi2::buffer<uint8_t> l_voltage_reg_contents;
                        // SWA
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R21_SWA_VOLTAGE_SETTING, l_voltage_reg_contents));
                        REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWA_EXPECTED);

                        // SWB
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R23_SWB_VOLTAGE_SETTING, l_voltage_reg_contents));
                        REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWB_EXPECTED);

                        // SWC
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R25_SWC_VOLTAGE_SETTING, l_voltage_reg_contents));
                        REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWC_EXPECTED);

                        // SWD
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R27_SWD_VOLTAGE_SETTING, l_voltage_reg_contents));
                        REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWD_EXPECTED);
                    }

                    // Now lets try something out of bounds on SWB (arbitrary)
                    {
                        static constexpr uint8_t SAMPLE_VOLTAGE = 0b01111110;
                        static constexpr uint8_t SAMPLE_OFFSET = 0b100; // will go out of bounds

                        REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                        REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_OFFSET));
                        REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, 0));

                        REQUIRE(uint64_t(mss::pmic::bias_with_spd_voltages<mss::pmic::vendor::IDT>(l_pmic, i_ocmb_target, l_id))
                                == uint64_t(fapi2::RC_PMIC_VOLTAGE_OUT_OF_RANGE));
                    }

                    // Put back attributes
                    REQUIRE_FALSE(mss::attr::set_swa_voltage_setting[l_id](i_ocmb_target, l_saved_swa_voltage));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, l_saved_swb_voltage));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_setting[l_id](i_ocmb_target, l_saved_swc_voltage));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_setting[l_id](i_ocmb_target, l_saved_swd_voltage));

                    REQUIRE_FALSE(mss::attr::set_swa_voltage_offset[l_id](i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, l_saved_swb_offset));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_offset[l_id](i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_offset[l_id](i_ocmb_target, l_saved_swd_offset));

                    REQUIRE_FALSE(mss::attr::set_efd_swa_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swa_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swb_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swc_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swc_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swd_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swd_offset));
                }

                FAPI_INF("Test current consumption warning threshold");
                {
                    uint8_t l_saved_swa_current_warning = 0;
                    uint8_t l_saved_swb_current_warning = 0;
                    uint8_t l_saved_swc_current_warning = 0;
                    uint8_t l_saved_swd_current_warning = 0;

                    // Save off attributes
                    REQUIRE_FALSE(mss::attr::get_swa_current_warning[l_id](i_ocmb_target, l_saved_swa_current_warning));
                    REQUIRE_FALSE(mss::attr::get_swb_current_warning[l_id](i_ocmb_target, l_saved_swb_current_warning));
                    REQUIRE_FALSE(mss::attr::get_swc_current_warning[l_id](i_ocmb_target, l_saved_swc_current_warning));
                    REQUIRE_FALSE(mss::attr::get_swd_current_warning[l_id](i_ocmb_target, l_saved_swd_current_warning));

                    {
                        static constexpr uint8_t SWA_CURRENT_WARNING = 0xA0;
                        static constexpr uint8_t SWB_CURRENT_WARNING = 0xB0;
                        static constexpr uint8_t SWC_CURRENT_WARNING = 0xC0;
                        static constexpr uint8_t SWD_CURRENT_WARNING = 0xE0;
                        fapi2::buffer<uint8_t> l_reg_contents;

                        REQUIRE_FALSE(mss::attr::set_swa_current_warning[l_id](i_ocmb_target, SWA_CURRENT_WARNING));
                        REQUIRE_FALSE(mss::attr::set_swb_current_warning[l_id](i_ocmb_target, SWB_CURRENT_WARNING));
                        REQUIRE_FALSE(mss::attr::set_swc_current_warning[l_id](i_ocmb_target, SWC_CURRENT_WARNING));
                        REQUIRE_FALSE(mss::attr::set_swd_current_warning[l_id](i_ocmb_target, SWD_CURRENT_WARNING));

                        // Run procedure
                        REQUIRE_FALSE(mss::pmic::set_current_limiter_warnings(l_pmic, i_ocmb_target));

                        // SWA
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R1C, l_reg_contents));
                        REQUIRE(l_reg_contents == SWA_CURRENT_WARNING);

                        // SWB
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R1D, l_reg_contents));
                        REQUIRE(l_reg_contents == SWB_CURRENT_WARNING);

                        // SWC
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R1E, l_reg_contents));
                        REQUIRE(l_reg_contents == SWC_CURRENT_WARNING);

                        // SWD
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R1F, l_reg_contents));
                        REQUIRE(l_reg_contents == SWD_CURRENT_WARNING);

                        // Test the 0 case
                        REQUIRE_FALSE(mss::attr::set_swc_current_warning[l_id](i_ocmb_target, 0x00));
                        REQUIRE_FALSE(mss::pmic::set_current_limiter_warnings(l_pmic, i_ocmb_target));

                        // SWC should remain unchanged. Should be the value from earlier.
                        REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R1E, l_reg_contents));
                        REQUIRE(l_reg_contents == SWC_CURRENT_WARNING);
                    }

                    // Put back attributes
                    REQUIRE_FALSE(mss::attr::set_swa_current_warning[l_id](i_ocmb_target, l_saved_swa_current_warning));
                    REQUIRE_FALSE(mss::attr::set_swb_current_warning[l_id](i_ocmb_target, l_saved_swb_current_warning));
                    REQUIRE_FALSE(mss::attr::set_swc_current_warning[l_id](i_ocmb_target, l_saved_swc_current_warning));
                    REQUIRE_FALSE(mss::attr::set_swd_current_warning[l_id](i_ocmb_target, l_saved_swd_current_warning));
                }
            }

            return 0;
        });
    }

    SECTION("Test status checking functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

            const auto& l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

            FAPI_INF("Check check_all_pmics correctness");
            {
                // So the PMIC simics model has all of these status registers as READ-ONLY (cant inject our own errors)
                // But we can check to make sure that we don't error out.
                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    l_r32_contents.setBit<FIELDS::R32_VR_ENABLE>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));
                }

                if (!l_pmics.empty())
                {
                    REQUIRE_FALSE(mss::pmic::status::check_all_pmics(i_ocmb_target));
                    // No-op to please the compiler (unused variable)
                    // Since we don't have a real error, l_pmic is just a dummy target that pmic_enable ignores
                }

                for (const auto& l_pmic : l_pmics)
                {
                    // Reset R32's
                    fapi2::buffer<uint8_t> l_r32_contents; // Empty buffer
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));
                }
            }

            FAPI_INF("Check mss::pmic::status::clear() correctness");
            {
                fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;
                uint8_t l_module_height = 0;

                // Save the dram module height for restoring after the test completion
                REQUIRE_FALSE(mss::attr::get_dram_module_height(i_ocmb_target, l_module_height));

                // Set the enable bit for all the PMICs
                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    l_r32_contents.setBit<FIELDS::R32_VR_ENABLE>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));
                }

                // Now let's iterate through and turn them off for 4U
                // Grab the targets as a struct, if they exist
                mss::pmic::target_info_redundancy l_target_info(i_ocmb_target, l_rc);
                REQUIRE_FALSE(mss::pmic::pmic_clear_vr_swa_swc_en(l_target_info));

                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

                    // Make sure bit is disabled
                    REQUIRE_FALSE(l_r32_contents.getBit<FIELDS::R32_VR_ENABLE>());
                }

                // Now let's iterate through and turn them off for 2U
                REQUIRE_FALSE(mss::attr::set_dram_module_height(i_ocmb_target, fapi2::ENUM_ATTR_MEM_EFF_DRAM_MODULE_HEIGHT_2U));
                REQUIRE_FALSE(mss::pmic::power_down_sequence_1u_2u(i_ocmb_target));

                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

                    // Make sure bit is disabled
                    REQUIRE_FALSE(l_r32_contents.getBit<FIELDS::R32_VR_ENABLE>());
                }

                // Restore dram module height
                REQUIRE_FALSE(mss::attr::set_dram_module_height(i_ocmb_target, l_module_height));
            }

            FAPI_INF("Check mss::pmic::pmic_clear_vr_swa_swc_en() correctness");
            {
                fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;

                // Set SWA_EN & SWC_EN. VR_ENABLE will not be tested here as it has been tested in other test case
                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r2f_contents;
                    l_r2f_contents.setBit<FIELDS::R2F_SWA_REGULATOR_CONTROL>();
                    l_r2f_contents.setBit<FIELDS::R2F_SWC_REGULATOR_CONTROL>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R2F, l_r2f_contents));
                }

                // Grab the targets as a struct, if they exist
                mss::pmic::target_info_redundancy l_target_info(i_ocmb_target, l_rc);
                REQUIRE_FALSE(mss::pmic::pmic_clear_vr_swa_swc_en(l_target_info));

                // Ckeck SWA and SWC bits for PMIC0 and PMIC2 only
                REQUIRE_RC_PASS(run_if_present(l_target_info.iv_pmic_map, mss::pmic::id::PMIC0, []
                                               (const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic) -> fapi2::ReturnCode
                {
                    fapi2::buffer<uint8_t> l_r2f_contents;

                    // Disable SWA and SWC of PMIC2
                    mss::pmic::i2c::reg_read_reverse_buffer(i_pmic, REGS::R2F, l_r2f_contents);

                    REQUIRE_FALSE(l_r2f_contents.getBit<FIELDS::R2F_SWA_REGULATOR_CONTROL>());
                    REQUIRE_FALSE(l_r2f_contents.getBit<FIELDS::R2F_SWC_REGULATOR_CONTROL>());

                    return fapi2::FAPI2_RC_SUCCESS;
                }));

                REQUIRE_RC_PASS(run_if_present(l_target_info.iv_pmic_map, mss::pmic::id::PMIC2, []
                                               (const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic) -> fapi2::ReturnCode
                {
                    fapi2::buffer<uint8_t> l_r2f_contents;

                    // Disable SWA and SWC of PMIC2
                    mss::pmic::i2c::reg_read_reverse_buffer(i_pmic, REGS::R2F, l_r2f_contents);

                    REQUIRE_FALSE(l_r2f_contents.getBit<FIELDS::R2F_SWA_REGULATOR_CONTROL>());
                    REQUIRE_FALSE(l_r2f_contents.getBit<FIELDS::R2F_SWC_REGULATOR_CONTROL>());

                    return fapi2::FAPI2_RC_SUCCESS;
                }));
            }

            return 0;
        });
    }

#endif
    SECTION("Test target ordering helper functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            FAPI_INF("Test find_targets_sorted_by_pos")
            {
                // We can't hardcode ATTR_FAPI_POS, so all we can really do is check that the resulting indices are in order
                auto l_pmics = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

                if (!l_pmics.empty())
                {
                    auto l_last_pmic = l_pmics[0];

                    for (uint8_t l_pmic_id = 1; l_pmic_id < l_pmics.size(); ++l_pmic_id)
                    {
                        REQUIRE(mss::fapi_pos(l_last_pmic) < mss::fapi_pos(l_pmics[l_pmic_id]));

                        // We should also expect REL_POS to agree with the ordrering,
                        // so let's check that too
                        REQUIRE(mss::index(l_last_pmic) < mss::index(l_pmics[l_pmic_id]));

                        // Set last PMIC to the most recent one checked
                        l_last_pmic = l_pmics[l_pmic_id];
                    }

                    // Now reverse them, and re-sort, make sure we're good
                    std::reverse(l_pmics.begin(), l_pmics.end());
                    mss::sort_targets_by_pos(l_pmics);

                    l_last_pmic = l_pmics[0];

                    for (uint8_t l_pmic_id = 1; l_pmic_id < l_pmics.size(); ++l_pmic_id)
                    {
                        REQUIRE(mss::fapi_pos(l_last_pmic) < mss::fapi_pos(l_pmics[l_pmic_id]));

                        // We should also expect REL_POS to agree with the ordrering,
                        // so let's check that too
                        REQUIRE(mss::index(l_last_pmic) < mss::index(l_pmics[l_pmic_id]));

                        // Set last PMIC to the most recent one checked
                        l_last_pmic = l_pmics[l_pmic_id];
                    }
                }
            }

            FAPI_INF("Check order_pmics_by_sequence correctness")
            {
                // Cronus does not give us 2 additional pmic targets in a dual drop configuration at this time,
                // as a result, our ddimm assumption of NUM_PMICS = NUM_DIMMS * 2 is not true, so we can't
                // appropriately test this algorithm. Down the road with appropriate SPD and i2c addresses
                // we expect that this will be fixed and therefore the below tests to work. For now, we will
                // take them out for DD
                if (mss::count_dimm(i_ocmb_target) == 1)
                {
                    auto l_pmics = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);
                    std::reverse(l_pmics.begin(), l_pmics.end());


                    uint8_t l_saved_pmic0_order = 0;
                    uint8_t l_saved_pmic1_order = 0;
                    REQUIRE_FALSE(mss::attr::get_pmic0_sequence(i_ocmb_target, l_saved_pmic0_order));
                    REQUIRE_FALSE(mss::attr::get_pmic1_sequence(i_ocmb_target, l_saved_pmic1_order));

                    // set to ascending order
                    REQUIRE_FALSE(mss::attr::set_pmic0_sequence(i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_pmic1_sequence(i_ocmb_target, 1));

                    REQUIRE_FALSE(mss::pmic::order_pmics_by_sequence(i_ocmb_target, l_pmics));


                    // Now let's check them in groups of 2
                    for (uint8_t l_pmic_offset = 0; l_pmic_offset < l_pmics.size() - 1; ++l_pmic_offset)
                    {
                        auto& l_first_pmic = l_pmics[l_pmic_offset];
                        auto& l_second_pmic = l_pmics[l_pmic_offset + 1];

                        // Each group of 2 should be in order of pos now
                        REQUIRE(mss::fapi_pos(l_first_pmic) < mss::fapi_pos(l_second_pmic));

                    }
                }
            }

            return 0;
        });
    }

#ifdef SUET_WORKING_NOW

    SECTION("Checks TI workarounds")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            auto l_pmics = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

            // Not asserting vectors non-empty because there could be OCMBs without DIMMs on them
            for (const auto& l_pmic : l_pmics)
            {
                // Get the PMIC ID (0 or 1)
                const mss::pmic::id l_id = static_cast<mss::pmic::id>(mss::index(l_pmic) % CONSTS::NUM_PRIMARY_PMICS);

                // Now we have the PMIC, it's ID, and the DIMM that we need for the helper functions
                FAPI_INF("Test voltage range workarounds for TI PMIC");
                {
                    // Save attributes to put back later
                    // We won't test the offset algorithm here, but we still need to make sure
                    // they are zero'd out for these tests
                    uint8_t l_saved_swa_volt = 0;
                    uint8_t l_saved_swb_volt = 0;
                    uint8_t l_saved_swc_volt = 0;
                    uint8_t l_saved_swd_volt = 0;

                    uint8_t l_saved_swa_range = 0;
                    uint8_t l_saved_swb_range = 0;
                    uint8_t l_saved_swc_range = 0;
                    uint8_t l_saved_swd_range = 0;

                    int8_t l_saved_swa_offset = 0;
                    int8_t l_saved_swb_offset = 0;
                    int8_t l_saved_swc_offset = 0;
                    int8_t l_saved_swd_offset = 0;

                    int8_t l_saved_efd_swa_offset = 0;
                    int8_t l_saved_efd_swb_offset = 0;
                    int8_t l_saved_efd_swc_offset = 0;
                    int8_t l_saved_efd_swd_offset = 0;

                    REQUIRE_FALSE(mss::attr::get_swa_voltage_setting[l_id](i_ocmb_target, l_saved_swa_volt));
                    REQUIRE_FALSE(mss::attr::get_swb_voltage_setting[l_id](i_ocmb_target, l_saved_swb_volt));
                    REQUIRE_FALSE(mss::attr::get_swc_voltage_setting[l_id](i_ocmb_target, l_saved_swc_volt));
                    REQUIRE_FALSE(mss::attr::get_swd_voltage_setting[l_id](i_ocmb_target, l_saved_swd_volt));

                    REQUIRE_FALSE(mss::attr::get_swa_voltage_range_select[l_id](i_ocmb_target, l_saved_swa_range));
                    REQUIRE_FALSE(mss::attr::get_swb_voltage_range_select[l_id](i_ocmb_target, l_saved_swb_range));
                    REQUIRE_FALSE(mss::attr::get_swc_voltage_range_select[l_id](i_ocmb_target, l_saved_swc_range));
                    REQUIRE_FALSE(mss::attr::get_swd_voltage_range_select[l_id](i_ocmb_target, l_saved_swd_range));

                    REQUIRE_FALSE(mss::attr::get_swa_voltage_offset[l_id](i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::get_swb_voltage_offset[l_id](i_ocmb_target, l_saved_swb_offset));
                    REQUIRE_FALSE(mss::attr::get_swc_voltage_offset[l_id](i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::get_swd_voltage_offset[l_id](i_ocmb_target, l_saved_swd_offset));

                    REQUIRE_FALSE(mss::attr::get_efd_swa_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swa_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swb_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swb_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swc_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swc_offset));
                    REQUIRE_FALSE(mss::attr::get_efd_swd_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swd_offset));

                    // Set 0's so they don't interfere with testing
                    REQUIRE_FALSE(mss::attr::set_swa_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_offset[l_id](i_ocmb_target, 0));

                    REQUIRE_FALSE(mss::attr::set_efd_swa_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_efd_swc_voltage_offset[l_id](i_ocmb_target, 0));
                    REQUIRE_FALSE(mss::attr::set_efd_swd_voltage_offset[l_id](i_ocmb_target, 0));

                    // TI workaround test for revision less than 0x23
                    {
                        // Let's put some sample voltages on SWA->SWD but set SWB and SWD to range 1. The algorithm
                        // should correct for this and re-align SWB to range 0
                        {
                            static constexpr uint8_t SAMPLE_VOLTAGE = 0b00111100;
                            constexpr auto RANGE_1 = CONSTS::RANGE_1;
                            constexpr auto RANGE_0 = CONSTS::RANGE_0;


                            REQUIRE_FALSE(mss::attr::set_swa_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swc_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swd_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));

                            REQUIRE_FALSE(mss::attr::set_swa_voltage_range_select[l_id](i_ocmb_target, RANGE_0));
                            REQUIRE_FALSE(mss::attr::set_swb_voltage_range_select[l_id](i_ocmb_target, RANGE_1));
                            REQUIRE_FALSE(mss::attr::set_swc_voltage_range_select[l_id](i_ocmb_target, RANGE_0));
                            REQUIRE_FALSE(mss::attr::set_swd_voltage_range_select[l_id](i_ocmb_target, RANGE_1));

                            for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
                            {
                                REQUIRE_FALSE(mss::pmic::bias_with_spd_voltages_TI_rev_less_then_23(l_pmic, i_ocmb_target, l_id, l_rail_index));
                            }

                            // The TI function does not re-set R2B (the range register) for SWA,B,C as those bits
                            // are reserved anyway. It should however set SWD (bit 0)

                            // Check SWD
                            fapi2::buffer<uint8_t> l_range_reg_contents;
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R2B, l_range_reg_contents));
                            REQUIRE(l_range_reg_contents.getBit<FIELDS::SWD_VOLTAGE_RANGE>() == RANGE_1);

                            // Check SWB
                            // Range 1 starts at 600mV. Range 0 starts at 800mV. So, we need to subtract
                            // the shift to convert from range 1 to range 0
                            static constexpr uint8_t SWB_EXPECTED = SAMPLE_VOLTAGE - CONSTS::CONVERT_RANGE1_TO_RANGE0;

                            fapi2::buffer<uint8_t> l_swb_voltage_contents;
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R23_SWB_VOLTAGE_SETTING, l_swb_voltage_contents));
                            REQUIRE((uint8_t(l_swb_voltage_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWB_EXPECTED);
                        }
                    }

                    // TI bias_with_spd_voltages test for revision >= 0x23
                    {
                        // Try a sample value/bitmap in range with sample offsets
                        {
                            static constexpr uint8_t SAMPLE_VOLTAGE = 0b00111100; // 1100mV or 900mV

                            static constexpr int8_t SAMPLE_SIGNED_OFFSET_1 = -3;
                            static constexpr int8_t SAMPLE_SIGNED_OFFSET_2 = -7;
                            static constexpr int8_t SAMPLE_SIGNED_OFFSET_3 = 0b101;
                            static constexpr int8_t SAMPLE_SIGNED_OFFSET_4 = 0b1100;

                            // Set some combinations
                            // SWA
                            REQUIRE_FALSE(mss::attr::set_swa_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swa_voltage_offset[l_id](i_ocmb_target, 0));
                            REQUIRE_FALSE(mss::attr::set_efd_swa_voltage_offset[l_id](i_ocmb_target, 0));
                            static constexpr uint8_t SWA_EXPECTED = SAMPLE_VOLTAGE;

                            // SWB
                            REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_2));
                            REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_3));
                            static constexpr uint8_t SWB_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_2 + SAMPLE_SIGNED_OFFSET_3;

                            // SWC
                            REQUIRE_FALSE(mss::attr::set_swc_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swc_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_1));
                            REQUIRE_FALSE(mss::attr::set_efd_swc_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_1));
                            static constexpr uint8_t SWC_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_1 + SAMPLE_SIGNED_OFFSET_1;

                            // SWD
                            REQUIRE_FALSE(mss::attr::set_swd_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swd_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_4));
                            REQUIRE_FALSE(mss::attr::set_efd_swd_voltage_offset[l_id](i_ocmb_target, SAMPLE_SIGNED_OFFSET_2));
                            static constexpr uint8_t SWD_EXPECTED = SAMPLE_VOLTAGE + SAMPLE_SIGNED_OFFSET_4 + SAMPLE_SIGNED_OFFSET_2;

                            // Run procedure. Should return with success
                            REQUIRE_FALSE(mss::pmic::bias_with_spd_voltages<mss::pmic::vendor::TI>(l_pmic, i_ocmb_target, l_id));

                            // Check the registers
                            fapi2::buffer<uint8_t> l_voltage_reg_contents;
                            // SWA
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R21_SWA_VOLTAGE_SETTING, l_voltage_reg_contents));
                            REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWA_EXPECTED);

                            // SWB
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R23_SWB_VOLTAGE_SETTING, l_voltage_reg_contents));
                            REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWB_EXPECTED);

                            // SWC
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R25_SWC_VOLTAGE_SETTING, l_voltage_reg_contents));
                            REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWC_EXPECTED);

                            // SWD
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R27_SWD_VOLTAGE_SETTING, l_voltage_reg_contents));
                            REQUIRE((uint8_t(l_voltage_reg_contents) >> CONSTS::SHIFT_VOLTAGE_FOR_REG) == SWD_EXPECTED);

                            // R2B
                            REQUIRE_FALSE(mss::pmic::i2c::reg_read(l_pmic, REGS::R2B, l_voltage_reg_contents));
                            REQUIRE(l_voltage_reg_contents.getBit<FIELDS::SWA_VOLTAGE_RANGE>());
                            REQUIRE_FALSE(l_voltage_reg_contents.getBit<FIELDS::SWB_VOLTAGE_RANGE>());
                            REQUIRE(l_voltage_reg_contents.getBit<FIELDS::SWC_VOLTAGE_RANGE>());
                            REQUIRE_FALSE(l_voltage_reg_contents.getBit<FIELDS::SWD_VOLTAGE_RANGE>());
                        }

                        // Now lets try something out of bounds on SWB (arbitrary)
                        {
                            static constexpr uint8_t SAMPLE_VOLTAGE = 0b01111110;
                            static constexpr uint8_t SAMPLE_OFFSET = 0b100; // will go out of bounds

                            REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, SAMPLE_VOLTAGE));
                            REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, SAMPLE_OFFSET));
                            REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, 0));

                            REQUIRE(uint64_t(mss::pmic::bias_with_spd_voltages<mss::pmic::vendor::TI>(l_pmic, i_ocmb_target, l_id))
                                    == uint64_t(fapi2::RC_PMIC_VOLTAGE_OUT_OF_RANGE));
                        }
                    }

                    // Put back attributes
                    REQUIRE_FALSE(mss::attr::set_swa_voltage_setting[l_id](i_ocmb_target, l_saved_swa_volt));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_setting[l_id](i_ocmb_target, l_saved_swb_volt));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_setting[l_id](i_ocmb_target, l_saved_swc_volt));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_setting[l_id](i_ocmb_target, l_saved_swd_volt));

                    REQUIRE_FALSE(mss::attr::set_swa_voltage_range_select[l_id](i_ocmb_target, l_saved_swa_range));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_range_select[l_id](i_ocmb_target, l_saved_swb_range));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_range_select[l_id](i_ocmb_target, l_saved_swc_range));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_range_select[l_id](i_ocmb_target, l_saved_swd_range));

                    REQUIRE_FALSE(mss::attr::set_swa_voltage_offset[l_id](i_ocmb_target, l_saved_swa_offset));
                    REQUIRE_FALSE(mss::attr::set_swb_voltage_offset[l_id](i_ocmb_target, l_saved_swb_offset));
                    REQUIRE_FALSE(mss::attr::set_swc_voltage_offset[l_id](i_ocmb_target, l_saved_swc_offset));
                    REQUIRE_FALSE(mss::attr::set_swd_voltage_offset[l_id](i_ocmb_target, l_saved_swd_offset));

                    REQUIRE_FALSE(mss::attr::set_efd_swa_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swa_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swb_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swb_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swc_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swc_offset));
                    REQUIRE_FALSE(mss::attr::set_efd_swd_voltage_offset[l_id](i_ocmb_target, l_saved_efd_swd_offset));
                }
            }

            return 0;
        });
    }

    SECTION("Check manual mode enable workarounds")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
            constexpr auto PROGRAMMABLE_MODE = CONSTS::PROGRAMMABLE_MODE;

            fapi2::buffer<uint8_t> l_reg_contents;
            const auto& l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

            for (const auto& l_pmic : l_pmics)
            {
                // Check that kicking off VR Enable sets the expected bits
                REQUIRE_FALSE(mss::pmic::start_vr_enable(l_pmic));

                // Check for programmable mode
                REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R2F, l_reg_contents));
                REQUIRE(l_reg_contents.getBit<FIELDS::R2F_SECURE_MODE>() == PROGRAMMABLE_MODE);

                l_reg_contents.flush<0>();

                // Check that VR Enable was executed
                REQUIRE_FALSE(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R32, l_reg_contents));
                REQUIRE(l_reg_contents.getBit<FIELDS::R32_VR_ENABLE>());
            }

            return 0;
        });
    }
#endif

    SECTION("Test revision/vendor checks")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
            {
                FAPI_INF("Test matching vendors");
                {
                    // IDT value
                    uint16_t l_vendor_attr = 0xB380;
                    uint8_t l_vendor_reg_lo = 0x80;
                    uint8_t l_vendor_reg_hi = 0xB3;
                    REQUIRE_RC_PASS(mss::pmic::check::matching_vendors_helper(l_pmic, l_vendor_attr, l_vendor_reg_lo, l_vendor_reg_hi));

                    l_vendor_reg_hi = 0xBE;
                    REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check::matching_vendors_helper(l_pmic, l_vendor_attr, l_vendor_reg_lo,
                                             l_vendor_reg_hi),
                                             fapi2::RC_PMIC_MISMATCHING_VENDOR_IDS);

                    l_vendor_attr = 0xBE80;
                    l_vendor_reg_hi = 0xB3;
                    REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check::matching_vendors_helper(l_pmic, l_vendor_attr, l_vendor_reg_lo,
                                             l_vendor_reg_hi),
                                             fapi2::RC_PMIC_MISMATCHING_VENDOR_IDS);
                }

                FAPI_INF("Test mismatching revisions");
                {
                    // IDT value
                    uint16_t l_rev_attr = 0x21;
                    uint16_t l_rev_reg = 0x21;
                    REQUIRE_RC_PASS(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg));

                    l_rev_attr = 0x31;
                    l_rev_reg = 0x31;
                    REQUIRE_RC_PASS(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg));

                    l_rev_reg = 0x11;
                    REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg),
                                             fapi2::RC_PMIC_MISMATCHING_REVISIONS);

                    l_rev_attr = 0x11;
                    l_rev_reg = 0x21;
                    REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg),
                                             fapi2::RC_PMIC_MISMATCHING_REVISIONS);

                    // Neither are IDT C1's, so we shouldn't do any asserting out...
                    l_rev_attr = 0x11;
                    l_rev_reg = 0x11;
                    REQUIRE_RC_PASS(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg));

                    // ... even if they are different values
                    l_rev_attr = 0x11;
                    l_rev_reg = 0x0B;
                    REQUIRE_RC_PASS(mss::pmic::check::validate_and_return_idt_revisions_helper(l_pmic, l_rev_attr, l_rev_reg));
                }
            }

            return 0;
        });
    } // section

    SECTION("Test 4U / Redundancy Functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            FAPI_INF("Test run_if_present behavior");
            {
                uint8_t l_force_n_mode_saved = 0;
                uint8_t l_force_n_mode_override = 0b10100000;
                uint8_t l_test_variable = 0xFF;
                REQUIRE_RC_PASS(mss::attr::get_pmic_force_n_mode(i_ocmb_target, l_force_n_mode_saved));

                // Override, now test
                REQUIRE_RC_PASS(mss::attr::set_pmic_force_n_mode(i_ocmb_target, l_force_n_mode_override));

                const auto l_pmics = mss::find_targets_sorted_by_pos<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

                if (l_pmics.size() >= 2)
                {
                    // Declare map object in the same way as the target_info_redundancy struct
                    std::map<size_t, fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmic_map =
                    {
                        {mss::pmic::id::PMIC0, l_pmics[0]},
                        {mss::pmic::id::PMIC1, l_pmics[1]},
                    };

                    // Test the one we expect to be enabled
                    REQUIRE_RC_PASS(mss::pmic::run_if_present(l_pmic_map, mss::pmic::id::PMIC0,
                                    [&l_test_variable]
                                    (const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic)
                    {
                        l_test_variable = 0xCC;
                        return fapi2::FAPI2_RC_SUCCESS;
                    }));

                    // Expecting changed
                    REQUIRE(l_test_variable == 0xCC);

                    l_test_variable = 0xFF;

                    // Test the one we expect to be enabled
                    REQUIRE_RC_PASS(mss::pmic::run_if_present(l_pmic_map, mss::pmic::id::PMIC1,
                                    [&l_test_variable]
                                    (const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic)
                    {
                        l_test_variable = 0xCC;
                        return fapi2::FAPI2_RC_SUCCESS;
                    }));

                    // Expecting unchanged
                    REQUIRE(l_test_variable == 0xFF);

                    // Test one that doesn't exist (make sure we dont null pointer out)
                    REQUIRE_RC_PASS(mss::pmic::run_if_present(l_pmic_map, mss::pmic::id::PMIC2,
                                    [&l_test_variable]
                                    (const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic)
                    {
                        l_test_variable = 0xDD;
                        return fapi2::FAPI2_RC_SUCCESS;
                    }));

                    // Expecting unchanged
                    REQUIRE(l_test_variable == 0xFF);
                }

                // Put back attr
                REQUIRE_RC_PASS(mss::attr::set_pmic_force_n_mode(i_ocmb_target, l_force_n_mode_saved));

            }
#ifdef SUET_WORKING_NOW

            FAPI_INF("Test set_soft_stop_time and set_soft_start_time");
            {
                using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
                using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
                using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

                constexpr uint8_t SOFT_STOP_TIME = FIELDS::SOFT_STOP_TIME;
                constexpr uint8_t R2C_R2D_4MS_ALL = CONSTS::R2C_R2D_4MS_ALL;

                const auto l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_saved_r22;
                    fapi2::buffer<uint8_t> l_saved_r24;
                    fapi2::buffer<uint8_t> l_saved_r26;
                    fapi2::buffer<uint8_t> l_saved_r28;
                    fapi2::buffer<uint8_t> l_saved_r2c;
                    fapi2::buffer<uint8_t> l_saved_r2d;

                    fapi2::buffer<uint8_t> l_r22;
                    fapi2::buffer<uint8_t> l_r24;
                    fapi2::buffer<uint8_t> l_r26;
                    fapi2::buffer<uint8_t> l_r28;
                    fapi2::buffer<uint8_t> l_r2c;
                    fapi2::buffer<uint8_t> l_r2d;

                    // Save off regs
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R22, l_saved_r22));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R24, l_saved_r24));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R26, l_saved_r26));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R28, l_saved_r28));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R2C, l_saved_r2c));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R2D, l_saved_r2d));

                    // Run function
                    REQUIRE_RC_PASS(mss::pmic::set_soft_stop_time(l_pmic));
                    REQUIRE_RC_PASS(mss::pmic::set_soft_start_time(l_pmic));

                    // Now read back out the regs
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R22, l_r22));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R24, l_r24));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R26, l_r26));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R28, l_r28));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R2C, l_r2c));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_read(l_pmic, REGS::R2D, l_r2d));

                    // Check them here, they are 2 bit fields
                    REQUIRE(l_r22.getBit<SOFT_STOP_TIME>());
                    REQUIRE(l_r22.getBit < SOFT_STOP_TIME + 1 > ());

                    REQUIRE(l_r24.getBit<SOFT_STOP_TIME>());
                    REQUIRE(l_r24.getBit < SOFT_STOP_TIME + 1 > ());

                    REQUIRE(l_r26.getBit<SOFT_STOP_TIME>());
                    REQUIRE(l_r26.getBit < SOFT_STOP_TIME + 1 > ());

                    REQUIRE(l_r28.getBit<SOFT_STOP_TIME>());
                    REQUIRE(l_r28.getBit < SOFT_STOP_TIME + 1 > ());

                    // Check start times
                    REQUIRE(l_r2d() == R2C_R2D_4MS_ALL);
                    REQUIRE(l_r2c() == R2C_R2D_4MS_ALL);

                    // Put back regs
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R22, l_saved_r22));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R24, l_saved_r24));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R26, l_saved_r26));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R28, l_saved_r28));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R2C, l_saved_r2c));
                    REQUIRE_RC_PASS(mss::pmic::i2c::reg_write(l_pmic, REGS::R2D, l_saved_r2d));
                }
            }
#endif
            FAPI_INF("Test n_mode attribute helper functions");
            {
                // Test two pmics 1,2 (arbitrary choice)
                uint8_t l_n_mode_attr = 0b01100000;
                mss::pmic::n_mode l_n_mode = mss::pmic::n_mode::N_PLUS_1_MODE;

                // Override
                REQUIRE_RC_PASS(mss::attr::set_pmic_n_mode(i_ocmb_target, l_n_mode_attr));

                // Check each one
                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC0, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_PLUS_1_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC1, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC2, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC3, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_PLUS_1_MODE);

                // Now use set_n_mode_helper to go from 0b0110 -> 0b1001
                REQUIRE_RC_PASS(mss::attr::set_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC0, mss::pmic::n_mode::N_MODE));
                REQUIRE_RC_PASS(mss::attr::set_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC1, mss::pmic::n_mode::N_PLUS_1_MODE));
                REQUIRE_RC_PASS(mss::attr::set_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC2, mss::pmic::n_mode::N_PLUS_1_MODE));
                REQUIRE_RC_PASS(mss::attr::set_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC3, mss::pmic::n_mode::N_MODE));

                // Verify correctness
                REQUIRE_RC_PASS(mss::attr::get_pmic_n_mode(i_ocmb_target, l_n_mode_attr));
                REQUIRE(l_n_mode_attr == 0b10010000); // PMIC0 and PMIC3

                // Check each one again for sanity
                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC0, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC1, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_PLUS_1_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC2, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_PLUS_1_MODE);

                REQUIRE_RC_PASS(mss::attr::get_n_mode_helper(i_ocmb_target, mss::pmic::id::PMIC3, l_n_mode));
                REQUIRE(l_n_mode == mss::pmic::n_mode::N_MODE);

                // Set back to all off state
                REQUIRE_RC_PASS(mss::attr::set_pmic_n_mode(i_ocmb_target, 0x00));
            }

            FAPI_INF("Test 4U settings from file helper function");
            {
                uint8_t l_nominal = 0;
                // We can't test the "iterate and set all the regs" function since
                // simics doesn't model the registers we need in the R90-R9F region

                // But, we can test the logic of calculate_4u_nominal_voltage

                for (const auto& l_pmic : mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target))
                {
                    if (mss::index(l_pmic) == mss::pmic::id::PMIC0 || mss::index(l_pmic) == mss::pmic::id::PMIC2)
                    {
                        // Test case with no offsets
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWA, l_nominal));
                        REQUIRE(l_nominal == PMIC0_NOMINALS[mss::pmic::rail::SWA]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWB, l_nominal));
                        REQUIRE(l_nominal == PMIC0_NOMINALS[mss::pmic::rail::SWB]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWC, l_nominal));
                        REQUIRE(l_nominal == PMIC0_NOMINALS[mss::pmic::rail::SWC]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWD, l_nominal));
                        REQUIRE(l_nominal == PMIC0_NOMINALS[mss::pmic::rail::SWD]);

                        // Now test with some offsets
                        {
                            int8_t l_swa_offset_saved = 0;
                            int8_t l_swa_offset_efd_saved = 0;

                            int8_t l_swa_offset = 0;
                            int8_t l_swa_offset_efd = 0;

                            uint8_t l_expected = 0;

                            REQUIRE_RC_PASS(mss::attr::get_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_saved));
                            REQUIRE_RC_PASS(mss::attr::get_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd_saved));

                            l_swa_offset = 0b00000011;
                            l_swa_offset_efd = 0b00000100;
                            REQUIRE_RC_PASS(mss::attr::set_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd));

                            // Make sure we match the nominal combined with the offsets (shifted over a bit for the register mapping)
                            l_expected = PMIC0_NOMINALS[mss::pmic::rail::SWA] + (l_swa_offset << 1) + (l_swa_offset_efd << 1);

                            REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWA, l_nominal));
                            REQUIRE(l_nominal == l_expected);

                            l_swa_offset = 0b01111111;
                            l_swa_offset_efd = 0b011111111;
                            REQUIRE_RC_PASS(mss::attr::set_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd));

                            // These will take us out of range
                            REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::calculate_4u_nominal_voltage(
                                                         l_pmic, mss::index(l_pmic),
                                                         mss::pmic::rail::SWA,
                                                         l_nominal),
                                                     fapi2::RC_PMIC_VOLTAGE_OUT_OF_RANGE);

                            // Put back attrs
                            REQUIRE_RC_PASS(mss::attr::set_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_saved));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic0_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd_saved));
                        }
                    }
                    else if (mss::index(l_pmic) == mss::pmic::id::PMIC1 || mss::index(l_pmic) == mss::pmic::id::PMIC3)
                    {
                        // Test case with no offsets
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWA, l_nominal));
                        REQUIRE(l_nominal == PMIC1_NOMINALS[mss::pmic::rail::SWA]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWB, l_nominal));
                        REQUIRE(l_nominal == PMIC1_NOMINALS[mss::pmic::rail::SWB]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWC, l_nominal));
                        REQUIRE(l_nominal == PMIC1_NOMINALS[mss::pmic::rail::SWC]);
                        REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWD, l_nominal));
                        REQUIRE(l_nominal == PMIC1_NOMINALS[mss::pmic::rail::SWD]);

                        // Now test with some offsets
                        {
                            int8_t l_swa_offset_saved = 0;
                            int8_t l_swa_offset_efd_saved = 0;

                            int8_t l_swa_offset = 0;
                            int8_t l_swa_offset_efd = 0;

                            uint8_t l_expected = 0;

                            REQUIRE_RC_PASS(mss::attr::get_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_saved));
                            REQUIRE_RC_PASS(mss::attr::get_efd_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd_saved));

                            l_swa_offset = 0b00000011;
                            l_swa_offset_efd = 0b00000100;
                            REQUIRE_RC_PASS(mss::attr::set_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd));

                            // Make sure we match the nominal combined with the offsets (shifted over a bit for the register mapping)
                            l_expected = PMIC1_NOMINALS[mss::pmic::rail::SWA] + (l_swa_offset << 1) + (l_swa_offset_efd << 1);

                            REQUIRE_RC_PASS(mss::pmic::calculate_4u_nominal_voltage(l_pmic, mss::index(l_pmic), mss::pmic::rail::SWA, l_nominal));
                            REQUIRE(l_nominal == l_expected);

                            l_swa_offset = 0b01111111;
                            l_swa_offset_efd = 0b011111111;
                            REQUIRE_RC_PASS(mss::attr::set_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd));

                            // These will take us out of range
                            REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::calculate_4u_nominal_voltage(
                                                         l_pmic, mss::index(l_pmic),
                                                         mss::pmic::rail::SWA,
                                                         l_nominal),
                                                     fapi2::RC_PMIC_VOLTAGE_OUT_OF_RANGE);

                            // Put back attrs
                            REQUIRE_RC_PASS(mss::attr::set_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_saved));
                            REQUIRE_RC_PASS(mss::attr::set_efd_pmic1_swa_voltage_offset(i_ocmb_target, l_swa_offset_efd_saved));
                        }
                    }

                    // Unless we have some fancy new simics model with all 4 pmics, we will just
                    // test PMIC0 and PMIC1. PMIC2 and PMIC3 mirror 0 and 1 anyway for settings
                }
            }

            FAPI_INF("Test assert_n_mode_states");
            {
                using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

                std::array<mss::pmic::n_mode, CONSTS::NUM_PMICS_4U> l_n_mode_pmic =
                {
                    mss::pmic::n_mode::N_PLUS_1_MODE,
                    mss::pmic::n_mode::N_PLUS_1_MODE,
                    mss::pmic::n_mode::N_PLUS_1_MODE,
                    mss::pmic::n_mode::N_PLUS_1_MODE
                };

                bool l_mnfg_thresholds = false;

                fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;
                // Don't check the RC here, since SUET doesn't return any GI2C targets
                mss::pmic::target_info_redundancy l_target_info(i_ocmb_target, l_rc);

                // redundant 4U, all N_PLUS_1_MODE, should return success
                l_target_info.iv_pmic_redundancy[0] = true;
                l_target_info.iv_pmic_redundancy[1] = true;
                REQUIRE_RC_PASS(mss::pmic::assert_n_mode_states(l_target_info,
                                l_n_mode_pmic,
                                l_mnfg_thresholds));

                // one N_MODE, should return DIMM_RUNNING_IN_N_MODE
                l_n_mode_pmic[2] = mss::pmic::n_mode::N_MODE;
                REQUIRE_SPECIFIC_RC_FAIL(fapi2::RC_DIMM_RUNNING_IN_N_MODE,
                                         mss::pmic::assert_n_mode_states(l_target_info,
                                                 l_n_mode_pmic,
                                                 l_mnfg_thresholds));

                // set another not in the pair with [2], should still return DIMM_RUNNING_IN_N_MODE
                l_n_mode_pmic[3] = mss::pmic::n_mode::N_MODE;
                REQUIRE_SPECIFIC_RC_FAIL(fapi2::RC_DIMM_RUNNING_IN_N_MODE,
                                         mss::pmic::assert_n_mode_states(l_target_info,
                                                 l_n_mode_pmic,
                                                 l_mnfg_thresholds));

                // set [0] which is in the pair with [2], should return PMIC_REDUNDANCY_FAIL
                l_n_mode_pmic[3] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[0] = mss::pmic::n_mode::N_MODE;
                REQUIRE_SPECIFIC_RC_FAIL(fapi2::RC_PMIC_REDUNDANCY_FAIL,
                                         mss::pmic::assert_n_mode_states(l_target_info,
                                                 l_n_mode_pmic,
                                                 l_mnfg_thresholds));

                // now test non-redundant 4U, all N_PLUS_1_MODE, should return success
                l_target_info.iv_pmic_redundancy[0] = false;
                l_target_info.iv_pmic_redundancy[1] = false;
                l_n_mode_pmic[0] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[1] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[2] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[3] = mss::pmic::n_mode::N_PLUS_1_MODE;
                REQUIRE_RC_PASS(mss::pmic::assert_n_mode_states(l_target_info,
                                l_n_mode_pmic,
                                l_mnfg_thresholds));

                // set a PMIC to N_MODE, should return PMIC_NON_REDUNDANT_FAIL
                l_n_mode_pmic[0] = mss::pmic::n_mode::N_MODE;
                REQUIRE_SPECIFIC_RC_FAIL(fapi2::RC_PMIC_NON_REDUNDANT_FAIL,
                                         mss::pmic::assert_n_mode_states(l_target_info,
                                                 l_n_mode_pmic,
                                                 l_mnfg_thresholds));

                // set the other primary PMIC to N_MODE, should return PMIC_NON_REDUNDANT_FAIL
                l_n_mode_pmic[0] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[1] = mss::pmic::n_mode::N_MODE;
                REQUIRE_SPECIFIC_RC_FAIL(fapi2::RC_PMIC_NON_REDUNDANT_FAIL,
                                         mss::pmic::assert_n_mode_states(l_target_info,
                                                 l_n_mode_pmic,
                                                 l_mnfg_thresholds));

                // set a non-existent PMIC to N_MODE, should return SUCCESS
                l_n_mode_pmic[1] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[2] = mss::pmic::n_mode::N_MODE;
                REQUIRE_RC_PASS(mss::pmic::assert_n_mode_states(l_target_info,
                                l_n_mode_pmic,
                                l_mnfg_thresholds));

                // set the other non-existent PMIC to N_MODE, should return SUCCESS
                l_n_mode_pmic[2] = mss::pmic::n_mode::N_PLUS_1_MODE;
                l_n_mode_pmic[3] = mss::pmic::n_mode::N_MODE;
                REQUIRE_RC_PASS(mss::pmic::assert_n_mode_states(l_target_info,
                                l_n_mode_pmic,
                                l_mnfg_thresholds));

            }
            return 0;
        });

        FAPI_INF("Test efuses_already_enabled_helper");
        {
            uint8_t l_polarity = mss::gpio::fields::EFUSE_POLARITY_SETTING;
            uint8_t l_output = mss::gpio::fields::EFUSE_OUTPUT_ON;
            uint8_t l_config = mss::gpio::fields::CONFIGURATION_IO_MAP;

            // In this case, we should see yes, that the efuses are enabled as the register
            // settings match the expected
            REQUIRE(mss::pmic::check::efuses_already_enabled_helper(l_polarity, l_output, l_config));

            // As soon as any of these change, it should fail. Using arbitrary values which
            // are different from the above constants
            l_config = 0x00;
            REQUIRE_FALSE(mss::pmic::check::efuses_already_enabled_helper(l_polarity, l_output, l_config));
            l_config = mss::gpio::fields::CONFIGURATION_IO_MAP;

            l_output = 0x00;
            REQUIRE_FALSE(mss::pmic::check::efuses_already_enabled_helper(l_polarity, l_output, l_config));
            l_output = mss::gpio::fields::EFUSE_OUTPUT_ON;

            l_polarity = 0xF0;
            REQUIRE_FALSE(mss::pmic::check::efuses_already_enabled_helper(l_polarity, l_output, l_config));
            l_polarity = mss::gpio::fields::EFUSE_POLARITY_SETTING;

            // Sanity check for total garbage
            l_config = 0x00;
            l_output = 0x00;
            l_polarity = 0xF0;
            REQUIRE_FALSE(mss::pmic::check::efuses_already_enabled_helper(l_polarity, l_output, l_config));
        }

        FAPI_INF("Test N-mode bool functions");
        {
            using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

            std::array<mss::pmic::n_mode, CONSTS::NUM_PMICS_4U> l_n_mode_pmic =
            {
                mss::pmic::n_mode::N_PLUS_1_MODE,
                mss::pmic::n_mode::N_PLUS_1_MODE,
                mss::pmic::n_mode::N_PLUS_1_MODE,
                mss::pmic::n_mode::N_PLUS_1_MODE
            };

            // All are good, so both should return false
            REQUIRE_FALSE(mss::pmic::check::bad_pair(l_n_mode_pmic));
            REQUIRE_FALSE(mss::pmic::check::bad_any(l_n_mode_pmic));
            REQUIRE_FALSE(mss::pmic::check::bad_primary(l_n_mode_pmic));

            // Set one to bad, arbitrary choice
            l_n_mode_pmic[2] = mss::pmic::n_mode::N_MODE;
            REQUIRE_FALSE(mss::pmic::check::bad_pair(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_any(l_n_mode_pmic));
            REQUIRE_FALSE(mss::pmic::check::bad_primary(l_n_mode_pmic));

            // Set another, not in the pair with [2]. Should see same results
            l_n_mode_pmic[3] = mss::pmic::n_mode::N_MODE;
            REQUIRE_FALSE(mss::pmic::check::bad_pair(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_any(l_n_mode_pmic));
            REQUIRE_FALSE(mss::pmic::check::bad_primary(l_n_mode_pmic));

            // Now knock out 0, killing a pair [0,2]
            l_n_mode_pmic[0] = mss::pmic::n_mode::N_MODE;
            REQUIRE(mss::pmic::check::bad_pair(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_any(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_primary(l_n_mode_pmic));

            // Just for fun, kill the last one too
            l_n_mode_pmic[1] = mss::pmic::n_mode::N_MODE;
            REQUIRE(mss::pmic::check::bad_pair(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_any(l_n_mode_pmic));
            REQUIRE(mss::pmic::check::bad_primary(l_n_mode_pmic));
        }

    }

    SECTION("Test IDT enable pins functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
            using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

            const auto& l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);

            FAPI_INF("Check mss::pmic::set_vr_enable_clear_camp_pwr_good_pins() correctness");
            {
                fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;

                for (const auto& l_pmic : l_pmics)
                {
                    fapi2::buffer<uint8_t> l_r32_contents;
                    l_r32_contents.clearBit<FIELDS::R32_EXECUTE_VR_ENABLE_CONTROL>();
                    l_r32_contents.setBit<FIELDS::R32_EXECUTE_CAMP_FAIL_N_FUNCTION_CONTROL>();
                    l_r32_contents.setBit<FIELDS::R32_CAMP_PWR_GOOD_OUTPUT_SIGNAL_CONTROL>();
                    REQUIRE_FALSE(mss::pmic::i2c::reg_write_reverse_buffer(l_pmic, REGS::R32, l_r32_contents));

                    REQUIRE_FALSE(mss::pmic::set_vr_enable_clear_camp_pwr_good_pins(l_pmic));

                    mss::pmic::i2c::reg_read_reverse_buffer(l_pmic, REGS::R32, l_r32_contents);

                    REQUIRE(l_r32_contents.getBit<FIELDS::R32_EXECUTE_VR_ENABLE_CONTROL>());
                    REQUIRE_FALSE(l_r32_contents.getBit<FIELDS::R32_EXECUTE_CAMP_FAIL_N_FUNCTION_CONTROL>());
                    REQUIRE_FALSE(l_r32_contents.getBit<FIELDS::R32_CAMP_PWR_GOOD_OUTPUT_SIGNAL_CONTROL>());
                }
            }

            return 0;
        });
    }

    SECTION("Checks VDDR1 rail workarounds functions")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            // Checks if the attributes mean that we will have to do the workaround or not
            {
                uint8_t l_swa_seq_orig = 0;
                REQUIRE_FALSE(mss::attr::get_pmic0_swa_sequence_order(i_ocmb_target, l_swa_seq_orig));

                // If this is set to a 0, then we should require the workaround
                bool l_value = false;
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_sequence_order(i_ocmb_target, 0));
                REQUIRE_FALSE(mss::pmic::workarounds::check_for_no_vddr1_rail(i_ocmb_target, l_value));
                REQUIRE(l_value == true);

                // non-zero value? no workaround (check a few just in case)
                l_value = true;
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_sequence_order(i_ocmb_target, 1));
                REQUIRE_FALSE(mss::pmic::workarounds::check_for_no_vddr1_rail(i_ocmb_target, l_value));
                REQUIRE(l_value == false);
                l_value = true;
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_sequence_order(i_ocmb_target, 2));
                REQUIRE_FALSE(mss::pmic::workarounds::check_for_no_vddr1_rail(i_ocmb_target, l_value));
                REQUIRE(l_value == false);
                l_value = true;
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_sequence_order(i_ocmb_target, 3));
                REQUIRE_FALSE(mss::pmic::workarounds::check_for_no_vddr1_rail(i_ocmb_target, l_value));
                REQUIRE(l_value == false);

                // Reset the original value
                REQUIRE_FALSE(mss::attr::set_pmic0_swa_sequence_order(i_ocmb_target, l_swa_seq_orig));
            }

            // Checks if the data is overwritten correctly based upon workaround settings
            {
                // Non-affected address(es)? no change needed
                REQUIRE(0x00 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( false, 0x15, 0x00));
                REQUIRE(0x00 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( true, 0x15, 0x00));
                REQUIRE(0x01 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( false, 0x13, 0x01));
                REQUIRE(0x01 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( true, 0x13, 0x01));

                // Affected address -> change is based upon the register + if a VDDR1 rail is present
                REQUIRE(0x00 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( false, mss::adc::ALERT_CH_SEL, 0x00));
                REQUIRE(mss::adc::ALERT_CH_SEL_ADC1_NO_VDDR1_RAIL_LOCAL_VOLT_ALERT ==
                        mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( true, mss::adc::ALERT_CH_SEL, 0x00));
                REQUIRE(0x01 == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( false, mss::adc::AUTO_SEQ_CH_SEL, 0x01));
                REQUIRE(mss::adc::AUTO_SEQ_CH_SEL_NO_VDDR1_RAIL == mss::pmic::workarounds::override_adc1_regs_for_no_vddr1( true,
                        mss::adc::AUTO_SEQ_CH_SEL, 0x01));
            }

            return 0;
        });
    }
    SECTION("Checks check_number_pmics_received_2u function")
    {
        for_each_target([](const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target)
        {
            uint8_t l_pmics_size = 2;
            FAPI_INF("Test for 2 PMICs received");

            REQUIRE_RC_PASS(mss::pmic::check_number_pmics_received_2u(i_ocmb_target, l_pmics_size));

            l_pmics_size = 1;
            FAPI_INF("Test for < 2 PMICs received");
            REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check_number_pmics_received_2u(i_ocmb_target, l_pmics_size),
                                     fapi2::RC_INVALID_2U_PMIC_TARGET_CONFIG);

            l_pmics_size = 4;
            FAPI_INF("Test for > 2 PMICs received");
            REQUIRE_SPECIFIC_RC_FAIL(mss::pmic::check_number_pmics_received_2u(i_ocmb_target, l_pmics_size),
                                     fapi2::RC_INVALID_2U_PMIC_TARGET_CONFIG);

            return 0;
        });
    }
} // scenario method
} // test
} // mss
