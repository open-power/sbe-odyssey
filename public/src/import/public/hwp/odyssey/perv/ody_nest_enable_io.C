/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_nest_enable_io.C $ */
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
/// @file  ody_nest_enable_io.C
/// @brief Enable chip/nest drivers and receivers
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_nest_enable_io.H>
#include <poz_perv_common_params.H>
#include <ody_scom_perv.H>
#include <target_filters.H>

using namespace fapi2;
using namespace scomt::perv;

SCOMT_PERV_USE_TPCHIP_NET_PCBRSPPERV_CTRL0;
typedef TPCHIP_NET_PCBRSPPERV_CTRL0_t NET_CTRL0_t;

enum ODY_NEST_ENABLE_IO_Private_Constants
{
};

ReturnCode ody_nest_enable_io(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    NET_CTRL0_t NET_CTRL0;

    FAPI_INF("Entering ...");

    FAPI_DBG("Chiplet receiver enable, Chiplet driver enable.");

    for (auto& chiplet : i_target.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_MC, TARGET_STATE_FUNCTIONAL))
    {
        NET_CTRL0 = 0;
        NET_CTRL0.set_CPLT_DCTRL(1);
        NET_CTRL0.set_CPLT_RCTRL(1);
        NET_CTRL0.set_CPLT_RCTRL2(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(chiplet));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
