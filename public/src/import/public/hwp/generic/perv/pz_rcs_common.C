/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_common.C $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
/// @brief Lock RCS(Redundant clock switch) FPLLs(pllclksw1/pllclksw2)
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP FW Maintainer   : Oliver Morlok (marquaro@in.ibm.com)
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

enum PZ_RCS_SETUP_Private_Constants
{
    CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER = 22,

    SNS1LTH_19P5_ERROR_A = 17,
    SNS1LTH_19P5_ERROR_B = 18,

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

ReturnCode rcs_verify_clean_state(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target,
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
    FAPI_INF("RCS Unlock Detect A %d | B %d", l_sns1lth.get_UNLOCKDET_A(), l_sns1lth.get_UNLOCKDET_B());

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_sw_switch(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;
    bool l_expect_clka = false;

    l_sns2lth.getScom(i_target);
    l_expect_clka = l_sns2lth.get_MUXSEL_CLK_A() == 1;
    FAPI_INF("Expect CLK A %d", l_expect_clka);

    l_root_ctrl5.getScom(i_target);

    // Force Switchover
    if (l_expect_clka)
    {
        FAPI_INF("Force CLK SEL 0");
        l_root_ctrl5.set_RCS_FORCE_CLKSEL(0);
    }
    else
    {
        FAPI_INF("Force CLK SEL 1");
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
    FAPI_INF("SWITCHED %d", l_sns1lth.get_SWITCHED());
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
    l_rcs_ctrl1.setBit<CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER>();
    l_rcs_ctrl1.putScom(i_target);
    l_rcs_ctrl1.clearBit<CTRL1_CLEAR_AUTO_BLOCK_SWITCHOVER>();
    l_rcs_ctrl1.putScom(i_target);

fapi_try_exit:
    return current_err;
}


ReturnCode rcs_check_errors(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target,
                            fapi2::ATTR_CP_REFCLOCK_SELECT_Type i_refclock_select)
{
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    // Clear Errors -- Wait for Errors to propagate
    FAPI_INF("RCS Clearing Errors.");
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.getScom(i_target);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(1);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(1);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    // If adding an OSC back in
    if (i_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0)
    {
        l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
        l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    }
    // If removing an OSC, only clear the error on the primary side
    else if (i_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    }
    else
    {
        l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    }

    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    // Check RCS Errors
    FAPI_INF("RCS Checking Errors.");
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_sns1lth.getScom(i_target);

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

    // Validate side
    FAPI_TRY(rcs_verify_clean_state(i_target, i_refclock_select));
fapi_try_exit:
    return current_err;
}


ReturnCode rcs_lock_fplla(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    l_root_ctrl3.getScom(i_target);

    FAPI_INF("Driving RCS FPLLA from OSC1 (Alt Refclk)");
    l_root_ctrl3.set_PLLCLKSW1_ALTREF_SEL(1); // Drive RCS FPLL1 with Alt Refclk from OSC1
    l_root_ctrl3.set_PLLCLKSW1_TEST_EN(0);
    // Release Reset, allow the FPLLs to attempt to lock
    l_root_ctrl3.set_PLLCLKSW1_RESET(1);

    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_root_ctrl3.set_PLLCLKSW1_RESET(0);

    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_5MS, WAIT_100KCYC);

    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWA));
    // Now that the FPLLs are locked, release bypass
    l_root_ctrl3.set_PLLCLKSW1_BYPASS_EN(0);

    l_root_ctrl3.putScom(i_target);

fapi_try_exit:
    return current_err;
}

ReturnCode rcs_lock_fpllb(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    l_root_ctrl3.getScom(i_target);

    FAPI_INF("Driving RCS FPLLB from OSC0 (Alt Refclk)");
    l_root_ctrl3.set_PLLCLKSW2_ALTREF_SEL(1); // Drive RCS FPLL2 with Alt Refclk from OSC0
    l_root_ctrl3.set_PLLCLKSW2_TEST_EN(0);
    // Release Reset, allow the FPLLs to attempt to lock
    l_root_ctrl3.set_PLLCLKSW2_RESET(1);

    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_root_ctrl3.set_PLLCLKSW2_RESET(0);

    l_root_ctrl3.putScom(i_target);
    fapi2::delay(WAIT_5MS, WAIT_100KCYC);

    FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWB));
    // Now that the FPLLs are locked, release bypass
    l_root_ctrl3.set_PLLCLKSW2_BYPASS_EN(0);

    l_root_ctrl3.putScom(i_target);

fapi_try_exit:
    return current_err;
}
