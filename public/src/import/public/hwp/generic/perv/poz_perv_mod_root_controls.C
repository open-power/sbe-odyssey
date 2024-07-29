/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_root_controls.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
/// @file  poz_perv_mod_root_controls.C
/// @brief Module definitions for root controls
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_root_controls.H>
#include "poz_scom_perv.H"

using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_GPWRP;
using GPWRP_t = FSXCOMP_FSXLOG_GPWRP_t;

using namespace fapi2;

static constexpr int CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF;

ReturnCode mod_restore_root_controls(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const static_array<root_ctrl_restore>& i_restores)
{
    GPWRP_t GPWRP;

    FAPI_DBG("Disable Write Protection for Root/Perv Control registers");
    GPWRP = CONTROL_WRITE_PROTECT_DISABLE;
    FAPI_TRY(GPWRP.putCfam(i_target));

    FAPI_DBG("Restoring root/perv control register values");

    for (auto restore : i_restores)
    {
        FAPI_TRY(putCfamRegister(i_target, restore.main_addr, restore.init_value));
        FAPI_TRY(putCfamRegister(i_target, restore.copy_addr, restore.init_value));
    }

fapi_try_exit:
    return current_err;
}
