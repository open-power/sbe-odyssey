/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_reset.C $ */
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
/// @file  ody_chiplet_reset.C
///
/// @brief  Enable and reset chiplets, setup clock controllers & OPCG delays
///         do SCAN0 on all regions, transfer PGOOD attr into CPLT_CTRL[2,3]
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "ody_chiplet_reset.H"
#include "poz_perv_common_params.H"
#include "poz_chiplet_reset.H"


using namespace fapi2;

enum ODY_CHIPLET_RESET_Private_Constants
{
};

ReturnCode ody_chiplet_reset(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");

    FAPI_TRY(poz_chiplet_reset(i_target, ody_chiplet_delay_table));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
