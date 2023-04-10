/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_bias/pmic_bias_utils.C $ */
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
/// @file pmic_bias_utils.C
/// @brief Utils for biasing PMIC
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#include <fapi2.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <lib/i2c/i2c_pmic.H>
#include <lib/utils/pmic_consts.H>
#include <lib/utils/pmic_common_utils.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/mss_math.H>
#include <generic/memory/lib/utils/find.H>
#include <mss_generic_attribute_getters.H>
#include <mss_pmic_attribute_accessors_manual.H>
#include <lib/utils/pmic_enable_4u_settings.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace bias
{

///
/// @brief Set the voltage of a rail (post-rounding)
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to set
/// @param[in] i_target_voltage voltage to set to
/// @param[in] i_range_selection range (0 or 1) of the rail
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode set_new_rail_voltage(
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::pmic::rail i_rail,
    const uint32_t i_target_voltage,
    const uint8_t i_range_selection)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    MSS_LAB_ASSERT_RETURN(i_range_selection < CONSTS::NUM_RANGES,
                          "set_new_rail_voltage(): The voltage setting provided for PMIC %s on rail %u was out of range (Valid: 0,1). Given: %u",
                          mss::c_str(i_pmic_target), uint8_t(i_rail), i_range_selection);
    {
        // Make sure voltage falls within range
        const uint32_t MIN_VOLT = mss::pmic::VOLT_RANGE_MINS[i_rail][i_range_selection];
        const uint32_t MAX_VOLT = mss::pmic::VOLT_RANGE_MAXES[i_rail][i_range_selection];

        MSS_LAB_ASSERT_RETURN((i_target_voltage >= MIN_VOLT) && (i_target_voltage <= MAX_VOLT),
                              "set_new_rail_voltage(): After rounding the bias voltage, "
                              "the resulting voltage %lumV was out of range LOW: %lumV HIGH: %lumV for PMIC %s on rail %u",
                              i_target_voltage, MIN_VOLT, MAX_VOLT, mss::c_str(i_pmic_target), uint8_t(i_rail));
        {
            // Convert to bit mapping and write back
            const uint32_t l_offset_from_min = i_target_voltage - MIN_VOLT;
            const uint8_t l_voltage_bitmap = l_offset_from_min / CONSTS::VOLT_STEP;

            // Shift and write back
            fapi2::buffer<uint8_t> l_voltage_write_back;

            l_voltage_write_back.insertFromRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

            FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, mss::pmic::VOLT_SETTING_ACTIVE_REGS[i_rail], l_voltage_write_back),
                     "set_voltage_percent: Error writing 0x%02hhX of PMIC %s", mss::pmic::VOLT_SETTING_ACTIVE_REGS[i_rail],
                     mss::c_str(i_pmic_target));
        }
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Round a target rail voltage to the nearest step of 5mV to create the voltage bitmap
///
/// @param[in] i_target_voltage_unrounded unrounded voltage
/// @return float rounded voltage
///
uint32_t round_rail_target_voltage(const uint32_t i_target_voltage_unrounded)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    // Round to nearest unit by converting to a MV integer
    const uint32_t l_target_multiple = CONSTS::VOLT_STEP;

    const uint32_t l_target_voltage = mss::round_to_nearest_multiple(i_target_voltage_unrounded, l_target_multiple);

    // Inform the user. This will be especially useful for debugging
    mss::logf(mss::TRACE, "Voltage rounded to %lumV", l_target_voltage);

    return l_target_voltage;
}

///
/// @brief Checks if bias percentage is within the MAX_BIAS threshold
///
/// @param[in] i_percent percentage to check
/// @param[in] i_force force change (would force evaluation to true)
/// @return true if in range, false if not
///
bool bias_percent_within_threshold(const float i_percent, const bool i_force)
{
    return i_force || ((i_percent < mss::lab::pmic::bias::PERCENT_MAX_BIAS)
                       && (i_percent > (-1) * mss::lab::pmic::bias::PERCENT_MAX_BIAS));
}

///
/// @brief Get the nominal rail voltage of a JEDEC-compliant PMIC via attribute
///
/// @param[in] i_ocmb_target OCMB target
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to read from
/// @param[out] o_nominial_rail_voltage voltage calculated for rail
/// @param[out] o_range_selection range selection of that voltage
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error
/// @note not templated as the arguments may differ for other chips (if we ever use others)
///
fapi2::ReturnCode get_nominal_rail_voltage(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::pmic::rail i_rail,
    uint32_t& o_nominial_rail_voltage,
    uint8_t& o_range_selection)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    const mss::pmic::id l_relative_pmic_id = mss::pmic::get_relative_pmic_id(i_pmic_target);
    uint8_t l_voltage_setting = 0;
    uint8_t l_range_selection = 0;
    uint8_t l_module_height = 0;

    FAPI_TRY(mss::attr::get_dram_module_height(i_ocmb_target, l_module_height));

    // For the 4U case, we will use the register settings provided
    // from file for "nominal"
    if (l_module_height == fapi2::ENUM_ATTR_MEM_EFF_DRAM_MODULE_HEIGHT_4U)
    {
        FAPI_TRY(mss::pmic::calculate_4u_nominal_voltage(i_pmic_target, l_relative_pmic_id, i_rail, l_voltage_setting));

        // calculate_4u_nominal_voltage returns the voltage shifted over to the left 1 for
        // easy setting of the register. In this case, shift it back so we can do some math with it
        l_voltage_setting = l_voltage_setting >> CONSTS::SHIFT_VOLTAGE_FOR_REG;

        // Only SWD has a range selection for the 4U PMIC part, otherwise it will remain 0
        if (i_rail == mss::pmic::rail::SWD)
        {
            l_range_selection = (l_relative_pmic_id == mss::pmic::id::PMIC0) ? PMIC0_SWD_RANGE : PMIC1_SWD_RANGE;
        }
    }
    else
    {
        // 1U/2U case will use the attribute / SPD voltage values for nominal
        FAPI_TRY(mss::pmic::calculate_voltage_bitmap_from_attr(
                     i_pmic_target,
                     l_relative_pmic_id,
                     i_rail,
                     l_voltage_setting));

        FAPI_TRY(mss::attr::get_volt_range_select[i_rail][l_relative_pmic_id](i_ocmb_target,
                 l_range_selection));
    }

    // While it's technically possible that we could just have a attribute value of 0 ( == 800mV),
    // we don't currently use this for any rail in any SPD, and it's unlikely that we would ever do so.
    // So, if both the voltage setting and range are 0, it's safe to assume the attributes are not set
    // and we should not continue.
    MSS_LAB_ASSERT_RETURN(!(l_voltage_setting == 0 && l_range_selection == 0),
                          "Nominal PMIC voltages are not defined for rail being biased. This could mean the rail is "
                          "disabled, or eff_config was not run, meaning biasing by percentage is not supported.");

    // Get nominial voltage using: range_min + (step * setting)
    o_nominial_rail_voltage = mss::pmic::VOLT_RANGE_MINS[i_rail][l_range_selection] +
                              (CONSTS::VOLT_STEP * l_voltage_setting);

    FAPI_INF("%s Rail %s Nominal voltage: %lumV", mss::c_str(i_pmic_target), mss::pmic::PMIC_RAIL_NAMES[i_rail],
             o_nominial_rail_voltage);

    o_range_selection = l_range_selection;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Set the voltage percent on the specified rail of a JEDEC-compliant PMIC
///
/// @param[in] i_ocmb_target parent OCMB target
/// @param[in] i_pmic_target PMIC to bias
/// @param[in] i_rail rail to bias
/// @param[in] i_percent percentage change
/// @param[in] i_force override 10% change limit
/// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode set_voltage_percent(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::pmic::rail i_rail,
    const float i_percent,
    const bool i_force)
{
    static constexpr float PERCENT_CONVERSION = 100;

    // Make sure less than 10% if not overriding or within threshold
    MSS_LAB_ASSERT_RETURN(bias_percent_within_threshold(i_percent, i_force),
                          "set_voltage_percent(): Bias percentage %f%% provided to PMIC %s exceed the maximum of 10%%. Use -f to override",
                          i_percent, mss::c_str(i_pmic_target));
    {
        uint32_t l_nominial_rail_voltage = 0;
        uint8_t l_range_selection = 0;

        FAPI_TRY(get_nominal_rail_voltage(i_ocmb_target, i_pmic_target, i_rail, l_nominial_rail_voltage, l_range_selection),
                 "set_voltage_percent: Error getting nominial rail voltage for rail # %u of PMIC %s", i_rail, mss::c_str(i_pmic_target));

        {
            // Obtain target voltage by percent offset
            const uint32_t l_target_voltage_unrounded = l_nominial_rail_voltage * ((PERCENT_CONVERSION + i_percent) /
                    PERCENT_CONVERSION);

            const uint32_t l_target_voltage = round_rail_target_voltage(l_target_voltage_unrounded);

            FAPI_TRY(set_new_rail_voltage(i_pmic_target, i_rail, l_target_voltage, l_range_selection),
                     "set_voltage_percent(): Error setting voltage on PMIC %s", mss::c_str(i_pmic_target));
        }
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Set the voltage of a JEDEC_COMPLIANT pmic's rail
///
/// @param[in] i_ocmb_target parent OCMB target
/// @param[in] i_pmic_target PMIC to bias
/// @param[in] i_rail rail to bias
/// @param[in] i_value value to set to
/// @param[in] i_force override 10% change limit
/// @return fapi2::ReturnCode fapi2::FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode set_voltage_value(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::pmic::rail i_rail,
    const float i_value,
    const bool i_force)
{
    static constexpr float V_TO_MV_CONVERSION = 1000;
    static constexpr float PERCENT_CONVERSION = 100;

    const uint32_t l_value_mv = i_value * V_TO_MV_CONVERSION;

    uint32_t l_rail_nominal_voltage = 0;
    uint8_t l_range_selection = 0;

    // Get range selection and voltage
    FAPI_TRY(get_nominal_rail_voltage(i_ocmb_target, i_pmic_target, i_rail, l_rail_nominal_voltage, l_range_selection),
             "set_voltage_percent: Error getting nominial rail voltage for rail # %u of PMIC %s", i_rail, mss::c_str(i_pmic_target));
    {
        // Calculate percent change
        // Float to avoid integer division
        const float l_percent_change = (((static_cast<float>(l_value_mv) / l_rail_nominal_voltage) - 1)) * PERCENT_CONVERSION;

        // Make sure less than 10% if not overriding or within threshold
        MSS_LAB_ASSERT_RETURN(bias_percent_within_threshold(l_percent_change, i_force),
                              "set_voltage_value(): Bias percentage %f%% provided to PMIC %s exceed the maximum of 10%%. Use -f to override",
                              l_percent_change, mss::c_str(i_pmic_target));

        const uint32_t l_target_voltage = round_rail_target_voltage(l_value_mv);

        // Verify new valid voltage and write to PMIC
        FAPI_TRY(set_new_rail_voltage(i_pmic_target, i_rail, l_target_voltage, l_range_selection),
                 "set_voltage_value(): Error setting voltage on PMIC %s", mss::c_str(i_pmic_target));
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // namespace bias

///
/// @brief Helper function for bias procedure
///
/// @param[in] i_ocmb_target - the parent OCMB target
/// @param[in] i_pmic_target - the pmic_target
/// @param[in] i_setting setting to change (swa_volt, swb_volt, etc.)
/// @param[in] i_amount amount to change by
/// @param[in] i_unit percentage or value
/// @param[in] i_force ignore 10% change limit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode bias_helper(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::lab::pmic::bias::setting i_setting,
    const float i_amount,
    const mss::lab::pmic::bias::unit i_unit,
    const bool i_force)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    if (i_unit == mss::lab::pmic::bias::unit::PERCENT)
    {
        FAPI_TRY(bias::set_voltage_percent(
                     i_ocmb_target,
                     i_pmic_target,
                     static_cast<mss::pmic::rail>(i_setting % CONSTS::NUMBER_OF_RAILS),
                     i_amount, i_force));
    }
    else // value
    {
        FAPI_TRY(bias::set_voltage_value(
                     i_ocmb_target,
                     i_pmic_target,
                     static_cast<mss::pmic::rail>(i_setting % CONSTS::NUMBER_OF_RAILS),
                     i_amount, i_force));
    }

fapi_try_exit:
    return fapi2::current_err;
}


///
/// @brief Bias procedure for PMIC devices
///
/// @param[in] i_ocmb_target explorer target
/// @param[in] i_id the PMIC to change (PMIC0,PMIC1,ALL)
/// @param[in] i_setting setting to change (swa_volt, swb_volt, etc.)
/// @param[in] i_amount amount to change by
/// @param[in] i_unit percentage or value
/// @param[in] i_force ignore 10% change limit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode bias_chip(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const mss::lab::pmic::id i_id,
    const mss::lab::pmic::bias::setting i_setting,
    const float i_amount,
    const mss::lab::pmic::bias::unit i_unit,
    const bool i_force)
{
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmics;

    if (i_id == mss::lab::pmic::id::ALL)
    {
        // All of 'em
        l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target);
    }
    else
    {
        // Either PMIC0 or PMIC1, can cast to mss::pmic::id and use the common_lab helper
        l_pmics = mss::pmic::get_valid_pmics_for_id(i_ocmb_target, static_cast<mss::pmic::id>(i_id));
    }

    for (const auto& l_pmic : l_pmics)
    {
        // Check to make sure VIN_BULK measures above minimum voltage tolerance, ensuring the PMIC
        // will function as expected
        FAPI_TRY(mss::pmic::check_vin_bulk_good(l_pmic),
                 "%s pmic_enable: check for VIN_BULK good either failed, or was below minimum voltage tolerance",
                 mss::c_str(l_pmic));

        mss::logf(mss::TRACE, "Performing BIAS on PMIC %s", mss::c_str(l_pmic));

        // Now call the helper function
        FAPI_TRY(bias_helper(i_ocmb_target, l_pmic, i_setting, i_amount, i_unit, i_force),
                 "Error biasing PMIC %s", mss::c_str(l_pmic));
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // pmic
} // lab
} // mss
