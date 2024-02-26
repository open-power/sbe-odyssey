/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_add.C $      */
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
/// @file  pz_rcs_add.C
/// @brief Lock RCS(Redundant clock switch) FPLLs(pllclksw1/pllclksw2)
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP FW Maintainer   : Oliver Morlok (marquaro@in.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_add.H"
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
    CTRL1_BLOCK_SWO_AUTO = 21,
    CTRL1_CLR_BLOCK_SWO_AUTO = 22,
    ROOT_CTRL5_BLOCK_SWO = 5,

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

// Pre-requisite: RCS Bypass / Clksel are setup for the correct clock
//   if redundant clocks are not needed
ReturnCode pz_rcs_add(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FAPI_INF("Start RCS Add");

    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    // Check ATTR for which OSC
    fapi2::ATTR_CP_REFCLOCK_SELECT_Type l_refclock_select = 0;
    fapi2::ATTR_CHIP_EC_FEATURE_EXTERNAL_PCIE_REFCLOCK_ON_SECONDARY_WHILE_AB_XB_MC_FROM_PRIMARY_CHIP_Type
    l_pcie_fpll_mux_fix;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_refclock_select));
    FAPI_TRY(FAPI_ATTR_GET_PRIVILEGED(
                 fapi2::ATTR_CHIP_EC_FEATURE_EXTERNAL_PCIE_REFCLOCK_ON_SECONDARY_WHILE_AB_XB_MC_FROM_PRIMARY_CHIP,
                 i_target, l_pcie_fpll_mux_fix));

    // Check if DD2 & Die 2 because RCS on Die 2 is disabled
    if (l_pcie_fpll_mux_fix)
    {
        FAPI_INF("DD2 & Die 2, skipping");
        goto fapi_try_exit;
    }

    // Sanity check
    if (!(l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0
          || l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1))
    {
        FAPI_INF("Wrong RCS Add provided (%d) skipping", l_refclock_select);
        goto fapi_try_exit;
    }

    l_rcs_ctrl1.getScom(i_target);
    l_root_ctrl5.getScom(i_target);

    FAPI_INF("Unblocking switchover");
    // Unblock switchover
    l_rcs_ctrl1.getScom(i_target);
    l_rcs_ctrl1.clearBit<CTRL1_BLOCK_SWO_AUTO>();
    l_rcs_ctrl1.putScom(i_target);
    l_root_ctrl5.getScom(i_target);
    l_root_ctrl5.clearBit<ROOT_CTRL5_BLOCK_SWO>();
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_rcs_ctrl1.getScom(i_target);
    l_root_ctrl5.getScom(i_target);
    FAPI_INF("BLOCK_SWO_AUTO %d.", l_rcs_ctrl1.getBit<CTRL1_BLOCK_SWO_AUTO>());
    FAPI_INF("BLOCK_SWO %d.", l_root_ctrl5.get_BLOCK_SWO());

    // Check Alt ref clk for FPLLs
    l_root_ctrl3.getScom(i_target);

    if (l_root_ctrl3.get_PLLCLKSW1_ALTREF_SEL())
    {
        FAPI_INF("Setting PLL CLK Sw 1 to 0");
        // Reset val to 0
        l_root_ctrl3.set_PLLCLKSW1_ALTREF_SEL(0); // Drive RCS FPLL1 with Refclk from OSC0

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
    }

    if (l_root_ctrl3.get_PLLCLKSW2_ALTREF_SEL())
    {
        FAPI_INF("Setting PLL CLK Sw 2 to 0");
        // Reset val to 0
        l_root_ctrl3.set_PLLCLKSW2_ALTREF_SEL(0); // Drive RCS FPLL2 with Refclk from OSC1

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
    }

    l_root_ctrl3.putScom(i_target);

    // Deskew Calibration
    l_rcs_ctrl1.getScom(i_target);
    l_rcs_ctrl1.set_DESKEW_AUTO_LOCK(0);
    l_rcs_ctrl1.putScom(i_target);
    fapi2::delay(WAIT_1MS, WAIT_100KCYC);
    l_rcs_ctrl1.set_DESKEW_AUTO_LOCK(1);
    l_rcs_ctrl1.putScom(i_target);
    l_sns1lth.getScom(i_target);
    FAPI_INF("RCS Auto Deskew A %d.", l_sns1lth.get_DESKEW_QOUT_A());
    FAPI_INF("RCS Auto Deskew B %d.", l_sns1lth.get_DESKEW_QOUT_B());

    // Set the clear clock errors to 0
    l_root_ctrl5.getScom(i_target);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(1);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(1);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_A(0);
    l_root_ctrl5.set_CLEAR_CLK_ERROR_B(0);
    l_root_ctrl5.putScom(i_target);
    fapi2::delay(WAIT_1US, WAIT_100KCYC);

    l_sns2lth.getScom(i_target);

    // if chosen osc is in use, pass
    // Set altrefclk to other source
    if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0)
    {
        if (l_sns2lth.get_MUXSEL_CLK_A() == 1)
        {
            FAPI_INF("RCS is already on Side A(OSC0 selected)");
        }
        else if (l_sns2lth.get_MUXSEL_CLK_B() == 1)
        {
            FAPI_INF("RCS is on Side B, switching to side A; OSC0 Selected");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
    }
    else if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        if (l_sns2lth.get_MUXSEL_CLK_B())
        {
            FAPI_INF("RCS is already on Side B(OSC1 selected)");
        }
        else if (l_sns2lth.get_MUXSEL_CLK_A())
        {
            FAPI_INF("RCS is on Side A, switching to side B; OSC1 Selected");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
    }

    FAPI_TRY(rcs_check_errors(i_target, l_refclock_select));

fapi_try_exit:
    FAPI_INF("End RCS Add");
    return current_err;
}
