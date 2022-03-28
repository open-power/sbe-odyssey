/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chip_clocking.C $ */
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
/// @file  poz_perv_mod_chip_clocking.C
///
/// @brief  contains definitions for modules poll/cleanup, scan0
///                                          start_stop_clocks, align_regions
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "poz_perv_mod_chip_clocking.H"
#include "poz_perv_common_params.H"
#include <p11_scom_perv.H>

using namespace fapi2;
using namespace scomt::perv;
using namespace pll;

enum POZ_PERV_MOD_CHIP_CLOCKING_Private_Constants
{
    PLL_LOCK_DELAY_CYCLES = 100000,
    PLL_LOCK_DELAY_LOOPS = 50,
    PLL_LOCK_DELAY_NS = 100000,
};


static ReturnCode _mod_poll_pll_lock(
    const Target < TARGET_TYPE_ANY_POZ_CHIP | TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target,
    uint32_t i_reg_addr,
    uint64_t i_check_value)
{
    fapi2::buffer<uint64_t> o_read_value;
    FAPI_INF("Entering ...");

    for(int loop = 0; loop < PLL_LOCK_DELAY_LOOPS; loop++)
    {
        FAPI_TRY(fapi2::getScom(i_target, i_reg_addr, o_read_value));

        if ((o_read_value & i_check_value) == i_check_value)
        {
            return fapi2::FAPI2_RC_SUCCESS;
        }

        FAPI_TRY(fapi2::delay(PLL_LOCK_DELAY_NS, PLL_LOCK_DELAY_CYCLES));
    }

    return fapi2::FAPI2_RC_FALSE;

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_poll_pll_lock(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target,
    pll_lock_bits i_pll_mask)
{
    FAPI_INF("Entering ...");
    return _mod_poll_pll_lock(i_target, PLL_LOCK_REG, ((uint64_t)i_pll_mask << 56));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
