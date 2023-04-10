/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/tests/pmic_ut_attribute_setters.H $ */
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
/// @file mss_pmic_attribute_accessors_manual.H
/// @brief Manual PMIC attribute accessors
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 1
// *HWP Consumed by: FSP:HB

#ifndef _PMIC_UT_ATTR_SETTERS_H_
#define _PMIC_UT_ATTR_SETTERS_H_

#include <fapi2.H>
#include <mss_pmic_attribute_getters.H>
#include <mss_pmic_attribute_setters.H>

namespace mss
{
namespace attr
{

// These attribute setters are similar to the accessors in mss_pmic_attribute_accessors_manual.H (used in FW)
// The goal of these setters is to make the PMIC0 vs PMIC1 attributes accessible via a nice array [0] or [1],

// Attribute setter FP type
typedef fapi2::ReturnCode (*pmic_attr_setter_ptr)(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        uint8_t i_value);
typedef fapi2::ReturnCode (*pmic_attr_setter_ptr_signed)(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        int8_t i_value);

// Voltage Setting
static constexpr pmic_attr_setter_ptr set_swa_voltage_setting[] =
{
    mss::attr::set_pmic0_swa_voltage_setting,
    mss::attr::set_pmic1_swa_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swb_voltage_setting[] =
{
    mss::attr::set_pmic0_swb_voltage_setting,
    mss::attr::set_pmic1_swb_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swc_voltage_setting[] =
{
    mss::attr::set_pmic0_swc_voltage_setting,
    mss::attr::set_pmic1_swc_voltage_setting
};
static constexpr pmic_attr_setter_ptr set_swd_voltage_setting[] =
{
    mss::attr::set_pmic0_swd_voltage_setting,
    mss::attr::set_pmic1_swd_voltage_setting
};

// Voltage Range Select
static constexpr pmic_attr_setter_ptr set_swa_voltage_range_select[] =
{
    mss::attr::set_pmic0_swa_voltage_range_select,
    mss::attr::set_pmic1_swa_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swb_voltage_range_select[] =
{
    mss::attr::set_pmic0_swb_voltage_range_select,
    mss::attr::set_pmic1_swb_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swc_voltage_range_select[] =
{
    mss::attr::set_pmic0_swc_voltage_range_select,
    mss::attr::set_pmic1_swc_voltage_range_select
};
static constexpr pmic_attr_setter_ptr set_swd_voltage_range_select[] =
{
    mss::attr::set_pmic0_swd_voltage_range_select,
    mss::attr::set_pmic1_swd_voltage_range_select
};

// Voltage Offset
static constexpr pmic_attr_setter_ptr_signed set_swa_voltage_offset[] =
{
    mss::attr::set_pmic0_swa_voltage_offset,
    mss::attr::set_pmic1_swa_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swb_voltage_offset[] =
{
    mss::attr::set_pmic0_swb_voltage_offset,
    mss::attr::set_pmic1_swb_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swc_voltage_offset[] =
{
    mss::attr::set_pmic0_swc_voltage_offset,
    mss::attr::set_pmic1_swc_voltage_offset
};
static constexpr pmic_attr_setter_ptr_signed set_swd_voltage_offset[] =
{
    mss::attr::set_pmic0_swd_voltage_offset,
    mss::attr::set_pmic1_swd_voltage_offset
};

// Sequence Delay
static constexpr pmic_attr_setter_ptr set_swa_sequence_delay[] =
{
    mss::attr::set_pmic0_swa_sequence_delay,
    mss::attr::set_pmic1_swa_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swb_sequence_delay[] =
{
    mss::attr::set_pmic0_swb_sequence_delay,
    mss::attr::set_pmic1_swb_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swc_sequence_delay[] =
{
    mss::attr::set_pmic0_swc_sequence_delay,
    mss::attr::set_pmic1_swc_sequence_delay
};
static constexpr pmic_attr_setter_ptr set_swd_sequence_delay[] =
{
    mss::attr::set_pmic0_swd_sequence_delay,
    mss::attr::set_pmic1_swd_sequence_delay
};

// Sequence Order
static constexpr pmic_attr_setter_ptr set_swa_sequence_order[] =
{
    mss::attr::set_pmic0_swa_sequence_order,
    mss::attr::set_pmic1_swa_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swb_sequence_order[] =
{
    mss::attr::set_pmic0_swb_sequence_order,
    mss::attr::set_pmic1_swb_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swc_sequence_order[] =
{
    mss::attr::set_pmic0_swc_sequence_order,
    mss::attr::set_pmic1_swc_sequence_order
};
static constexpr pmic_attr_setter_ptr set_swd_sequence_order[] =
{
    mss::attr::set_pmic0_swd_sequence_order,
    mss::attr::set_pmic1_swd_sequence_order
};

// Phase Combination
static constexpr pmic_attr_setter_ptr set_phase_comb[] =
{
    mss::attr::set_pmic0_phase_comb,
    mss::attr::set_pmic1_phase_comb
};

// Offset
static constexpr pmic_attr_setter_ptr_signed set_efd_swa_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swa_voltage_offset,
    mss::attr::set_efd_pmic1_swa_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swb_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swb_voltage_offset,
    mss::attr::set_efd_pmic1_swb_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swc_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swc_voltage_offset,
    mss::attr::set_efd_pmic1_swc_voltage_offset
};

static constexpr pmic_attr_setter_ptr_signed set_efd_swd_voltage_offset[] =
{
    mss::attr::set_efd_pmic0_swd_voltage_offset,
    mss::attr::set_efd_pmic1_swd_voltage_offset
};

// Current limiter warnings (EFD)
static constexpr pmic_attr_setter_ptr set_swa_current_warning[] =
{
    mss::attr::set_pmic0_swa_current_warning,
    mss::attr::set_pmic1_swa_current_warning
};

static constexpr pmic_attr_setter_ptr set_swb_current_warning[] =
{
    mss::attr::set_pmic0_swb_current_warning,
    mss::attr::set_pmic1_swb_current_warning
};

static constexpr pmic_attr_setter_ptr set_swc_current_warning[] =
{
    mss::attr::set_pmic0_swc_current_warning,
    mss::attr::set_pmic1_swc_current_warning
};

static constexpr pmic_attr_setter_ptr set_swd_current_warning[] =
{
    mss::attr::set_pmic0_swd_current_warning,
    mss::attr::set_pmic1_swd_current_warning
};

} // ns attr
} // ns mss

#endif
