/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_status/pmic_status_utils.H $ */
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
/// @file pmic_status_utils.H
/// @brief Util functions for pmic_status lab tool
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#ifndef __PMIC_STATUS_UTILS_H__
#define __PMIC_STATUS_UTILS_H__

#include <map>
#include <fapi2.H>
#include <pmic_regs.H>
#include <pmic_regs_fld.H>
#include <lib/utils/pmic_consts.H>
#include <lib/i2c/i2c_pmic.H>
#include <math.h>
#include <pmic_common_lab.H>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace status
{

using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;

///
/// @brief Constants for status helper functions
///
enum consts
{
    // See PMIC specs for registers 0x0C --> 0x0F:
    // Output Power measurement
    // 000001 = 0.125A
    // 000010 = 0.25 A
    // 000011 = 0.375
    // ...

    // Here we are using mA, so we can just multiply the bitmap by 125 to get the value
    CURRENT_BITMAP_MULTIPLIER  = 125,

    // current warning threshold (mA) constants from PMIC registers 0x1C, 0x1D, 0x1E, 0x1F
    CURRENT_WARNING_BASE       = 0,
    CURRENT_WARNING_RESOLUTION = CURRENT_BITMAP_MULTIPLIER,
    CURRENT_WARNING_BIT_LENGTH = FIELDS::HIGH_CURRENT_WARNING_LENGTH,

    // current limiter threshold (mA) constants from PMIC register 0x20
    LOW_CURRENT_LIMITER_BASE  = 2000,
    HIGH_CURRENT_LIMITER_BASE = 4000,
    CURRENT_LIMITER_RESOLUTION = 500,
    CURRENT_LIMITER_BIT_LENGTH = FIELDS::R20_OUTPUT_CURRENT_LIMITER_WARNING_THRESHOLD_SETTING_LENGTH,
};

static const uint16_t CURRENT_WARNING_MAX_VALUE  = CURRENT_WARNING_BASE +
        (CURRENT_WARNING_RESOLUTION * (std::pow(2, static_cast<int>(CURRENT_WARNING_BIT_LENGTH)) - 1));
static const uint16_t CURRENT_LIMITER_MAX_VALUE  = HIGH_CURRENT_LIMITER_BASE +
        (CURRENT_LIMITER_RESOLUTION * (std::pow(2, static_cast<int>(CURRENT_LIMITER_BIT_LENGTH)) - 1));

///
/// @brief Constants for pmic rail enum descriptions
///
static std::map<mss::pmic::rail, std::string> RAIL_NAME_MAP =
{
    {mss::pmic::rail::SWA, "SWA"},
    {mss::pmic::rail::SWB, "SWB"},
    {mss::pmic::rail::SWC, "SWC"},
    {mss::pmic::rail::SWD, "SWD"},
};

///
/// @brief Constants for pmic rail
///
enum rail_current
{
    SWA  = 0,
    SWB  = 1,
    SWC  = 2,
    SWD  = 3,
    NONE = 4,
};

static std::map<mss::lab::pmic::status::rail_current, std::string> CURRENT_RAIL_MAP =
{
    {mss::lab::pmic::status::rail_current::SWA,  "SWA"},
    {mss::lab::pmic::status::rail_current::SWB,  "SWB"},
    {mss::lab::pmic::status::rail_current::SWC,  "SWC"},
    {mss::lab::pmic::status::rail_current::SWD,  "SWD"},
    {mss::lab::pmic::status::rail_current::NONE, "NONE"},
};

///
/// @brief Constants for pmic output current or power meter select
///
enum meter_select
{
    CURRENT = 0,
    POWER = 1,
};

///
/// @brief Constants for ADC select  and register bit values
///
enum adc_select
{
    ADC_SWA                 =  0,
    ADC_SWB                 =  1,
    ADC_SWC                 =  2,
    ADC_SWD                 =  3,
    RESERVED                =  4,
    ADC_VIN_BULK            =  5,
    ADC_VIN_MGMT            =  6,
    ADC_VBIAS               =  7,
    ADC_VOUT_1P8V           =  8,
    ADC_VOUT_1P1V           =  9,
    ADC_TEMPERATURE         = 10,
};

///
/// @brief Get the vendor string from the provided (short) ID
///
/// @param[in] l_vendor_id vendor ID byte 1 from PMIC
/// @return std::string& vendor id string
/// @note this is a unit-testable helper function
///
std::string get_vendor_from_reg_helper(const uint8_t l_vendor_id);

///
/// @brief Return a vendor string determined by the provided PMIC target's vendor ID register
///
/// @param[in] i_pmic PMIC target
/// @param[out] o_vendor std::string vendor name
/// @return std::string Vendor name
///
fapi2::ReturnCode get_vendor_from_reg(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic,
                                      std::string& o_vendor);

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
        bool& o_rail_control_enable);

///
/// @brief Get the pmic current capability setting
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_pmic_high_current_capable PMIC current capability bit (0=low current, 1=high current)
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode get_pmic_high_current_capability(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        bool& o_pmic_high_current_capable);

///
/// @brief Calculate the current value in mA from register bitmal
///
/// @param[in] i_bitmap 8 bit bitmap buffer from register
/// @return uint16_t current in mA
///
uint16_t calculate_current_from_bitmap(const fapi2::buffer<uint8_t>& i_bitmap);

///
/// @brief Get the pmic phase mode select bit
///
/// @param[in] i_pmic_target PMIC target
/// @param[out] o_pmic_swa_swb_dual_phase_mode PMIC SWA/B phase mode select bit
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode get_pmic_phase_mode_select_bit(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        bool& o_pmic_swa_swb_dual_phase_mode);

///
/// @brief Display the current for the given rail
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail RAIL to display
/// @param[in] i_current_power_select input selection for current or power measurements
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode display_rail_current_power(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::pmic::rail i_rail,
        const mss::lab::pmic::status::meter_select i_current_power_select);

///
/// @brief Display current information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_currents(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Display power information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_powers(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Display ADC readout data for the given ADC selection
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_selection ADC selection to display
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_adc_readout(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target,
                                      const mss::lab::pmic::status::adc_select i_selection);

///
/// @brief Display ADC information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_adc_data(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Display LDO voltage information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_ldo_voltages(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Display voltage information as reported from the PMIC registers
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_voltages(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

///
/// @brief Display temperature as reported from the PMIC register
///
/// @param[in] i_pmic_target PMIC target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error code
///
fapi2::ReturnCode display_temperature(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target);

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
        std::vector<mss::lab::pmic::status::rail_current>& io_rail_v);

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
        const uint16_t i_current_warning_threshold);

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
        const uint16_t i_current_warning_threshold);

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
        const uint16_t i_current_limiter_threshold);

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
        const uint16_t i_current_limiter_threshold);

} // status

///
/// @brief Helper function to poll individual PMICs
///
/// @param[in] i_pmic_target PMIC target to poll for statuses
/// @param[in] i_clear clear status regs
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode status_helper(const fapi2::Target<fapi2::TARGET_TYPE_PMIC>& i_pmic_target, const bool i_clear);

///
/// @brief PMIC status function
///
/// @param[in] i_ocmb_target OCMB target with PMIC children to poll
/// @param[in] i_id ID of pmic (PMIC0, PMIC1, ALL)
/// @param[in] i_clear CLEAR status regs
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
    const uint16_t i_current_limiter_threshold);

} // pmic
} // lab
} // mss

#endif
