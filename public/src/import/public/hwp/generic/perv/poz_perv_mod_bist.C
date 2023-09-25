/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_bist.C $ */
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
/// @file  poz_perv_mod_bist.C
/// @brief Contains definitions for modules bist_poll and bist_reg_cleanup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_bist.H>
#include <poz_perv_mod_chiplet_clocking.H>
#include <poz_perv_mod_chiplet_clocking_regs.H>
#include <poz_perv_mod_bist_common.H>
#include <target_filters.H>
#include <poz_perv_mod_misc.H>

using namespace fapi2;

enum POZ_PERV_MOD_BIST_Private_Constants
{
};


ReturnCode mod_stagger_idle_setup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    const std::vector<Target<TARGET_TYPE_PERV>>& i_chiplets_uc,
    const uint64_t i_base_idle,
    const uint64_t i_linear_stagger,
    const uint64_t i_zigzag_stagger,
    const uint64_t i_base_opcg_reg1)
{
    OPCG_REG1_t OPCG_REG1;
    OPCG_REG1 = i_base_opcg_reg1;

    uint64_t l_idle_count = i_base_idle;
    uint64_t l_chiplets_idle_count[64] = {0};

    FAPI_INF("Entering ...");

    if (i_zigzag_stagger)
    {
        uint8_t l_ex_zigzag_stagger;

        FAPI_DBG("Calculating zigzag stagger OPCG idle");
        FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_EC_FEATURE_EX_ZIGZAG_STAGGER,
                               i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>(),
                               l_ex_zigzag_stagger));

        if (l_ex_zigzag_stagger)
        {
            for (auto& targ : i_target.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_EX))
            {
                FAPI_DBG("Applying zigzag stagger to EX chiplet %d", targ.getChipletNumber());
                l_chiplets_idle_count[targ.getChipletNumber()] += i_zigzag_stagger;
            }

            for (auto& targ : i_target.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_EXZIG))
            {
                FAPI_DBG("Applying extra zigzag stagger to zig EX chiplet %d", targ.getChipletNumber());
                l_chiplets_idle_count[targ.getChipletNumber()] += i_zigzag_stagger;
            }
        }
        else
        {
            FAPI_DBG("***WARNING*** zigzag stagger requested but not implemented for this chip");
        }
    }

    FAPI_DBG("Applying staggered OPCG idle via unicast");

    for (auto& targ : i_chiplets_uc)
    {
        OPCG_REG1.insertFromRight<0, 36>(l_idle_count + l_chiplets_idle_count[targ.getChipletNumber()]);
        FAPI_TRY(OPCG_REG1.putScom(targ));
        l_idle_count += i_linear_stagger;
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_bist_poll(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_AND > & i_target,
    bool i_poll_abist_done,
    bool i_assert_abist_done,
    uint32_t i_max_polls,
    uint32_t i_poll_delay_hw,
    uint32_t i_poll_delay_sim)
{
    FAPI_INF("Entering ...");

    CPLT_STAT0_t CPLT_STAT0;
    PCB_OPCG_STOP_t PCB_OPCG_STOP;
    OPCG_REG0_t OPCG_REG0;
    FSXCOMP_FSXLOG_SCRATCH_REGISTER_11_t SCRATCH_REGISTER_11;
    uint32_t l_total_polls = 0;
    // Infinite polling if negative signed (or massive unsigned) number
    const bool l_infinite_polling = i_max_polls > 0x7FFFFFFF;
    auto l_chip = i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>();

    if (i_poll_abist_done)
    {
        FAPI_INF("Watching OPCG_DONE, ABIST_DONE, and BIST_HALT");
    }
    else
    {
        FAPI_INF("Watching OPCG_DONE and BIST_HALT");
    }

    if (l_infinite_polling)
    {
        FAPI_DBG("Poll count value triggers infinite polling");
    }

    while ((l_total_polls < i_max_polls) || l_infinite_polling)
    {
        FAPI_TRY(SCRATCH_REGISTER_11.getScom(l_chip));

        if (SCRATCH_REGISTER_11.getBit<27>() == 1)
        {
            FAPI_INF("BIST_HALT observed");
            break;
        }

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

        FAPI_TRY(fapi2::delay(i_poll_delay_hw, i_poll_delay_sim));
    }

    FAPI_DBG("Total poll count: %d", l_total_polls);

    if (CPLT_STAT0.get_OPCG_DONE() == 0)
    {
        FAPI_INF("Forcibly stopping OPCG");
        PCB_OPCG_STOP = 0;
        PCB_OPCG_STOP.set_PCB_OPCGSTOP(1);
        FAPI_TRY(PCB_OPCG_STOP.putScom(i_target));
    }

    FAPI_ASSERT((l_total_polls < i_max_polls) || l_infinite_polling,
                fapi2::DONE_HALT_NOT_SET()
                .set_PERV_CPLT_STAT0(CPLT_STAT0)
                .set_POLL_COUNT(l_total_polls)
                .set_POLL_DELAY_HW(i_poll_delay_hw)
                .set_POLL_DELAY_SIM(i_poll_delay_sim)
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
    const bool i_skip_net_ctrl0)
{
    CPLT_CTRL0_t CPLT_CTRL0;                    ///< 0x00000
    CPLT_CTRL1_t CPLT_CTRL1;                    ///< 0x00001
    CPLT_CONF0_t CPLT_CONF0;                    ///< 0x00008
    NET_CTRL0_t NET_CTRL0;                      ///< 0xF0040
    FSXCOMP_FSXLOG_SCRATCH_REGISTER_11_t SCRATCH_REGISTER_11;
    auto l_chip = i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>();
    FAPI_INF("Entering ...");

    FAPI_INF("Zeroing out OPCG and region registers.");
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_REG0, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_REG1, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_REG2, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::SCAN_REGION_TYPE, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::CLK_REGION, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::BIST, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_CAPT1, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_CAPT2, 0));
    FAPI_TRY(putScom(i_target, scomt::poz::OPCG_CAPT3, 0));

    FAPI_INF("Restoring CPLT_CTRL0 register settings.");
    CPLT_CTRL0.set_ABSTCLK_MUXSEL(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

    FAPI_INF("Restoring CPLT_CTRL1 register settings.");
    CPLT_CTRL1.set_MULTICYCLE_TEST_FENCE(1);
    FAPI_TRY(CPLT_CTRL1.putScom_CLEAR(i_target));

    FAPI_INF("Restoring CPLT_CONF0 register settings.");
    CPLT_CONF0.set_SDIS_N(1);
    FAPI_TRY(CPLT_CONF0.putScom_CLEAR(i_target));

    if (!i_skip_net_ctrl0)
    {
        FAPI_INF("Restoring NET_CTRL0 register settings.");
        NET_CTRL0.set_FENCE_EN(1);
        FAPI_TRY(NET_CTRL0.putScom_SET(i_target));
    }

    FAPI_INF("Resetting BIST halt signal.");
    FAPI_TRY(SCRATCH_REGISTER_11.getScom(l_chip));
    SCRATCH_REGISTER_11.clearBit<27>();
    FAPI_TRY(SCRATCH_REGISTER_11.putScom(l_chip));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}


ReturnCode mod_lbist_setup(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
    const bist_params& i_params,
    const uint64_t i_ctrl_chiplets,
    const uint16_t i_lbist_sequence,
    const uint16_t i_lbist_weight)
{
    CPLT_CTRL0_t CPLT_CTRL0_set, CPLT_CTRL0_clear;
    CPLT_CTRL1_t CPLT_CTRL1;
    CPLT_CONF0_t CPLT_CONF0;
    CPLT_CONF1_t CPLT_CONF1;
    CLK_REGION_t CLK_REGION;
    OPCG_REG0_t OPCG_REG0;
    OPCG_REG0_t OPCG_REG0_STATE;
    OPCG_REG1_t OPCG_REG1;
    OPCG_REG2_t OPCG_REG2;
    OPCG_CAPT1_t OPCG_CAPT1;
    OPCG_CAPT2_t OPCG_CAPT2;
    OPCG_CAPT3_t OPCG_CAPT3;
    SCAN_REGION_TYPE_t SCAN_REGION_TYPE;
    NET_CTRL0_t NET_CTRL0;
    OPCG_ALIGN_t OPCG_ALIGN;

    auto l_chiplets_uc = i_target.getChildren<TARGET_TYPE_PERV>();
    buffer<uint64_t> l_ctrl_chiplets = i_ctrl_chiplets;
    const bool l_trigger_mode = (i_ctrl_chiplets != 0);
    const struct STUMPSWeightSettings stumpsWeightSetting = stumpsWeightSettings[i_lbist_weight];  //grab weights enum
    const struct OPCGCaptureSettings opcgCaptureSetting = opcgCaptureSettings[i_lbist_sequence];   //grab capture enum

    // NET_CTRL0
    if (!(i_params.flags & i_params.bist_flags::CHIPLET_FENCE_ACTIVE))
    {
        NET_CTRL0.set_FENCE_EN(1); // bit18
        FAPI_INF("LBIST: Writing NET_CTRL0 ...");
        FAPI_TRY(NET_CTRL0.putScom_CLEAR(i_target));
    }

    // CPLT_CTRL0
    CPLT_CTRL0_set.set_SYNCCLK_MUXSEL(1); // bit1
    CPLT_CTRL0_set.set_FLUSHMODE_INH(1); // bit2
    CPLT_CTRL0_set.set_FORCE_ALIGN(1); // bit3
    CPLT_CTRL0_set.set_VITL_PROTECTION(1); // bit6
    CPLT_CTRL0_set.set_DETERMINISTIC_TEST_EN(1); // bit13
    CPLT_CTRL0_set.set_CONSTRAIN_SAFESCAN(1); // bit14
    CPLT_CTRL0_set.set_RRFA_TEST_EN(1); // bit15

    if (i_params.flags & i_params.bist_flags::LBIST_COMBINED)
    {
        CPLT_CTRL0_clear.set_ARY_WRT_THRU(1); // bit4
    }
    else
    {
        CPLT_CTRL0_set.set_ARY_WRT_THRU(1); // bit4
    }

    if (i_params.flags & i_params.bist_flags::INT_MODE)
    {
        CPLT_CTRL0_set.set_BSC_INTMODE(1); // bit29
    }
    else
    {
        CPLT_CTRL0_clear.set_BSC_INTMODE(1); // bit29
    }

    FAPI_INF("LBIST: Writing CPLT_CTRL0 ...");
    FAPI_TRY(CPLT_CTRL0_set.putScom_SET(i_target));

    if (CPLT_CTRL0_clear != 0)
    {
        FAPI_TRY(CPLT_CTRL0_clear.putScom_CLEAR(i_target));
    }

    // CPLT_CTRL1
    CPLT_CTRL1.set_MULTICYCLE_TEST_FENCE(1);
    FAPI_INF("LBIST: Writing CPLT_CTRL1 ...");
    FAPI_TRY(CPLT_CTRL1.putScom_SET(i_target));

    // SCAN_REGION_TYPE & CLK_REGION REGS
    // Multicast base CLK regions and SCAN_TYPE
    FAPI_TRY(CLK_REGION.getScom(i_target));
    CLK_REGION.insertFromRight<CLK_REGION_CLOCK_REGION_PERV, 16>(i_params.base_regions);
    FAPI_TRY(CLK_REGION.putScom(i_target));

    FAPI_TRY(SCAN_REGION_TYPE.getScom(i_target));
    SCAN_REGION_TYPE.insertFromRight<SCAN_REGION_TYPE_SCAN_REGION_PERV, 16>(i_params.base_regions);
    SCAN_REGION_TYPE.insertFromRight<SCAN_REGION_TYPE_SCAN_TYPE_FUNC, 12>(i_params.lbist_scan_types);
    FAPI_TRY(SCAN_REGION_TYPE.putScom(i_target));

    // If we know there are custom regions by chiplet, unicast them
    FAPI_TRY(apply_regions_by_chiplet(l_chiplets_uc, CLK_REGION, CLK_REGION.addr,
                                      CLK_REGION_CLOCK_REGION_PERV, i_params.chiplets_regions));
    FAPI_TRY(apply_regions_by_chiplet(l_chiplets_uc, SCAN_REGION_TYPE, SCAN_REGION_TYPE.addr,
                                      SCAN_REGION_TYPE_SCAN_REGION_PERV, i_params.chiplets_regions));

    // OPCG_ALIGN
    OPCG_ALIGN = i_params.lbist_opcg_align;
    FAPI_INF("LBIST: Writing ALIGN Register ...");
    FAPI_TRY(OPCG_ALIGN.putScom(i_target));

    // OPCG_REG0
    // Multicast reciever mode to all chiplets. Control mode will later be assigned via unicast to all control chiplets (if any)
    if(l_trigger_mode)
    {
        OPCG_REG0.set_OPCG_TRIGGER_CTRL_MODE(0);
        OPCG_REG0.set_OPCG_TRIGGER_RCVR_MODE(1);
    }

    OPCG_REG0.set_LOOP_COUNT(i_params.opcg_count);
    FAPI_TRY(OPCG_REG0.putScom(i_target));

    // Save state of OPCG0 register
    OPCG_REG0_STATE = OPCG_REG0;

    // Trigger and LOOP_COUNT override
    // Trying to balance out LBIST runtime so that LBIST runs roughly for the same duration in all chiplets
    // shift_val = (# of bits in array entry) - (# of bits we want to extract) - (bits we care about * (first 5bits of chiplet_number))
    for (auto& targ : l_chiplets_uc)
    {
        const int chiplet_number = targ.getChipletNumber();
        const int array_index = chiplet_number >> 5;
        const int shift_val = 64 - 2 - (2 * (chiplet_number & 0x1F));
        uint8_t targ_opcg_divisor = (i_params.opcg_count_adjust[array_index] >> shift_val) & 0x03;

        // Reapply saved state of OPCG0 in case previous iteration of loop modified OPCG_REG0
        OPCG_REG0 = OPCG_REG0_STATE;

        // Trigger setting override
        if (l_ctrl_chiplets.getBit(chiplet_number))
        {
            OPCG_REG0.set_OPCG_TRIGGER_CTRL_MODE(1);
            OPCG_REG0.set_OPCG_TRIGGER_RCVR_MODE(0);
        }

        // Loop count override
        OPCG_REG0.set_LOOP_COUNT(i_params.opcg_count / (targ_opcg_divisor + 1));

        // putscom only if we indicated a ctrl chiplet, change in loop count, or both!
        if(l_ctrl_chiplets.getBit(chiplet_number) || targ_opcg_divisor)
        {
            FAPI_TRY(OPCG_REG0.putScom(targ));
        }
    }

    uint8_t l_pcie_clk_en;

    FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_EC_FEATURE_PCIE_CLK_EN,
                           i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>(),
                           l_pcie_clk_en));

    if (l_pcie_clk_en)
    {
        // PCIE non clocking bug fix
        for (auto& targ : i_target.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_PEC))
        {
            // save whats in CPLT_CONF0
            FAPI_TRY(CPLT_CONF0.getScom(targ));
            // set bits 48 - 51
            CPLT_CONF0.insertFromRight<CPLT_CONF0_TOPOLOGY_ID, CPLT_CONF0_TOPOLOGY_ID_LEN>(0b1111);
            FAPI_TRY(CPLT_CONF0.putScom(targ));
        }

    }

    // OPCG_REG1
    OPCG_REG1 = i_params.lbist_opcg1;
    OPCG_REG1.set_DISABLE_ARY_CLK_DURING_FILL(bool(i_params.flags & i_params.bist_flags::LBIST_COMBINED)); // bit53
    FAPI_INF("LBIST: Writing OPCG_REG1 Register ...");
    FAPI_TRY(OPCG_REG1.putScom(i_target));

    // OPCG_REG2
    // TODO: Add OPCG2 logic as a parameter
    FAPI_TRY(OPCG_REG2.getScom(i_target));
    OPCG_REG2.insertFromRight<48, 4>(stumpsWeightSetting.PRIMARY_WEIGHT);
    OPCG_REG2.insertFromRight<52, 4>(stumpsWeightSetting.SECONDARY_WEIGHT);
    FAPI_INF("LBIST: Writing OPCG2 Register with STUMPS Weight ...");
    FAPI_TRY(OPCG_REG2.putScom(i_target));

    //* OPCG_CAPT REGS
    OPCG_CAPT1.insertFromRight<0, 64>(opcgCaptureSetting.OPCG_CAPT1);
    OPCG_CAPT2.insertFromRight<0, 64>(opcgCaptureSetting.OPCG_CAPT2);
    OPCG_CAPT3.insertFromRight<0, 64>(opcgCaptureSetting.OPCG_CAPT3);
    FAPI_INF("LBIST: Writing OPCG CAPTURE Registers with CLK SEQ ...");
    FAPI_TRY(OPCG_CAPT1.putScom(i_target));
    FAPI_TRY(OPCG_CAPT2.putScom(i_target));
    FAPI_TRY(OPCG_CAPT3.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode trigger_start(
    const Target < TARGET_TYPE_PERV >& i_target)
{
    SCAN_REGION_TYPE_t SCAN_REGION_TYPE = 0;        // 0x30005
    CLK_REGION_t CLK_REGION = 0;                    // 0x30006
    OPCG_REG0_t OPCG_REG0;                          // 0x30002
    PCB_OPCG_GO_t PCB_OPCG_GO;                      // 0x30020

    // setup scom registers
    FAPI_TRY(SCAN_REGION_TYPE.putScom(i_target));

    FAPI_TRY(CLK_REGION.putScom(i_target));

    FAPI_TRY(OPCG_REG0.getScom(i_target));
    OPCG_REG0.set_RUNN_MODE(1);
    OPCG_REG0.set_OPCG_GO(0);
    OPCG_REG0.set_OPCG_TRIGGER_RCVR_MODE(0);
    OPCG_REG0.set_OPCG_TRIGGER_CTRL_MODE(1);
    FAPI_TRY(OPCG_REG0.putScom(i_target));

    FAPI_TRY(PCB_OPCG_GO.getScom(i_target));
    PCB_OPCG_GO.set_PCB_OPCGGO(1);
    FAPI_TRY(PCB_OPCG_GO.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
