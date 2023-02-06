/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_ppe_other_regs.H $ */
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
/// @file  poz_ppe_other_regs.H
/// @brief Dump a PPE engine's state.
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 2
/// *HWP Consumed by     : SBE, Cronus

#pragma once

#include <poz_ppe_common.H>
#include <vector>

/// @typedef poz_ppe_other_regs_FP_t
/// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*poz_ppe_other_regs_FP_t) (
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >&,
    const enum PPE_TYPES i_ppe_type,
    const uint32_t i_ppe_inst_num,
    std::vector<Reg64Value_t>& v_other_reg_value);

extern "C"
{

/// @brief Dump the PPE Instance Specific registers
/// @param [in] i_target fapi target type for chip
/// @param [in] i_ppe_type which ppe type in the system
/// @param [in] i_ppe_instance_num if GPE/CE which instance number of it
/// @param [in] v_other_reg_value dumpped instance specific register values of ppe
/// @return FAPI2_RC_SUCCESS
    fapi2::ReturnCode
    poz_ppe_other_regs(const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target,
                       const enum PPE_TYPES i_ppe_type,
                       const uint32_t i_ppe_inst_num,
                       std::vector<Reg64Value_t>& v_other_reg_value);

} // extern C
