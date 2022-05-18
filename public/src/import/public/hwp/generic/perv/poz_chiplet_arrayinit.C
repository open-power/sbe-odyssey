/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_arrayinit.C $ */
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
/// @file  poz_chiplet_arrayinit.C
/// @brief Arrayinit all chiplets except TP
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_arrayinit.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_chiplet_clocking.H>


using namespace fapi2;
using namespace cc;

enum POZ_CHIPLET_ARRAYINIT_Private_Constants
{
};

ReturnCode poz_chiplet_arrayinit(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    auto l_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_INF("Entering ...");

    FAPI_DBG("ABIST start for all chiplets except TP (all regions)");
    FAPI_TRY(mod_abist_start(l_chiplets_mc, REGION_ALL));

    FAPI_DBG("ABIST poll");
    FAPI_TRY(mod_abist_poll(l_chiplets_mc));

    FAPI_DBG("ABIST cleanup");
    FAPI_TRY(mod_abist_cleanup(l_chiplets_mc));

    FAPI_DBG("SCAN0 for all chiplets except TP (all regions)");
    FAPI_TRY(mod_scan0(l_chiplets_mc, REGION_ALL));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
