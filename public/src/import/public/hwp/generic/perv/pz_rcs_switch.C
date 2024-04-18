/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_rcs_switch.C $   */
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
/// @file  pz_rcs_switch.C
/// @brief Switch oscillator RCS(Redundant clock switch) path
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Josh Chica (josh.chica@ibm.com)
// *HWP FW Maintainer   : Chris Steffen (cwsteffen@us.ibm.com)
// *HWP Consumed by     : SBE
//------------------------------------------------------------------------------

#include "pz_rcs_switch.H"
#include "pz_rcs_common.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_chip_clocking.H"
#include <poz_scom_perv.H>
#include <poz_perv_utils.H>


using namespace fapi2;
using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_RCS_CTRL1;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SNS2LTH;

enum PZ_RCS_SETUP_Private_Constants
{
    CTRL1_BLOCK_SWO_AUTO = 21,
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

ReturnCode pz_rcs_switch(const Target < TARGET_TYPE_PROC_CHIP | TARGET_TYPE_HUB_CHIP > & i_target)
{
    FAPI_INF("Start RCS Switch");

    FSXCOMP_FSXLOG_RCS_CTRL1_t l_rcs_ctrl1;
    FSXCOMP_FSXLOG_ROOT_CTRL5_t l_root_ctrl5;
    FSXCOMP_FSXLOG_SNS2LTH_t l_sns2lth;

    // Check ATTR for which OSC
    fapi2::ATTR_CP_REFCLOCK_SELECT_Type l_refclock_select = 0;
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target, l_refclock_select));

    FAPI_INF("Checking block switchover");
    l_rcs_ctrl1.getScom(i_target);
    l_root_ctrl5.getScom(i_target);

    // If either switch over is blocked, skip switchover
    if (l_rcs_ctrl1.getBit<CTRL1_BLOCK_SWO_AUTO>() || l_root_ctrl5.getBit<ROOT_CTRL5_BLOCK_SWO>())
    {
        FAPI_INF("Switch over blocked, likely because an OSC has been removed");
        goto fapi_try_exit;
    }

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
        else
        {
            FAPI_ERR("No MUX selected, exiting");
            goto fapi_try_exit;
        }
    }
    else if ((l_refclock_select & fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1) == fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1)
    {
        if (l_sns2lth.get_MUXSEL_CLK_B() == 1)
        {
            FAPI_INF("RCS is already on Side B(OSC1 selected)");
        }
        else if (l_sns2lth.get_MUXSEL_CLK_A() == 1)
        {
            FAPI_INF("RCS is on Side A, switching to side B; OSC1 Selected");
            FAPI_TRY(rcs_sw_switch(i_target));
        }
        else
        {
            FAPI_ERR("No MUX selected, exiting");
            goto fapi_try_exit;
        }
    }

    FAPI_TRY(rcs_check_errors(i_target, l_refclock_select));

fapi_try_exit:
    FAPI_INF("End RCS Switch");
    return current_err;
}
