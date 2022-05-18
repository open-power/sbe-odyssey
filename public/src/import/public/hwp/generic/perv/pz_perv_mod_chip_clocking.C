/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_perv_mod_chip_clocking.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
/// @file  pz_perv_mod_chip_clocking.C
///
/// @brief clock test module for PZ chips to check whether the ref clock is valid.
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "pz_perv_mod_chip_clocking.H"
#include <p11_scom_perv.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS1LTH;

using namespace fapi2;
using namespace scomt::perv;

enum POZ_PERV_MOD_CHIP_CLOCKING_Private_Constants
{
    POLL_COUNT = 10,
    HW_NS_DELAY = 20, // unit is nano seconds
    SIM_CYCLE_DELAY = 1000, // unit is sim cycles
};

ReturnCode mod_clock_test(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target,
                          bool i_use_cfam_path)
{
    FSXCOMP_FSXLOG_ROOT_CTRL5_t ROOT_CTRL5;
    FSXCOMP_FSXLOG_SNS1LTH_t SNS1LTH;
    fapi2::ATTR_CP_REFCLOCK_SELECT_Type l_cp_refclock_select;

    FAPI_INF("Entering ...");

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_cp_refclock_select));

    for(int i = 0; i < POLL_COUNT; i++)
    {
        for(int test_value = 0 ; test_value < 2 ; test_value++)
        {
            ROOT_CTRL5.flush<0>();
            ROOT_CTRL5.set_TCFSI_AN_CLKTOP_RCS_CLK_TEST_IN_DC(1);

            if (i_use_cfam_path)
            {
                if (test_value)
                {
                    FAPI_TRY(ROOT_CTRL5.putCfam_SET(i_target));
                }
                else
                {
                    FAPI_TRY(ROOT_CTRL5.putCfam_CLEAR(i_target));
                }

                FAPI_TRY(delay(HW_NS_DELAY, SIM_CYCLE_DELAY));

                FAPI_TRY(SNS1LTH.getCfam(i_target));
            }
            else
            {
                if (test_value)
                {
                    FAPI_TRY(ROOT_CTRL5.putScom_SET(i_target));
                }
                else
                {
                    FAPI_TRY(ROOT_CTRL5.putScom_CLEAR(i_target));
                }

                FAPI_TRY(delay(HW_NS_DELAY, SIM_CYCLE_DELAY));

                FAPI_TRY(SNS1LTH.getScom(i_target));
            }

            const bool clockA_functional = SNS1LTH.get_REFCLK_DATA_OUT_A() == test_value;
            const bool clockB_functional = SNS1LTH.get_REFCLK_DATA_OUT_B() == test_value;

            /* Find out for each clock whether it is required or merely optional
             * according to the refclock_select value.
             */

            const bool clockA_required = l_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1 &&
                                         l_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1_NORED;
            const bool clockB_required = l_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0 &&
                                         l_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0_NORED;

            const bool clockA_ok = clockA_functional || !clockA_required;
            const bool clockB_ok = clockB_functional || !clockB_required;

            const uint8_t callout_clock = clockA_ok ? fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1 :
                                          fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0;

            FAPI_ASSERT(clockA_ok && clockB_ok,
                        fapi2::RCS_CLOCK_TEST_OUT_ERROR()
                        .set_PROC_TARGET(i_target)
                        .set_READ_SNS1LTH(SNS1LTH)
                        .set_ATTR_CP_REFCLOCK_SELECT_VALUE(l_cp_refclock_select)
                        .set_RCS_CLOCK_TEST_IN(test_value)
                        .set_CLOCK_A_OK(clockA_ok)
                        .set_CLOCK_B_OK(clockB_ok)
                        .set_CLOCK_POS(callout_clock),
                        "Bad reference clock: A functional/required: %d/%d  B functional/required: %d/%d",
                        clockA_functional, clockA_required, clockB_functional, clockB_required
                       );

        }
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
