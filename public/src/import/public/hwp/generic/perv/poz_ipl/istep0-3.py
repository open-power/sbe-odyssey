# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/poz_ipl/istep0-3.py $
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
"""
IPL 0..3 low-level definition

This is named and roughly formatted like a Python file so we get syntax
highlighting, but it's not real Python code :) It's more of a mix between
pseudo code and semi-formal descriptions of register accesses.

isteps are delimited by an ISTEP(major_num, minor_num, name, platform) line,
where "platform" denotes which platform(s) will execute the step.

HWPs and modules are declared using "def" just like a Python function.
If no parameters are provided for a HWP, it will only take a chip target.

Register reads/writes are described like record accesses:
     REGISTER.FIELD = value

The target to use for these accesses should always be obvious from context:
  * At a procedure/module top level it's the function's input target
  * Inside a for loop over targets it's the for loop variable
  * Otherwise it's explicitly specified through a with block

Consecutive changes to the same register can be bundled into a single write
unless specified otherwise. If a write ends up statically setting or clearing
bits, please use the WRITE_OR / WRITE_CLEAR addresses if available.
"""


"""
Coding guidelines:

* #ifdef is evil, use compile-time constants and if/then/else whenever possible
* If you pull parts of code out into utility functions that aren't shared:
   * make them static and local to the .C file (allows for inlining)
   * put them above the HWP function (avoids prototype declaration)
"""


# All mod_* modules are defined in external files
from perv_modules import *

# Definition of ISTEP to clarify the format
def ISTEP(major_num, minor_num, name, platform):
    pass

# Definition of delay to clarify the format
def delay(time, simcycles):
    pass


"""
Step 0: Power on, CFAM reset, basic set up and start the CBS
"""

ISTEP(0, 99, "sim_model_to_standby", "Cronus")

def sim_model_to_standby():
    Enable standby voltages (VSB, V3p3, what else?), leave the rest off
    Disable reference clock
    Set up model facs as needed
    DO NOT issue CFAM reset yet

ISTEP(0, 1, "pre_cfam_reset", "BMC")

def p11s_shutdown():
    # This HWP is called both prior to power off as well as prior to pulling CFAM reset during IPL
    # CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF
    GPWRP = CONTROL_WRITE_PROTECT_DISABLE         # Disable Write Protection for Root/Perv Control registers (in case the chip has just been reset)

    ROOT_CTRL1.GLOBAL_PERST_OVERRIDE = 1          # Assert all PERST# outputs
    ROOT_CTRL0.OCMB0_RESET = 1                    # Assert OCMB reset
    ROOT_CTRL0.OCMB1_RESET = 1                    # Assert OCMB reset
    delay(100ms, 0cyc)                            # Don't wait in sim, but do wait on real HW to give PCI devices time to reset

    PERV_CTRL0.PCB_EP_RESET = 1                   # Raise PERV EP reset to raise all PERV region fences
    PERV_CTRL0.PERV2FSI_CHIPLET_FENCE = 1         # Raise Pervasive chiplet fence to protect FSI
    ROOT_CTRL0.CFAM_PROTECTION_0 = 1              # Raise CFAM protections
    ROOT_CTRL0[bits 8:15] = 0xFF                  # Raise CFAM protections
    ROOT_CTRL0.GLOBAL_EP_RESET = 1                # Assert global endpoint reset to reset all chiplets
    ROOT_CTRL7 = 0                                # Turn off all outgoing refclocks
    ROOT_CTRL1[5 bits starting at TP_RI_DC_B] = 0 # Disable TP drivers and receivers
    ROOT_CTRL0.TP_I2C_BUS_FENCE = 0               # Clear FSI I2C fence to allow access from FSP side

def zme_shutdown():
    p11s_shutdown()

ISTEP(0, 2, "cfam_reset", "BMC")

# BMC issues CFAM_RESET at this point
def sim_p11s_cfam_reset():
    # Do not toggle Tap reset, this is done by Spinal later
    Toggle Spinal CFAM_RESET

    # Sim workaround for X states in efuse that can't be cleaned out any other way (EWM 284776)
    wait for 24k cycles
    set OTPROM.SINGLE_OTP_ROM.OTPROM.EFUSE.FIRST_CSB to 1

ISTEP(0, 3, "poweron", "BMC")
# if power is already on, this is a no-op

def p11s_pre_poweron():
    # placeholder for lab workarounds

# BMC executes voltage power on sequence
def sim_poweron_sequence():
    Turn on VDN, VDD, VIO in that order; clock a few thousand FSI cycles in between each
    Turn on Odyssey power
    Enable refclock, wait a few thousand refclock cycles

    Toggle Ody CFAM_RESET
    # Depending on model wiring the processor might still be holding Ody CFAM_RESET
    # at this point. Let's find out!

ISTEP(0, 6, "setup_ref_clock", "BMC")

def pz_setup_ref_clock(target<PROC_CHIP | HUB_CHIP>):
    ## Disable Write Protection for Root/Perv Control registers
    # CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF
    GPWRP = CONTROL_WRITE_PROTECT_DISABLE

    ## Set RCS control signals to CFAM reset values, apply basic configuration for output clock enables and forced input clock.
    ROOT_CTRL5 = 0
    ROOT_CTRL5.TPFSI_RCS_RESET_DC = 1
    ROOT_CTRL5.TPFSI_RCS_BYPASS_DC = 1
    ROOT_CTRL5.TPFSI_RCS_FORCE_CLKSEL_DC = 1 if ATTR_CP_REFCLOCK_SELECT is *OSC1* else 0
    ROOT_CTRL5.TPFSI_RCS_CLK_TEST_IN_DC = 0
    ROOT_CTRL5.EN_REFCLK = ATTR_CLOCK_RCS_OUTPUT_MUX20 == SYNC
    ROOT_CTRL5.EN_ASYNC_OUT = ATTR_CLOCK_RCS_OUTPUT_MUX20 == ASYNC

    ROOT_CTRL5_COPY = ROOT_CTRL5      # Update copy register to match

    ## Set up refclock receiver termination
    ROOT_CTRL6 = 0
    ROOT_CTRL6.TP_AN_SYS0_RX_REFCLK_TERM = ATTR_SYS0_REFCLOCK_RCVR_TERM
    ROOT_CTRL6.TP_AN_SYS1_RX_REFCLK_TERM = ATTR_SYS1_REFCLOCK_RCVR_TERM

    ROOT_CTRL6_COPY = ROOT_CTRL6      # Update copy register to match

    ## Unprotect inputs to RCS sense register
    ROOT_CTRL0.CFAM_PROTECTION_0 = 0
    ROOT_CTRL0_COPY.CFAM_PROTECTION_0 = 0

def p11s_setup_ref_clock():
    pz_setup_ref_clock()

    ## Set up refclock transmitter termination
    ROOT_CTRL7 = 0
    ROOT_CTRL7.TAP_TERM_EN     = ATTR_TAP_TERM_EN          # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.OMI_TERM_EN     = ATTR_OMI_TERM_EN          # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.E0_TERM_EN      = ATTR_PCI_TERM_EN          # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.E8OP_TERM_EN    = ATTR_PCI8OP_TERM_EN       # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.E9OP_TERM_EN    = ATTR_PCI9OP_TERM_EN       # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.E10OP_TERM_EN   = ATTR_PCI10OP_TERM_EN      # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.E11OP_TERM_EN   = ATTR_PCI11OP_TERM_EN      # uint8, DISABLE = 0, ENABLE = 1
    ROOT_CTRL7.TAP_2X_CUR_EN   = ATTR_TAP_DRV_STRENGTH     # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.OMI_2X_CUR_EN   = ATTR_OMI_DRV_STRENGTH     # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.E0_2X_CUR_EN    = ATTR_PCI_DRV_STRENGTH     # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.E8OP_2X_CUR_EN  = ATTR_PCI8OP_DRV_STRENGTH  # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.E9OP_2X_CUR_EN  = ATTR_PCI9OP_DRV_STRENGTH  # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.E10OP_2X_CUR_EN = ATTR_PCI10OP_DRV_STRENGTH # uint8, NORMAL = 0, DOUBLE = 1
    ROOT_CTRL7.E11OP_2X_CUR_EN = ATTR_PCI11OP_DRV_STRENGTH # uint8, NORMAL = 0, DOUBLE = 1

    ROOT_CTRL7_COPY = ROOT_CTRL7      # Update copy register to match

    ## Set up clock muxing, application dependent
    ROOT_CTRL4 = 0
    ROOT_CTRL4.CLKMUX0_SEL  = ATTR_CLOCK_TOD_INPUT_MUX0             # uint8, FPLL1 = 0, FPLL2 = 1
    ROOT_CTRL4.TEST_TCK_SEL = ATTR_CLOCK_NEST_TCK_MUX1              # uint8, NEST_PLL = 0, TCK = 1
    ROOT_CTRL4.CLKMUX20_SEL = ATTR_CLOCK_RCS_OUTPUT_MUX20           # uint8, BYPASS = 0, SYNC = 1, ASYNC = 2
    ROOT_CTRL4.CLKMUX21_SEL = ATTR_CLOCK_FPLL2_INPUT_MUX21          # uint8, FPLL1 = 0, MUX20 = 1
    ROOT_CTRL4.CLKMUX22_SEL = ATTR_CLOCK_FPLL34_INPUT_MUX22         # uint8, FPLL2 = 0, MUX21 = 1
    ROOT_CTRL4.CLKMUX23_SEL = ATTR_CLOCK_133M_FPLL12_MUX23          # uint8, FPLL1 = 0, FPLL2 = 1
    ROOT_CTRL4.CLKMUX24_SEL = ATTR_CLOCK_133M_NEST_REFCLK_MUX24     # uint8, MUX23 = 0, FPLL3 = 1, FPLL4 = 2
    ROOT_CTRL4.CLKMUX25_SEL = ATTR_CLOCK_133M_MC_REFCLK_MUX25       # uint8, MUX23 = 0, FPLL3 = 1, FPLL4 = 2
    ROOT_CTRL4.CLKMUX26_SEL = ATTR_CLOCK_133M_PAXO_REFCLK_MUX26     # uint8, MUX23 = 0, FPLL3 = 1, FPLL4 = 2
    ROOT_CTRL4.CLKMUX27_SEL = ATTR_CLOCK_100M_PAXO_REFCLK_MUX27     # uint8, FPLL2 = 0, FPLL3 = 1, FPLL4 = 2
    ROOT_CTRL4.CLKMUX28_SEL = ATTR_CLOCK_100M_PAXO_REFCLK_MUX28     # uint8, MUX23 = 0, FPLL3 = 1, FPLL4 = 2
    ROOT_CTRL4.NEST_GLM_ASYNC_RESET = 0

    ROOT_CTRL4_COPY = ROOT_CTRL4      # Update copy register to match

def zme_setup_ref_clock():
    pz_setup_ref_clock()

    ## Set up refclock transmitter termination
    ROOT_CTRL7 = 0
    # TBD

    ROOT_CTRL7_COPY = ROOT_CTRL7      # Update copy register to match

    ## Set up clock muxing, application dependent
    ROOT_CTRL4 = 0
    # TODO:  Set up zMe refclock muxing
    ROOT_CTRL4.NEST_GLM_ASYNC_RESET = 0
    ROOT_CTRL4.NEST_DIV2_ASYNC_RESET = 0

    ROOT_CTRL4_COPY = ROOT_CTRL4      # Update copy register to match

ISTEP(0, 7, "clock_test", "BMC")

def p11s_clock_test():
    mod_clock_test(path=CFAM)

def zme_clock_test():
    mod_clock_test(path=CFAM)

ISTEP(0, 13, "ph_sppe_config_update", "BMC")

def p11s_sppe_config_update():
    # provided by VBU team

def ody_sppe_config_update():
    # provided by VBU team

def zme_sppe_config_update():
    # TBD

ISTEP(0, 14, "cbs_start", "BMC")

def p11s_cbs_start(target<HUB_CHIP>, bool i_start_sbe=true):
    mod_cbs_start(i_target, i_start_sbe)

def ody_cbs_start(target<OCMB_CHIP>, bool i_start_sbe=true):
    mod_cbs_start(i_target, i_start_sbe)

def zme_cbs_start(target<PROC_CHIP>, bool i_start_sbe=true):
    mod_cbs_start(i_target, i_start_sbe)

# You can use this HWP instead of cbs_start if you want to
# emulate the behavior of the CBS via explicit CFAM accesses
def poz_cbs_emulate(target<ANY_POZ_CHIP>, bool i_start_sbe=true, bool i_scan0_clockstart=true):
    # This module uses CFAM accesses for everything

    mod_cbs_start_prep(i_target, i_start_sbe, i_scan0_clockstart)

    # CBS state 1: load_gp_regs
    for i in 0..11:
        putCfam(ROOT_CTRL0 + i, getCfam(ROOT_CTRL0_COPY + i))

    # CBS state 2: set_ep_reset
    ROOT_CTRL0.VDD2VIO_LVL_FENCE = 0
    PERV_CTRL0.PCB_EP_RESET = 1
    PERV_CTRL0.GLM_TCK_ASYNC_RESET = 0

    # CBS state 3: clear_ep_reset
    ROOT_CTRL0.CFAM_PROTECTION_1 = 0
    PERV_CTRL0.PCB_EP_RESET = 0

    if i_scan0_clockstart:
        # CBS state 4: set_cc_cmd
        ROOT_CTRL0.FSI_CC_CBS_CMD = 0b111

        # CBS state 5: set_cc_req
        ROOT_CTRL0.FSI_CC_CBS_REQ = 1

        # CBS state 6: wait for cc_ack
        timeout = 100
        while not CBS_STAT.CBS_STAT_DBG_TP_TPFSI_CBS_ACK:
            timeout -= 1
            if timeout == 0:
                ASSERT(CBS_NOT_IN_IDLE_STATE)

        # CBS state 7: clear_cc_req
        ROOT_CTRL0.FSI_CC_CBS_REQ = 0

        # CBS state 8: wait for scan0 done
        timeout = 1000
        while CBS_STAT.CBS_STAT_DBG_TP_TPFSI_CBS_ACK:
            timeout -= 1
            if timeout == 0:
                ASSERT(CBS_NOT_IN_IDLE_STATE)

    # CBS state 9: clear_fences
    ROOT_CTRL0[bits 9:13] = 0
    ROOT_CTRL0.FSI2PCB = 0
    ROOT_CTRL1.TC_PERV_RI_DC_N = 1
    ROOT_CTRL1.TC_PERV_DI1_DC_N = 1
    PERV_CTRL0.CHIPLET_EN = 1
    PERV_CTRL0.PERV2FSI_CHIPLET_FENCE = 0

    # CBS state 10 and 11 no longer exist

    # CBS state 12: set_sppe_start
    if i_start_sbe:
        SB_CS.SB_CS_START_RESTART_VECTOR0 = 1

    mod_cbs_cleanup(i_target)

def poz_prep_chip_for_tp_lbist():
    Kick CBS without scan0/clockstart/SBE
    Set up PCB mux

"""
Step 1: Spinal TP init, SPPE tries to find its feet

high level flow:
  1. enable SEEPROM
  2. reset/enable TP chiplet
  3. lock PLLs
  4. set up PIB region only (for PIBMEM) via command table
  5. run measurement, boot into full SPPE
  6. set up rest of TP regions (perv, net, occ)
  7. perform misc TP setup (hang pulses, clocking ...)

alternate flow:
  4. set up all TP regions via command table
  5. run measurement, boot into full SPPE
  6. perform misc TP setup
"""

ISTEP(1, 1, "ph_enable_seeprom", "SPPE")
# executes from OTPROM, implemented in Assembler by SBE team

def p11s_enable_seeprom():
    "NOTE set up SPI for at-speed operation already"
    # delivered by SBE team

def ody_enable_seeprom():
    "NOTE set up SPI for at-speed operation already"
    # delivered by SBE team

def zme_enable_seeprom():
    "NOTE set up SPI for at-speed operation already"
    # delivered by SBE team

ISTEP(1, 2, "ph_tp_chiplet_reset", "SPPE")
# executes from ROM driven by command table

def p11s_tp_chiplet_reset():
    ROOT_CTRL0.PCB_RESET = 0           # Drop PCB interface reset to enable access into TP chiplet
    OPCG_ALIGN.OPCG_WAIT_CYCLES = 0x20 # Increase OPCG wait cycles to enable scanning

    ROOT_CTRL1.TP_TPM_DI1_DC_B = 0b1   # Enable TPM SPI port

    CPLT_CONF1.NEST_PDLY = 7                       # Set up static progdelay for the nest mesh
    PERV_CTRL1.TP_CPLT_CLK_NEST_PDLY_BYPASS_DC = 0 # Drop nest PDLY/DCC bypass

    PERV_CTRL0.SRAM_ENABLE_DC = 1

def ody_tp_chiplet_reset():
    ROOT_CTRL0.PCB_RESET = 0           # Drop PCB interface reset to enable access into TP chiplet
    OPCG_ALIGN.OPCG_WAIT_CYCLES = 0x20 # Increase OPCG wait cycles to enable scanning

def zme_tp_chiplet_reset():
    ROOT_CTRL0.PCB_RESET = 0           # Drop PCB interface reset to enable access into TP chiplet
    OPCG_ALIGN.OPCG_WAIT_CYCLES = 0x20 # Increase OPCG wait cycles to enable scanning

    if ATTR_BURNIN:
        ROOT_CTRL1.BURNIN_MODE = 1

    PERV_CTRL0.SRAM_ENABLE_DC = 1

ISTEP(1, 3, "ph_tp_pll_initf", "SPPE")
# executes from ROM driven by command table
# PLL buckets for Z!

def p11s_tp_pll_initf():
    putRing( perv_pll = perv_pll_bndy )

def ody_tp_pll_initf():
    putRing( perv_pll = perv_pll_bndy )

def zme_tp_pll_initf():
    putRing( perv_pll = perv_pll_bndy )

ISTEP(1, 4, "ph_tp_pll_setup", "SPPE")
# executes from ROM driven by command table

def p11s_tp_pll_setup():
    CPLT_CTRL1.REGION13_FENCE = 0   # Drop PLL region fence

    if not ATTR_CP_PLLFLT_BYPASS:
        ## Start chip filter PLLs
        ROOT_CTRL3.TP_PLLFLT1_TEST_EN_DC = 0
        ROOT_CTRL3.TP_PLLFLT2_TEST_EN_DC = 0
        ROOT_CTRL3.TP_PLLFLT3_TEST_EN_DC = 0
        ROOT_CTRL3.TP_PLLFLT4_TEST_EN_DC = 0
        # write ROOT_CTRL3
        ROOT_CTRL3.TP_PLLFLT1_RESET_DC = 0
        ROOT_CTRL3.TP_PLLFLT2_RESET_DC = 0
        ROOT_CTRL3.TP_PLLFLT3_RESET_DC = 0
        ROOT_CTRL3.TP_PLLFLT4_RESET_DC = 0

        mod_poll_pll_lock_fsi2pib(i_target, P11S_PERV_FPLL1 | P11S_PERV_FPLL2 | P11S_PERV_FPLL3 | P11S_PERV_FPLL4)

        ## Take chip filter PLLs out of bypass
        ROOT_CTRL3.TP_PLLFLT1_BYPASS_DC = 0
        ROOT_CTRL3.TP_PLLFLT2_BYPASS_DC = 0
        ROOT_CTRL3.TP_PLLFLT3_BYPASS_DC = 0
        ROOT_CTRL3.TP_PLLFLT4_BYPASS_DC = 0

    if ATTR_CP_PLLNEST_BYPASS:
        SCRATCH_REGISTER_6[16:31] = 100         # store refclock frequency in nest frequency field of scratch reg
    else:
        ## Attempt to lock Nest PLL
        ROOT_CTRL3.TP_PLLNEST_TEST_EN_DC = 0    # not available in headers yet - bit 24
        # write ROOT_CTRL3
        ROOT_CTRL3.TP_PLLNEST_RESET_DC = 0      # not available in headers yet - bit 25

        mod_poll_pll_lock_fsi2pib(i_target, P11S_PERV_PLLNEST)

        ## Prepare chip for at-speed operation
        OPCG_ALIGN.SCAN_RATIO = 3               # Switch scan ratio to 4:1

        ## Take Nest PLL out of bypass
        ROOT_CTRL3.TP_PLLNEST_BYPASS_DC = 0

def ody_tp_pll_setup():
    CPLT_CTRL1.REGION13_FENCE = 0   # Drop PLL region fence

    if ATTR_PLL_BYPASS:
        SCRATCH_REGISTER_6[16:31] = 133       # store refclock frequency in nest frequency field of scratch reg
    else:
        ## Attempt to lock PLL
        ROOT_CTRL3.TP_MCPLL_TEST_EN_DC = 0    # not available in headers yet - bit 24
        ROOT_CTRL3.TP_MCPLL_RESET_DC = 0      # not available in headers yet - bit 25
        mod_poll_pll_lock_fsi2pib(i_target, ODY_PERV_MCPLL)    # ODY_PERV_MCPLL = 0x02

        ## Prepare chip for at-speed operation
        OPCG_ALIGN.SCAN_RATIO = 3             # Switch scan ratio to 4:1

        ## Take PLL out of bypass
        ROOT_CTRL3.TP_MCPLL_BYPASS_DC = 0     # not available in headers yet - bit 26

def zme_tp_pll_setup():
    like p11s_tp_pll_setup but do not set up SCAN_RATIO

ISTEP(1, 5, "ph_pib_repr_initf", "SPPE")
# executes from ROM driven by command table
# scan0 has been performed by CBS

def p11s_pib_repr_initf():
    putRing( pib_rtg = pib_gptr+pib_time+pib_repr(+ optional pib_abst) )

def ody_pib_repr_initf():
    putRing( pib_rtg = pib_gptr+pib_time+pib_repr(+ optional pib_abst) )

def zme_pib_repr_initf():
    putRing( pib_rtg = pib_gptr+pib_time+pib_repr(+ optional pib_abst) )

"""
Arrayinit is split into two steps; the first starts+polls, the second cleans up.
The intent is for DFT to use these steps for ABIST; they can scan in an ABIST
configuration during repr_initf, then run arrayinit to run ABIST and move in
to collect results before we clean up.
"""

ISTEP(1, 6, "ph_pib_arrayinit", "SPPE")
# executes from ROM driven by command table

def p11s_pib_arrayinit():
    mod_abist_start(regions=[pib])
    mod_abist_poll()

def ody_pib_arrayinit():
    same as p11s_pib_arrayinit()

def zme_pib_arrayinit():
    same as p11s_pib_arrayinit()

ISTEP(1, 7, "ph_pib_arrayinit_cleanup", "SPPE")
# executes from ROM driven by command table

def p11s_pib_arrayinit_cleanup():
    mod_abist_cleanup()
    mod_scan0(regions=[pib])

def ody_pib_arrayinit_cleanup():
    same as p11s_pib_arrayinit_cleanup()

def zme_pib_arrayinit_cleanup():
    same as p11s_pib_arrayinit_cleanup()

"""
PIB/SBE LBIST flow:
 1. mod_start_cbs(i_sbe_start=false, i_scan0_clockstart=false)
 2. mod_switch_pcbmux(mux::FSI2PCB on P/Z or mux::I2C2PCB on Odyssey)
 3. run command table up to this point using FAPI or Python interpreter
    remember to translate mailbox scoms into cfams (use cmdtable.py run --cfam)
 4. mod_start_stop_clocks(regions=[sbe], start_not_stop=false)
 5. run LBIST
"""

ISTEP(1, 9, "ph_pib_startclocks", "SPPE")
# executes from ROM driven by command table

def p11s_pib_startclocks():
    mod_start_stop_clocks(regions=[pib])

def ody_pib_startclocks():
    same as p11s_pib_startclocks()

def zme_pib_startclocks():
    same as p11s_pib_startclocks()

ISTEP(1, 10, "ph_sppe_measure", "SPPE")
# executes from ROM
# copy SPPE L2 loader code into SPPE RAM

ISTEP(1, 11, "ph_sppe_load", "SPPE")
# copy SPPE code into SPPE RAM
# Break up into individual steps?
# Z: Also load SBE

ISTEP(1, 12, "ph_sppe_boot", "SPPE, BMC")
# SPPE boots, BMC can monitor progress and time out if boot fails
# Z: Also boot SBE
# This is the sync point where Cronus intercepts the SPPE if USE_SBE_FIFO == istep
# After this point the SPPE either continues autoboot or waits for Cronus to request individual isteps

ISTEP(1, 13, "ph_sppe_check_for_ready", "BMC")

def p11s_sppe_check_for_ready():
    # This runs on the BMC and monitors SPPE boot progress
    # Cronus HWP may wish to output progress updates :)
    # It will wait for SPPE standby, which depending on scratch settings
    # is either _before_ the rest of isteps or _after_ (in case of autoboot)

def ody_sppe_check_for_ready():
    "TODO: need this?"

def zme_sppe_check_for_ready():
    p11s_sppe_check_for_ready()

ISTEP(1, 13, "ph_sppe_attr_setup", "SPPE")

def p11s_sppe_attr_setup():
    pass

def ody_sppe_attr_setup():
    pass

def zme_sppe_attr_setup():
    pass

ISTEP(1, 15, "ph_rcs_setup", "SPPE")

def poz_rcs_setup():
    # provided by IO team / Ryan Miller followon

def p11s_rcs_setup():
    poz_rcs_setup()

def zme_rcs_setup():
    poz_rcs_setup()

ISTEP(1, 17, "ph_tp_repr_initf", "SPPE")

def p11s_tp_repr_initf():
    putRing( TP chiplet, perv_rtg = {perv,net,occ}_{gptr,time,repr}(+*_abst as needed))} )

def ody_tp_repr_initf():
    putRing( TP chiplet, perv_rtg = {perv,net}_{gptr,time,repr}(+*_abst as needed))} )

def zme_tp_repr_initf():
    putRing( TP chiplet, perv_rtg = {perv,net,ana,mbio}_{gptr,time,repr}(+*_abst as needed))} )

ISTEP(1, 18, "ph_tp_arrayinit", "SPPE")

def poz_tp_arrayinit():
    mod_abist_start(regions=[all except sbe/pib/pll])
    mod_abist_poll()
    compareRing( perv_abst_check )   # if perv_abst_check is empty or does not exist, skip check

def p11s_tp_arrayinit():
    poz_tp_arrayinit()

def ody_tp_arrayinit():
    poz_tp_arrayinit()

def zme_tp_arrayinit():
    poz_tp_arrayinit()

ISTEP(1, 19, "ph_tp_arrayinit_cleanup", "SPPE")

def poz_tp_arrayinit_cleanup():
    mod_abist_cleanup()
    mod_scan0(regions=[all except sbe/pib/pll])

def p11s_tp_arrayinit_cleanup():
    poz_tp_arrayinit_cleanup()

def ody_tp_arrayinit_cleanup():
    poz_tp_arrayinit_cleanup()

def zme_tp_arrayinit_cleanup():
    poz_tp_arrayinit_cleanup()

"""
LBIST: DFT can wedge in LBIST for perv, net, occ in here
"""

ISTEP(1, 20, "ph_tp_initf", "SPPE")

def p11s_tp_initf():
    putRing( TP chiplet, perv_initf = {perv,net,occ}_func )

def ody_tp_initf():
    putRing( TP chiplet, perv_initf = {perv,net}_func )

def zme_tp_initf():
    putRing( TP chiplet, perv_initf = {perv,net,ana,mbio}_func )

ISTEP(1, 21, "ph_tp_startclocks", "SPPE")

def p11s_tp_startclocks():
    mod_start_stop_clocks(regions=[perv, net, occ])

def ody_tp_startclocks():
    mod_start_stop_clocks(regions=[perv, net])

def zme_tp_startclocks():
    mod_start_stop_clocks(regions=[perv, net, ana, mbio])

ISTEP(1, 22, "ph_tp_init", "SPPE")

def p11s_tp_init():
    # TODO : Set up TOD error routing, error mask via scan inits
    # TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    ## Start using PCB network
    ROOT_CTRL0.GLOBAL_EP_RESET = 0        # Drop Global Endpoint reset
    mod_switch_pcbmux(i_target, mux::PCB2PCB)
    ROOT_CTRL0.OOB_MUX = 0

    ## Set up static multicast groups
    mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)

    ## Set up chiplet hang pulses
    mod_hangpulse_setup(MCGROUP_GOOD, 9, {{0, 14, 0}, {5, 4, 0}, {6, 5, 0, 1}})
    mod_hangpulse_setup(TP chiplet, 9, {{1, 1, 0, 1}})
    mod_hangpulse_setup(N0 chiplet, 9, {{1, 22, 1}, {2, 29, 1}, {3, 13, 1, 1}})
    mod_hangpulse_setup(N1 chiplet, 9, {{1, 22, 1}, {2, 29, 1}, {3, 13, 1}, {4, 15, 1, 1}})
    mod_hangpulse_setup(N2 chiplet, 9, {{1, 20, 1}, {2, 14, 1}, {3, 18, 1}, {4, 2, 1, 1}})
    mod_hangpulse_setup(all good PAXO, 9, {{2, 1, 0, 1}})
    mod_hangpulse_setup(all good TBUS, 9, {{2, 1, 0, 1}})
    mod_hangpulse_setup(all good MC,   9, {{2, 1, 0, 1}})

    ## Unmask TP PLL unlock reporting
    if not ATTR_CP_PLLFLT_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(P11S_PERV_FPLL1 | P11S_PERV_FPLL2 | P11S_PERV_FPLL3 | P11S_PERV_FPLL4)

    if not ATTR_CP_PLLNEST_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(P11S_PERV_PLLNEST)

    ## Miscellaneous TP setup
    mod_poz_tp_init_common(i_target)

def ody_tp_init():
    # TODO : Set up TOD error routing, error mask via scan inits
    # TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    ## Start using PCB network
    ROOT_CTRL0.GLOBAL_EP_RESET = 0        # Drop Global Endpoint reset
    mod_switch_pcbmux(i_target, mux::PCB2PCB)
    ROOT_CTRL0.OOB_MUX = 0

    ## Set up static multicast groups
    mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)

    ## Set up chiplet hang pulses
    mod_hangpulse_setup(MCGROUP_GOOD, 9, {{0, 14, 0}, {1, 1, 0}, {2, 1, 0}, {5, 4, 0}, {6, 5, 0, 1}})

    ## Unmask TP PLL unlock reporting
    if not ATTR_PLL_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(ODY_PERV_PLLMC)

    ## Miscellaneous TP setup
    mod_poz_tp_init_common(i_target)

def zme_tp_init():
    # TODO : Set up TOD error routing, error mask via scan inits
    # TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    ## Start using PCB network
    ROOT_CTRL0.GLOBAL_EP_RESET = 0        # Drop Global Endpoint reset
    mod_switch_pcbmux(i_target, mux::PCB2PCB)
    ROOT_CTRL0.OOB_MUX = 0

    ## Set up static multicast groups
    mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)

    ## Set up chiplet hang pulses
    uint8_t pre_divider = bla;
    mod_hangpulse_setup(MCGROUP_GOOD, pre_divider, {{0, 16, 0}, {1, 1, 0}, {5, 6, 0}, {6, 7, 0, 1}})

    ## Unmask TP PLL unlock reporting
    if not ATTR_FILTER_PLL_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(ZME_PERV_PLLABFLT | ZME_PERV_PLLXBFLT | ZME_PERV_PLLMCFLT)

    if not ATTR_NEST_PLL_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(ZME_PERV_PLLNEST)

    ## Miscellaneous TP setup
    mod_poz_tp_init_common(i_target)

ISTEP(1, 30, "zme_mbus_deskew_slow", "SBE")

ISTEP(1, 31, "zme_mbus_realign", "SBE")

ISTEP(1, 32, "zme_mbus_mesh_setup", "SBE")

ISTEP(1, 33, "zme_mbus_deskew_fast", "SBE")

"""
Step 2: SBE load, Tap TP init
"""

ISTEP(2, 1, "ph_fsi_init", "SPPE")

def sim_p11t_fsi_init():
    # Run this only on tap-only models
    Toggle Tap CFAM_RESET

    # Sim workaround for X states in efuse that can't be cleaned out any other way (EWM 284776)
    wait for 24k cycles
    set OTPROM.SINGLE_OTP_ROM.OTPROM.EFUSE.FIRST_CSB to 1

def p11s_fsi_init():
    # start with list of functional Taps based on PG+deconfig (expected to be provided by FAPI platform code)
    buffer<uint8_t> l_expected_taps = 0;
    for all functional Tap targets:
        l_expected_taps[i] = 1

    ## Take PIB FSI controller out of reset
    PERV_CTRL0.PIB_FSICTL_RESET = 0    # bit 26

    ## Enable Tap reference clocks
    ROOT_CTRL7.TAP_REFCLK_EN = l_expected_taps

    delay(1ms, 1kcyc)

    ## Drop Tap CFAM_RESETs
    ROOT_CTRL6.TAP_CFAM_RESET_N = l_expected_taps

    call fsi_driver_init(i_expected_taps=l_taps, o_detected_taps=l_detected_taps)

    ## Disable failed Taps
    ROOT_CTRL6.TAP_CFAM_RESET_N = l_detected_taps
    delay(1ms, 1kcyc)
    ROOT_CTRL7.TAP_REFCLK_EN = l_detected_taps

    if l_detected_taps != l_expected_taps:
        ASSERT(TAP_FSI_INIT_FAILURE, l_expected_taps, l_detected_taps)

ISTEP(2, 2, "pc_fsi_config", "SPPE")
# Configure Tap root controls and scratch

def p11t_fsi_config():
    ## Disable Write Protection for Root/Perv Control registers
    # CONTROL_WRITE_PROTECT_DISABLE = 0x4453FFFF
    GPWRP = CONTROL_WRITE_PROTECT_DISABLE

    ## Set up Tap clock muxing
    ROOT_CTRL4 = 0 (reset value)
    ROOT_CTRL4.TEST_TCK_SEL = ATTR_CLOCK_MUX1
    ROOT_CTRL4.NEST_GLM_ASYNC_RESET = 0
    ROOT_CTRL4.NEST_DIV2_ASYNC_RESET = 0

    ROOT_CTRL4_COPY = ROOT_CTRL4      # Update copy register to match

ISTEP(2, 3, "pc_cbs_start", "SPPE")

def p11t_cbs_start():
    mod_cbs_start(i_target, false)

ISTEP(2, 4, "pc_tp_chiplet_reset", "SPPE")

def p11t_tp_chiplet_reset():
    ROOT_CTRL0.PCB_RESET = 0           # Drop PCB interface reset to enable access into TP chiplet
    OPCG_ALIGN.OPCG_WAIT_CYCLES = 0x20 # Increase OPCG wait cycles to enable scanning

    # No scan0 needed since the CBS just took care of that

ISTEP(2, 5, "pc_pib_initf", "SPPE")

def p11t_pib_initf():
    putRing( pc_pib_rtg = *_gptr, *_time, *_repr, pib_abst (optional) )

"""
For Tap, the arrayinit start+poll steps are separate to allow for parallel
execution of arrayinit - the SPPE can first go through starting all Taps
before moving on to polling on all Taps in sequence.
"""

ISTEP(2, 6, "pc_tp_arrayinit_start", "SPPE")

def p11t_tp_arrayinit_start():
    mod_abist_start(TP chiplet, regions=pib+perv)

ISTEP(2, 7, "pc_tp_arrayinit_poll", "SPPE")

def p11t_tp_arrayinit_poll():
    mod_abist_poll(TP chiplet)
    compareRing( pc_tp_abist_check )   # skip if image empty or not present

ISTEP(2, 8, "pc_tp_arrayinit_cleanup", "SPPE")

def p11t_tp_arrayinit_cleanup():
    mod_abist_cleanup(TP chiplet)
    mod_scan0(TP chiplet, regions=pib+perv)

ISTEP(2, 9, "pc_tp_lbist", "SPPE")

def p11t_tp_lbist():
    poz_bist(lbist settings for pib+perv+net)

ISTEP(2, 10, "pc_pib_startclocks", "SPPE")

def p11t_pib_startclocks():
    mod_start_stop_clocks(regions=pib)

# 2.11 used to be ph_g2p_init but we dropped G2P acceleration from the plan

ISTEP(2, 12, "proc_sbe_load", "SPPE")

def p11s_sbe_load():
    # SPPE loads SPINAL & TAP SBE common image (FW code, common data)
    # Performs a signature check on the entire block.
    # Loads block into SPINAL SBE PIBMEM
    # Loads block into TAP SBE PIBMEM via G2P
    MODULE( load PPE image( Spinal SBE via PIB, base image ) )
    MODULE( load PPE image( G2P mcast to Taps, base image ) )

ISTEP(2, 13, "proc_sbe_customize", "SPPE")

def p11s_sbe_customize():
    # Same as above but no signature check
    # Data is Tap unique
    MODULE( load PPE image( Spinal SBE via PIB, Spinal cust image ) )
    for tap in taps:
        MODULE( load PPE image( tap, Tap cust image ) )

ISTEP(2, 14, "proc_sbe_config_update", "SPPE")

def p11s_sbe_config_update():
    # Set up Tap SBE scratch regs by copying from Spinal scratch regs
    # Optional: Set up Spinal SBE scratch regs - assumption is that SBE scratch is subset of SPPE scratch so no need to modify

ISTEP(2, 15, "proc_sbe_start", "SPPE")

def p11_sbe_start(vector<Target<COMPUTE_CHIP | HUB_CHIP> & i_targets>):
    ## Boot SBEs on both Spinal and Tap
    for chip in i_targets:
        # Use TP_TPCHIP_PIB_SBE_SBEPM_SBEPPE_PPE_XIXCR_t for SBE_PPE_XIXCR
        SBE_PPE_XIXCR = 0
        # Hard reset SBE
        SBE_PPE_XIXCR.PPE_XIXCR_XCR = XCR_HARD_RESET   # XCR_HARD_RESET = 6
        # write XIXCR

        # Start SBE instructions
        SBE_PPE_XIXCR.PPE_XIXCR_XCR = XCR_START        # XCR_START = 2

    ## Wait for all SBEs to complete boot
    bool all_done = false
    for loop in 1..100:
        delay(1ms, 10kcyc)
        all_done = true
        for chip in i_targets:
            all_done = all_done and SB_MSG2[0]     # SB_MSG2 not defined in headers yet, it's at 5000A
        if all_done:
            break

    fail if not all_done

ISTEP(2, 16, "proc_sbe_attr_setup", "SSBE, TSBE")

def p11s_sbe_attr_setup():
    pass # provided by Thi

def p11t_sbe_attr_setup():
    pass # provided by Thi

ISTEP(2, 17, "pc_pll_initf", "TSBE")

def p11t_pll_initf():
    putRing( pc_pll = perv_pll_cfg ) # perv_pll_cfg for DPLL

ISTEP(2, 18, "pc_pll_setup", "TSBE")

def p11t_pll_setup():
    if ATTR_TAP_DPLL_BYPASS:
        return

    ## Write sector buffer strength
    PERV_CTRL1.SEC_BUF_STRENGTH = ATTR_SECTOR_BUFFER_STRENGTH

    ## Initialize DPLL to mode1, write frequency settings
    DPLL_CTRL = 0
    DPLL_CTRL.DPLL_LOCK_SEL = 1
    DPLL_CTRL.FF_BYPASS = 1
    DPLL_CTRL.FF_SLEWRATE_UP = 1
    DPLL_CTRL.FF_SLEWRATE_DN = 1

    # value = 8 * (desired frequency in MHz) / (refclock frequency in MHz)
    # Add 1 to the desired frequency to compensate for 2033.3333 being represented as 2033 etc.
    # REFCLOCK = 133.333 = 400 / 3
    # ==> value = (ATTR_FREQ_CORE_BOOT_MHZ + 1) * 8 * 3 / 400
    const uint32_t l_multiplier = (ATTR_FREQ_CORE_BOOT_MHZ + 1) * 3 / 50

    DPLL_FREQ = 0
    DPLL_FREQ.FMAX  = l_multiplier
    DPLL_FREQ.FMULT = l_multiplier
    DPLL_FREQ.FMIN  = l_multiplier

    ROOT_CTRL3.TP_DPLL_FUNC_CLKSEL = 0
    # write ROOT_CTRL3
    ROOT_CTRL3.TP_DPLL_RESET = 0

    ## Start PLL clock and check for lock
    mod_start_stop_clock(TP chiplet, P11T_PERV_DPLLNEST)

    if mod_poll_pll_lock_fsi2pib(TP chiplet, pll::P11T_PERV_DPLLNEST) returns TIMEOUT:
        ASSERT(DPLL_LOCK_ERR)

    ## Take DPLL out of bypass and enable frequency slewing
    ROOT_CTRL3.TP_DPLL_TEST_ENABLE = 0
    ROOT_CTRL3.TP_DPLL_BYPASS = 0

    DPLL_CTRL.FF_BYPASS = 0

ISTEP(2, 19, "pc_tp_initf", "TSBE")

def p11t_tp_initf():
    putRing( TP chiplet, perv_initf = perv_func+net_func )

ISTEP(2, 20, "pc_tp_startclocks", "TSBE")

def p11t_tp_startclocks():
    mod_start_stop_clocks(TP chiplet, regions=[perv, net])

ISTEP(2, 21, "pc_tp_init", "TSBE")

def p11t_tp_init():
    # TODO : Set up TOD error routing, error mask via scan inits
    # TODO : Set up perv LFIR, XSTOP_MASK, RECOV_MASK via scan inits

    ## Start using PCB network
    ROOT_CTRL0.GLOBAL_EP_RESET = 0        # Drop Global Endpoint reset
    mod_switch_pcbmux(i_target, mux::PCB2PCB)
    ROOT_CTRL0.OOB_MUX = 0

    ## Set up static multicast groups
    mod_multicast_setup(i_target, MCGROUP_GOOD, 0x7FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TP, 0x3FFFFFFFFFFFFFFF, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_GOOD_NO_TPEQ, 0x3FFFFFFF00000000, TARGET_STATE_FUNCTIONAL)
    mod_multicast_setup(i_target, MCGROUP_EQ, 0x00000000FFFF0000, TARGET_STATE_FUNCTIONAL)

    ## Set up chiplet hang pulses
    mod_hangpulse_setup(MCGROUP_GOOD, 9, {{0, 16, 0}, {5, 6, 0}, {6, 7, 0, 1}})
    mod_hangpulse_setup(N0 chiplet,   9, {{4, 17, 1, 1}})
    mod_hangpulse_setup(MGCGROUP_EQ,  9, {{1, 33, 0, 1}})

    # We cannot set up constant hang pulses here yet since the generation
    # logic is in the TBUS chiplet. The constant hang pulse setup for Tap
    # will happen at the end of p11t_chiplet_startclocks.

    ## Set up special NET_CTRL1 init value for EQs
    with all EQ chiplets via multicast:
        # EQ_NET_CTRL1_INIT_VALUE = 0xFFFF_FF00_0000_0000
        NET_CTRL1 = EQ_NET_CTRL1_INIT_VALUE

    ## Unmask TP PLL unlock reporting
    if not ATTR_TAP_DPLL_BYPASS:
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS &= ~(P11T_PERV_DPLLNEST)

    ## Miscellaneous TP setup
    mod_poz_tp_init_common(i_target)

"""
Step 3: Spinal+Tap chiplet init
"""

ISTEP(3, 1, "proc_chiplet_force_on", "SSBE, TSBE")
# NOT executed as part of hotplug

def p11s_chiplet_force_on():
    pass  # No power gated regions on Spinal

def p11t_chiplet_force_on():
    pass

def zme_chiplet_force_on():
    pass

ISTEP(3, 2, "proc_chiplet_clk_config", "SSBE, TSBE")

def p11s_chiplet_clk_config():

    def paxo_mux_setup():
        NET_CTRL1.REFCLK_CLKMUX_SEL[0] = ATTR_CLOCK_PLL_CROSS_FEED_MUX89    # uint8, OWN_PLL = 0, OTHER_PLL = 1
        NET_CTRL1.REFCLK_CLKMUX_SEL[1] = ATTR_CLOCK_NEST_SYNC_MUX67         # uint8, PLL = 0, NEST = 1
        NET_CTRL1.REFCLK_CLKMUX_SEL[2:3] = ATTR_CLOCK_PLL_INPUT_MUX3031     # uint8, 133M_MUX26 = 0, 100M_MUX27 = 1, 133M_MUX28 = 2

    def pci_mux_setup():
        NET_CTRL1.REFCLK_CLKMUX_SEL[0] = ATTR_CLOCK_PLL_USED_MUX5           # uint8, EVEN_PAXO = 0, ODD_PAXO = 1
        NET_CTRL1.REFCLK_CLKMUX_SEL[1] = ATTR_CLOCK_NEST_SYNC_MUX4          # uint8, PAXO = 0, NEST = 1

    def mux_setup():
        for chiplet in all MC that match i_chiplet_mask:
            NET_CTRL1.REFCLK_CLKMUX_SEL[0] = ATTR_CLOCK_NEST_SYNC_MUX3      # uint8, PLL = 0, NEST = 1

        for chiplet in all PAX+PAXO that match i_chiplet_mask:
            paxo_mux_setup()

        for chiplet in all PEC2P+PEC6P that match l_chiplet_mask:
            pci_mux_setup()

    poz_chiplet_clk_config(i_target, mux_setup)

def p11t_chiplet_clk_config():
    def mux_setup():
        pass  # Nothing to do

    poz_chiplet_clk_config(i_target, mux_setup)

def ody_chiplet_clk_config():
    poz_chiplet_clk_config()

def zme_chiplet_clk_config():
    poz_chiplet_clk_config()

# typedef fapi2::ReturnCode (*chiplet_mux_setup_FP_t)(
#     const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_chip_target,
#     const fapi2::buffer<uint64_t> i_chiplet_mask);

def poz_chiplet_clk_config(target<ANY_POZ_CHIP> i_target, chiplet_mux_setup_FP_t i_mux_setup):
    ## Inhibit PLAT flush on Pervasive chiplet
    # Some pervasive PLATs are needed to facilitate synchronization
    # between chiplets, so make sure they're not flushed
    # (this is important for hotplug and won't hurt in IPL)
    with TP chiplet:
        CPLT_CTRL0.CTRL_CC_FLUSHMODE_INH_DC = 1

    if ATTR_HOTPLUG:
        chiplets = All functional chiplets except TP
    else:
        chiplets = All PRESENT chiplets regardless of functional or not, except TP

    ## Set up chiplet clock muxing
    i_mux_setup(i_target, chiplets converted to chiplet mask)

    with chiplets via multicast:
        ## Enable chiplet clocks
        NET_CTRL0.CLK_ASYNC_RESET = 0
        NET_CTRL0.LVLTRANS_FENCE = 0
        # write NET_CTRL0
        NET_CTRL0.PLLFORCE_OUT_ENABLE = 1

ISTEP(3, 3, "proc_chiplet_deskew", "SSBE")

def p11s_chiplet_deskew():
    # TBD

def zme_chiplet_deskew():
    pass # placeholder

ISTEP(3, 4, "proc_chiplet_reset", "SSBE, TSBE")

def p11s_chiplet_reset():
    poz_chiplet_reset(i_target, p11s_chiplet_delay_table)

def p11t_chiplet_reset():
    poz_chiplet_reset(i_target, p11t_chiplet_delay_table)

    if not ATTR_HOTPLUG:
        p11t_hcd_ex_manual_poweron(all good cores)
        p11t_hcd_cache_reset(all good cores)
        p11t_hcd_core_reset(all good cores)

def ody_chiplet_reset():
    poz_chiplet_reset(i_target, ody_chiplet_delay_table, PRE_SCAN0)

    with MC chiplet:
        # Assert ATPGMODE_PUBMAC while we might scan to work around
        # a problem where a latch controlling async resets is on the scan chain.
        # Setting this mux to 1 will disconnect the async resets from that latch
        # so we can scan without issue.
        CPLT_CONF1.DDR01_ATPGMODE_PUBMAC = 1

    poz_chiplet_reset(i_target, ody_chiplet_delay_table, SCAN0_AND_UP)

def zme_chiplet_reset():
    poz_chiplet_reset(i_target, zme_chiplet_delay_table)

    with Nest chiplet:
        ## Force Nest chiplet out of flush unconditionally
        # This is necessary because some dual clocked arrays in I/O chiplets
        # are partially controlled from the Nest side too, and to be able to
        # ABIST/arrayinit them in the Hotplug case we must ensure that Nest
        # is out of flush.
        CPLT_CTRL0.CTRL_CC_FLUSHMODE_INH_DC = 1
        CPLT_CONF0.SDIS_N = 1

enum poz_chiplet_reset_phases : uint8_t {
    PRE_SCAN0 = 0x80,
    SCAN0_AND_UP = 0x40,
    ALL = 0xFF,
};

def poz_chiplet_reset(target<ANY_POZ_CHIP>, const uint8_t i_chiplet_delays[64], const poz_chiplet_reset_phases i_phases = ALL):
    if ATTR_HOTPLUG:
        chiplets = All functional chiplets except TP
    else:
        chiplets = All PRESENT chiplets regardless of functional or not except TP

    if i_phases & PRE_SCAN0:
        with chiplets via multicast:
            ## Enable and reset chiplets
            NET_CTRL0.PCB_EP_RESET = 1
            # write NET_CTRL0
            NET_CTRL0.PCB_EP_RESET = 0
            NET_CTRL0.CHIPLET_EN = 1

            ## Set up clock controllers
            SYNC_CONFIG = 0
            SYNC_CONFIG.SYNC_PULSE_DELAY = 0b1001    # decimal 9 -> 10 cycles

        ## Set up per-chiplet OPCG delays
        for chiplet in chiplets:
            OPCG_ALIGN = 0
            OPCG_ALIGN.INOP_ALIGN = 7     # 16:1 INOP align
            OPCG_ALIGN.INOP_WAIT  = 0
            OPCG_ALIGN.SCAN_RATIO = 3     # 4:1 scan ratio
            OPCG_ALIGN.OPCG_WAIT_CYCLES = 0x30 - 4 * i_chiplet_delays[chiplet.getChipletNumber()]

    if i_phases & SCAN0_AND_UP:
        ## Scan-zero
        # NOTE ignore errors on non-functional chiplets
        mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_RTG)
        mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

        ## Transfer partial good attributes into region PGOOD and PSCOM enable registers
        for chiplet in chiplets:
            CPLT_CTRL2 = 0
            CPLT_CTRL2[bits 4:18] = ~ATTR_PG[bits 4:18]

            CPLT_CTRL3 = CPLT_CTRL2

ISTEP(3, 5, "proc_chiplet_unused_psave", "SSBE, TSBE")
# NOT executed as part of hotplug

def p11t_chiplet_unused_psave():
    poz_chiplet_unused_psave()

def p11s_chiplet_unused_psave():
    poz_chiplet_unused_psave()

def ody_chiplet_unused_psave():
    poz_chiplet_unused_psave()

def zme_chiplet_unused_psave():
    poz_chiplet_unused_psave()

def poz_chiplet_unused_psave():
    # now put all non-functional chiplets into a state of minimal power usage
    for chiplet in all PRESENT chiplets:
        if chiplet is not functional:
            NET_CTRL0.CHIPLET_ENABLED = 0
            NET_CTRL0.PCB_EP_RESET = 1
            NET_CTRL0.CHIPLET_FENCE_PCB = 1
            # write NET_CTRL0
            NET_CTRL0.PLLFORCE_OUT_ENABLE = 0
            NET_CTRL0.CLK_ASYNC_RESET = 1
            NET_CTRL0.LVLTRANS_FENCE = 1

ISTEP(3, 6, "proc_chiplet_pll_initf", "SSBE, TSBE")

def p11s_chiplet_pll_initf():
    putRing( chiplet_pll = all static PLL settings if any )

    # Load bucketed rings based on bucket attrs
    # Default to max freq during boot to catch errors early

    for all PAX:
        putRingBucket(pax_pll_XX, ATTR_PAXO_PLL_BUCKET[i])

    for all PAXO:
        putRingBucket(paxo_pll_XX, ATTR_PAXO_PLL_BUCKET[i+8])

    for all PEC2P:
        putRingBucket(pec2p_pll_XX, ATTR_TBD) # TODO Joachim

    for all PEC6P:
        putRingBucket(pec6p_pll_XX, ATTR_TBD) # TODO Joachim

    for all MC:
        putRingBucket(mc_pll_XX, ATTR_OMI_PLL_BUCKET[i])

    # Single setting for all TBUS, can be scanned via the chip
    # NEST_LCPLL_BUCKET is correct here, we use that same setting for the TBUS buckets
    putRingBucket(tbus_pll_XX, ATTR_NEST_LCPLL_BUCKET)

def p11t_chiplet_pll_initf():
    putRing( chiplet_pll_common = all static PLL settings if any )

    # Load bucketed rings based on bucket attrs
    # Default to max freq during boot to catch errors early
    # NEST_LCPLL_BUCKET is correct here, we use that same setting for the TBUS buckets
    putRingBucket(tbus_pll_XX, ATTR_NEST_LCPLL_BUCKET)

def zme_chiplet_pll_initf():
    putRing( chiplet_pll_common = all static PLL settings if any )
    # Load bucketed rings based on bucket attrs
    # Default to max freq during boot to catch errors early
    putRing( xx_pll_n ) # TODO Daniel
    ....

ISTEP(3, 7, "proc_chiplet_pll_setup", "SSBE, TSBE")

def p11t_chiplet_pll_setup():
    poz_chiplet_pll_setup()

def p11s_chiplet_pll_setup():
    poz_chiplet_pll_setup()

# No ody_chiplet_pll_setup - The single PLL is started in step 1 by the command table

def zme_chiplet_pll_setup():
    with all enabled PCI chiplets via multicast:
        ## Start PCI filter PLLs
        NET_CTRL0.DANIELS_MAGIC_FPLL_TEST_ENABLE = 0
        # write NET_CTRL0
        NET_CTRL0.DANIELS_MAGIC_FPLL_RESET = 0
        # write NET_CTRL0

        ## Check for PLL lock
        mod_poll_pll_lock(chiplets, pll::PCI_FILTER_PLL)

        ## Relase PLL bypass
        NET_CTRL0.DANIELS_MAGIC_FPLL_BYPASS = 0

    poz_chiplet_pll_setup()

def poz_chiplet_pll_setup():
    if ATTR_IO_TANK_PLL_BYPASS:
        return

    CPLT_CTRL1.REGION13_FENCE = 0    # Drop PLL region fences

    with all chiplets except TP via multicast:
        ## Start chiplet PLLs
        NET_CTRL0.PLL_TEST_ENABLE = 0
        # write NET_CTRL0
        NET_CTRL0.PLL_RESET = 0
        # write NET_CTRL0

        ## Check for PLL lock
        mod_poll_pll_lock(chiplets, pll::ALL_PLLS)

        ## Relase PLL bypass
        NET_CTRL0.PLL_BYPASS = 0

    ## Enable PLL unlock error reporting
    for chiplet in all chiplets except TP:
        # gotta do this via RMW because there's no write-clear address
        PCB_RESPONDER_CONFIG_REG.CFG_MASK_PLL_ERRS = 0

"""
---------- IF user elects to skip BIST, skip from here -----------------------------------------------------------------
"""

ISTEP(3, 8, "proc_bist_repr_initf", "SSBE, TSBE")

def p11s_bist_repr_initf():
    if ATTR_ENABLE_LBIST or ATTR_ENABLE_ABIST:
        p11s_chiplet_repr_initf()

def p11t_bist_repr_initf():
    if ATTR_ENABLE_LBIST or ATTR_ENABLE_ABIST:
        p11t_chiplet_repr_initf()

def ody_bist_repr_initf():
    if ATTR_ENABLE_LBIST or ATTR_ENABLE_ABIST:
        ody_chiplet_repr_initf()

def zme_bist_repr_initf():
    if ATTR_ENABLE_LBIST or ATTR_ENABLE_ABIST:
        zme_chiplet_repr_initf()

ISTEP(3, 9, "proc_abist", "SSBE, TSBE")

def p11s_abist():
    if not ATTR_ENABLE_ABIST:
        return

    poz_bist({"s_abst_setup", "s_abst_cmp", chiplets=all, abist+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def p11t_abist():
    if not ATTR_ENABLE_ABIST:
        return

    poz_bist({"t_abst_setup", "t_abst_cmp", chiplets=all, abist+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def ody_abist():
    if not ATTR_ENABLE_ABIST:
        return

    poz_bist({"cpl_abst_setup", "cpl_abst_cmp", chiplets=all, abist+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def zme_abist():
    if not ATTR_ENABLE_ABIST:
        return

    poz_bist({"cpl_abst_setup", "cpl_abst_cmp", chiplets=all, abist+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

ISTEP(3, 10, "proc_lbist", "SSBE, TSBE")

def p11s_lbist():
    if not ATTR_ENABLE_LBIST:
        return

    poz_bist({"s_lbst_setup", "s_lbst_cmp", chiplets=all, lbist+arrayinit+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_RTG)
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def p11t_lbist():
    if not ATTR_ENABLE_LBIST:
        return

    poz_bist({"t_lbst_setup", "t_lbst_cmp", chiplets=all, lbist+arrayinit+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_RTG)
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def ody_lbist():
    if not ATTR_ENABLE_LBIST:
        return

    poz_bist({"cpl_lbst_setup", "cpl_lbst_cmp", chiplets=all, lbist+arrayinit+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_RTG)
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

def zme_lbist():
    if not ATTR_ENABLE_LBIST:
        return

    poz_bist({"cpl_lbst_setup", "cpl_lbst_cmp", chiplets=all, lbist+arrayinit+setup+run+compare+cleanup, regions=all})
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_RTG)
    mod_scan0(chiplets via multicast, regions=all, scan_types=cc::SCAN_TYPE_NOT_RTG)

"""
---------- IF user elects to skip BIST, skip until here -----------------------------------------------------------------
"""

ISTEP(3, 11, "proc_chiplet_repr_initf", "SSBE, TSBE")

def p11s_chiplet_repr_initf():
    poz_chiplet_repr_initf()

def p11t_chiplet_repr_initf():
    poz_chiplet_repr_initf()

def ody_chiplet_repr_initf():
    poz_chiplet_repr_initf()

def poz_chiplet_repr_initf():
    putRing( chiplet_rtg = *_gptr+*_time+*_repr for all chiplets except TP)

ISTEP(3, 12, "proc_chiplet_arrayinit", "SSBE, TSBE")

def p11s_chiplet_arrayinit():
    poz_chiplet_arrayinit()

def p11t_chiplet_arrayinit():
    poz_chiplet_arrayinit()

def ody_chiplet_arrayinit():
    poz_chiplet_arrayinit()

def poz_chiplet_arrayinit():
    # Note that we're including the "PLL" regions in arrayinit too:
    # Clocking them will not cause any damage, and this way we don't
    # have to special-case EQs since they have MMAs on region 13.
    mod_abist_start(all chiplets except TP, regions=all)
    mod_abist_poll()
    mod_abist_cleanup()
    mod_scan0(all chiplets except TP, regions=all)

ISTEP(3, 13, "proc_chiplet_undo_force_on", "SSBE, TSBE")
# NOT executed as part of hotplug

def p11s_chiplet_undo_force_on():
    pass

def p11t_chiplet_undo_force_on():
    p11t_hcd_ex_manual_poweroff(all good cores)

ISTEP(3, 14, "proc_chiplet_initf", "SSBE, TSBE")

def p11s_chiplet_initf():
    putRing( chiplet_initf = everything under the sun that has not been scanned yet )

def p11t_chiplet_initf():
    putRing( chiplet_initf = everything under the sun that has not been scanned yet )

def ody_chiplet_initf():
    putRing( chiplet_initf = everything under the sun that has not been scanned yet )

ISTEP(3, 15, "proc_chiplet_init", "SSBE, TSBE")

def p11s_chiplet_init():
    ## Set up topology ID
    CPLT_CONF0.TOPOLOGY_MODE = ATTR_FABRIC_TOPOLOGY_MODE
    CPLT_CONF0.TOPOLOGY_ID   = ATTR_FABRIC_TOPOLOGY_ID

def p11t_chiplet_init():
    ## Set up topology and octant ID
    CPLT_CONF0.TOPOLOGY_MODE = ATTR_FABRIC_TOPOLOGY_MODE
    CPLT_CONF0.TOPOLOGY_ID   = ATTR_FABRIC_TOPOLOGY_ID
    CPLT_CONF0.TC_OCTANT_ID_DC     = ATTR_POS

def ody_chiplet_init():
    with MC chiplet:
        ## Program DDR PHY Nto1 clock division ratios
        CPLT_CONF1[24:29] = all 1s

        ## Force MC ATPG regions disabled despite ATTR_PG settings
        CPLT_CTRL2[bit 11..16] = 0

def zme_chiplet_init():
    if not ATTR_HOTPLUG:
        with Nest chiplet:
            CPLT_CONF0.TCNEST_FBC_XBUS0_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[0]   # uint8_t[6]
            CPLT_CONF0.TCNEST_FBC_XBUS0_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[0]  # uint8_t[6]
            CPLT_CONF0.TCNEST_FBC_XBUS1_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[1]
            CPLT_CONF0.TCNEST_FBC_XBUS1_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[1]
            CPLT_CONF0.TCNEST_FBC_XBUS2_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[2]
            CPLT_CONF0.TCNEST_FBC_XBUS2_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[2]
            CPLT_CONF0.TCNEST_FBC_XBUS3_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[3]
            CPLT_CONF0.TCNEST_FBC_XBUS3_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[3]
            CPLT_CONF0.TCNEST_FBC_XBUS4_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[4]
            CPLT_CONF0.TCNEST_FBC_XBUS4_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[4]
            CPLT_CONF0.TCNEST_FBC_XBUS5_DCM_ID_DC  = ATTR_FBC_XBUS_DCM_ID[5]
            CPLT_CONF0.TCNEST_FBC_XBUS5_CHIP_ID_DC = ATTR_FBC_XBUS_CHIP_ID[5]

            CPLT_CONF1.TCNEST_FBC_TM_DCM_ID_DC = ATTR_FBC_TM_DCM_ID  # uint8_t

            CPLT_CONF1.TC_FBC_DWR_ID = ATTR_FBC_DWR_ID    # uint8_t
            CPLT_CONF1.TC_FBC_DCM_ID = ATTR_FBC_DCM_ID    # uint8_t
            CPLT_CONF1.TC_FBC_CHIP_ID = ATTR_FBC_CHIP_ID  # uint8_t

            CPLT_CONF1.TCNEST_FBC_XBUS0_XBUS1_DWR_ID_DC = ATTR_FBC_XBUS_DWR_ID[0]  # uint8_t[3]
            CPLT_CONF1.TCNEST_FBC_XBUS2_XBUS3_DWR_ID_DC = ATTR_FBC_XBUS_DWR_ID[1]
            CPLT_CONF1.TCNEST_FBC_XBUS4_XBUS5_DWR_ID_DC = ATTR_FBC_XBUS_DWR_ID[2]

            CPLT_CONF1.TCNEST_FBC_MBUS_ABUS_DWR_ID_DC = ATTR_FBC_MABUS_DWR_ID      # uint8_t

ISTEP(3, 20, "proc_chiplet_startclocks", "SSBE, TSBE")

def p11s_chiplet_startclocks():
    ## Start chiplet clocks
    poz_chiplet_startclocks(MCGROUP_GOOD_NO_TP)

def p11t_chiplet_startclocks():
    ## Starting chiplet clocks (except EQ)
    poz_chiplet_startclocks(MCGROUP_GOOD_NO_TPEQ)

    ## Starting EQ chiplet clocks
    poz_chiplet_startclocks(MCGROUP_EQ, cc::P11T_EQ_PERV | cc::P11T_EQ_QME | cc::P11T_EQ_CLKADJ)

    ## Set up constant hang pulses
    # Formula:
    #   final period = (1 / mesh freq) * (pre divider + 1) * 2^(hang pulse value - 1) * post divider configured on c_hang_pulse_edge_detect
    # Values:
    #   Mesh at 2400 MHz
    #   HP0 at 33.33ns
    #   HP2 at 4ns / 250 MHz
    # Calculation (assuming hang pulse value == 1):
    #   pre divider = (period in ns * 2.4 / post divider) - 1
    #   HP0 -> (33.33 * 2.4 / 2) - 1 = 39 (or pre-div 9 and hp value 3 as on the other chips)
    #   HP2 -> (4 * 2.4 / 1) - 1 = 8.6, round up to 9, yielding 4.16667 ns or 240 MHz
    mod_constant_hangpulse_setup(i_target, scomt::tbusl::HANGP_HANG_PULSE_CONFIG_REG, {{9, 3, 0}, {0, 0, 0}, {9, 1, 0}, {0, 0, 0}})

def ody_chiplet_startclocks():
    ## Drop TP chiplet fence
    PERV_CTRL0.TC_PERV_CHIPLET_FENCE_DC = 0    # new field - bit 17

    with MC chiplet:
        # Deassert ATPGMODE_PUBMAC (asserted in chiplet_reset)
        # before we go through the DDR PHY reset.
        CPLT_CONF1.DDR01_ATPGMODE_PUBMAC = 0

        ## Reset DDR PHY
        # Assert DDR PHY resets
        CPLT_CONF1.DDR0_PWROKIN = 0
        CPLT_CONF1.DDR1_PWROKIN = 0
        CPLT_CONF1.DDR0_RESET = 1
        CPLT_CONF1.DDR1_RESET = 1
        CPLT_CONF1.DDR0_DFICLKRESET = 1
        CPLT_CONF1.DDR1_DFICLKRESET = 1
        CPLT_CONF1.DDR0_APBRESETN = 0
        CPLT_CONF1.DDR1_APBRESETN = 0

        # Drop fences between hard and soft macro
        # Synopsys specs likely did not anticipate a fence running through the middle of
        # their PHY so we should drop the fence before taking the PHY through reset.
        CPLT_CTRL1.REGION13_FENCE = 0
        CPLT_CTRL1.REGION14_FENCE = 0

        # Align chiplet since we're about to start some clocks
        SYNC_CONFIG.LISTEN_TO_SYNC_DIS = 1
        mod_align_regions(MC chiplet, REGION_ALL)

        # Start PHY clocks to make the functional reset propagate.
        # The PHY won't be fully cleaned up at this point so we don't drop fences just yet.
        mod_start_stop_clocks(MC chiplet, [pub0, pub1, prim0, prim1], i_manage_fences=false)

        # Assert DDR PHY PWROKIN to complete PHY reset sequence
        CPLT_CONF1.DDR0_PWROKIN = 1   # bit 6
        CPLT_CONF1.DDR1_PWROKIN = 1   # bit 7

        # Give the PHY some time to reset - 64 clock cycles ain't much but let's be safe
        delay(1us, 1kcyc)

        # At this point the DDR PHY is in reset state; leave it to the memory code to take it out of there

    ## Start remaining clocks (and drop region fences)
    poz_chiplet_startclocks(MC chiplet, [perv, io, core, cfg, dfi, pub0, pub1, prim0, prim1])

def zme_chiplet_startclocks():
    ## Start chiplet clocks
    poz_chiplet_startclocks(MCGROUP_GOOD_NO_TP)

    with Nest chiplet:
        ## Put Nest chiplet back into flush
        # Undo the hack from chiplet_reset
        CPLT_CTRL0.CTRL_CC_FLUSHMODE_INH_DC = 0
        CPLT_CONF0.SDIS_N = 0

def poz_chiplet_startclocks(target<ANY_POZ_CHIP>, target<PERV|MC>, uint16_t i_clock_regions=cc::REGION_ALL_BUT_PLL):
    ## Drop TP chiplet fence
    PERV_CTRL0.TC_PERV_CHIPLET_FENCE_DC = 0    # new field - bit 17

    NET_CTRL0.PERV2CHIPLET_CHIPLET_FENCE = 0    # bit 11

    ## Switch ABIST and sync clock muxes to functional state
    CPLT_CTRL0.ABSTCLK_MUXSEL = 0
    CPLT_CTRL0.SYNCCLK_MUXSEL = 0

    ## Disable listen to sync
    # Use read-compare here to make sure all clock controllers are set up the same coming in
    SYNC_CONFIG.LISTEN_TO_SYNC_DIS = 1

    ## Align chiplets
    mod_align_regions(i_target, i_clock_regions)

    ## Drop chiplet fence
    # Drop fences before starting clocks because fences are DC and might glitch
    NET_CTRL0.PERV2FSI_CHIPLET_FENCE = 0

    ## Start chiplet clocks
    mod_start_stop_clocks(i_target, i_clock_regions)

    ## Put PLATs into flush mode
    CPLT_CTRL0.CTRL_CC_FLUSHMODE_INH_DC = 0

    ## Put Pervasive chiplet PLATs into flush mode
    # We have to inhibit Pervasive flush until now because some Pervasive PLATs
    # are used for the sync mechanism.
    with TP chiplet:
        CPLT_CTRL0.CTRL_CC_FLUSHMODE_INH_DC = 0  # Allow chiplet PLATs to enter flush


ISTEP(3, 21, "proc_chiplet_fir_init", "SSBE, TSBE")

def p11s_chiplet_fir_init():
    mod_setup_clockstop_on_xstop(MCGROUP_GOOD_NO_TP, p11s_chiplet_delay_table)

def p11t_chiplet_fir_init():
    mod_setup_clockstop_on_xstop(MCGROUP_GOOD_NO_TP, p11t_chiplet_delay_table)

def ody_chiplet_fir_init():
    mod_setup_clockstop_on_xstop(MCGROUP_GOOD_NO_TP, ody_chiplet_delay_table)

def zme_chiplet_fir_init():
    mod_setup_clockstop_on_xstop(MCGROUP_GOOD_NO_TP, zme_chiplet_delay_table)

ISTEP(3, 22, "proc_chiplet_dts_init", "SSBE, TSBE")

def p11s_chiplet_dts_init():
    poz_chiplet_dts_init(target, AN_CHIP_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL)

def p11t_chiplet_dts_init():
    poz_chiplet_dts_init(target, ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL)

def ody_chiplet_dts_init():
    pass

def zme_chiplet_dts_init():
    poz_chiplet_dts_init(target, ???)   # TODO tbd

def poz_chiplet_dts_init(target<ANY_POZ_CHIP> i_target, uint32_t i_start_cal_address):
    # DTS calibration is scanned in via the repr ring, but we can use this to calibrate KVREF
    ## Calibrating voltage reference
    KVREF_START_CAL = 1ULL << 63   # use getScom(i_start_cal_address) here
    delay(10us, 1000cyc)   # Give KVREF some time to get the message and drop CAL_DONE

    try 100 times:
        if KVREF_CAL_DONE:   # use getScom(i_start_cal_address + 4) here
            break
        delay(100us, 1000cyc)
    else:
        ASSERT(KVREF_CAL_NOT_DONE_ERR)

    KVREF_START_CAL = 0

ISTEP(3, 23, "proc_chiplet_skewadj_setup", "TSBE")

def p11t_chiplet_skewadj_setup():
    with all EQs via multicast:
        ## Drop SkewAdj / DCAdj resets
        NET_CTRL1[16:23] = 0

    with all cores on all EQs via multicast:
        ## Initialize Skew Adjust
        # Put SKADJ into INIT state
        SKEW_WRAP_SKEWADJ_SET_INIT_MODE = 0
        # Put SKADJ into HOLD state
        SKEW_WRAP_SKEWADJ_SET_HOLD_MODE = 0
        # Set up wait cycles between adjust and next measurement
        SKEW_WRAP_SKEWADJ_SET_WAIT_CNT = 0
        SKEW_WRAP_SKEWADJ_SET_WAIT_CNT.WAIT_CNT_VALUE = SKEWADJ_WAIT_CNT       # 8
        # Set up number of sensor toggles to ignore
        SKEW_WRAP_SKEWADJ_SET_IGNORE_CNT = 0
        SKEW_WRAP_SKEWADJ_SET_IGNORE_CNT.IGNORE_CNT_VALUE = SKEWADJ_IGNORE_CNT # 2
        # Set up core PDLY override
        SKEW_WRAP_SKEWADJ_CORE_OVERRIDE = 0
        SKEW_WRAP_SKEWADJ_CORE_OVERRIDE.CORE_OVR_ENABLE = ATTR_EQ_SKEWADJ_CORE_PDLY_OVERRIDE[0]
        SKEW_WRAP_SKEWADJ_CORE_OVERRIDE.CORE_OVR_VALUE  = ATTR_EQ_SKEWADJ_CORE_PDLY_OVERRIDE[12:15]
        # Set up cache PDLY override
        SKEW_WRAP_SKEWADJ_CACHE_OVERRIDE = 0
        SKEW_WRAP_SKEWADJ_CACHE_OVERRIDE.CACHE_OVR_ENABLE = ATTR_EQ_SKEWADJ_CACHE_PDLY_OVERRIDE[0]
        SKEW_WRAP_SKEWADJ_CACHE_OVERRIDE.CACHE_OVR_VALUE  = ATTR_EQ_SKEWADJ_CACHE_PDLY_OVERRIDE[12:15]

        ## Initialize DC Adjust
        # Put DCAdj into INIT state
        DCADJ_WRAP_SET_INIT_MODE = 0
        # Put DCAdj into HOLD state
        DCADJ_WRAP_SET_HOLD_MODE = 0
        # Set up DCADJ configuration:  - Invert adjustment direction
        DCADJ_WRAP_SET_CONFIG = 0
        DCADJ_WRAP_SET_CONFIG.ADJUSTMENT_DIR = 1
        # Set up low pass filter delay
        DCADJ_WRAP_SET_LOW_PASS_DLY = 0
        DCADJ_WRAP_SET_LOW_PASS_DLY.LOW_PASS_DELAY_VALUE = DCADJ_LOW_PASS_DELAY  # 8192
        # Set up DAC settlement delay
        DCADJ_WRAP_SET_DAC_DLY = 0
        DCADJ_WRAP_SET_DAC_DLY.DAC_DELAY_VALUE = DCADJ_DAC_DELAY                 # 256
        # Set up comparator settlement delay
        DCADJ_WRAP_SET_COMP_DLY = 0
        DCADJ_WRAP_SET_COMP_DLY.COMP_DELAY_VALUE = DCADJ_COMP_DELAY              # 64
        # Set up DCC override
        DCADJ_WRAP_DCC_OVERRIDE = 0
        DCADJ_WRAP_DCC_OVERRIDE.OVR_ENABLE = ATTR_EQ_DCADJ_DCC_OVERRIDE[0]
        DCADJ_WRAP_DCC_OVERRIDE.OVR_VALUE  = ATTR_EQ_DCADJ_DCC_OVERRIDE[8:15]
        # Set up duty cycle target
        const int8_t target_override = ATTR_EQ_DCADJ_TARGET_OVERRIDE[0] ? ATTR_EQ_DCADJ_TARGET_OVERRIDE[8:15] : 0;
        DCADJ_WRAP_SET_DCC_TARGET = 0
        DCADJ_WRAP_SET_DCC_TARGET.TARGET_VALUE = DCADJ_DEFAULT_TARGET + target_override   # DCADJ_DEFAULT_TARGET = 64

    ## Start adjusting
    # Drop SkewAdj and DCAdj bypass
    with all EQs via multicast:
        if ATTR_EQ_DCADJ_BYPASS == 0:
            NET_CTRL1[0:7] = 0
        if ATTR_EQ_SKEWADJ_BYPASS == 0:
            NET_CTRL1[8:15] = 0

    # Start Adjust logic
    with all cores on all EQs via multicast:
        if ATTR_EQ_DCADJ_BYPASS == 0 and not ATTR_EQ_DCADJ_DCC_OVERRIDE[0]:
            DCADJ_WRAP_SET_ADJUST_MODE = 0
        if ATTR_EQ_SKEWADJ_BYPASS == 0 and not ATTR_EQ_SKEWADJ_CORE_PDLY_OVERRIDE[0]:
            SKEW_WRAP_SKEWADJ_SET_ADJUST_MODE = 0

ISTEP(3, 24, "proc_nest_enable_io", "SSBE, TSBE")

def p11s_nest_enable_io():
    poz_nest_enable_io()

def p11t_nest_enable_io():
    poz_nest_enable_io()

def ody_nest_enable_io():
    for chiplet in TARGET_FILTER_MC:
        NET_CTRL0.CPLT_DCTRL = 1
        NET_CTRL0.CPLT_RCTRL = 1
        NET_CTRL0.CPLT_RCTRL2 = 1

def zme_nest_enable_io():
    poz_nest_enable_io()

def poz_nest_enable_io():
    ROOT_CTRL1.GLOBAL_PERST_OVERRIDE = 0

    for chiplet in TARGET_FILTER_NEST:
        NET_CTRL0.DCTRL = 1
        NET_CTRL0.RCTRL = 1
        NET_CTRL0.RCTRL2 = 1

ISTEP(3, 25, "proc_chiplet_scominit", "SSBE, TSBE")

# Generated code

ISTEP(3, 28, "proc_mbus_iobist", "SPPE")

def zme_mbus_iobist():
    pass  # TODO Daniel

ISTEP(3, 29, "proc_mbus_calib", "SPPE")

def zme_mbus_calib():
    pass  # TODO Daniel

ISTEP(3, 30, "proc_ioppe_load", "SPPE")
# SPPE fetches IOPPE code & verify
#  Sub-step 1: load all PAXO IOPPEs (SPINAL only)
#  Sub-step 2: load TBUS IOPPEs
#    multicast to TAP to IOPPE via G2P
#    multicast/unicast on SPINAL to all used IOPPEs (TBUS)
# Dynamic reload: only needed for Spinal IOPPEs, which have a fast load path
# On IO chiplet reboot, IOPPE SRAM will have remaining state from previous run and may have to be overwritten with a fresh image.

def ody_ioppe_load():
    Load IOPPE and boot it
    Load ARC but do not boot it

def zme_ioppe_load():
    pass

### ZME AUTOBOOT IS DONE HERE, hand off to SE

ISTEP(3, 32, "proc_tbus_setup", "SSBE, TSBE")
# TODO Chris: put your steps here

ISTEP(3, 40, "proc_fabricinit", "SSBE, TSBE")

def p11s_fabricinit():
    pass # Leaving the implementation details to the Nest team

def p11t_fabricinit():
    pass # Leaving the implementation details to the Nest team

ISTEP(3, 41, "proc_bmc_pci_init", "SSBE")
# TODO Chris: put your steps here

ISTEP(3, 50, "proc_select_boot_cores", "SPPE")

ISTEP(3, 99, "stopclocks", "Cronus only, NOT part of IPL")
"""
Stopclocks traditionally means a lot more than "literally just stop the clocks" - it means "get the chip ready for scanning".
At the very least this means raising fences in addition to stopping clocks.

It usually also entails quiescing buses and dropping iovalids prior to stopping the clocks to minimize the disruption to dependent logic.
This also means Power Management should be disabled before we go on to clockstop anything on the chip.

This HWP must be bullet proof - if some operations fail it should press on and try to continue stopping clocks.

Cronus note: Stopping "all" means stop Taps, then stop Spinals
"""
def poz_stopclocks():
    # Escalation levels:
    # 1. Stop cores/caches (only on Tap)
    # 2. Stop chiplets (including TBUS)
    # 3. Stop Pervasive (perv, net, occ) - switch to PIB2PCB path
    # 4. Stop SBE (pib, sbe) - switch to FSI2PCB path

    if not CBS_ENVSTAT.CBS_ENVSTAT_C4_TEST_ENABLE:
        Close down all drivers/receivers to prevent chip IOs from toggling during scan

    if Odyssey:
        with MC chiplet:
            # Assert ATPGMODE_PUBMAC while we might scan to work around
            # a problem where a latch controlling async resets is on the scan chain.
            # Setting this mux to 1 will disconnect the async resets from that latch
            # so we can scan without issue.
            CPLT_CONF1.DDR01_ATPGMODE_PUBMAC = 1
