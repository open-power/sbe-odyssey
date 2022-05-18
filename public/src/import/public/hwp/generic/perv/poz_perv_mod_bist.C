/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_bist.C $ */
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
/// @file  poz_perv_mod_bist.C
/// @brief Contains definitions for modules bist_poll and bist_reg_cleanup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_bist.H>
#include <poz_perv_mod_chiplet_clocking.H>

using namespace fapi2;

enum POZ_PERV_MOD_BIST_Private_Constants
{
};


ReturnCode mod_bist_poll(const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target)
{
    FAPI_TRY(mod_abist_poll(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_bist_reg_cleanup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    bool i_clear_sram_abist_mode)
{
    FAPI_TRY(mod_abist_cleanup(i_target, i_clear_sram_abist_mode));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
