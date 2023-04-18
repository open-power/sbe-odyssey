# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/poz_ipl/perv_modules.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG

ISTEP(99, 99, "poz_perv_mod_chip_clocking", "")

def mod_clock_test(target<PROC_CHIP | HUB_CHIP>, bool i_use_cfam_path):
    # Copy the main loop of p10_clock_test here ("for(int i = 0; i < POLL_COUNT; i++)")
    # and adapt it so that it will do either CFAM or SCOM accesses based on the value of the i_use_cfam_path parameter
    for loop in 1..10:
        for test_value in (0, 1):
            ROOT_CTRL5.TPFSI_RCS_CLK_TEST_IN_DC = test_value
            delay(20ns, 1kcyc)

            const bool clockA_functional = SNS1LTH.REFCLK_DATA_OUT_A == test_value;
            const bool clockB_functional = SNS1LTH.REFCLK_DATA_OUT_B == test_value;

            /* Find out for each clock whether it is required or merely optional
             * according to the refclock_select value.
             */
            i_cp_refclock_select = ATTR_CP_REFCLOCK_SELECT;
            const bool clockA_required = i_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1 &&
                                         i_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC1_NORED;
            const bool clockB_required = i_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0 &&
                                         i_cp_refclock_select != fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_BOTH_OSC0_NORED;

            const bool clockA_ok = clockA_functional || !clockA_required;
            const bool clockB_ok = clockB_functional || !clockB_required;

            const uint8_t callout_clock = clockA_ok ? fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC1 :
                                          fapi2::ENUM_ATTR_CP_REFCLOCK_SELECT_OSC0;

            ASSERT(clockA_ok && clockB_ok, RCS_CLOCK_TEST_OUT_ERROR)

def _mod_poll_pll_lock(target<ANY_POZ_CHIP|PERV|MC, MCAST_AND>, uint32_t i_reg_addr, uint64_t i_check_value):
    # basically this is p10_perv_sbe_cmn_poll_pll_lock, only with a shifted parameter
    for loop in 1..50:
        if (getScom(i_target, i_reg_addr) & i_check_value) == i_check_value:
            return SUCCESS
        delay(100us, 100kcyc)

    return TIMEOUT

# Standard poll for PLL lock using the PCB Responder
def mod_poll_pll_lock(target<PERV|MC, MCAST_AND>, pll_lock_bits i_pll_mask):
    return _mod_poll_pll_lock(i_target, PLL_LOCK_REG, i_pll_mask << 56)

# Poll for PLL lock using the FSI2PIB status register, for cases where the PCB is not up yet
def mod_poll_pll_lock_fsi2pib(target<ANY_POZ_CHIP>, pll_lock_bits i_pll_mask):
    # FSI2PIB_STATUS_MIRROR = 0x50000
    return _mod_poll_pll_lock(i_target, FSI2PIB_STATUS_MIRROR, i_pll_mask << 44)

ISTEP(99, 99, "poz_perv_mod_rcs", "")

def mod_rcs_setup(target<HUB_CHIP|PROC_CHIP>):
    pass

ISTEP(99, 99, "poz_perv_mod_chiplet_clocking", "")

def mod_abist_setup(target<PERV|MC>, uint16_t i_clock_regions, uint32_t i_runn_cycles=0x42FFF, uint32_t i_abist_start_at=0xF0, uint32_t i_abist_start_stagger=0):
    # Switch dual-clocked arrays to ABIST clock domain
    CPLT_CTRL0.ABSTCLK_MUXSEL = 1

    # Set up BISTed regions
    BIST = 0
    BIST.TC_SRAM_ABIST_MODE_DC = 1
    BIST[16 bits starting at BIST_REGION_PERV] = i_clock_regions

    # Set up clocking
    CLK_REGION = 0
    CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions
    CLK_REGION.SEL_THOLD_SL = 1
    CLK_REGION.SEL_THOLD_NSL = 1
    CLK_REGION.SEL_THOLD_ARY = 1

    # Configure idle count
    uint32_t idle_count = i_abist_start_at;
    for chiplet in unicast children of i_target:
        OPCG_REG1[bits 0:35] = idle_count
        idle_count += i_abist_start_stagger

    # Configure loop count and start OPCG
    OPCG_REG0 = 0
    OPCG_REG0.RUNN_MODE = 1
    OPCG_REG0.OPCG_STARTS_BIST = 1
    OPCG_REG0.LOOP_COUNT = i_runn_cycles

def mod_opcg_go(target<PERV|MC>):
    OPCG_REG0.OPCG_GO = 1

def mod_abist_start(target<PERV|MC>, uint16_t i_clock_regions, uint32_t i_runn_cycles=0x42FFF, uint32_t i_abist_start_at=0xF0, uint32_t i_abist_start_stagger=0):
    mod_abist_setup(i_target,
                    i_clock_regions,
                    i_runn_cycles,
                    i_abist_start_at,
                    i_abist_start_stagger)
    mod_opcg_go(i_target)

def mod_abist_poll(target<PERV|MC, MCAST_AND>, bool i_poll_abist_done=False):
    poll_opcg_done(i_target, 200us, 1120kcyc, 400, i_poll_abist_done)

    # Check that ABIST_DONE is set
    if not CPLT_STAT0.ABIST_DONE:
        ASSERT(SRAM_ABIST_DONE_BIT_ERR)

def mod_abist_cleanup(target<PERV|MC>, bool i_clear_sram_abist_mode=true):
    OPCG_REG0 = 0
    CLK_REGION = 0
    CPLT_CTRL0.ABSTCLK_MUXSEL = 0
    BIST = 0
    BIST.TC_SRAM_ABIST_MODE_DC = !i_clear_sram_abist_mode

def poll_opcg_done(target<PERV|MC, MCAST_AND>, uint32_t i_hw_delay, uint32_t i_sim_delay, uint32_t i_poll_count, bool i_poll_abist_done=False):
    for i in 1..i_poll_count:
        if CPLT_STAT.OPCG_DONE or (i_poll_abist_done and CPLT_STAT.ABIST_DONE):
            return SUCCESS

        delay(i_hw_delay, i_sim_delay)

    ASSERT(CPLT_OPCG_DONE_NOT_SET_ERR)

def check_clock_status(target<PERV|MC>, uint16_t i_clock_regions, uint16_t i_clock_type, bool i_start_stop):
    if i_clock_type == CLOCK_TYPE_SL:
        addr = CLOCK_STAT_SL
    elif i_clock_type == CLOCK_TYPE_NSL:
        addr = CLOCK_STAT_NSL
    elif i_clock_type == CLOCK_TYPE_ARY:
        addr = CLOCK_STAT_ARY
    else:
        return SUCCESS   # nothing to do

    if i_start_stop:
        value = getScom(i_target, addr)
        expect = 0
    else:
        value = getScom(Target<MCAST_AND>(i_target), addr)
        expect = i_clock_regions

    if value & i_clock_regions != expect:
        ASSERT(THOLD_ERR)

def mod_align_regions(target<PERV|MC>, uint16_t i_clock_regions):
    ## Enable alignment
    CPLT_CTRL4[4:19] = i_clock_regions
    CPLT_CTRL0.CTRL_CC_FORCE_ALIGN_DC = 1

    ## Clear "chiplet is aligned" indication
    for chiplet in i_target.getChildren<PERV>:
        SYNC_CONFIG.CLEAR_CHIPLET_IS_ALIGNED = 1
        # write SYNC_CONFIG
        SYNC_CONFIG.CLEAR_CHIPLET_IS_ALIGNED = 0

    ## Wait for chiplets to be aligned
    try 10 times:
        with i_target using MCAST_AND:
            if CPLT_STAT0.CHIPLET_IS_ALIGNED:
                break
        delay(100us, 1000cyc)
    else:
        ASSERT(CPLT_NOT_ALIGNED_ERR)

    ## Disable alignment
    CPLT_CTRL0.CTRL_CC_FORCE_ALIGN_DC = 0
    delay(10us, 1000cyc)
    CPLT_CTRL4 = 0

def mod_scan0(target<PERV|MC>, uint16_t i_clock_regions, uint16_t i_scan_types=SCAN_TYPE_NOT_GTR):
    OPCG_ALIGN_t opcg_align_save[64];

    if ATTR_SCAN0_SCAN_RATIO != 0:    # uint8_t, targets all chips, default 0, provided by MRW
        for cplt in unicast children of i_target:
            opcg_align_save[cplt.getChipletNumber()] = OPCG_ALIGN
            OPCG_ALIGN.SCAN_RATIO = ATTR_SCAN0_SCAN_RATIO

    # Set up clock regions for NSL fill
    CLK_REGION = 0
    CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions
    CLK_REGION.SEL_THOLD_NSL = 1
    CLK_REGION.SEL_THOLD_ARY = 1

    # Set up scan regions for scan0
    SCAN_REGION_TYPE = 0
    SCAN_REGION_TYPE[16 bits starting at SCAN_REGION_PERV] = i_clock_regions
    SCAN_REGION_TYPE[12 bits starting at SCAN_TYPE_FUNC] = i_scan_types

    # Trigger scan0
    OPCG_REG0 = 0
    OPCG_REG0.RUN_SCAN0 = 1

    # Wait for scan0 to complete
    poll_opcg_done(i_target, 16us, 800kcyc, 200)

    # Clean up
    CLK_REGION = 0
    SCAN_REGION_TYPE = 0

    # Restore scan ratios
    if ATTR_SCAN0_SCAN_RATIO != 0:
        for cplt in unicast children of i_target:
            OPCG_ALIGN = opcg_align_save[cplt.getChipletNumber()]

def mod_start_stop_clocks(target<PERV|MC>, uint16_t i_clock_regions, uint16_t i_clock_types=CLOCK_TYPE_ALL, bool i_start_not_stop=true, bool i_manage_fences=true):
    SCAN_REGION_TYPE = 0

    # Drop fences before starting clocks
    if i_start_not_stop and i_manage_fences:
        CPLT_CTRL1_WO_CLEAR = i_clock_regions << 44

    # Issue clock start/stop command
    CLK_REGION = 0
    CLK_REGION.CLOCK_CMD = i_start_not_stop ? 1 : 2
    CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions
    CLK_REGION[4 bits starting at SEL_THOLD_SL] = i_clock_types

    # Wait for command to be done
    poll_opcg_done(i_target, 10us, 1kcyc, 10)

    # Check that the clock status is as expected
    for check_type in (CLOCK_TYPE_SL, CLOCK_TYPE_NSL, CLOCK_TYPE_ARY):
        check_clock_status(i_target, i_clock_regions, i_clock_types & check_type, i_start_not_stop)

    # Raise fences after clocks are stopped
    if not i_start_not_stop and i_manage_fences:
        CPLT_CTRL1_WO_OR = i_clock_regions << 44

ISTEP(99, 99, "poz_perv_mod_misc", "")

def mod_cbs_start_prep(target<ANY_POZ_CHIP>, bool i_start_sbe=true, bool i_scan0_clockstart=true):
    # This module uses CFAM accesses for everything

    # Drop CFAM protection 0 to ungate VDN_PRESENT
    ROOT_CTRL0.CFAM_PROTECTION_0 = 0
    ROOT_CTRL0_COPY.CFAM_PROTECTION_0 = 0

    # check for PGOOD
    if not FSI2PIB_STATUS.VDN_PRESENT:
        ASSERT(VDN_PGOOD_NOT_SET)

    SB_MSG = 0                        # Clear Selfboot Message Register
    SB_CS.SB_CS_START_RESTART_VECTOR0 = 0    # Clear SBE start bits to make sure the CBS creates a 0->1 transition
    SB_CS.SB_CS_START_RESTART_VECTOR1 = 0
    FSB_DOWNFIFO_RESET = 0x80000000   # Reset SBE FIFO

    if CBS_ENVSTAT.CBS_ENVSTAT_C4_TEST_ENABLE:
        ## Test mode, enable TP drivers/receivers for GSD scan out
        ROOT_CTRL1.TP_RI_DC_B = 1
        ROOT_CTRL1.TP_DI2_DC_B = 1
        ROOT_CTRL1_COPY.TP_RI_DC_B = 1
        ROOT_CTRL1_COPY.TP_DI2_DC_B = 1

    # Prepare for CBS start
    CBS_CS.START_BOOT_SEQUENCER = 0
    CBS_CS.OPTION_SKIP_SCAN0_CLOCKSTART = not i_scan0_clockstart
    CBS_CS.OPTION_PREVENT_SBE_START = not i_start_sbe
    # write CBS_CS

def mod_cbs_start(target<ANY_POZ_CHIP>, bool i_start_sbe=true, bool i_scan0_clockstart=true):
    # This module uses CFAM accesses for everything

    mod_cbs_start_prep(i_target, i_start_sbe, i_scan0_clockstart)

    CBS_CS.START_BOOT_SEQUENCER = 1   # Start CBS
    # Leave START_BOOT_SEQUENCER at 1 to prevent accidental restarts

    # CBS_STATE_IDLE = 0x002
    wait until CBS_CS.INTERNAL_STATE_VECTOR == CBS_STATE_IDLE, time out after 200 x delay(640us, 750kcyc)
    if timed out:
        ASSERT(CBS_NOT_IN_IDLE_STATE)

def mod_switch_pcbmux(target<ANY_POZ_CHIP>, mux_type i_path):
    uint8_t l_oob_mux_save = ROOT_CTRL0.OOB_MUX
    ROOT_CTRL0.OOB_MUX = 1
    # write ROOT_CTRL0
    ROOT_CTRL0.PCB_RESET = 1
    # write ROOT_CTRL0
    ROOT_CTRL0[bit i_path] = 1               # Enable the new path first to prevent glitches
    # write ROOT_CTRL0
    ROOT_CTRL0[16+18+19 except i_path] = 0   # Then disable the old path
    # write ROOT_CTRL0
    ROOT_CTRL0.PCB_RESET = 0
    # write ROOT_CTRL0
    ROOT_CTRL0.OOB_MUX = l_oob_mux_save

def mod_unmask_firs(target<ANY_POZ_CHIP>):
    with all good chiplets except TP via multicast:
        EPS_MASK = LFIR_MASK_DEFAULT    # 0x80dfffffffffffff
        XSTOP_MASK = XSTOP_MASK_ANY_ATTN_AND_DBG   # 0x3000000000000000
        RECOV_MASK = RECOV_MASK_LOCAL_XSTOP        # 0x2000000000000000
        ATTN_MASK = 0
        LOCAL_XSTOP_MASK = 0

def mod_setup_clockstop_on_xstop(target<ANY_POZ_CHIP>, const uint8_t i_chiplet_delays[64]):
    if not ATTR_CLOCKSTOP_ON_XSTOP:
        return

    # XSTOP1_INIT_VALUE = 0x97FFE00000000000
    # you can copy this code from p10_sbe_chiplet_fir_init.C
    if ATTR_CLOCKSTOP_ON_XSTOP[bit EPS_FIR_CLKSTOP_ON_XSTOP_MASK1_SYS_XSTOP_STAGED_ERR]:
        ## Staged xstop is masked, leave all delays at 0 for fast stopping
        with MCGROUP_ALL_BUT_TP:
            XSTOP1 = XSTOP1_INIT_VALUE
    else:
        ## Staged xstop is unmasked, set up per-chiplet delays
        for chiplet in MCGROUP_ALL_BUT_TP:
            XSTOP1 = XSTOP1_INIT_VALUE
            XSTOP1.WAIT_CYCLES = 4 * (4 - i_chiplet_delays[chiplet.getChipletNumber()])

    ## Enable clockstop on checkstop
    EPS_FIR_CLKSTOP_ON_XSTOP_MASK1.flush<1>.insertFromRight<0, 8>(ATTR_CLOCKSTOP_ON_XSTOP);

def mod_hangpulse_setup(target<PERV | MC>, uint8_t i_pre_divider, hang_pulse_t *i_hangpulse_table):
    PRE_COUNTER_REG = 0
    PRE_COUNTER_REG.PRE_COUNTER = i_pre_divider
    while True:
        HANG_PULSE_0_REG = 0
        HANG_PULSE_0_REG.HANG_PULSE_REG_0 = i_hangpulse_table->value
        HANG_PULSE_0_REG.SUPPRESS_HANG_0  = i_hangpulse_table->stop_on_xstop
        putScom(i_target, scomt::perv::HANG_PULSE_0_REG + i_hangpulse_table->id, HANG_PULSE_0_REG);

        if i_hangpulse_table->last:
            return

        i_hangpulse_table++;

def mod_constant_hangpulse_setup(target<ANY_POZ_CHIP>, uint32_t i_base_address, const constant_hang_pulse_t i_hangpulses[4]):
    for i in 0..3:
        PRE_COUNTER_REG_0 = 0
        PRE_COUNTER_REG_0.PRE_COUNTER0 = i_hangpulses[i].pre_divider
        putScom(i_target, i_base_address + i * 2 + 2, PRE_COUNTER_REG_0);

        HANG_PULSE_0_REG = 0
        HANG_PULSE_0_REG.HANG_PULSE_REG_0 = i_hangpulses[i].value
        HANG_PULSE_0_REG.SUPPRESS_HANG_0  = i_hangpulses[i].stop_on_xstop
        putScom(i_target, i_base_address + i * 2 + 1, HANG_PULSE_0_REG);

def mod_multicast_setup(target<ANY_POZ_CHIP>, uint8_t i_group_id, uint64_t i_chiplets, TargetState i_pgood_policy):
    if i_group_id > 6:
        ASSERT(invalid parameter)

    ## Determine required group members
    l_eligible_chiplets = 0
    for chiplet in i_target.getChildren(all chiplets, i_pgood_policy):
        l_eligible_chiplets.setBit(chiplet.getChipletNumber());
    l_required_group_members = l_eligible_chiplets & i_chiplets

    ## Determine current group members
    # MC_GROUP_MEMBERSHIP_BITX_READ = 0x500F0001
    # This performs a multicast read with the BITX merge operation.
    # It reads a register that has bit 0 tied to 1, so the return value
    # will have a 1 for each chiplet that is a member of the targeted group.
    l_current_group_members = getScom(i_target, MC_GROUP_MEMBERSHIP_BITX_READ | ((uint32_t)i_group_id << 24))

    ## Update group membership where needed
    for i in 0..63:
        const bool want = l_required_group_members.getBit(i);
        const bool have = l_current_group_members.getBit(i);

        if want == have:
            continue

        const uint64_t prev_group = have ? i_group_id : 7;
        const uint64_t new_group  = want ? i_group_id : 7;
        putScom(i_target, (PCB_RESPONDER_MULTICAST_GROUP_1 + i_group_id) | (i << 24), (new_group << 58) | (prev_group << 42));

def mod_poz_tp_init_common(target<ANY_POZ_CHIP>):
    SB_CS.SB_CS_START_RESTART_VECTOR0 = 0    # Clear SBE start bits to be tidy; they're no longer needed now
    SB_CS.SB_CS_START_RESTART_VECTOR1 = 0
    ROOT_CTRL0.FSI_CC_CBS_CMD = 0            # Clear CBS command to enable clock gating inside clock controller

    # IPOLL_MASK_INIT = 0xF800_0000_0000_0000
    HOST_MASK_REG = IPOLL_MASK_INIT          # Set up IPOLL mask
    CPLT_CTRL2 = ATTR_PG(PERV)               # Transfer PERV partial good attribute into region good register
    PERV_CTRL0.VITL_CG_DIS = 0               # Enabe PERV vital clock gating
    CPLT_CTRL0.CTRL_CC_FORCE_ALIGN_DC = 0    # Disable alignment pulse

    EPS_MASK = TP_LFIR_MASK_DEFAULT            # 0x80c1c7fcf3fbffff
    XSTOP_MASK = XSTOP_MASK_ANY_ATTN_AND_DBG   # 0x3000000000000000
    RECOV_MASK = RECOV_MASK_LOCAL_XSTOP        # 0x2000000000000000
    ATTN_MASK = 0
    LOCAL_XSTOP_MASK = 0

    ## Enable Pervasive drivers/receivers
    ROOT_CTRL1.TP_RI_DC_B  = 1
    ROOT_CTRL1.TP_DI1_DC_B = 1
    ROOT_CTRL1.TP_DI2_DC_B = 1

ISTEP(99, 99, "poz_perv_mod_bist", "")

def poz_bist(target<ANY_POZ_CHIP>, bist_params &i_params):
    mod_multicast_setup(group6 = i_params.chiplets)

    if i_params.do_pre_scan0_gtr:
        mod_scan0(group6, regions=i_params.regions, scan_types=[gptr, time, repr])

    if i_params.do_pre_scan0_rest:
        mod_scan0(group6, regions=i_params.regions)

    if i_params.do_setup:
        putRing(i_params.setup_image)
        if i_params.abist_not_lbist:
            mod_abist_setup(group6, i_params.abist)
        else:
            mod_lbist_setup(group6, i_params.lbist)

    if i_params.do_run:
        mod_bist_go(group6)
        mod_bist_poll(group6, i_params.flags)   # poll on the right thing depending on flags

    if i_params.do_compare:
        compareRing(i_params.check_image)

    if i_params.do_cleanup:
        mod_bist_cleanup(group6)      # zero out a bunch of CC regs, doesn't matter if we over-clear

    if i_params.do_scan0_gtr:
        mod_scan0(group6, regions=i_params.regions, scan_types=[gptr, time, repr])

    if i_params.do_scan0_rest:
        mod_scan0(group6, regions=i_params.regions)

    if i_params.do_arrayinit:
        mod_abist_start(group6, regions=i_params.regions)
        mod_abist_poll(group6)
        mod_abist_cleanup(group6)
        mod_scan0(group6, regions=i_params.regions)
