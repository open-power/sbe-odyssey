/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_status/pmic_status_utils.C $ */
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
/// @file pmic_status_utils.C
/// @brief Util functions for pmic_status lab tool
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <map>
#include <fapi2.H>
#include <lib/utils/pmic_consts.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>
#include <lib/i2c/i2c_pmic.H>
#include <lib/utils/pmic_common_utils.H>
#include <pmic_common_lab.H>
#include <pmic_status/pmic_status_utils.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/lib/utils/poll.H>
#include <pmic_bias/pmic_bias_utils.H>
#include <generic/memory/lib/utils/buffer_ops.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace status
{

///
/// @brief Get the vendor string from the provided (short) ID
///
/// @param[in] l_vendor_id vendor ID byte 1 from PMIC
/// @return std::string& vendor id string
/// @note this is a unit-testable helper function
///
std::string get_vendor_from_reg_helper(const uint8_t l_vendor_id)
{
    // Pick the right vendor string
    switch(l_vendor_id)
    {
        case mss::pmic::vendor::TI_SHORT:
            return "TI";
            break;

        case mss::pmic::vendor::IDT_SHORT:
            return "IDT";
            break;

        default:
            // Not worth bombing out here; as long as the pmic is JEDEC compliant,
            // the rest of the registers should still be fair game to read and parse,
            // But this certainly should be updated to support future PMIC types
            return "Unknown";
            break;
    }

    // Will not be executed
    return "Unknown";
}

///
/// @brief Return a vendor string determined by the provided PMIC target's vendor ID register
///
/// @param[in] i_pmic PMIC target
/// @return std::string Vendor name
///
fapi2::ReturnCode get_vendor_from_reg(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic,
                                      std::string& o_vendor)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    fapi2::buffer<uint8_t> l_reg_contents;

    // Only need to read one of the two bytes to determine the vendor
    FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic, REGS::R3D_VENDOR_ID_BYTE_1, l_reg_contents));

    o_vendor = get_vendor_from_reg_helper(static_cast<uint8_t>(l_reg_contents));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Calculate the current value in mA from register bitmap
///
/// @param[in] i_bitmap 8 bit bitmap buffer from register
/// @return uint16_t current in mA
///
uint16_t calculate_current_from_bitmap(const fapi2::buffer<uint8_t>& i_bitmap)
{
    // Multiply bitmapping by 125 mA to convert from bitmapping
    return (static_cast<uint16_t>(i_bitmap) * consts::CURRENT_BITMAP_MULTIPLIER);
}

///
/// @brief Get the PMIC SW output control state (disabled or enabled) for given rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to check
/// @param[out] o_rail_control_enable whether rail is disabled or enabled
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode pmic_sw_output_control_state(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::pmic::rail i_rail,
        bool& o_rail_control_enable)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    static std::map<mss::pmic::rail, uint8_t> RAIL_CONTROL_OUTPUT_BIT_MAP =
    {
        {mss::pmic::rail::SWA, FIELDS::R2F_SWA_REGULATOR_CONTROL},
        {mss::pmic::rail::SWB, FIELDS::R2F_SWB_REGULATOR_CONTROL},
        {mss::pmic::rail::SWC, FIELDS::R2F_SWC_REGULATOR_CONTROL},
        {mss::pmic::rail::SWD, FIELDS::R2F_SWD_REGULATOR_CONTROL},
    };

    fapi2::buffer<uint8_t> l_reg_contents;

    // Get SW regulator control bit
    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R2F, l_reg_contents));

    o_rail_control_enable = l_reg_contents.getBit(RAIL_CONTROL_OUTPUT_BIT_MAP[i_rail]);

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Get the pmic phase mode select bit
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_pmic_swa_swb_dual_phase_mode PMIC SWA/B phase mode select bit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode get_pmic_phase_mode_select_bit(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        bool& o_pmic_swa_swb_dual_phase_mode)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;

    FAPI_TRY(mss::pmic::unlock_vendor_region(i_pmic_target));
    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R4F, l_reg_contents));
    o_pmic_swa_swb_dual_phase_mode = l_reg_contents.getBit<FIELDS::R4F_SWA_SWB_PHASE_MODE_SELECT>();

fapi_try_exit:
    // Try to lock vendor region even in the case of an error in this function
    return mss::pmic::lock_vendor_region(i_pmic_target, fapi2::current_err);
}

///
/// @brief Get the pmic current capability setting
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_pmic_high_current_capable PMIC current capability bit (0=low current, 1=high current)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode get_pmic_high_current_capability(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        bool& o_pmic_high_current_capable)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;

    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R3B_REVISION, l_reg_contents));
    o_pmic_high_current_capable = l_reg_contents.getBit<FIELDS::R3B_PMIC_CURRENT_CAPABILITY>();

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display the current or power for the given rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to display
/// @param[in] i_current_power_select input selection for current or power measurements
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode display_rail_current_power(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::pmic::rail i_rail,
        const mss::lab::pmic::status::meter_select i_current_power_select)

{
    static std::map<mss::pmic::rail, uint8_t> RAIL_CURRENT_POWER_MAP =
    {
        {mss::pmic::rail::SWA, REGS::R0C},
        {mss::pmic::rail::SWB, REGS::R0D},
        {mss::pmic::rail::SWC, REGS::R0E},
        {mss::pmic::rail::SWD, REGS::R0F},
    };

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;
    bool l_report_total_power = false;
    bool l_sw_ab_dual_phase_mode = false;
    bool l_sw_output_is_total_power = false;
    bool l_sw_output_is_individual = false;

    // get output power select bit (0=individual output power, 1=total output power) for output power readings
    // not applicable for output current readings
    if (i_current_power_select == mss::lab::pmic::status::meter_select::POWER)
    {
        FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R1A, l_reg_contents));
        l_report_total_power = l_reg_contents.getBit<FIELDS::R1A_OUTPUT_POWER_SELECT>();
    }

    // get SWA/B phase mode select (0=single phase for SWA/B, 1=dual phase for SWA/B)
    if ((i_rail == mss::pmic::rail::SWA) || (i_rail == mss::pmic::rail::SWB))
    {
        FAPI_TRY(get_pmic_phase_mode_select_bit(i_pmic_target, l_sw_ab_dual_phase_mode));
    }

    // if i_current_power_select is power and l_report_total_power are both set
    // then only report SWA values (total power is reported here)
    // otherwise print all output values
    l_sw_output_is_total_power = ( (i_current_power_select == mss::lab::pmic::status::meter_select::POWER) &&
                                   (l_report_total_power) &&
                                   (i_rail == mss::pmic::rail::SWA)  );
    l_sw_output_is_individual = (  (i_current_power_select == mss::lab::pmic::status::meter_select::CURRENT) ||
                                   ((!l_report_total_power) && (i_current_power_select == mss::lab::pmic::status::meter_select::POWER))  );

    if ((l_sw_output_is_total_power) || (l_sw_output_is_individual))
    {
        // set output current or power select if not already set (0=current, 1=power)
        FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R1B, l_reg_contents));

        if (l_reg_contents.getBit<FIELDS::R1B_CURRENT_OR_POWER_METER_SELECT>() != i_current_power_select)
        {
            l_reg_contents.writeBit<FIELDS::R1B_CURRENT_OR_POWER_METER_SELECT>(i_current_power_select);
            FAPI_TRY(mss::pmic::i2c::reg_write_reverse_buffer(i_pmic_target, REGS::R1B, l_reg_contents));
        }

        // Enable the PMIC ADC
        FAPI_TRY(mss::pmic::enable_pmic_adc(i_pmic_target));

        // read the current/power value from the pmic register
        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, RAIL_CURRENT_POWER_MAP[i_rail], l_reg_contents));
        const auto l_current_power = calculate_current_from_bitmap(l_reg_contents);

        // Display the value
        mss::logf(mss::TRACE, "%s PMIC%u: %s %s: %s: %u %s",
                  mss::c_str(i_pmic_target),
                  mss::index(i_pmic_target),
                  ((i_current_power_select == mss::lab::pmic::status::meter_select::CURRENT) ? "CURRENT" : "WATTAGE"),
                  mss::lab::pmic::status::RAIL_NAME_MAP[i_rail].c_str(),
                  (  (  (l_sw_ab_dual_phase_mode) &&
                        (  (i_rail == mss::pmic::rail::SWA) || (i_rail == mss::pmic::rail::SWB)  )  )  ?
                     " (Dual Phase SWA+SWB)" : " (Single Phase)"  ),
                  l_current_power,
                  ((i_current_power_select == mss::lab::pmic::status::meter_select::CURRENT) ? "mA" : "mW")
                 );

        // Disable the PMIC ADC
        FAPI_TRY(mss::pmic::disable_pmic_adc(i_pmic_target));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display current information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_currents(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
    {
        FAPI_TRY(display_rail_current_power(i_pmic_target, static_cast<mss::pmic::rail>(l_rail_index),
                                            mss::lab::pmic::status::meter_select::CURRENT));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display power information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_powers(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
    {
        FAPI_TRY(display_rail_current_power(i_pmic_target, static_cast<mss::pmic::rail>(l_rail_index),
                                            mss::lab::pmic::status::meter_select::POWER));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display ADC readout data for the given ADC selection
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_selection ADC selection to display
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_adc_readout(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                      const mss::lab::pmic::status::adc_select i_selection)
{

    static std::map<mss::lab::pmic::status::adc_select, std::string> ADC_SELECT_NAME_MAP =
    {
        {mss::lab::pmic::status::adc_select::ADC_SWA,         "ADC_SWA"},
        {mss::lab::pmic::status::adc_select::ADC_SWB,         "ADC_SWB"},
        {mss::lab::pmic::status::adc_select::ADC_SWC,         "ADC_SWC"},
        {mss::lab::pmic::status::adc_select::ADC_SWD,         "ADC_SWD"},
        {mss::lab::pmic::status::adc_select::ADC_VIN_BULK,    "ADC_VIN_BULK"},
        {mss::lab::pmic::status::adc_select::ADC_VIN_MGMT,    "ADC_VIN_MGMT"},
        {mss::lab::pmic::status::adc_select::ADC_VBIAS,       "ADC_VBIAS"},
        {mss::lab::pmic::status::adc_select::ADC_VOUT_1P8V,   "ADC_VOUT_1P8V"},
        {mss::lab::pmic::status::adc_select::ADC_VOUT_1P1V,   "ADC_VOUT_1P1V"},
        {mss::lab::pmic::status::adc_select::ADC_TEMPERATURE, "ADC_TEMPERATURE"},
    };

    static std::map<mss::lab::pmic::status::adc_select, uint8_t> ADC_SELECT_STEP_MAP =
    {
        {mss::lab::pmic::status::adc_select::ADC_SWA,         15},
        {mss::lab::pmic::status::adc_select::ADC_SWB,         15},
        {mss::lab::pmic::status::adc_select::ADC_SWC,         15},
        {mss::lab::pmic::status::adc_select::ADC_SWD,         15},
        {mss::lab::pmic::status::adc_select::ADC_VIN_BULK,    70},
        {mss::lab::pmic::status::adc_select::ADC_VIN_MGMT,    15},
        {mss::lab::pmic::status::adc_select::ADC_VBIAS,       25},
        {mss::lab::pmic::status::adc_select::ADC_VOUT_1P8V,   15},
        {mss::lab::pmic::status::adc_select::ADC_VOUT_1P1V,   15},
        {mss::lab::pmic::status::adc_select::ADC_TEMPERATURE,  2},
    };

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;
    fapi2::buffer<uint8_t> l_selection_modify;
    uint16_t l_adc_readout = 0;
    bool l_sw_ab_dual_phase_mode = false;
    bool l_pmic_is_ti = false;
    uint16_t l_calculated_adc_read_value = 0;
    static const uint8_t MIN_NEGATIVE_REG_VALUE = (pow(2, (FIELDS::R31_ADC_READ_SETTING_LENGTH - 1)));
    static const uint16_t MIN_NEGATIVE_CALC_TEMP = MIN_NEGATIVE_REG_VALUE * (ADC_SELECT_STEP_MAP[i_selection]);

    // get SWA/B phase mode select (0=single phase for SWA/B, 1=dual phase for SWA/B)
    if (i_selection == mss::lab::pmic::status::adc_select::ADC_SWB)
    {
        FAPI_TRY(get_pmic_phase_mode_select_bit(i_pmic_target, l_sw_ab_dual_phase_mode));
    }

    FAPI_TRY(mss::pmic::pmic_is_ti(i_pmic_target, l_pmic_is_ti));

    // ADC SWB register data not applicable when in dual phase mode
    // Only TI suppports ADC Temperature based on the pmic specs
    if ( !( ((l_sw_ab_dual_phase_mode) && (i_selection == mss::lab::pmic::status::adc_select::ADC_SWB)) ||
            ((l_pmic_is_ti == false) && (i_selection == mss::lab::pmic::status::adc_select::ADC_TEMPERATURE)) ||
            (i_selection == mss::lab::pmic::status::adc_select::RESERVED)
          )
       )
    {
        // program the ADC select bits
        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R30, l_reg_contents));
        l_reg_contents.insertFromRight<FIELDS::R30_ADC_SELECT_START, FIELDS::R30_ADC_SELECT_LENGTH>(i_selection);
        FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, REGS::R30, l_reg_contents));

        // Enable the PMIC ADC
        FAPI_TRY(mss::pmic::enable_pmic_adc(i_pmic_target));

        // get PMIC ADC_READ register contents
        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R31, l_reg_contents));
        l_reg_contents.extractToRight<FIELDS::R31_ADC_READ_SETTING_START, FIELDS::R31_ADC_READ_SETTING_LENGTH>(l_adc_readout);

        // calculate ADC_READ value
        l_calculated_adc_read_value = ((l_adc_readout >= MIN_NEGATIVE_REG_VALUE)
                                       && (i_selection == mss::lab::pmic::status::adc_select::ADC_TEMPERATURE))  ?
                                      (MIN_NEGATIVE_CALC_TEMP - ((l_adc_readout - MIN_NEGATIVE_REG_VALUE) * (ADC_SELECT_STEP_MAP[i_selection]))) :
                                      (l_adc_readout * (ADC_SELECT_STEP_MAP[i_selection]));

        mss::logf(mss::TRACE, "%s PMIC%u: %s: %s%u %s",
                  mss::c_str(i_pmic_target),
                  mss::index(i_pmic_target),
                  ADC_SELECT_NAME_MAP[i_selection].c_str(),
                  (((l_adc_readout >= MIN_NEGATIVE_REG_VALUE)
                    && (i_selection == mss::lab::pmic::status::adc_select::ADC_TEMPERATURE)) ? "-" : ""),
                  l_calculated_adc_read_value,
                  ((i_selection == mss::lab::pmic::status::adc_select::ADC_TEMPERATURE) ? "C" : "mV"));

        // Disable the PMIC ADC
        FAPI_TRY(mss::pmic::disable_pmic_adc(i_pmic_target));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display ADC information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_adc_data(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    for (uint8_t l_select_index = mss::lab::pmic::status::adc_select::ADC_SWA;
         l_select_index <= mss::lab::pmic::status::adc_select::ADC_TEMPERATURE;
         ++l_select_index)
    {
        FAPI_TRY(display_adc_readout(i_pmic_target, static_cast<mss::lab::pmic::status::adc_select>(l_select_index)));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display VR enable bit setting from the PMIC register
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_vr_enable(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;

    // get PMIC enable register
    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R32, l_reg_contents));

    // print out VR enable bit value (R32(7):  0=OFF, 1=ON)
    mss::logf(mss::TRACE, "%s PMIC%u: %s: %s",
              mss::c_str(i_pmic_target), mss::index(i_pmic_target), "VR_ENABLE",
              (l_reg_contents.getBit<FIELDS::R32_VR_ENABLE>() ? "ON" : "OFF") );

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display LDO voltage information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_ldo_voltages(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;
    uint8_t l_ldo_1p8_setting = 0;
    uint8_t l_ldo_1p1_setting = 0;
    // these constants come from PMIC REG 0x2B (bits 7:6 for 1.8V) and (bits 2:1 for 1.0V)
    static const uint16_t l_ldo_1p8_voltage_mv[] = {1700, 1800, 1900, 2000};
    static const uint16_t l_ldo_1p1_voltage_mv[] = { 900, 1000, 1100, 1200};

    // get PMIC LDO voltage settings
    FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R2B, l_reg_contents));
    l_reg_contents.extractToRight<FIELDS::R2B_LDO_1P8_VOLT_SETTING_START, FIELDS::R2B_LDO_1P8_VOLT_SETTING_LENGTH>
    (l_ldo_1p8_setting);
    l_reg_contents.extractToRight<FIELDS::R2B_LDO_1P1_VOLT_SETTING_START, FIELDS::R2B_LDO_1P1_VOLT_SETTING_LENGTH>
    (l_ldo_1p1_setting);

    mss::logf(mss::TRACE, "%s PMIC%u: %s: %u mV",
              mss::c_str(i_pmic_target), mss::index(i_pmic_target), "1P8V", l_ldo_1p8_voltage_mv[l_ldo_1p8_setting]);
    mss::logf(mss::TRACE, "%s PMIC%u: %s: %u mV",
              mss::c_str(i_pmic_target), mss::index(i_pmic_target), "1P1V", l_ldo_1p1_voltage_mv[l_ldo_1p1_setting]);

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display voltage information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_voltages(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    uint32_t l_current_rail_voltage = 0;
    uint8_t l_range_selection = 0;
    bool l_rail_control_enable = false;
    bool l_sw_ab_dual_phase_mode = false;

    for (uint8_t l_rail_index = mss::pmic::rail::SWA; l_rail_index <= mss::pmic::rail::SWD; ++l_rail_index)
    {
        FAPI_TRY(pmic_sw_output_control_state(i_pmic_target, static_cast<mss::pmic::rail>(l_rail_index),
                                              l_rail_control_enable));
        FAPI_TRY(mss::lab::pmic::get_current_rail_voltage(i_pmic_target,
                 static_cast<mss::pmic::rail>(l_rail_index), l_current_rail_voltage, l_range_selection));

        // SWB regulator control only applicable in single phase mode
        if (static_cast<mss::pmic::rail>(l_rail_index) == mss::pmic::rail::SWB)
        {
            FAPI_TRY(get_pmic_phase_mode_select_bit(i_pmic_target, l_sw_ab_dual_phase_mode));
        }

        mss::logf(mss::TRACE, "%s PMIC%u: %s: %s: %u mV",
                  mss::c_str(i_pmic_target),
                  mss::index(i_pmic_target),
                  mss::lab::pmic::status::RAIL_NAME_MAP[static_cast<mss::pmic::rail>(l_rail_index)].c_str(),
                  (  (  (static_cast<mss::pmic::rail>(l_rail_index) == mss::pmic::rail::SWB) &&
                        (l_sw_ab_dual_phase_mode)  ) ?
                     "(NA - Dual Phase)" :
                     ((l_rail_control_enable) ? "(Output Enabled)" : "(Output DISABLED)")  ),
                  l_current_rail_voltage);
    }

    FAPI_TRY(display_vr_enable(i_pmic_target));

    FAPI_TRY(display_ldo_voltages(i_pmic_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display temperature as reported from the PMIC register
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_temperature(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target)
{
    static std::map<uint8_t, std::string> TEMP_DESC_MAP =
    {
        {0, "<=85 C"},
        {1, "85 C"},
        {2, "95 C"},
        {3, "105 C"},
        {4, "115 C"},
        {5, "125 C"},
        {6, "135 C"},
        {7, ">140 C"},
    };

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    fapi2::buffer<uint8_t> l_reg_contents;
    uint8_t l_temperature_bits = 0;

    // get temperature measurement data
    FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R33, l_reg_contents));
    l_reg_contents.extractToRight<FIELDS::R33_TEMPERATURE_SETTING_START, FIELDS::R33_TEMPERATURE_SETTING_LENGTH>
    (l_temperature_bits);

    mss::logf(mss::TRACE, "%s PMIC%u: %s", mss::c_str(i_pmic_target), mss::index(i_pmic_target),
              TEMP_DESC_MAP[l_temperature_bits].c_str());

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Determine vector of PMIC rails to get or set the current threshold value
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to display
/// @param[in] i_set_rail RAIL to set
/// @param[in,out] io_rail_v vector of pmic rails to get/set current threshold values
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode get_rails_for_current_thresholds(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::status::rail_current i_rail,
        const mss::lab::pmic::status::rail_current i_set_rail,
        std::vector<mss::lab::pmic::status::rail_current>& io_rail_v)
{
    bool l_sw_ab_dual_phase_mode = false;

    // get SWA/B phase mode select (0=single phase for SWA/B, 1=dual phase for SWA/B)
    // For dual phase SWA+SWB, SWA setting should be the same as the SWB setting so program both
    // PMIC outputs SWC and SWD are single phase only
    if (  (  (i_set_rail == mss::lab::pmic::status::rail_current::SWA) ||
             (i_set_rail == mss::lab::pmic::status::rail_current::SWB)  ) &&
          (i_rail == i_set_rail)  )
    {
        FAPI_TRY(get_pmic_phase_mode_select_bit(i_pmic_target, l_sw_ab_dual_phase_mode));
    }

    if (l_sw_ab_dual_phase_mode)
    {
        io_rail_v.push_back(mss::lab::pmic::status::rail_current::SWA);
        io_rail_v.push_back(mss::lab::pmic::status::rail_current::SWB);
    }
    else
    {
        io_rail_v.push_back(i_rail);
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Get/set the PMIC high current consumption warning threshold for given rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to display
/// @param[in] i_set_rail RAIL to set
/// @param[in] i_current_warning_threshold  high current warning threshold value to program in mA
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_set_current_warning_rail(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::status::rail_current i_rail,
        const mss::lab::pmic::status::rail_current i_set_rail,
        const uint16_t i_current_warning_threshold)
{

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    static std::map<mss::lab::pmic::status::rail_current, uint8_t> RAIL_CURRENT_WARNING_MAP =
    {
        {mss::lab::pmic::status::rail_current::SWA, REGS::R1C},
        {mss::lab::pmic::status::rail_current::SWB, REGS::R1D},
        {mss::lab::pmic::status::rail_current::SWC, REGS::R1E},
        {mss::lab::pmic::status::rail_current::SWD, REGS::R1F},
    };

    uint16_t l_reg_setting = 0;
    fapi2::buffer<uint8_t> l_reg_contents;
    std::vector<mss::lab::pmic::status::rail_current> l_rail_v;

    // get vector of rails to get or set current threshold value
    FAPI_TRY(get_rails_for_current_thresholds(i_pmic_target, i_rail, i_set_rail, l_rail_v));

    // program the high current consumption warning threshold register
    // use closest valid setting equal to or lower than specified value
    for (const auto& l_rail : l_rail_v)
    {
        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, RAIL_CURRENT_WARNING_MAP[l_rail], l_reg_contents));
        l_reg_contents.extractToRight<FIELDS::HIGH_CURRENT_WARNING_START, FIELDS::HIGH_CURRENT_WARNING_LENGTH>
        (l_reg_setting);

        // Only program for rail specified
        if (i_rail == i_set_rail)
        {
            // Only program for valid values
            if ((i_current_warning_threshold >= mss::lab::pmic::status::consts::CURRENT_WARNING_RESOLUTION) &&
                (i_current_warning_threshold <= mss::lab::pmic::status::CURRENT_WARNING_MAX_VALUE))
            {
                l_reg_setting = i_current_warning_threshold / mss::lab::pmic::status::consts::CURRENT_WARNING_RESOLUTION;
                l_reg_contents.insertFromRight<FIELDS::HIGH_CURRENT_WARNING_START, FIELDS::HIGH_CURRENT_WARNING_LENGTH>
                (l_reg_setting);
                FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, RAIL_CURRENT_WARNING_MAP[l_rail], l_reg_contents));
            }
            // error out for invalid settings
            else if (i_current_warning_threshold > 0)
            {
                FAPI_TRY(fapi2::FAPI2_RC_INVALID_PARAMETER,
                         "Invalid current warning threshold setting found (%u), not programming", i_current_warning_threshold);
            }
        }

        mss::logf(mss::TRACE, "%s PMIC%u: %s: High Current Warning Threshold: %u mA",
                  mss::c_str(i_pmic_target),
                  mss::index(i_pmic_target),
                  mss::lab::pmic::status::CURRENT_RAIL_MAP[l_rail].c_str(),
                  (calculate_current_from_bitmap(l_reg_setting)));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display PMIC high current consumption warning threshold for all rails and program threshold for specified rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to set
/// @param[in] i_current_warning_threshold  high current warning threshold value to program in mA
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_set_current_warning(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::status::rail_current i_rail,
        const uint16_t i_current_warning_threshold)
{
    for (uint8_t l_rail_index = mss::lab::pmic::status::rail_current::SWA;
         l_rail_index <= mss::lab::pmic::status::rail_current::SWD; ++l_rail_index)
    {
        FAPI_TRY(display_set_current_warning_rail(i_pmic_target,
                 static_cast<mss::lab::pmic::status::rail_current>(l_rail_index),
                 i_rail,
                 i_current_warning_threshold));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Get/set the PMIC current limiter warning threshold for given rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to display
/// @param[in] i_set_rail RAIL to set
/// @param[in] i_current_limiter_threshold  current limiter threshold value to program in mA
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_set_current_limiter_rail(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::status::rail_current i_rail,
        const mss::lab::pmic::status::rail_current i_set_rail,
        const uint16_t i_current_limiter_threshold)
{

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

    static std::map<mss::lab::pmic::status::rail_current, uint8_t> RAIL_CURRENT_LIMITER_START_MAP =
    {
        {mss::lab::pmic::status::rail_current::SWA, FIELDS::R20_SWA_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_START},
        {mss::lab::pmic::status::rail_current::SWB, FIELDS::R20_SWB_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_START},
        {mss::lab::pmic::status::rail_current::SWC, FIELDS::R20_SWC_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_START},
        {mss::lab::pmic::status::rail_current::SWD, FIELDS::R20_SWD_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_START},
    };

    uint16_t l_reg_setting = 0;
    fapi2::buffer<uint8_t> l_reg_contents;
    std::vector<mss::lab::pmic::status::rail_current> l_rail_v;
    bool l_pmic_high_current_capable = false;
    uint16_t l_current_limiter_base = 0;

    // get vector of rails to get or set current threshold value
    FAPI_TRY(get_rails_for_current_thresholds(i_pmic_target, i_rail, i_set_rail, l_rail_v));

    // see if pmic is a low current or high current part
    get_pmic_high_current_capability(i_pmic_target, l_pmic_high_current_capable);
    l_current_limiter_base = (l_pmic_high_current_capable) ?
                             mss::lab::pmic::status::consts::HIGH_CURRENT_LIMITER_BASE :
                             mss::lab::pmic::status::consts::LOW_CURRENT_LIMITER_BASE;

    // program the current limiter warning threshold register
    // use closest valid setting equal to or lower than specified value
    for (const auto& l_rail : l_rail_v)
    {
        FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, REGS::R20, l_reg_contents));

        FAPI_TRY(l_reg_contents.extractToRight(l_reg_setting,
                                               RAIL_CURRENT_LIMITER_START_MAP[l_rail],
                                               FIELDS::R20_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_LENGTH));

        // Only program for rail specified
        if (i_rail == i_set_rail)
        {
            // Only program for valid values
            if ((i_current_limiter_threshold >= l_current_limiter_base) &&
                (i_current_limiter_threshold <= mss::lab::pmic::status::CURRENT_LIMITER_MAX_VALUE))
            {
                l_reg_setting = (i_current_limiter_threshold - l_current_limiter_base) /
                                mss::lab::pmic::status::consts::CURRENT_LIMITER_RESOLUTION;

                FAPI_TRY(l_reg_contents.insertFromRight(l_reg_setting,
                                                        RAIL_CURRENT_LIMITER_START_MAP[l_rail],
                                                        FIELDS::R20_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_LENGTH));

                FAPI_TRY(mss::pmic::i2c::reg_write(i_pmic_target, REGS::R20, l_reg_contents));
            }
            // error out for invalid settings
            else if (i_current_limiter_threshold > 0)
            {
                FAPI_TRY(fapi2::FAPI2_RC_INVALID_PARAMETER,
                         "Invalid current limiter threshold setting found (%u), not programming", i_current_limiter_threshold);
            }
        }

        mss::logf(mss::TRACE, "%s PMIC%u: %s: Current Limiter Warning Threshold: %u mA",
                  mss::c_str(i_pmic_target),
                  mss::index(i_pmic_target),
                  mss::lab::pmic::status::CURRENT_RAIL_MAP[l_rail].c_str(),
                  (l_current_limiter_base + (l_reg_setting * mss::lab::pmic::status::consts::CURRENT_LIMITER_RESOLUTION)));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Display PMIC current limiter threshold for all rails and program threshold for specified rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to set
/// @param[in] i_current_limiter_threshold  high current warning threshold value to program in mA
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_set_current_limiter(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::lab::pmic::status::rail_current i_rail,
        const uint16_t i_current_limiter_threshold)
{
    for (uint8_t l_rail_index = mss::lab::pmic::status::rail_current::SWA;
         l_rail_index <= mss::lab::pmic::status::rail_current::SWD; ++l_rail_index)
    {
        FAPI_TRY(display_set_current_limiter_rail(i_pmic_target,
                 static_cast<mss::lab::pmic::status::rail_current>(l_rail_index),
                 i_rail,
                 i_current_limiter_threshold));
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // status

///
/// @brief Helper function to poll individual PMICs
///
/// @param[in] i_pmic_target PMIC target to poll for statuses
/// @param[in] i_clear clear status regs
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode status_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target, const bool i_clear)
{
    // Denotes if an error bit was set in the status registers
    bool l_error = false;
    bool l_local_error = false;
    bool l_pmic_is_idt = false;
    mss::pmic::status::status_regs l_values = {0};

    // Just a clear operation
    if (i_clear)
    {
        return mss::pmic::status::clear(i_pmic_target);
    }

    FAPI_TRY(mss::pmic::pmic_is_idt(i_pmic_target, l_pmic_is_idt));

    // Check IDT specific fields. Assert all are good (unlike PMIC enable where we didn't assert these)
    if (l_pmic_is_idt)
    {
        FAPI_TRY(mss::pmic::status::check_fields(i_pmic_target, "PMIC_WARNING_BIT",
                 mss::pmic::status::IDT_SPECIFIC_STATUS_FIELDS, l_values, l_local_error));
        l_error |= l_local_error;
        l_local_error = false;
    }

    // Check statuses common to all vendors
    FAPI_TRY(mss::pmic::status::check_fields(i_pmic_target, "ERROR",
             mss::pmic::status::STATUS_FIELDS, l_values, l_local_error));
    l_error |= l_local_error;

    // Log if there were no errors
    if (l_error)
    {
        // The wrapper can abort knowing this return code and the caller of the
        // wrapper can use that to know to stop polling
        return fapi2::FAPI2_RC_FALSE;
    }

    mss::logf(mss::TRACE, "All status codes were OK for %s", mss::c_str(i_pmic_target));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief PMIC status function
///
/// @param[in] i_ocmb_target OCMB target with PMIC children to poll
/// @param[in] i_id ID of pmic (PMIC0, PMIC1, ALL)
/// @param[in] i_clear clear status regs
/// @param[in] i_telemetry get/set telemetry data (curent, power, voltage, temperature)
/// @param[in] i_power get PMIC output current and voltage setpoints
/// @param[in] i_temperature get PMIC temperatures
/// @param[in] i_pmic_current_rail PMIC output rail to set current limits
/// @param[in] i_current_warning_threshold  high current warning threshold value to program in mA
/// @param[in] i_current_limiter_threshold  current limiter warning threshold value to program in mA
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode get_statuses(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
    const mss::lab::pmic::id i_id,
    const bool i_clear,
    const bool i_telemetry,
    const bool i_power,
    const bool i_temperature,
    const mss::lab::pmic::status::rail_current i_pmic_current_rail,
    const uint16_t i_current_warning_threshold,
    const uint16_t i_current_limiter_threshold)

{
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmics;
    fapi2::ReturnCode l_rc = fapi2::FAPI2_RC_SUCCESS;

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

    mss::logf(mss::TRACE, "PMICs on OCMB %s", mss::c_str(i_ocmb_target));

    for (const auto& l_pmic : l_pmics)
    {
        // Don't poll for PBULK good in case it is in fact not-good, in which case we wouldn't
        // even attempt to poll for other possible errors
        std::string l_vendor;

        FAPI_TRY(status::get_vendor_from_reg(l_pmic, l_vendor));

        mss::logf(mss::TRACE, "%s PMIC%u VENDOR: %s", mss::c_str(l_pmic), mss::index(l_pmic), l_vendor.c_str());

        // Display/set various current/power/voltage registers if requested
        if ((i_telemetry) || (i_pmic_current_rail != mss::lab::pmic::status::rail_current::NONE))
        {
            FAPI_TRY(status::display_set_current_warning(l_pmic, i_pmic_current_rail, i_current_warning_threshold));
            FAPI_TRY(status::display_set_current_limiter(l_pmic, i_pmic_current_rail, i_current_limiter_threshold));
        }

        // Display various current/power/voltage registers if requested
        if (i_telemetry)
        {
            FAPI_TRY(status::display_currents(l_pmic));
            FAPI_TRY(status::display_powers(l_pmic));
            FAPI_TRY(status::display_voltages(l_pmic));
            FAPI_TRY(status::display_adc_data(l_pmic));
            FAPI_TRY(status::display_temperature(l_pmic));
        }
        // The following options are in place for users to get specific data faster (vs using telemetry option)
        // No need to run these if telemetry option is specified because that will get all the data from the pmic
        else
        {

            // Display pmic output current and voltage if requested (and telemetry not requested)
            if (i_power)
            {
                FAPI_TRY(status::display_currents(l_pmic));
                FAPI_TRY(status::display_voltages(l_pmic));
            }

            // Display pmic temperatures if requested (and telemetry not requested)
            if (i_temperature)
            {
                FAPI_TRY(display_adc_readout(l_pmic, mss::lab::pmic::status::adc_select::ADC_TEMPERATURE));
                FAPI_TRY(status::display_temperature(l_pmic));
            }
        }

        // Done this way so that way both PMICs are checked, instead of potentially bombing out on the first one.
        // There are FAPI_ERR's in status_helper that identify specific error bits with target trace.
        // For a lab tool this should be more than sufficient.

        // |= isn't playing nicely with fapi2::ReturnCode
        l_rc = uint64_t(l_rc) | uint64_t(status_helper(l_pmic, i_clear));
    }

    FAPI_TRY(l_rc, "Error status bits were set on one or more PMICs on %s", mss::c_str(i_ocmb_target));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

} // pmic
} // lab
} // mss
