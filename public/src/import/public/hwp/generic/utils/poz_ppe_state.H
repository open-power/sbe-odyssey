/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_ppe_state.H $  */
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
/// @file  poz_ppe_state.H
/// @brief Dump a PPE engine's state.
///
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 2
/// *HWP Consumed by     : SBE, Cronus

#pragma once

#include <poz_ppe_common.H>
#include <vector>

enum PPE_DUMP_MODE
{
    NONE           = 0x0,
    XIRS           = 0x4,
    SNAPSHOT       = 0x1,
    HALT           = 0x2,
    FORCE_HALT     = 0x3
};
enum VERBOSE_MODE
{
    NOVERBOSE        = 0x0,
    VERBOSE          = 0x1,
    VERBOSEP         = 0x2,
    VERBOSE1         = 0x3,

};

/// @typedef poz_ppe_state_FP_t
/// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*poz_ppe_state_FP_t) (
    const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >&,
    enum PPE_TYPES i_ppe_type, uint32_t i_ppe_instance_num, const PPE_DUMP_MODE,
    std::vector<Reg32Value_t>& v_ppe_gprs_value,
    std::vector<Reg32Value_t>& v_ppe_sprs_value,
    std::vector<Reg32Value_t>& v_ppe_xirs_value);

extern "C"
{

/// @brief Dump the PPE state based on the based base address default for each ppe type
/// @param [in] i_target fapi target type for chip
/// @param [in] i_ppe_type which ppe type in the system
/// @param [in] i_ppe_instance_num if GPE/CE which instance number of it
/// @param [in] i_mode different dump and display mode
/// @param [in] v_ppe_gprs_value dumpped gpr values of pee
/// @param [in] v_ppe_sprs_value dumpped spr values of ppe
/// @param [in] v_ppe_xirs_value dumpped xir values of ppe
/// @return FAPI2_RC_SUCCESS
    fapi2::ReturnCode
    poz_ppe_state(const fapi2::Target < fapi2::TARGET_TYPE_ANY_POZ_CHIP >& i_target,
                  enum PPE_TYPES i_ppe_type, uint32_t i_ppe_instance_num, const PPE_DUMP_MODE i_mode,
                  std::vector<Reg32Value_t>& v_ppe_gprs_value,
                  std::vector<Reg32Value_t>& v_ppe_sprs_value,
                  std::vector<Reg32Value_t>& v_ppe_xirs_value);

} // extern C
