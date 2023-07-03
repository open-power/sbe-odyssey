/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_reset.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
/// @file  ody_chiplet_reset.C
/// @brief Enable and reset chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_chiplet_reset.H>
#include <poz_perv_common_params.H>
#include <poz_chiplet_reset.H>
#include <ody_scom_perv.H>
#include <target_filters.H>

using namespace fapi2;
using namespace scomt::perv;

SCOMT_PERV_USE_TPCHIP_TPC_CPLT_CONF1;
typedef TPCHIP_TPC_CPLT_CONF1_t CPLT_CONF1_t;

SCOMT_PERV_USE_TPCHIP_TPC_CPLT_CTRL2;
typedef TPCHIP_TPC_CPLT_CTRL2_t CPLT_CTRL2_t;

enum ODY_CHIPLET_RESET_Private_Constants
{
};

ReturnCode ody_chiplet_reset(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    CPLT_CONF1_t CPLT_CONF1;
    CPLT_CTRL2_t CPLT_CTRL2;
    auto l_target_MC = i_target.getChildren<fapi2::TARGET_TYPE_PERV>(fapi2::TARGET_FILTER_MC, TARGET_STATE_PRESENT)[0];

    FAPI_INF("Entering ...");
    FAPI_TRY(poz_chiplet_reset(i_target, 10, PRE_SCAN0));

    //Assert ATPGMODE_PUBMAC while we might scan to work around
    //a problem where a latch controlling async resets is on the scan chain.
    //Setting this mux to 1 will disconnect the async resets from that latch
    //so we can scan without issue.
    CPLT_CONF1 = 0;
    CPLT_CONF1.setBit<16>();
    FAPI_TRY(CPLT_CONF1.putScom_SET(l_target_MC));

    FAPI_INF("Program DDR PHY Nto1 clock division ratios");
    CPLT_CONF1 = 0;
    CPLT_CONF1.setBit<24, 6>();
    FAPI_TRY(CPLT_CONF1.putScom_SET(l_target_MC));

    FAPI_TRY(poz_chiplet_reset(i_target, 10, SCAN0_AND_UP));

    FAPI_INF("Force MC ATPG regions disabled despite ATTR_PG settings");
    CPLT_CTRL2 = 0;
    CPLT_CTRL2.setBit<11, 6>();
    FAPI_TRY(CPLT_CTRL2.putScom_CLEAR(l_target_MC));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
