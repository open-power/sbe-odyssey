/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_setup_ref_clock.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
/// @file poz_setup_ref_clock.C
///
/// @brief shared code for setup_ref_clock procedure
/// -- Disable Write Protection for Root/Perv Control registers
/// -- Set RCS control signals to CFAM reset values
/// -- Setup refclock receiver termination & Unprotect inputs to RCS sense register
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "poz_setup_ref_clock.H"
#include "poz_perv_common_params.H"
#include <p11_scom_perv.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_GPWRP;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL6;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL6_COPY;

using namespace fapi2;
using namespace scomt::perv;

enum POZ_SETUP_REF_CLOCK_Private_Constants
{
    CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF,
};

ReturnCode poz_setup_ref_clock(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FSXCOMP_FSXLOG_GPWRP_t GPWRP;
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;
    FSXCOMP_FSXLOG_ROOT_CTRL0_COPY_t ROOT_CTRL0_COPY;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t ROOT_CTRL5;
    FSXCOMP_FSXLOG_ROOT_CTRL5_COPY_t ROOT_CTRL5_COPY;
    FSXCOMP_FSXLOG_ROOT_CTRL6_t ROOT_CTRL6;
    FSXCOMP_FSXLOG_ROOT_CTRL6_COPY_t ROOT_CTRL6_COPY;
    fapi2::ATTR_CLOCK_RCS_OUTPUT_MUX20_Type l_clock_rcs_output_mux20;
    fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> FAPI_SYSTEM;
    uint8_t l_sys0_term, l_sys1_term, l_pci0_term, l_pci1_term, l_cp_refclock_select;

    FAPI_INF("Entering ...");

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SYS0_REFCLOCK_RCVR_TERM, FAPI_SYSTEM, l_sys0_term),
             "Error from FAPI_ATTR_GET (ATTR_SYS0_REFCLOCK_RCVR_TERM)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SYS1_REFCLOCK_RCVR_TERM, FAPI_SYSTEM, l_sys1_term),
             "Error from FAPI_ATTR_GET (ATTR_SYS1_REFCLOCK_RCVR_TERM)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PCI0_REFCLOCK_RCVR_TERM, FAPI_SYSTEM, l_pci0_term),
             "Error from FAPI_ATTR_GET (ATTR_PCI0_REFCLOCK_RCVR_TERM)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PCI1_REFCLOCK_RCVR_TERM, FAPI_SYSTEM, l_pci1_term),
             "Error from FAPI_ATTR_GET (ATTR_PCI1_REFCLOCK_RCVR_TERM)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_cp_refclock_select),
             "Error from FAPI_ATTR_GET (ATTR_CP_REFCLOCK_SELECT)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CLOCK_RCS_OUTPUT_MUX20, i_target, l_clock_rcs_output_mux20),
             "Error from FAPI_ATTR_GET (ATTR_CLOCK_PLL_MUX20)");

    FAPI_INF("Disable Write Protection for Root/Perv Control registers");
    GPWRP = CONTROL_WRITE_PROTECT_DISABLE;
    FAPI_TRY(GPWRP.putCfam(i_target));

    FAPI_INF("Set RCS control signals to CFAM reset values, apply basic configuration for output clock enables and forced input clock.");
    ROOT_CTRL5 = 0;
    ROOT_CTRL5.set_RCS_RESET(1);
    ROOT_CTRL5.set_RCS_BYPASS(1);

    if ((l_cp_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) ||
        (l_cp_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1) ||
        (l_cp_refclock_select == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1_NORED))
    {
        ROOT_CTRL5.set_RCS_FORCE_CLKSEL(1);
    }

    ROOT_CTRL5.set_RCS_CLK_TEST_IN(0);
    ROOT_CTRL5.set_EN_REFCLK(l_clock_rcs_output_mux20 == fapi2::ENUM_ATTR_CLOCK_RCS_OUTPUT_MUX20_SYNC);
    ROOT_CTRL5.set_EN_ASYNC_OUT(l_clock_rcs_output_mux20 == fapi2::ENUM_ATTR_CLOCK_RCS_OUTPUT_MUX20_ASYNC);
    FAPI_TRY(ROOT_CTRL5.putCfam(i_target));

    ROOT_CTRL5_COPY = ROOT_CTRL5;
    FAPI_TRY(ROOT_CTRL5_COPY.putCfam(i_target));

    FAPI_INF("Set up refclock receiver termination");
    ROOT_CTRL6 = 0;
    ROOT_CTRL6.set_TC_AN_SYS0_RX_REFCLK_TERM(l_sys0_term);
    ROOT_CTRL6.set_TC_AN_SYS1_RX_REFCLK_TERM(l_sys1_term);
    ROOT_CTRL6.set_TAP_CFAM_RESET_DC(-1);
    FAPI_TRY(ROOT_CTRL6.putCfam(i_target));

    ROOT_CTRL6_COPY = ROOT_CTRL6;
    FAPI_TRY(ROOT_CTRL6_COPY.putCfam(i_target));

    FAPI_INF("Unprotect inputs to RCS sense register");
    FAPI_TRY(ROOT_CTRL0.getCfam(i_target));
    ROOT_CTRL0.set_CFAM_PROTECTION_0(0);
    FAPI_TRY(ROOT_CTRL0.putCfam(i_target));

    ROOT_CTRL0_COPY = ROOT_CTRL0;
    FAPI_TRY(ROOT_CTRL0_COPY.putCfam(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
