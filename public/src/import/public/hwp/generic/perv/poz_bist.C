/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_bist.C $        */
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
/// @file  poz_bist.C
/// @brief Common module for ABIST/LBIST
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include <poz_bist.H>
#include <poz_perv_common_params.H>
#include <poz_perv_mod_misc.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_mod_bist.H>
#include <poz_chiplet_arrayinit.H>
#include <target_filters.H>

using namespace fapi2;

enum POZ_BIST_Private_Constants
{
};

// Determine if we only have one chiplet and can unicast BIST
bool can_unicast(uint64_t i_chiplets)
{
    // Currently, only one bit is set means only one chiplet
    return (i_chiplets != 0 && (i_chiplets & (i_chiplets - 1)) == 0);
}

// Given a 64-bit mask, return the index of the first set bit
uint8_t get_set_bit_index(uint64_t i_mask)
{
    uint8_t l_index = 0;
    uint64_t l_mask_scanner = 0x8000000000000000;

    while (l_mask_scanner && !(l_mask_scanner & i_mask))
    {
        l_mask_scanner >>= 1;
        l_index++;
    }

    return l_index;
}

void print_bist_params(const bist_params& i_params)
{
    FAPI_DBG("BIST_PARAMS_VERSION = %d", i_params.BIST_PARAMS_VERSION);
    FAPI_DBG("program = %s", i_params.program);
    FAPI_DBG("ring_patch = %s", i_params.ring_patch);
    FAPI_DBG("chiplets = %#018lx", i_params.chiplets);
    FAPI_DBG("flags = %#010x", i_params.flags);
    FAPI_DBG("opcg_count = %lu", i_params.opcg_count);
    FAPI_DBG("idle_count = %lu", i_params.idle_count);
    FAPI_DBG("max_polls = %u", i_params.max_polls);
    FAPI_DBG("linear_stagger = %lu", i_params.linear_stagger);
    FAPI_DBG("zigzag_stagger = %lu", i_params.zigzag_stagger);
    FAPI_DBG("regions = %#06x", i_params.regions);
    FAPI_DBG("outer_loop_mask = %#06x", i_params.outer_loop_mask);
    FAPI_DBG("inner_loop_mask = %#06x", i_params.inner_loop_mask);
}

ReturnCode poz_bist_execute(const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST,
                            MULTICAST_AND > & i_chiplets_target,
                            const bist_params& i_params, const uint16_t i_enum_condition_a, const uint16_t i_enum_condition_b)
{
    ////////////////////////////////////////////////////////////////
    // STAGE: REG_SETUP
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_REG_SETUP)
    {
        FAPI_INF("Setup all SCOM registers");

        // TODO pass in SKIP_FIRST_CLOCK to setup functions once supported
        if (i_params.flags & i_params.bist_flags::SKIP_FIRST_CLOCK)
        {
            FAPI_DBG("SKIP_FIRST_CLOCK not yet implemented; check back later");
            return FAPI2_RC_FALSE;
        }

        // TODO pass in SKIP_LAST_CLOCK to setup functions once supported
        if (i_params.flags & i_params.bist_flags::SKIP_LAST_CLOCK)
        {
            FAPI_DBG("SKIP_LAST_CLOCK not yet implemented; check back later");
            return FAPI2_RC_FALSE;
        }

        // TODO pass in zigzag_stagger to setup functions once supported
        if (i_params.zigzag_stagger)
        {
            FAPI_DBG("zigzag_stagger not yet implemented; check back later");
            return FAPI2_RC_FALSE;
        }

        if (i_params.flags & i_params.bist_flags::ABIST_NOT_LBIST)
        {
            FAPI_TRY(mod_abist_setup(i_chiplets_target,
                                     i_params.regions,
                                     i_params.opcg_count,
                                     i_params.idle_count,
                                     i_params.linear_stagger));
        }
        else
        {
            // TODO create sequence / weight enums for condition_a / condition_b
            FAPI_TRY(mod_lbist_setup(i_chiplets_target,
                                     i_params.regions,
                                     i_params.opcg_count,
                                     i_params.idle_count,
                                     i_params.linear_stagger,
                                     i_enum_condition_a,
                                     i_enum_condition_b));
        }
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: GO
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_GO)
    {
        // TODO pass in UNICAST_GO to go function once supported
        if (i_params.flags & i_params.bist_flags::UNICAST_GO)
        {
            FAPI_DBG("UNICAST_GO not yet implemented; check back later");
            return FAPI2_RC_FALSE;
        }

        FAPI_INF("Start BIST with OPCG GO");
        FAPI_TRY(mod_opcg_go(i_chiplets_target));
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: POLL
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_POLL)
    {
        FAPI_INF("Poll for DONE or HALT");
        // TODO pass in poll count and delay arguments once supported
        // TODO put ASSERT_ABIST_DONE in bist_params and turn off if ABIST infinite or LBIST
        FAPI_TRY(mod_bist_poll(i_chiplets_target,
                               i_params.flags & i_params.bist_flags::POLL_ABIST_DONE,
                               i_params.flags & i_params.bist_flags::ASSERT_ABIST_DONE,
                               i_params.max_polls));
    }


fapi_try_exit:
    return current_err;
}

ReturnCode poz_bist(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, const bist_params& i_params)
{
    FAPI_INF("Entering ...");

    // Our pervasive target (can be multicast or unicast)
    Target<TARGET_TYPE_PERV> l_chiplets_uc;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > l_chiplets_target;

    // Needed for unicast condition
    uint8_t l_chiplet_number = 0;

    // Scan0 regions are different for TP BIST
    clock_region l_scan0_region = REGION_ALL;

    // Helper buffers produced from bist_params constituents
    const buffer<uint16_t> l_outer_loop_mask = i_params.outer_loop_mask;
    const buffer<uint16_t> l_inner_loop_mask = i_params.inner_loop_mask;

    // Check to see if structure BIST_PARAMS is the expected version.
    // if(i_params.BIST_PARAMS_VERSION != BIST_PARAMS_CURRENT_VERSION)
    FAPI_ASSERT(i_params.BIST_PARAMS_VERSION == BIST_PARAMS_CURRENT_VERSION,
                fapi2::BAD_BIST_PARAMS_FORMAT().
                set_BIST_PARAMS(i_params.BIST_PARAMS_VERSION),
                "Expect Version = %d, got Version = %d instead for bist_params structure,",
                BIST_PARAMS_CURRENT_VERSION, i_params.BIST_PARAMS_VERSION)
    FAPI_DBG("Received Structure Version = %d, Check Passed.", i_params.BIST_PARAMS_VERSION);

    // Print all BIST parameters for debug
    print_bist_params(i_params);

    // Configure our target (unicast OR multicast)
    FAPI_INF("Configure BIST target");

    if (i_params.chiplets)
    {
        if (can_unicast(i_params.chiplets))
        {
            FAPI_DBG("Only one chiplet requested; using unicast");
            l_chiplet_number = get_set_bit_index(i_params.chiplets);
            FAPI_TRY(mod_get_chiplet_by_number(i_target, l_chiplet_number, l_chiplets_uc));
            l_chiplets_target = l_chiplets_uc;

            // If TP BIST, don't scan0 SBE region
            if (l_chiplet_number == 1)
            {
                l_scan0_region = REGION_ALL_BUT_PERV_SBE;
            }
        }
        else
        {
            FAPI_DBG("Setup and use multicast group 6");
            FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_6, i_params.chiplets, TARGET_STATE_FUNCTIONAL));
            l_chiplets_target = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_6);
        }
    }
    else
    {
        FAPI_DBG("No chiplet mask provided; opting for all good, no TP group");
        l_chiplets_target = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_GOOD_NO_TP);
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: SCAN0
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_SCAN0)
    {
        if (i_params.flags & i_params.bist_flags::SCAN0_REPR)
        {
            FAPI_INF("Scan0 all regions");
            FAPI_TRY(mod_scan0(l_chiplets_target, l_scan0_region, SCAN_TYPE_ALL));
        }
        else
        {
            FAPI_INF("Scan0 all regions minus repr");
            FAPI_TRY(mod_scan0(l_chiplets_target, l_scan0_region, SCAN_TYPE_NOT_REPR));
        }
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: ARRAYINIT
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_ARRAYINIT)
    {
        FAPI_INF("Do an arrayinit");
        FAPI_TRY(poz_chiplet_arrayinit(i_target));
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: RING_SETUP
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_RING_SETUP)
    {
        FAPI_INF("Scan load BIST programming");
        // TODO add in scan code once supported
        // FAPI_TRY(putRing(i_params.program.load));
        FAPI_DBG("BIST ring setup not yet implemented; check back later");
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: RING_PATCH
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_RING_PATCH)
    {
        FAPI_INF("Apply scan patches if needed");
        // TODO add in scan code once supported
        // FAPI_TRY(putRing(i_params.patch_image));
        FAPI_DBG("BIST ring patch not yet implemented; check back later");
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: REG_SETUP, GO, POLL
    ////////////////////////////////////////////////////////////////
    for (uint8_t outer_index = 0; outer_index < 16; outer_index++)
    {
        if (l_outer_loop_mask.getBit(outer_index))
        {
            FAPI_DBG("Bit %d present in outer loop mask; proceeding ...", outer_index);

            for (uint8_t inner_index = 0; inner_index < 16; inner_index++)
            {
                if (l_inner_loop_mask.getBit(inner_index))
                {
                    FAPI_DBG("Bit %d present in inner loop mask; proceeding ...", inner_index);

                    FAPI_TRY(poz_bist_execute(l_chiplets_target, i_params, outer_index, inner_index));
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: REG_CLEANUP
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_REG_CLEANUP)
    {
        FAPI_INF("Cleanup all SCOM registers");
        FAPI_TRY(mod_bist_reg_cleanup(l_chiplets_target));
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: COMPARE
    ////////////////////////////////////////////////////////////////
    if (i_params.flags & i_params.bist_flags::DO_COMPARE)
    {
        FAPI_INF("Compare scan chains against expects");
        // TODO add in scan code once supported
        // FAPI_TRY(getRing(i_params.program.hash));
        FAPI_DBG("BIST compare not yet implemented; check back later");
    }

    // TODO return a PG fail mask once supported


fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
