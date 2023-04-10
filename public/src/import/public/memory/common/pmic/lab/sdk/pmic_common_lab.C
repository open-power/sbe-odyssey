/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/sdk/pmic_common_lab.C $ */
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
/// @file pmic_common_lab.C
/// @brief Common definitions for PMIC lab input processing
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2

#include <fapi2.H>
#include <pmic_common_lab.H>
#include <generic/memory/lab/mss_lab_tools.H>
#include <generic/memory/lab/mss_lab_input_processing.H>
#include <lib/utils/pmic_common_utils.H>
#include <lib/i2c/i2c_pmic.H>

namespace mss
{
namespace lab
{
namespace pmic
{

///
/// @brief mappings from text to mss::pmic::id enum
///
template<> const std::map<std::string, mss::pmic::id>
mss::lab::enum_traits<mss::pmic::id>::iv_string_to_enum_map
{
    {"PMIC0", mss::pmic::id::PMIC0},
    {"PMIC1", mss::pmic::id::PMIC1},
};

template<> const char* mss::lab::enum_traits<mss::pmic::id>::iv_type = "mss::pmic::id";


/// @brief Get the current rail voltage of a JEDEC PMIC
///
/// @param[in] i_pmic_target PMIC target
/// @param[in] i_rail rail to read from
/// @param[out] o_current_rail_voltage voltage calculated for rail
/// @param[out] o_range_selection range selection of that voltage
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success, else error
/// @note not templated as the arguments may differ for other chips (if we ever use others)
///
fapi2::ReturnCode get_current_rail_voltage(const fapi2::Target<fapi2::TargetType::TARGET_TYPE_PMIC>& i_pmic_target,
        const mss::pmic::rail i_rail,
        uint32_t& o_current_rail_voltage,
        uint8_t& o_range_selection)
{
    fapi2::buffer<uint8_t> l_voltage_setting_reg_contents;
    fapi2::buffer<uint8_t> l_voltage_range_reg_contents;

    using REGS = pmicRegs<mss::pmic::product::JEDEC_COMPLIANT>;
    using FIELDS = pmicFields<mss::pmic::product::JEDEC_COMPLIANT>;
    using CONSTS = mss::pmic::consts<mss::pmic::product::JEDEC_COMPLIANT>;

    // Get voltage for rail
    FAPI_TRY(mss::pmic::i2c::reg_read(i_pmic_target, mss::pmic::VOLT_SETTING_ACTIVE_REGS[i_rail],
                                      l_voltage_setting_reg_contents),
             "set_voltage_percent: Error reading 0x%02hhX of PMIC %s", mss::pmic::VOLT_SETTING_ACTIVE_REGS[i_rail],
             mss::c_str(i_pmic_target));

    // Get voltage range selections
    FAPI_TRY(mss::pmic::i2c::reg_read_reverse_buffer(i_pmic_target, REGS::R2B, l_voltage_range_reg_contents),
             "set_voltage_percent: Error reading 0x%02hhX of PMIC %s", REGS::R2B, mss::c_str(i_pmic_target));
    {
        // Identify range
        const uint8_t l_range_selection = l_voltage_range_reg_contents.getBit(mss::pmic::VOLT_RANGE_FLDS[i_rail]);
        uint8_t l_voltage_setting = 0;

        l_voltage_setting_reg_contents.extractToRight<FIELDS::VOLTAGE_SETTING_START, FIELDS::VOLTAGE_SETTING_LENGTH>
        (l_voltage_setting);

        // Get current voltage using: range_min + (step * setting)
        o_current_rail_voltage = mss::pmic::VOLT_RANGE_MINS[i_rail][l_range_selection] + (CONSTS::VOLT_STEP *
                                 l_voltage_setting);
        o_range_selection = l_range_selection;
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // lab
} // pmic
} // mss
