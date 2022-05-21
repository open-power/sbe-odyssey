/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_bist.C $        */
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
/// @brief
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "poz_bist.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_misc.H"
#include "poz_perv_mod_chiplet_clocking.H"
#include "poz_perv_mod_bist.H"
#include <target_filters.H>


using namespace fapi2;

enum POZ_BIST_Private_Constants
{
};

ReturnCode poz_bist(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target /*, bist_params &i_params*/)
{
    FAPI_INF("Entering ...");

    FAPI_DBG("Setup multicast group 6");
    FAPI_DBG("BIST multicast setup not yet implemented; check back later");
    // FAPI_TRY(mod_multicast_setup(i_target, 6, i_params.chiplets, TARGET_STATE_FUNCTIONAL));
    // TODO once above code is supported, use the line below to get group 6
    auto l_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);

    FAPI_DBG("Scan0 all regions");
    FAPI_TRY(mod_scan0(l_chiplets_mc, REGION_ALL));

    FAPI_DBG("Scan load BIST programming");
    // TODO add in scan code once supported
    // FAPI_TRY(putRing(i_params.setup_image));
    FAPI_DBG("BIST ring setup not yet implemented; check back later");

    FAPI_DBG("Apply scan patches if needed");
    // TODO add in scan code once supported
    // FAPI_TRY(putRing(i_params.patch_image));
    FAPI_DBG("BIST ring patch not yet implemented; check back later");

    FAPI_DBG("Setup all SCOM registers");

    // TODO update me once ABIST/LBIST mode param is supported
    if (/* i_params.action == ABIST */ true)
    {
        FAPI_TRY(mod_abist_setup(l_chiplets_mc, REGION_ALL));
    }
    else
    {
        FAPI_TRY(mod_lbist_setup(l_chiplets_mc, REGION_ALL));
    }

    FAPI_DBG("Start BIST with OPCG GO");
    FAPI_TRY(mod_opcg_go(l_chiplets_mc));

    FAPI_DBG("Poll for DONE (BIST, OPCG, or halt)");
    FAPI_TRY(mod_bist_poll(l_chiplets_mc));

    FAPI_DBG("Cleanup all SCOM registers");
    FAPI_TRY(mod_bist_reg_cleanup(l_chiplets_mc));


fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
