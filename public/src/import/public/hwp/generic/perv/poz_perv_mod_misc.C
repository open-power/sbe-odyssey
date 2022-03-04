/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_misc.C $ */
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
/// @file  poz_perv_mod_misc.C
///
/// @brief  definitions for modules CBS start, switch pcbmux
///                                 multicast setup & hangpulse setup
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include "poz_perv_mod_misc.H"
#include <poz_perv_utils.H>
#include <p11_scom_perv.H>
#include <p11_scom_pc.H>
#include <target_filters.H>

SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_CS;
SCOMT_PC_USE_TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET;
SCOMT_PC_USE_TP_CFAM_FSI_W_FSI2PIB_STATUS;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SB_MSG;
SCOMT_PERV_USE_HANG_PULSE_0_REG;
//SCOMT_PERV_USE_FSXCOMP_FSXLOG_CBS_ENVSTAT; TODO
SCOMT_PERV_USE_PRE_COUNTER_REG;
SCOMT_PERV_USE_PCBCTL_COMP_INTR_HOST_MASK_REG;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_PERV_CTRL0;
SCOMT_PC_USE_TP_TPCHIP_TPC_CPLT_CTRL0;

using namespace fapi2;
//using namespace fapi2::p11t;
using namespace scomt::perv;
using namespace scomt::pc;

enum POZ_PERV_MOD_MISC_Private_Constants
{
    P11_CFAM_CBS_POLL_COUNT = 200, // Observed Number of times CBS read for CBS_INTERNAL_STATE_VECTOR
    CBS_IDLE_VALUE = 0x002, // Read the value of CBS_CS_INTERNAL_STATE_VECTOR
    P11_CBS_IDLE_HW_NS_DELAY = 640000, // unit is nano seconds [min : 64k x (1/100MHz) = 64k x 10(-8) = 640 us
    //                       max : 64k x (1/50MHz) = 128k x 10(-8) = 1280 us]
    P11_CBS_IDLE_SIM_CYCLE_DELAY = 750000, // unit is sim cycles,to match the poll count change ( 250000 * 30 )
    MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001,
    HOST_MASK_REG_IPOLL_MASK = 0xFC00000000000000,
    PGOOD_REGIONS_STARTBIT = 4,
    PGOOD_REGIONS_LENGTH = 15,
    PGOOD_REGIONS_OFFSET = 12,
};

ReturnCode mod_cbs_start(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    bool start_sbe)
{
    FSXCOMP_FSXLOG_CBS_CS_t CBS_CS;
    TP_CFAM_FSI_W_SBE_FIFO_FSB_DOWNFIFO_RESET_t FSB_DOWNFIFO_RESET;
    TP_CFAM_FSI_W_FSI2PIB_STATUS_t FSI2PIB_STATUS;
    FSXCOMP_FSXLOG_SB_MSG_t SB_MSG;
    int l_timeout = 0;

    FAPI_INF("Entering ...");
    FAPI_INF("Read FSI2PIB_STATUS register and check whether VDN power is on or not(VDD_NEST_OBSERVE).");
    FAPI_TRY(FSI2PIB_STATUS.getCfam(i_target));

    FAPI_ASSERT(FSI2PIB_STATUS.get_VDD_NEST_OBSERVE(),
                fapi2::VDN_POWER_NOT_ON()
                .set_FSI2PIB_STATUS_READ(FSI2PIB_STATUS)
                .set_PROC_TARGET(i_target),
                "ERROR: VDN power is NOT on. i.e. FSI2PIB_STATUS register bit 16 is NOT set.");

    FAPI_INF("Clear Selfboot Message Register, Reset SBE FIFO.");
    SB_MSG = 0;
    FAPI_TRY(SB_MSG.putCfam(i_target));
    FSB_DOWNFIFO_RESET = 0x80000000;
    FAPI_TRY(FSB_DOWNFIFO_RESET.putCfam(i_target));

    FAPI_INF("Prepare for CBS start.");
    FAPI_TRY(CBS_CS.getCfam(i_target));
    CBS_CS.set_START_BOOT_SEQUENCER(0);
    CBS_CS.set_OPTION_SKIP_SCAN0_CLOCKSTART(0);
    CBS_CS.set_OPTION_PREVENT_SBE_START(not start_sbe);
    FAPI_TRY(CBS_CS.putCfam(i_target));

    FAPI_INF("Start CBS.");
    CBS_CS.set_START_BOOT_SEQUENCER(1);
    FAPI_TRY(CBS_CS.putCfam(i_target));
    // Leave START_BOOT_SEQUENCER at 1 to prevent accidental restarts

    FAPI_DBG("Monitor CBS_CS INTERNAL_STATE_VECTOR to know current state of CBS state machine.");
    l_timeout = P11_CFAM_CBS_POLL_COUNT;

    while (l_timeout != 0)
    {
        FAPI_TRY(CBS_CS.getCfam(i_target));

        if (CBS_CS.get_INTERNAL_STATE_VECTOR() == CBS_IDLE_VALUE)
        {
            break;
        }

        FAPI_TRY(fapi2::delay(P11_CBS_IDLE_HW_NS_DELAY, P11_CBS_IDLE_SIM_CYCLE_DELAY));
        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    // Finding the clock used for starting CBS. TODO
    //FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CP_REFCLOCK_SELECT, i_target_chip, l_cp_refclck_select));

    FAPI_ASSERT(l_timeout > 0,
                fapi2::CBS_NOT_IN_IDLE_STATE()
                .set_CBS_CS_READ(CBS_CS)
                .set_CBS_CS_IDLE_VALUE(CBS_IDLE_VALUE)
                .set_LOOP_COUNT(P11_CFAM_CBS_POLL_COUNT)
                .set_HW_DELAY(P11_CBS_IDLE_HW_NS_DELAY)
                .set_PROC_TARGET(i_target),
                //.set_CLOCK_POS(l_callout_clock),
                "ERROR: CBS HAS NOT REACHED IDLE STATE VALUE 0x002 ");

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_switch_pcbmux(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    mux_type i_path)
{
    FSXCOMP_FSXLOG_ROOT_CTRL0_t ROOT_CTRL0;

    FAPI_INF("Entering ...");
    FAPI_DBG("Raise OOB Mux.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_OOB_MUX(1);
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Set PCB_RESET_DC bit in ROOT_CTRL0 register.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET_DC(1);
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Enable the new path first to prevent glitches.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.setBit(i_path);
    FAPI_TRY(ROOT_CTRL0.putScom_SET(i_target));

    FAPI_DBG("Disable the old path.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.setBit<FSXCOMP_FSXLOG_ROOT_CTRL0_FSI2PCB_DC>()
    .setBit<FSXCOMP_FSXLOG_ROOT_CTRL0_PIB2PCB_DC>()
    .setBit<FSXCOMP_FSXLOG_ROOT_CTRL0_PCB2PCB_DC>()
    .clearBit(i_path);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Clear PCB_RESET_DC.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_PCB_RESET_DC(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Drop OOB Mux.");
    ROOT_CTRL0 = 0;
    ROOT_CTRL0.set_OOB_MUX(1);
    FAPI_TRY(ROOT_CTRL0.putScom_CLEAR(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_multicast_setup(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint8_t i_group_id,
    uint64_t i_chiplets,
    TargetState i_pgood_policy)
{
    fapi2::buffer<uint64_t> l_eligible_chiplets = 0;
    fapi2::buffer<uint64_t> l_required_group_members;
    fapi2::buffer<uint64_t> l_current_group_members;
    auto l_func = i_target.getChildren<fapi2::TARGET_TYPE_PERV>( i_pgood_policy);

    FAPI_INF("Entering ...");
    FAPI_ASSERT(!(i_group_id > 6),
                fapi2::INVALID_GROUP_ID()
                .set_GROUP_ID_VALUE(i_group_id)
                .set_PROC_TARGET(i_target),
                "ERROR: INVALID group id passed to module multicast setup.");

    FAPI_INF("Determine required group members.");

    for (auto& targ : l_func)
    {
        l_eligible_chiplets.setBit(targ.getChipletNumber());
    }

    l_required_group_members = l_eligible_chiplets & i_chiplets;
    FAPI_DBG("Required multicast group members : %#018lX", l_required_group_members);

    // MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001
    // This performs a multicast read with the BITX merge operation.
    // It reads a register that has bit 0 tied to 1, so the return value
    // will have a 1 for each chiplet that is a member of the targeted group.
    FAPI_INF("Determine current group members");
    FAPI_TRY(fapi2::getScom(i_target, MC_GROUP_MEMBERSHIP_BITX_READ | ((uint32_t)i_group_id << 24),
                            l_current_group_members));
    FAPI_DBG("Current multicast group members : %#018lX", l_current_group_members);

    FAPI_INF("Update group membership where needed");

    for (int i = 0; i <= 63; i++)
    {
        const bool want = l_required_group_members.getBit(i);
        const bool have = l_current_group_members.getBit(i);

        if (want == have)
        {
            continue;
        }

        const uint64_t prev_group = have ? i_group_id : 7;
        const uint64_t new_group  = want ? i_group_id : 7;
        FAPI_TRY(fapi2::putScom(i_target, (PCB_RESPONDER_MULTICAST_GROUP_1 + i_group_id) | (i << 24),
                                (new_group << 58) | (prev_group << 42)));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_hangpulse_setup(const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target,
                               uint8_t i_pre_divider, const hang_pulse_t* i_hangpulse_table)
{
    HANG_PULSE_0_REG_t HANG_PULSE_0_REG;
    PRE_COUNTER_REG_t PRE_COUNTER_REG;

    FAPI_INF("Entering ...");
    FAPI_DBG("Set pre_divider value in pre_counter register.");
    PRE_COUNTER_REG = 0;
    PRE_COUNTER_REG.set_PRE_COUNTER(i_pre_divider);
    FAPI_TRY(PRE_COUNTER_REG.putScom(i_target));

    while(1)
    {
        HANG_PULSE_0_REG = 0;
        HANG_PULSE_0_REG.set_HANG_PULSE_REG_0(i_hangpulse_table->value);
        HANG_PULSE_0_REG.set_SUPPRESS_HANG_0(i_hangpulse_table->stop_on_xstop);
        FAPI_TRY(putScom(i_target, scomt::perv::HANG_PULSE_0_REG + i_hangpulse_table->id, HANG_PULSE_0_REG));

        if (i_hangpulse_table->last)
        {
            break;
        }

        i_hangpulse_table++;
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_constant_hangpulse_setup(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, uint32_t i_base_address,
                                        const constant_hang_pulse_t i_hangpulses[4])
{
    PRE_COUNTER_REG_t PRE_COUNTER_REG;
    HANG_PULSE_0_REG_t HANG_PULSE_0_REG;

    FAPI_INF("Entering ...");

    for (int i = 0; i <= 3; i++)
    {
        PRE_COUNTER_REG = 0;
        PRE_COUNTER_REG.set_PRE_COUNTER(i_hangpulses[i].pre_divider);
        FAPI_TRY(putScom(i_target, i_base_address + i * 2 + 2, PRE_COUNTER_REG));

        HANG_PULSE_0_REG = 0;
        HANG_PULSE_0_REG.set_HANG_PULSE_REG_0(i_hangpulses[i].value);
        HANG_PULSE_0_REG.set_SUPPRESS_HANG_0(i_hangpulses[i].stop_on_xstop);
        FAPI_TRY(putScom(i_target, i_base_address + i * 2 + 1, HANG_PULSE_0_REG));
    }

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}

ReturnCode mod_poz_tp_init_common(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    PCBCTL_COMP_INTR_HOST_MASK_REG_t HOST_MASK_REG;
    FSXCOMP_FSXLOG_PERV_CTRL0_t PERV_CTRL0;
    TP_TPCHIP_TPC_CPLT_CTRL0_t CPLT_CTRL0;
    fapi2::buffer<uint32_t> l_attr_pg;
    fapi2::buffer<uint64_t> l_data64;

    FAPI_INF("Entering ...");
    FAPI_DBG("Set up IPOLL mask");
    HOST_MASK_REG = HOST_MASK_REG_IPOLL_MASK ;
    FAPI_TRY(HOST_MASK_REG.putScom(i_target));

    FAPI_DBG("Transfer PERV partial good attribute into region good register (cplt_ctrl2 reg)");
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PG, get_tp_chiplet_target(i_target), l_attr_pg));
    l_attr_pg.invert();
    l_data64.flush<0>();
    l_data64.insert< PGOOD_REGIONS_STARTBIT, PGOOD_REGIONS_LENGTH, PGOOD_REGIONS_OFFSET >(l_attr_pg);
    FAPI_TRY(putScom(get_tp_chiplet_target(i_target), scomt::perv::CPLT_CTRL2_RW, l_data64));

    FAPI_DBG("Enabe PERV vital clock gating");
    PERV_CTRL0 = 0;
    PERV_CTRL0.set_TP_TCPERV_VITL_CG_DIS(1);
    FAPI_TRY(PERV_CTRL0.putScom_CLEAR(i_target));

    FAPI_DBG("Allow chiplet PLATs to enter flush");
    CPLT_CTRL0.flush<0>();
    CPLT_CTRL0.set_CTRL_CC_FLUSHMODE_INH(1);
    FAPI_TRY(CPLT_CTRL0.putScom_CLEAR(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
