/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chip_clocking.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
/// @brief Chip PLL lock checking
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Pretty Mariam Jacob (prettymjacob@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_chip_clocking.H>
#include <poz_perv_mod_chip_clocking_regs.H>

using namespace fapi2;
using namespace pll;

enum POZ_PERV_MOD_CHIP_CLOCKING_Private_Constants
{
    PLL_LOCK_DELAY_CYCLES = 100000,
    PLL_LOCK_DELAY_LOOPS = 1000,
    PLL_LOCK_DELAY_NS = 100000,
};

static ReturnCode _mod_poll_pll_lock(
    const Target < TARGET_TYPE_ANY_POZ_CHIP | TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target,
    uint32_t i_reg_addr,
    uint64_t i_check_value)
{
    fapi2::buffer<uint64_t> o_read_value;
    FAPI_DBG("Entering ...");

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
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_poll_pll_lock(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > &i_target,
    pll_lock_bits i_pll_mask)
{
    PLL_LOCK_t PLL_LOCK;
    fapi2::ReturnCode l_rc;
    fapi2::buffer<uint64_t> l_read_value;
    uint64_t l_check_value = (uint64_t)i_pll_mask << 56;

    FAPI_DBG("Entering ...");
    l_rc = _mod_poll_pll_lock(i_target, PLL_LOCK.addr, l_check_value);

    if (l_rc == fapi2::FAPI2_RC_FALSE)
    {
        FAPI_INF("Gather information about failing chiplets and PLLs");
        uint64_t l_failed_chiplets = 0;
        uint64_t l_failed_plls = 0;

        for (auto l_chiplet : i_target.getChildren<fapi2::TARGET_TYPE_PERV>())
        {
            FAPI_TRY(fapi2::getScom(l_chiplet, PLL_LOCK.addr, l_read_value));

            if ((l_read_value & l_check_value) != l_check_value)
            {
                FAPI_ERR("Chiplet %d PLL lock failed: PLL_LOCK(0:8)=0x%02x",
                         l_chiplet.getChipletNumber(), l_read_value >> 56);
                l_failed_plls |= (l_read_value & l_check_value) ^ l_check_value;
                l_failed_chiplets |= (1ULL << (63 - l_chiplet.getChipletNumber()));
            }
        }

        FAPI_ASSERT(false,
                    fapi2::POZ_PLL_LOCK_ERROR()
                    .set_FAILED_CHIPLETS(l_failed_chiplets)
                    .set_FAILED_PLLS(l_failed_plls)
                    .set_PROC_TARGET(i_target),
                    "ERROR: PLL LOCK ERROR");
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_poll_pll_lock_fsi2pib(
    const Target < TARGET_TYPE_ANY_POZ_CHIP >& i_target,
    pll_lock_bits i_pll_mask)
{
    FAPI_DBG("Entering ...");
    return _mod_poll_pll_lock(i_target, FSXCOMP_FSXLOG_MIRR_FSI2PIB_STAT, ((uint64_t)i_pll_mask << 36));

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}
