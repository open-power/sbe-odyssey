/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_common.h $    */
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
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_common.h
// *! TITLE       :
// *! DESCRIPTION : Common functions and defines for IOO eyeopt steps
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// dmb22062100 |dmb     | HW282905 Changed PPE_INIT_CAL_TIME_US_BUDGET_AXO_1th to 433
// mwh22060600 |mwh     | Changed PPE_INIT_CAL_TIME_US_BUDGET_AXO_1th to 410 add code
// dmb22032900 |dmb     | Changed PPE_INIT_CAL_TIME_US_BUDGET_AXO_1th to 400 for PR1538
// jfg22031100 |jfg     | Add PPE_INIT_CAL_TIME_US_BUDGET_* for use in eo_main
// mbs22021000 |mbs     | Updates to reduce code size
// vbr22011201 |vbr     | Added dummy get lane bad functions
// jfg21120100 |jfg     | Move t_seek to ioo_common
// mwh21101900 |mwh     | Add poff_avg code make common for all three methods of path offset
// vbr21102600 |vbr     | Added rx peak/lte preset function
// vbr21101900 |vbr     | Added tx dcc save/restore functions
// vbr21101200 |vbr     | Added latch dac save/restore functions
// vbr21092200 |vbr     | Moved bank_sel controls to own register so can fast write.
// vbr21052700 |vbr     | Rename sl*ve_mode to external_mode
// mwh21092300 |mwh     | Moved tx_write_4_bit_pat and in_tx_half_width_mode into libs because need be common for dcc iot and ioo
// vbr21052700 |vbr     | Rename slave_mode to external_mode
// vbr21040100 |vbr     | Added CDR edge track control constants.
// vbr21020500 |vbr     | Moved cal_mode definition here. Moved ioo/iot common funcs to io_lib.
// vbr20121100 |vbr     | Removed P10 dd1 code
// vbr20043000 |vbr     | Copied efficient servo queue check from eo_lte() into common function for use by other steps
// vbr20030400 |vbr     | Added function to switch just the data and rlm_clk (not dl_clk)
//-------------|P11 ----|-------------------------------------------------------
// mbs21041200 |mbs     | Removed eo_copy_lane_cal (no longer used)
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// bja20092800 |bja     | Use shared constants for TX register overrides
// bja20090800 |bja     | HW542315: make in_tx_half_width_mode() return false for p10 dd1 omi
// gap20082500 |gap     | HW542315: add tx_write_4_bit_pat and in_tx_half_width_mode for half/full width
// mwh20022400 |mwh     | Add in function check_for_rxpsave_req_sts and check_for_txpsave_req_sts
// vbr20021100 |vbr     | HW522731: smarter lane cal copy based on flywheel lane.
// vbr20020600 |vbr     | Added set/clr_rx_lane_bad() functions
// bja20020500 |bja     | Add tx_fifo_init()
// vbr19080900 |vbr     | Added macro to return just the bank_sel_a/rlm_clk_sel_a values
// vbr19060300 |vbr     | HW486157/HW492011: Added DL/RLM clock_sel to set_cal_bank()
// mbs10051600 |mbs     | HW491617: Separated servo setup for dfe_fast and dfe_full
// vbr19012200 |vbr     | Only check recal abort when init_done.
// vbr18100200 |vbr     | wait_for_cdr_lock() now has an option to set the fir on a lock fail.
// cws18092600 |cws     | Added rx_eo_servo_setup()
// vbr18081500 |vbr     | Updated return codes
// jfg18082000 |jfg     | Expand pr constants to normal 0/31
// jfg18080300 |jfg     | Add phase rotator count limit constant
// vbr18033100 |vbr     | Updated CDR controls.
// vbr18022000 |vbr     | Updating CDR controls.
// vbr17081400 |vbr     | Changed t_bank to an enumerated type and removed edge bank.
// vbr16021600 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _IOO_COMMON_H_
#define _IOO_COMMON_H_

#include <stdbool.h>
#include <stdint.h>
//#include <stdlib.h> have had problems with abs() linking properly...

#include "io_lib.h"
#include "pk.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


///////////////////////////////////////
// CONSTANTS & DEFINITIONS
///////////////////////////////////////

// Banks, t_cal_bank variable type and values (chosen to minimize code size); macros must match.
typedef enum
{
    bank_a = 0b010,
    bank_b = 0b101
} t_bank;
// Macro to convert bank value to the opposite bank value (A->B or B->A)
#define opposite_bank(input_bank) ((input_bank) ^ 0b111)
// Macro to convert cal_bank to the bank_sel_a/rlm_clk_sel_a/dl_clk_sel_a register values without using IF statements (branches).
//   The DL clock always comes from the Main bank and the RLM clock always comes from the Alt bank.
//   When Cal A: use B for main data, A for alt data. bank_sel_a=0; rlm_clk_sel_a=1; dl_clk_sel_a=0.
//   When Cal B: use A for main data, B for alt data. bank_sel_a=1; rlm_clk_sel_a=0; dl_clk_sel_a=1.
#define cal_bank_to_bank_rlmclk_dlclk_sel_a(cal_bank) ((cal_bank))
#define cal_bank_to_bank_rlmclk_sel_a(cal_bank) ((cal_bank) >> 1)
PK_STATIC_ASSERT(rx_bank_rlmclk_dlclk_sel_a_full_reg_width == 16);
PK_STATIC_ASSERT(rx_bank_rlmclk_dlclk_sel_a_alias_width == 3);
PK_STATIC_ASSERT(rx_bank_rlmclk_dlclk_sel_a_alias_startbit == 13);
PK_STATIC_ASSERT(rx_bank_rlmclk_sel_a_alias_width == 2);
PK_STATIC_ASSERT(rx_bank_rlmclk_sel_a_alias_startbit == 13);
PK_STATIC_ASSERT(rx_bank_sel_a_startbit    == 13);
PK_STATIC_ASSERT(rx_rlm_clk_sel_a_startbit == 14);
PK_STATIC_ASSERT(rx_dl_clk_sel_a_startbit  == 15);

// Constants for setting the CDRs into specific modes
#define cdr_a_dis_cdr_b_dis        0b000000
#define cdr_a_lcl_cdr_b_lcl        0b100100
#define cdr_a_lcl_cdr_b_ext        0b100110
#define cdr_a_ext_cdr_b_lcl        0b110100
#define cdr_a_lcl_cdr_b_lcl_align  0b100101
#define cdr_a_lcl_align_cdr_b_lcl  0b101100
PK_STATIC_ASSERT(rx_pr_edge_track_cntl_full_reg_width == 16);
PK_STATIC_ASSERT(rx_pr_edge_track_cntl_ab_alias_width == 6);
PK_STATIC_ASSERT(rx_pr_edge_track_cntl_ab_alias_startbit == 0);
PK_STATIC_ASSERT(rx_pr_enable_a_startbit == 0);
PK_STATIC_ASSERT(rx_pr_enable_b_startbit == 3);
PK_STATIC_ASSERT(rx_pr_external_mode_a_startbit == 1);
PK_STATIC_ASSERT(rx_pr_external_mode_b_startbit == 4);
PK_STATIC_ASSERT(rx_pr_bank_align_mode_a_startbit == 2);
PK_STATIC_ASSERT(rx_pr_bank_align_mode_b_startbit == 5);

// Initial training calibration mode.  This varies based on which phase of PCIe training we are in.
typedef enum
{
    C_PCIE_GEN1_CAL = 0,
    C_PCIE_GEN2_CAL = 1,
    C_PCIE_GEN3_CAL = 2,
    C_PCIE_GEN4_CAL = 3,
    C_PCIE_GEN5_CAL = 4,
    C_AXO_CAL = 7,
} t_init_cal_mode;

// PPE Initial Calibration Runtime limits as specified in Workbook 4.5 Calibration Main Loop
// See eo_main.c
// Added with 4% margin
#define PPE_INIT_CAL_TIME_US_BUDGET_PCIE_4TH 360
#define PPE_INIT_CAL_TIME_US_BUDGET_PCIE_1TH 255
#define PPE_INIT_CAL_TIME_US_BUDGET_AXO_4TH  645
//#define PPE_INIT_CAL_TIME_US_BUDGET_AXO_1TH  385 //real 330-382

// ***** Note: As of 6/22/22 (for HW282905) data is showing that the
//   5-sigma cutoff is ~433 US. If we hit this error again we'll
//   need to re-evaluate the data and init cal.
#define PPE_INIT_CAL_TIME_US_BUDGET_AXO_1TH  433

// Phase rotator size constants
#define pr_mini_min 0x00
#define pr_mini_max 0x1F
#define pr_mini_cen 0x10

// TX post-RIT register override constants
#define TX_DCC_SEL_ALIAS_DEFAULT 0b10

// Used by DDC and QPA PR searching functions
// Order is used to iterate. noseekNS and noseekEW must remain after doseek
typedef enum  {noseek, doseek, noseekNS, noseekEW} t_seek;

// Making a procedure inline means that the compiled code is copied to the location it is used instead of having to jump to the code location.
// This saves a jump/return and should also save having to push values onto the stack (which is additional code).
// However, the code in an inline procedure gets repeated which potentially increases the binary size.
// In general, large procedures should never be inline; short procedures where you would spend more time pushing/jumping should be inline.
// If a function refers to its address, that function can not be inlined (compiler will give an error).
// For more info: https://gcc.gnu.org/onlinedocs/gcc/Inline.html

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OTHER FUNCTIONS
// These all get used a fair amount so they should only be inline if they are very simple/short.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Set the bank to cal (A or B). Controls data and clock selects (DL and RLM).
static inline void set_cal_bank(t_gcr_addr* gcr_addr, t_bank new_cal_bank)
{
    unsigned int bank_rlmclk_dlclk_sel_a = cal_bank_to_bank_rlmclk_dlclk_sel_a(new_cal_bank);
    put_ptr_field(gcr_addr, rx_bank_rlmclk_dlclk_sel_a_alias, bank_rlmclk_dlclk_sel_a,
                  fast_write); // only field in this register
}

// Set the bank to cal (A or B). Controls data and RLM clock selects; DL clock select is left unchanged.
static inline void set_cal_bank_leave_dlclk(t_gcr_addr* gcr_addr, t_bank new_cal_bank)
{
    unsigned int bank_rlmclk_sel_a = cal_bank_to_bank_rlmclk_sel_a(new_cal_bank);
    put_ptr_field(gcr_addr, rx_bank_rlmclk_sel_a_alias, bank_rlmclk_sel_a, read_modify_write);
}

// Switch the bank between A & B.  Controls data and clock selects (DL and RLM).
static inline t_bank switch_cal_bank(t_gcr_addr* gcr_addr, t_bank current_cal_bank)
{
    t_bank new_cal_bank = opposite_bank(current_cal_bank);
    set_cal_bank(gcr_addr, new_cal_bank);
    return new_cal_bank;
}


// Wait for lock on both banks.
// Set FIR if not locked after timeout (optional).
// Lock could take relatively long due to spread spectrum, so use sleep in between checks.
int wait_for_cdr_lock(t_gcr_addr* gcr_addr, bool set_fir_on_error);


// Eyeopt abort check/handling. Call this a lot and does enough that inline is not the right option.
int check_rx_abort(t_gcr_addr* gcr_addr);


// Take ALT bank out of psave and wait for it to finish powering up
void alt_bank_psave_clear_and_wait(t_gcr_addr* gcr_addr);


// Set the AMP servo op config based on the calibration step
typedef enum
{
    SERVO_SETUP_VGA      = 0x00,
    SERVO_SETUP_DFE_FAST = 0x01,
    SERVO_SETUP_DFE_FULL = 0x02
} t_amp_servo_setup;
void rx_eo_amp_servo_setup(t_gcr_addr* i_tgt, const t_amp_servo_setup i_servo_setup);

// Set the LOFF servo op config based on the calibration step
typedef enum
{
    SERVO_SETUP_LOFF = 0x00,
    SERVO_SETUP_EOFF = 0x01
} t_loff_servo_setup;
void rx_eo_loff_servo_setup(t_gcr_addr* i_tgt, const t_loff_servo_setup i_servo_setup);

//Check that tx psave is quiesced and that req is not = 1 will set fir
int check_for_txpsave_req_sts(t_gcr_addr* gcr_addr);

//Check that tx psave is quiesced and that req is not = 1 will set fir
int check_for_rxpsave_req_sts(t_gcr_addr* gcr_addr);

//for updating poff_avg_a/b across all 3 diff methods
void eo_update_poff_avg( t_gcr_addr* gcr_addr, int poff_value, t_bank bank, int lane);


// Functions for using the DAC / Mini-PR accelerators
// This typedef must use these values since it is based on the register arrangement bits[0:3]: ae_run, ad_run, ae_done, ad_done
typedef enum
{
    data_only     = 0x4,
    edge_only     = 0x8,
    data_and_edge = 0xC
} t_data_edge_dac_sel;
void apply_rx_dac_offset(t_gcr_addr* gcr_addr, t_data_edge_dac_sel dac_sel, t_bank bank, int offset);
#define apply_rx_data_dac_offset(gcr_addr, bank, offset) apply_rx_dac_offset(gcr_addr, data_only, bank, offset);
#define apply_rx_edge_dac_offset(gcr_addr, bank, offset) apply_rx_dac_offset(gcr_addr, edge_only, bank, offset);
#define apply_rx_data_and_edge_dac_offset(gcr_addr, bank, offset) apply_rx_dac_offset(gcr_addr, data_and_edge, bank, offset);


// PCIe Only: Special blocks in mem_regs for storing RX data latch offsets and TX DCC settings
// See memory map in workbook
#define saved_rx_ad_loff_addr              pcie_ppe_saved_rx_ad_loff_lane_0_addr  //0x50
#define saved_rx_bd_loff_addr              pcie_ppe_saved_rx_bd_loff_lane_0_addr  //0x60
#define saved_rx_ad_loff_size              16
#define saved_rx_bd_loff_size              16
#define saved_rx_ad_loff_lane_offset       32
#define saved_rx_bd_loff_lane_offset       32

#define saved_tx_dcc_addr                  pcie_ppe_saved_tx_dcc_lane_0_addr  //0xF0
#define saved_tx_dcc_size                  8
#define saved_tx_dcc_lane_offset           8
#define saved_tx_dcc_genx_offset           2

#define preset_rx_peak_lte_addr            pcie_ppe_preset_rx_peak_lte_lane_0_addr  //0x118
#define preset_rx_peak_lte_size            8
#define preset_rx_peak_lte_lane_offset     8
#define preset_rx_peak_lte_genx_offset     1
#define preset_rx_ctle_peak1_peak2_mask    pcie_gen1_lane0_preset_rx_ctle_peak1_peak2_mask   //0xff00
#define preset_rx_ctle_peak1_peak2_shift   pcie_gen1_lane0_preset_rx_ctle_peak1_peak2_shift  //8
#define preset_rx_ctle_lte_gain_mask       pcie_gen1_lane0_preset_rx_lte_gain_mask   //0x0070
#define preset_rx_ctle_lte_gain_shift      pcie_gen1_lane0_preset_rx_lte_gain_shift  //4
#define preset_rx_ctle_lte_zero_mask       pcie_gen1_lane0_preset_rx_lte_zero_mask   //0x0007
#define preset_rx_ctle_lte_zero_shift      pcie_gen1_lane0_preset_rx_lte_zero_shift  //0


// PCIe Only: Save to the mem_regs the current data latch dac values for all RX data latches in a bank
// Must set the gcr reg_id to rx_group before calling this
void save_rx_data_latch_dac_values(t_gcr_addr* gcr_addr, t_bank target_bank);

// PCIe Only: Write the RX data latches in a bank with the saved value + the recorded path offset
// Must set the gcr reg_id to rx_group before calling this
void restore_rx_data_latch_dac_values(t_gcr_addr* gcr_addr, t_bank target_bank);

// PCIe Only: Save to the mem_regs the TX DCC settings for the current PCIe GenX
// Must set the gcr reg_id to tx_group before calling this
void save_tx_dcc_tune_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode);

// PCIe Only: Write the TX DCC config with the saved values for the current PCIe GenX
// Must set the gcr reg_id to tx_group before calling this
void restore_tx_dcc_tune_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode);

// PCIe Only: Write the CTLE Peak1/Peak2 and LTE Gain/Zero registers with the presets for the current PCIe GenX
// Must set the gcr reg_id to rx_group before calling this
void preset_rx_peak_lte_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode);

// TX Thermometer code functions
uint32_t tx_ffe_fromVecWithHalf(const uint32_t i_vec, const uint32_t i_width);
uint32_t tx_ffe_toTherm(const uint32_t i_dec);
uint32_t tx_ffe_toThermRev(const uint32_t i_dec, const uint32_t i_width);
uint32_t tx_ffe_toThermWithHalf(const uint32_t i_dec, const uint32_t i_width);
uint32_t tx_ffe_toThermWithHalfRev(const uint32_t i_dec, const uint32_t i_width);

#endif //_IOO_COMMON_H_
