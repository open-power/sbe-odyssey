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
#include <poz_perv_mod_chiplet_clocking_regs.H>

using namespace fapi2;

enum POZ_PERV_MOD_BIST_Private_Constants
{
};


ReturnCode mod_bist_poll(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > & i_target,
    bool i_poll_abist_done,
    bool i_assert_abist_done,
    uint32_t i_max_polls,
    uint32_t i_hw_delay,
    uint32_t i_sim_delay)
{
    FAPI_INF("Entering ...");

    // BH_SCRATCH_REG_t BH_SCRATCH_REG;
    CPLT_STAT0_t CPLT_STAT0;
    PCB_OPCG_STOP_t PCB_OPCG_STOP;
    OPCG_REG0_t OPCG_REG0;

    uint32_t l_total_polls = 0;
    // Infinite polling if negative signed (or massive unsigned) number
    const bool l_infinite_polling = i_max_polls > 0x7FFFFFFF;

    // Get chiplet container for OPCG count diagnostic readout
    auto l_chiplets = i_target.getChildren<fapi2::TARGET_TYPE_PERV>();

    if (i_poll_abist_done)
    {
        FAPI_INF("Watching OPCG_DONE, ABIST_DONE, and BIST_HALT");
    }
    else
    {
        FAPI_INF("Watching OPCG_DONE and BIST_HALT");
    }

    FAPI_DBG("BIST_HALT not yet implemented; check back later");

    if (l_infinite_polling)
    {
        FAPI_DBG("Poll count value triggers infinite polling");
    }

    while ((l_total_polls < i_max_polls) || l_infinite_polling)
    {
        // TODO add BIST_HALT functionality once supported
        /*
        FAPI_TRY(BH_SCRATCH_REG.getScom(i_target));
        if (BH_SCRATCH_REG.get_BIST_HALT() == 1)
        {
            FAPI_INF("BIST_HALT observed");
            break;
        }
        */

        FAPI_TRY(CPLT_STAT0.getScom(i_target));

        ++l_total_polls;

        // Only print once every 4 polls to reduce logging volume
        if ((l_total_polls & 3) == 0)
        {
            if (l_infinite_polling)
            {
                FAPI_DBG("Polls elapsed: %d", l_total_polls);
            }
            else
            {
                FAPI_DBG("Polls remaining: %d", i_max_polls - l_total_polls);
            }
        }

        if (i_poll_abist_done && CPLT_STAT0.get_ABIST_DONE() == 1)
        {
            FAPI_INF("ABIST_DONE observed");
            break;
        }

        if (CPLT_STAT0.get_OPCG_DONE() == 1)
        {
            FAPI_INF("OPCG_DONE observed");
            break;
        }

        FAPI_TRY(fapi2::delay(i_hw_delay, i_sim_delay));
    }

    FAPI_DBG("Total poll count: %d", l_total_polls);

    if (CPLT_STAT0.get_OPCG_DONE() == 0)
    {
        FAPI_INF("Forcibly stopping OPCG");
        PCB_OPCG_STOP = 0;
        PCB_OPCG_STOP.set_PCB_OPCGSTOP(1);
        FAPI_TRY(PCB_OPCG_STOP.putScom(i_target));
    }

    for (auto& chiplet : l_chiplets)
    {
        FAPI_TRY(OPCG_REG0.getScom(chiplet));
        FAPI_DBG("OPCG cycles elapsed for chiplet %d: %lu",
                 chiplet.getChipletNumber(),
                 OPCG_REG0.get_LOOP_COUNT());
    }

    FAPI_ASSERT((l_total_polls < i_max_polls) || l_infinite_polling,
                fapi2::DONE_HALT_NOT_SET()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_POLL_COUNT(l_total_polls)
                .set_HW_DELAY(i_hw_delay)
                .set_SIM_DELAY(i_sim_delay)
                .set_PROC_TARGET(i_target),
                "ERROR: DONE / HALT DID NOT OCCUR IN TIME");

    FAPI_ASSERT(CPLT_STAT0.get_ABIST_DONE() || !i_assert_abist_done,
                fapi2::POZ_SRAM_ABIST_DONE_BIT_ERR()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_SELECT_SRAM(true)
                .set_PROC_TARGET(i_target),
                "ERROR: ABIST_DONE NOT SET");

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


ReturnCode mod_lbist_setup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    uint16_t i_regions,
    uint64_t i_runn_cycles,
    uint64_t i_lbist_start_at,
    uint64_t i_lbist_start_stagger,
    const uint16_t* i_chiplets_regions,
    const uint64_t i_ctrl_chiplets,
    const uint16_t i_lbist_sequence,
    const uint16_t i_lbist_weight)
{
    FAPI_DBG("mod_lbist_setup not yet implemented; check back later");

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
