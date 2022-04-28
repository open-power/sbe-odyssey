/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_clk_config.C $ */
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
/// @file  ody_chiplet_clk_config.C
///
/// @brief Enable clocks for chiplets
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "ody_chiplet_clk_config.H"
#include "poz_perv_common_params.H"
#include "poz_chiplet_clk_config.H"


using namespace fapi2;

enum ODY_CHIPLET_CLK_CONFIG_Private_Constants
{
};

static ReturnCode ody_chiplet_mux_setup(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_chip_target,
    const buffer<uint64_t> i_chiplet_mask)
{

    return FAPI2_RC_SUCCESS;
}


ReturnCode ody_chiplet_clk_config(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{

    FAPI_INF("Entering ...");
    FAPI_TRY(poz_chiplet_clk_config(i_target, ody_chiplet_mux_setup));



fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
