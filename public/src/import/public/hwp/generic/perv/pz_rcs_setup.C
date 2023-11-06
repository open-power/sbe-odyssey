/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_setup.C $    */
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
//------------------------------------------------------------------------------
/// @brief
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP FW Maintainer   : Oliver Morlok (marquaro@in.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_setup.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_chip_clocking.H"
#include <poz_scom_perv.H>
#include <poz_perv_utils.H>


using namespace fapi2;
using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_RCS_CTRL1;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL3;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS1LTH;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS2LTH;

enum PZ_RCS_SETUP_Private_Constants
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
    WAIT_10US =    10000,
    WAIT_1MS  =  1000000,
    WAIT_5MS  =  5000000,
    WAIT_20MS = 20000000,
    WAIT_1500CYC = 1500,

    WAIT_100KCYC =  100000,
    WAIT_5MCYC   = 5000000,
};

static ReturnCode rcs_verify_clean_state(const Target<TARGET_TYPE_PROC_CHIP>& i_target,
        const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select)
{
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    l_sns2lth.getScom(i_target);

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

    l_sns1lth.getScom(i_target);

    FAPI_ASSERT(l_sns1lth.get_SWITCHED() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Verify SW Switch: Switched Initial Error.");

    FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_A() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Clk A Error.");

    FAPI_ASSERT(l_sns1lth.get_CLK_ERROR_B() == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Clk B Error.");

    FAPI_ASSERT(l_sns1lth.getBit(SNS1LTH_19P5_ERROR_A) == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS 19.5ps Error A.");

    FAPI_ASSERT(l_sns1lth.getBit(SNS1LTH_19P5_ERROR_B) == 0,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS 19.5ps Error B.");

    // Do not need to check these, but would like to monitor them for debug
    FAPI_INF("RCS Unlock A Error %d", l_sns1lth.get_UNLOCKDET_A());
    FAPI_INF("RCS Unlock B Error %d", l_sns1lth.get_UNLOCKDET_B());

fapi_try_exit:
    return current_err;
}

static ReturnCode rcs_ppm_watchdog_test(const Target<TARGET_TYPE_PROC_CHIP>& i_target)
{

    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    // Release Reset
    l_rcs_ctrl1.getScom(i_target);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_RESET);
    l_rcs_ctrl1.putScom(i_target);
    fapi2::delay(WAIT_10US, WAIT_1500CYC);

    // l_sns2lth.getScom(i_target);
    // FAPI_ASSERT(((l_sns2lth.getBit(SNS2LTH_PPM_FASTA_OUT) == 1) | (l_sns2lth.getBit(SNS2LTH_PPM_FASTB_OUT) == 1)),
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog First Check Failed.");


    // // Inject FAST A
    // l_rcs_ctrl1.writeBit(1, CTRL1_PPM_FASTA_ERR_INJ);
    // l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTB_ERR_INJ);
    // l_rcs_ctrl1.putScom(i_target);
    // fapi2::delay(WAIT_10US, WAIT_1500CYC);
    // l_sns2lth.getScom(i_target);
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTA_OUT) == 1,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Inject A -- FASTA Failed.");
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTB_OUT) == 0,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Inject A -- FASTB Failed.");

    // // Inject FAST B
    // l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTA_ERR_INJ);
    // l_rcs_ctrl1.writeBit(1, CTRL1_PPM_FASTB_ERR_INJ);
    // l_rcs_ctrl1.putScom(i_target);
    // fapi2::delay(WAIT_10US, WAIT_1500CYC);
    // l_sns2lth.getScom(i_target);
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTA_OUT) == 0,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Inject B -- FASTA Failed.");
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTB_OUT) == 1,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Inject B -- FASTB Failed.");


    // Setup Functional Mode
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTA_ERR_INJ);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTB_ERR_INJ);
    l_rcs_ctrl1.putScom(i_target);
    fapi2::delay(WAIT_10US, WAIT_1500CYC);
    l_sns2lth.getScom(i_target);
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTA_OUT) == 0,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Func Mode -- FASTA Failed.");
    // FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FASTB_OUT) == 0,
    //             fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
    //             "RCS PPM Watchdog Func Mode -- FASTB Failed.");
    FAPI_ASSERT(l_sns2lth.getBit(SNS2LTH_PPM_FAIL) == 0,
                fapi2::POZ_PPM_ERROR().set_PROC_TARGET(i_target),
                "RCS PPM Watchdog Func Mode -- PPM Failed.");

fapi_try_exit:
    return current_err;
}

static ReturnCode rcs_simple_clock_test(
    const Target<TARGET_TYPE_PROC_CHIP>& i_target,
    const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select,
    const uint32_t i_test)
{
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;

    // Simple Clock Test 0/1 -- Expect 1s/0s
    l_root_ctrl5.getScom(i_target);
    l_root_ctrl5.set_RCS_CLK_TEST_IN(i_test);
    l_root_ctrl5.putScom(i_target);

    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_sns1lth.getScom(i_target);

    if ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0) ||
        ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0))
    {
        FAPI_ASSERT(l_sns1lth.get_REFCLK_DATA_OUT_A() == i_test,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Simple Clock A Check %d Failed.", i_test);
    }

    if ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0) ||
        ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1))
    {
        FAPI_ASSERT(l_sns1lth.get_REFCLK_DATA_OUT_B() == i_test,
                    fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                    "RCS Simple Clock B Check %d Failed.", i_test);
    }

    l_root_ctrl5.set_RCS_CLK_TEST_IN(0);
    l_root_ctrl5.putScom(i_target);

fapi_try_exit:
    return current_err;
}

static ReturnCode rcs_lock_fplls(
    const Target<TARGET_TYPE_PROC_CHIP>& i_target,
    const fapi2::ATTR_CP_REFCLOCK_SELECT_Type& i_refclk_select)
{
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    l_root_ctrl3.getScom(i_target);
    l_root_ctrl3.set_PLLCLKSW1_ALTREF_SEL(0);
    l_root_ctrl3.set_PLLCLKSW2_ALTREF_SEL(0);

    if ((i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0) == 0x0)
    {
        if (i_refclk_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
        {
            FAPI_INF("Driving RCS FPLLA from OSC1 (Alt Refclk)");
            l_root_ctrl3.set_PLLCLKSW1_ALTREF_SEL(1); // Drive RCS FPLL1 with Alt Refclk from OSC1
        }
        else
        {
            FAPI_INF("Driving RCS FPLLB from OSC0 (Alt Refclk)");
            l_root_ctrl3.set_PLLCLKSW2_ALTREF_SEL(1); // Drive RCS FPLL2 with Alt Refclk from OSC0
        }
    }

    l_root_ctrl3.set_PLLCLKSW1_TEST_EN(0);
    l_root_ctrl3.set_PLLCLKSW2_TEST_EN(0);

    // Release Reset, allow the FPLLs to attempt to lock
    l_root_ctrl3.set_PLLCLKSW1_RESET(1);
    l_root_ctrl3.set_PLLCLKSW2_RESET(1);
    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl3.set_PLLCLKSW1_RESET(0);
    l_root_ctrl3.set_PLLCLKSW2_RESET(0);
    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_5MS, WAIT_100KCYC);

    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWA));
    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWB));

    // Now that the FPLLs are locked, release bypass
    l_root_ctrl3.set_PLLCLKSW1_BYPASS_EN(0);
    l_root_ctrl3.set_PLLCLKSW2_BYPASS_EN(0);
    l_root_ctrl3.putScom(i_target);
fapi_try_exit:
    return current_err;
}


static ReturnCode rcs_sw_switch(const Target<TARGET_TYPE_PROC_CHIP>& i_target)
{
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;
    bool l_expect_clka = false;

    l_sns2lth.getScom(i_target);
    l_expect_clka = l_sns2lth.get_MUXSEL_CLK_A() == 1;

    l_root_ctrl5.getScom(i_target);

    // Force Switchover
    if (l_expect_clka)
    {
        l_root_ctrl5.set_RCS_FORCE_CLKSEL(0);
    }
    else
    {
        l_root_ctrl5.set_RCS_FORCE_CLKSEL(1);
    }

    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_SWO_FORCE_LOW(1);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_SWO_FORCE_LOW(0);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_sns1lth.getScom(i_target);
    l_sns2lth.getScom(i_target);
    FAPI_ASSERT(l_sns1lth.get_SWITCHED() == 1,
                fapi2::POZ_RCS_ERROR().set_PROC_TARGET(i_target),
                "RCS Verify SW Switch: Switched Error after force low.");

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

    l_root_ctrl5.set_RCS_FORCE_CLKSEL(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(1);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(1);
    l_root_ctrl5.putScom(i_target);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    l_root_ctrl5.putScom(i_target);


    // Clear the Auto Block Switchover Signal
    l_rcs_ctrl1.getScom(i_target);
    l_rcs_ctrl1.writeBit(1, CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.putScom(i_target);
    l_rcs_ctrl1.writeBit(0, CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.putScom(i_target);

fapi_try_exit:
    return current_err;
}



// Pre-requisite: RCS Bypass / Clksel are setup for the correct clock
//   if redundant clocks are not needed
ReturnCode pz_rcs_setup(const Target<TARGET_TYPE_PROC_CHIP>& i_target)
{
    FAPI_INF("RCS Entering...");
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL0_t l_root_ctrl0;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    fapi2::ATTR_CP_REFCLOCK_SELECT_Type l_refclock_select;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_refclock_select));

    l_root_ctrl0.set_CFAM_PROTECTION_0(1);
    l_root_ctrl0.putScom_CLEAR(i_target);

    // Even if the RCS is disabled, we still want to provide a clock to the PLLs
    //   for end of life.
    l_root_ctrl5 = 0;
    l_root_ctrl5.set_RCS_RESET(1); // Should be in reset already, but to be sure
    l_root_ctrl5.set_RCS_BYPASS(1); // Should be in bypass already, but to be sure
    l_root_ctrl5.set_RCS_FORCE_CLKSEL(0);
    l_root_ctrl5.set_RCS_CLK_TEST_IN(0);
    l_root_ctrl5.set_SWO_FORCE_LOW(0);
    l_root_ctrl5.set_BLOCK_SWO(1); // Block Switchovers before we release reset
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    l_root_ctrl5.set_MUXCTL_CLK_CHKSW(1); // Must be high for typical operation
    l_root_ctrl5.set_FILT(0);
    l_root_ctrl5.set_JUMP_SEL(3); // 19.5ps mode
    l_root_ctrl5.set_SEL_RES(2);
    l_root_ctrl5.set_EN_RISE_EARLY_DETECT(1);
    l_root_ctrl5.set_EN_RISE_LATE_DETECT(1);
    l_root_ctrl5.set_EN_GLITCH_DETECT(1);
    l_root_ctrl5.set_OVRBIT(0);
    l_root_ctrl5.set_EN_REFCLK(1);
    l_root_ctrl5.set_EN_ASYNC_OUT(0);
    l_root_ctrl5.putScom(i_target);

    l_rcs_ctrl1 = 0;
    l_rcs_ctrl1.set_DESKEW_SEL_A(0);
    l_rcs_ctrl1.set_DESKEW_SEL_B(0);
    l_rcs_ctrl1.writeBit(1, CTRL1_ENABLE_19P5_DLL);
    l_rcs_ctrl1.writeBit(0, CTRL1_LOCK_19P5_DLL_CODE);
    l_rcs_ctrl1.set_DESKEW_AUTO_EN(1);
    l_rcs_ctrl1.set_DESKEW_AUTO_LOCK(0);
    l_rcs_ctrl1.set_DESKEW_AUTO_FILT(0);
    l_rcs_ctrl1.set_TESTOUT_SEL(0);
    l_rcs_ctrl1.set_TESTOUT_EN(0);
    l_rcs_ctrl1.writeBit(0, CTRL1_EN_AUTO_BLOCK_SWITCHOVER); // Leaving this at zero for now until a later point
    l_rcs_ctrl1.writeBit(0, CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTA_ERR_INJ);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_FASTB_ERR_INJ);
    l_rcs_ctrl1.writeBit(1, CTRL1_PPM_RESET);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_OUTMUX_SEL0);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_OUTMUX_SEL1);
    l_rcs_ctrl1.writeBit(1, CTRL1_PPM_OUTMUX_SEL2);
    l_rcs_ctrl1.writeBit(0, CTRL1_PPM_INMUX_SEL0);
    l_rcs_ctrl1.writeBit(1, CTRL1_PPM_INMUX_SEL1);
    l_rcs_ctrl1.writeBit(0, CTRL1_FORCE_ERROR_HIGH);
    l_rcs_ctrl1.putScom(i_target);

    // Simple Clock Check
    FAPI_INF("RCS Simple Clock Check.");
    FAPI_TRY(rcs_simple_clock_test(i_target, l_refclock_select, 0));
    FAPI_TRY(rcs_simple_clock_test(i_target, l_refclock_select, 1));

    // Lock RCS FPLLs
    FAPI_INF("RCS Lock Filter PLLs.");
    FAPI_TRY(rcs_lock_fplls(i_target, l_refclock_select));

    // Wait for good clocks to propagate
    fapi2::delay(WAIT_1US, WAIT_100KCYC); // Need the RCS Reset to high for at least 1uS

    if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0) == 0x0)
    {
        // - To detect errors, we would need bypass low and out of reset
        // - We need to be out of bypass in case at a future point, we would want to enable redundancy and not see a large phase jump
        l_root_ctrl5.set_RCS_RESET(0);
        l_root_ctrl5.putScom(i_target);
        fapi2::delay(WAIT_1US, WAIT_100KCYC); // Need the RCS Reset to high for at least 1uS

        l_root_ctrl5.set_RCS_BYPASS(0);
        l_root_ctrl5.putScom(i_target);
        goto fapi_try_exit;
    }

    // Release RCS Reset
    FAPI_INF("RCS Release Reset");
    l_root_ctrl5.set_RCS_RESET(0);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    // Deskew Calibration
    l_rcs_ctrl1.set_DESKEW_AUTO_LOCK(1);
    l_rcs_ctrl1.putScom(i_target);
    l_sns1lth.getScom(i_target);
    FAPI_INF("RCS Auto Deskew A %d.", l_sns1lth.get_DESKEW_QOUT_A());
    FAPI_INF("RCS Auto Deskew B %d.", l_sns1lth.get_DESKEW_QOUT_B());

    // Lock the 19.5ps DLL
    FAPI_INF("RCS Locking 19.5ps DLL.");
    fapi2::delay(WAIT_1MS, WAIT_100KCYC); // Need at least 1ms of stability for the DLL to lock
    l_rcs_ctrl1.writeBit(1, CTRL1_LOCK_19P5_DLL_CODE);
    l_rcs_ctrl1.putScom(i_target);

    // Release RCS Bypass
    FAPI_INF("RCS Release Bypass.");
    l_root_ctrl5.set_RCS_BYPASS(0);
    l_root_ctrl5.putScom(i_target);

    // Clear Errors -- Wait for Errors to propagate
    FAPI_INF("RCS Clearing Errors.");
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(1);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(1);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    FAPI_INF("RCS Set BLOCK_SWO = 0.");
    l_root_ctrl5.set_BLOCK_SWO(0);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    // Clear the Auto Block Switchover Signal
    FAPI_INF("RCS Clearing Block Switchover.");
    l_rcs_ctrl1.getScom(i_target);
    l_rcs_ctrl1.writeBit(1, CTRL1_EN_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.writeBit(1, CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_rcs_ctrl1.writeBit(0, CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER);
    l_rcs_ctrl1.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_sns1lth.getScom(i_target);
    FAPI_INF("RCS Sense1 Register(0x2944): 0x%08X.", l_sns1lth);
    l_sns2lth.getScom(i_target);
    FAPI_INF("RCS Sense2 Register(0x2945): 0x%08X.", l_sns2lth);



    FAPI_INF("RCS Verifying Correct Side.");
    l_sns2lth.getScom(i_target);

    if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0)
    {
        if (l_sns2lth.get_MUXSEL_CLK_B() == 1)
        {
            FAPI_INF("RCS Needs to be on OSC0 but on Side B. Forcing Switchover.");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
    }

    if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        if (l_sns2lth.get_MUXSEL_CLK_A() == 1)
        {
            FAPI_INF("RCS Needs to be on OSC1 but on Side A. Forcing Switchover.");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
    }

    // Verify Clean State
    FAPI_INF("Verifying Clean State.");
    FAPI_TRY(rcs_verify_clean_state(i_target, l_refclock_select));

    // Clear Errors -- Wait for Errors to propagate
    FAPI_INF("Clearing Errors.");
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(1);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(1);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    l_root_ctrl5.putScom(i_target);

    // Verify Clean State
    FAPI_INF("Verifying Clean State.");
    FAPI_TRY(rcs_verify_clean_state(i_target, l_refclock_select));

    // Software Switch to Verify
    FAPI_INF("First Switchover Test.");
    FAPI_TRY(rcs_sw_switch(i_target)); // Switch to backup Osc
    FAPI_INF("Second Switchover Test.");
    FAPI_TRY(rcs_sw_switch(i_target)); // Switch basck to normal Osc

    // PPM Watchdog Check
    FAPI_INF("PPM Watchdog Test.");
    FAPI_TRY(rcs_ppm_watchdog_test(i_target));

    // Simple Clock Check, 2nd check is intentional in case clocks died during RCS cal
    FAPI_INF("First Simple Clock Test.");
    FAPI_TRY(rcs_simple_clock_test(i_target, l_refclock_select, 0));
    FAPI_INF("Second Simple Clock Test.");
    FAPI_TRY(rcs_simple_clock_test(i_target, l_refclock_select, 1));

    // Check that all errors are clean
    FAPI_INF("Last Clean State Check.");
    FAPI_TRY(rcs_verify_clean_state(i_target, l_refclock_select));

fapi_try_exit:
    FAPI_INF("RCS Exiting...");
    return current_err;
}
