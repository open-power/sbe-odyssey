/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_bist.C $        */
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
#include <poz_perv_mod_chiplet_clocking_regs.H>
#include <poz_perv_mod_bist.H>
#include <poz_perv_utils.H>
#include <poz_scan_compare.H>
#include <poz_ring_ids.H>
#include <file_access.H>
#include <target_filters.H>
#include <endian.h>

using namespace fapi2;

enum POZ_BIST_Private_Constants
{
    // Hardcode mod_arrayinit i_runn_count to worst case value across all chip types
    ARRAYINIT_RUNN_CYCLES = 0x24FFFF,
};

// Determine if we only have one chiplet and can unicast BIST
static inline bool can_unicast(uint64_t i_chiplets)
{
    // Currently, only one bit is set means only one chiplet
    return (i_chiplets != 0 && (i_chiplets & (i_chiplets - 1)) == 0);
}

// Given a 64-bit mask, return the index of the first set bit
static inline uint8_t get_set_bit_index(uint64_t i_mask)
{
    return __builtin_clzll(i_mask);  // clzll == count leading zeros long long
}

static void print_bist_params(const bist_params& i_params)
{
    FAPI_DBG("BIST_PARAMS_VERSION = %d", i_params.BIST_PARAMS_VERSION);
    FAPI_DBG("stages = 0x%04x", i_params.stages);
    FAPI_DBG("flags = 0x%08x", i_params.flags);
    FAPI_DBG("chiplets = 0x%08x%08x",
             i_params.chiplets >> 32,
             i_params.chiplets & 0xFFFFFFFF);
    FAPI_DBG("uc_go_chiplets = 0x%08x%08x",
             i_params.uc_go_chiplets >> 32,
             i_params.uc_go_chiplets & 0xFFFFFFFF);
    FAPI_DBG("opcg_count = 0x%08x%08x",
             i_params.opcg_count >> 32,
             i_params.opcg_count & 0xFFFFFFFF);
    FAPI_DBG("idle_count = 0x%08x%08x",
             i_params.idle_count >> 32,
             i_params.idle_count & 0xFFFFFFFF);
    FAPI_DBG("linear_stagger = 0x%08x%08x",
             i_params.linear_stagger >> 32,
             i_params.linear_stagger & 0xFFFFFFFF);
    FAPI_DBG("zigzag_stagger = 0x%08x%08x",
             i_params.zigzag_stagger >> 32,
             i_params.zigzag_stagger & 0xFFFFFFFF);
    FAPI_DBG("lbist_opcg_align = 0x%08x%08x",
             i_params.lbist_opcg_align >> 32,
             i_params.lbist_opcg_align & 0xFFFFFFFF);
    FAPI_DBG("lbist_opcg1 = 0x%08x%08x",
             i_params.lbist_opcg1 >> 32,
             i_params.lbist_opcg1 & 0xFFFFFFFF);

    for (uint8_t mask_entry = 0; mask_entry < 2; mask_entry++)
    {
        if (i_params.opcg_count_adjust[mask_entry])
        {
            FAPI_DBG("opcg_count_adjust[%d] = 0x%08x%08x",
                     mask_entry,
                     i_params.opcg_count_adjust[mask_entry] >> 32,
                     i_params.opcg_count_adjust[mask_entry] & 0xFFFFFFFF);
        }
    }

    FAPI_DBG("max_polls = %u", i_params.max_polls);
    FAPI_DBG("poll_delay_hw = %u", i_params.poll_delay_hw);
    FAPI_DBG("poll_delay_sim = %u", i_params.poll_delay_sim);
    FAPI_DBG("scan0_types = 0x%04x", i_params.scan0_types);
    FAPI_DBG("lbist_scan_types = 0x%04x", i_params.lbist_scan_types);
    FAPI_DBG("base_regions = 0x%04x", i_params.base_regions);

    for (uint8_t chiplet_id = 0; chiplet_id < 64; chiplet_id++)
    {
        if (i_params.chiplets_regions[chiplet_id])
        {
            FAPI_DBG("chiplets_regions[%d] = 0x%04x",
                     chiplet_id, i_params.chiplets_regions[chiplet_id]);
        }
    }

    FAPI_DBG("outer_loop_mask = 0x%04x", i_params.outer_loop_mask);
    FAPI_DBG("inner_loop_mask = 0x%04x", i_params.inner_loop_mask);
    FAPI_DBG("program_1st_half (ASCII) = 0x%08x%08x%08x%08x",
             be32toh(((uint32_t*)i_params.program)[0]),
             be32toh(((uint32_t*)i_params.program)[1]),
             be32toh(((uint32_t*)i_params.program)[2]),
             be32toh(((uint32_t*)i_params.program)[3]));
    FAPI_DBG("program_2nd_half (ASCII) = 0x%08x%08x%08x%08x",
             be32toh(((uint32_t*)i_params.program)[4]),
             be32toh(((uint32_t*)i_params.program)[5]),
             be32toh(((uint32_t*)i_params.program)[6]),
             be32toh(((uint32_t*)i_params.program)[7]));
    FAPI_DBG("ring_patch_1st_half (ASCII) = 0x%08x%08x%08x%08x",
             be32toh(((uint32_t*)i_params.ring_patch)[0]),
             be32toh(((uint32_t*)i_params.ring_patch)[1]),
             be32toh(((uint32_t*)i_params.ring_patch)[2]),
             be32toh(((uint32_t*)i_params.ring_patch)[3]));
    FAPI_DBG("ring_patch_2nd_half (ASCII) = 0x%08x%08x%08x%08x",
             be32toh(((uint32_t*)i_params.ring_patch)[4]),
             be32toh(((uint32_t*)i_params.ring_patch)[5]),
             be32toh(((uint32_t*)i_params.ring_patch)[6]),
             be32toh(((uint32_t*)i_params.ring_patch)[7]));
}

static ReturnCode poz_bist_execute(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_chiplets_target,
    const std::vector<Target<TARGET_TYPE_PERV>>& i_chiplets_uc,
    const bist_params& i_params,
    const uint16_t i_enum_condition_a,
    const uint16_t i_enum_condition_b,
    bool& o_bist_halt_requested,
    bist_diags& o_diags)
{
    // Helper buffers produced from bist_params constituents
    const buffer<uint64_t> l_uc_go_chiplets_buffer = i_params.uc_go_chiplets;
    buffer<uint64_t> l_ctrl_chiplets_buffer = 0;

    // If uc_go_chiplets is equal to main chiplets mask, leave control chiplets as 0
    // Else, plug uc_go_chiplets into control  buffer
    if (i_params.uc_go_chiplets != i_params.chiplets)
    {
        l_ctrl_chiplets_buffer = i_params.uc_go_chiplets;
    }

    // Needed for OPCG count diagnostic readout
    OPCG_REG0_t OPCG_REG0;

    // Needed for go_mode = 'trigger' feature
    PCB_OPCG_GO_t PCB_OPCG_GO;
    Target<TARGET_TYPE_PERV> l_perv_chiplet;

    // Clear out completed stages from potential previous iterations
    o_diags.completed_stages &= ~(i_params.bist_stages::REG_SETUP |
                                  i_params.bist_stages::GO |
                                  i_params.bist_stages::POLL);


    ////////////////////////////////////////////////////////////////
    // STAGE: REG_SETUP
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::REG_SETUP)
    {
        FAPI_INF("Setup all SCOM registers");

        if (i_params.flags & i_params.bist_flags::ABIST_NOT_LBIST)
        {
            FAPI_TRY(mod_abist_setup(i_chiplets_target,
                                     i_params.base_regions,
                                     i_params.opcg_count,
                                     i_params.idle_count,
                                     i_params.chiplets_regions,
                                     i_params.flags & i_params.bist_flags::SKIP_FIRST_CLOCK,
                                     i_params.flags & i_params.bist_flags::SKIP_LAST_CLOCK));

            if (i_params.linear_stagger || i_params.zigzag_stagger)
            {
                OPCG_REG1_t OPCG_REG1;
                OPCG_REG1.set_INFINITE_MODE(runn_triggers_opcg_infinite(i_params.opcg_count));
                FAPI_TRY(mod_stagger_idle_setup(i_chiplets_target,
                                                i_chiplets_uc,
                                                i_params.idle_count,
                                                i_params.linear_stagger,
                                                i_params.zigzag_stagger,
                                                OPCG_REG1));
            }
        }
        else
        {
            // TODO consider enabling LBIST to use idle and stagger
            FAPI_TRY(mod_lbist_setup(i_chiplets_target,
                                     i_params,
                                     l_ctrl_chiplets_buffer,
                                     i_enum_condition_a,
                                     i_enum_condition_b));
        }

        o_diags.completed_stages |= i_params.bist_stages::REG_SETUP;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: GO
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::GO)
    {
        if (i_params.flags & i_params.bist_flags::TESTER_HANDSHAKE)
        {
            FAPI_TRY(mod_handshake_with_tester(i_chiplets_target));
        }

        FAPI_INF("Start BIST with OPCG GO");

        if (i_params.uc_go_chiplets)
        {
            FAPI_DBG("Enforcing OPCG_GO by chiplet via unicast");

            if (l_ctrl_chiplets_buffer.getBit(1))
            {
                FAPI_TRY(mod_get_chiplet_by_number(i_chiplets_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>(), 1, l_perv_chiplet));
                FAPI_INF("Setting up scom registers to run trigger mode on pervasive chiplet ...");
                FAPI_TRY(mod_trigger_start(l_perv_chiplet));
            }

            for (auto& chiplet : i_chiplets_uc)
            {
                if (l_uc_go_chiplets_buffer.getBit(chiplet.getChipletNumber()))
                {
                    FAPI_TRY(mod_opcg_go(chiplet));
                }
            }

            if (l_ctrl_chiplets_buffer.getBit(1))
            {
                OPCG_REG0 = 0;
                PCB_OPCG_GO = 0;
                FAPI_TRY(OPCG_REG0.putScom(l_perv_chiplet));
                FAPI_TRY(PCB_OPCG_GO.putScom(l_perv_chiplet));
            }
        }
        else
        {
            FAPI_TRY(mod_opcg_go(i_chiplets_target));
        }

        o_diags.completed_stages |= i_params.bist_stages::GO;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: POLL
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::POLL)
    {
        FAPI_INF("Poll for DONE or HALT");
        FAPI_TRY(mod_bist_poll(i_chiplets_target,
                               o_bist_halt_requested,
                               i_params.flags & i_params.bist_flags::POLL_ABIST_DONE,
                               i_params.flags & i_params.bist_flags::ASSERT_ABIST_DONE,
                               i_params.max_polls,
                               i_params.poll_delay_hw,
                               i_params.poll_delay_sim,
                               runn_triggers_opcg_infinite(i_params.opcg_count)));
        o_diags.completed_stages |= i_params.bist_stages::POLL;
        o_diags.pass_counter++;

        if (i_params.flags & i_params.bist_flags::TESTER_HANDSHAKE)
        {
            FAPI_TRY(mod_handshake_with_tester(i_chiplets_target));
        }

        if (i_params.flags & i_params.bist_flags::DIAGNOSTICS)
        {
            for (auto& chiplet : i_chiplets_uc)
            {
                FAPI_TRY(OPCG_REG0.getScom(chiplet));
                FAPI_DBG("OPCG cycles elapsed for chiplet %d: %lu",
                         chiplet.getChipletNumber(),
                         OPCG_REG0.get_LOOP_COUNT());
                o_diags.opcg_counts[chiplet.getChipletNumber()] = OPCG_REG0.get_LOOP_COUNT();
            }
        }
    }


fapi_try_exit:
    return current_err;
}

ReturnCode poz_bist(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, const bist_params& i_params, bist_diags& o_diags,
    hwp_data_ostream& o_stream, const uint16_t i_dict_def) // TODO can we remove i_dict_def?
{
    FAPI_INF("Entering poz_bist...");

    // All required pervasive targets and target containers
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_target;
    std::vector<Target<TARGET_TYPE_PERV>> l_chiplets_uc;

    // Needed for unicast condition
    uint8_t l_chiplet_number = 0;
    bool l_is_tp_bist = false;

    // Regions are handled a little differently for TP BIST
    // TODO do we need a more complete solution for P11T EQ since region 13 is not PLL there?
    clock_region l_all_active_regions = REGION_ALL_BUT_PLL;
    uint16_t l_tp_regions = i_params.base_regions;

    // Variables for accessing pak scan data
    char l_program[sizeof(i_params.program)] = {};
    char l_load_dir[9] = {'l', 'b', 'i', 's', 't', '/', 'm', '/'};
    char l_load_path[sizeof(l_load_dir) + sizeof(l_program) - 1] = {};
    char l_compare_hash_dir[12] = {'l', 'b', 'i', 's', 't', '/', 'm', '/', 'c', 'h', '/'};
    char l_compare_hash_path[sizeof(l_compare_hash_dir) + sizeof(l_program) - 1] = {};
    char l_compare_mask_dir[14] = {'l', 'b', 'i', 's', 't', '/', 'm', '/', 'c', 'm', '/', '0', '/'};
    char l_unload_mask_dir[14] = {'l', 'b', 'i', 's', 't', '/', 'm', '/', 'c', 'm', '/', '0', '/'};
    const void* l_hash_file_data = NULL;
    size_t l_hash_file_size;
    const hash_data* l_hash_data = NULL;
    bool l_hash_file_loaded = false;

    if (i_params.flags & i_params.bist_flags::ABIST_NOT_LBIST)
    {
        l_load_dir[0] = 'a';
        l_compare_hash_dir[0] = 'a';
        l_compare_mask_dir[0] = 'a';
        l_unload_mask_dir[0] = 'a';
    }

    if (i_params.flags & i_params.bist_flags::SYS_BIST_PAK)
    {
        l_load_dir[6] = 's';
        l_compare_hash_dir[6] = 's';
        l_compare_mask_dir[6] = 's';
        l_unload_mask_dir[6] = 's';
    }

    // Helper buffers produced from bist_params constituents
    const buffer<uint64_t> l_chiplets_mask = i_params.chiplets;
    const buffer<uint16_t> l_outer_loop_mask = i_params.outer_loop_mask;
    const buffer<uint16_t> l_inner_loop_mask = i_params.inner_loop_mask;

    // Needed for burnin stress
    const bool repeat_exec_loop = l_inner_loop_mask.getBit(15);

    // Stuff for saving register values to restore after cleanup
    const bool l_allow_fast_cleanup = !(i_params.stages & i_params.bist_flags::DIAGNOSTICS);
    const bool l_save_uc_reg_values = (i_params.stages & i_params.bist_stages::REG_SETUP) &&
                                      (i_params.stages & i_params.bist_stages::REG_CLEANUP) &&
                                      (!(i_params.flags & i_params.bist_flags::ABIST_NOT_LBIST));
    CPLT_CTRL0_t CPLT_CTRL0;
    OPCG_ALIGN_t OPCG_ALIGN;
    CPLT_CTRL0_t l_cplt_ctrl0_save[64];
    OPCG_ALIGN_t l_opcg_align_save[64];

    // To temporarily store return codes for evaluation before throwing error
    ReturnCode l_rc;

    // Assert the provided bist_params struct is the expected version
    FAPI_ASSERT(i_params.BIST_PARAMS_VERSION == BIST_PARAMS_CURRENT_VERSION,
                BAD_BIST_PARAMS_FORMAT().
                set_BIST_PARAMS(i_params.BIST_PARAMS_VERSION),
                "BIST_PARAMS_VERSION mismatch (expected = %d, got = %d)",
                BIST_PARAMS_CURRENT_VERSION, i_params.BIST_PARAMS_VERSION);
    FAPI_DBG("Received bist_params version = %d; check passed", i_params.BIST_PARAMS_VERSION);

    // Print all BIST parameters for debug
    print_bist_params(i_params);

    // Configure our target (unicast OR multicast)
    FAPI_INF("Configure BIST target");

    if (i_params.chiplets)
    {
        if (can_unicast(i_params.chiplets))
        {
            Target<TARGET_TYPE_PERV> l_chiplet;
            FAPI_DBG("Only one chiplet requested; using unicast");
            l_chiplet_number = get_set_bit_index(i_params.chiplets);
            FAPI_TRY(mod_get_chiplet_by_number(i_target, l_chiplet_number, l_chiplet));
            l_chiplets_target = l_chiplet;

            // Safe scan0 for TP depends on PCB mux; just default to regions requested for BIST
            if (l_chiplet_number == 1)
            {
                l_is_tp_bist = true;

                if (i_params.chiplets_regions[1])
                {
                    l_tp_regions = i_params.chiplets_regions[1];
                }

                l_all_active_regions = (clock_region)l_tp_regions;
            }
        }
        else
        {
            FAPI_ASSERT(!l_chiplets_mask.getBit(1),
                        MIXED_TP_CHIPLET_BIST_REQUESTED().
                        set_CHIPLETS(i_params.chiplets),
                        "Cannot currently run BIST on a combination of TP and non-TP chiplets");

            FAPI_DBG("Setup and use multicast group 6");
            FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_6, i_params.chiplets, TARGET_STATE_FUNCTIONAL));
            l_chiplets_target = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_6);
        }
    }
    else
    {
        FAPI_DBG("No chiplet mask provided; opting for all good, no TP group");
        FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_target));
    }

    // Set up unicast chiplets vector now that parent container is defined
    l_chiplets_uc = l_chiplets_target.getChildren<TARGET_TYPE_PERV>();

    // Calculate PCB mux path
    switch (i_params.flags & i_params.bist_flags::PCB_MUX_MASK)
    {
        case i_params.bist_flags::PCB_MUX_UNCHANGED:
            FAPI_DBG("Leaving PCB mux unchanged");
            break;

        case i_params.bist_flags::PCB_MUX_FSI2PCB:
            FAPI_DBG("Configuring PCB mux for FSI2PCB");
            FAPI_TRY(mod_switch_pcbmux(i_target, FSI2PCB));
            break;

        case i_params.bist_flags::PCB_MUX_PIB2PCB:
            FAPI_DBG("Configuring PCB mux for PIB2PCB");
            FAPI_TRY(mod_switch_pcbmux(i_target, PIB2PCB));
            break;

        case i_params.bist_flags::PCB_MUX_PCB2PCB:
            FAPI_DBG("Configuring PCB mux for PCB2PCB");
            FAPI_TRY(mod_switch_pcbmux(i_target, PCB2PCB));
            break;
    }

    // Resolve any program name indirection
    strcpy(l_program, i_params.program);

    if (!strcmp(l_program, "istep"))
    {
        char l_istep_program_path[14] = {'l', 'b', 'i', 's', 't', '/', 'm', '/', 'i', 's', 't', 'e', 'p'};
        const void* l_istep_program = NULL;
        size_t l_istep_program_size;

        if (i_params.flags & i_params.bist_flags::ABIST_NOT_LBIST)
        {
            l_istep_program_path[0] = 'a';
        }

        if (i_params.flags & i_params.bist_flags::SYS_BIST_PAK)
        {
            l_istep_program_path[6] = 's';
        }

        l_rc = loadEmbeddedFile(i_target, l_istep_program_path, l_istep_program, l_istep_program_size);

        if (l_rc == FAPI2_RC_FILE_NOT_FOUND)
        {
            FAPI_INF("istep program requested, but istep program file not found in the bist.pak");
            current_err = FAPI2_RC_SUCCESS;
        }
        else if (l_rc == FAPI2_RC_SUCCESS)
        {
            strcpy(l_program, (const char*)l_istep_program);
            FAPI_INF("Running istep program (1st half ASCII) = 0x%08x%08x%08x%08x",
                     be32toh(((uint32_t*)l_program)[0]),
                     be32toh(((uint32_t*)l_program)[1]),
                     be32toh(((uint32_t*)l_program)[2]),
                     be32toh(((uint32_t*)l_program)[3]));
            FAPI_INF("Running istep program (2nd half ASCII) = 0x%08x%08x%08x%08x",
                     be32toh(((uint32_t*)l_program)[4]),
                     be32toh(((uint32_t*)l_program)[5]),
                     be32toh(((uint32_t*)l_program)[6]),
                     be32toh(((uint32_t*)l_program)[7]));
            FAPI_TRY(freeEmbeddedFile(l_istep_program));
        }
        else
        {
            FAPI_TRY(l_rc, "loadEmbeddedFile failed");
        }
    }

    strcpy(l_compare_hash_path, l_compare_hash_dir);
    strcat(l_compare_hash_path, l_program);

    if (i_params.stages & (i_params.bist_stages::RING_SETUP |
                           i_params.bist_stages::COMPARE |
                           i_params.bist_stages::UNLOAD))
    {
        FAPI_INF("Loading compare hash file for BIST program config keys");
        FAPI_TRY(loadEmbeddedFile(i_target, l_compare_hash_path, l_hash_file_data, l_hash_file_size));
        l_hash_file_loaded = true;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: SCAN0
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::SCAN0)
    {
        FAPI_INF("Do a scan0");
        FAPI_TRY(mod_scan0(l_chiplets_target, l_all_active_regions, i_params.scan0_types,
                           i_params.flags & i_params.bist_flags::SCAN0_ARY_FILL));
        o_diags.completed_stages |= i_params.bist_stages::SCAN0;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: ARRAYINIT
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::ARRAYINIT)
    {
        FAPI_INF("Do an arrayinit");

        uint8_t l_no_repr_lbist;
        FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_EC_FEATURE_NO_REPR_LBIST, i_target, l_no_repr_lbist));

        if (l_no_repr_lbist)
        {
            // Scan load VPD RTG
            FAPI_TRY(putRing(i_target, ring_id::chiplet_rtg));
            FAPI_TRY(putRing(i_target, ring_id::core_rtg));
            FAPI_TRY(putRing(i_target, ring_id::sh_rtg));

            // Scan load generic RTG optimizations for ABIST
            FAPI_TRY(putRing(l_chiplets_target, ring_id::abist_chiplet_rtg));
            FAPI_TRY(putRing(l_chiplets_target, ring_id::abist_core_rtg));
            FAPI_TRY(putRing(l_chiplets_target, ring_id::abist_sh_rtg));
        }

        // Do the arrayinit
        FAPI_TRY(mod_arrayinit(l_chiplets_target, l_all_active_regions, ARRAYINIT_RUNN_CYCLES, false));

        if (l_no_repr_lbist)
        {
            // Post-arrayinit scan0
            FAPI_TRY(mod_scan0(l_chiplets_target, l_all_active_regions, i_params.scan0_types,
                               i_params.flags & i_params.bist_flags::SCAN0_ARY_FILL));
        }

        o_diags.completed_stages |= i_params.bist_stages::ARRAYINIT;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: RING_SETUP
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::RING_SETUP)
    {
        FAPI_INF("Scan load BIST programming");

        // Load base image if it exists
        strcpy(l_load_path, l_load_dir);
        strcat(l_load_path, "base_image_0");
        // The compare hash file's third to last byte indicates which base image to use
        strhex(l_load_path + 19, ((uint8_t*)l_hash_file_data)[l_hash_file_size - 3], 1);
        FAPI_DBG("Attempting to load base BIST image");
        FAPI_TRY(putRing(l_chiplets_target, l_load_path));

        // Load program image
        strcpy(l_load_path, l_load_dir);
        strcat(l_load_path, l_program);
        FAPI_DBG("Attempting to load overlay BIST image");
        FAPI_TRY(putRing(l_chiplets_target, l_load_path));

        o_diags.completed_stages |= i_params.bist_stages::RING_SETUP;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: RING_PATCH
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::RING_PATCH)
    {
        FAPI_INF("Apply scan patches if needed");
        FAPI_TRY(putRing(l_chiplets_target, i_params.ring_patch));
        o_diags.completed_stages |= i_params.bist_stages::RING_PATCH;
    }

    ////////////////////////////////////////////////////////////////
    // STAGE: REG_SETUP, GO, POLL
    ////////////////////////////////////////////////////////////////
    if (l_save_uc_reg_values)
    {
        // If fast cleanup requested, save just the first chiplet's register values
        if (l_allow_fast_cleanup)
        {
            FAPI_TRY(CPLT_CTRL0.getScom(l_chiplets_uc[0]));
            FAPI_TRY(OPCG_ALIGN.getScom(l_chiplets_uc[0]));
        }
        // Else, save via unicast
        else
        {
            for (auto& cplt : l_chiplets_uc)
            {
                auto i = cplt.getChipletNumber();
                FAPI_TRY(l_cplt_ctrl0_save[i].getScom(cplt));
                FAPI_TRY(l_opcg_align_save[i].getScom(cplt));
            }
        }
    }

    do
    {
        bool l_bist_halt_requested = false;

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

                        FAPI_TRY(poz_bist_execute(l_chiplets_target,
                                                  l_chiplets_uc,
                                                  i_params,
                                                  outer_index,
                                                  inner_index,
                                                  l_bist_halt_requested,
                                                  o_diags));
                    }

                    if (l_bist_halt_requested)
                    {
                        break;
                    }
                }
            }

            if (l_bist_halt_requested)
            {
                break;
            }
        }

        if (l_bist_halt_requested)
        {
            break;
        }

    }
    while(repeat_exec_loop);

    ////////////////////////////////////////////////////////////////
    // STAGE: REG_CLEANUP
    ////////////////////////////////////////////////////////////////
    if (i_params.stages & i_params.bist_stages::REG_CLEANUP)
    {
        FAPI_INF("Cleanup all SCOM registers");
        FAPI_TRY(mod_bist_reg_cleanup(l_chiplets_target, l_is_tp_bist));
        o_diags.completed_stages |= i_params.bist_stages::REG_CLEANUP;
    }

    if (l_save_uc_reg_values)
    {
        // If fast cleanup requested, reapply register values via multicast
        if (l_allow_fast_cleanup)
        {
            FAPI_TRY(CPLT_CTRL0.putScom(l_chiplets_target));
            FAPI_TRY(OPCG_ALIGN.putScom(l_chiplets_target));
        }
        // Else, reapply via unicast
        else
        {
            for (auto& cplt : l_chiplets_uc)
            {
                auto i = cplt.getChipletNumber();
                FAPI_TRY(l_cplt_ctrl0_save[i].putScom(cplt));
                FAPI_TRY(l_opcg_align_save[i].putScom(cplt));
            }
        }
    }

    // Read compare hash file then run compare and/or unload together
    if (i_params.stages & (i_params.bist_stages::COMPARE | i_params.bist_stages::UNLOAD))
    {
        FAPI_INF("Compare scan chains against expects and/or unload them");

        bool l_incomplete_compare = false;

        // The compare hash file's second to last byte indicates which compare care mask set to use
        strhex(l_compare_mask_dir + 11, ((uint8_t*)l_hash_file_data)[l_hash_file_size - 2], 1);
        // The compare hash file's last byte indicates which unload care mask set to use
        strhex(l_unload_mask_dir + 11, ((uint8_t*)l_hash_file_data)[l_hash_file_size - 1], 1);

        l_hash_data = (hash_data*)l_hash_file_data;

        // Queue up all rings to be unloaded; compare will flag any which can be skipped
        for (auto& cplt : l_chiplets_uc)
        {
            CPLT_CTRL2_t CPLT_CTRL2;
            CPLT_CTRL2.getScom(cplt);

            for (uint8_t i = 0; i < l_hash_file_size / sizeof(hash_data); i++)
            {
                uint32_t l_base_ring_address, l_ring_address;
                bool l_skip_unload = false;

                // Skip if this ring address doesn't apply to the input target
                if (!(get_hash_target_addresses(cplt,
                                                l_hash_data[i],
                                                l_base_ring_address,
                                                l_ring_address)))
                {
                    continue;
                }

                // Skip if this ring isn't configured for the input target
                const uint16_t l_ring_region = get_ring_region(l_base_ring_address);

                if (!(CPLT_CTRL2.getBits<4, 16>() & l_ring_region))
                {
                    FAPI_DBG("Ring 0x%08x region 0x%04x deconfigured per PG; skipping scans ...",
                             l_ring_address,
                             l_ring_region);
                    continue;
                }

                ////////////////////////////////////////////////////////////////
                // STAGE: COMPARE
                ////////////////////////////////////////////////////////////////
                if (i_params.stages & i_params.bist_stages::COMPARE)
                {
                    l_rc = poz_scan_compare(cplt,
                                            l_base_ring_address,
                                            l_compare_mask_dir,
                                            be32toh(l_hash_data[i].hash_value),
                                            l_hash_data[i].care_mask_id_override);

                    if (l_rc == FAPI2_RC_SUCCESS)
                    {
                        l_skip_unload = true;
                        FAPI_DBG("Scan compare passed; unload can be skipped ...");
                    }
                    else if (l_rc == FAPI2_RC_FALSE)
                    {
                        FAPI_DBG("Adding 0x%04x to chiplet %d fail regions",
                                 l_ring_region,
                                 cplt.getChipletNumber());
                        o_diags.failing_regions[cplt.getChipletNumber()] |= l_ring_region;
                    }
                    else if(l_rc == FAPI2_RC_FILE_NOT_FOUND)
                    {
                        l_incomplete_compare = true;
                        FAPI_DBG("No compare care mask found for ring 0x%08x", l_ring_address);
                    }
                    else
                    {
                        FAPI_TRY(l_rc, "poz_scan_compare failed");
                    }
                }

                ////////////////////////////////////////////////////////////////
                // STAGE: UNLOAD
                ////////////////////////////////////////////////////////////////
                if ((i_params.stages & i_params.bist_stages::UNLOAD) && !l_skip_unload)
                {
                    char l_unload_mask_file_path[21];
                    char* l_fpath_write_ptr = NULL;

                    l_fpath_write_ptr = stpcpy(l_unload_mask_file_path, l_unload_mask_dir);
                    strhex(l_fpath_write_ptr,
                           l_hash_data[i].care_mask_id_override ?
                           (l_hash_data[i].care_mask_id_override << 24) | (l_base_ring_address & 0x00FFFFFF) :
                           l_base_ring_address,
                           8);
                    // Write trailing null ptr since strhex doesn't do that automatically
                    l_fpath_write_ptr[8] = 0;
                    FAPI_INF("Scanning out ring 0x%08x on chiplet %d",
                             l_base_ring_address,
                             cplt.getChipletNumber());
                    FAPI_TRY(poz_write_tlv_ring_unload(cplt,
                                                       l_base_ring_address,
                                                       l_unload_mask_file_path,
                                                       o_stream));
                }
            }
        }

        // Only set bits for completed stages if stages ran for all rings
        if ((i_params.stages & i_params.bist_stages::COMPARE) && !l_incomplete_compare)
        {
            o_diags.completed_stages |= i_params.bist_stages::COMPARE;
        }

        if (i_params.stages & i_params.bist_stages::UNLOAD)
        {
            o_diags.completed_stages |= i_params.bist_stages::UNLOAD;
        }
    }

    // Write TLV diagnostics block out through stream
    if (i_params.flags & i_params.bist_flags::DIAGNOSTICS)
    {
        FAPI_INF("Streaming out poz_bist diagnostics");

        FAPI_TRY(poz_write_tlv_diags(i_target, i_dict_def, sizeof(bist_diags), &o_diags, o_stream));
    }

fapi_try_exit:

    if (l_hash_file_loaded)
    {
        freeEmbeddedFile(l_hash_file_data);
    }

    FAPI_INF("Exiting poz_bist...");
    return current_err;
}
