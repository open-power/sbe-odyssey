/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_startclocks.C $ */
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
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Kevin Duffy (kjduffy@us.ibm.com)
// *HWP Consumed by     : SPPE
//------------------------------------------------------------------------------

#include "ody_tp_startclocks.H"
#include "poz_perv_common_params.H"
#include "poz_perv_mod_chiplet_clocking.H"
#include <target_filters.H>
#include <poz_perv_utils.H>

using namespace fapi2;
using namespace cc;

enum ODY_TP_STARTCLOCKS_Private_Constants
{
    ODY_PERV_NET_PERV = ODY_PERV_NET | ODY_PERV_PERV,
};

ReturnCode ody_tp_startclocks(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_INF("Entering ...");
    fapi2::Target<fapi2::TARGET_TYPE_PERV> l_tpchiplet = get_tp_chiplet_target(i_target);

    FAPI_INF("Calling mod_start_stop_clocks for PERV & NET regions ...");
    FAPI_TRY(mod_start_stop_clocks(l_tpchiplet, ODY_PERV_NET_PERV));


fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
