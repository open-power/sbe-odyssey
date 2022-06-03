/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_init.C $ */
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
/// @file ody_chiplet_init.C
///
/// @brief Program DDR PHY Nto1 clock division ratios
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "ody_chiplet_init.H"
#include "poz_perv_common_params.H"
#include "p11_scom_perv.H"

SCOMT_PERV_USE_CPLT_CONF1;

using namespace fapi2;
using namespace scomt::perv;

enum ODY_CHIPLET_INIT_Private_Constants
{
};

ReturnCode ody_chiplet_init(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    CPLT_CONF1_t CPLT_CONF1;
    auto l_mc_chiplets   = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_INF("Program DDR PHY Nto1 clock division ratios");
    CPLT_CONF1 = 0;
    CPLT_CONF1.insertFromRight<24, 6>(0b111111);  //TODO update when register headers are updated
    FAPI_TRY(CPLT_CONF1.putScom_SET(l_mc_chiplets));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
