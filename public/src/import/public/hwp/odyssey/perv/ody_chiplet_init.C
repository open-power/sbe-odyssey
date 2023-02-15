/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_init.C $ */
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
/// @file  ody_chiplet_init.C
/// @brief Program DDR PHY Nto1 clock division ratios
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_chiplet_init.H>
#include <poz_perv_common_params.H>
#include <ody_scom_perv.H>
#include <target_filters.H>

using namespace fapi2;
using namespace scomt::perv;

SCOMT_PERV_USE_TPCHIP_TPC_CPLT_CONF1;
typedef TPCHIP_TPC_CPLT_CONF1_t CPLT_CONF1_t;

SCOMT_PERV_USE_TPCHIP_TPC_CPLT_CTRL2;
typedef TPCHIP_TPC_CPLT_CTRL2_t CPLT_CTRL2_t;

enum ODY_CHIPLET_INIT_Private_Constants
{
};

ReturnCode ody_chiplet_init(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    CPLT_CONF1_t CPLT_CONF1;
    CPLT_CTRL2_t CPLT_CTRL2;
    auto l_mc_chiplets   = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_INF("Program DDR PHY Nto1 clock division ratios");
    CPLT_CONF1 = 0;
    CPLT_CONF1.setBit<24, 6>();
    FAPI_TRY(CPLT_CONF1.putScom_SET(l_mc_chiplets));

    FAPI_INF("Force MC ATPG regions disabled despite ATTR_PG settings");
    CPLT_CTRL2 = 0;
    CPLT_CTRL2.setBit<11, 6>();
    FAPI_TRY(CPLT_CTRL2.putScom_CLEAR(l_mc_chiplets));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
