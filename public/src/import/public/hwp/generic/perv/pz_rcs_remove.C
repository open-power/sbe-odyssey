/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_remove.C $   */
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
/// @file  pz_rcs_remove.C
/// @brief Remove oscillator from RCS(Redundant clock switch) path
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_remove.H"
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

ReturnCode pz_rcs_remove(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FAPI_INF("Start RCS Remove");

    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS1LTH_t l_sns1lth;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;
    FSXCOMP_FSXLOG_ROOT_CTRL3_t l_root_ctrl3;

    // Check ATTR for which OSC
    fapi2::ATTR_CP_REFCLOCK_SELECT_Type l_refclock_select;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_refclock_select));

    FAPI_TRY(print_debug_info(i_target, l_refclock_select));

    // Set PPM WD Reset
    FAPI_TRY(l_rcs_ctrl1.getScom(i_target));
    l_rcs_ctrl1.setBit<RCS_CONSTS::CTRL1_PPM_RESET>();
    FAPI_TRY(l_rcs_ctrl1.putScom(i_target));

    FAPI_TRY(l_root_ctrl3.getScom(i_target));

    if (l_root_ctrl3.get_PLLCLKSW1_ALTREF_SEL() || l_root_ctrl3.get_PLLCLKSW2_ALTREF_SEL())
    {
        FAPI_INF("One of the PLLs is already in ALTREF mode. Cannot remove another OSC");
        goto fapi_try_exit;
    }

    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_ALTREF_SEL>();
    l_root_ctrl3.clearBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_ALTREF_SEL>();

    // Remove OSC0
    if (l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        // Ensure FPLLB is driven by OSC1
        // Switchover
        if (l_sns2lth.get_MUXSEL_CLK_A())
        {
            FAPI_INF("RCS is on Side A, switching to side B; Remove OSC0");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
        else if (l_sns2lth.get_MUXSEL_CLK_B())
        {
            FAPI_INF("RCS is already on Side B, skipping");
        }

        // Set altrefclk on FPLLA
        FAPI_INF("Driving RCS FPLLA from OSC1 (Alt Refclk)");
        l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW1_ALTREF_SEL>(); // Drive RCS FPLL1 with Alt Refclk from OSC1

        FAPI_TRY(l_root_ctrl3.putScom(i_target));

        // Lock the FPLL
        FAPI_TRY(rcs_lock_fplla(i_target));
    }
    // Remove OSC1
    else if (l_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0)
    {
        // Ensure FPLLA is driven by OSC0
        // Switchover
        if (l_sns2lth.get_MUXSEL_CLK_B())
        {
            FAPI_INF("RCS is on Side B, switching to side A; Remove OSC1");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
        else if (l_sns2lth.get_MUXSEL_CLK_A())
        {
            FAPI_INF("RCS is already on Side A, skipping");
        }

        // Set altrefclk on FPLLB
        FAPI_INF("Driving RCS FPLLB from OSC0 (Alt Refclk)");
        l_root_ctrl3.setBit<FSXCOMP_FSXLOG_ROOT_CTRL3_PLLCLKSW2_ALTREF_SEL>(); // Drive RCS FPLL2 with Alt Refclk from OSC0

        FAPI_TRY(l_root_ctrl3.putScom(i_target));

        // Lock the FPLL
        FAPI_TRY(rcs_lock_fpllb(i_target));
    }
    else
    {
        FAPI_INF("Wrong RCS Remove provided (%d), skipping alt ref clock selection", l_refclock_select);
    }

    // Block switchover
    fapi2::delay(RCS_CONSTS::WAIT_1US, RCS_CONSTS::WAIT_100KCYC);
    FAPI_TRY(l_root_ctrl5.getScom(i_target));
    l_root_ctrl5.setBit<FSXCOMP_FSXLOG_ROOT_CTRL5_BLOCK_SWO>();
    FAPI_TRY(l_root_ctrl5.putScom(i_target));

    // Validate state
    FAPI_INF("Verifing state");
    FAPI_TRY(rcs_check_errors(i_target, l_refclock_select));
    FAPI_TRY(rcs_verify_clean_state(i_target, l_refclock_select));

    FAPI_TRY(print_debug_info(i_target, l_refclock_select));

fapi_try_exit:
    FAPI_INF("End RCS Remove");
    return current_err;
}
