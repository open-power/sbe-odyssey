/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_common.C $   */
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
/// @file  pz_rcs_common.C
/// @brief Common functions for RCS(Redundant clock switch)
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_common.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_chip_clocking.H"
#include <poz_scom_perv.H>
#include <poz_perv_utils.H>



using namespace fapi2;
using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_RCS_CTRL1;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL3;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS1LTH;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS2LTH;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_GPWRP;

ReturnCode print_debug_info(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                            const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select)
{
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_GPWRP_t l_gpwrp;

    FAPI_TRY(l_sns1lth.getScom(i_target));
    FAPI_TRY(l_sns2lth.getScom(i_target));
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    FAPI_TRY(l_root_ctrl3.getScom(i_target));
    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    FAPI_TRY(l_gpwrp.getScom(i_target));

    FAPI_INF("REFCLKSEL: 0x%02X", i_refclk_select);
    FAPI_INF("GPWRP: 0x%08X", l_gpwrp);
    FAPI_INF("SNS1: 0x%08X", l_sns1lth);
    FAPI_INF("SNS2: 0x%08X", l_sns2lth);
    FAPI_INF("RCS CTRL1: 0x%08X", l_rcs_ctrl1);
    FAPI_INF("ROOT CTRL3: 0x%08X", l_root_ctrl3);
    FAPI_INF("ROOT CTRL5: 0x%08X", l_root_ctrl5);

fapi_try_exit:
    return current_err;
}


ReturnCode clear_rcs_injected_errs(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;

    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.clearBit<RCS_CONSTS::CTRL1_FORCE_ERROR_HIGH>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_SWO_FORCE_LOW>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));

fapi_try_exit:
    return current_err;
}


ReturnCode clear_cfam_write_protect(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    constexpr uint32_t c_val_write_protect = 0x4453FFFF;
    constexpr uint8_t c_start_write_protect = 0;
    constexpr uint8_t c_len_write_protect = 32;

    FSXCOMP_FSXLOG_GPWRP_t l_gpwrp;

    FAPI_TRY(l_gpwrp.getScom(i_target));
    l_gpwrp.insertFromRight<c_start_write_protect, c_len_write_protect>(c_val_write_protect);
    FAPI_TRY(l_gpwrp.putScom(i_target));

fapi_try_exit:
    return current_err;
}

ReturnCode pz_rcs_pdown(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    FAPI_INF("RCS Power Down");

    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;

    // RCS
    FAPI_TRY(l_root_ctrl5.getScom(i_target));

    // set RESET
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_RCS_RESET>();
    // set BYPASS
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_RCS_BYPASS>();
    // set CLK_ERR_A/B hi
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    // clear EN_REFCLK
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_EN_REFCLK>();
    // clear ASYNC_OUT
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_EN_ASYNC_OUT>();

    FAPI_TRY(l_root_ctrl5.putScom(i_target));

    // both PLLs
    FAPI_TRY(l_root_ctrl3.getScom(i_target));

    // set BYPASS hi
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_BYPASS_EN>();
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_BYPASS_EN>();
    // set test_en low
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_TEST_EN>();
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_TEST_EN>();
    // set RESET hi
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_RESET>();
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_RESET>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_deskew_manual_cal(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                                 uint32_t i_osc_side_a)
{
    FAPI_INF("RCS Manual Deskew Calibration on side %d", i_osc_side_a);

    constexpr uint32_t c_ERROR4DESKEW_A = 16;
    constexpr uint32_t c_ERROR4DESKEW_B = 24;
    constexpr uint32_t c_deskew_sel_a_bit = 0;
    constexpr uint32_t c_deskew_sel_b_bit = 5;
    constexpr uint32_t c_deskew_sel_len = 5;

    constexpr int32_t deskew2gray[] =
    {
        0b00000,
        0b10000,
        0b11000,
        0b01000,
        0b01100,
        0b11100,
        0b10100,
        0b00100,
        0b00110,
        0b10110,
        0b11110,
        0b01110,
        0b01010,
        0b11010,
        0b10010,
        0b00010,
        0b00011,
        0b10011,
        0b11011,
        0b01011,
        0b01111,
        0b11111,
        0b10111,
        0b00111,
        0b00101,
        0b10101,
        0b11101,
        0b01101,
        0b01001,
        0b11001,
        0b10001,
        0b00001
    };

    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;

    uint32_t l_deskew_err_bit = 0;
    int32_t l_deskew_gray = 0;
    int32_t l_current_val = -1;
    int32_t l_prev_val = -1;
    int32_t l_falling_edge = -1;
    int32_t l_rising_edge = -1;
    int32_t l_center = -1;

    // TOOD: remove, should alreayd be in this space
    // clear deskew en & lock
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.clearBit<12>(); // Deskew Auto En
    l_rcs_ctrl1.clearBit<13>(); // Deskew Auto Lock
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

    // Side A
    if (i_osc_side_a == 1)
    {
        l_deskew_err_bit = c_ERROR4DESKEW_A;
    }
    // Side B
    else
    {
        l_deskew_err_bit = c_ERROR4DESKEW_B;
    }

    for (int32_t l_deskew_val = 0; l_deskew_val < 32; ++l_deskew_val)
    {
        // Toggle clk error clears (both A/B)
        FAPI_TRY(l_root_ctrl5.getScom(i_target));
        l_root_ctrl5.setBit<6>();
        l_root_ctrl5.setBit<7>();
        FAPI_TRY(l_root_ctrl5.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
        l_root_ctrl5.clearBit<6>();
        l_root_ctrl5.clearBit<7>();
        FAPI_TRY(l_root_ctrl5.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_5US, RCS_CONSTS::WAIT_100KCYC);

        // write gray coded deskew value
        FAPI_TRY(l_rcs_ctrl1.getScom(i_target));

        l_deskew_gray = deskew2gray[l_deskew_val];

        if (i_osc_side_a == 1)
        {
            l_rcs_ctrl1.insertFromRight<c_deskew_sel_a_bit, c_deskew_sel_len>(l_deskew_gray);
        }
        else
        {
            l_rcs_ctrl1.insertFromRight<c_deskew_sel_b_bit, c_deskew_sel_len>(l_deskew_gray);
        }

        FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

        // read deskew error
        FAPI_TRY(l_sns1lth.getScom(i_target));
        l_current_val = l_sns1lth.getBit(l_deskew_err_bit);

        // Check for falling edge
        if ((l_prev_val == 1) && (l_current_val == 0))
        {
            l_falling_edge = l_deskew_val;
        }
        // Check for rising edge
        else if ((l_falling_edge > -1) && (l_prev_val == 0) && (l_current_val == 1))
        {
            l_rising_edge = l_deskew_val;
        }

        // TODO: Min window size check
        // TODO: Find all windows/centers & analyze best later
        // Calc the center value
        if ((l_falling_edge > -1) && (l_rising_edge > -1))
        {
            l_center = (l_falling_edge + l_rising_edge) / 2;
            break;
        }

        // Set prev val
        l_prev_val = l_current_val;
    }

    // corner cases
    if (l_center == -1)
    {
        // no edges found
        if ((l_falling_edge == -1) && (l_rising_edge == -1))
        {
            FAPI_ERR("RCS No edges found");
            l_center = 15;
        }
        // rising edge found, but not falling edge
        else if ((l_falling_edge == -1) && (l_rising_edge > -1))
        {
            FAPI_ERR("RCS No falling edge found");
            l_center = l_rising_edge / 2;
        }
        // falling edge found, but not rising edge
        else if ((l_falling_edge > -1) && (l_rising_edge == -1))
        {
            FAPI_ERR("RCS No rising edge found");
            l_center = (l_falling_edge + 31) / 2;
        }
    }

    // Bound checking
    if ((l_center > 31) || (l_center < 0))
    {
        FAPI_ERR("RCS Center value (%d) out of bounds");
        l_center = 15;
    }

    // write gray coded center deskew value
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));

    FAPI_INF("RCS Left(%d) Right(%d) Center value (%d)", l_falling_edge, l_rising_edge, l_center);
    l_deskew_gray = deskew2gray[l_center];

    if (i_osc_side_a)
    {
        l_rcs_ctrl1.insertFromRight<c_deskew_sel_a_bit, c_deskew_sel_len>(l_deskew_gray);
    }
    else
    {
        l_rcs_ctrl1.insertFromRight<c_deskew_sel_b_bit, c_deskew_sel_len>(l_deskew_gray);
    }

    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_verify_clean_state(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                                  const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select)
{
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    FAPI_TRY(l_sns2lth.getScom(i_target));

    if ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0)
    {
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_A() == 1 && l_sns2lth.get_MUXSEL_CLK_B() == 0,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify Expected A but not on correct side.");
    }

    if ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_A() == 0 && l_sns2lth.get_MUXSEL_CLK_B() == 1,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify Expected A but not on correct side.");
    }

    FAPI_TRY(l_sns1lth.getScom(i_target));

    FAPI_ASSERT(l_sns1lth.get_SWITCHED() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Verify SW Switch: Switched Initial Error.");

    FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_A() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Clk A Error.");

    FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_B() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Clk B Error.");

    // FAPI_ASSERT(l_sns1lth.getBit(RCS_CONSTS::SNS1LTH_19P5_ERROR_A) == 0,
    //             fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
    //             "RCS 19.5ps Error A.");

    // FAPI_ASSERT(l_sns1lth.getBit(RCS_CONSTS::SNS1LTH_19P5_ERROR_B) == 0,
    //             fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
    //             "RCS 19.5ps Error B.");

    // Do not need to check these, but would like to monitor them for debug
    FAPI_INF("RCS Unlock Detect A %d | B %d", l_sns1lth.get_UNLOCKDET_A(), l_sns1lth.get_UNLOCKDET_B());

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_sw_switch(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;
    bool l_expect_clka = false;

    FAPI_TRY(l_sns2lth.getScom(i_target));
    l_expect_clka = l_sns2lth.get_MUXSEL_CLK_A() == 1;
    FAPI_INF("Expect CLK A = %d", l_expect_clka);

    FAPI_TRY(l_root_ctrl5.getScom(i_target));

    // Force Switchover
    if (l_expect_clka)
    {
        FAPI_INF("Force CLK SEL 0");
        l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_RCS_FORCE_CLKSEL>();
    }
    else
    {
        FAPI_INF("Force CLK SEL 1");
        l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_RCS_FORCE_CLKSEL>();
    }

    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_SWO_FORCE_LOW>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_SWO_FORCE_LOW>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    FAPI_TRY(l_sns1lth.getScom(i_target));
    FAPI_TRY(l_sns2lth.getScom(i_target));

    if (l_expect_clka)
    {
        FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_A() == 1,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Clk A Error after force.");
        FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_B() == 0,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Clk B Error after force.");
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_A() == 0,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Muxsel Clk A Error after force.");
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_B() == 1,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Muxsel Clk B Error after force.");
    }
    else
    {
        FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_A() == 0,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Clk A Error after force.");
        FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_B() == 1,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Clk B Error after force.");
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_A() == 1,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Muxsel Clk A Error after force.");
        FAPI_ASSERT(l_sns2lth.get_MUXSEL_CLK_B() == 0,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Verify SW Switch: Muxsel Clk B Error after force.");
    }

    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_RCS_FORCE_CLKSEL>();
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);


    // Clear the Auto Block Switchover Signal
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.setBit<RCS_CONSTS::CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    l_rcs_ctrl1.clearBit<RCS_CONSTS::CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

fapi_try_exit:
    return current_err;
}


ReturnCode rcs_check_errors(const Target < TARGET_TYPE_PROC_CHIP >& i_target,
                            fapi2::ATTR_CP_REFCLOCK_SELECT_Type i_refclock_select)
{
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    // Clear Errors -- Wait for Errors to propagate
    FAPI_INF("RCS Clearing Errors.");
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    // If adding an OSC back in
    if (i_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0)
    {
        FAPI_INF("Adding OSC, clearing both CLEAR_CLK_ERROR_A/B");
        l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
        l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    }
    // If removing an OSC, only clear the error on the primary side
    else if (i_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        FAPI_INF("Removing OSC, clearing CLEAR_CLK_ERROR_B");
        l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    }
    else
    {
        FAPI_INF("Removing OSC, clearing CLEAR_CLK_ERROR_A");
        l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    }

    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    // Check RCS Errors
    FAPI_INF("RCS Checking Errors.");
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    FAPI_TRY(l_sns1lth.getScom(i_target));

    if (l_sns1lth.get_CLK_ERROR_A())
    {
        // Should this be a FAPI_ASSERT and bomb out?
        FAPI_ERR("RCS CLK Error A did not clear");
        goto fapi_try_exit;
    }

    if (l_sns1lth.get_CLK_ERROR_B())
    {
        FAPI_ERR("RCS CLK Error B did not clear");
        goto fapi_try_exit;
    }

fapi_try_exit:
    return current_err;
}


ReturnCode rcs_lock_fplla(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    FAPI_TRY(l_root_ctrl3.getScom(i_target));

    FAPI_INF("Driving RCS FPLLA from OSC1 (Alt Refclk)");
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_ALTREF_SEL>(); // Drive RCS FPLL1 with Alt Refclk from OSC1
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_TEST_EN>();
    // Release Reset, allow the FPLLs to attempt to lock
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_RESET>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_RESET>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_5MS, RCS_CONSTS::WAIT_100KCYC);

    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWA));
    // Now that the FPLLs are locked, release bypass
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_BYPASS_EN>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_lock_fpllb(const Target < TARGET_TYPE_PROC_CHIP >& i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    FAPI_TRY(l_root_ctrl3.getScom(i_target));

    FAPI_INF("Driving RCS FPLLB from OSC0 (Alt Refclk)");
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_ALTREF_SEL>(); // Drive RCS FPLL2 with Alt Refclk from OSC0
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_TEST_EN>();
    // Release Reset, allow the FPLLs to attempt to lock
    l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_RESET>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_RESET>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_5MS, RCS_CONSTS::WAIT_100KCYC);

    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWB));
    // Now that the FPLLs are locked, release bypass
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_BYPASS_EN>();

    FAPI_TRY(l_root_ctrl3.putScom(i_target));

fapi_try_exit:
    return current_err;
}
