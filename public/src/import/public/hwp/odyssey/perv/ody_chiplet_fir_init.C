/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_chiplet_fir_init.C $ */
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
/// @file  ody_chiplet_fir_init.C
/// @brief Initialize chiplet level FIR registers
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_chiplet_fir_init.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_misc.H>
#include <poz_scom_perv.H>

SCOMT_PERV_USE_PCBCTL_COMP_INTR_HOST_MASK_REG;

using namespace scomt::poz;
using namespace fapi2;

typedef PCBCTL_COMP_INTR_HOST_MASK_REG_t INTR_HOST_MASK_t;

enum ODY_CHIPLET_FIR_INIT_Private_Constants
{
    DBG_SCOM_BASE = 0x107C0,
};

ReturnCode ody_chiplet_fir_init(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    INTR_HOST_MASK_t HOST_MASK;

    FAPI_INF("Entering ...");
    FAPI_TRY(mod_setup_clockstop_on_xstop(i_target, cc::REGION_ALL_BUT_PERV, ody_chiplet_delay_table));
    FAPI_TRY(mod_setup_tracestop_on_xstop(i_target, DBG_SCOM_BASE));
    FAPI_TRY(mod_unmask_firs(i_target));

    // Mask FIR reporting to host via OMI until OMI is set up;
    // will be cleared in ody_unmask later.
    FAPI_TRY(HOST_MASK.getScom(i_target));
    HOST_MASK.set_ERROR_MASK_0(1);
    HOST_MASK.set_ERROR_MASK_1(1);
    HOST_MASK.set_ERROR_MASK_2(1);
    HOST_MASK.set_ERROR_MASK_3(1);
    FAPI_TRY(HOST_MASK.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
