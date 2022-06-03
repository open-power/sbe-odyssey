/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_startclocks.C $ */
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
/// @file  ody_chiplet_startclocks.C
///
/// @brief start clocks for all chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "ody_chiplet_startclocks.H"
#include "poz_perv_common_params.H"
#include "poz_chiplet_startclocks.H"
#include <p11_scom_perv.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_PERV_CTRL0;

using namespace fapi2;
using namespace cc;
using namespace scomt::perv;

enum ODY_CHIPLET_STARTCLOCKS_Private_Constants
{
};

ReturnCode ody_chiplet_startclocks(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0;
    auto l_mc_NO_TP = i_target.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_INF("Drop TP chiplet fence");
    PERV_CTRL0 = 0;
    PERV_CTRL0.setBit<17>(); // temporary - when headers catch up, remove this and uncomment below
    //PERV_CTRL0.set_TC_PERV_CHIPLET_FENCE_DC(1);
    FAPI_TRY(PERV_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Starting chiplet clocks");
    FAPI_TRY(poz_chiplet_startclocks(l_mc_NO_TP, REGION_ALL));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
