/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_update/pmic_update_utils.C $ */
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
/// @file pmic_update_utils.C
/// @brief Update util function and helper functions
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>
#include <lib/i2c/i2c_pmic.H>
#include <lib/utils/pmic_common_utils.H>
#include <pmic_update/pmic_update_utils.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/lib/utils/poll.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace update
{

///
/// @brief Poll for completion of vendor region burn
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS when burn successful, or FAPI2_RC_FALSE if unsuccessful
///
fapi2::ReturnCode poll_for_burn_completion(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    // Using default poll parameters
    mss::poll_parameters l_poll_params;

    MSS_LAB_ASSERT_RETURN( mss::poll(i_pmic_target, l_poll_params, [&i_pmic_target]()->bool
    {
        fapi2::buffer<uint8_t> l_command_code_reg_contents;

        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R39_COMMAND_CODES, l_command_code_reg_contents),
        "poll_for_burn_completion: Error reading 0x%02x on %s ", REGS::R39_COMMAND_CODES, mss::c_str(i_pmic_target));

        return l_command_code_reg_contents == CONSTS::BURN_COMPLETE;

    fapi_try_exit:
        // No ack from device, return false and continue polling
        return false;
    }), "Polling timed out waiting for PMIC %s vendor region burn to complete",
    mss::c_str(i_pmic_target));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Checks that the range modification of SWA,SWB,SWC is not ocurring on an invalid pmic
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_ti boolean true if TI pmic
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS unless invalid TI range change, in which case FAPI2_RC_FALSE
///
fapi2::ReturnCode ti_pmic_range_workaround(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::update::setting i_setting)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;

    // TI does not have ranges for SWA,SWB,SWC. Make sure we aren't trying to set those on a TI
    // Let's read the registers here instead of the attributes. Since we are at runtime,
    // attributes could have been overwritten
    fapi2::buffer<uint8_t> l_vendor_byte_1;
    bool l_invalid_range = false;
    FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R3D_VENDOR_ID_BYTE_1, l_vendor_byte_1),
             "Error reading address %02x of PMIC %s", REGS::R3D_VENDOR_ID_BYTE_1, mss::c_str(i_pmic_target));

    // No error trace as that is done in the caller
    l_invalid_range = ((l_vendor_byte_1 == mss::pmic::vendor::TI_SHORT) &&
                       (i_setting != mss::lab::pmic::update::setting::SWD_RANGE));

    MSS_LAB_ASSERT_RETURN(!l_invalid_range,
                          "Ranges unsupported on SWA, SWB, SWC for TI PMIC %s. No action taken", i_pmic_target);

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Set the provided voltage on the current range
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (in V)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_voltage_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                     const mss::pmic::rail i_rail,
                                     const float i_value)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    // First, round the voltage
    static constexpr float V_TO_MV_CONVERSION = 1000;

    const uint32_t l_value_mv = i_value * V_TO_MV_CONVERSION;
    const uint32_t l_voltage_rounded = mss::lab::pmic::bias::round_rail_target_voltage(l_value_mv);

    // Get current range
    fapi2::buffer<uint8_t> l_range_reg_contents;
    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R51, l_range_reg_contents));

    {
        const uint8_t l_range_bit = mss::pmic::VOLT_RANGE_FLDS[i_rail];
        const uint8_t l_range = l_range_reg_contents.getBit(l_range_bit);

        // Check in bounds
        const uint32_t MIN_VOLT = mss::pmic::VOLT_RANGE_MINS[i_rail][l_range];
        const uint32_t MAX_VOLT = mss::pmic::VOLT_RANGE_MAXES[i_rail][l_range];

        MSS_LAB_ASSERT_RETURN((l_voltage_rounded >= MIN_VOLT) && (l_voltage_rounded <= MAX_VOLT),
                              "set_voltage(): After rounding the bias voltage, "
                              "the resulting voltage %lumV was out of range LOW: %lumV HIGH: %lumV for PMIC %s on rail %u",
                              l_voltage_rounded, MIN_VOLT, MAX_VOLT, mss::c_str(i_pmic_target), uint8_t(i_rail));
        {
            // Since we're in range, we can set the new value

            // Convert to bit mapping and write back
            const uint32_t l_offset_from_min = l_voltage_rounded - MIN_VOLT;
            const uint8_t l_voltage_bitmap = l_offset_from_min / CONSTS::VOLT_STEP;

            // Shift and write back
            fapi2::buffer<uint8_t> l_voltage_write_back;

            l_voltage_write_back.insertFromRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>(l_voltage_bitmap);

            mss::logf(mss::TRACE, "Writing %0.3fV with shifted bitmap 0x%02x to register 0x%02x",
                      i_value, l_voltage_write_back, mss::pmic::VOLT_SETTING_VENDOR_REGS[i_rail]);

            FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target,
                                               mss::pmic::VOLT_SETTING_VENDOR_REGS[i_rail],
                                               l_voltage_write_back),
                     "set_voltage_percent: Error writing 0x%02X of PMIC %s",
                     mss::pmic::VOLT_SETTING_VENDOR_REGS[i_rail],
                     mss::c_str(i_pmic_target));
        }
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Calls helper function to set voltage on the desired rail, and then poll for completion
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (in V)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_voltage(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                              const mss::pmic::rail i_rail,
                              const float i_value)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    // Call helper to calculate and set voltage register
    FAPI_TRY(set_voltage_helper(i_pmic_target, i_rail, i_value),
             "Error attempting to set voltage of PMIC %s",
             mss::c_str(i_pmic_target));

    // Next, burn in the register contents
    FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, REGS::R39_COMMAND_CODES, CONSTS::BURN_R40_TO_R4F),
             "Error writing BURN command to PMIC %s", mss::c_str(i_pmic_target));

    // Poll for completion
    FAPI_TRY(poll_for_burn_completion(i_pmic_target),
             "set_voltage(): Polling timed out waiting for burn completition on %s",
             mss::c_str(i_pmic_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Set the provided range on the provided rail in the PMIC vendor region
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (0 or 1. Will be truncated to integer)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_range_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                   const mss::pmic::rail i_rail,
                                   const float i_value)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    const uint8_t l_range_setting = static_cast<uint8_t>(i_value);
    const uint8_t l_range_bit = mss::pmic::VOLT_RANGE_FLDS[i_rail];
    fapi2::buffer<uint8_t> l_range_reg_contents;

    MSS_LAB_ASSERT_RETURN(l_range_setting < CONSTS::NUM_RANGES, // 0 or 1
                          "Range setting was not identified as valid (0,1). Found: %u",
                          l_range_setting);

    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R51, l_range_reg_contents),
             "Error reading from address %02x of PMIC %s", REGS::R51, mss::c_str(i_pmic_target));

    // Set/clear range bit and write back
    l_range_reg_contents.writeBit(l_range_setting, l_range_bit);

    mss::logf(mss::TRACE, "Setting PMIC rail %u to range %u", i_rail, l_range_setting);
    FAPI_TRY(mss::pmic::i2c::reg_write_reverse_buffer(i_pmic_target, REGS::R51, l_range_reg_contents),
             "Error writing to address %02x of PMIC %s", REGS::R51, mss::c_str(i_pmic_target));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Calls helper function to set desired range, and then poll for completion
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to update
/// @param[in] i_value value to update with (0 or 1. Will be truncated to integer)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff no errors
///
fapi2::ReturnCode set_range(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                            const mss::pmic::rail i_rail,
                            const float i_value)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    mss::logf(mss::TRACE, "\n"
              "\n* PMIC %s"
              "\n* Warning: updating the range leaves the voltage setting register unaffected!"
              "\n* It will shift +/- 200mV dependinding on whether the range was increased or decreased."
              "\n* Be sure to update the voltages for the affected rail."
              "\n", mss::c_str(i_pmic_target));

    // Call helper function
    FAPI_TRY(set_range_helper(i_pmic_target, i_rail, i_value), "Error setting range on PMIC %s", mss::c_str(i_pmic_target));

    // Next, burn in the register contents
    FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, REGS::R39_COMMAND_CODES, CONSTS::BURN_R50_TO_R5F),
             "Error writing BURN command to PMIC %s", mss::c_str(i_pmic_target));

    // Poll for completion
    FAPI_TRY(poll_for_burn_completion(i_pmic_target),
             "set_voltage(): Polling timed out waiting for burn completition on %s",
             mss::c_str(i_pmic_target));

fapi_try_exit:
    return fapi2::current_err;
}

} // namespace update


// ----------------------------------
//          Update Functions
// ----------------------------------

///
/// @brief Helper function to update PMIC
///
/// @param[in] i_pmic_target PMIC target to perform update on
/// @param[in] i_setting SETTING to modify
/// @param[in] i_value value to update with
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode update_helper(
    const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
    const mss::lab::pmic::update::setting i_setting,
    const float i_value)
{
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    FAPI_TRY(mss::pmic::unlock_vendor_region(i_pmic_target),
             "update_helper: Error unlocking vendor region for PMIC %s", mss::c_str(i_pmic_target));

    switch (i_setting)
    {
        case update::setting::SWA_VOLT:
        case update::setting::SWB_VOLT:
        case update::setting::SWC_VOLT:
        case update::setting::SWD_VOLT:
            {
                // Use modulus operator to get the matching rail
                FAPI_TRY(update::set_voltage(i_pmic_target, static_cast<mss::pmic::rail>(i_setting % CONSTS::NUMBER_OF_RAILS), i_value),
                         "update_helper(): Error setting voltage on PMIC %s", mss::c_str(i_pmic_target));
                break;
            }

        case update::setting::SWA_RANGE:
        case update::setting::SWB_RANGE:
        case update::setting::SWC_RANGE:
        case update::setting::SWD_RANGE:
            {
                FAPI_TRY(update::ti_pmic_range_workaround(i_pmic_target, i_setting));
                // Use modulus operator to get the matching rail
                FAPI_TRY(update::set_range(i_pmic_target, static_cast<mss::pmic::rail>(i_setting % CONSTS::NUMBER_OF_RAILS), i_value),
                         "update_helper(): Error setting voltage range on PMIC %s", mss::c_str(i_pmic_target));
                break;
            }

        default:
            MSS_LAB_ASSERT_RETURN(false, "Unknown setting enum value %u to update on PMIC %s",
                                  static_cast<uint8_t>(i_setting), mss::c_str(i_pmic_target));
    }

fapi_try_exit:
    // Lock on exit
    return mss::pmic::lock_vendor_region(i_pmic_target, fapi2::current_err);
}

///
/// @brief PMIC update function to modify vendor region
///
/// @param[in] i_ocmb_target OCMB target with PMIC children to update
/// @param[in] i_id ID of pmic (PMIC0, PMIC1)
/// @param[in] i_setting SETTING to modify
/// @param[in] i_value value to update with
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode update_chip(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const mss::lab::pmic::id i_id,
    const mss::lab::pmic::update::setting i_setting,
    const float i_value)
{
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmics;

    if (i_id == mss::lab::pmic::id::ALL)
    {
        // All of 'em
        l_pmics = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target, fapi2::TARGET_STATE_PRESENT);

        if(l_pmics.empty())
        {
            mss::logf(mss::ERROR, "No PMICs found on OCMB %s", mss::c_str(i_ocmb_target));
        }
    }
    else
    {
        // Either PMIC0 or PMIC1, can cast to mss::pmic::id and use the common_lab helper
        l_pmics = mss::pmic::get_valid_pmics_for_id(i_ocmb_target, static_cast<mss::pmic::id>(i_id));

        if(l_pmics.empty())
        {
            mss::logf(mss::ERROR, "%s No PMIC found matching id %d", mss::c_str(i_ocmb_target), static_cast<uint8_t>(i_id));
        }
    }

    for (const auto& l_pmic : l_pmics)
    {
        // Check to make sure VIN_BULK measures above minimum voltage tolerance, ensuring the PMIC
        // will function as expected
        FAPI_TRY(mss::pmic::check_vin_bulk_good(l_pmic),
                 "%s pmic_enable: check for VIN_BULK good either failed, or was below minimum voltage tolerance",
                 mss::c_str(l_pmic));

        mss::logf(mss::TRACE, "Performing pmic_update on PMIC %s", mss::c_str(l_pmic));

        // Now call the helper function
        FAPI_TRY(update_helper(l_pmic, i_setting, i_value),
                 "Error performing UPDATE on PMIC %s", mss::c_str(l_pmic));
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // pmic
} // lab
} // mss
