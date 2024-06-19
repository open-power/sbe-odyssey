/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_pipe_ifc.c $  */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_pipe_ifc.c
// *! TITLE       :
// *! DESCRIPTION : Functions for handling the PIPE Interface (PCIe DL)
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23090700 |vbr     | Remove PIPE RX Margining to free up PPE SRAM space (~3.6KB).
// jjb23082800 |jjb     | Issue 309272: Updated to use rx_group address prior to writing rx_off_dm.
// jjb23082500 |jjb     | Issue 309272: Save and resetore rx_off_dm during pipe_reset_active processing to maintain sigdet calibrated value in pcie mode.
// vbr23062300 |vbr     | EWM 307543: Have CDR and DL clock disabled when doing a flywheel reset to avoid clock glitch from CDR async violations
// jjb23060200 |jjb     | Issue 305073: updated references to tx_pcie_idle_loz_del_sel_alias to tx_pcie_idle_loz_del_sel_pl_alias
// vbr23060800 |vbr     | EWM 306611: Combined reads, added sleep to powerdown handler.
// jjb23060200 |jjb     | Issue 306014: Added checking for pending rate change and power state change prior to asserting rxdatavalid in 0xFC3B processing
// jjb23060100 |jjb     | Issue 306014: removed clearing rx_init_done in reset_inactive since redundant.
// jjb23060100 |jjb     | Issue 306014: added reseting flywheel prior checking for cdr lock in rxelecidle_inactive non run_ininitial_training cases.
// jjb23060100 |jjb     | Issue 306014: added pipe_preset_rx to entering P1 and P2.
// jjb23053100 |jjb     | Issue 306014: gate executing run_initial_training during rxelecidle_inactive with rx_init_done. Clear rx_init_done on reset_active, rate_change, powerdown to L1 or L2.
// jjb23051800 |jjb     | Issue 304568: Updated lane group to TX prior pipe_resetn_active checking in reset_inactive function.
// jjb23051000 |jjb     | Issue 304568: Clear pipe_state_reset_inactive at end of each of it's function phases if pipe_resetn_active is set.
// jjb23051600 |jjb     | Issue 305073: update rate change to update tx_pcie_idle_loz_del_sel_pl_alias instead of tx_pcie_idle_loz_del_sel_alias
// vbr23050100 |vbr     | EWM 303579: Consolidate bank switching, flywheel resetting from ei_inactive into eo_main to save time
// vbr23042500 |vbr     | EWM 303525/302758: Split reset active into 2 phases, reset inactive into 4 phases (from 2)
// jjb23050200 |jjb     | Issue 304124: Moved rxelecidle_en prior to phystatus pulse in pclkchangeack_active
// vbr23040300 |vbr     | Issue 301953: Split initial training into 3 phases (from 2) on EI exit
// jjb23040300 |jjb     | Issue 302460: Added timeout loop in PMB busy checking functions
// jjb23032700 |jjb     | Issue 302010: fixed lane offset multiplier in timer and dac mem reg restore read
// jjb23032400 |jjb     | Issue 302010: updated to read mem regs for tdr_dac_cntl value during txdetectrx
// jjb23032300 |jjb     | Issue 301288: updated masking for txdetrx timer mem regs restore
// jjb23032200 |jjb     | Issue 301288: txdetrx timers now loaded from ppe mem regs
// vbr23032000 |vbr     | EWM 301548: Move sleep within reset phase1 for active time fail.
// vbr23031600 |vbr     | EWM 301312: Increased RxEqEval thread active limit from 45us to 52us
// jjb23031300 |jjb     | Issue 301192: Moved io_sleep earlier in pipe_clock_change to fix thread timeout.
// gap23030700 |gap     | Issue 300655: change detrx controls from PG to PL for Pll
// jjb23030200 |jjb     | Issue 300481: Remove wait on txdeemph writes on rate changes
// vbr23030200 |vbr     | Issue 300456: Removed many wait for cdr locks in RxMargining since bit_lock_done is likely set; can only check bank where it is cleared.
// jjb23030100 |jjb     | Issue 300241: Increased rxeqeval thread timeout to 45us
// jjb23021700 |jjb     | Issue 299567: Updates for rate change txdeemph writes
// jjb23021300 |jjb     | Issue 299292: Added pipe_cmd_pclkchangeack_active
// jjb23021000 |jjb     | Issue 299113: added io_sleeps to address multi lane link pipe sequence deadlocks
// vbr23020300 |vbr     | Issue 298086: remove edge pr offset on rate change or rx eq eval
// jjb23020100 |jjb     | Issue 298467: Made reset_active and txdetect io_waits conditional on ppe_sim_speedup.
// jjb22013000 |jjb     | Issue 295572: resetn_active update to clear reset_active prior to clearing ppe requests
// jjb22121300 |jjb     | Issue 295572: RXDATAVALID and RXACTIVE are not cleared on entrance to P0s or P0.
// vbr23011300 |vbr     | Masked rxstandby_active in the command handler since there is no longer a handler for it.
// vbr23010400 |vbr     | Fixed end_addr passed into rxmargin_loffad_check().
// jjb23010500 |jjb     | Issue 296994: Split txdetectrx into 2 phases to avoid starving other lanes in the thread.
// vbr23010300 |vbr     | Set psave_cdrlock_mode before powering up alt bank in rxelecidle_inactive
// jjb22121200 |jjb     | merge 288424 with main clean up
// jjb22110700 |jjb     | added get_rxmargin_sample_count to reduce code space
// jjb22110700 |jjb     | added debug state comments to add understanding during grep
// jjb22110700 |jjb     | updated rxmargin_minipr_check to reduce code space.
// jjb22101100 |jjb     | Issue 288424: Removed pcie bist gating since pcie bist no longer calls pipe functions.
// jjb22082600 |jjb     | Issue 288424: Support fw_bist_en operation mode where pipe interface is fenced off.
// vbr22120300 |vbr     | Removed group power down/up from reset_active
// vbr22120200 |vbr     | Split rxelecidle_inactive into two phases
// vbr22120100 |vbr     | Split reset_inactive into two phases, moved some reset_active/inactive stuff around.
// vbr22113000 |vbr     | Issue 295175: Added PIPE lane stress mode
// vbr22111501 |vbr     | Redeuced sleeps in pipe_preset_rx in rate_change and rxeqeval. Use spin instead of wait in rxeqeval.
// jjb22111000 |jjb     | Removed rxstandby_active handler as it is now done with hardware.
// vbr22110800 |vbr     | Remove alt bank power down (init training always does this now)
// jjb22101800 |jjb     | Issue 292521: P0s does not clear all pending pipe requests.
// jjb22101700 |jjb     | Issue 292521: P0s only affects TX.
// jjb22092800 |jjb     | Issue 290210: Fixed txdetectrx check for Gen1 and P1 state
// jjb22092700 |jjb     | Issue 290210: Added pipe_state_txdetectrx_status_set/clear
// jjb22092600 |jjb     | Issue 290210: Updated code to remove RMW hazards with pipe interface logic updates
// mbs22082601 |mbs     | Updated with PSL comments
// jjb22081900 |jjb     | Issue 287283: set pipe_state_rxelecidle_en high at end of resetn_inactive to allow exit of EI detection to occur after reset, disable pipe_state_rxelecidle_en at completion of rxelecidle_inactive
// jjb22080400 |jjb     | Issue 284576: Disable tx segment update for gen1 and gen2 initial MAC based txdeemph updates as these are not actual coeffs
// vbr22071900 |vbr     | Issue 284981: Shuffled sleeps around pipe_preset_rx for RX EQ Eval and Rate Change
// jjb22071500 |jjb     | Added io_wait prior to P2M PMB FOM WRC to meet 1.6us rx data valid prior to FOM WRC requirement
// jjb22071300 |jjb     | Removed unnecessary group address changes in rx_eq_eval
// jjb22071100 |jjb     | Gated rx dl clock and rx data during rx eq eval calibrations
// jjb22071200 |jjb     | Reduced code size for rx margining by 1056 bytes using common sub functions
// jjb22062400 |jjb     | Issue 283596: Check for pipe_state_resetn_active in rate_changed and txdetectrx prior to setting FIR
// gap22062200 |gap     | Repartitioned pipe_clock_change, adding clock_change_tx to ioo_init_and_reset
// gap22062200 |gap     | Moved per-gen fifo l2u settings to tx_fifo_init
// jjb22062000 |jjb     | Issue 281062: Clear pipe request prior to final WRC to avoid erroniously clearing pending back to back PMB initiated commands
// jjb22062000 |jjb     | Issue 281062: Clear pipe request prior to final WRC to avoid erroniously clearing pending back to back PMB initiated commands
// jjb22061600 |jjb     | Issue 280716: Added io_lane_power_on_tx to pipe_resetn_active to allow EI to propagate
// jjb22061600 |jjb     | Issue 280716: Added io_lane_power_on_tx to pipe_resetn_active to allow EI to propagate
// jjb22060800 |jjb     | Issue 281175: Added io_sleep prior to run_initial_training calls
// jjb22052600 |jjb     | Issue 281175: Added io_sleep to pipe_cmd_txdetectrx polling
// jjb22052600 |jjb     | Issue 281175: Added io_sleep to rate_updated to resolve ppe thread timeout
// jjb22060300 |jjb     | Issue 281092: Simplfied cdr disable code
// jjb22052600 |jjb     | Issue 281092: Disable cdr and reset flywheel during rx clock gating
// jjb22050900 |jjb     | Issue 270922: Disable rx_io_clock and rx_dl_clock during rate changes
// gap22042000 |gap     | Issue 277497: Correct polarity of IOOCC_TX_RXDET_COMP.v model
// jjb22042000 |jjb     | Issue 258017: Update rx analog control regs on rate changes
// jjb22040700 |jjb     | Issue 270919: Enable tx_eol_mode_disable while changing tx_pcie_clk_sel
// jjb22040500 |jjb     | Issue 270919: Enable tx_bank_control(tx_fifo_pdwn - bit 5) while changing tx_pcie_clk_sel
// jjb22040400 |jjb     | Abort l_rx_mini_pr_step_a_done loop if pipe_state_resetn_active is high
// jjb22040400 |jjb     | Only assert phystatus_pulse in txdetectrx and powerdown if pipe_state_resetn_active is low
// jjb22040400 |jjb     | Issue 276484: abort while loops in data rate change if pipe_state_resetn_active is set
// jjb22032900 |jjb     | Issue 267085: Add 120us wait prior to pulsing phystatus at end of txdetectrx
// jjb22030700 |jjb     | Issue 273939: Disable CDR prior to resetting CDR flywheel in rxelecidle_inactive function
// jjb22030300 |jjb     | Issue 266708: Save last FOM value per lane for debug
// jjb22030300 |jjb     | Issue 267346: Restore LTE, CTLE settings for RESETN deassertion for GEN1.
// bja22012800 |bja     | Use tx_idle_ovr_alias in pipe_cmd_rate_updated
// bja22012500 |bja     | Issue 266438: Set tx_fifo_l2u_dly based on pipe rate
// jjb22020900 |jjb     | Issue 270871: updated reading pipe preset values to elliminate unneeded masking and shifting.
// jjb22020800 |jjb     | Issue 269696: Move bank switch logic from rate change to rxelecidle inactive
// jjb22012400 |jjb     | Issue 266318: clear rx_ber_pcie_mode when stopping rx margining.
// vbr21111700 |vbr     | Issue 257157: bump CDR during bank switch to ensure banks are not 180deg out of phase
// jjb21112300 |jjb     | Issue 259325: rxelecidle inactive updated to use correct addressing groups
// jjb21111600 |jjb     | Updated rx margining for timing to validate request timing steps against available mini PR steps and to move both data and edge mini PRs to achieve maximum range
// jjb21111600 |jjb     | updated rx margining stop to compress restoring original lane and switching t tx_group into single command
// vbr21111600 |vbr     | Issue 256712: Reset the flywheel when exiting electrical idle
// jjb21111500 |jjb     | used broadcast read to accomplish OR of ber_en check in determination of any lanes running margining as per review comments.
// jjb21111500 |jjb     | replaced switch_cal_bank with set_cal_bank in rx margining as per review comments.
// jjb21110900 |jjb     | Updated rx margining sample count conversion calculation pushing gen4/5 count control into group sample counter scaling
// jjb21110900 |jjb     | Added checks on rx margining sample count write back to make non zero if was zero and not reset to handle case of no group level sample counter overflow yet.
// jjb21110100 |jjb     | Added rx margining disable if offset is NAKed.
// jjb21102900 |jjb     | Added check to rx stop margining to check all other lanes in group to potential disable group level sample counter
// jjb21102500 |jjb     | Updated rxmargin_sample_count_update to handle gen4 vs gen5 sample count conversion properly
// jjb21101900 |jjb     | Added rxmargin sample and error count reset functions
// jjb21101800 |jjb     | Added rxmargin_bank_enable function, rxmargin_sample_count_conv
// vbr21102700 |vbr     | Added call for pre-setting Peak/LTE
// vbr21101900 |vbr     | Add calls for restoring TX DCC and RX LOFF
// jjb21100100 |jjb     | Updated rx margining start procedure
// vbr21092200 |vbr     | Use wo pulse for psave_req_alt
// jjb21091000 |jjb     | Added pipe_state_rxelecidle_en support
// jjb21091000 |jjb     | Added temporary rx margining sequence code
// jjb21090700 |jjb     | Updated Lx power state references to Px power state references.
// jjb21090700 |jjb     | removed P1 Gen1 check during reset.
// jjb21090700 |jjb     | updated rate change and powerdown change to seperate rxactive recal into seperate request
// jjb21082700 |jjb     | Updated resetn_inactive processing to clear resetn_active when clearing resetn_inactive
// vbr21081200 |vbr     | Rearrranged TX FIFO config writes
// jjb21081600 |jjb     | Updated resetn_inactive to put pipe request mask just prior to phystatus clear
// jjb21081600 |jjb     | Fixed rxelecidle_inavtive to set rxactive high if previous not set after run_calibration completes
// gap21062900 |gap     | Added product code for pipe_cmd_txdetectrx
// jjb21072000 |jjb     | Updated power on to use cmd_dccal_pl
// jjb21070800 |jjb     | Updated RXSTANDBY to clear RXDATAVALID
// jjb21070600 |jjb     | Added txdeemph_updated for reset and rate changes
// jjb21062900 |jjb     | Updated txdetectrx to check for P1 and 2.5GTs else FIR and not detected.
// jjb21062100 |jjb     | Removed RXACTIVE, RXSTANDBY functions as these are handled in rate_changed function.
// vbr21061800 |vbr     | Force bank A to cal bank on a rate change
// jjb21061700 |jjb     | removed specific pclkchangack processing as this is handled within rate change code
// jjb21061600 |jjb     | removed unneccessary set_gcr_addr_lane and fixed gen1/2 rate compare
// jjb21061500 |jjb     | Added txfifo initialization register writes prior to issuing any tx fifo init
// jjb21061000 |jjb     | Added pipe powerdown change code
// jjb21060700 |jjb     | Added pipe data rate change code
// jjb21060200 |jjb     | removed redundant register bit defines, pipe_put_blk now calls pipe_put, updated reset inactive rate and powerdown checks
// jjb21052400 |jjb     | Added pipe_put_blk for write committed as this must wait for write acknowledge to occur to make forward progress.
// jjb21051700 |jjb     | Updated for PIPE v5.1.1. RXEqTraining only for USB in v5.1.1.
// gap21040700 |gap     | Added stub code for pipe_cmd_txdetectrx
// vbr21040700 |vbr     | Added mask and disable handlers for txelecidle_* per Gary P.
// vbr21040500 |vbr     | Updated base access function name/prototype
// vbr21040200 |vbr     | Fixed command table indexing, pipe registers are in tx_group
// vbr21040100 |vbr     | First pass at FOM calculation
// vbr21033100 |vbr     | Implemented PIPE reg access functions, run_command, function prototypes
// vbr21020800 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_pipe_ifc.h"
#include "ioo_common.h"
#include "io_logger.h"
#include "io_init_and_reset.h"
#include "eo_wrappers.h"
#include "eo_main.h"
#include "io_ext_cmd.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

// Ignore pipe_state_rxstandby_active since handled by hardware and cleared in rate_changed and reset
// Ignore txelecidle_in/active since handled by hardware
// Ignore rxelecidle_active since handled by pipe_state_rxactive
// Ignore txdeemph_updated since handled within reset_inactive and rate_changed
#define PIPE_STATE_MASK 0xDF48
PK_STATIC_ASSERT(pipe_state_rxstandby_active_startbit == 2);
PK_STATIC_ASSERT(pipe_state_pclkchangeack_startbit == 3);
PK_STATIC_ASSERT(pipe_state_rxelecidle_active_startbit == 8);
PK_STATIC_ASSERT(pipe_state_txelecidle_active_startbit == 10);
PK_STATIC_ASSERT(pipe_state_txelecidle_inactive_startbit == 11);
PK_STATIC_ASSERT(pipe_state_txdeemph_updated_startbit == 13);
// Bits 14 and 15 do not exist

// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value is 2
#define PIPE_IFC_DBG_LVL 1

///////////////////////////////////
// pipe_clock_change
///////////////////////////////////

void pipe_clock_change(t_gcr_addr* gcr_addr,
                       uint32_t rate_one_hot)                                                                                      // Start pipe_clock_change function
{
    // Disable RX IO Clock, CDR, RX DL Clock and reset flywheel when switching clocks to avoid glitches.                                                   //  Manage RX Clocks
    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                               //  Change Addressing to RX group
    put_ptr_field(gcr_addr, rx_berpl_count_en, 0b0,
                  read_modify_write);                                                                                    //  Disable rx_berpl_count_en to fully disable all io clocks
    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0,
                  read_modify_write);                                                                                         //  Disable rx_dl_clk
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1,
                  fast_write);                                                                                         //  Disable rx_cal_lane_sel (VBR: likely not needed, should already be cleared)
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis,
                  fast_write);                                                              //  Disable CDR
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b11,
                  read_modify_write);                                                                             //  Reset Flywheel
    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,  0b11,
                  read_modify_write);                                                                          //  Assert hold_div_clock to freeze c16 and c32
    put_ptr_field(gcr_addr, rx_pcie_clk_sel, rate_one_hot,
                  read_modify_write);                                                                             //  Change RX clock dividers

    io_sleep(get_gcr_addr_thread(
                 gcr_addr));                                                                                                               //  sleep to stop from causing ppe thread timeout

    update_rx_rate_dependent_analog_ctrl_pl_regs(gcr_addr,
            rate_one_hot);                                                                                  //  Update RX analog controls

    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,  0b00,
                  read_modify_write);                                                                          //  Deassert hold_div_clock to enable c16 and c32
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b00,
                  read_modify_write);                                                                             //  Release flywheel reset

    // Update TX analog clock generator and divider to support requested data rate                                                                         //  Manage TX Clocks
    clock_change_tx(gcr_addr, rate_one_hot);
} // pipe_clock_change                                                                                                                                   // end function

///////////////////////////////////
// rxmargin_snapshot_error_and_sample_counters
///////////////////////////////////

uint32_t get_rxmargin_sample_count(t_gcr_addr*
                                   gcr_addr)                                                                                                 // Start get_rxmargin_sample_count function
{
    put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_snapshot, 0b1,
                  fast_write);                                                                             //  Snapshot sample and error counters
    uint32_t l_rx_berpl_pcie_sample = get_ptr_field(gcr_addr,
                                      rx_berpl_pcie_sample);                                                                       //  Read raw sample count

    // PSL samp_err_timeout_set_sample
    if (l_rx_berpl_pcie_sample ==
        0)                                                                                                                       //  Check to see if sample count is zero
    {
        l_rx_berpl_pcie_sample =
            1;                                                                                                                           //   set sample count to 1 as group sample counter may not have overflowed yet
    }                                                                                                                                                      //  end if

    return l_rx_berpl_pcie_sample;                                                                                                                         //  return rx_berpl_pcie_sample
} // get_rxmargin_sample_count                                                                                                                           // end function

///////////////////////////////////
// rxmargin_bank_enable : sets bank b as main and bank a as cal
///////////////////////////////////

static void rxmargin_bank_enable(t_gcr_addr*
                                 gcr_addr)                                                                                                   // Start rxmargin_bank_enable function
{
    alt_bank_psave_clear_and_wait(
        gcr_addr);                                                                                                               //  Ensure both alt and main banks are powered up
    int bank_sel_a  = get_ptr_field(gcr_addr,
                                    rx_bank_sel_a);                                                                                              //  read rx_bank_sel_a
    t_bank cal_bank = (bank_sel_a == 0) ? bank_a :
                      bank_b;                                                                                                 //  determine cal_bank

    // PSL bank_a
    if (cal_bank ==
        bank_a)                                                                                                                                //  psave logic sets PR bit lock done on disabled bank so must clear on present cal bank
    {
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_a_clr, 0b1,
                      fast_write);                                                                                 //   clear PR bit lock done if bank a is cal bank
    }
    else     //bank_b                                                                                                                                      //  else
    {
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_b_clr, 0b1,
                      fast_write);                                                                                 //   clear PR bit lock done if bank b is cal bank
    }                                                                                                                                                      //  end if

    put_ptr_field(gcr_addr, rx_iodom_ioreset, 0b1,
                  read_modify_write);                                                                                     //  set per lane ioreset to resolve any glitches
    put_ptr_field(gcr_addr, rx_iodom_ioreset, 0b0,
                  read_modify_write);                                                                                     //  clear per lane ioreset

    if (cal_bank ==
        bank_b)                                                                                                                                //  if cal_bank is b then switch cal_bank to a
    {
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl_align,
                      fast_write);                                                       //   Enable both CDRs to track on their local data
        io_sleep(get_gcr_addr_thread(
                     gcr_addr));                                                                                                              //   wait to allow CDRs to settle
        set_cal_bank(gcr_addr,
                     bank_a);                                                                                                                       //   set cal bank to bank a
    }                                                                                                                                                      //  end if, following done for either cal bank value

    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_ext_cdr_b_lcl,
                  fast_write);                                                              //  Enable both CDRs, Bank A CDR uses Bank B data, Bank B CDR uses Bank B data
    bool set_fir_on_error =
        false;                                                                                                                         //  initialize set_fir_on_error
    wait_for_cdr_lock(gcr_addr, cal_bank,
                      set_fir_on_error);                                                                                               //  wait for cdr lock
    put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);
} // rxmargin_bank_enable                                                                                                                                // end function

///////////////////////////////////
// rxmargin_sample_count_update : converts raw rx margining sample count into 3*log2(rx_margining_sample_count) and writes back to rx_margin_status_1 register
///////////////////////////////////

static void rxmargin_sample_count_update(t_gcr_addr* gcr_addr,
        uint32_t l_rx_berpl_pcie_sample)                                                          // Start rx margin sample count update function
{
    uint32_t l_rx_berpl_pcie_sample_log2 =
        0;                                                                                                              //  Create 3log2(sample), initialize l_rx_berpl_pcie_sample_log2 to 0
    uint32_t l_rx_berpl_pcie_sample_mask =
        0x8000;                                                                                                         //  initialize l_rx_berpl_pcie_sample_mask to 0x80000000
    uint32_t l_rx_berpl_pcie_sample_scale =
        41;                                                                                                            //  initialize  l_rx_berpl_pcie_sample_scale to 41
    int i;                                                                                                                                                 //  declare integer i

    for (i = 0; i < 16;
         i++)                                                                                                                               //  for loop to count bits set in l_rx_berpl_pcie_sample
    {
        // PSL rx_berpl_pcie_sample
        if (l_rx_berpl_pcie_sample &
            l_rx_berpl_pcie_sample_mask)                                                                                             //   if bit is set
        {
            l_rx_berpl_pcie_sample_log2 = l_rx_berpl_pcie_sample_log2 + l_rx_berpl_pcie_sample_scale -
                                          i;                                                        //   incremenent l_rx_berpl_pcie_sample_log2 by appropriate log2 value
        }                                                                                                                                                     //   end if

        l_rx_berpl_pcie_sample_mask = l_rx_berpl_pcie_sample_mask >>
                                      1;                                                                                       //   shift bit mask
    }                                                                                                                                                      //  end for loop

    uint32_t l_rx_berpl_pcie_sample_3log2 = ((l_rx_berpl_pcie_sample_log2 << 1) +
                                            l_rx_berpl_pcie_sample_log2);                                            //  multiply by 3 and align
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                               //  set gcr addr to tx group
    pipe_put(gcr_addr, pipe_mac_reg_rx_margin_status_1_reg_sel, pipe_mac_reg_rx_margin_status_1_addr, pipe_reg_cmd_wr_u,
             l_rx_berpl_pcie_sample_3log2);    //  WRU MAC RxMarginStatus1.SampleCount with 3log2(sample)                                                                                                                                                      //
} // rxmargin_sample_count_update                                                                                                                        // end function

///////////////////////////////////
// rxmargin_loffad_check Function : checks bank A data loff + requested offset remain in range
///////////////////////////////////

static inline bool rxmargin_loffad_check(t_gcr_addr* gcr_addr, int lane, uint32_t l_dac_addr, uint32_t end_address,
        int check_offset)                     // start rx margin latch offset bank A data range check function
{
    int new_loff;                                                                                                                                           //  declare new_loff integer
    bool result =
        true;                                                                                                                                     //  decalare result boolean and initialize to true

    for (; l_dac_addr <= end_address;
         ++l_dac_addr)                                                                                                          //  loop thru all dac addresses
    {
        new_loff = LatchDacToInt(get_ptr(gcr_addr, l_dac_addr, rx_ad_latch_dac_n000_startbit,
                                         rx_ad_latch_dac_n000_endbit)) + check_offset;                    //   calculate new_loff as original loff + offset

        // PSL loff_oob
        if ((new_loff > 127)
            || (new_loff <
                -127))                                                                                                             //   if new_loff is larger than 127 or less than -127
        {
            result = false; // return false if requested offset exceeds DAC range                                                                                 //    set result as false
        }                                                                                                                                                      //   end if
    }                                                                                                                                                       //  end loop

    return result;                                                                                                                                          //  return result
} // rxmargin_loffad_check                                                                                                                               // end function

///////////////////////////////////
// rxmargin_minipr_check Function : checks bank A mini PR + requested offset remain in range
///////////////////////////////////

static inline bool rxmargin_minipr_check(t_gcr_addr* gcr_addr,
        int check_offset)                                                                         // start rx margin mini PR offset bank A data range check function
{
    bool result =
        true;                                                                                                                                     //  decalare result boolean and initialize to true
    uint32_t l_check_offset_div2 = check_offset /
                                   2;                                                                                                          //  divide check_offset by 2
    uint32_t l_rx_a_pr_ns_data = get_ptr_field(gcr_addr,
                                 rx_a_pr_ns_data);                                                                                  //  get bank a ns data mini pr offset
    uint32_t l_rx_a_pr_ns_edge = get_ptr_field(gcr_addr,
                                 rx_a_pr_ns_edge);                                                                                  //  get bank a ns edge mini pr offset
    uint32_t l_rx_a_pr_ew_data = get_ptr_field(gcr_addr,
                                 rx_a_pr_ew_data);                                                                                  //  get bank a ew data mini pr offset
    uint32_t l_rx_a_pr_ew_edge = get_ptr_field(gcr_addr,
                                 rx_a_pr_ew_edge);                                                                                  //  get bank a ew edge mini pr offset

// PSL minipr_oob
    if (((l_rx_a_pr_ns_data + l_check_offset_div2) > 31)
        ||                                                                                                 //  if bank a ns data + offset/2 > 31 OR
        ((l_rx_a_pr_ns_data + l_check_offset_div2) <  0)
        ||                                                                                                 //     bank a ns data + offset/2 <  0 OR
        ((l_rx_a_pr_ew_data + l_check_offset_div2) > 31)
        ||                                                                                                 //     bank a ew data + offset/2 > 31 OR
        ((l_rx_a_pr_ew_data + l_check_offset_div2) <  0)
        ||                                                                                                 //     bank a ew data + offset/2 <  0 OR
        ((l_rx_a_pr_ns_edge - l_check_offset_div2) > 31)
        ||                                                                                                 //     bank a ns edge - offset/2 > 31 OR
        ((l_rx_a_pr_ns_edge - l_check_offset_div2) <  0)
        ||                                                                                                 //     bank a ns edge - offset/2 <  0 OR
        ((l_rx_a_pr_ew_edge - l_check_offset_div2) > 31)
        ||                                                                                                 //     bank a ew edge - offset/2 > 31 OR
        ((l_rx_a_pr_ew_edge - l_check_offset_div2) <
         0))                                                                                                   //     bank a ew edge - offset/2 <  0
    {
        result = false;                                                                                                                                         //   result is false
    };                                                                                                                                                      //  end if

    return result;                                                                                                                                          //  return result
} // rxmargin_minipr_check                                                                                                                               // end function

////////////////////////////////////
// rx_margin_remove_timing_offset Function : removes rx margin timing offset
////////////////////////////////////
void rx_margin_remove_timing_offset(t_gcr_addr* gcr_addr,
                                    int l_lane)                                                                                    // start rx margin remove timing offset function
{
    int l_rx_margin_timing_offset_int = mem_pl_field_get(rx_margin_timing_offset,
                                        l_lane);                                                                 //  get timing margin offset

    // PSL margin_timing_offset_ne_0
    if (l_rx_margin_timing_offset_int !=
        0)                                                                                                                //  if timing offset is not zero the remove timing offset from PRs
    {
        rxmargin_bank_enable(
            gcr_addr);                                                                                                                       //   set up banks for rx margining. Bank B is main. Bank A is alt.
        put_ptr_field(gcr_addr, rx_mini_pr_step_data_adj, IntToTwosComp(-l_rx_margin_timing_offset_int / 2, 8),
                      read_modify_write);                              //   Set mini PR data adjust to nagative rx margin offset divided by 2
        put_ptr_field(gcr_addr, rx_mini_pr_step_edge_adj, IntToTwosComp(l_rx_margin_timing_offset_int / 2, 8),
                      read_modify_write);                               //   Set mini PR edge adjust to rx margin offset divided by 2
        put_ptr_field(gcr_addr, rx_mini_pr_step_a_run_done_alias, 0b11110000,
                      read_modify_write);                                                             //   Set mini PR step run for Bank A mini PRs
        uint32_t l_pipe_state = get_ptr_field(gcr_addr,
                                              pipe_state_0_15_alias);                                                                               //   Get pipe_state register
        uint32_t l_pipe_state_resetn_active = l_pipe_state &
                                              pipe_state_resetn_active_mask;                                                                   //   Get pipe_state_resetn_active value
        int l_rx_mini_pr_step_a_done =
            0;                                                                                                                     //   initialize l_rx_mini_pr_step_a_done  to 0

        // PSL step_not_done_or_reset
        while((l_rx_mini_pr_step_a_done == 0) & (l_pipe_state_resetn_active ==
                0))                                                                            //   while l_rx_mini_pr_step_a_done is 0 and  pipe_state_resetn_active is 0
        {
            int l_rx_mini_pr_step_a_done_alias = get_ptr_field(gcr_addr,
                                                 rx_mini_pr_step_a_done_alias);                                                          //    read rx_mini_pr_step_a_done_alias

            // PSL mini_pr_step_done
            if (l_rx_mini_pr_step_a_done_alias ==
                15)                                                                                                            //    if all bank a data and edge mini pr stepping are complete
            {
                l_rx_mini_pr_step_a_done =
                    1;                                                                                                                       //     set l_rx_mini_pr_step_a_done when all done
            }                                                                                                                                                    //    end if

            l_pipe_state = get_ptr_field(gcr_addr,
                                         pipe_state_0_15_alias);                                                                                       //    Get pipe_state register
            l_pipe_state_resetn_active = l_pipe_state &
                                         pipe_state_resetn_active_mask;                                                                           //    Get pipe_state_resetn_active value
        }                                                                                                                                                     //   end while

        put_ptr_field(gcr_addr, rx_mini_pr_step_a_run_done_alias, 0,
                      read_modify_write);                                                                      //   clear mini PR step run for Bank A mini PRs
    }                                                                                                                                                      //  end if
} // rx_margin_remove_timing_offset                                                                                                                      // end function

////////////////////////////////////
// rx_margin_apply_timing_offset Function : applies rx margin timing offset
////////////////////////////////////
void rx_margin_apply_timing_offset(t_gcr_addr* gcr_addr, int l_lane,
                                   int l_rx_margin_offset_int)                                                         // start rx margin apply timing offset function
{
    mem_pl_field_put(rx_margin_timing_offset, l_lane,
                     l_rx_margin_offset_int);                                                                             //  save timing margin offset integer value to mem reg for future restore
    rxmargin_bank_enable(
        gcr_addr);                                                                                                                        //  set up banks for rx margining. Bank B is main. Bank A is alt.
    put_ptr_field(gcr_addr, rx_mini_pr_step_data_adj, IntToTwosComp(l_rx_margin_offset_int / 2, 8),
                  read_modify_write);                                       //  Set mini PR data adjust to rx margin offset divided by 2
    put_ptr_field(gcr_addr, rx_mini_pr_step_edge_adj, IntToTwosComp(-l_rx_margin_offset_int / 2, 8),
                  read_modify_write);                                      //  Set mini PR edge adjust to negative rx margin offset divided by 2
    put_ptr_field(gcr_addr, rx_mini_pr_step_a_run_done_alias, 0b11110000,
                  fast_write);                                                                     //  Set mini PR step run for Bank A mini PRs on data and edge
    uint32_t l_pipe_state = get_ptr_field(gcr_addr,
                                          pipe_state_0_15_alias);                                                                                //  Get pipe_state register
    uint32_t l_pipe_state_resetn_active = l_pipe_state &
                                          pipe_state_resetn_active_mask;                                                                    //  Get pipe_state_resetn_active value
    int l_rx_mini_pr_step_a_done =
        0;                                                                                                                      //  initialize l_rx_mini_pr_step_a_done  to 0

    // PSL step_not_done_or_reset
    while((l_rx_mini_pr_step_a_done == 0) & (l_pipe_state_resetn_active ==
            0))                                                                             //  while l_rx_mini_pr_step_a_done is 0 and  pipe_state_resetn_active is 0
    {
        int l_rx_mini_pr_step_a_done_alias = get_ptr_field(gcr_addr,
                                             rx_mini_pr_step_a_done_alias);                                                           //   read rx_mini_pr_step_a_done_alias

        // PSL mini_pr_step_done
        if (l_rx_mini_pr_step_a_done_alias ==
            15)                                                                                                             //   if all bank a data and edge mini pr stepping are complete
        {
            l_rx_mini_pr_step_a_done =
                1;                                                                                                                        //    set l_rx_mini_pr_step_a_done when all done
        }                                                                                                                                                     //   end if

        l_pipe_state = get_ptr_field(gcr_addr,
                                     pipe_state_0_15_alias);                                                                                        //   Get pipe_state register
        l_pipe_state_resetn_active = l_pipe_state &
                                     pipe_state_resetn_active_mask;                                                                            //   Get pipe_state_resetn_active value
    }                                                                                                                                                      //  end while

    put_ptr_field(gcr_addr, rx_mini_pr_step_a_run_done_alias, 0,
                  fast_write);                                                                              //  clear mini PR step run for Bank A mini PRs
} // rx_margin_apply_timing_offset                                                                                                                       // end function

////////////////////////////////////
// rx_margin_clr_samplecount_errorcount_offsetupdated
////////////////////////////////////
void rx_margin_clr_samplecount_errorcount_offsetupdated(t_gcr_addr*
        gcr_addr)                                                                            // start rx margin clear sample_count, error_count, offset_updated function
{
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                               //  set gcr addr to tx group
    put_ptr_field(gcr_addr, pipe_state_cntl4_pl_0_15_alias,
                  (pipe_state_rxmargin_sample_count_reset_clear_mask
                   |                                           //  clear rxmargin_sample_count_reset
                   pipe_state_rxmargin_error_count_reset_clear_mask
                   |                                           //  clear rxmargin_error_count_reset
                   pipe_state_rxmargin_offset_updated_clear_mask) ,
                  fast_write);                                 //  clear rxmargin_offset_updated
} // rx_margin_clr_samplecount_errorcount_offsetupdated                                                                                                  // end function

////////////////////////////////////
// rx_margin_clr_sample_count_and_wru
////////////////////////////////////
void rx_margin_clr_sample_count_and_wru(t_gcr_addr*
                                        gcr_addr)                                                                                            // start rx margin clear sample counter and WRU function
{
    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                               //  set gcr addr to rx group
    put_ptr_field(gcr_addr, rx_ber_reset, 0b1,
                  fast_write);                                                                                                //  Clear PHY sample count LSBs
    put_ptr_field(gcr_addr, rx_berpl_pcie_sample_reset, 0b1,
                  fast_write);                                                                                  //  Clear PHY sample count MSBs
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                               //  set gcr addr to tx group
    pipe_put(gcr_addr, pipe_mac_reg_rx_margin_status_1_reg_sel, pipe_mac_reg_rx_margin_status_1_addr, pipe_reg_cmd_wr_u,
             0);                               //  WRU MAC RxMarginStatus1.SampleCount with 0
} // rx_margin_clr_sample_count_and_wru                                                                                                                                                       // end funtion

///////////////////////////////////
// rx_margin_clr_error_count_and_wru
///////////////////////////////////
void rx_margin_clr_error_count_and_wru(t_gcr_addr*
                                       gcr_addr)                                                                                             // Start rx margin clear error counter and WRU function
{
    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                               //  set gcr addr to rx group
    put_ptr_field(gcr_addr, rx_berpl_pcie_error_reset, 0b1,
                  fast_write);                                                                                   //  Clear error count
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                               //  set gcr addr to tx group
    pipe_put(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_u,
             0);                               //  WRU MAC RxMarginStatus2.ErrorCount with 0
} // rx_margin_clr_error_count_and_wru                                                                                                                   // end function

///////////////////////////////////
// rx_margin_clr_counts_wrc_status1
///////////////////////////////////
void rx_margin_clr_counts_wrc_status1(t_gcr_addr*
                                      gcr_addr)                                                                                               // Start rx margin clr counts and WRC Status 1
{
    rx_margin_clr_samplecount_errorcount_offsetupdated(
        gcr_addr);                                                                                        //   clear rx margin sample_count error_count and offset_updated
    put_ptr_field(gcr_addr, pipe_state_rxmargin_start_clear, 0b1,
                  fast_write);                                                                           //   clear pipe_state_rxmargin_start   , get ready for next command prior to WRC
//   put_ptr_field(gcr_addr, pipe_state_rxmargincontrol_clear, 0b1, fast_write);  // Dont clear rxmargincontrol to allow rx margining timeout polling     //   clear pipe_state_rxmargingcontrol , get ready for next command prior to WRC
    pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_0_reg_sel, pipe_mac_reg_rx_margin_status_0_addr, pipe_reg_cmd_wr_c,
                 0x00000001);                //   WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                             //   set gcr addr to rx group
    put_ptr_field(gcr_addr, rx_berpl_pcie_rx_margin_start, 0b1,
                  read_modify_write);                                                                      //   Start Margining
} // rx_margin_clr_counts_wrc_status1                                                                                                                     // end function

///////////////////////////////////
// rx_margin_clr_counts_wrc_status2
///////////////////////////////////
void rx_margin_clr_counts_wrc_status2(t_gcr_addr*
                                      gcr_addr)                                                                                               // Start rx margin clr counts and WRC Status 2
{
    rx_margin_clr_samplecount_errorcount_offsetupdated(
        gcr_addr);                                                                                        //   clear rx margin sample_count error_count and offset_updated
    put_ptr_field(gcr_addr, pipe_state_rxmargincontrol_clear, 0b1,
                  fast_write);                                                                          //   clear pipe_state_rxmargingcontrol , get ready for next command prior to WRC
    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                             //   set gcr addr to rx group
    put_ptr_field(gcr_addr, rx_ber_pcie_mode, 0b0,
                  read_modify_write);                                                                                   //   clear per group rx_ber_pcie_mode when stopping rx margining
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                             //   set gcr addr to tx group
    pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_0_reg_sel, pipe_mac_reg_rx_margin_status_0_addr, pipe_reg_cmd_wr_c,
                 0x00000002);                //   WRC MAC RxMarginStatus0.MarginNak to 1 (bit 1)
} // rx_margin_clr_counts_wrc_status2                                                                                                                     // end function

///////////////////////////////////
// Register Access Functions
///////////////////////////////////

PK_STATIC_ASSERT(pipe_reg_acc_regsel_cmd_rst_addr_alias_width == 16);
PK_STATIC_ASSERT(pipe_reg_acc_busy_rddata_alias_width == 16);

// Non-blocking PIPE Write (Write Uncommitted)
void pipe_put(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr, uint32_t cmd, uint32_t data)
{
    // Poll until PIPE is not busy or timeout
    int busy = 1;
    int timeout = 8; // make longer than PMB fir timeout = 0.5ns*16*64=512ns

    // PSL busy
    do
    {
        timeout--;
        busy = get_ptr_field(gcr_addr, pipe_reg_acc_busy);
    }
    while (busy && timeout > 0);

    // Write the Data (only field in register, so use fast_write)
    put_ptr_field(gcr_addr, pipe_reg_acc_wrdata, data, fast_write);

    // Write the Command (reset=0)
    uint32_t wr_data = (reg_sel  << pipe_reg_acc_reg_sel_shift) |
                       (reg_addr << pipe_reg_acc_address_shift) |
                       (cmd      << pipe_reg_acc_command_shift);
    put_ptr_field(gcr_addr, pipe_reg_acc_regsel_cmd_rst_addr_alias, wr_data, fast_write);

    // Return without waiting for command completion (non-blocking)
    return;
} //pipe_put

// Blocking PIPE Write (Write Committed)
void pipe_put_blk(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr, uint32_t cmd, uint32_t data)
{
    pipe_put(gcr_addr, reg_sel, reg_addr, cmd, data);
    // Return after Write Acknowledge is received and clears reg_acc_busy (blocking)
    // Poll until PIPE is not busy or timeout
    int busy = 1;
    int timeout = 8; // make longer than PMB fir timeout = 0.5ns*16*64=512ns

    // PSL busy
    do
    {
        timeout--;
        busy = get_ptr_field(gcr_addr, pipe_reg_acc_busy);
    }
    while (busy && timeout > 0);

    return;
} //pipe_put_blk


// PIPE Read
uint32_t pipe_get(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr)
{
    // Poll until PIPE is not busy
    int busy = 1;
    int timeout = 8; // make longer than PMB fir timeout = 0.5ns*16*64=512ns

    // PSL busy
    do
    {
        timeout--;
        busy = get_ptr_field(gcr_addr, pipe_reg_acc_busy);
    }
    while (busy && timeout > 0);

    // Write the Read Command (reset=0)
    uint32_t cmd = pipe_reg_cmd_read;
    uint32_t wr_data = (reg_sel  << pipe_reg_acc_reg_sel_shift) |
                       (reg_addr << pipe_reg_acc_address_shift) |
                       (cmd      << pipe_reg_acc_command_shift);
    put_ptr_field(gcr_addr, pipe_reg_acc_regsel_cmd_rst_addr_alias, wr_data, fast_write);

    // Poll for Data
    uint32_t rd_data;
    // PSL busy_read
    timeout = 8; // make longer than PMB fir timeout = 0.5ns*16*64=512ns

    do
    {
        timeout--;
        rd_data = get_ptr_field(gcr_addr, pipe_reg_acc_busy_rddata_alias);
        busy = rd_data & pipe_reg_acc_busy_mask;
    }
    while (busy && timeout > 0);

    // Return the Data
    return bitfield_get(rd_data, pipe_reg_acc_rddata_mask, pipe_reg_acc_rddata_shift);
} //pipe_get

///////////////////////////////////
// Helper Functions
///////////////////////////////////

// Wrapper for restoring the RX Data latch values, reset DFE, and preset Peak/LTE on a lane
void pipe_preset_rx(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode, bool restore)
{
    // Switch to RX group (if not RX since already set to rx_group)
    int l_saved_reg_id = get_gcr_addr_reg_id(gcr_addr);

    if (l_saved_reg_id != rx_group)
    {
        set_gcr_addr_reg_id(gcr_addr, rx_group);
    }

    // Preset Peak/LTE
    preset_rx_peak_lte_values(gcr_addr, cal_mode);

    // Two different methods for restoring the data latch dacs and removing any offset (pr_offset_d/e, dfe_clk/ddc, qpa) from the mini PRs.
    //   On a restore (rate change):
    //     The edge and data mini PRs are set exactly to nominal (16).
    //     The data latch dacs are set back to their original loff + poff value. Edge latch dacs are untouched.
    //   Otherwise (rx eq eval):
    //     The edge mini PRs are left unchanged and the data mini PRs are centered around nominal (16). This leaves QPA untouched and is faster.
    //     The data latch dacs have the original DFE Fast coefficients subtracted out (faster). Edge latch dacs are untouched.
    if (restore)
    {
        // Rate Change:
        // Restore saved values for Bank A & B, reset mini PR, and clear pr_offset_applied
        restore_rx_data_latch_dac_values(gcr_addr, bank_a);
        int thread = get_gcr_addr_thread(gcr_addr);
        io_sleep(thread);
        restore_rx_data_latch_dac_values(gcr_addr, bank_b);
        io_sleep(thread);
    }
    else     //!restore
    {
        // RxEqEval:
        // Subtract DFE From Both Banks
        clear_rx_dfe(gcr_addr);
        // Remove the offset (pr_offset_e) from the edge mini PRs and clear pr_offset_applied
        remove_edge_pr_offset(gcr_addr);
    }

    // Restore original reg_id (if not RX since already set to rx_group)
    if (l_saved_reg_id != rx_group)
    {
        set_gcr_addr_reg_id(gcr_addr, l_saved_reg_id);
    }
} //pipe_preset_rx


//////////////////////////////////////////////////////
// Run PIPE Commands - 1 per lane
//////////////////////////////////////////////////////
void run_pipe_commands(t_gcr_addr* gcr_addr, uint32_t num_lanes)
{
    set_debug_state(0xFC00, PIPE_IFC_DBG_LVL); // PIPE: Start of Lane Processing of PIPE Transactions

    // Stress mode PL Config Vector
    uint32_t l_pipe_lane_stress_mode_0_3 = mem_pg_field_get(ppe_pipe_lane_stress_mode_0_3) <<
                                           (32 - ppe_pipe_lane_stress_mode_0_3_width);

    // Run one command per lane to prevent starving lanes
    int l_lane;

    for (l_lane = 0; l_lane < num_lanes; l_lane++)
    {
        mem_pg_field_put(ppe_pipe_current_lane, l_lane);

        // If the stress mode bit is set for this lane, wait for at least 400us and then skip checking for commands
        if (l_pipe_lane_stress_mode_0_3 & (0x80000000 >> l_lane))
        {
            set_debug_state(0xFCA0 | l_lane, PIPE_IFC_DBG_LVL); // PIPE: Start of Pipe Lane Stressor
            io_wait_us(get_gcr_addr_thread(gcr_addr), 400);
            continue;
        }

        // Read the PIPE state (command) vector
        set_gcr_addr_reg_id_lane(gcr_addr, tx_group, l_lane);
        uint32_t pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
        pipe_state = pipe_state & PIPE_STATE_MASK;
        //set_debug_state(pipe_state, PIPE_IFC_DBG_LVL); // PIPE: Display pipe_state

        // The pipe_state bits are positioned by priority, so handle the most signficant set command
        uint32_t cmd_sel = __builtin_clz(pipe_state) -
                           16; //count leading zeros ignoring the top u16 since pipe_state_0_15 is only 16b

        // Skip if no request set
        // PSL cmd_sel_ge_max
        if (cmd_sel >= MAX_NUM_PIPE_CMD)
        {
            continue;
        }

        // Execute the pipe command handler function
        ioo_pipe_cmd_table[cmd_sel](gcr_addr);

        set_debug_state(0xFCFF, PIPE_IFC_DBG_LVL); // PIPE: End of PIPE Lane Processing
        io_sleep(get_gcr_addr_thread(gcr_addr));
        mem_pg_field_put(ppe_thread_active_time_us_limit, PCIE_THREAD_ACTIVE_TIME_LIMIT_US);
    } //for(l_lane)

    // Switch back to default (RX) reg space
    set_gcr_addr_reg_id(gcr_addr, rx_group);

    return;
} //run_pipe_commands

///////////////////////////
// Default / NOP command
// Sets a FIR if called
///////////////////////////

void pipe_cmd_nop(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFCAA, 1); // PIPE: ERROR - pipe_cmd_nop
    ADD_LOG(DEBUG_BAD_PIPE_CMD, gcr_addr, 0x0);
    // PSL set_fir_fatal_error
    set_fir(fir_code_fatal_error);
} //pipe_nop

///////////////////////////////////
// PIPE Command Handlers
///////////////////////////////////


//////////////////////////////////
// RESETN_ACTIVE
//////////////////////////////////
PK_STATIC_ASSERT(ppe_pipe_reset_active_phase_0_3_width == 4);
void pipe_cmd_resetn_active(t_gcr_addr* gcr_addr)
{
    int l_lane = get_gcr_addr_lane(gcr_addr);

    // Determine the phase of this command
    uint32_t l_pipe_reset_active_phase_0_3 = mem_pg_field_get(ppe_pipe_reset_active_phase_0_3);
    uint32_t l_phase = l_pipe_reset_active_phase_0_3 & (0x8 >> l_lane);

    /////////////
    // PHASE 0 //
    /////////////
    // PSL phase_zero
    if (l_phase == 0)
    {
        set_debug_state(0xFC01, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESETN_ACTIVE Transaction

        // Set phase for next call
        l_pipe_reset_active_phase_0_3 |= (0x8 >> l_lane);
        mem_pg_field_put(ppe_pipe_reset_active_phase_0_3, l_pipe_reset_active_phase_0_3);

        // 1.) Save rx sigdet off_dm value prior to reset
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        uint32_t l_rx_off_dm = get_ptr_field(gcr_addr, rx_off_dm);

        // 2.) Run per lane reset which powers down the lane
        io_reset_lane_tx(gcr_addr); //sleeps at end
        io_reset_lane_rx(gcr_addr); //sleeps at end

        // 3.) Restore sigdet off_dm value after reset
        put_ptr_field(gcr_addr, rx_off_dm, l_rx_off_dm, read_modify_write);

        // Return so don't run final phase
        return;
    } //if (phase0)

    /////////////
    // PHASE 1 //
    /////////////
    set_debug_state(0xFC11, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESETN_ACTIVE Transaction

    // Clear phase for next call
    l_pipe_reset_active_phase_0_3 &= ~(0x8 >> l_lane);
    mem_pg_field_put(ppe_pipe_reset_active_phase_0_3, l_pipe_reset_active_phase_0_3);

    // 2.) Power up TX to allow EI propagation.
    //     Also go ahead and power up RX here (DL clock will be disabled).
    io_lane_power_on_tx(gcr_addr); //sleeps at end
    io_lane_power_on_rx(gcr_addr, main_only,
                        false); // Power on but leave dl_clk_en untouched (HW508366), has sleep in middle

    // 3.) Enable idle_loz and idle_mode hardware
    //     The FIFO and delay logic for pipe_tx_elecidle don't work during reset.
    //     Assume that the MAC will raise pipe_tx_elecidle during reset and drive
    //     the circuit pins with the override registers instead.
    //     tx_idle_mode_ovr_en,tx_idle_mode_ovr_val,tx_idle_loz_ovr_en,tx_idle_loz_ovr_val
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b1111, read_modify_write);

    // 4.) Wait with low impedance to allow the driver cap to (dis)charge
    //     from rail voltage to somewhere near the common mode
    uint32_t l_ppe_sim_speedup = img_field_get(ppe_sim_speedup);

    // PSL ppe_sim_speedup_wait_10
    if (!l_ppe_sim_speedup)
    {
        io_wait_us(get_gcr_addr_thread(gcr_addr), 10);
    }

    // 5.) Lower idle_loz so the TX transitions into an EI state
    //     tx_idle_mode_ovr_en,tx_idle_mode_ovr_val,tx_idle_loz_ovr_en,tx_idle_loz_ovr_val
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b1110, read_modify_write);

    // 6.) clear pipe_state_txdetectrx_status
    //     clear pipe_state_pclkchangeok_pulse
    //     clear pipe_state_rxdatavalid
    //     clear pipe_state_phystatus_clear
    //     clear pipe_state_phystatus_pulse
    put_ptr(gcr_addr, pipe_state_cntl1_pl_addr, pipe_state_cntl1_pl_full_reg_startbit, pipe_state_cntl1_pl_full_reg_endbit,
            0x0000, fast_write);

    // 7.) clear pipe_state_resetn_active to disable hardware reset prior to clearing pipe_update status
    put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, pipe_state_resetn_active_clear_mask, fast_write);

    // 8.) clear following pending pipe processes due to reset assertion
    //     DO NOT CLEAR pipe_state_resetn_inactive
    //     pipe_state_rxstandby_active_clear
    //     pipe_state_pclkchangeack_clear
    //     pipe_state_rate_updated_clear
    //     pipe_state_powerdown_updated_clear
    //     pipe_state_rxeqeval_clear
    //     pipe_state_txdetectrx_clear
    //     pipe_state_rxelecidle_active_clear
    //     pipe_state_rxelecidle_inactive_clear
    //     pipe_state_txelecidle_active_clear
    //     pipe_state_txelecidle_inactive_clear
    //     pipe_state_rxmargincontrol_clear
    uint32_t l_register_data = pipe_state_rxstandby_active_clear_mask |
                               pipe_state_pclkchangeack_clear_mask |
                               pipe_state_rate_updated_clear_mask |
                               pipe_state_powerdown_updated_clear_mask |
                               pipe_state_rxeqeval_clear_mask |
                               pipe_state_txdetectrx_clear_mask |
                               pipe_state_rxelecidle_active_clear_mask |
                               pipe_state_rxelecidle_inactive_clear_mask |
                               pipe_state_txelecidle_active_clear_mask |
                               pipe_state_txelecidle_inactive_clear_mask |
                               pipe_state_rxmargincontrol_clear_mask;
    put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, l_register_data, fast_write);

    // 9.) // Set RX Clock/Circuit to Gen5 Rate to speed up the servos in DC Cal (LOFF) that is run in reset_inactive
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    int rate_one_hot = 0b10000; //gen5
    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,     0b11,          read_modify_write);
    put_ptr_field(gcr_addr, rx_pcie_clk_sel,               rate_one_hot,  read_modify_write);
    update_rx_rate_dependent_analog_ctrl_pl_regs(gcr_addr, rate_one_hot);
    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,     0b00,          read_modify_write);

    // 10.) Clear phase/state bits for other PIPE commands (since the lane is being reset)
    uint32_t l_pipe_reset_inactive_phase_0_3 = mem_pg_field_get(ppe_pipe_reset_inactive_phase_0_3);
    l_pipe_reset_inactive_phase_0_3 &= ~(0xC0 >> (2 * l_lane));
    mem_pg_field_put(ppe_pipe_reset_inactive_phase_0_3, l_pipe_reset_inactive_phase_0_3);

    uint32_t l_pipe_rx_ei_inactive_phase_0_3 = mem_pg_field_get(ppe_pipe_rx_ei_inactive_phase_0_3);
    l_pipe_rx_ei_inactive_phase_0_3 &= ~(0xC0 >> (2 * l_lane));
    mem_pg_field_put(ppe_pipe_rx_ei_inactive_phase_0_3, l_pipe_rx_ei_inactive_phase_0_3);

    uint32_t l_pipe_txdetectrx_phase_0_3 = mem_pg_field_get(ppe_pipe_txdetectrx_phase_0_3);
    l_pipe_txdetectrx_phase_0_3 &= ~(0x8 >> l_lane);
    mem_pg_field_put(ppe_pipe_txdetectrx_phase_0_3, l_pipe_txdetectrx_phase_0_3);

    mem_pl_bit_clr(rx_dccal_done, l_lane);

    return;
} //pipe_cmd_resetn_active

//////////////////////////////////
// RESETN_INACTIVE
//////////////////////////////////
PK_STATIC_ASSERT(rx_dc_enable_loff_ab_data_edge_alias_width == 4);
PK_STATIC_ASSERT(ppe_pipe_reset_inactive_phase_0_3_width == 8); // 2 bits per lane
void pipe_cmd_resetn_inactive(t_gcr_addr* gcr_addr)
{

    set_gcr_addr_reg_id(gcr_addr, rx_group);
    int l_lane = get_gcr_addr_lane(gcr_addr);

    // Determine the phase of this command
    uint32_t l_pipe_reset_inactive_phase_0_3 = mem_pg_field_get(ppe_pipe_reset_inactive_phase_0_3);
    uint32_t l_phase = ( l_pipe_reset_inactive_phase_0_3 >> (2 * (3 - l_lane)) ) & 0x03;

    /////////////
    // PHASE 0 //
    /////////////
    // PSL phase_zero
    if (l_phase == 0b00)
    {
        set_debug_state(0xFC02, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESETN_INACTIVE Transaction

        // Set Phase 1 (01) for next call
        l_pipe_reset_inactive_phase_0_3 |= (0x40 >> (2 * l_lane));
        mem_pg_field_put(ppe_pipe_reset_inactive_phase_0_3, l_pipe_reset_inactive_phase_0_3);

        // 1a.) Run RX DC Cal on A Data
        mem_pg_field_put(rx_dc_enable_loff_ab_data_edge_alias, 0b1010); //A Data Only
        eo_main_dccal_rx(gcr_addr);

        // Abort reset_inactive if reset_active is pending
        set_gcr_addr_reg_id(gcr_addr, tx_group);
        uint32_t l_pipe_state_resetn_active = get_ptr_field(gcr_addr, pipe_state_0_15_alias) & pipe_state_resetn_active_mask;

        // PSL pipe_reset_inactive_p2_aborted_by_pipe_reset_active
        if(l_pipe_state_resetn_active != 0)
        {
            put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, pipe_state_resetn_inactive_clear_mask, fast_write);
        }

        // Return so don't run final phase
        return;
    } //if (phase0)

    /////////////
    // PHASE 1 //
    /////////////
    // PSL phase_one
    if (l_phase == 0b01)
    {
        set_debug_state(0xFC12, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESEN_INACTIVE Phase 1 Processing

        // Set Phase 2 (11) for next call
        l_pipe_reset_inactive_phase_0_3 |= (0x80 >> (2 * l_lane));
        mem_pg_field_put(ppe_pipe_reset_inactive_phase_0_3, l_pipe_reset_inactive_phase_0_3);

        // 1b.) Run RX DC Cal on A Edge (also saves A Data loff results)
        mem_pg_field_put(rx_dc_enable_loff_ab_data_edge_alias, 0b1001); //A Edge Only
        eo_main_dccal_rx(gcr_addr);

        // Abort reset_inactive if reset_active is pending
        set_gcr_addr_reg_id(gcr_addr, tx_group);
        uint32_t l_pipe_state_resetn_active = get_ptr_field(gcr_addr, pipe_state_0_15_alias) & pipe_state_resetn_active_mask;

        // PSL pipe_reset_inactive_p0_aborted_by_pipe_reset_active
        if(l_pipe_state_resetn_active != 0)
        {
            put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, pipe_state_resetn_inactive_clear_mask, fast_write);
        }

        // Return so don't run final phase
        return;
    } //if (phase1)

    /////////////
    // PHASE 2 //
    /////////////
    if (l_phase == 0b11)
    {
        set_debug_state(0xFC22, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESEN_INACTIVE Phase 2 Processing

        // Set Phase 3 (10) for next call
        l_pipe_reset_inactive_phase_0_3 &= ~(0x40 >> (2 * l_lane));
        mem_pg_field_put(ppe_pipe_reset_inactive_phase_0_3, l_pipe_reset_inactive_phase_0_3);

        // 2a.) Run RX DC Cal on B Data
        mem_pg_field_put(rx_dc_enable_loff_ab_data_edge_alias, 0b0110); //B Data Only
        eo_main_dccal_rx(gcr_addr);

        // Abort reset_inactive if reset_active is pending
        set_gcr_addr_reg_id(gcr_addr, tx_group);
        uint32_t l_pipe_state_resetn_active = get_ptr_field(gcr_addr, pipe_state_0_15_alias) & pipe_state_resetn_active_mask;

        // PSL pipe_reset_inactive_p2_aborted_by_pipe_reset_active
        if(l_pipe_state_resetn_active != 0)
        {
            put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, pipe_state_resetn_inactive_clear_mask, fast_write);
        }

        // Return so don't run final phase
        return;
    } //if (phase2)

    /////////////
    // PHASE 3 //
    /////////////
    set_debug_state(0xFC32, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RESEN_INACTIVE Phase 3 Processing

    // Clear phase (Phase 0, 00) for next call
    l_pipe_reset_inactive_phase_0_3 &= ~(0xC0 >> (2 * l_lane));
    mem_pg_field_put(ppe_pipe_reset_inactive_phase_0_3, l_pipe_reset_inactive_phase_0_3);

    // 2b.) Run RX DC Cal on B Edge (also saves B Data loff results)
    mem_pg_field_put(rx_dc_enable_loff_ab_data_edge_alias, 0b0101); //B Edge Only
    eo_main_dccal_rx(gcr_addr);
    mem_pl_bit_set(rx_dccal_done, l_lane);

    // 3.) Implied data rate change to Gen1
    pipe_clock_change(gcr_addr, 1);  // has a sleep, sets tx_group
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    preset_rx_peak_lte_values(gcr_addr, C_PCIE_GEN1_CAL);

    // 4.) Enable rx_dl_clk
    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write);

    // 5.) Initialize TX FIFO and load TX DCC settings
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    tx_fifo_init(gcr_addr);
    io_sleep(get_gcr_addr_thread(gcr_addr));
    put_ptr_field(gcr_addr, tx_bank_controls_d2_en_b_alias, 0b1, read_modify_write); // disable d2 for gen1
    restore_tx_dcc_tune_values(gcr_addr, C_PCIE_GEN1_CAL);

    // 6.) Set idle_del_sel, loz_del_sel to gen1 values
    uint32_t l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_1_alias);
    put_ptr_field(gcr_addr, tx_pcie_idle_loz_del_sel_pl_alias, l_idle_loz_del_sel, fast_write); //Only fields in register

    // 7.) Drive idle_mode and idle_loz from FIFO rather than registers.
    //     Switch-over should be glitchless.
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b0000, read_modify_write);

    // 8.) Update txdeemph for -3.5dB for GEN 1 TX EQ
    // Read P1 Preset Coeffs
    uint32_t l_pipe_preset1_pre  = get_ptr_field(gcr_addr, pipe_preset1_pre);
    uint32_t l_pipe_preset1_post = get_ptr_field(gcr_addr, pipe_preset1_post);
    uint32_t l_pipe_preset1_main = 32 - l_pipe_preset1_pre - l_pipe_preset1_post;
    // Write txdeemph PHY registers
    pipe_put(gcr_addr, pipe_phy_reg_tx_control_2_reg_sel, pipe_phy_reg_tx_control_2_addr, pipe_reg_cmd_wr_u,
             l_pipe_preset1_pre);
    pipe_put(gcr_addr, pipe_phy_reg_tx_control_3_reg_sel, pipe_phy_reg_tx_control_3_addr, pipe_reg_cmd_wr_u,
             l_pipe_preset1_main);
    pipe_put(gcr_addr, pipe_phy_reg_tx_control_4_reg_sel, pipe_phy_reg_tx_control_4_addr, pipe_reg_cmd_wr_c,
             l_pipe_preset1_post);

    // 9.) WRU MAC TX_Status_3 of FS of 32
    pipe_reg_write_uncommitted(gcr_addr, pipe_mac_reg_tx_status_3, 0x20);

    // 10.) WRC MAC TX_Status_4 of LF of 12
    pipe_reg_write_committed(gcr_addr, pipe_mac_reg_tx_status_4, 0x0C);

    // 11.) Check to see if reset_active has been set while processing resetn_inactive
    //      if resetn_active is set then do not clear phystatus
    //      if resetn_active is set then clear resetn_inactive since it has been aborted.
    //      The above scenario is protected by requiring pipe_resetn to be asserted for a minimum time that ensures it will complete reset_active ppe processing (>4ms)
    //      if resetn_active is not set then clear phystatus and clear reset_inactive

    uint32_t l_pipe_state_resetn_active = get_ptr_field(gcr_addr, pipe_state_0_15_alias) & pipe_state_resetn_active_mask;

    // PSL resetn_active
    if (l_pipe_state_resetn_active == 0)   // no pending resetn_active request, clear reset_inactive, clear phystatus
    {
        uint32_t l_register_data = pipe_state_resetn_inactive_clear_mask |
                                   pipe_state_rxstandby_active_clear_mask |
                                   pipe_state_pclkchangeack_clear_mask |
                                   pipe_state_rate_updated_clear_mask |
                                   pipe_state_powerdown_updated_clear_mask |
                                   pipe_state_rxeqeval_clear_mask |
                                   pipe_state_txdetectrx_clear_mask |
                                   pipe_state_rxelecidle_active_clear_mask |
                                   pipe_state_rxelecidle_inactive_clear_mask |
                                   pipe_state_txelecidle_active_clear_mask |
                                   pipe_state_txelecidle_inactive_clear_mask |
                                   pipe_state_rxmargincontrol_clear_mask |
                                   pipe_state_txdeemph_updated_clear_mask;
        put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, l_register_data, fast_write);
        put_ptr_field(gcr_addr, pipe_state_phystatus_clear, 0b1, fast_write); // PHY pipe_state_phystatus_clear bit to 1
        put_ptr_field(gcr_addr, pipe_state_rxelecidle_en_set, 0b1, fast_write); // set rxelecidle_en
    }
    else     // pending resetn_active request, clear resetn_inactive, do not clear phystatus
    {
        uint32_t l_register_data = pipe_state_resetn_inactive_clear_mask |
                                   pipe_state_rxstandby_active_clear_mask |
                                   pipe_state_pclkchangeack_clear_mask |
                                   pipe_state_rate_updated_clear_mask |
                                   pipe_state_powerdown_updated_clear_mask |
                                   pipe_state_rxeqeval_clear_mask |
                                   pipe_state_txdetectrx_clear_mask |
                                   pipe_state_rxelecidle_active_clear_mask |
                                   pipe_state_rxelecidle_inactive_clear_mask |
                                   pipe_state_txelecidle_active_clear_mask |
                                   pipe_state_txelecidle_inactive_clear_mask |
                                   pipe_state_rxmargincontrol_clear_mask |
                                   pipe_state_txdeemph_updated_clear_mask;
        put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, l_register_data, fast_write);
    }

    return;
} //pipe_cmd_resetn_inactive

//////////////////////////////////
// RXSTANDBY_ACTIVE - Handled in hardware
//////////////////////////////////

//void pipe_cmd_rxstandby_active(t_gcr_addr *gcr_addr) {
//  set_debug_state(0xFC03, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXSTANDBY_ACTIVE Transaction
//  // 1.) clear rxdatavalid, clear rxactive, set rxelecidle_en
//  put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, (pipe_state_rxdatavalid_clear_mask | pipe_state_rxactive_clear_mask | pipe_state_rxelecidle_en_set_mask), fast_write);
//
//  // 2.) clear rxstandby_active and rxelecidle status
//  put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, (pipe_state_rxstandby_active_clear_mask | pipe_state_rxelecidle_active_clear_mask | pipe_state_rxelecidle_inactive_clear_mask), fast_write);
//  return;
//} //pipe_cmd_rxstandby_active

//////////////////////////////////
// PCLKCHANGEACK_ACTIVE
//////////////////////////////////

void pipe_cmd_pclkchangeack_active(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFC05, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_PCLKCHANGEACK_ACTIVE Transaction

    // 1.) Abort if pipe_resetn is active
    uint32_t l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
    uint32_t l_pipe_state_resetn_active = l_pipe_state & pipe_state_resetn_active_mask;

    // PSL pclkchangeack_resetn_asserted
    if (l_pipe_state_resetn_active)
    {
        return;
    }

    // 2.) Change RX and TX Clocks; set d2_en
    uint32_t l_rate = get_ptr_field(gcr_addr, pipe_state_rate); // gen1=0, gen2=1, gen3=2, gen4=3, gen5=4
    uint32_t l_rate_one_hot = (1 <<
                               l_rate);                    // gen1=00001, gen2=00010, gen3=00100, gen4=01000, gen5=10000
    pipe_clock_change(gcr_addr, l_rate_one_hot); // has a sleep

    // 3.) Initialize TX FIFO at new data rate
    tx_fifo_init(gcr_addr);
    put_ptr_field(gcr_addr, tx_bank_controls_d2_en_b_alias, l_rate_one_hot > 2 ? 0b0 : 0b1,
                  read_modify_write); // update based on speed

    // 4.) Enable idle_loz and idle_mode hardware
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b0000, read_modify_write);
    io_sleep(get_gcr_addr_thread(gcr_addr)); //   wait to allow CDRs to settle

    // 5.) Load TX DCC Results for requested gen rate
    restore_tx_dcc_tune_values(gcr_addr, l_rate);

    // 6.) Restore RX Latch Offsets, reset DFE, and preset Peak/LTE for the requested gen rate  (function sets and restores reg_id as needed)
    bool l_restore = true;
    pipe_preset_rx(gcr_addr, l_rate, l_restore);

    // 7.) If Gen1 or Gen 2 Check then check precursor for value of 0 (P0), 1 (P1), otherwise no EQ (P4).  Update txdeemph as per presets.
    uint32_t l_pipe_preset_pre;
    uint32_t l_pipe_preset_post;
    uint32_t l_pipe_preset_main;

    if (l_rate <
        2)   // GEN 1 or GEN 2, mac immediately writes tx_control_2 with preset when pipe_rate changes, must remap presets to coeffs
    {
        l_pipe_preset_pre = pipe_get(gcr_addr, pipe_phy_reg_tx_control_2_reg_sel, pipe_phy_reg_tx_control_2_addr);

        // PSL preset_0
        if (l_pipe_preset_pre == 0)   // Preset 0
        {
            l_pipe_preset_pre  = get_ptr_field(gcr_addr, pipe_preset0_pre);
            l_pipe_preset_post = get_ptr_field(gcr_addr, pipe_preset0_post);
            // PSL preset_1
        }
        else if (l_pipe_preset_pre == 1)     // Preset 1
        {
            l_pipe_preset_pre  = get_ptr_field(gcr_addr, pipe_preset1_pre);
            l_pipe_preset_post = get_ptr_field(gcr_addr, pipe_preset1_post);
            // PSL preset_4
        }
        else     // Preset 4 : No preemphasis and no deemphasis
        {
            l_pipe_preset_pre  = get_ptr_field(gcr_addr, pipe_preset4_pre);
            l_pipe_preset_post = get_ptr_field(gcr_addr, pipe_preset4_post);
        }

        l_pipe_preset_main = 32 - l_pipe_preset_pre - l_pipe_preset_post;
        // Enable TX Segment Calculation Update
        put_ptr_field(gcr_addr, tx_pcie_eq_calc_enable, 0b1, read_modify_write);
        // Write txdeemph PHY registers
        pipe_put(gcr_addr, pipe_phy_reg_tx_control_2_reg_sel, pipe_phy_reg_tx_control_2_addr, pipe_reg_cmd_wr_u,
                 l_pipe_preset_pre);
        pipe_put(gcr_addr, pipe_phy_reg_tx_control_3_reg_sel, pipe_phy_reg_tx_control_3_addr, pipe_reg_cmd_wr_u,
                 l_pipe_preset_main);
        pipe_put(gcr_addr, pipe_phy_reg_tx_control_4_reg_sel, pipe_phy_reg_tx_control_4_addr, pipe_reg_cmd_wr_c,
                 l_pipe_preset_post);
    }

    // 8.) Clear pending pipe requests for service prior to pulsing phystatus except resetn active and reset inactive
    uint32_t l_register_data = pipe_state_rxstandby_active_clear_mask |
                               pipe_state_pclkchangeack_clear_mask |
                               pipe_state_rate_updated_clear_mask |
                               pipe_state_powerdown_updated_clear_mask |
                               pipe_state_rxeqeval_clear_mask |
                               pipe_state_txdetectrx_clear_mask |
                               pipe_state_rxelecidle_active_clear_mask |
                               pipe_state_rxelecidle_inactive_clear_mask |
                               pipe_state_txelecidle_active_clear_mask |
                               pipe_state_txelecidle_inactive_clear_mask |
                               pipe_state_rxmargincontrol_clear_mask |
                               pipe_state_txdeemph_updated_clear_mask;
    put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, l_register_data, fast_write);

    // 9.) Enable rxelecidle_en then fast write pipe_state_cntl1_pl pipe_state_phystatus_pulse to 1 and pipe_state_pclkchangeack_clr_apsp to 1 if pipe_resetn_active is low
    l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
    l_pipe_state_resetn_active = l_pipe_state & pipe_state_resetn_active_mask;

    // PSL resetn_active
    if (!l_pipe_state_resetn_active)
    {
        put_ptr_field(gcr_addr, pipe_state_rxelecidle_en_set, 0b1, fast_write);
        put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg,
                      (pipe_state_phystatus_pulse_mask | pipe_state_pclkchangeack_clr_apsp_mask), fast_write);
    }

    return;
} //pipe_cmd_pclkchangeack_active

//////////////////////////////////
// RATE_UDATED
//////////////////////////////////

void pipe_cmd_rate_updated(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFC06, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RATE_UPDATED Transaction

    // 1.) clear rxdatavalid, clear pipe_state_rxactive, clear rxelecidle_en
    put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, (pipe_state_rxdatavalid_clear_mask |
                  pipe_state_rxactive_clear_mask |
                  pipe_state_rxelecidle_en_clear_mask), fast_write);
    // 2.) Get present pipe rate value
    uint32_t l_rate = get_ptr_field(gcr_addr, pipe_state_rate); // gen1=0, gen2=1, gen3=2, gen4=3, gen5=4
    uint32_t l_rate_one_hot = (1 <<
                               l_rate);                    // gen1=00001, gen2=00010, gen3=00100, gen4=01000, gen5=10000

    // 3.) Disable idle_loz and idle_mode hardware; idle_mode_ovr and idle_loz_ovr val are 1 and 0, respectively
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b1111, read_modify_write);

    // 4.) Update idle_del_sel, loz_del_sel, and l2u_dly values based on gen
    uint32_t l_idle_loz_del_sel;

    // PSL rate_gen5
    if (l_rate_one_hot == 0b10000)   // gen5
    {
        l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_5_alias);
        // PSL rate_gen4
    }
    else if (l_rate_one_hot == 0b01000)     // gen4
    {
        l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_4_alias);
        // PSL rate_gen3
    }
    else if (l_rate_one_hot == 0b00100)     // gen3
    {
        l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_3_alias);
        // PSL rate_gen2
    }
    else if (l_rate_one_hot == 0b00010)     // gen2
    {
        l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_2_alias);
        // PSL rate_gen1
    }
    else     // default to gen1
    {
        l_idle_loz_del_sel = mem_pg_field_get(tx_pcie_idle_loz_del_sel_1_alias);
    }

    put_ptr_field(gcr_addr, tx_pcie_idle_loz_del_sel_pl_alias, l_idle_loz_del_sel, fast_write); //Only fields in register

    // 5.) Clear rx_init_done and set unused since not run at new rate as yet (EWM304487).
    int l_lane = get_gcr_addr_lane(gcr_addr);
    mem_pl_bit_clr(rx_init_done, l_lane);
    set_recal_or_unused(l_lane);

    // 6.) Clear pending pipe requests for service prior to waiting for pclkchangeack to be asserted.
    uint32_t l_register_data = pipe_state_rxstandby_active_clear_mask |
                               pipe_state_pclkchangeack_clear_mask |
                               pipe_state_rate_updated_clear_mask |
                               pipe_state_powerdown_updated_clear_mask |
                               pipe_state_rxeqeval_clear_mask |
                               pipe_state_txdetectrx_clear_mask |
                               pipe_state_rxelecidle_active_clear_mask |
                               pipe_state_rxelecidle_inactive_clear_mask |
                               pipe_state_txelecidle_active_clear_mask |
                               pipe_state_txelecidle_inactive_clear_mask |
                               pipe_state_rxmargincontrol_clear_mask |
                               pipe_state_txdeemph_updated_clear_mask;
    put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, l_register_data, fast_write);

    // 7.) fast write pipe_state_cntl1_pl and set pipe_state_pclkchangeok_pulse to 1
    put_ptr_field(gcr_addr, pipe_state_pclkchangeok_pulse, 0b1, fast_write);

    return;
} //pipe_cmd_rate_updated

//////////////////////////////////
// POWERDOWN_UDATED
//////////////////////////////////
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_addr == pipe_state_powerdown_addr);
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_addr == pipe_state_rate_addr);
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_width == 16);
void pipe_cmd_powerdown_updated(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFC07, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_POWERDOWN_UPDATED Transaction
    //
    // PCIe powerdown=00 : P0  : Active power state
    // PCIe powerdown=01 : P0s : Low  latency powerdown state. No power savings taken on P11. Immediate phystatus pulsed. Only affects TX.
    // PCIe powerdown=10 : P1  : Mid  latency powerdown state. No power savings taken on P11. Immediate phystatus pulsed
    // PCIe powerdown=11 : P2  : High latency powerdown state. Maximum power savings taken on P11. PSAVE lane then phystatus pulsed. Requires reset to exit for P11.

    bool l_restore = true;                                                               // rx_preset_restore enabled
    int l_lane = get_gcr_addr_lane(gcr_addr);                                            // Get Lane

    uint32_t l_pipe_state_status2_regval = get_ptr_field(gcr_addr, pipe_state_status2_pl_full_reg);
    uint32_t l_pipe_state_powerdown = bitfield_get(l_pipe_state_status2_regval, pipe_state_powerdown_mask,
                                      pipe_state_powerdown_shift); // get pipe_state_powerdown
    uint32_t l_rate = bitfield_get(l_pipe_state_status2_regval, pipe_state_rate_mask,
                                   pipe_state_rate_shift); // Get present rate value

    // PSL powerdown_eq_0
    if (l_pipe_state_powerdown == 0)                                                     // Change to P0 power state
    {
        put_ptr_field(gcr_addr,
                      pipe_state_0_15_clear_alias,                                // Clear selected pending PIPE requests for PPE service
                      (pipe_state_rxstandby_active_clear_mask
                       |                             // Clear pending rxstandy requests for PPE service
                       pipe_state_pclkchangeack_clear_mask
                       |                                // Clear pending pclkchangeack requests for PPE service
                       pipe_state_rate_updated_clear_mask
                       |                                 // Clear pending rate requests for PPE service
                       pipe_state_powerdown_updated_clear_mask
                       |                            // Clear pending powerdown requests for PPE service
                       pipe_state_rxeqeval_clear_mask
                       |                                     // Clear pending rxeqeval requests for PPE service
                       pipe_state_txdetectrx_clear_mask
                       |                                   // Clear pending txdetectrx requests for PPE service
                       pipe_state_rxelecidle_active_clear_mask
                       |                            // Clear pending rxelecidle_active requests for PPE service
                       pipe_state_rxelecidle_inactive_clear_mask
                       |                          // Clear pending rxelecidle_inactive requests for PPE service
                       pipe_state_txelecidle_active_clear_mask
                       |                            // Clear pending txelecidle_active requests for PPE service
                       pipe_state_txelecidle_inactive_clear_mask
                       |                          // Clear pending txelecidle_inactive requests for PPE service
                       pipe_state_rxmargincontrol_clear_mask
                       |                              // Clear pending rxmargincontrol requests for PPE service
                       pipe_state_txdeemph_updated_clear_mask),                             // Clear pending txdeemph requests for PPE service
                      fast_write);                                                         // use fast write
        put_ptr_field(gcr_addr, pipe_state_rxelecidle_en_set, 0b1,
                      fast_write);             // Set pipe_state_rxelecidle_en, do not alter rxdatavalid, do not alter rxactive
        // PSL powerdown_eq_1
    }
    else if (l_pipe_state_powerdown == 1)                                                // Change to P0s power state
    {
        put_ptr_field(gcr_addr,
                      pipe_state_0_15_clear_alias,                                // Clear selected pending PIPE requests for PPE service
                      (pipe_state_pclkchangeack_clear_mask
                       |                                // Clear pending pclkchangeack requests for PPE service
                       pipe_state_rate_updated_clear_mask
                       |                                 // Clear pending rate requests for PPE service
                       pipe_state_powerdown_updated_clear_mask
                       |                            // Clear pending powerdown requests for PPE service
                       pipe_state_rxeqeval_clear_mask
                       |                                     // Clear pending rxeqeval requests for PPE service
                       pipe_state_txdetectrx_clear_mask
                       |                                   // Clear pending txdetectrx requests for PPE service
                       pipe_state_txelecidle_active_clear_mask
                       |                            // Clear pending txelecidle_active requests for PPE service
                       pipe_state_txelecidle_inactive_clear_mask
                       |                          // Clear pending txelecidle_inactive requests for PPE service
                       pipe_state_rxmargincontrol_clear_mask
                       |                              // Clear pending rxmargincontrol requests for PPE service
                       pipe_state_txdeemph_updated_clear_mask),                             // Clear pending txdeemph requests for PPE service
                      fast_write);                                                         // use fast write
        // PSL powerdown_eq_2
    }
    else if (l_pipe_state_powerdown == 2)                                                // Change to P1 power state
    {
        mem_pl_bit_clr(rx_init_done, l_lane);                                               // Clear RX Init Done
        io_sleep(get_gcr_addr_thread(gcr_addr));
        pipe_preset_rx(gcr_addr, l_rate,
                       l_restore);                                        // Restore RX analog state for preparation for future entry into P0
        put_ptr_field(gcr_addr,
                      pipe_state_cntl1_pl_full_reg,                               // Update pipe_state_cntl1_pl register
                      (pipe_state_rxactive_clear_mask |                                     // Clear pipe_state_rxactive
                       pipe_state_rxdatavalid_clear_mask),                                  // Clear pipe_state_rxdatavalid
                      fast_write);                                                         // use fast write
        put_ptr_field(gcr_addr,
                      pipe_state_0_15_clear_alias,                                // Clear selected pending PIPE requests for PPE service
                      (pipe_state_rxstandby_active_clear_mask
                       |                             // Clear pending rxstandy requests for PPE service
                       pipe_state_pclkchangeack_clear_mask
                       |                                // Clear pending pclkchangeack requests for PPE service
                       pipe_state_rate_updated_clear_mask
                       |                                 // Clear pending rate requests for PPE service
                       pipe_state_powerdown_updated_clear_mask
                       |                            // Clear pending powerdown requests for PPE service
                       pipe_state_rxeqeval_clear_mask
                       |                                     // Clear pending rxeqeval requests for PPE service
                       pipe_state_txdetectrx_clear_mask
                       |                                   // Clear pending txdetectrx requests for PPE service
                       pipe_state_rxelecidle_active_clear_mask
                       |                            // Clear pending rxelecidle_active requests for PPE service
                       pipe_state_rxelecidle_inactive_clear_mask
                       |                          // Clear pending rxelecidle_inactive requests for PPE service
                       pipe_state_txelecidle_active_clear_mask
                       |                            // Clear pending txelecidle_active requests for PPE service
                       pipe_state_txelecidle_inactive_clear_mask
                       |                          // Clear pending txelecidle_inactive requests for PPE service
                       pipe_state_rxmargincontrol_clear_mask
                       |                              // Clear pending rxmargincontrol requests for PPE service
                       pipe_state_txdeemph_updated_clear_mask),                             // Clear pending txdeemph requests for PPE service
                      fast_write);                                                         // use fast write
    }
    else                                                                                 // Change to P2 power state
    {
        mem_pl_bit_clr(rx_init_done, l_lane);                                               // Clear RX Init Done
        io_sleep(get_gcr_addr_thread(gcr_addr));
        pipe_preset_rx(gcr_addr, l_rate,
                       l_restore);                                        // Restore RX analog state for preparation for future entry into P0
        put_ptr_field(gcr_addr,
                      pipe_state_cntl1_pl_full_reg,                               // Update pipe_state_cntl1_pl register
                      (pipe_state_rxactive_clear_mask |                                     // Clear pipe_state_rxactive
                       pipe_state_rxdatavalid_clear_mask),                                  // Clear pipe_state_rxdatavalid
                      fast_write);                                                         // use fast write
        io_lane_power_off_tx(gcr_addr);                                                     // Power off TX lane
        io_lane_power_off_rx(gcr_addr, both_banks);                                         // Power off RX lane
        set_gcr_addr_reg_id(gcr_addr,
                            tx_group);                                            // restore gcr group address to tx_group
        put_ptr_field(gcr_addr,
                      pipe_state_0_15_clear_alias,                                // Clear selected pending PIPE requests for PPE service
                      (pipe_state_rxstandby_active_clear_mask
                       |                             // Clear pending rxstandy requests for PPE service
                       pipe_state_pclkchangeack_clear_mask
                       |                                // Clear pending pclkchangeack requests for PPE service
                       pipe_state_rate_updated_clear_mask
                       |                                 // Clear pending rate requests for PPE service
                       pipe_state_powerdown_updated_clear_mask
                       |                            // Clear pending powerdown requests for PPE service
                       pipe_state_rxeqeval_clear_mask
                       |                                     // Clear pending rxeqeval requests for PPE service
                       pipe_state_txdetectrx_clear_mask
                       |                                   // Clear pending txdetectrx requests for PPE service
                       pipe_state_rxelecidle_active_clear_mask
                       |                            // Clear pending rxelecidle_active requests for PPE service
                       pipe_state_rxelecidle_inactive_clear_mask
                       |                          // Clear pending rxelecidle_inactive requests for PPE service
                       pipe_state_txelecidle_active_clear_mask
                       |                            // Clear pending txelecidle_active requests for PPE service
                       pipe_state_txelecidle_inactive_clear_mask
                       |                          // Clear pending txelecidle_inactive requests for PPE service
                       pipe_state_rxmargincontrol_clear_mask
                       |                              // Clear pending rxmargincontrol requests for PPE service
                       pipe_state_txdeemph_updated_clear_mask),                             // Clear pending txdeemph requests for PPE service
                      fast_write);                                                         // use fast write
    }                                                                                    // end if

    uint32_t l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);              // get pipe_state
    uint32_t l_pipe_state_resetn_active = l_pipe_state & pipe_state_resetn_active_mask;  // get pipe_resetn_active

    // PSL resetn_active
    if (!l_pipe_state_resetn_active)                                                     // only pulse phystatus if pipe_resetn_active is low
    {
        put_ptr_field(gcr_addr, pipe_state_phystatus_pulse, 0b1, fast_write);               // Pulse phystatus
    }                                                                                    // end if

    return;                                                                              // return
} //pipe_cmd_powerdown_updated

//////////////////////////////////
// RXEQEVAL
//////////////////////////////////
PK_STATIC_ASSERT(rx_eo_phase_select_0_2_width == 3);
void pipe_cmd_rxeqeval(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFC08, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXEQEVAL Transaction
    int l_lane = get_gcr_addr_lane(gcr_addr);
    int l_rate = get_ptr_field(gcr_addr, pipe_state_rate);

    // Reset DFE, and preset Peak/LTE for the requested gen rate  (function sets and restores reg_id as needed)
    bool l_restore = false;
    pipe_preset_rx(gcr_addr, l_rate, l_restore);

    // Run Training (run_initial_training sets and restores reg_id as needed)
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, pipe_state_rxdatavalid_clear, 0b1, fast_write);              // Disable pipe_rxdatavalid
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0, read_modify_write);                       // RMW Disable rx_dl_clk

    // Run in rxeqeval mode (all phases, skip some sleeps so higher thread active limit)
    mem_pg_field_put(ppe_thread_active_time_us_limit, 52);
    mem_pg_field_put(rx_eo_phase_select_0_2, 0b111);
    mem_pg_bit_set(rx_running_eq_eval);
    run_initial_training(gcr_addr, l_lane);                                              // Run Initial Training
    mem_pg_bit_clr(rx_running_eq_eval);

    put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write);                       // RMW Enable rx_dl_clk
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, pipe_state_rxdatavalid_set, 0b1, fast_write);                // Enable pipe_rxdatavalid

    // Calculate the FOM
    // TODO VBR210331 Are these the correct status fields to use?  Seem like the correct ones (unsure about DDC).
    uint32_t l_eye_width  = mem_pl_field_get(rx_a_ddc_hyst_left_edge, l_lane) + mem_pl_field_get(rx_a_ddc_hyst_right_edge,
                            l_lane);
    uint32_t l_eye_height = mem_pl_field_get(rx_dfe_ap, l_lane);

    // FOM = 256 * (width / 64) * (height / 64)
    int32_t l_fom = ( (256 / 64) * l_eye_width * l_eye_height ) / 64;

    // Apply DFE H1 penalty
    int l_dfe_h1 = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_h1, l_lane), rx_dfe_coef_h1_width);

    // PSL h1_lt_0
    if (l_dfe_h1 < 0)
    {
        l_fom = l_fom - abs(l_dfe_h1);
    }

    // Limit FOM to between 0 and 255
    l_fom = limit(l_fom, 0, 255);

    // Save last FOM value for debug
    put_ptr_field(gcr_addr, pipe_state_last_fom, l_fom, fast_write);

    // Need to ensure that at least 1.6us of valid rx data is returned to PCS/MAC prior to conducting P2M PMB FOM WRC
    // Wait an additional 1-2 us to meet this timing requirement
    io_spin_us(2); //io_wait_us(get_gcr_addr_thread(gcr_addr), 1);

    // Clear selected pending PIPE requests for PPE service (clear prior pipe request prior to PMB WRC to avoid erroniously clear back to back rxeqeval requests)
    put_ptr_field(gcr_addr, pipe_state_0_15_clear_alias, pipe_state_rxeqeval_clear_mask,
                  fast_write); // Clear selected pending PIPE requests for PPE service

    // Send FOM to MAC
    pipe_reg_write_committed(gcr_addr, pipe_mac_reg_rx_link_eval_status_0, l_fom);

    return;
} //pipe_cmd_rxeqeval

//////////////////////////////////
// TXDETECTRX
//////////////////////////////////
PK_STATIC_ASSERT(ppe_pipe_txdetectrx_phase_0_3_width == 4);
void pipe_cmd_txdetectrx(t_gcr_addr* gcr_addr)
{

    // RX Detect occurs when txdetectrx and txelecidle are both asserted high. Lane must be in P1 and Gen1 rate.
    // Loopback occurs when txdetectrx is high and txelecidle is low. Lane can be in any rate.
    uint32_t l_pipe_state_status2_pl_full_reg = get_ptr_field(gcr_addr, pipe_state_status2_pl_full_reg);
    uint32_t l_txdetectrx_ok = (((pipe_state_rate_mask | pipe_state_powerdown_mask) & l_pipe_state_status2_pl_full_reg) ==
                                0x0400);
    uint32_t l_detectrx_val;

    // Determine the phase of this command
    int l_lane = get_gcr_addr_lane(gcr_addr);
    uint32_t l_pipe_txdetectrx_phase_0_3 = mem_pg_field_get(ppe_pipe_txdetectrx_phase_0_3);
    uint32_t l_phase = l_pipe_txdetectrx_phase_0_3 & (0x8 >> l_lane);

    /////////////
    // PHASE 0 //
    /////////////
    // PSL txdetectrx_phase_zero
    if (l_phase == 0)
    {
        set_debug_state(0xFC09, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_TXDETECTRX Transaction Phase 0

        // Set phase for next call
        l_pipe_txdetectrx_phase_0_3 |= (0x8 >> l_lane);
        mem_pg_field_put(ppe_pipe_txdetectrx_phase_0_3, l_pipe_txdetectrx_phase_0_3);

        // Load per lane tx_detrx_idle_timer_sel, tx_detrx_samp_timer_val, tx_tdr_dac_cntl values from firmware initialized mem regs

        uint32_t l_tx_detrx_idle_samp_timer = mem_regs_u16[pcie_ppe_txdetrx_idle_samp_timer_lane_0_addr + 2 *
                                              l_lane]; // (0:4)=Reserved,(5:7)=idle_timer,(8:15)=samp_timer
        uint32_t l_tx_tdr_dac_cntl_pl = mem_regs_u16[pcie_ppe_txdetrx_dac_cntl_lane_0_addr + 2 *
                                        l_lane]; // (0:7)=tx_tdr_dac_cntl(0:7),(8:15)=reserved
        put_ptr_field(gcr_addr, tx_detrx_idle_timer_sel,
                      ((l_tx_detrx_idle_samp_timer & pcie_ppe_txdetrx_idle_timer_lane_0_mask) >> pcie_ppe_txdetrx_idle_timer_lane_0_shift),
                      read_modify_write);
        put_ptr_field(gcr_addr, tx_detrx_samp_timer_val,
                      ((l_tx_detrx_idle_samp_timer & pcie_ppe_txdetrx_samp_timer_lane_0_mask) >> pcie_ppe_txdetrx_samp_timer_lane_0_shift),
                      read_modify_write);
        put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl,
                      ((l_tx_tdr_dac_cntl_pl & pcie_ppe_txdetrx_dac_cntl_lane_0_mask) >> pcie_ppe_txdetrx_dac_cntl_lane_0_shift),
                      read_modify_write);

        // PSL txdetectrx_ok
        if (l_txdetectrx_ok)   //  Check that powerdown is P1 and rate is 2.5Gbps else error
        {
            uint32_t l_detectrx_ovr_en;

            // PSL lane_lt_16
            if (l_lane < 16)
            {
                l_detectrx_ovr_en = (mem_pg_field_get(tx_detectrx_ovr_en_0_15) & (0b1 << (15 - l_lane))) >> (15 - l_lane);
            }
            else
            {
                l_detectrx_ovr_en = (mem_pg_field_get(tx_detectrx_ovr_en_16_23) & (0b1 << (23 - l_lane))) >> (23 - l_lane);
            }

            // PSL detectrx_ovr_en
            if (l_detectrx_ovr_en == 1)
            {
                // PSL detectrx_ovr_en_lane_lt_16
                if (l_lane < 16)
                {
                    l_detectrx_val = (mem_pg_field_get(tx_detectrx_ovr_val_0_15) & (0b1 << (15 - l_lane))) >> (15 - l_lane);
                }
                else
                {
                    l_detectrx_val = (mem_pg_field_get(tx_detectrx_ovr_val_16_23) & (0b1 << (23 - l_lane))) >> (23 - l_lane);
                }
            }
            else
            {
                put_ptr_field(gcr_addr, tx_detrx_start, 0b1, read_modify_write);

                // PSL tx_detrx_done
                while(get_ptr_field(gcr_addr, tx_detrx_done) == 0)
                {
                    io_sleep(get_gcr_addr_thread(gcr_addr));
                }

                // PSL tx_detrx_comp_eq_0
                if (get_ptr_field(gcr_addr, tx_detrx_p_comp) + get_ptr_field(gcr_addr, tx_detrx_n_comp) == 0)
                {
                    l_detectrx_val = 1;
                }
                else
                {
                    l_detectrx_val = 0;
                }

                put_ptr_field(gcr_addr, tx_detrx_start, 0b0, read_modify_write);
            }

            // PSL update_txdetectrx_status
            if (l_detectrx_val)
            {
                put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, pipe_state_txdetectrx_status_set_mask, fast_write);
            }
            else
            {
                put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, pipe_state_txdetectrx_status_clear_mask, fast_write);
            }
        } // txdetectrx_ok

        return;
    } // phase_zero

    /////////////
    // PHASE 1 //
    /////////////
    // PSL txdetectrx_phase_one
    if (l_phase != 0)
    {
        set_debug_state(0xFC19, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_TXDETECTRX Transaction Phase 1

        // Clear phase for next call
        l_pipe_txdetectrx_phase_0_3 &= ~(0x8 >> l_lane);
        mem_pg_field_put(ppe_pipe_txdetectrx_phase_0_3, l_pipe_txdetectrx_phase_0_3);

        if (l_txdetectrx_ok)
        {
            uint32_t l_ppe_sim_speedup = img_field_get(ppe_sim_speedup);

            // PSL ppe_sim_speedup_wait_120
            if (!l_ppe_sim_speedup)
            {
                io_wait_us(get_gcr_addr_thread(gcr_addr),
                           120); // Must wait 120 us to allow TX to restore EI prior to returning txdetectrx results
            }

            // Only assert phystatus_pulse if pipe_resetn_active is not set
            // Clear txdetectrx_status after phystatus pulse
            uint32_t l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
            uint32_t l_pipe_state_resetn_active = l_pipe_state & pipe_state_resetn_active_mask;

            // PSL resetn_active
            if (!l_pipe_state_resetn_active)
            {
                put_ptr_field(gcr_addr, pipe_state_phystatus_pulse, 0b1, fast_write);
            }

            put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, pipe_state_txdetectrx_status_clear_mask, fast_write);
            put_ptr_field(gcr_addr, pipe_state_txdetectrx_clear, 0b1, fast_write); // all bits in this reg are self clearing
        }
        else   // error set FIR if pipe_reset active is not pending, pipe_reset asserted during txdetectrx transaction
        {
            uint32_t l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
            uint32_t l_pipe_state_resetn_active = l_pipe_state & pipe_state_resetn_active_mask;

            if (!l_pipe_state_resetn_active)
            {
                // PSL set_fir_fatal_error
                set_fir(fir_code_fatal_error);
            }
        } // txdetectrx_ok

        return;
    } // phase 1
} //pipe_cmd_txdetectrx

//////////////////////////////////
// RXELECIDLE_ACTIVE
//////////////////////////////////

//void pipe_cmd_rxelecidle_active(t_gcr_addr *gcr_addr) {
//  set_debug_state(0xFC0A, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_ACTIVE Transaction
//  put_ptr_field(gcr_addr, pipe_state_rxelecidle_active_clear, 0b1, fast_write);
//  return;
//} //pipe_cmd_rxelecidle_active

//////////////////////////////////
// RXELECIDLE_INACTIVE
//////////////////////////////////
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_addr == pipe_state_rxactive_addr);
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_addr == pipe_state_powerdown_addr);
PK_STATIC_ASSERT(pipe_state_status2_pl_full_reg_width == 16);
PK_STATIC_ASSERT(ppe_pipe_rx_ei_inactive_phase_0_3_width == 8); // 2 bits per lane
void pipe_cmd_rxelecidle_inactive(t_gcr_addr* gcr_addr)
{
    int l_lane = get_gcr_addr_lane(gcr_addr);

    // Small additional thread active time allowance for sleep optimization (EWM 303579)
    mem_pg_field_put(ppe_thread_active_time_us_limit, 14);

    // Determine if RX initialization was previously completed
    int l_rx_init_done = mem_pl_field_get(rx_init_done, l_lane);
    bool set_fir_on_error = false;

    // Determine the phase of this command (gray coded: 00, 01, 11)
    uint32_t l_pipe_rx_ei_inactive_phase_0_3 = mem_pg_field_get(ppe_pipe_rx_ei_inactive_phase_0_3);
    uint32_t l_phase = ( l_pipe_rx_ei_inactive_phase_0_3 >> (2 * (3 - l_lane)) ) & 0x03;

    /////////////
    // PHASE 0 //
    /////////////
    // PSL phase_zero
    if ((l_phase == 0b00) && (l_rx_init_done == 0))
    {
        set_debug_state(0xFC0B, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_INACTIVE Transaction

        uint32_t l_pipe_state_status2_regval = get_ptr_field(gcr_addr, pipe_state_status2_pl_full_reg);
        uint32_t l_pipe_state_rxactive  = bitfield_get(l_pipe_state_status2_regval, pipe_state_rxactive_mask,
                                          pipe_state_rxactive_shift);  // get pipe_state_rxactive
        uint32_t l_pipe_state_powerdown = bitfield_get(l_pipe_state_status2_regval, pipe_state_powerdown_mask,
                                          pipe_state_powerdown_shift); // get pipe_state_powerdown

        // PSL not_rxactive_and_powerdown_eq_0
        if ((!l_pipe_state_rxactive) && (l_pipe_state_powerdown == 0))                   // if rxactive=0 and powerdown=P0
        {
            // Set Phase 1 (01) for next call
            l_pipe_rx_ei_inactive_phase_0_3 |= (0x40 >> (2 * l_lane));
            mem_pg_field_put(ppe_pipe_rx_ei_inactive_phase_0_3, l_pipe_rx_ei_inactive_phase_0_3);

            // Disable the DL clock before calling initial training
            // eo_main handles switching Bank A to the cal bank, disabling the CDR, and resetting the flywheel
            set_gcr_addr_reg_id(gcr_addr, rx_group);                                        // select rx_group addressing
            put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0, read_modify_write);                  // Disable DL clock to avoid glitching

            // Run Phase 0 of training
            mem_pg_field_put(rx_eo_phase_select_0_2, 0b100);
            run_initial_training(gcr_addr, l_lane);                                         //  Run initial training

            // Return so don't do final handshakes when (rxactive=0 and powerdown=P0)
            return;
        } // if(rxactive=0 and powerdown=P0)
    } //if (phase0)

    /////////////
    // PHASE 1 //
    /////////////
    // PSL phase_one
    if (l_phase == 0b01)
    {
        set_debug_state(0xFC1B, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_INACTIVE Phase 1 Processing

        // Set Phase 2 (11) for next call
        l_pipe_rx_ei_inactive_phase_0_3 |= (0x80 >> (2 * l_lane));
        mem_pg_field_put(ppe_pipe_rx_ei_inactive_phase_0_3, l_pipe_rx_ei_inactive_phase_0_3);

        // Run Phase 1 of training
        set_gcr_addr_reg_id(gcr_addr, rx_group);
        mem_pg_field_put(rx_eo_phase_select_0_2, 0b010);
        run_initial_training(gcr_addr, l_lane);

        // Return so don't do final handshakes
        return;
    } //if (phase1)

    /////////////
    // PHASE 2 //
    /////////////
    // PSL phase_two
    if (l_phase == 0b11)
    {
        set_debug_state(0xFC2B, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_INACTIVE Phase 2 Processing

        // Clear phase (Phase 0, 00) for next call
        l_pipe_rx_ei_inactive_phase_0_3 &= ~(0xC0 >> (2 * l_lane));
        mem_pg_field_put(ppe_pipe_rx_ei_inactive_phase_0_3, l_pipe_rx_ei_inactive_phase_0_3);

        // Run Phase 2 of training
        set_gcr_addr_reg_id(gcr_addr, rx_group);
        mem_pg_field_put(rx_eo_phase_select_0_2, 0b001);
        run_initial_training(gcr_addr, l_lane);

        // Done with all training phases
        put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write);                  //  Enable RX DL Clock
        set_gcr_addr_reg_id(gcr_addr, tx_group);                                        //  select tx_group addressing
        put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, (pipe_state_rxactive_set_mask | pipe_state_rxdatavalid_set_mask),
                      fast_write); //  set pipe_state_rxactive, set pipe_state_rxdatavalid
    } //if (phase2)

    /////////////
    // PHASE 3 //
    /////////////
    // Do not run rx initialization if rx_init_done=1
    // Wait for CDR Lock
    // Enable DL Clock
    // Set pipe_state_rxactive
    // Set pipe_state_rxdatavalid
    /////////////
    // PSL phase_three
    if ((l_phase == 0b00) && (l_rx_init_done == 1))   // Phase 0 and rx initialization previously completed
    {
        set_debug_state(0xFC3B, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_INACTIVE Phase 3 Processing
        // Clear cdr lock detection, toggle flywheel reset, wait for cdr lock, enable cdr lockdetection
        set_gcr_addr_reg_id(gcr_addr, rx_group);
        put_ptr_field(gcr_addr, rx_dl_clk_en, 0b0,
                      read_modify_write);                            //EWM 307543: Prevent DL clock glitch from fw_reset
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis,
                      fast_write); //EWM 307543: Disable CDR during fw_reset
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, 0b11, fast_write);
        put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b11, read_modify_write);
        put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b00, read_modify_write);
        int bank_sel_a  = get_ptr_field(gcr_addr, rx_bank_sel_a);
        t_bank live_bank = (bank_sel_a == 1) ? bank_a : bank_b;
        int edge_track_cnt_val = (bank_sel_a == 1) ? cdr_a_lcl_cdr_b_dis : cdr_a_dis_cdr_b_lcl;
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, edge_track_cnt_val,
                      fast_write);  //EWM 307543: Reenable CDR after fw_reset
        wait_for_cdr_lock(gcr_addr, live_bank, set_fir_on_error);
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_set_alias, 0b11, fast_write);
        // Check that no pending rate change or power state changes exist
        set_gcr_addr_reg_id(gcr_addr, tx_group);
        uint32_t l_pipe_state = get_ptr_field(gcr_addr, pipe_state_0_15_alias);
        uint32_t l_pipe_state_rate_updated_or_powerdown_updated = l_pipe_state & (pipe_state_rate_updated_mask |
                pipe_state_powerdown_updated_mask);

        // PSL no_pending_rate_or_power_state_change
        if (l_pipe_state_rate_updated_or_powerdown_updated == 0)
        {
            set_gcr_addr_reg_id(gcr_addr, rx_group);
            put_ptr_field(gcr_addr, rx_dl_clk_en, 0b1, read_modify_write); // Enable rx_dl_clk
            set_gcr_addr_reg_id(gcr_addr, tx_group);
            put_ptr_field(gcr_addr, pipe_state_cntl1_pl_full_reg, (pipe_state_rxactive_set_mask | pipe_state_rxdatavalid_set_mask),
                          fast_write); //  set pipe_state_rxactive, set pipe_state_rxdatavalid
        }
    } //if (phase3)

    ////////////////
    // HANDSHAKES //
    ////////////////
    // Disable the rxelecidle enable and clear the rxelecidle_inactive state if finished with Phase 2 or if !(rxactive=0 and powerdown=P0)
    set_gcr_addr_reg_id(gcr_addr, tx_group);                                         // select tx_group addressing
    put_ptr_field(gcr_addr, pipe_state_rxelecidle_en_clear, 0b1, fast_write);        // disable rxelecidle enable
    put_ptr_field(gcr_addr, pipe_state_rxelecidle_inactive_clear, 0b1, fast_write);  // clear pipe_state_rxelecidle_inactive
    return;                                                                          // return
} //pipe_cmd_rxelecidle_inactive

//////////////////////////////////
// TXELECIDLE_ACTIVE
//////////////////////////////////

//void pipe_cmd_txelecidle_active(t_gcr_addr *gcr_addr) {
//  set_debug_state(0xFC0C, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_TXELECIDLE_ACTIVE Transaction
//  put_ptr_field(gcr_addr, pipe_state_txelecidle_active_clear, 0b1, fast_write);
//  return;
//} //pipe_cmd_txelecidle_active

//////////////////////////////////
// TXELECIDLE_INACTIVE
//////////////////////////////////

//void pipe_cmd_txelecidle_inactive(t_gcr_addr *gcr_addr) {
//  set_debug_state(0xFC0D, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXELECIDLE_INACTIVE Transaction
//  put_ptr_field(gcr_addr, pipe_state_txelecidle_inactive_clear, 0b1, fast_write);
//  return;
//} //pipe_cmd_txelecidle_inactive

//////////////////////////////////
// RXMARGINCONTROL
//////////////////////////////////
PK_STATIC_ASSERT(ppe_pipe_margin_mode_0_3_width == 4);
void pipe_cmd_rxmargincontrol(t_gcr_addr* gcr_addr)
{
    set_debug_state(0xFC0E, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_RXMARGINGCONTROL Transaction

    uint32_t l_pipe_state_rxmargin_full_reg = get_ptr_field(gcr_addr,
            pipe_state_rxmargin_full_reg);                                                        // read pipe_state_rxmargin register
    uint32_t l_pipe_state_rxmargin_sample_count_reset = pipe_state_rxmargin_sample_count_reset_mask &
            l_pipe_state_rxmargin_full_reg;                       // parse out rxmargin_sample_count_reset
    uint32_t l_pipe_state_rxmargin_error_count_reset = pipe_state_rxmargin_error_count_reset_mask &
            l_pipe_state_rxmargin_full_reg;                         // parse out rxmargin_error_count_reset
    uint32_t l_pipe_state_rxmargin_start = pipe_state_rxmargin_start_mask &
                                           l_pipe_state_rxmargin_full_reg;                                                 // parse out rxmargin_start
    uint32_t l_pipe_state_rxmargin_stop = pipe_state_rxmargin_stop_mask &
                                          l_pipe_state_rxmargin_full_reg;                                                   // parse out rxmargin_stop
    uint32_t l_pipe_state_rxmargin_offset_updated = pipe_state_rxmargin_offset_updated_mask &
            l_pipe_state_rxmargin_full_reg;                               // parse out rxmargin_offset_updated
//uint32_t l_pipe_state_rxmargin_count_timeout = pipe_state_rxmargin_count_timeout_mask & l_pipe_state_rxmargin_full_reg;                                 // parse out rxmargin_count_timeout

    int l_lane = get_gcr_addr_lane(
                     gcr_addr);                                                                                                               // get lane address

    //////////////////////////////////
    //  Start RX Margining
    //////////////////////////////////

    // PSL rxmargin_start
    if (l_pipe_state_rxmargin_start !=
        0)                                                                                                                   // if rx margining start
    {
        int l_pipe_margin_mode_0_3 = mem_pg_field_get(ppe_pipe_margin_mode_0_3) | (0x8 >>
                                     l_lane);                                                             // Set margin_mode for the lane
        mem_pg_field_put(ppe_pipe_margin_mode_0_3, l_pipe_margin_mode_0_3);
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        put_ptr_field(gcr_addr, rx_ber_pcie_mode, 0b1,
                      read_modify_write);                                                                                     //  set per group rx_ber_pcie_mode
        uint32_t l_rate = get_ptr_field(gcr_addr,
                                        pipe_state_rate);                                                                                            //  get pcie rate : gen1=0, gen2=1, gen3=2, gen4=3, gen5=4

        // PSL rate_eq_four
        if (l_rate ==
            4)                                                                                                                                       //  if rate is gen5 then
        {
            put_ptr_field(gcr_addr, rx_ber_timer_sel, 0b0101,
                          read_modify_write);                                                                                 //   set per group sample counter to 2**20
        }
        else                                                                                                                                                   //  else gen4
        {
            put_ptr_field(gcr_addr, rx_ber_timer_sel, 0b0100,
                          read_modify_write);                                                                                 //   set per group sample counter to 2**21
        };                                                                                                                                                     //  end if

        put_ptr_field(gcr_addr, rx_ber_en, 0b1,
                      read_modify_write);                                                                                            //  set per group rx_ber_en

        put_ptr_field(gcr_addr, rx_berpl_exp_data_sel, 0b0,
                      read_modify_write);                                                                                //  set per lane BER checker to main compared to alt data mode

        put_ptr_field(gcr_addr, rx_berpl_pattern_sel, 0b000,
                      read_modify_write);                                                                               //  disable per lane BER PRBS generator

        put_ptr_field(gcr_addr, rx_berpl_mask_mode, 0b0,
                      read_modify_write);                                                                                   //  disable per lane BER mask mode

        put_ptr_field(gcr_addr, rx_berpl_lane_invert, 0b0,
                      read_modify_write);                                                                                 //  disable per lane BER invert mode

        put_ptr_field(gcr_addr, rx_berpl_sat_thresh, 0b000000111111,
                      read_modify_write);                                                                       //  set per lane BER error counter threshold to 0x03F

        put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_timeout, 0b0,
                      read_modify_write);                                                                       //  clear periodic sample /

        put_ptr_field(gcr_addr, rx_berpl_count_en, 0b1,
                      read_modify_write);                                                                                    //  enable per lane BER counter

        set_gcr_addr_reg_id(gcr_addr,
                            tx_group);                                                                                                               //  set gcr addr to tx group

        uint32_t l_rx_margin_control_0 = pipe_get(gcr_addr, pipe_phy_reg_rx_margin_control_0_reg_sel,
                                         pipe_phy_reg_rx_margin_control_0_addr);                  //  read rx_margin_control0 PIPE PHY register

        // PSL sample_count_reset
        if (l_pipe_state_rxmargin_sample_count_reset !=
            0)                                                                                                     //  if rx_margin_control0.sample_count_reset was set (Sample Count Reset should be set)
        {
            rx_margin_clr_sample_count_and_wru(
                gcr_addr);                                                                                                         //   clear sample counter and WRU MAC RxMarginStatus1.SampleCount with 0
        }                                                                                                                                                      //  end if

        // PSL error_count_reset
        if (l_pipe_state_rxmargin_error_count_reset !=
            0)                                                                                                      //  if rx_margin_control0.error_count_reset was set (Error Count Reset should be set)
        {
            rx_margin_clr_error_count_and_wru(
                gcr_addr);                                                                                                          //   clear error counter and WRU MAC RxMarginStatus2.ErrorCount with 0
        }                                                                                                                                                      //  end if

        uint32_t l_rx_margining_v_or_t = (l_rx_margin_control_0 & 0x00000002) >>
                                         1;                                                                            //  determine if voltage or time margining is being applied via rx_margin_control0.margin_voltage_or_timing bit (0=voltage,1=time)
        uint32_t l_rx_margin_control_1 = pipe_get(gcr_addr, pipe_phy_reg_rx_margin_control_1_reg_sel,
                                         pipe_phy_reg_rx_margin_control_1_addr);                  //  read rx_margin_control1 PIPE PHY register
        uint32_t l_rx_margin_offset_signed_mag = (l_rx_margin_control_1 &
                0x000000FF);                                                                         //  create signed magnitude 8bit value of offset value
        int l_rx_margin_offset_int = SignedMagToInt(l_rx_margin_offset_signed_mag,
                                     8);                                                                         //  create interger value of offset value
        int l_rx_margin_offset_twos_comp = IntToTwosComp(l_rx_margin_offset_int,
                                           8);                                                                           //  create twos compliment value of offset value
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        bool l_rx_margin_voffset_ok = rxmargin_loffad_check(gcr_addr, l_lane, rx_ad_latch_dac_n000_alias_addr,
                                      rx_ad_latch_dac_w111_alias_addr,
                                      l_rx_margin_offset_int);   //  check voltage offset to ensure it does not overflow any bank A data LOFF DAC. PCIe only, so can ignore Issue 296947 (Odyssey address adjust).

        // PSL v_or_t_eq_0_and_voffset_ok
        if ((l_rx_margining_v_or_t == 0)
            && l_rx_margin_voffset_ok)                                                                                            //  if voltage margining and offset is within DAC range
        {
            mem_pl_field_put(rx_margin_voltage_offset, l_lane,
                             l_rx_margin_offset_int);                                                                          //   save voltage margin offset integer value to mem reg for future restore
            rxmargin_bank_enable(
                gcr_addr);                                                                                                                      //   set up banks for rx margining. Bank B is main. Bank A is cal.
            put_ptr_field(gcr_addr, rx_poff_adj, l_rx_margin_offset_twos_comp,
                          fast_write);                                                                      //   Update DAC accelerator offset value
            put_ptr_field(gcr_addr, rx_apply_poff_a_run_done_alias, 0b0100,
                          fast_write);                                                                         //   Update DAC accelerator to run bank A data offset updates and clear done bits, no need to check done bits
            rx_margin_clr_counts_wrc_status1(
                gcr_addr);                                                                                                          //   clear sample and error count and WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
            // PSL v_or_t_eq_1_and_minipr_check
        }
        else if ((l_rx_margining_v_or_t == 1)
                 && rxmargin_minipr_check(gcr_addr,
                                          l_rx_margin_offset_int))                                                    //  else if timing margining in range
        {
            rx_margin_apply_timing_offset(gcr_addr, l_lane,
                                          l_rx_margin_offset_int);                                                                             //   Apply RX timing margin offset
            rx_margin_clr_counts_wrc_status1(
                gcr_addr);                                                                                                          //   clear sample and error count and WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
        }
        else                                                                                                                                                   //  else invalid rx margining request
        {
            put_ptr_field(gcr_addr, pipe_state_rxmargin_start_clear, 0b1,
                          fast_write);                                                                           //   clear pipe_state_rxmargin_start   , get ready for next command prior to WRC
            rx_margin_clr_counts_wrc_status2(
                gcr_addr);                                                                                                          //   clear sample and error counts and WRC MAC RxMarginStatus0.MarginNak to 1 (bit 1)
        }                                                                                                                                                      //  end if

        l_pipe_state_rxmargin_sample_count_reset =
            0;                                                                                                          //  clear l_pipe_state_rxmargin_sample_count_reset
        l_pipe_state_rxmargin_error_count_reset =
            0;                                                                                                           //  clear l_pipe_state_rxmargin_error_count_reset
        l_pipe_state_rxmargin_offset_updated =
            0;                                                                                                              //  clear l_pipe_state_rxmargin_offset_updated
        l_pipe_state_rxmargin_start =
            0;                                                                                                                       //  clear l_pipe_state_rxmargin_start , get ready for next command
    }                                                                                                                                                       // end l_pipe_state_rxmargin_start

    //////////////////////////////////
    // Stop RX Margining
    //////////////////////////////////

    // PSL rxmargin_stop
    if (l_pipe_state_rxmargin_stop !=
        0)                                                                                                                    // if rx margining stop
    {
        int l_pipe_margin_mode_0_3 = mem_pg_field_get(ppe_pipe_margin_mode_0_3) & ~(0x8 >>
                                     l_lane);                                                            // Clear margin_mode for the lane
        mem_pg_field_put(ppe_pipe_margin_mode_0_3, l_pipe_margin_mode_0_3);
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        uint32_t l_rx_berpl_pcie_sample = get_rxmargin_sample_count(
                                              gcr_addr);                                                                                 //  read rxmargining sample count
        put_ptr_field(gcr_addr, rx_berpl_pcie_rx_margin_start, 0b0,
                      read_modify_write);                                                                        //  Stop Margining
        int l_rx_margin_voltage_offset_int = mem_pl_field_get(rx_margin_voltage_offset,
                                             l_lane);                                                               //  get voltage margin offset

        // PSL pcie_voltage_offset
        if (l_rx_margin_voltage_offset_int !=
            0)                                                                                                               //  if voltage margin is not zero then remove voltage marginging from DACs
        {
            rxmargin_bank_enable(
                gcr_addr);                                                                                                                      //   set up banks for rx margining. Bank B is main. Bank A is alt.
            put_ptr_field(gcr_addr, rx_poff_adj, IntToTwosComp(-l_rx_margin_voltage_offset_int, 8),
                          fast_write);                                                  //   Update DAC accelerator offset value to negative of stored voltage offset
            put_ptr_field(gcr_addr, rx_apply_poff_a_run_done_alias, 0b0100,
                          fast_write);                                                                         //   Update DAC accelerator to run bank A data offset updates and clear done bits, no need to check done bits
            mem_pl_field_put(rx_margin_voltage_offset, l_lane,
                             0);                                                                                               //   update mem reg voltage margin offset integer value to 0
        }                                                                                                                                                      //  end if

        rx_margin_remove_timing_offset(gcr_addr,
                                       l_lane);                                                                                                      //  Remove potential timing offset margining
        rxmargin_sample_count_update(gcr_addr,
                                     l_rx_berpl_pcie_sample);                                                                                        //  update sample count
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  setgcr addr to rx group
        uint32_t l_rx_berpl_pcie_error = get_ptr_field(gcr_addr,
                                         rx_berpl_count);                                                                              //  read error count
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl,
                      fast_write);                                                              //  Enable both CDRs to use local data
        //bool set_fir_on_error = false;                                                                                                                         //  initialize set_fir_on_error
        //TODO must clear bit_lock_done and only wait for that bank: wait_for_cdr_lock(gcr_addr, set_fir_on_error);                                                                                                         //  wait for cdr lock
        set_gcr_addr_reg_id(gcr_addr,
                            tx_group);                                                                                                               //  set gcr addr to tx group
        pipe_put(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_u,
                 l_rx_berpl_pcie_error);           //  WRU MAC RxMarginStatus2.ErrorCount with error
        l_pipe_state_rxmargin_sample_count_reset =
            0;                                                                                                          //  clear l_pipe_state_rxmargin_sample_count_reset
        l_pipe_state_rxmargin_error_count_reset =
            0;                                                                                                           //  clear l_pipe_state_rxmargin_error_count_reset
        l_pipe_state_rxmargin_offset_updated =
            0;                                                                                                              //  clear l_pipe_state_rxmargin_offset_updated
        rx_margin_clr_samplecount_errorcount_offsetupdated(
            gcr_addr);                                                                                          //  clear rx margin sample_count error_count and offset_updated
        l_pipe_state_rxmargin_stop =
            0;                                                                                                                        //  clear l_pipe_state_rxmargin_stop  , get ready for next command prior to WRC
        put_ptr_field(gcr_addr, pipe_state_rxmargin_stop_clear, 0b1,
                      fast_write);                                                                              //  clear pipe_state_rxmargin_stop    , get ready for next command prior to WRC
        put_ptr_field(gcr_addr, pipe_state_rxmargincontrol_clear, 0b1,
                      fast_write);                                                                            //  clear pipe_state_rxmargingcontrol , get ready for next command prior to WRC
        int l_orig_lane = get_gcr_addr_lane(
                              gcr_addr);                                                                                                         //  Disable group BER counter if all lanes are not running margining. Save original lane
        set_gcr_addr_reg_id_lane(gcr_addr, rx_group,
                                 bcast_all_lanes);                                                                                         //  Set to rx_group and broadcast to all lanes to produce ORed result from all lanes in group
        uint32_t l_rx_berpl_count_en_any = get_ptr_field(gcr_addr,
                                           rx_berpl_count_en);                                                                         //  get OR of rx_berpl_count_en for all lanes in group

        // PSL rx_berpl_count_en_any
        if (l_rx_berpl_count_en_any ==
            0)                                                                                                                      //  if any lane had rx_berpl_count_en set then
        {
            put_ptr_field(gcr_addr, rx_ber_en, 0,
                          read_modify_write);                                                                                             //   clear group level rx_ber_en bit
        }                                                                                                                                                      //  end if

        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        put_ptr_field(gcr_addr, rx_ber_pcie_mode, 0b0,
                      read_modify_write);                                                                                     //  clear per group rx_ber_pcie_mode when stopping rx margining
        set_gcr_addr_reg_id_lane(gcr_addr, tx_group,
                                 l_orig_lane);                                                                                             //  restore original lane number and set to tx_group addressing
        pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_0_reg_sel, pipe_mac_reg_rx_margin_status_0_addr, pipe_reg_cmd_wr_c,
                     0x00000001);                  //  WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set rx_group addressing
    }                                                                                                                                                       // end if

    //////////////////////////////////
    // RX Margining Offset Updated
    //////////////////////////////////

    // PSL rxmargin_offset_updated
    if (l_pipe_state_rxmargin_offset_updated !=
        0)                                                                                                          // if rx margin offset updated
    {
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_snapshot, 0b1,
                      fast_write);                                                                             //  snapshot sample and error counters
        put_ptr_field(gcr_addr, rx_berpl_pcie_rx_margin_start, 0b0,
                      read_modify_write);                                                                        //  stop sample and error counters

        // PSL offset_updated_sample_count_reset
        if (l_pipe_state_rxmargin_sample_count_reset !=
            0)                                                                                                     //  if rx_margin_control0.sample_count_reset was set (Sample Count Reset should be set)
        {
            rx_margin_clr_sample_count_and_wru(
                gcr_addr);                                                                                                         //   clear sample counter and WRU MAC RxMarginStatus1.SampleCount with 0
        }                                                                                                                                                      //  end if

        // PSL offset_updated_error_count_reset
        if (l_pipe_state_rxmargin_error_count_reset !=
            0)                                                                                                      //  if rx_margin_control0.error_count_reset was set (Error Count Reset should be set)
        {
            rx_margin_clr_error_count_and_wru(
                gcr_addr);                                                                                                          //   clear error counter and WRU MAC RxMarginStatus2.ErrorCount with 0
        }                                                                                                                                                      //  end if

        uint32_t l_rx_berpl_pcie_sample = get_ptr_field(gcr_addr,
                                          rx_berpl_pcie_sample);                                                                       //  Read raw sample count

        // PSL offset_updated_set_sample
        if ((l_rx_berpl_pcie_sample = 0) & (l_pipe_state_rxmargin_sample_count_reset =
                                                0))                                                                     //  Check to see if sample count is zero and sample was not reset
        {
            l_rx_berpl_pcie_sample =
                1;                                                                                                                           //   set sample count to 1 as group sample counter may not have overflowed yet
        }                                                                                                                                                      //  end if

        uint32_t l_rx_margin_control_0 = pipe_get(gcr_addr, pipe_phy_reg_rx_margin_control_0_reg_sel,
                                         pipe_phy_reg_rx_margin_control_0_addr);                  //  read rx_margin_control0 PIPE PHY register
        uint32_t l_rx_margining_v_or_t = (l_rx_margin_control_0 & 0x00000002) >>
                                         1;                                                                            //  determine if voltage or time margining is being applied via rx_margin_control0.margin_voltage_or_timing bit (0=voltage,1=time)
        uint32_t l_rx_margin_control_1 = pipe_get(gcr_addr, pipe_phy_reg_rx_margin_control_1_reg_sel,
                                         pipe_phy_reg_rx_margin_control_1_addr);                  //  read rx_margin_control1 PIPE PHY register
        uint32_t l_rx_margin_offset_signed_mag = (l_rx_margin_control_1 &
                0x000000FF);                                                                         //  create signed magnitude 8bit value of offset value
        int l_rx_margin_offset_int = SignedMagToInt(l_rx_margin_offset_signed_mag,
                                     8);                                                                         //  create interger value of offset value
        int l_rx_margin_offset_twos_comp = IntToTwosComp(l_rx_margin_offset_int,
                                           8);                                                                           //  create twos compliment value of offset value
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group

        // PSL offset_updated_v_or_t
        if (l_rx_margining_v_or_t ==
            0)                                                                                                                        //  if voltage margining first remove any prior voltage offset
        {
            int l_rx_margin_voltage_offset = mem_pl_field_get(rx_margin_voltage_offset,
                                             l_lane);                                                                  //   get voltage margin offset
            rxmargin_bank_enable(
                gcr_addr);                                                                                                                       //   set up banks for rx margining. Bank B is main. Bank A is alt.
            put_ptr_field(gcr_addr, rx_poff_adj, IntToTwosComp(-l_rx_margin_voltage_offset, 8),
                          fast_write);                                                       //   update DAC accelerator offset value to negative of stored voltage offset
            put_ptr_field(gcr_addr, rx_apply_poff_a_run_done_alias, 0b0100,
                          fast_write);                                                                          //   update DAC accelerator to run bank A data offset updates and clear done bits, no need to check done bits
            mem_pl_field_put(rx_margin_voltage_offset, l_lane,
                             0);                                                                                                //   update mem reg voltage margin offset integer value to 0
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl,
                          fast_write);                                                             //   Enable both CDRs to use local data
            //bool set_fir_on_error = false;                                                                                                                        //   initialize set_fir_on_error
            //TODO must clear bit_lock_done and only wait for that bank: wait_for_cdr_lock(gcr_addr, set_fir_on_error);                                                                                                        //   wait for cdr lock
            bool l_rx_margin_voffset_ok = rxmargin_loffad_check(gcr_addr, l_lane, rx_ad_latch_dac_n000_alias_addr,
                                          rx_ad_latch_dac_w111_alias_addr,
                                          l_rx_margin_offset_int);  //   check voltage offset to ensure it does not overflow any bank A data LOFF DAC. PCIe only, so can ignore Issue 296947 (Odyssey address adjust).

            // PSL offset_updated_ok
            if (l_rx_margin_voffset_ok)                                                                                                                           //   if voltage margin offset is within DAC range
            {
                mem_pl_field_put(rx_margin_voltage_offset, l_lane,
                                 l_rx_margin_offset_int);                                                                          //    save voltage margin offset integer value to mem reg for future restore
                rxmargin_bank_enable(
                    gcr_addr);                                                                                                                      //    set up banks for rx margining. Bank B is main. Bank A is cal.
                put_ptr_field(gcr_addr, rx_poff_adj, l_rx_margin_offset_twos_comp,
                              fast_write);                                                                      //    Update DAC accelerator offset value
                put_ptr_field(gcr_addr, rx_apply_poff_a_run_done_alias, 0b0100,
                              fast_write);                                                                         //    Update DAC accelerator to run bank A data offset updates and clear done bits, no need to check done bits
                l_pipe_state_rxmargin_sample_count_reset =
                    0;                                                                                                        //    clear l_pipe_state_rxmargin_sample_count_reset
                l_pipe_state_rxmargin_sample_count_reset =
                    0;                                                                                                        //    clear l_pipe_state_rxmargin_sample_count_reset
                l_pipe_state_rxmargin_error_count_reset =
                    0;                                                                                                         //    clear l_pipe_state_rxmargin_error_count_reset
                l_pipe_state_rxmargin_offset_updated =
                    0;                                                                                                            //    clear l_pipe_state_rxmargin_offset_updated
                rx_margin_clr_counts_wrc_status1(
                    gcr_addr);                                                                                                          //    clear sample and error count and WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
            }
            else                                                                                                                                                  //   else voltage margin offset exceeds DAC range. Voltage margin already removed. Remove any timing margin. NAK rx margining request.
            {
                l_pipe_state_rxmargin_sample_count_reset =
                    0;                                                                                                        //    clear l_pipe_state_rxmargin_sample_count_reset
                l_pipe_state_rxmargin_error_count_reset =
                    0;                                                                                                         //    clear l_pipe_state_rxmargin_error_count_reset
                l_pipe_state_rxmargin_offset_updated =
                    0;                                                                                                            //    clear l_pipe_state_rxmargin_offset_updated
                rx_margin_remove_timing_offset(gcr_addr,
                                               l_lane);                                                                                                    //    Remove potential timing offset margining
                rx_margin_clr_counts_wrc_status2(
                    gcr_addr);                                                                                                          //    clear sample and error counts and WRC MAC RxMarginStatus0.MarginNak to 1 (bit 1)
            }                                                                                                                                                     //   end if
        }
        else                                                                                                                                                   //  else timing margining then remove any prior timing offset
        {
            rx_margin_remove_timing_offset(gcr_addr,
                                           l_lane);                                                                                                     //   Remove potential timing offset margining
            mem_pl_field_put(rx_margin_timing_offset, l_lane,
                             0);                                                                                                 //   update mem reg timing margin offset integer value to 0
            put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl,
                          fast_write);                                                             //   Enable both CDRs to use local data

            //bool set_fir_on_error = false;                                                                                                                        //   initialize set_fir_on_error
            //TODO must clear bit_lock_done and only wait for that bank: wait_for_cdr_lock(gcr_addr, set_fir_on_error);                                                                                                        //   wait for cdr lock
            // PSL minipr_check
            if (rxmargin_minipr_check(gcr_addr,
                                      l_rx_margin_offset_int))                                                                                          //   if timing offset is within mini PR range, then apply timing offset and WRC
            {
                rx_margin_apply_timing_offset(gcr_addr, l_lane,
                                              l_rx_margin_offset_int);                                                                             //    Apply RX timing margin offset
                mem_pl_field_put(rx_margin_timing_offset, l_lane,
                                 0);                                                                                                //    update mem reg timing margin offset integer value to 0
                put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_lcl,
                              fast_write);                                                            //    Enable both CDRs to use local data
                //bool set_fir_on_error = false;                                                                                                                       //    initialize set_fir_on_error
                //TODO must clear bit_lock_done and only wait for that bank: wait_for_cdr_lock(gcr_addr, set_fir_on_error);                                                                                                       //    wait for cdr lock
                l_pipe_state_rxmargin_sample_count_reset =
                    0;                                                                                                        //    clear l_pipe_state_rxmargin_sample_count_reset
                l_pipe_state_rxmargin_error_count_reset =
                    0;                                                                                                         //    clear l_pipe_state_rxmargin_error_count_reset
                l_pipe_state_rxmargin_offset_updated =
                    0;                                                                                                            //    clear l_pipe_state_rxmargin_offset_updated
                rx_margin_clr_counts_wrc_status1(
                    gcr_addr);                                                                                                          //    clear sample and error count and WRC MAC RxMarginStatus0.MarginStatus to 1 (bit 0)
            }
            else                                                                                                                                                  //   else timing offset is greater than mini pr range. Remove any voltage or timing margining. NAK rx margining request.
            {
                int l_rx_margin_voltage_offset = mem_pl_field_get(rx_margin_voltage_offset,
                                                 l_lane);                                                                 //    get voltage margin offset

                // PSL voltage_offset_ne_0
                if (l_rx_margin_voltage_offset !=
                    0)                                                                                                                 //    if voltage margin is not zero then remove voltage marginging from DACs
                {
                    set_cal_bank(gcr_addr,
                                 bank_a);                                                                                                                    //     Set Bank B as Main, Bank A as Alt (margined bank)
                    put_ptr_field(gcr_addr, rx_poff_adj, IntToTwosComp(-l_rx_margin_voltage_offset, 8),
                                  fast_write);                                                    //     Update DAC accelerator offset value to negative of stored voltage offset
                    put_ptr_field(gcr_addr, rx_apply_poff_a_run_done_alias, 0b0100,
                                  fast_write);                                                                       //     Update DAC accelerator to run bank A data offset updates and clear done bits, no need to check done bits
                    mem_pl_field_put(rx_margin_voltage_offset, l_lane,
                                     0);                                                                                             //     update mem reg voltage margin offset integer value to 0
                }                                                                                                                                                    //    end if

                l_pipe_state_rxmargin_sample_count_reset =
                    0;                                                                                                        //    clear l_pipe_state_rxmargin_sample_count_reset
                l_pipe_state_rxmargin_error_count_reset =
                    0;                                                                                                         //    clear l_pipe_state_rxmargin_error_count_reset
                l_pipe_state_rxmargin_offset_updated =
                    0;                                                                                                            //    clear l_pipe_state_rxmargin_offset_updated
                rx_margin_remove_timing_offset(gcr_addr,
                                               l_lane);                                                                                                    //    Remove potential timing offset margining
                rx_margin_clr_counts_wrc_status2(
                    gcr_addr);                                                                                                          //    clear sample and error counts and WRC MAC RxMarginStatus0.MarginNak to 1 (bit 1)
            }                                                                                                                                                     //   end if
        }                                                                                                                                                      //  end if
    }                                                                                                                                                       // end if margin updated

/////////////////////////////////////
//// Reset RX Margining Sample Count Only with no Offset Update (Should Never Occur)
/////////////////////////////////////
//
//// PSL sample_reset
//if ((l_pipe_state_rxmargin_sample_count_reset != 0) && (l_pipe_state_rxmargin_error_count_reset == 0) && (l_pipe_state_rxmargin_offset_updated == 0)) { // if sample count reset is active and error count reset is inactive and offset_updated is inactive
// set_gcr_addr_reg_id(gcr_addr, rx_group);                                                                                                               //  set gcr addr to rx group
// put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_snapshot, 0b1, fast_write);                                                                             //  Snapshot sample and error counters
// uint32_t l_rx_berpl_pcie_error = get_ptr_field(gcr_addr, rx_berpl_count);                                                                              //  read error count
// rx_margin_clr_sample_count_and_wru(gcr_addr);                                                                                                          //  clear sample count and WRU MAC RxMarginStatus1.SampleCount with 0
// l_pipe_state_rxmargin_sample_count_reset  = 0;                                                                                                         //  clear l_pipe_state_rxmargin_sample_count_reset
// put_ptr_field(gcr_addr, pipe_state_rxmargin_sample_count_reset_clear, 0b1, fast_write);                                                                //  clear rxmargin_sample_count_reset
// pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_c, l_rx_berpl_pcie_error);       //  WRC MAC RxMarginStatus2.ErrorCount with error
//}                                                                                                                                                       // end if

    ///////////////////////////////////
    // Reset RX Margining Error Count Only with no Offset Update
    ///////////////////////////////////

    // PSL error_reset
    if ((l_pipe_state_rxmargin_sample_count_reset == 0) && (l_pipe_state_rxmargin_error_count_reset != 0)
        && (l_pipe_state_rxmargin_offset_updated ==
            0))   // if sample count reset is inactive and error count reset is active and offset_updated is inactive
    {
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  setgcr addr to rx group
        put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_snapshot, 0b1,
                      fast_write);                                                                             //  Snapshot sample and error counters
        put_ptr_field(gcr_addr, rx_berpl_pcie_error_reset, 0b1,
                      fast_write);                                                                                   //  clear error count
        uint32_t l_rx_berpl_pcie_sample = get_ptr_field(gcr_addr,
                                          rx_berpl_pcie_sample);                                                                       //  Read raw sample count
        rxmargin_sample_count_update(gcr_addr,
                                     l_rx_berpl_pcie_sample);                                                                                        //  update sample count
        l_pipe_state_rxmargin_error_count_reset =
            0;                                                                                                           //  clear l_pipe_state_rxmargin_error_count_reset
        put_ptr_field(gcr_addr, pipe_state_rxmargin_error_count_reset_clear, 0b1,
                      fast_write);                                                                 //  set rxmargin_error_count_reset
        pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_c,
                     0);                           //  WRC MAC RxMarginStatus2.ErrorCount with zero
    }                                                                                                                                                       // end if

/////////////////////////////////////
//// Reset RX Margining Sample Count and Error Count with no Offset Update (Should Never Occur)
/////////////////////////////////////
//
//// PSL sample_reset_and_error_reset
//if ((l_pipe_state_rxmargin_sample_count_reset != 0) && (l_pipe_state_rxmargin_error_count_reset != 0) && (l_pipe_state_rxmargin_offset_updated == 0)) { // if sample count reset is active and error count reset is active and offset_updated is inactive
// set_gcr_addr_reg_id(gcr_addr, rx_group);                                                                                                               //  set gcr addr to rx group
// put_ptr_field(gcr_addr, rx_berpl_pcie_error_reset, 0b1, fast_write);                                                                                   //  clear error count
// rx_margin_clr_sample_count_and_wru(gcr_addr);                                                                                                          //  clear sample count and WRU MAC RxMarginStatus1.SampleCount with 0
// l_pipe_state_rxmargin_sample_count_reset  = 0;                                                                                                         //  clear l_pipe_state_rxmargin_sample_count_reset
// put_ptr_field(gcr_addr, pipe_state_rxmargin_sample_count_reset_clear, 0b1, fast_write);                                                                //  clear rxmargin_sample_count_reset
// l_pipe_state_rxmargin_error_count_reset = 0;                                                                                                           //  clear l_pipe_state_rxmargin_error_count_reset
// put_ptr_field(gcr_addr, pipe_state_rxmargin_error_count_reset_clear, 0b1, fast_write);                                                                 //  set rxmargin_error_count_reset
// pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_c, 0);                           //  WRC MAC RxMarginStatus2.ErrorCount with zero
//}                                                                                                                                                       // end if

    //////////////////////////////////
    // RX Margining Count Timeout
    //////////////////////////////////
    // Timeout RX Margining requests need to poll rx_berpl_pcie_samp_err_timeout register when l_pipe_state_rxmargincontrol is active
    //////////////////////////////////

    set_gcr_addr_reg_id(gcr_addr,
                        rx_group);                                                                                                                // set gcr addr to rx group
    uint32_t l_rx_berpl_pcie_full_reg = get_ptr_field(gcr_addr,
                                        rx_berpl_pcie_full_reg);                                                                    // read rx_berpl_pcie_full_reg register
    uint32_t l_rx_berpl_pcie_samp_err_timeout = rx_berpl_pcie_samp_err_timeout_mask &
            l_rx_berpl_pcie_full_reg;                                             // get rx_berpl_pcie_samp_err_timeout

    // PSL samp_err_timeout
    if (l_rx_berpl_pcie_samp_err_timeout !=
        0)                                                                                                              // if rx_berpl_pcie_samp_err_timeout is non zero then
    {
        uint32_t l_rx_berpl_pcie_sample = get_rxmargin_sample_count(
                                              gcr_addr);                                                                                 //  read rxmargining sample count
        rxmargin_sample_count_update(gcr_addr,
                                     l_rx_berpl_pcie_sample);                                                                                        //  convert sample count and WRU to MAC
        set_gcr_addr_reg_id(gcr_addr,
                            rx_group);                                                                                                               //  set gcr addr to rx group
        uint32_t l_rx_berpl_pcie_error = get_ptr_field(gcr_addr,
                                         rx_berpl_count);                                                                              //  get error count
        l_rx_berpl_pcie_samp_err_timeout =
            0;                                                                                                                  //  clear l_rx_berpl_pcie_samp_err_timeout
        put_ptr_field(gcr_addr, rx_berpl_pcie_samp_err_timeout, 0b0,
                      read_modify_write);                                                                       //  clear rx_berpl_pcie_samp_err_timeout, get ready for next command prior to WRC
        set_gcr_addr_reg_id(gcr_addr,
                            tx_group);                                                                                                               //  set gcr addr to tx group
        pipe_put_blk(gcr_addr, pipe_mac_reg_rx_margin_status_2_reg_sel, pipe_mac_reg_rx_margin_status_2_addr, pipe_reg_cmd_wr_c,
                     l_rx_berpl_pcie_error);       //  WRC error count to MAC
    }                                                                                                                                                       // end if

    return;                                                                                                                                                 // return
} //pipe_cmd_rxmargincontrol

//////////////////////////////////
// TXDEEMPH_UPDATED
//////////////////////////////////

//void pipe_cmd_txdeemph_updated(t_gcr_addr *gcr_addr) {
//  set_debug_state(0xFC0F, PIPE_IFC_DBG_LVL); // PIPE: Start of PIPE_TXDEEMPH_UPDATED Transaction
//  put_ptr_field(gcr_addr, pipe_state_txdeemph_updated_clear, 0b1, fast_write);
//  return;
//} //pipe_cmd_txdeemph_updated

//////////////////////////////////////////////////////
// PIPE Command Handler Table and Assumptions
//////////////////////////////////////////////////////

IOO_PIPE_CMD_TABLE_START
IOO_PIPE_CMD_HANDLER(pipe_cmd_resetn_active)         // [00]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_resetn_inactive)       // [01]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [02]: Unused. rxstandby handled in hardware.
IOO_PIPE_CMD_HANDLER(pipe_cmd_pclkchangeack_active)  // [03]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_rate_updated)          // [04]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_powerdown_updated)     // [05]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_rxeqeval)              // [06]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_txdetectrx)            // [07]:
IOO_PIPE_CMD_HANDLER(
    pipe_cmd_nop)                   // [08]: Unused. pipe_cmd_rxelecidle_active monitored via pipe_state_rxactive.
IOO_PIPE_CMD_HANDLER(pipe_cmd_rxelecidle_inactive)   // [09]:
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [10]: Unused. pipe_cmd_txelecidle_active unused.
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [11]: Unused. pipe_cmd_txelecidle_inactive unused.
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [12]: Unused: pipe_cmd_rxmargincontrol unused.
IOO_PIPE_CMD_HANDLER(
    pipe_cmd_nop)                   // [13]: Unused. pipe_cmd_txdeemph_updated used only for monitoring state .
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [14]: Reserved.
IOO_PIPE_CMD_HANDLER(pipe_cmd_nop)                   // [15]: Reserved.
IOO_PIPE_CMD_TABLE_END

PK_STATIC_ASSERT(pipe_state_resetn_active_startbit == 0);
PK_STATIC_ASSERT(pipe_state_resetn_inactive_startbit == 1);
PK_STATIC_ASSERT(pipe_state_rxstandby_active_startbit == 2);
PK_STATIC_ASSERT(pipe_state_pclkchangeack_startbit == 3);
PK_STATIC_ASSERT(pipe_state_rate_updated_startbit == 4);
PK_STATIC_ASSERT(pipe_state_powerdown_updated_startbit == 5);
PK_STATIC_ASSERT(pipe_state_rxeqeval_startbit == 6);
PK_STATIC_ASSERT(pipe_state_txdetectrx_startbit == 7);
PK_STATIC_ASSERT(pipe_state_rxelecidle_active_startbit == 8);
PK_STATIC_ASSERT(pipe_state_rxelecidle_inactive_startbit == 9);
PK_STATIC_ASSERT(pipe_state_txelecidle_active_startbit == 10);
PK_STATIC_ASSERT(pipe_state_txelecidle_inactive_startbit == 11);
PK_STATIC_ASSERT(pipe_state_rxmargincontrol_startbit == 12);
PK_STATIC_ASSERT(pipe_state_txdeemph_updated_startbit == 13);

PK_STATIC_ASSERT(pipe_state_resetn_active_startbit == pipe_state_resetn_active_clear_startbit);
PK_STATIC_ASSERT(pipe_state_resetn_inactive_startbit == pipe_state_resetn_inactive_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rxstandby_active_startbit == pipe_state_rxstandby_active_clear_startbit);
PK_STATIC_ASSERT(pipe_state_pclkchangeack_startbit == pipe_state_pclkchangeack_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rate_updated_startbit == pipe_state_rate_updated_clear_startbit);
PK_STATIC_ASSERT(pipe_state_powerdown_updated_startbit == pipe_state_powerdown_updated_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rxeqeval_startbit == pipe_state_rxeqeval_clear_startbit);
PK_STATIC_ASSERT(pipe_state_txdetectrx_startbit == pipe_state_txdetectrx_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rxelecidle_active_startbit == pipe_state_rxelecidle_active_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rxelecidle_inactive_startbit == pipe_state_rxelecidle_inactive_clear_startbit);
PK_STATIC_ASSERT(pipe_state_txelecidle_active_startbit == pipe_state_txelecidle_active_clear_startbit);
PK_STATIC_ASSERT(pipe_state_txelecidle_inactive_startbit == pipe_state_txelecidle_inactive_clear_startbit);
PK_STATIC_ASSERT(pipe_state_rxmargincontrol_startbit == pipe_state_rxmargincontrol_clear_startbit);
PK_STATIC_ASSERT(pipe_state_txdeemph_updated_startbit == pipe_state_txdeemph_updated_clear_startbit);
