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
/// @brief Add oscillator to RCS(Redundant clock switch) path
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_add.H"
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
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_refclock_select));

    FAPI_TRY(print_debug_info(i_target, l_refclock_select));

    // Sanity check
    if (!(l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0
          || l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1))
    {
        FAPI_INF("Wrong RCS Add provided (%d) skipping", l_refclock_select);
        goto fapi_try_exit;
    }

    // Set PPM WD Reset
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.setBit<RCS_CONSTS::CTRL1_PPM_RESET>();
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));

    // Check Alt ref clk for FPLLs
    FAPI_TRY(l_root_ctrl3.getScom(i_target));

    if (l_root_ctrl3.get_PLLCLKSW1_ALTREF_SEL())
    {
        FAPI_INF("Setting PLL CLK Sw 1 to 0");

        l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_RESET>();
        FAPI_TRY(l_root_ctrl3.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

        // Reset val to 0
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_ALTREF_SEL>(); // Drive RCS FPLL1 with Refclk from OSC0
        FAPI_TRY(l_root_ctrl3.putScom(i_target));

        fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

        // Release Reset, allow the FPLLs to attempt to lock
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_RESET>();
        FAPI_TRY(l_root_ctrl3.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_5MS, RCS_CONSTS::WAIT_100KCYC);

        FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWA));

        // Now that the FPLLs are locked, release bypass
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_BYPASS_EN>();
    }

    if (l_root_ctrl3.get_PLLCLKSW2_ALTREF_SEL())
    {
        FAPI_INF("Setting PLL CLK Sw 2 to 0");

        l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_RESET>();
        FAPI_TRY(l_root_ctrl3.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

        // Reset val to 0
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_ALTREF_SEL>(); // Drive RCS FPLL2 with Refclk from OSC1
        FAPI_TRY(l_root_ctrl3.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

        // Release Reset, allow the FPLLs to attempt to lock
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_RESET>();
        FAPI_TRY(l_root_ctrl3.putScom(i_target));
        fapi2::delay(RCS_CONSTS::WAIT_5MS, RCS_CONSTS::WAIT_100KCYC);

        FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, pll::PZ_PERV_PLLCLKSWB));

        // Now that the FPLLs are locked, release bypass
        l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_BYPASS_EN>();
    }

    FAPI_TRY(l_root_ctrl3.putScom(i_target));

    // Deskew Calibration
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.clearBit<FSXCOMP_FSXLOG_RCS_CTRL1_DESKEW_AUTO_LOCK>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1MS, RCS_CONSTS::WAIT_100KCYC);
    l_rcs_ctrl1.setBit<FSXCOMP_FSXLOG_RCS_CTRL1_DESKEW_AUTO_LOCK>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));
    FAPI_TRY(l_sns1lth.getScom(i_target));
    FAPI_INF("RCS Auto Deskew A %d.", l_sns1lth.get_DESKEW_QOUT_A());
    FAPI_INF("RCS Auto Deskew B %d.", l_sns1lth.get_DESKEW_QOUT_B());

    // Set the clear clock errors to 0
    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_A>();
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_CLEAR_CLK_ERROR_B>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    // Validate state
    FAPI_INF("Verifing state");
    FAPI_TRY(rcs_check_errors(i_target, l_refclock_select));
    FAPI_TRY(rcs_verify_clean_state(i_target, l_refclock_select));

    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    FAPI_TRY(l_root_ctrl5.getScom(i_target));

    FAPI_INF("Unblocking switchover");
    // Unblock switchover
    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    l_root_ctrl5.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL5_BLOCK_SWO>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);

    // Clear PPM WD Reset
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.clearBit<RCS_CONSTS::CTRL1_PPM_RESET>();
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));

fapi_try_exit:
    FAPI_INF("End RCS Add");
    return current_err;
}
