/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_common.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024,2025                        */
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
//------------------------------------------------------------------------------
/// @file  pz_rcs_common.H
/// @brief Common functions for RCS(Redundant clock switch)
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

using namespace fapi2;

namespace RCS_CONSTS
{

enum PZ_RCS_Private_Constants
{
    CTRL1_ENABLE_19P5_DLL = 10,
    CTRL1_LOCK_19P5_DLL_CODE = 11,
    CTRL1_EN_AUTO_BLOCK_SWITCHOVER = 21,
    CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER = 22,
    CTRL1_PPM_FASTA_ERR_INJ = 23,
    CTRL1_PPM_FASTB_ERR_INJ = 24,
    CTRL1_PPM_RESET = 25,
    CTRL1_PPM_OUTMUX_SEL0 = 26,
    CTRL1_PPM_OUTMUX_SEL1 = 27,
    CTRL1_PPM_OUTMUX_SEL2 = 28,
    CTRL1_PPM_INMUX_SEL0 = 29,
    CTRL1_PPM_INMUX_SEL1 = 30,
    CTRL1_FORCE_ERROR_HIGH = 31,

    SNS1LTH_19P5_ERROR_A = 17,
    SNS1LTH_19P5_ERROR_B = 18,
    SNS1LTH_19P5_CNT_0 = 25,
    SNS1LTH_19P5_CNT_1 = 26,

    SNS2LTH_19P5_CNT_3 = 14,
    SNS2LTH_19P5_CNT_4 = 24,
    SNS2LTH_19P5_CNT_5 = 25,
    SNS2LTH_19P5_CNT_6 = 26,
    SNS2LTH_PPM_FAIL = 27,
    SNS2LTH_PPM_FASTA_OUT = 28,
    SNS2LTH_PPM_FASTB_OUT = 29,
    SNS2LTH_PPM_FASTAB_ERR = 30,

    WAIT_20NS =       20,
    WAIT_1US  =     1000,
    WAIT_5US  =     5000,
    WAIT_10US =    10000,
    WAIT_1MS  =  1000000,
    WAIT_5MS  =  5000000,
    WAIT_20MS = 20000000,

    WAIT_1500CYC = 1500,
    WAIT_100KCYC =  100000,
    WAIT_5MCYC   = 5000000,
};
}

/// @brief Print the RCS registers
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode print_debug_info(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                            const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select);

/// @brief Set/Clear the RCS FIRs to mitigate risk of Millicode seeing a fake error due to async polling
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_set_mask         Bool for whether or not to set the FIRs or clear them
/// @return  FAPI2_RC_SUCCESS if success, else error code.
inline ReturnCode update_fir_mask(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                                  bool i_set_mask)
{
    FAPI_INF("Update RCS FIRs | Setting Mask %d", i_set_mask);

    // TPTOP.TPCHIP.TPC.LFIR
    constexpr uint64_t c_lfir_rw_clear_addr = 0x01040100;
    constexpr uint64_t c_lfir_mask_rw_clear_addr = 0x01040102;
    constexpr uint64_t c_lfir_mask_wo_or_addr = 0x01040103;
    constexpr uint8_t c_in57 = 57; // RCS - OSC0 Error (async) TYPE: RECOV CLEAR: only LFIR
    constexpr uint8_t c_in58 = 58; // RCS - OSC1 Error (async) TYPE: RECOV CLEAR: only LFIR

    fapi2::buffer<uint64_t> l_buffer = 0x0;

    l_buffer.setBit<c_in57>();
    l_buffer.setBit<c_in58>();

    if (i_set_mask)
    {
        // Set FIR Mask
        FAPI_TRY(putScom(i_target, c_lfir_mask_wo_or_addr, l_buffer)); // Mask WO WOR
    }
    else
    {
        // Clear the FIRs
        FAPI_TRY(putScom(i_target, c_lfir_rw_clear_addr, l_buffer));

        // Clear FIR Mask
        FAPI_TRY(putScom(i_target, c_lfir_mask_rw_clear_addr, l_buffer)); // Mask RW WClear
    }

fapi_try_exit:
    return current_err;
}

/// @brief Clear software inject errors before running HWPs
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode clear_rcs_injected_errs(const Target < TARGET_TYPE_PROC_CHIP >& i_target);

/// @brief Clear the write protection on CFAM regs before running other RCS operations
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   io_write_protect   Value to read out/write to from the write protect reg
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode clear_cfam_write_protect(const Target < TARGET_TYPE_PROC_CHIP >& i_target);

/// @brief
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode pz_rcs_pdown(const Target < TARGET_TYPE_PROC_CHIP >& i_target);

/// @brief Sweep through deskew values to set best value for both clocks
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_deskew_manual_cal(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                                 uint32_t i_osc_side_a);

/// @brief Switch RCS oscillators
/// @param[in]   i_target     Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_sw_switch(const Target < TARGET_TYPE_PROC_CHIP >& i_target);

/// @brief Validate RCS changes
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_verify_clean_state(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                                  const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select);

/// @brief Clear RCS errors and check them
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @param[in]   i_refclk_select    The current refclock being used
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_check_errors(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                            fapi2::ATTR_CP_REFCLOCK_SELECT_Type i_refclock_select);

/// @brief Lock FPLL A
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_lock_fplla( const Target < TARGET_TYPE_PROC_CHIP >& i_target);

/// @brief Lock FPLL B
/// @param[in]   i_target           Reference to TARGET_TYPE_PROC/HUB_CHIP
/// @return  FAPI2_RC_SUCCESS if success, else error code.
ReturnCode rcs_lock_fpllb( const Target < TARGET_TYPE_PROC_CHIP >& i_target);
