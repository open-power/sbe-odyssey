/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_arrayinit.C $ */
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
/// @file  ody_chiplet_arrayinit.C
/// @brief Arrayinit all chiplets except TP
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_chiplet_arrayinit.H>
#include <poz_chiplet_arrayinit.H>
#include <poz_perv_common_params.H>

using namespace fapi2;

enum ODY_CHIPLET_ARRAYINIT_Private_Constants
{
    RUNN_CYCLES = 0x42FFF,
};

ReturnCode ody_chiplet_arrayinit(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");
    FAPI_TRY(poz_chiplet_arrayinit(i_target, cc::REGION_ALL, RUNN_CYCLES));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
