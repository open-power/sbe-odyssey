/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_misc.C $ */
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
/// @file  poz_perv_mod_misc.C
/// @brief Miscellaneous module definitions
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_mod_misc.H>
#include <poz_perv_utils.H>
#include <poz_perv_mod_misc_regs.H>
#include <target_filters.H>

using namespace fapi2;

enum POZ_PERV_MOD_MISC_Private_Constants
{
    DELAY_10us = 10000,      // unit in nano seconds
    SIM_CYCLE_DELAY = 1000, // unit in cycles
    SEMAPHORE_POLL_COUNT = 30000,
    MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001,
    PCB_RESPONDER_MCAST_GROUP_1 = 0xF0001,
    HOST_MASK_REG_IPOLL_MASK = 0xF800000000000000,
    PGOOD_REGIONS_STARTBIT = 4,
    PGOOD_REGIONS_LENGTH = 15,
    PGOOD_REGIONS_OFFSET = 12,

    LFIR_MASK_DEFAULT = 0x80dfffffffffffff,
    TP_LFIR_SPPE_HALTED_BIT = 30,
    TP_LFIR_MASK_DEFAULT = 0x80c1c7fcf3fbffff,
    TP_LFIR_MASK_SPPE_HALTED = 0x80c1c7fef3fbffff,
    XSTOP_MASK_ANY_ATTN_AND_DBG = 0x3000000000000000,
    RECOV_MASK_LOCAL_XSTOP = 0x2000000000000000,
    CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF,
};

ReturnCode mod_switch_pcbmux(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const mux_type i_path)
{
    ROOT_CTRL0_t ROOT_CTRL0;
    uint8_t l_oob_mux_save = 0;

    FAPI_DBG("Entering ...");
    FAPI_DBG("Save OOB Mux setting.");
    FAPI_TRY(ROOT_CTRL0.getScom(i_target));
    l_oob_mux_save = ROOT_CTRL0.get_OOB_MUX();

    FAPI_DBG("Raise OOB Mux.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_OOB_MUX(1);
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Set PCB_RESET bit in ROOT_CTRL0 register.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET(1);
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Enable the new path first to prevent glitches.");
    ROOT_CTRL0 = 0;
    FAPI_TRY(ROOT_CTRL0.setBit(i_path));
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Disable the old path.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_FSI2PCB(1);
    ROOT_CTRL0.set_PIB2PCB(1);
    ROOT_CTRL0.set_PCB2PCB(1);
    FAPI_TRY(ROOT_CTRL0.clearBit(i_path));
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Clear PCB_RESET.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    if (l_oob_mux_save == 0)
    {
        FAPI_DBG("Restore OOB Mux setting.");
        ROOT_CTRL0 = 0;
        ROOT_CTRL0.set_OOB_MUX(1);
        FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_switch_pcbmux_cfam(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const mux_type i_path)
{
    ROOT_CTRL0_t ROOT_CTRL0;
    uint8_t l_oob_mux_save = 0;

    FAPI_DBG("Entering ...");
    FAPI_DBG("Save OOB Mux setting.");
    FAPI_TRY(ROOT_CTRL0.getCfam(i_target));
    l_oob_mux_save = ROOT_CTRL0.get_OOB_MUX();

    FAPI_DBG("Raise OOB Mux.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_OOB_MUX(1);
    FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));

    FAPI_DBG("Set PCB_RESET bit in ROOT_CTRL0 register.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET(1);
    FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));

    FAPI_DBG("Enable the new path first to prevent glitches.");
    ROOT_CTRL0 = 0;
    FAPI_TRY(ROOT_CTRL0.setBit(i_path));
    FAPI_TRY(ROOT_CTRL0.putCfam_SET(i_target));

    FAPI_DBG("Disable the old path.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_FSI2PCB(1);
    ROOT_CTRL0.set_PIB2PCB(1);
    ROOT_CTRL0.set_PCB2PCB(1);
    FAPI_TRY(ROOT_CTRL0.clearBit(i_path));
    FAPI_TRY(ROOT_CTRL0.putCfam_CLEAR(i_target));

    FAPI_DBG("Clear PCB_RESET.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET(1);
    FAPI_TRY(ROOT_CTRL0.putCfam_CLEAR(i_target));

    FAPI_DBG("Restore OOB Mux setting.");

    if (l_oob_mux_save == 0)
    {
        ROOT_CTRL0 = 0;
        ROOT_CTRL0.set_OOB_MUX(1);
        FAPI_TRY(ROOT_CTRL0.putCfam_CLEAR(i_target));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_multicast_setup(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint8_t i_group_id,
    const uint64_t i_chiplets,
    const TargetState i_pgood_policy)
{
    uint8_t l_group_id = i_group_id;
    fapi2::buffer<uint64_t> l_eligible_chiplets = 0;
    fapi2::buffer<uint64_t> l_required_group_members;
    fapi2::buffer<uint64_t> l_current_group_members;
    fapi2::buffer<uint64_t> l_attr_sim_chiplet_mask;

    FAPI_DBG("Entering ...");
    FAPI_ASSERT(!(i_group_id > 6),
                fapi2::POZ_INVALID_GROUP_ID()
                .set_GROUP_ID_VALUE(i_group_id)
                .set_PROC_TARGET(i_target),
                "ERROR: INVALID group id passed to module multicast setup.");

    FAPI_TRY(mod_multicast_setup_plat_remap(i_group_id, l_group_id));

    for (const auto& targ : i_target.getChildren<fapi2::TARGET_TYPE_PERV>(i_pgood_policy))
    {
        l_eligible_chiplets.setBit(targ.getChipletNumber());
    }

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SIM_CHIPLET_MASK, i_target, l_attr_sim_chiplet_mask));

    l_required_group_members = l_eligible_chiplets & i_chiplets & l_attr_sim_chiplet_mask;

    // MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001
    // This performs a multicast read with the BITX merge operation.
    // It reads a register that has bit 0 tied to 1, so the return value
    // will have a 1 for each chiplet that is a member of the targeted group.
    FAPI_TRY(fapi2::getScom(i_target, MC_GROUP_MEMBERSHIP_BITX_READ | (static_cast<uint32_t>(l_group_id << 24)),
                            l_current_group_members));

    FAPI_INF("MCG members: Current=0x%08X%08X, Required=0x%08X%08X",
             l_current_group_members >> 32,
             l_current_group_members & 0xFFFFFFFF,
             l_required_group_members >> 32,
             l_required_group_members & 0xFFFFFFFF);

    for (int i = 0; i <= 63; i++)
    {
        const bool want = l_required_group_members.getBit(i);
        const bool have = l_current_group_members.getBit(i);

        if (want == have)
        {
            continue;
        }

        const uint64_t prev_group = have ? l_group_id : 7;
        const uint64_t new_group  = want ? l_group_id : 7;
        FAPI_TRY(fapi2::putScom(i_target, (PCB_RESPONDER_MCAST_GROUP_1 + l_group_id) | (i << 24),
                                (new_group << 58) | (prev_group << 42)));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_get_chiplet_by_number(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_ANY_POZ_CHIP > & i_target,
    const uint8_t i_chiplet_number,
    Target < TARGET_TYPE_PERV >& o_target)
{
    FAPI_DBG("Entering ...");

    for (const auto& chiplet : i_target.getChildren<fapi2::TARGET_TYPE_PERV>())
    {
        if (chiplet.getChipletNumber() == i_chiplet_number)
        {
            o_target = chiplet;
            goto fapi_try_exit;
        }
    }

    FAPI_ASSERT(false,
                fapi2::POZ_CHIPLET_NOT_FOUND()
                .set_CHIPLET_NUMBER(i_chiplet_number)
                .set_PROC_TARGET(i_target),
                "ERROR: Provided chiplet number does not match anything in provided target.");

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_hangpulse_setup(const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
                               const uint8_t i_pre_divider, const hang_pulse_t* i_hangpulse_table)
{
    HANG_PULSE_0_t HANG_PULSE_0;
    PRE_COUNTER_t PRE_COUNTER;

    FAPI_DBG("Entering ...");
    FAPI_DBG("Set pre_divider value in pre_counter register.");
    PRE_COUNTER = 0;
    PRE_COUNTER.set_PRE_COUNTER(i_pre_divider);
    FAPI_TRY(PRE_COUNTER.putScom(i_target));

    while(1)
    {
        FAPI_DBG("Set frequency value for the hang pulse");
        HANG_PULSE_0 = 0;
        HANG_PULSE_0.set_HANG_PULSE_REG_0(i_hangpulse_table->value);
        HANG_PULSE_0.set_SUPPRESS_HANG_0(i_hangpulse_table->stop_on_xstop);
        FAPI_TRY(putScom(i_target, HANG_PULSE_0.addr + i_hangpulse_table->id, HANG_PULSE_0));

        if (i_hangpulse_table->last)
        {
            break;
        }

        i_hangpulse_table++;
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_constant_hangpulse_setup(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, const uint32_t i_base_address,
                                        const constant_hang_pulse_t i_hangpulses[4])
{
    PRE_COUNTER_t PRE_COUNTER;
    HANG_PULSE_0_t HANG_PULSE_0;

    FAPI_DBG("Entering ...");

    for (int i = 0; i <= 3; i++)
    {
        PRE_COUNTER = 0;
        PRE_COUNTER.set_PRE_COUNTER(i_hangpulses[i].pre_divider);
        FAPI_TRY(putScom(i_target, i_base_address + i * 2 + 2, PRE_COUNTER));

        HANG_PULSE_0 = 0;
        HANG_PULSE_0.set_HANG_PULSE_REG_0(i_hangpulses[i].value);
        HANG_PULSE_0.set_SUPPRESS_HANG_0(i_hangpulses[i].stop_on_xstop);
        FAPI_TRY(putScom(i_target, i_base_address + i * 2 + 1, HANG_PULSE_0));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_poz_tp_init_common(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    INTR_HOST_MASK_t HOST_MASK;
    ROOT_CTRL0_t ROOT_CTRL0;
    PERV_CTRL0_t PERV_CTRL0;
    CPLT_CTRL0_t CPLT_CTRL0;
    CPLT_CTRL2_t CPLT_CTRL2;
    SB_CS_t SB_CS;

    fapi2::buffer<uint32_t> l_attr_pg;
    fapi2::buffer<uint64_t> l_data64;

    FAPI_DBG("Entering ...");
    fapi2::Target<fapi2::TARGET_TYPE_PERV> l_tpchiplet = get_tp_chiplet_target(i_target);

    FAPI_DBG("Clear SBE start bits to be tidy");
    FAPI_TRY(SB_CS.getScom(i_target));
    SB_CS.set_START_RESTART_VECTOR0(0);
    SB_CS.set_START_RESTART_VECTOR1(0);
    FAPI_TRY(SB_CS.putScom(i_target));

    FAPI_DBG("Clear CBS command to enable clock gating inside clock controller");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_FSI_CC_CBS_CMD(-1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Set up IPOLL mask");
    HOST_MASK = HOST_MASK_REG_IPOLL_MASK;
    FAPI_TRY(HOST_MASK.putScom(i_target));

    FAPI_DBG("Transfer PERV partial good attribute into region good register (cplt_ctrl2 reg)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PG, l_tpchiplet, l_attr_pg));
    l_attr_pg.invert();
    l_data64.flush<0>();
    l_data64.insert< PGOOD_REGIONS_STARTBIT, PGOOD_REGIONS_LENGTH, PGOOD_REGIONS_OFFSET >(l_attr_pg);
    CPLT_CTRL2 = l_data64();
    FAPI_TRY(CPLT_CTRL2.putScom(l_tpchiplet));

    FAPI_DBG("Enable PERV vital clock gating");
    PERV_CTRL0 = 0;
    PERV_CTRL0.set_VITL_CG_DIS(1);
    FAPI_TRY(PERV_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Disable alignment pulse");
    CPLT_CTRL0.flush<0>();
    CPLT_CTRL0.set_FORCE_ALIGN(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(l_tpchiplet));

    FAPI_DBG("Vital align out disable");
    PERV_CTRL0 = 0;
    PERV_CTRL0.setBit<12>(); // VITL_AL_OUT_DIS
    FAPI_TRY(PERV_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Unmask pervasive FIRs");
    // If SPPE is currently halted, don't unmask the corresponding LFIR bit
    FAPI_TRY(getScom(l_tpchiplet, LFIR_RW_WCLEAR, l_data64));
    {
        const uint64_t l_mask = l_data64.getBit<TP_LFIR_SPPE_HALTED_BIT>() ?
                                TP_LFIR_MASK_SPPE_HALTED : TP_LFIR_MASK_DEFAULT;
        FAPI_TRY(putScom(l_tpchiplet, EPS_MASK_RW_WCLEAR, ~l_mask));
    }
    FAPI_TRY(putScom(l_tpchiplet, XSTOP_MASK_RW, XSTOP_MASK_ANY_ATTN_AND_DBG));
    FAPI_TRY(putScom(l_tpchiplet, RECOV_MASK_RW, RECOV_MASK_LOCAL_XSTOP));
    FAPI_TRY(putScom(l_tpchiplet, ATTN_MASK_RW, 0));
    FAPI_TRY(putScom(l_tpchiplet, LOCAL_XSTOP_MASK_RW, 0));

    FAPI_DBG("Drop TP chiplet fence");
    PERV_CTRL0 = 0;
    PERV_CTRL0.setBit<17>(); // bit 17: PERV_CHIPLET_FENCE required for Odyssey only
    FAPI_TRY(PERV_CTRL0.putScom_CLEAR(i_target));

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_unmask_firs(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    FAPI_DBG("Entering ...");

    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc));

    FAPI_DBG("Unmask chiplet FIRs");
    FAPI_TRY(putScom(l_chiplets_mc, EPS_MASK_RW_WCLEAR, ~LFIR_MASK_DEFAULT));
    FAPI_TRY(putScom(l_chiplets_mc, XSTOP_MASK_RW, XSTOP_MASK_ANY_ATTN_AND_DBG));
    FAPI_TRY(putScom(l_chiplets_mc, RECOV_MASK_RW, RECOV_MASK_LOCAL_XSTOP));
    FAPI_TRY(putScom(l_chiplets_mc, ATTN_MASK_RW, 0));
    FAPI_TRY(putScom(l_chiplets_mc, LOCAL_XSTOP_MASK_RW, 0));

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_setup_clockstop_on_xstop(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint16_t i_regions_to_stop,
    const uint8_t i_chiplet_delays[64])
{
    XSTOP1_t XSTOP1;
    CLKSTOP_ON_XSTOP_MASK1_t EPS_CLKSTOP_ON_XSTOP_MASK1;
    XSTOP_INTERRUPT_REG_t XSTOP_INTERRUPT_REG;

    fapi2::buffer<uint8_t>  l_clkstop_on_xstop;

    FAPI_INF("Entering mod_setup_clockstop_on_xstop...");

    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_BITX > l_chiplets_bitx;

    std::vector<Target<TARGET_TYPE_PERV>> l_chiplets_uc;
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, &l_chiplets_uc));
    l_chiplets_bitx = l_chiplets_mc;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CLOCKSTOP_ON_XSTOP, i_target, l_clkstop_on_xstop));

    if (l_clkstop_on_xstop)
    {
        XSTOP1.flush<0>();
        XSTOP1.set_ENABLE(1);
        XSTOP1.set_WAIT_ALWAYS(1);
        XSTOP1.insertFromRight<4, 15>(i_regions_to_stop);

        EPS_CLKSTOP_ON_XSTOP_MASK1.flush<1>();
        EPS_CLKSTOP_ON_XSTOP_MASK1.insert<0, 8>(l_clkstop_on_xstop);

        FAPI_DBG("Enable clockstop on checkstop");
        FAPI_TRY(EPS_CLKSTOP_ON_XSTOP_MASK1.putScom(l_chiplets_mc));

        if (EPS_CLKSTOP_ON_XSTOP_MASK1.get_SYS_XSTOP_STAGED_ERR())
        {
            FAPI_DBG("Staged xstop is masked, leave all delays at 0 for fast stopping.");
            FAPI_TRY(XSTOP1.putScom(l_chiplets_mc));
        }
        else
        {
            FAPI_DBG("Staged xstop is unmasked, set up per-chiplet delays");

            for (auto& l_chiplet : l_chiplets_uc)
            {
                XSTOP1.set_WAIT_CYCLES(4 * (4 - i_chiplet_delays[l_chiplet.getChipletNumber()]));
                FAPI_TRY(XSTOP1.putScom(l_chiplet));
            }
        }

        FAPI_TRY(XSTOP_INTERRUPT_REG.getScom(l_chiplets_bitx));
        FAPI_ASSERT(!XSTOP_INTERRUPT_REG, fapi2::POZ_IMMEDIATE_CLOCKSTOP_ON_XSTOP(),
                    "Clockstop on xstop immediately after configuring it in chiplets %08X%08X", (uint64_t(XSTOP_INTERRUPT_REG) >> 32),
                    static_cast<uint32_t>(uint64_t(XSTOP_INTERRUPT_REG) & 0xFFFFFFFF));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_setup_tracestop_on_xstop_chiplet(
    const Target<TARGET_TYPE_PERV>& i_chiplet,
    const uint32_t i_dbg_scom_base)
{
    fapi2::buffer<uint64_t> DBG_TRACE_MODE_REG_2 = 0;
    fapi2::buffer<uint64_t> DBG_MODE_REG = 0;

    FAPI_DBG("Set up trace stop on xstop for chiplet %d",
             i_chiplet.getChipletNumber());

    FAPI_TRY(getScom(i_chiplet, i_dbg_scom_base + 0, DBG_MODE_REG));
    DBG_MODE_REG.setBit<EPS_DBG_MODE_REG_STOP_ON_XSTOP_SELECTION>();
    FAPI_TRY(putScom(i_chiplet, i_dbg_scom_base + 0, DBG_MODE_REG));

    FAPI_TRY(getScom(i_chiplet, i_dbg_scom_base + 0xF, DBG_TRACE_MODE_REG_2));
    DBG_TRACE_MODE_REG_2.setBit<EPS_DBG_TRACE_MODE_REG_2_STOP_ON_ERR>();
    FAPI_TRY(putScom(i_chiplet, i_dbg_scom_base + 0xF, DBG_TRACE_MODE_REG_2));

fapi_try_exit:
    return current_err;
}

ReturnCode mod_setup_tracestop_on_xstop(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint32_t i_dbg_scom_base)
{
    FAPI_DBG("Entering mod_setup_tracestop_on_xstop...");

    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > l_chiplets_mc;
    std::vector<Target<TARGET_TYPE_PERV>> l_chiplets_uc;
    FAPI_TRY(get_hotplug_targets(i_target, l_chiplets_mc, &l_chiplets_uc));

    for (auto& l_chiplet : l_chiplets_uc)
    {
        FAPI_TRY(mod_setup_tracestop_on_xstop_chiplet(l_chiplet, i_dbg_scom_base));
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_semaphore_reserve(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint32_t i_semaphore,
    const SemaphoreMode i_silent,
    const SemaphoreSide i_side)
{
    FAPI_DBG("Entering mod_semaphore_reserve...");
    fapi2::buffer<uint64_t> l_data = 0, l_status = 0;

    static const uint32_t FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER = 0x50191ull;

    l_data.setBit((2 * i_semaphore) + (uint32_t)i_side);

    for (uint32_t i = 0; i < SEMAPHORE_POLL_COUNT; i++)
    {
        FAPI_TRY(putScom(i_target, FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER, l_data));
        FAPI_TRY(getScom(i_target, FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER, l_status));

        if (l_status.getBit((2 * i_semaphore) + (uint32_t)i_side))
        {
            FAPI_DBG("Exiting after successfully reserving semaphore 0x%X side %x...", i_semaphore, (uint32_t)i_side);
            return FAPI2_RC_SUCCESS;
        }

        FAPI_TRY(fapi2::delay(DELAY_10us, SIM_CYCLE_DELAY));
    }

    if (i_silent == SemaphoreMode::silent)
    {
        FAPI_ERR("Semaphore 0x%X side %x reservation timeout, force release semaphore with 0b11", i_semaphore,
                 (uint32_t)i_side);
        l_data.setBit((2 * i_semaphore) + (uint32_t)SemaphoreSide::CFAM);
        l_data.setBit((2 * i_semaphore) + (uint32_t)SemaphoreSide::SBE);
        FAPI_TRY(putScom(i_target, FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER, l_data));
        return FAPI2_RC_SUCCESS;
    }
    else
    {
        FAPI_ASSERT(false, fapi2::POZ_ARBSEMA_RESERVE_TIMEOUT().set_SEMAPHORE(i_semaphore).set_SIDE(i_side),
                    "Semaphore 0x%X side %x reservation timeout", i_semaphore, (uint32_t)i_side);
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}

ReturnCode mod_semaphore_release(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint32_t i_semaphore,
    const SemaphoreMode i_silent,
    const SemaphoreSide i_side)
{
    FAPI_DBG("Entering mod_semaphore_release...");

    fapi2::buffer<uint64_t> l_data = 0, l_status = 0;

    static const uint32_t FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER = 0x50191ull;

    FAPI_TRY(getScom(i_target, FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER, l_status));

    if (l_status.getBit((2 * i_semaphore) + (uint32_t)i_side))
    {
        l_data.setBit((2 * i_semaphore) + (uint32_t)SemaphoreSide::CFAM);
        l_data.setBit((2 * i_semaphore) + (uint32_t)SemaphoreSide::SBE);
        FAPI_TRY(putScom(i_target, FSXCOMP_FSXLOG_I2CARBSEMA_REGISTER, l_data));
    }
    else
    {
        if (i_silent == SemaphoreMode::silent)
        {
            FAPI_ERR("Trying to free unreserved semaphore 0x%X side %x...", i_semaphore, (uint32_t)i_side);
            return FAPI2_RC_SUCCESS;
        }
        else
        {
            FAPI_ASSERT(false, fapi2::POZ_ARBSEMA_RELEASE_ERROR().set_SEMAPHORE(i_semaphore).set_SIDE(i_side),
                        "Trying to free unreserved semaphore 0x%X side %x...", i_semaphore, (uint32_t)i_side);
        }
    }

fapi_try_exit:
    FAPI_DBG("Exiting ...");
    return current_err;
}
