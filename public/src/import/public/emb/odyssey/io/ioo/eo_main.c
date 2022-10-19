/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_main.c $       */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_main.c
// *! TITLE       :
// *! DESCRIPTION : Call the Eyeopt steps
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr22100401 |vbr     | Issue 291616: Skip init steps on a pipe_abort; skip vga_poff in recal if vga had error.
// vbr22100400 |vbr     | Issue 291616: Adjust pipe_abort sources for dccal/init vs recal
// mbs22082601 |mbs     | Updated with PSL comments
// mwh22083000 |mwh     | Putting back in gating logic for rx_sigdetbist_test when AXO is run issue 288779
// vbr22071200 |vbr     | Moved some repeated code for running tx dcc into a common function
// vbr22071100 |vbr     | Moved eloff_alias_live_data_vote write from here to eo_eoff
// mwh22071200 |mwh     | Add code back in to run rx_eo_dfe_full for abank
// gap22062200 |gap     | Added call to clock_change_tx to per-gen pcie dcc calls
// mwh22062200 |mwh     | Removing code for rx_eo_dfe_full for abank -- need go back in when story 283492 is closed.
// vbr22061500 |vbr     | Add external command fail reporting
// vbr22060800 |vbr     | Move the CDR disable to as soon after the alt bank power up in INIT as possible; re-enable psave waiting on cdr lock after INIT on AXO.
// vbr22060100 |vbr     | Power down ALT bank at the end of DC Cal
// vbr22052600 |vbr     | No longer wait for CDR lock when first enable in initial training (AXO, PCIe Gen3-5)
// mwh22072100 |mwh     | Gating rx_sigdetbist_test with pcie mode since should not run in 7nm
// mwh22062900 |mwh     | add in bool to eo_eoff_1_lat too turn of vote on tran if need
// mwh22060600 |mwh     | add in rx_eo_dfe_full for abank for bist only run issue 282252 -- do not need 2 recals
// vbr22041400 |vbr     | Issue 278628: Do not increment recal count on an abort
// vbr22032201 |vbr     | Run the same steps in Init for PCIe Gen1 and Gen2; some optimization of step decodes
// vbr22032200 |vbr     | Run bank align on the ALT bank in Init to be consistent with recal
// jfg22032100 |jfg     | Replaced !pcie_mode conditional with pcie_mode!=1
// jfg22031100 |jfg     | Add PPE_INIT_CAL_TIME_US_BUDGET_* based conditional for setting ppe_init_time_us_warning
// vbr22031700 |vbr     | Run all 4 DFE Full quadrants when peak changes
// vbr22031100 |vbr     | Updated when hysteresis is enabled on DFE Full and CTLE
// vbr22020900 |vbr     | Issue 254482: no longer call eo_main_recal 3 consecutive times on first recal
// vbr22022300 |vbr     | QPA is AXO only in INIT and AXO/Gen4/Gen5 in Recal.
// jjb22011200 |jjb     | Addedd rx_sigdet_bist to eo_main_dccal_rx().
// vbr21120200 |vbr     | Issue 259400: Move bank sync to after B Bank steps and force a wait in AXO and Gen1
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21110200 |vbr     | Issue 256140: re-enable lock detector before DDC since it checks for lock
// vbr21110100 |vbr     | Ensure that wait long enough for bank sync
// jjb21101900 |jjb     | restored static to eo_main_recal_rx as no longer used by pipe code.
// vbr21102500 |vbr     | Remove LTE; EOFF only run on Bank B in initcal for AXO.
// vbr21101900 |vbr     | Added running of TX DCC at 3 rates and storing of results
// jjb21011500 |jjb     | removed static from eo_main_recal_rx to enable function access by pcie rx margining
// vbr21101200 |vbr     | Issue 254483: Do not set min_recal_cnt_reached on an abort so recal can continue as expected afterwards
// vbr21093000 |vbr     | Updates for new threading method
// vbr21092200 |vbr     | Various optimizations related to GCR accesses
// vbr21090800 |vbr     | Updated ctle peaking function calls and removed peaking cal on B in INIT.
// vbr21091500 |vbr     | Split pr_offset_applied into a/b (Issue 250901)
// vbr21082500 |vbr     | Added sleep to help thread active time in cases where no steps are enabled
// mwh21008160 |mwh     | add epoff check  - called eo_rxbist_epoff_final-- cover all 3 diff methods
// mwh21008110 |mwh     | To support removing of gcr  rx_a/b_lane_fail_0_15_16_23 and moving to using rx_lane_fail_0_15,16_23
// mwh21007090 |mwh     | Issue (243388-abort)replaced vga_loop with first_recal -- vga_loop not need anymore
// vbr21062400 |vbr     | Removed ALT bank remaining powered on after PCIe INIT
// vbr21061800 |vbr     | Add safely switching to bank A for init
// vbr21061700 |vbr     | Split abort_code into 2 variants
// vbr21061000 |vbr     | Moved clearing of recal_abort_sticky to eo_wrappers
// mwh21060700 |mwh     | Up date code to handle hyst
// vbr21060300 |vbr     | Removed some sleeps to improve cal time
// vbr21060200 |vbr     | Minor cleanup of poff on bank B in init. Moved servo queue check to once at beginning of training.
// vbr21052600 |vbr     | Fixed reading of pipe_rate
// vbr21052700 |vbr     | Rename sl*ve_mode to external_mode
// vbr21060300 |vbr     | Added additional sleeps for checking psave in dc cal
// vbr21060200 |vbr     | Issue 241008: Added sleep after DDC in recal
// mwh21052700 |mwh     | Up date code to match tables for pathoffset.
// mwh21052000 |mwh     | Add code for pathoff vga
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21052100 |vbr     | Updated Recal steps based on PCIe Gen
// vbr21050400 |vbr     | Add calls to eo_poff
// vbr21040700 |vbr     | New inputs for DFE, CTLE
// vbr21040100 |vbr     | Implement P11 training algorithm for initial training. Added extra sleeps.
// jfg21040502 |jfg     | EWM228237 Also remove it from recal loops.
// jfg21040500 |jfg     | EWM228237 Remove first_iteration setting for bank-b CTLE catchup at the end of main_init
// vbr21031700 |vbr     | Updated writes to CDR controls
// vbr21020500 |vbr     | Determine cal_mode in eo_main, moved macros to header
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr21012500 |vbr     | Set min_recal_cnt_reached on a recal abort
// vbr21012100 |vbr     | Split number of RX/TX lanes
// bja20120100 |bja     | HW553981: Enable CDR bank alignment on bank B in init
// vbr20110400 |vbr     | Fixed DFE step enables post merge from p10 main.
// vbr20050100 |vbr     | Combined register accesses
// vbr20043000 |vbr     | Step run in INIT based on PCIe gen or AXO.
// vbr20030200 |vbr     | Added initial hooks for handling PCIe training / PIPE signals
// vbr20102800 |vbr     | Moved bank align to before peaking.
// vbr20102700 |vbr     | Replaced bank align function (bumps) with CDR align mode.
//-------------|P11     |-------------------------------------------------------
// mbs21041401 |mbs     | HW563765: Changed skip RX DCCAL to rx_recal_abort
// mbs21041400 |mbs     | HW563765: Skip RX DCCAL when firmware has forced psave_req_dl on lane
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// mbs20102300 |mbs     | HW548202: Removed eo_main_recal_tx (no longer used)
// mwh20100801 |mwh     | HW549165: add if rx_min_recal_cnt_reached to running eo_vlcq check for recal taken it out of loop.
// mwh20100800 |mwh     | HW549165: add in bist_check if ture turn on checks if false turn off checks
// vbr20091600 |vbr     | HW542599: move rx_vga_amax from mem_regs to a stack variable (only used in init)
// vbr20091100 |vbr     | Move cal_time_us from PL to PG to make space
// jfg20090400 |jfg     | HW532333 Changed PR offset from PL to PG
// jfg20090100 |jfg     | HW532333 Add new static PR Offset feature
// jfg20051500 |jfg     | HW528360 Force ctle init cal to run as "first_iteration" on all loops not just the first. Even with increase of Reg hysteresis to 2 there is plenty of hysteresis.
// mbs20080500 |mbs     | HW539048- Added separate dfe_full training enable in init
// mbs20073000 |mbs     | LAB - Override rx_loff_timeout to 8
// vbr20061600 |vbr     | HW532652: Set bit_lock_done when could be bumping (training/DL), clear when CDR locking.
// vbr20042300 |vbr     | HW529150: Changes for lab testing to allow rx_disable_bank_pdwn to change after INIT
// bja20041700 |bja     | HW522518: Don't switch banks or power down the alt bank if init failed.
// mbs20030900 |mbs     | HW525009: Set bit_lock_done=1 during bank sync and dfe to avoid bank B unlocks
// jfg20031000 |jfg     | fix copy-paste error
// jfg20030900 |jfg     | HW525009 add a rough_only mode to set initial coarse peak 1&2
// vbr20021300 |vbr     | Added Min Eye Height enable to dfe_full calls
// vbr20020600 |vbr     | HW522210: Added setting of rx_lane_bad in INIT and every recal iteration.
// bja20020500 |bja     | Use new tx_fifo_init() in eo_main_dccal()
// cws19121100 |cws     | Moved Bist tests to External Commands
// bja20011400 |bja     | Remove TDR ZCAL from DCCAL. Moved to ext cmd.
// vbr19111500 |vbr     | Initial implementation of debug levels
// mbs19111100 |mbs     | Leave dl_clk_en untouched in eo_main_dccal (HW508366)
// mbs19091000 |mbs     | Added rx iodom reset after recal alt bank power up (HW504112)
// jfg19091100 |jfg     | Add first_run parm to eo_ctle
// gap19091000 |gap     | Change  rx_rc_enable_dcc, rx_dc_enable_dcc to tx_* and rx_dc_enable_zcal to tx_dc_enable_zcal_tdr HW503432
// bja19082100 |bja     | Correct rx_bist_max_lanes condition
// mwh19082000 |mwh     | removed rx_bist_max_ovrd and rx_bist_max_lanes_ovrd not need
// bja19081900 |bja     | Move eo_vclq_checks() to end of INIT
// bja19081901 |bja     | Only run eo_vclq_checks() when at least one check is enabled
// bja19081902 |bja     | Simplify code that calls ll and pr bist checks
// vbr19080900 |vbr     | HW499874: Move EOFF to back before bank sync in init.
// mbs19072500 |mbs     | Modified vga loop so that it will quit after the first iteration if rx_eo_converged_end_count is 0 or 1
// vbr19072202 |vbr     | Fix setting of tx_group in recal.
// vbr19072201 |vbr     | Minor refactoring and potential hang fix.
// vbr19072200 |vbr     | HW493618: Split recal into RX/TX functions and only run each when not in psave.
// vbr19062800 |vbr     | Need DL clock to remain on in INIT Cal.
// vbr19060700 |vbr     | Remove unnecessary pr_phase_force
// vbr19060601 |vbr     | In recal only set cal_lane_sel after powering up the Alt bank (to avoid clock glitches)
// vbr19060600 |vbr     | Put DL clock disable in correct location (after lane power up since it enables the DL clock)
// vbr19060300 |vbr     | HW486157/HW492011: Enable DL clock only at end of DC Cal. Remove power down at end of DC Cal. Move CDR Init to DC Cal.
// gap19041700 |gap     | Add tdr zcal; rename dcc_main to tx_dcc_main
// mbs19051600 |mbs     | HW491617: Added second run of dfe_full after ddc, if enabled
// mwh19050100 |mwh     | cq485000 moved b_bank eoff after bank sync in init
// mwh19042300 |mwh     | removed eo_bist_init_ovride function and put into ioo_thread.c
// mbs19041001 |mbs     | Added recal flag to dfe_full function (HW486784)
// mwh19042300 |mwh     | Fixed if statement for kick of ber max = lane now.
// mwh19042200 |mwh     | Change name of eo_rxbist_init_ovride to eo_bist_init_ovride
// mwh19041600 |mwh     | Add in rx_llbist code
// mwh19032800 |mwh     | Updated when we run pr check -- end of recal
// mwh19031400 |mwh     | in recal changed bank_a to cal_bank for bist checks
// mwh19031300 |mwh     | add rx_ber_check_en to skip eo_rxbist_ber if wanted
// vbr19040900 |vbr     | HW488485: Skip QPA before first CTLE in INIT.
// vbr19040400 |vbr     | HW472570: Check main bank psave before proceeding with recal.
// mwh19032100 |mwh     | Add in vga_loop_count so that we do not do the latch offset check more than once
// vbr19032700 |vbr     | Unconditionally disable cdr external mode at end of recal.
// vbr19030100 |vbr     | HW484186: reversed order of cal_lane_sel and bank_sel_a in several locations.
// gap19022700 |gap     | add fifo init before tx_bist and dcc_init
// jfg19030500 |jfg     | Rename eo_qpa recal_1strun as recal_2ndrun to match usage in main
// jfg19030400 |jfg     | Add qpa changed parameter
// jfg19022200 |jfg     | restore qpa recal
// jfg19021501 |jfg     | remove qpa recal
// jfg19020600 |jfg     | add eo_qpa function
// vbr19021800 |vbr     | HW482059: CDR is enabled before EOFF. All steps are run each VGA-CTLE loop in INIT. Check all steps for change.
// mwh19022501 |mwh     | removed set_cal_lane_sel on line 188 and the clear on line 201. Not Need
// mwh19022500 |mwh     | add in cal sel off and on where we are doing bank_sel (loff and eoff)
// mwh19021100 |mwh     | add enable of cdr before running eoff, this is where cdr is on now
// mwh19020600 |mwh     | add in bank sel change for eoff and put latch offset out into main
// vbr19013100 |vbr     | Only enable CDR external mode in Recal if no errors.
// vbr19012200 |vbr     | Updated function calls
// mwh19020501 |mwh     | add in to init that we do bank_a and bank_b for eoff
// mwh19011101 |mwh     | add eo_vclq_checks into recal flow
// mwh19011100 |mwh     | add eo_rxbist_init_ovrride
// vbr18121300 |vbr     | Updated the handling of abort at the very begining of recal.
// vbr18121200 |vbr     | HW474757: Increment recal count even on an error or abort. Clear recal abort sticky at the end of init.
// vbr18120400 |vbr     | Updated cal stopwatch to use constant from pk_app_cfg for divider.
// vbr18112900 |vbr     | Added assumption assertions
// gap18112700 |gap     | Rename tx_bist_enable_alias to tx_bist_en_alias
// vbr18112800 |vbr     | Added set_fir call for first recal abort exit. Added enable CDR in recal if no errors.
// vbr18111400 |vbr     | Updated calls to set_fir and added cal lane debug and lane_bad in recal. Added ability to disable cal timers.
// gap18102200 |gap     | Call txbist if requested
// vbr18101700 |vbr     | Added cal_timer macros to measure cal time.
// vbr18100200 |vbr     | Updated wait_for_cdr_lock() calls with set_fir_on_error.
// vbr18092800 |vbr     | All steps take recal=false in first recal loop so Bank B gets an "init" run.
// vbr18092700 |vbr     | New initial calibration flow where CDR is enabled after VGA and external mode only used for DFE/DDC. Removed static_config.
// cws18092800 |cws     | Added in eo dfe support
// mbs18091801 |mbs     | Moved CDR start, lock check and AB sync to after VGA (first loop only)
// mbs18091800 |mbs     | Moved CDR lock check and AB sync to after live edge lock (first loop only)
// vbr18091800 |vbr     | Added first_recal available for all functions in eo_recal.
// vbr18081701 |vbr     | Added some assumption checking
// vbr18081600 |vbr     | Added call to LTE cal step.
// jfg16082200 |jfg     | Change DDC recal_dac_changed bool to false as placeholder.
// jfg16081000 |jfg     | Update ddc call with recal_dac_changed
// vbr18080900 |vbr     | Make sure CDR is disabled in DC Cal.
// vbr18080600 |vbr     | Disable CDR before resetting it in case it is a retrain.
// vbr18072001 |vbr     | HW450195: Restructure power up/down (DC Cal) and move CDR Reset back into INIT.
// vbr18080100 |vbr     | HW456546: Power up the Alt bank bank at the beginning of DC cal and INIT since may be powered down in a retrain situation.
// vbr18070900 |vbr     | Clean-up of TX/RX steps in DC cal and recal.
// mwh18070500 |mwh     | Update for live edge offset eoff
// gap18070200 |gap     | Added tx dcc calls to recal and dccal
// vbr18071200 |vbr     | HW455558: Move CDR reset from eo_main_init() to io_hw_reg_init().
// vbr18062200 |vbr     | Added initialization of CDR accumulators to 0 at begining of INIT.
// vbr18061500 |vbr     | Switched to psave logic controls instead of directly writing bank power controls.
// jfg18042500 |jfg     | Update ctle with recal parm
// vbr18042500 |vbr     | HW446931: Call VGA as init (instead of recal) in very first recal (run on bank b) so converges faster. This may need to be moved for other steps in future.
// vbr16021600 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "eo_main.h"
#include "eo_loff.h"
#include "eo_vga.h"
#include "eo_vga_pathoffset.h"
#include "eo_ctle.h"
#include "eo_lte.h"
#include "eo_dfe.h"
#include "eo_ddc.h"
#include "eo_eoff.h"
#include "eo_eoff_1_lat.h"
#include "eo_qpa.h"
#include "eo_rxbist_epoff_final.h"
#include "rx_sigdetbist_test.h"

#include "tx_zcal_tdr.h"
#include "tx_dcc_main.h"
#include "eo_vclq_checks.h"

#include "io_init_and_reset.h"
#include "io_logger.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "io_config.h"

// Local Functions
static int eo_main_recal_rx(t_gcr_addr* gcr_addr);
//static int eo_main_recal_tx(t_gcr_addr *gcr_addr);


// Assumption Checking
PK_STATIC_ASSERT(rx_pr_edge_track_cntl_full_reg_width == 16);
PK_STATIC_ASSERT(rx_servo_intr_config_full_reg_width == 16);


////////////////////////////////////////////////////////////////////////////////////
// Servo Interrupt Config
// The Round Robin with Servo Pending Threading Method needs to set the
// servo interrupt config based on the calibration phase.
////////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    INTR_CFG_DCCAL   = 2,
    INTR_CFG_INITCAL = 1,
    INTR_CFG_RECAL   = 0
} t_intr_cfg;
#if IO_THREADING_METHOD == 2 // Round Robin With Servo Pending
static inline void servo_interrupt_config(t_gcr_addr* gcr_addr, t_intr_cfg interrupt_config, int pcie_mode)
{
    int reg_val;

    if (interrupt_config == INTR_CFG_RECAL)
    {
        // Enable servo queue interrupts and the recal abort interrupts based on PCIe or AXO mode
        if(pcie_mode)
        {
            reg_val = rx_servo_intr_op_queue_empty_en_mask | rx_servo_intr_result_queue_full_en_mask |
                      rx_servo_intr_pipe_recal_abort_en_mask;
        }
        else     //AXO mode
        {
            reg_val = rx_servo_intr_op_queue_empty_en_mask | rx_servo_intr_result_queue_full_en_mask |
                      rx_servo_intr_dl_recal_abort_sticky_en_mask | rx_servo_intr_bump_recal_abort_en_mask;
        }
    }
    else     // INTR_CFG_DCCAL || INTR_CFG_INITCAL
    {
        // Only enable servo queue interrupts, exclude the abort interrupts
        reg_val = rx_servo_intr_op_queue_empty_en_mask | rx_servo_intr_result_queue_full_en_mask;
    }

    // Fast write the dedicated register
    put_ptr_field(gcr_addr, rx_servo_intr_config_full_reg, reg_val, fast_write);
}
#else
// Simple Round Robin: Function unused
#define servo_interrupt_config(gcr_addr, cal_phase, pcie_mode) {}
#endif


////////////////////////////////////////////////////////////////////////////////////
// DC CAL
// Run DC Cal on both banks of the lane specified in gcr_addr.
// ASSUMPTIONS:
// 1) Main bank is powered up (though the lane may be powered down).
////////////////////////////////////////////////////////////////////////////////////
int eo_main_dccal(t_gcr_addr* gcr_addr)
{
    CAL_TIMER_START;
    set_debug_state(0x1000); // Debug - DC Cal Start
    int status = rc_no_error;
    int lane = get_gcr_addr_lane(gcr_addr);
#if IO_DEBUG_LEVEL >= 1
    mem_pg_field_put(rx_current_cal_lane, lane);
#endif

    /////////////////////////////
    // Power up group and lane //
    /////////////////////////////
    io_group_power_on(gcr_addr);
    status |= io_lane_power_on(gcr_addr, false); // Power on but leave dl_clk_en untouched (HW508366)

    // Run RX if lane exists
    int num_lanes_rx = get_num_rx_lane_slices();

    if (lane < num_lanes_rx)
    {
        // PSL eo_main_dccal_rx
        status |= eo_main_dccal_rx(gcr_addr);
    }

    // Run TX if lane exists
    int num_lanes_tx = get_num_tx_lane_slices();

    if (lane < num_lanes_tx)
    {
        // PSL eo_main_dccal_tx
        status |= eo_main_dccal_tx(gcr_addr);
    }

    set_debug_state(0x102F); // DEBUG - DC Cal Done
    CAL_TIMER_STOP;
    return status;
} //eo_main_dccal()


int eo_main_dccal_rx(t_gcr_addr* gcr_addr)
{
    set_debug_state(0x1010); // Debug - DC Cal RX Start
    int status = rc_no_error;
    int lane = get_gcr_addr_lane(gcr_addr);
#if IO_DEBUG_LEVEL >= 1
    mem_pg_field_put(rx_current_cal_lane, lane);
#endif

    // Make sure servo queues are in a good state
    int servo_queue_status = check_servo_queues_empty(gcr_addr, 0x101A, DEBUG_DCCAL_SERVO_QUEUE_NOT_EMPTY);

    if (servo_queue_status)
    {
        return error_code;
    }

    // HW563765: Skip RX DCCAL when firmware has forced rx_recal_abort on lane
    int rx_recal_abort = (0x80000000 >> lane) & ( (mem_pg_field_get(rx_recal_abort_0_15) << 16) | (mem_pg_field_get(
                             rx_recal_abort_16_23) << (16 - rx_recal_abort_16_23_width)) );

    // PSL recal_abort
    if (rx_recal_abort)
    {
        return error_code;
    }

    // Configure the servo interrupts
    servo_interrupt_config(gcr_addr, INTR_CFG_DCCAL, 0);

    // Configure the PIPE Abort
    pipe_abort_config(gcr_addr, PIPE_ABORT_CFG_DCCAL);


    //////////////////////////////////////////////////////////////
    // Disable Clock to RX DL During DC Cal to prevent glitches //
    //////////////////////////////////////////////////////////////
    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0, read_modify_write);


    //////////////////////////////////////////////////////////////
    // Run RX SIGDET BIST if enabled
    //////////////////////////////////////////////////////////////

// Run RX SIGDET BIST if enabled
    int pcie_mode = fw_field_get(fw_pcie_mode);

    if (pcie_mode)  //only run for pcie
    {
        int rx_sigdet_check_en_int = get_ptr(gcr_addr, rx_sigdet_check_en_addr, rx_sigdet_check_en_startbit,
                                             rx_sigdet_check_en_endbit);//pg

        if (rx_sigdet_check_en_int)
        {
            rx_sigdetbist_test(gcr_addr);
        }
    }

    // Make sure the ALT bank is powered up
    alt_bank_psave_clear_and_wait(gcr_addr);

    // Prior to asserting cal_lane_sel:
    //   Initialize/Reset CDR by clearing FW; Phase accumulator not cleared since that can cause a glitch.
    //   Make sure the CDR is disabled.
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis, fast_write);
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias,        0b11,                read_modify_write);
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias,        0b00,                read_modify_write);

    // Select the cal lane (servo logic) but do not assert cal_lane_sel as yet
    put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, lane, read_modify_write);

    // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.
    clear_all_cal_lane_sel(gcr_addr);

    // Cal Step: Latch Offset
    int loff_enable = mem_pg_field_get(rx_dc_enable_latch_offset_cal);

    if (loff_enable)
    {
        // Safely switch to bank_a and run LOFF (assumes cal_lane_sel is unasserted)
        set_cal_bank(gcr_addr, bank_a);// Set Bank B as Main, Bank A as Alt (cal_bank)
        put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // turn on cal lane sel
        // PSL eo_loff_fenced_bank_a
        status |= eo_loff_fenced(gcr_addr, bank_a);

        // Safely switch to bank_b and run LOFF
        put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write); // clear rx_cal_lane_sel
        set_cal_bank(gcr_addr, bank_b);// Set Bank A as Main, Bank B as Alt (cal_bank)
        put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // turn on cal lane sel
        // PSL eo_loff_fenced_bank_b
        status |= eo_loff_fenced(gcr_addr, bank_b);
    }

    // Clear cal lane sel and switch back to Bank B as Main, Bank A as Alt (cal_bank)
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);
    set_cal_bank(gcr_addr, bank_a);

    // Power Down the ALT Bank (if configured to power down). This should be Bank A after the above cal steps.
    int disable_bank_powerdown = mem_pg_field_get(rx_disable_bank_pdwn);

    // PSL disable_bank_powerdown
    if (!disable_bank_powerdown)
    {
        set_debug_state(0x101A, 3); // DEBUG - DC Cal Alt Bank Power Down
        put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
    }

    // Removed (HW508366) // Enable Clock to RX DL
    // Removed (HW508366) // put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write);

    set_debug_state(0x101F); // Debug - DC Cal RX End
    return status;
} //eo_main_dccal_rx()


// Helper function for running TX DCC at different PCIe rates and saving the results
int run_tx_dcc_and_save_results(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode)
{
    // Change the clock rate and initialize the FIFO at the new rate
    uint32_t rate_one_hot = (1 << cal_mode);
    clock_change_tx(gcr_addr, rate_one_hot);
    tx_fifo_init(gcr_addr);

    // Run TX DCC and save the results
    // PSL tx_dcc_main_init
    int status = tx_dcc_main_init(gcr_addr);
    save_tx_dcc_tune_values(gcr_addr, cal_mode);

    return status;
} //run_tx_dcc_and_save_results

int eo_main_dccal_tx(t_gcr_addr* gcr_addr)
{
    set_debug_state(0x1030); // Debug - DC Cal TX Start
    int status = rc_no_error;
#if IO_DEBUG_LEVEL >= 1
    int lane = get_gcr_addr_lane(gcr_addr);
    mem_pg_field_put(rx_current_cal_lane, lane);
#endif

    set_gcr_addr_reg_id(gcr_addr, tx_group); // set to tx gcr address

    // run tx_fifo_init; needs to be run before dcc or bist or Zcal
    tx_fifo_init(gcr_addr);

    // Cal Step: TX Duty Cycle Correction; needs to be run before tx_bist_dcc
    int tx_dcc_enable = mem_pg_field_get(tx_dc_enable_dcc);

    if (tx_dcc_enable)
    {
        int pcie_mode = fw_field_get(fw_pcie_mode);

        if (pcie_mode)
        {
            // Run TX DCC at each of three rates and save the results
            // ASSUMPTION: DCCAL with TX DCC enabled is only called with the default tx_pcie_clk_sel of Gen5.
            //             Thus, do not need to save the tx_pcie_clk_sel value and revert to those settings.

            // Gen3
            status |= run_tx_dcc_and_save_results(gcr_addr, C_PCIE_GEN3_CAL);

            // Gen4
            status |= run_tx_dcc_and_save_results(gcr_addr, C_PCIE_GEN4_CAL);

            // Gen5
            // PSL run_tx_dcc_and_save_results
            status |= run_tx_dcc_and_save_results(gcr_addr, C_PCIE_GEN5_CAL);
        }
        else     //AXO
        {
            // PSL tx_dcc_main_init
            status |= tx_dcc_main_init(gcr_addr);
        }
    }

    set_gcr_addr_reg_id(gcr_addr, rx_group); // set to rx gcr address
    set_debug_state(0x103F); // Debug - DC Cal TX End
    return status;
} //eo_main_dccal_tx()


////////////////////////////////////////////////////////////////////////////////////
// INIT CAL
// Run Initial Cal on both banks of the lane specified in gcr_addr.
// ASSUMPTIONS:
// 1) Main bank is powered up.
// 2) DC Cal has already been run.
// 3) PRBS data is on the bus.
////////////////////////////////////////////////////////////////////////////////////
int eo_main_init(t_gcr_addr* gcr_addr)
{
    CAL_TIMER_START;
    set_debug_state(0x2000); // Debug - INIT Cal Start
    int lane = get_gcr_addr_lane(gcr_addr);
#if IO_DEBUG_LEVEL >= 1
    mem_pg_field_put(rx_current_cal_lane, lane);
#endif

    // Make sure servo queues are in a good state
    int servo_queue_status = check_servo_queues_empty(gcr_addr, 0x200A, DEBUG_INITCAL_SERVO_QUEUE_NOT_EMPTY);

    if (servo_queue_status)
    {
        return error_code;
    }

    // Determine Calibration mode
    t_init_cal_mode cal_mode;
    int pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL set_cal_mode
    if (pcie_mode)
    {
        set_gcr_addr_reg_id(gcr_addr, tx_group); // PIPE registers are in TX reg space
        int rate = get_ptr_field(gcr_addr, pipe_state_rate);
        set_gcr_addr_reg_id(gcr_addr, rx_group);
        cal_mode = rate; // Gen1=0...Gen5=4
    }
    else
    {
        cal_mode = C_AXO_CAL;
    }

    // What steps we run depends on the cal_mode
    bool pcie_cal          = (pcie_mode != 0); //(cal_mode != C_AXO_CAL);
    bool pcie_gen1_cal     = (cal_mode == C_PCIE_GEN1_CAL);
    bool pcie_gen2_cal     = (cal_mode == C_PCIE_GEN2_CAL);
    bool pcie_gen3_cal     = (cal_mode == C_PCIE_GEN3_CAL);
    //bool pcie_gen4_cal     = (cal_mode == C_PCIE_GEN4_CAL);
    //bool pcie_gen5_cal     = (cal_mode == C_PCIE_GEN5_CAL);
    bool pcie_gen3_4_5_cal = (cal_mode == C_PCIE_GEN5_CAL) || (cal_mode == C_PCIE_GEN4_CAL)
                             || (cal_mode == C_PCIE_GEN3_CAL);
    bool pcie_gen1_2_cal   = (cal_mode == C_PCIE_GEN1_CAL) || (cal_mode == C_PCIE_GEN2_CAL);

    // Configure the servo interrupts
    servo_interrupt_config(gcr_addr, INTR_CFG_INITCAL, pcie_mode);

    // Configure the PIPE Abort
    pipe_abort_config(gcr_addr, PIPE_ABORT_CFG_INITCAL);

    // In initial cal, track step status for marking lane bad.
    // But unlike recal, do not skip steps on status being set.
    int status = rc_no_error;

    // Make sure the ALT bank is powered up
    alt_bank_psave_clear_and_wait(gcr_addr);

    // AXO: Make sure the CDRs are disabled for INIT.
    // PCIe: Leave the CDRs in their current state.
    // PSL axo_disable_edge_track_cntl
    if (!pcie_cal)
    {
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis, fast_write);
    }

    // HW532652: The psave logic sets bit_lock_done, so clear it here
    put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, 0b11, fast_write);

    // Prior to asserting cal_lane_sel: Set Bank B as Main, Bank A as Alt (cal_bank)
    // This may cause a DL clock glitch if the lane was not already in this state (HW496723)
    // AXO: Setting B as Main and A as Alt (with DL clock change) should be fine since that is the state after DC Cal
    // PCIe: On a rate change we have already switched back to Bank A as ALT (pipe_cmd_rate_updated, step 10).
    //       On a RXEQ (Gen 3+) we need to safely switch to A.
    int bank_sel_a = get_ptr_field(gcr_addr, rx_bank_sel_a);

    // PSL bank_sel_a_edge_track_cntl
    if (bank_sel_a)
    {
        // On a RXEQ (Gen 3+) the banks should be well enough calibrated to align before switching Bank A to ALT.
        // Put Bank B into align_mode so don't change the DL data alignment on Bank A.
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl_align, fast_write);
        // Max Sync Time: Gen5 = 500ns, Gen4 = 1us, Gen3 = 2us, Gen2 = 4us, Gen1 = 8us
        io_spin_us(2); //busy wait
    }

    set_cal_bank(gcr_addr, bank_a);

    // Select the cal lane
    clear_all_cal_lane_sel(
        gcr_addr); // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.
    put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
    put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, lane, read_modify_write);


    /////////////////////////////////////////////////////////////////////////
    // Steps run on Bank A in loop; select steps run later on Bank B once.
    /////////////////////////////////////////////////////////////////////////

    // Cal Step: VGA (Gain) - Only run in AXO (A) or PCIe Gen1/Gen2 (A/B)
    int vga_enable  = (pcie_gen3_4_5_cal) ? 0 : mem_pg_field_get(rx_eo_enable_vga_cal);

    // Cal Step: Path Offset (Ap+An/2) - Only run in PCIe Gen1/Gen2 (A/B)
    int poff_enable = (pcie_gen1_2_cal) ? mem_pg_field_get(rx_eo_enable_edge_offset_cal) : 0;

    // Cal Step: CTLE (Peaking) - Enabled for AXO (Peak1/2), Gen3 (Peak2), Gen4 (Peak2), Gen5 (Peak1/2) - Bank A only
    int ctle_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_eo_enable_ctle_peak_cal);

    // Cal Step: Edge/Path Offset (Live Data) - Only run in AXO (A/B) or PCIe Gen4/Gen5 (A)
    int eoff_enable = (pcie_gen1_2_cal || pcie_gen3_cal) ? 0 : mem_pg_field_get(rx_eo_enable_edge_offset_cal);

    // Cal Step: Quad Phase Adjust - Enabled for AXO only (A)
    int quad_enable = (!pcie_cal) ? mem_pg_field_get(rx_eo_enable_quad_phase_cal) : 0;

    // Loop for VGA + EOF + CTLE + LTE
    // Always go through this loop at least once.
    // 1) Run VGA on A Bank. Copy result to B Bank.
    // 2) If this is the first loop iteration, enable the CDR (local data mode) on both banks.
    // 3) Run Edge Offset on both banks.
    // 4) Run CTLE and LTE on A Bank. Copy result to B Bank.
    // 5) If this is the first loop iteration, run Quad Phase Adjust on A Bank.
    // 6) If AXO and gain or peak changed, repeat the loop from #1.
    //
    // Must always run loop at least once so CDR gets enabled
    bool run_vga_loop = true;
    unsigned int vga_loop_count = 0;
    int  saved_Amax = 0;
    int  saved_Amax_poff = 0;

    while (run_vga_loop)
    {
        bool first_loop_iteration = (vga_loop_count == 0);

        // Cal Step: VGA (Gain) - Only run in AXO or PCIe Gen1 or PCIe Gen2
        // Does not require edge tracking or bank alignment
        bool gain_changed = false;

        if (vga_enable && ((status & abort_code) == 0))
        {
            bool recal = false;
            bool copy_gain_to_b = true;
            bool copy_gain_to_b_loop = true;
            // PSL eo_vga
            status |= eo_vga(gcr_addr, bank_a, &saved_Amax, &saved_Amax_poff, &gain_changed, recal, copy_gain_to_b,
                             copy_gain_to_b_loop, first_loop_iteration);
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // Cal Step: Path Offset (Ap+An/2) - Only run in PCIe Gen1 or Gen2
        if (poff_enable && ((status & abort_code) == 0))
        {
            bool recal = false;
            bool first_recal = false;
            // PSL eo_vga_pathoffset
            status |= eo_vga_pathoffset(gcr_addr, bank_a, saved_Amax_poff, recal, first_recal, pcie_gen1_cal, pcie_gen2_cal, false);
        }

        // CDR must be locked prior to running EOFF, Quad Adjust, CTLE, or LTE.
        // To lock properly, it requires:
        //   1) The edge latches have all offset (latch and path) calibrated out.
        //   2) The data bits are being received well enough that don't get false invalid_locks.
        // It is likely that the CDR will not lock due to the above conditions not being met as yet and thus will run to the timeout.
        // For this reason, we ignore the status of this CDR lock wait.
        // PSL first_loop
        if (first_loop_iteration)
        {
            // Enable Independent Edge Tracking (both banks local data) and wait for lock on both banks.
            // Only wait for lock if PCIe Gen 1/2 where the next step is bank sync;
            // AXO and PCIe Gen 3-5 have EOFF and Peaking as the next steps which can handle the CDR not being locked as yet.
            set_debug_state(0x200F); // DEBUG - Init Cal Enable Edge Tracking
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);

            // PSL first_loop_gen1_2_cal
            if (pcie_gen1_2_cal)
            {
                bool set_fir_on_error = false;
                wait_for_cdr_lock(gcr_addr, set_fir_on_error);
            }
            else
            {
                io_spin_us(2); //busy wait
            }

            // Need to force a sleep in the case where none of the steps in this loop are enabled (Gen 2, Sim) to limit thread active time
            int vpec_enable = vga_enable | poff_enable | eoff_enable | ctle_enable;

            if (!vpec_enable)
            {
                io_sleep(get_gcr_addr_thread(gcr_addr));
            }
        } //first_loop_iteration

        // Cal Step: Edge/Path Offset (Live Data) - Only run in AXO, PCIe Gen4, PCIe Gen5
        // Requires edge tracking (local data mode) but does not require bank alignment
        if (eoff_enable && ((status & abort_code) == 0))  //begin eoff_enable
        {
            bool recal = false;
            bool vote_sel = first_loop_iteration;//transition vote not used when true
            // PSL eo_eoff_1_lat
            status |= eo_eoff_1_lat(gcr_addr, recal, bank_a, vote_sel);
            //io_sleep(get_gcr_addr_thread(gcr_addr));
        }//end eoff_enable

        // Cal Step: CTLE (Peaking) - Enabled for AXO (Peak1/2), Gen3 (Peak2), Gen4 (Peak2), Gen5 (Peak1/2)
        // Requires edge tracking (local data mode) but does not require bank alignment
        bool peak_changed = false;

        if (ctle_enable && ((status & abort_code) == 0))
        {
            bool recal = false;
            bool hysteresis_en = false;
            bool copy_peak_to_b = true;
            bool start_at_zero = first_loop_iteration;
            // PSL eo_ctle
            status |= eo_ctle(gcr_addr, cal_mode, bank_a, copy_peak_to_b, start_at_zero, recal, hysteresis_en, &peak_changed);
        }

        // Cal Step: LTE - Disabled for all rates/modes in INIT by default. Read mem_reg enable instead of hardcoding 0 to include code for lab/sim.
        // Requires edge tracking (local data mode) but does not require bank alignment
        int lte_enable = 0; //mem_pg_field_get(rx_eo_enable_lte_cal);

        if (lte_enable && ((status & abort_code) == 0))
        {
            bool lte_changed = false;
            bool recal = false;
            bool copy_lte_to_b = true;
            // Impossible to run PSL eo_lte
            status |= eo_lte(gcr_addr, bank_a, copy_lte_to_b, recal, &lte_changed);
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // Cal Step: Quad Phase Adjust (Edge NS to EW phase adjustment) - Only run in first loop. Enabled for AXO only.
        // Requires edge tracking (local data mode) but does not require bank alignment
        if (quad_enable  && first_loop_iteration && ((status & abort_code) == 0))
        {
            bool quad_adjust_changed = false;
            bool recal_2ndrun = false;
            // PSL eo_qpa
            status |= eo_qpa(gcr_addr, bank_a, recal_2ndrun, &quad_adjust_changed);
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // Check for loop termination.
        // Run loop at least twice for AXO (VGA+Peak+QPA) but only once for PCIe (Peak+QPA).
        // Keep running if settings (gain/peak) changed, but limit the number of loops.
        vga_loop_count = vga_loop_count + 1;

        // PSL vga_loop_converged_outer
        if (!pcie_cal && (first_loop_iteration || gain_changed || peak_changed))
        {
            unsigned int converged_cnt_max = mem_pg_field_get(rx_eo_converged_end_count);

            // PSL vga_loop_converged_inner
            if (vga_loop_count < converged_cnt_max)
            {
                run_vga_loop = true;
            }
            else
            {
                // Loop failed to converge
                run_vga_loop = false;
                mem_pl_bit_set(rx_eo_vga_ctle_loop_not_converged, lane);
            }
        }
        else     // pcie_cal || (!first_loop_iteration && !(gain_changed || peak_changed))
        {
            // Loop completed successfuly
            run_vga_loop = false;
            mem_pl_bit_clr(rx_eo_vga_ctle_loop_not_converged, lane);
        }
    } //while run_vga_ctle_loop

    // These B Bank Cal Steps are run in the same situations as the A Bank Cal steps
    // Cal Step: VGA + Path Offset (Ap+An/2) on Bank B - Only run in PCIe Gen1/Gen2
    // Cal Step: Edge/Path Offset (Live Data) on Bank B - Only run in AXO (Not PCIe Gen4/Gen5)
    eoff_enable = pcie_cal ? 0 : eoff_enable;

    if (poff_enable | eoff_enable)
    {
        // Safely switch to bank_b without changing dl_clk_sel_a to avoid DL clock chopping (HW485000)
        // The cal_lane_sel toggling shouldn't be needed since the banks are already in sync - unless there is an error
        if (status)
        {
            put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);    // clear rx_cal_lane_sel
        }

        set_cal_bank_leave_dlclk(gcr_addr, bank_b);// Set Bank A as Main, Bank B as Alt (cal_bank)

        if (status)
        {
            put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write);    // turn on cal lane sel
        }

        if (poff_enable && ((status & abort_code) == 0))
        {
            bool recal = false;
            bool first_recal =  false;

            if (vga_enable && ((status & abort_code) == 0))  //begin vga_enable
            {
                bool gain_changed = false;
                bool copy_gain_to_b = false;
                bool copy_gain_to_b_loop = false;
                bool first_loop_iteration = false;//copy of A should put close
                // PSL poff_eo_vga
                status |= eo_vga(gcr_addr, bank_b, &saved_Amax, &saved_Amax_poff, &gain_changed, recal, copy_gain_to_b,
                                 copy_gain_to_b_loop, first_loop_iteration);
                io_sleep(get_gcr_addr_thread(gcr_addr));
            }//end vga_enable

            // PSL poff_eo_vga_pathoffset
            if ((status & abort_code) == 0)
            {
                status |= eo_vga_pathoffset(gcr_addr, bank_b, saved_Amax_poff, recal, first_recal, pcie_gen1_cal, pcie_gen2_cal, false);
            }
        }//poff_en

        if (eoff_enable && ((status & abort_code) == 0))
        {
            bool recal = false;
            bool vote_sel = true;//transition vote not used when true
            // PSL poff_eo_eoff_1_lat
            status |= eo_eoff_1_lat(gcr_addr, recal, bank_b, vote_sel);
            //io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // Safely switch to bank_a
        // The cal_lane_sel toggling shouldn't be needed since the banks are already in sync - unless there is an error
        if (status)
        {
            put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);    // clear rx_cal_lane_sel
        }

        set_cal_bank(gcr_addr, bank_a);// Set Bank B as Main, Bank A as Alt (cal_bank)

        if (status)
        {
            put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write);    // turn on cal lane sel
        }

        //io_sleep(get_gcr_addr_thread(gcr_addr));
    } //bank_b poff/eoff

    // Perform Bank A/B UI Alignment by putting Alt bank into align mode. Requires edge tracking.
    int bank_sync_enable = mem_pg_field_get(rx_eo_enable_bank_sync);

    if (bank_sync_enable && ((status & abort_code) == 0))
    {
        set_debug_state(0x2010); // DEBUG - Init Cal Bank Sync
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_align_cdr_b_lcl, fast_write);

        // Max Sync Time: AXO/Gen5 = 500ns, Gen4 = 1us, Gen3 = 2us, Gen2 = 4us, Gen1 = 8us
        // PSL bank_sync_pcie_gen1_2_cal
        if (pcie_gen1_2_cal)
        {
            io_wait_us(get_gcr_addr_thread(gcr_addr), 8); //sleep
        }
        else     //AXO, PCIe Gen 3-5
        {
            io_spin_us(2); //busy wait
        }
    }

    // HW525009: Turn off invalid lock detection to avoid coming out of sync on bank B after bank sync
    put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);

    // Disable bank sync mode before running DFE/DDC since alter the data (Issue 291915: do not do at same time as ext_mode_en)
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);

    // Check for abort during bank sync
    status |= check_rx_abort(gcr_addr);

    // Put Alt (Cal, A) bank into CDR External mode (if no pipe abort) for DFE Amp Measurements and DDC
    if ((status & abort_code) == 0)
    {
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_ext_cdr_b_lcl, fast_write);
    }

    // Cal Step: DFE Fast - Only run in AXO (H1-H3), PCIe Gen3 (H1), PCIe Gen4 (H1), PCIe Gen5 (H1-H3)
    // Requires edge tracking (external data mode) and bank alignment
    int dfe_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_eo_enable_dfe_cal);

    if (dfe_enable && ((status & abort_code) == 0))
    {
        // PSL rx_eo_dfe_fast
        status |= rx_eo_dfe_fast(gcr_addr, cal_mode);
        io_sleep(get_gcr_addr_thread(gcr_addr)); // TODO - can this be removed? Issue 268776
    }

    // Issue 256140: Re-enable invalid lock detection and lock detector before DDC
    put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, 0b11, fast_write);

    // Cal Step: DDC - Only run in AXO or PCIe Gen3/4/5
    // Requires edge tracking (external data mode) and bank alignment
    int ddc_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_eo_enable_ddc);

    if (ddc_enable && ((status & abort_code) == 0))
    {
        bool recal = false;
        bool recal_dac_changed = false;
        // PSL eo_ddc
        status |= eo_ddc(gcr_addr, bank_a, recal, recal_dac_changed);
        io_sleep(get_gcr_addr_thread(gcr_addr)); // TODO - can this be removed?
    }

    //DFE FULL need to be run after DDC. This is for bist only.
    int bist_check_en = get_ptr_field(gcr_addr, rx_check_en_alias);

    if (dfe_enable && (bist_check_en & rx_dfe_check_en_mask) && ((status & abort_code) == 0))
    {
        bool run_all_quads = true;
        bool dfe_hyst_en = false;
        bool enable_min_eye_height = true;
        // PSL eo_dfe_full
        status |= rx_eo_dfe_full(gcr_addr, bank_a, run_all_quads, dfe_hyst_en, enable_min_eye_height);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Perform Check of VGA, CTLE, LTE, and QPA values
    // Check pass/fail status of other steps
    // part of bist
    if ( bist_check_en )  //bring bist_check_en and mask the vector so that see if stuff is enabled
    {
        if (bist_check_en & rx_eoff_check_en_mask) //must be before vclq so set fail_flag
        {
            // PSL eo_rxbist_epoff_final_bank_a
            eo_rxbist_epoff_final(gcr_addr, bank_a);//edge only
            // PSL eo_rxbist_epoff_final_bank_b
            eo_rxbist_epoff_final(gcr_addr, bank_b);//edge only
        }

        // PSL eo_vclq_checks
        eo_vclq_checks(gcr_addr, bank_a, bist_check_en);
    }

    // Cal Done: Re-enable CDR local data mode on both banks and double check for lock with invalid lock detection on
    set_debug_state(0x2015); // DEBUG - Init Cal Final Edge Tracking
    //Moved to before DDC: put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, 0b11, fast_write);
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);
    bool set_fir_on_error = true;
    status |= wait_for_cdr_lock(gcr_addr, set_fir_on_error);

    // HW532652: Turn off invalid lock detection to help with DL bumping
    put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);

    // Clear cal lane sel
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);

    // HW522518: Don't switch banks or power down the alt bank if init failed.
    // The primary concern is to avoid a DL clock glitch when bank sync fails.
    if (status == rc_no_error)
    {
        // Set Bank A as Main and Bank B as Alt
        // PSL set_cal_bank
        set_cal_bank(gcr_addr, bank_b);

        // Power Down the ALT Bank (if configured to power down). This should be Bank B after the above cal steps.
        int disable_bank_powerdown = mem_pg_field_get(rx_disable_bank_pdwn);

        if (!disable_bank_powerdown)
        {
            set_debug_state(0x201A, 3); // DEBUG - Init Alt Bank Power Down
            put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
        }
    }

    // AXO: Now that the lane is trained, re-enable the psave logic waiting on the CDR lock (for dynamic bus powerdown)
    // PSL axo_psave_cdrlock_mode_sel
    if (!pcie_cal)
    {
        put_ptr_field(gcr_addr, rx_psave_cdrlock_mode_sel, 0b00, read_modify_write); //pl
    }

    // Clear the recal count
    mem_pl_field_put(rx_lane_recal_cnt, lane, 0);
    mem_pl_bit_clr(rx_min_recal_cnt_reached, lane);

    // Moved to run_recalibration(): Clear the recal_abort sticky bit in case it was previously asserted
    //put_ptr_field(gcr_addr, rx_dl_phy_recal_abort_sticky_clr, 0b1, fast_write); // strobe bit

    // Warning FIR and Bad Lane status on Warning/Error
    if ( (status & (warning_code | error_code)) )
    {
        set_rx_lane_fail(lane);
        // PSL set_fir_bad_lane_warning
        set_fir(fir_code_bad_lane_warning);
    }
    else if (!bist_check_en)
    {
        clr_rx_lane_fail(lane); // Clear in case it was set by a prior training (and this is a retrain)
    }

#if IO_DEBUG_LEVEL > 1
    // Check the measured cal time (only measured when debug_level>=2)
    int io_threads = img_field_get(ppe_num_threads);
    int                                       cal_time_limit = PPE_INIT_CAL_TIME_US_BUDGET_PCIE_1TH;

    if ((pcie_mode == 1) && (io_threads > 1))
    {
        cal_time_limit = PPE_INIT_CAL_TIME_US_BUDGET_PCIE_4TH;
    }

    if ((pcie_mode != 1) && (io_threads > 1))
    {
        cal_time_limit = PPE_INIT_CAL_TIME_US_BUDGET_AXO_4TH ;
    }

    if ((pcie_mode != 1) && (io_threads <= 1))
    {
        cal_time_limit = PPE_INIT_CAL_TIME_US_BUDGET_AXO_1TH ;
    }

    CAL_TIMER_STOP;

    if (cal_time_us > cal_time_limit)
    {
        mem_pg_field_put(ppe_init_time_us_warning, 1);
        ADD_LOG(DEBUG_PPE_INIT_OVER_TIME, cal_time_us);
    }

#endif

    set_debug_state(0x202F); // DEBUG - INIT Cal Done
    return status;
} //eo_main_init


////////////////////////////////////////////////////////////////////////////////////
// RECAL
// Run Recal on the ALT bank of the lane specified in gcr_addr.
// Return code indicates rc_no_error or rc_abort.
////////////////////////////////////////////////////////////////////////////////////
int eo_main_recal(t_gcr_addr* gcr_addr)
{
    CAL_TIMER_START;
    set_debug_state(0x3000); // Debug - Recal Start
    int lane = get_gcr_addr_lane(gcr_addr);
#if IO_DEBUG_LEVEL >= 1
    mem_pg_field_put(rx_current_cal_lane, lane);
#endif

    // Run RX Recal
    int status = rc_no_error;

    if (status == rc_no_error)
    {
        // HW493618: Skip RX Recal when DL has powered down RX (bad lane)
        int dl_phy_rx_psave_req = get_ptr_field(gcr_addr, rx_psave_req_dl);

        // PSL rx_psave_req
        if (!dl_phy_rx_psave_req)
        {
            // PSL eo_main_recal_rx
            status |= eo_main_recal_rx(gcr_addr);
        }
        else
        {
            // Avoid hang condition by returning abort status when RX recal not run
            status |= abort_clean_code;
        }
    }

//  // Run TX Recal if no RX status
//  if (status == rc_no_error) {
//    // HW493618: Skip TX Recal when DL has powered down TX (bad lane)
//    set_gcr_addr_reg_id(gcr_addr, tx_group); // set to tx gcr address
//    int dl_phy_tx_psave_req_0_23 =
//      (get_ptr_field(gcr_addr, tx_psave_req_dl_0_15_sts)  << 16) |
//      (get_ptr_field(gcr_addr, tx_psave_req_dl_16_23_sts) << (16 - tx_psave_req_dl_16_23_sts_width));
//    set_gcr_addr_reg_id(gcr_addr, rx_group); // set back to rx gcr address
//    uint32_t lane_mask = (0x80000000 >> lane);
//    int dl_phy_tx_psave_req = dl_phy_tx_psave_req_0_23 & lane_mask;
//    if (!dl_phy_tx_psave_req) {
//      status |= eo_main_recal_tx(gcr_addr);
//    }
//  }

    set_debug_state(0x302F); // DEBUG - Recal Done
    CAL_TIMER_STOP;
    return status;
} //eo_main_recal


//////////////
// RX Recal //
//////////////
static int eo_main_recal_rx(t_gcr_addr* gcr_addr)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // Make sure servo queues are in a good state
    int status = check_servo_queues_empty(gcr_addr, 0x300A, DEBUG_RECAL_SERVO_QUEUE_NOT_EMPTY);

    // Check for abort and exit with abort code if detected
    status |= check_rx_abort(gcr_addr);

    if (status != rc_no_error)
    {
        set_debug_state(0x30FF); // Debug - Recal Abort
        //check_rx_abort sets the FIR as needed set_fir(fir_code_recal_abort);
        return status;
    }

    // Determine Calibration mode
    t_init_cal_mode cal_mode;
    int pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL set_cal_mode
    if (pcie_mode)
    {
        set_gcr_addr_reg_id(gcr_addr, tx_group); // PIPE registers are in TX reg space
        int rate = get_ptr_field(gcr_addr, pipe_state_rate);
        set_gcr_addr_reg_id(gcr_addr, rx_group);
        cal_mode = rate; // Gen1=0...Gen5=4
    }
    else
    {
        cal_mode = C_AXO_CAL;
    }

    // What steps we run depends on the cal_mode
    //bool pcie_cal          = (pcie_mode != 0); //(cal_mode != C_AXO_CAL);
    bool pcie_gen1_cal     = (cal_mode == C_PCIE_GEN1_CAL);
    bool pcie_gen2_cal     = (cal_mode == C_PCIE_GEN2_CAL);
    bool pcie_gen3_cal     = (cal_mode == C_PCIE_GEN3_CAL);
    //bool pcie_gen4_cal     = (cal_mode == C_PCIE_GEN4_CAL);
    //bool pcie_gen5_cal     = (cal_mode == C_PCIE_GEN5_CAL);
    //bool pcie_gen3_4_5_cal = (cal_mode == C_PCIE_GEN5_CAL) || (cal_mode == C_PCIE_GEN4_CAL) || (cal_mode == C_PCIE_GEN3_CAL);
    bool pcie_gen1_2_cal   = (cal_mode == C_PCIE_GEN1_CAL) || (cal_mode == C_PCIE_GEN2_CAL);

    // Configure the servo interrupts
    servo_interrupt_config(gcr_addr, INTR_CFG_RECAL, pcie_mode);

    // Configure the PIPE Abort
    pipe_abort_config(gcr_addr, PIPE_ABORT_CFG_RECAL);

    // Use bank_sel_a (HW latch) to tell which is the Main Bank so can calibrate the Alternate Bank. Could improve the speed of this by storing
    // this info in the mem_regs for each lane; however, reading it once per-recal from HW is less prone to errors and does not significantly hurt speed.
    int bank_sel_a  = get_ptr_field(gcr_addr, rx_bank_sel_a);
    // PSL set_cal_bank
    t_bank cal_bank = (bank_sel_a == 0) ? bank_a : bank_b;

    // Wait for the lane to be powered up (controlled by DL)
    int phy_dl_rx_psave_sts = 1;

    // PSL rx_psave_sts_outer
    while (phy_dl_rx_psave_sts)
    {
        phy_dl_rx_psave_sts = get_ptr_field(gcr_addr, rx_psave_sts_phy);

        // PSL rx_psave_sts_inner
        if (phy_dl_rx_psave_sts)
        {
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }
    }

    // Power up the ALT bank
    set_debug_state(0x3004); // DEBUG - Recal Alt Bank Power Up
    alt_bank_psave_clear_and_wait(gcr_addr);

    // HW532652: The psave logic sets bit_lock_done, so clear it on the ALT bank (leave it set on main bank)
    // PSL bank_a_bit_lock_done
    if (cal_bank == bank_a)
    {
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_a_clr, 0b1, fast_write);
    }
    else     //bank_b
    {
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_b_clr, 0b1, fast_write);
    }

    // reset rx io domain  - HW504112
    put_ptr_field(gcr_addr, rx_iodom_ioreset,        0b1, read_modify_write); //pl  reset rx io domain
    put_ptr_field(gcr_addr, rx_iodom_ioreset,        0b0, read_modify_write); //pl  reset rx io domain

    // Select the cal lane after powering up the ALT bank
    clear_all_cal_lane_sel(
        gcr_addr); // HW507890: Broadcast write rx_clr_cal_lane_sel to briefly enable all clocks for data pipe latches to clear them when switching cal lane.
    put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write); // strobe bit
    put_ptr_field(gcr_addr, rx_cal_lane_pg_phy_gcrmsg, lane, read_modify_write);

    io_sleep(get_gcr_addr_thread(gcr_addr));

    // Remove any applied static offset. If this is not set before initial training it could get tied up in hysteresis and act strangely.
    int offset_applied = (cal_bank == bank_a) ? mem_pl_field_get(ppe_pr_offset_applied_a,
                         lane) : mem_pl_field_get(ppe_pr_offset_applied_b, lane);

    // PSL offset_applied
    if ( offset_applied )
    {
        // Load ****both**** data and edge values on read. Assumes in same reg address in data + edge order
        uint32_t bank_pr_save[2];
        int pr_active[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}

        // PSL offset_applied_bank_a
        if (cal_bank == bank_a)
        {
            bank_pr_save[0] = get_ptr(gcr_addr, rx_a_pr_ns_data_addr,  rx_a_pr_ns_data_startbit, rx_a_pr_ns_edge_endbit);
            bank_pr_save[1] = get_ptr(gcr_addr, rx_a_pr_ew_data_addr,  rx_a_pr_ew_data_startbit, rx_a_pr_ew_edge_endbit);
        }
        else
        {
            bank_pr_save[0] = get_ptr(gcr_addr, rx_b_pr_ns_data_addr,  rx_b_pr_ns_data_startbit, rx_b_pr_ns_edge_endbit);
            bank_pr_save[1] = get_ptr(gcr_addr, rx_b_pr_ew_data_addr,  rx_b_pr_ew_data_startbit, rx_b_pr_ew_edge_endbit);
        }

        pr_active[prDns_i] = prmask_Dns(bank_pr_save[0]);
        pr_active[prEns_i] = prmask_Ens(bank_pr_save[0]);
        pr_active[prDew_i] = prmask_Dew(bank_pr_save[1]);
        pr_active[prEew_i] = prmask_Eew(bank_pr_save[1]);

        uint32_t prDsave[2];
        prDsave[0] = prmask_Dns(bank_pr_save[0]);
        prDsave[1] = prmask_Dew(bank_pr_save[1]);
        uint32_t prEsave[2];
        prEsave[0] = prmask_Ens(bank_pr_save[0]);
        prEsave[1] = prmask_Eew(bank_pr_save[1]);

        int pr_offset_static_d = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_d_override), ppe_pr_offset_d_override_width);
        int pr_offset_static_e = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_e_override), ppe_pr_offset_e_override_width);
        int pr_offset_vec_d[2] = {0 - pr_offset_static_d, 0 - pr_offset_static_d};
        int pr_offset_neg_e = 0 - pr_offset_static_e;

        pr_recenter(gcr_addr, cal_bank, pr_active, prEsave, prDsave, pr_offset_vec_d, pr_offset_neg_e);

        if (cal_bank == bank_a)
        {
            mem_pl_bit_clr(ppe_pr_offset_applied_a, lane);
        }
        else
        {
            mem_pl_bit_clr(ppe_pr_offset_applied_b, lane);
        }

        io_sleep(get_gcr_addr_thread(gcr_addr));
    } //remove offset

    // See if this is the very first recal on un-initialized bank B.
    // Check both recal_cnt and min_recal_cnt_reached in case recal_cnt was cleared (or rolled over).
    // If this is the very first recal, run it as init so that Bank B gets one pass through init sequence.
    unsigned int recal_cnt = mem_pl_field_get(rx_lane_recal_cnt, lane);
    int min_recal_cnt_reached = mem_pl_field_get(rx_min_recal_cnt_reached, lane);
    bool first_recal = (recal_cnt == 0) && (!min_recal_cnt_reached);
    bool recal = !first_recal;

    // Cal Step: VGA (Gain & Path Offset) - Run in all modes
    // Does not require edge tracking or bank alignment
    int saved_Amax_poff = 0;
    int vga_enable = mem_pg_field_get(rx_rc_enable_vga_cal);

    if (vga_enable && (status == rc_no_error))
    {
        bool copy_gain_to_b = false;
        bool copy_gain_to_b_loop = false;
        int  saved_Amax = 0;
        bool gain_changed = false;
        bool first_loop_iteration =
            true; //Used in VGA logic for init hardware so we want run into dither issue, hardcode for recal
        // PSL eo_vga
        status |= eo_vga(gcr_addr, cal_bank, &saved_Amax, &saved_Amax_poff, &gain_changed, recal, copy_gain_to_b,
                         copy_gain_to_b_loop, first_loop_iteration);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Cal Step: Path Offset (Ap+An/2) - Only run in PCIe Gen1/2/3
    int poff_enable = (pcie_gen1_2_cal || pcie_gen3_cal) ? mem_pg_field_get(rx_rc_enable_edge_offset_cal) : 0;

    if (poff_enable && (status == rc_no_error))
    {
        // PSL eo_vga_pathoffset
        status |= eo_vga_pathoffset(gcr_addr, cal_bank, saved_Amax_poff, recal, first_recal, pcie_gen1_cal, pcie_gen2_cal,
                                    pcie_gen3_cal);
    }

    // Enable both Bank CDRs in Local Mode and wait for lock on both banks
    if (status == rc_no_error)
    {
        set_debug_state(0x300F); // DEBUG - Recal Enable Edge Tracking
        // PSL put_rx_pr_edge_track_cntl
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);
        bool set_fir_on_error = false;
        wait_for_cdr_lock(gcr_addr, set_fir_on_error);
    }

    // Cal Step: Edge Offset (Live Data) - Only run in AXO, PCIe Gen4, PCIe Gen5
    // Requires edge tracking (local data mode) but does not require bank alignment
    int eoff_enable = (pcie_gen1_2_cal || pcie_gen3_cal) ? 0 : mem_pg_field_get(rx_rc_enable_edge_offset_cal);

    if (eoff_enable && (status == rc_no_error))
    {
        // PSL eo_eoff
        status |= eo_eoff(gcr_addr, recal, first_recal, cal_bank);//vga_loop_count=1 no latch offset check
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Cal Step: Quad Phase Adjust (Edge NS to EW phase adjustment) - Only run in AXO, PCIe Gen4, PCIe Gen5
    // Requires edge tracking (local data mode) but does not require bank alignment
    int quad_enable = (pcie_gen1_2_cal || pcie_gen3_cal) ? 0 : mem_pg_field_get(rx_rc_enable_quad_phase_cal);

    if (quad_enable && (status == rc_no_error))
    {
        bool quad_adjust_changed = false;
        // PSL eo_qpa
        status |= eo_qpa(gcr_addr, cal_bank, recal, &quad_adjust_changed);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Cal Step: CTLE (Peaking) - Enabled for AXO (Peak1/2), Gen3 (Peak2), Gen4 (Peak2), Gen5 (Peak1/2)
    // Requires edge tracking (local data mode) but does not require bank alignment
    bool peak_changed = false;
    int ctle_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_rc_enable_ctle_peak_cal);

    if (ctle_enable && (status == rc_no_error))
    {
        bool ctle_recal = true;
        bool ctle_hyst_en = min_recal_cnt_reached;
        bool copy_peak_to_main = false;
        bool start_at_zero = false;
        // PSL eo_ctle
        status |= eo_ctle(gcr_addr, cal_mode, cal_bank, copy_peak_to_main, start_at_zero, ctle_recal, ctle_hyst_en,
                          &peak_changed);
    }

    // Perform Bank A/B UI Alignment by putting Alt bank into align mode. Requires edge tracking.
    int bank_sync_enable = mem_pg_field_get(rx_rc_enable_bank_sync);

    if (bank_sync_enable && (status == rc_no_error))
    {
        set_debug_state(0x3010); // DEBUG - Recal Bank Sync

        // PSL enable_bank_sync_cal_bank_a
        if (cal_bank == bank_a)
        {
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_align_cdr_b_lcl, fast_write);
        }
        else     //bank_b
        {
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl_align, fast_write);
        }

        // Max Sync Time: Gen5 = 500ns, Gen4 = 1us, Gen3 = 2us, Gen2 = 4us, Gen1 = 8us
        io_wait_us(get_gcr_addr_thread(gcr_addr), 8); //sleep

        // Check for abort during bank sync
        status |= check_rx_abort(gcr_addr);
    }

    // Cal Step: LTE - Disabled for all rates/modes in Recal by default. Read mem_reg enable instead of hardcoding 0 to include code for lab/sim.
    // Requires edge tracking (local data mode) but does not require bank alignment
    int lte_enable = 0; //mem_pg_field_get(rx_rc_enable_lte_cal);

    if (lte_enable && (status == rc_no_error))
    {
        bool lte_changed = false;
        bool lte_recal = true;
        bool copy_lte_to_b = false;
        // Impossible to run PSL eo_lte
        status |= eo_lte(gcr_addr, cal_bank, copy_lte_to_b, lte_recal, &lte_changed);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Disable bank sync mode before running DFE/DDC since alter the data
    // Put Alt (Cal) bank into CDR External mode (if no errors) for  DFE Amp Measurements and DDC
    if (status == rc_no_error)
    {
        // PSL disable_bank_sync_cal_bank_a
        if (cal_bank == bank_a)
        {
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_ext_cdr_b_lcl, fast_write);
        }
        else     //bank_b
        {
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_ext, fast_write);
        }
    }
    else     //!rc_no_error
    {
        // Disable bank sync, but do not enable external data mode on an error
        // PSL rc_error_rx_pr_edge_track_cntl
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);
    }

    // Cal Step: DFE Full - Only run in AXO or PCIe Gen3/4/5
    // Requires edge tracking (external data mode) and bank alignment
    int dfe_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_rc_enable_dfe_cal);

    if (dfe_enable && (status == rc_no_error))
    {
        // Enable the min eye height checks if past the first set of recals on Bank B or on the last iteration of that first set of recals
        bool enable_min_eye_height = ( min_recal_cnt_reached || (mem_pg_field_get(rx_min_recal_cnt) == (recal_cnt + 1)) );
        // On very first recal on B, run all 4 quads; also run all 4 if peak changes.
        bool run_all_quads = first_recal || peak_changed;
        // Run without hysteresis until reach recal count where enable it
        bool dfe_hyst_en = mem_pl_field_get(rx_dfe_hyst_enabled, lane);

        if ( !dfe_hyst_en && (mem_pg_field_get(rx_dfe_hyst_min_recal_cnt) == recal_cnt) )
        {
            dfe_hyst_en = true;
            mem_pl_bit_set(rx_dfe_hyst_enabled, lane);
        }

        // PSL rx_eo_dfe_full
        status |= rx_eo_dfe_full(gcr_addr, cal_bank, run_all_quads, dfe_hyst_en, enable_min_eye_height);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Cal Step: DDC - Only run in AXO or PCIe Gen3/4/5
    // Requires edge tracking (external data mode) and bank alignment
    int ddc_enable = (pcie_gen1_2_cal) ? 0 : mem_pg_field_get(rx_rc_enable_ddc);

    if (ddc_enable && (status == rc_no_error))
    {
        bool recal_dac_changed =
            false; //TIE: This is a placeholder. The default behavior is NOT to change the DDC JUST because a DAC changed.
        // PSL eo_ddc
        status |= eo_ddc(gcr_addr, cal_bank, recal, recal_dac_changed);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    }

    // Re-enable CDR local data mode on both banks
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl, fast_write);

    // Clear cal lane sel
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);

    // If recal was not aborted, Update recal count and check if the min recal count has been reached
    status |= check_rx_abort(gcr_addr);

    if ( !(status & (abort_error_code | abort_clean_code)) )
    {
        recal_cnt = recal_cnt + 1;
        mem_pl_field_put(rx_lane_recal_cnt, lane, recal_cnt);

        if (!min_recal_cnt_reached)
        {
            unsigned int min_recal_cnt = mem_pg_field_get(rx_min_recal_cnt);

            if (recal_cnt >= min_recal_cnt)
            {
                mem_pl_bit_set(rx_min_recal_cnt_reached, lane);
                min_recal_cnt_reached = 1;
            }
        }
    }

    // Perform Check of VGA, CTLE, LTE, and QPA values
    // Check pass/fail status of other steps, too
    // part of bist
    if (min_recal_cnt_reached)
    {
        int bist_check_en = get_ptr_field(gcr_addr, rx_check_en_alias);

        if ( bist_check_en )
        {
            if (bist_check_en & rx_eoff_check_en_mask) //must be before vclq so set fail_flag
            {
                // PSL eo_rxbist_epoff_final
                eo_rxbist_epoff_final(gcr_addr, cal_bank);//edge only
            }

            // PSL eo_vclq_checks
            eo_vclq_checks(gcr_addr, cal_bank, bist_check_en);
        } //bist_check_in
    }//min_recal_cnt_reached

    // In Recal, only switch banks at the end of a lane's recal when no abort or error and if reached the min lane recal count
    if (status == rc_no_error)
    {

        if (min_recal_cnt_reached)
        {
            // Add PR Offset
            bool pr_offset_pause = (0 != mem_pg_field_get(ppe_pr_offset_pause));

            // PSL min_recal_cnt_pr_offset_pause
            if ( !pr_offset_pause )
            {
                // Load ****both**** data and edge values on read. Assumes in same reg address in data + edge order
                uint32_t bank_pr_save[2];
                int pr_active[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}

                // PSL min_recal_cnt_cal_bank_a
                if (cal_bank == bank_a)
                {
                    bank_pr_save[0] = get_ptr(gcr_addr, rx_a_pr_ns_data_addr,  rx_a_pr_ns_data_startbit, rx_a_pr_ns_edge_endbit);
                    bank_pr_save[1] = get_ptr(gcr_addr, rx_a_pr_ew_data_addr,  rx_a_pr_ew_data_startbit, rx_a_pr_ew_edge_endbit);
                }
                else
                {
                    bank_pr_save[0] = get_ptr(gcr_addr, rx_b_pr_ns_data_addr,  rx_b_pr_ns_data_startbit, rx_b_pr_ns_edge_endbit);
                    bank_pr_save[1] = get_ptr(gcr_addr, rx_b_pr_ew_data_addr,  rx_b_pr_ew_data_startbit, rx_b_pr_ew_edge_endbit);
                }

                pr_active[prDns_i] = prmask_Dns(bank_pr_save[0]);
                pr_active[prEns_i] = prmask_Ens(bank_pr_save[0]);
                pr_active[prDew_i] = prmask_Dew(bank_pr_save[1]);
                pr_active[prEew_i] = prmask_Eew(bank_pr_save[1]);

                uint32_t prDsave[2];
                prDsave[0] = prmask_Dns(bank_pr_save[0]);
                prDsave[1] = prmask_Dew(bank_pr_save[1]);
                uint32_t prEsave[2];
                prEsave[0] = prmask_Ens(bank_pr_save[0]);
                prEsave[1] = prmask_Eew(bank_pr_save[1]);

                int pr_offset_static_d = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_d_override), ppe_pr_offset_d_override_width);
                int pr_offset_static_e = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_e_override), ppe_pr_offset_e_override_width);
                int pr_offset_vec_d[2] = {pr_offset_static_d, pr_offset_static_d};

                pr_recenter(gcr_addr, cal_bank, pr_active, prEsave, prDsave, pr_offset_vec_d, pr_offset_static_e);

                if (cal_bank == bank_a)
                {
                    mem_pl_bit_set(ppe_pr_offset_applied_a, lane);
                }
                else
                {
                    mem_pl_bit_set(ppe_pr_offset_applied_b, lane);
                }
            }

            // Done: Add PR offset

            // Cal Done:  Double check lock
            set_debug_state(0x3015); // DEBUG - Recal Final Edge Tracking and Bank Switch
            bool set_fir_on_error = true;
            wait_for_cdr_lock(gcr_addr, set_fir_on_error);

            // HW532652: Turn off invalid lock detection to help with DL bumping
            put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);

            // Switch banks
            cal_bank = switch_cal_bank(gcr_addr, cal_bank);
        } //!min_recal_cnt_reached

    }
    else     // status!=rc_no_error - some status needs to be handled
    {
        // Recal Abort
        if ( (status & (abort_error_code | abort_clean_code)) )
        {
            // DO NOTHING
            //check_rx_abort sets the FIR as needed set_fir(fir_code_recal_abort);
        }

        // Warning FIR and Bad Lane status on Warning/Error
        if ( (status & (warning_code | error_code)) )
        {
            set_rx_lane_fail(lane);

            // PSL set_fir_bad_lane_warning
            set_fir(fir_code_bad_lane_warning);
        }
    } //if(status==rc_no_error)


    // Power Down the Alternate Bank (new cal_bank)
    int disable_bank_powerdown = mem_pg_field_get(rx_disable_bank_pdwn);

    // PSL disable_bank_powerdown
    if (!disable_bank_powerdown)
    {
        set_debug_state(0x301A, 3); // DEBUG - Recal Alt Bank Power Down
        put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
    }

    return status;
} //eo_main_recal_rx


////////////////
//// TX Recal //
////////////////
//static int eo_main_recal_tx(t_gcr_addr *gcr_addr) {
//  int status = rc_no_error;
//  set_gcr_addr_reg_id(gcr_addr, tx_group); // set to tx gcr address
//
//  // Wait for the lane to be powered up (controlled by DL)
//  int lane = get_gcr_addr_lane(gcr_addr);
//  uint32_t lane_mask = (0x80000000 >> lane);
//  int phy_dl_tx_psave_sts = 1;
//  while (phy_dl_tx_psave_sts) {
//    int phy_dl_tx_psave_sts_0_23 =
//      (get_ptr_field(gcr_addr, tx_psave_sts_phy_0_15_sts)  << 16) |
//      (get_ptr_field(gcr_addr, tx_psave_sts_phy_16_23_sts) << (16 - tx_psave_sts_phy_16_23_sts_width));
//    phy_dl_tx_psave_sts = phy_dl_tx_psave_sts_0_23 & lane_mask;
//  }
//
//  // Cal Step: TX DCC
//  int tx_dcc_enable = mem_pg_field_get(tx_rc_enable_dcc);
//  if (tx_dcc_enable && (status == rc_no_error)) {
//    status |= tx_dcc_main_adjust(gcr_addr);
//  }
//
//  set_gcr_addr_reg_id(gcr_addr, rx_group); // set back to rx gcr address
//  return status;
//} //eo_main_recal_tx
