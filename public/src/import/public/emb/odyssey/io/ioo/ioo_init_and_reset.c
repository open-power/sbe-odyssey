/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_init_and_reset.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_init_and_reset.c
// *! TITLE       :
// *! DESCRIPTION : Functions for initialize HW Regs and Resetting lanes.
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23040300 |vbr     | EWM302139: 16G/21.3G phase step 0.296875 -> 0.375.
// jjb23040400 |jjb     | Issue 302460: Enable PMB timeouts in pcie functional mode.
// vbr23032900 |vbr     | EWM301686: updated dl/rlm clock phase select settings; different for Odyssey and zMetis.
// vbr23030800 |vbr     | Always set peak2 h_sel to H2 since no longer run peak2 in AXO/Gen5
// vbr23031300 |vbr     | EWM300848: Updated BIST freq/atten settings based on Odyssey lab.
// vbr23030200 |vbr     | Issue 300178: Increasing invalid lock thresh setting for gen4 peak2 mesa fails.
// vbr23011000 |vbr     | Issue 297222: Disable thread active time at start of hw_reg_init when bist enabled
// vbr23012500 |vbr     | EWM298127: Refactored PCIe BIST overrides in hw reg init to avoid various broadcast rmw issues.
// jjb23012600 |jjb     | Issue 297515: added rx_bist_lfpath_sel_dc to per data rate control, updated rx_bist_freq_adjust_8xx/9xx per rate values
// vbr23011800 |vbr     | EWM297404: Set rx_dl_clk_phase_select to 1 (previously 0) for PCIe Gen1-4.
// mbs22121600 |mbs     | Changed psave order per Yang (Issue 295635)
// jjb22121200 |jjb     | set restore to true on pipe_preset_rx for bist
// jjb22120900 |jjb     | set pipe_state_rxactive for pcie bist operation
// jjb22110700 |jjb     | added isolate_pipe_interface function to reduce code space
// jjb22100300 |jjb     | Issue 288424: Updates for PCIe BIST mode
// jjb22082600 |jjb     | Issue 288424: Enable pipe fences for PCIe bist mode and execute reset and rate change
// vbr22120500 |vbr     | Give AXO a different thread active time limit than PCIe
// vbr22120300 |vbr     | Removed/Shifted some sleeps
// vbr22120100 |vbr     | Add rate override to update rx rate settings
// vbr22111600 |vbr     | Issue 294580: only clear psave_cdrlock_mode in lane power off for AXO
// vbr22111000 |vbr     | Set the PIPE abort config in hw_reg_init and pl_reset for PCIe
// gap22101300 |gap     | If IOO and not 3to1, set to narrower track and hold pulse for subsequent TDR
// vbr22101200 |vbr     | Updated CDR settings based on ssc and bus rate / PCIe gen
// mbs22100400 |mbs     | Removed rx_16to1 bit
// vbr22100300 |vbr     | Issue 237742: revert peak invalid lock thresh settings from Issue 280618
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22083100 |vbr     | Issue 288864: set clkgen cmlmux irctrl based on rate
// vbr22082900 |vbr     | Updated for new circuit (regs and fix for issue 286507)
// vbr22081800 |vbr     | Issue 286969: iref_vset_dac is different reg for Odyssey vs P/Z
// jjb22081200 |jjb     | Issue 287068: fixed tx_eol_mode_disable masking in clock_change_tx function
// mbs22080200 |mbs     | Issue 286507: Setting rx_clkgen_en_abank_outbuf to work around circuit bug - remove when bug is fixed
// gap22062200 |gap     | Add tx to PCIe rate dependent analog controls EWM258016
// vbr22061500 |vbr     | Add external command fail reporting
// jjb22061500 |jjb     | Issue 282881: set tx_pcie_eq_calc_enable in write_tx_pl_overrides when in pcie mode
// vbr22060700 |vbr     | Issue 282339: Increase time between fifo_init and unload_clock_disable in TX psave
// vbr22060300 |vbr     | Psave logic does not wait for CDR Lock on both PCIe and AXO (new for AXO)
// vbr22060200 |vbr     | Combined writes to improve code speed/size; re-added rx_vdac_config (Issue 282069)
// vbr22060100 |vbr     | Added banks_sel input to rx lane power_off/on to select which banks are operated on
// vbr22052600 |vbr     | Issue 280618: Enable invalid_lock voting for peak2 and adjust thresholds to allow voting up to max for peak1 and peak2
// jjb22050900 |jjb     | set tx_pcie_ffe_mode_dc when in pcie mode
// mbs22041300 |mbs     | Removed old za_xbus code
// jjb22041800 |jjb     | Added support for PCIe rate dependent analog controls.
// jjb22041300 |jjb     | Enable auto recal for PCIe mode
// jjb22041200 |jjb     | Issue 276748: disable cdr prior to holding rx c16 clock in io_lane_power_off_rx()
// jjb22041100 |jjb     | update io_hw_reg_init to use reset values for AXO in pipe_config_pg_alias update.
// jjb22041100 |jjb     | updated write_rx_pl_overrides to make use of reset values of rx_term_adjust and rx_sigdet_pd for AXO mode
// jjb22040800 |jjb     | Added per lane RX sigdet powerdown setup to write_rx_pl_overrides
// jjb22031100 |jjb     | Added per lane RX termination setup to write_rx_pl_overrides
// jjb22022500 |jjb     | Added pipe_nto1_ratio and pipe_nto1_ratio_sel initialization
// mwh22002310 |mwh     | Add new bank_control seq bit 4 tx_pcie_clk_sel first on last off
// vbr22012700 |vbr     | Use shared functions for HW reg writes that are repeated in multiple places
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21092200 |vbr     | Use wo pulse for psave_req_alt abd fast write for cdr tracking control
// vbr21081200 |vbr     | Rearrranged PIPE config writes in hw_reg_init to be with other bcasts
// vbr21081100 |vbr     | Disable the scanclk (opt_gckn) to the circuit/CDR in hw_reg_init and lane reset
// vbr21072800 |vbr     | Replaced non-conditional setting overwrites with RegDef updates
// jjb21080600 |jjb     | Enabled pipe recal abort masking when not in pcie mode
// jjb21071200 |jjb     | added PIPE fencing initialization to per lane reset
// jjb21070200 |jjb     | Fence PIPE when in AXO mode
// vbr21061800 |vbr     | Config psave to ignore cdr lock for PCIe
// bja21060800 |bja     | tx_boost_hs_en and tx_dcc_sel_alias regs removed
// vbr21060300 |vbr     | Issue 241127: Added additional sleeps for lane reset
// jjb21052400 |jjb     | shadow fw_pcie_mode to pipe_phy_mode.
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21050400 |vbr     | Story 235330: Reduce Peak1/2 filter and enable vote_ratio_converge
// vbr21031700 |vbr     | Some CDR settings no longer need to be overwritten from defaults
// vbr21030400 |vbr     | Split group power on/off into rx and tx
// vbr21012100 |vbr     | Split number of RX/TX lanes
// vbr20121100 |vbr     | Removed P10 dd1 code
//-------------|P11     |-------------------------------------------------------
// mbs20111800 |mbs     | HW562628: Updated tx_iref_clock_dac to 2 for all except p10 dd1
// mbs21032300 |mbs     | HW542501: Update rx_dl_clk_phase_sel to 0b10 for zA Xbus
// vbr21030800 |vbr     | HW560156: Only force rx_iref_vset to 3 when at >=32Gbps
// vbr20111100 |vbr     | HW552824: Updated LTE Zero Settings based on sim and lab
// vbr21010500 |vbr     | HW556573: Set rx_vdac_config_dc based on voltage
// mbs20111800 |mbs     | HW552774: Updated tx_iref_clock_dac to 4
// mwh20111300 |mwh     | HW550299rx_loff_timeout to 10
// vbr20091000 |vbr     | HW536853: CDR lock ratio decreased from 464 to 456.
// vbr20101301 |vbr     | HW548766: rx_iref_vset_dac=3 in BIST for DD1 V units only
// vbr20101300 |vbr     | HW543601: Increased LTE timeout due to errors in BIST (PRBS15)
// vbr20100800 |vbr     | HW543601: Updated Peak and LTE servo settings to match settings used in ncsim
// vbr20100500 |vbr     | HW548766: Updated rx_iref_vset_dac and rx_bist_freq_adjust_dc settings.
// vbr20100100 |vbr     | HW541162: Updated rx_freq_adjust for 25.6G.
// vbr20092800 |vbr     | HW547883: Update ppe_data_rate to have 4 settings: 21.3, 25.6, 32, 38.4
// vbr20092400 |vbr     | HW544450: Use H2 for Peak2 servo at 25.6/21.3 Gbps
// bja20092801 |bja     | HW547884: Use constant values in tx_d2_* settings for all p10dd1 data rates
// bja20092800 |bja     | Use shared constants for TX register overrides
// mbs20092401 |mbs     | HW546921: Update to set rx_psave_fw_valid_bypass to 1 by default (can be cleared by software after training)
// mbs20091800 |mbs     | HW546645: Update rx_ctle_config_dc setting to 0x2
// bja20091501 |bja     | HW544277: Use main path by default for dcc
// bja20091500 |bja     | HW544277: Overwrite tx_dcc_sel_alias default value in p10dd1
// vbr20091500 |vbr     | HW541162: rx_freq_adj changed from 0x1 to 0x5 for 25.6 Gbps.
// bja20090900 |bja     | Use common is_p10_dd1() check
// mwh20090100 |mwh     | HW541978 changed psave tx_psave_time and rx_psave_time blackout time to 52ns was 100ns
// mbs20090200 |mbs     | HW544036: Cleaned up io_hw_reg_init setting organization based on review feedback
// mbs20083101 |mbs     | HW544036: Set rx_psave_fw_valid_bypass=1 when fw_spread_en=0
// mbs20082400 |mbs     | HW541155: Change rx_clk_phase_select per Glen for DD1 and DD2
// vbr20061501 |vbr     | HW533452: Increase CDR phase_step to 0.296875 for 32G (and 50G)
// vbr20061500 |vbr     | HW532468: Peak2 Servo Op should use H3 instead of H4 for 32G
// vbr20061600 |vbr     | HW532326: Set Flwheel range based on whether spread is enabled or not
// mbs20061601 |mbs     | HW532825 - SEt rx_pr_psave_val_ena_a/b=1 during io_lane_power_off to prevent flywheel snapshot hang
// mwh20052801 |mwh     | Change comments that were did not make sense
// mwh20052800 |mwh     | Add in ( get_chip_id()  == CHIP_ID_ZA) for ZA DD1 -- no using  MAJOR_EC_??? at this time
// mwh20052700 |mwh     | Add in if else for code so work with both DD1 and DD2 p10 CHIP_ID_P10  ) && ( get_major_ec() == MAJOR_EC_DD2 ))
// gap20052300 |gap     | Change for CQ521314 -- return gcr addr to rx_group
// mwh20052000 |mwh     | Change for CQ521314 -- 20 to 0x20
// mwh20051300 |mwh     | Change for CQ521314 -- dcc circuit is not be powered up
// mbs20050700 |mbs     | Updated lte timeout to 8
// mbs20042102 |mbs     | Restored tx_16to1 bit and 32:1 settings for p10 dd1 mode
// mbs20030400 |mbs     | Removed tx_16to1 mode bit
// mbs20030400 |mbs     | Removed 32:1 content (rx_16to1 now selects 8:1 mode for DL data path only)
// vbr20030500 |vbr     | HW523782/HW523779: leave lock_filter at default (2) for all cases
// mwh20021300 |mwh     | Add in code to check if req=sts and other stuff before power off lane -- fir was add
// mbs20051300 |mbs     | HW530311: Updated io_lane_power_off procedure to flush psave state machine req and sts back to 0
// vbr20021000 |vbr     | HW521651: Configure CDR lock_filter in hw_reg_init and lane_reset based on fw_spread_en
// bja20011600 |bja     | Override rx_clk_phase_select reset val in reg init and lane reset
// bja20011701 |bja     | HW519449: Reset flywheel for both banks
// bja20011700 |bja     | HW519449: Reset flywheel and force phase accumulator in lane reset
// bja19112100 |bja     | Write rx_berpl_count_en=0 in power off
// mbs19111402 |mbs     | Removed io_lane_power_on from io_reset_lane
// vbr19112000 |vbr     | HW476919: Set default psave flywheel snapshot lanes
// mbs19111100 |mbs     | Added enable_dl_clk flag to io_lane_power_on (HW508366)
// vbr19102400 |vbr     | Increased 32:1 peak timeout for mesa fails after dpipe changes
// mwh19101400 |mwh     | Change stagger for tx lanes per cq5090000
// mwh19100800 |mwh     | change stagger order per Glen, and put in if to prevent powering on/off
//             |        | same lane back to back (HW506505)
// mbs19091000 |mbs     | Added rx and tx iodom reset after power up (HW504112)
// mwh19073000 |mwh     | removed commints related to psave
// mwh19072600 |mwh     | removed tx_lane_pdwn and replaced with tx_bank_controls
// mwh19062100 | mwh    | Corrected spell error in rx_ab_bank_controls_alias
// mwh19062100 | mwh    | changed io_lane_power_off/on back to non-inline function
// cws19061800 |cws     | Updated PPE FW API
// mwh19061400 | mwh    | removed rx_lane_ana_pdwn and replaced with rx_a/b_bank_controls
// vbr19051501 |vbr     | HW491895: Add Latch DAC reverse control to hw_reg_init()
// vbr19060600 |vbr     | HW493155: hold_div_clks defaults to 1; using ab alias.
// vbr19060300 |vbr     | HW486157/HW492011: Added DL Clock gating to lane power up/down.
// mbs19052300 |mbs     | HW493155: Added toggling of hold_div_clks
// mbs19052200 |mbs     | Changed tx and rx clkdist_pdwn register values from 3 bits to 1
// bja19051000 |bja     | Add tx_iref_pdwn_b to power on/off functions
// vbr19042300 |vbr     | Increased lte timeout
// vbr18080200 |vbr     | Added lte timeout to hw_reg_init().
// mbs18091200 |mbs     | Fixed group address for tx_16to1 setting in io_hw_reg_init()
// vbr18072001 |vbr     | HW450195: Restructure power up/down functions.
// vbr18071200 |vbr     | HW455558: Move CDR reset from eo_main_init() to io_hw_reg_init().
// vbr18070900 |mwh     | Updated amp_timeout for 32:1.--cq454345 servo timeout
// vbr18062100 |vbr     | Removed rx_pr_reset.
// mwh18060900 |mwh     | removed rx_lane_dig_pdwn
// vbr18060600 |vbr     | Updated ctle_timeout for 32:1.
// vbr18060500 |vbr     | Updated amp_timeout for 32:1.
// vbr18022800 |vbr     | Added clearing of init_done, recal_done in reset.
// vbr17120600 |vbr     | Added clearing of tx_lane_pdwn after reset.
// vbr17120500 |vbr     | Added restoring of some mem_regs after reset.
// vbr17120100 |vbr     | Added lane reset function.
// vbr17110300 |vbr     | Added 32:1 settings.
// vbr16101700 |vbr     | Removed rx_octant_select.
// vbr16101200 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "io_init_and_reset.h"

#include "ppe_img_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "ppe_fw_reg_const_pkg.h"

#include "eo_bist_init_ovride.h"
#include "ioo_common.h"
#include "ioo_pipe_ifc.h"


// CDR KP Settings
#define c_phase_step_125     0x10
#define c_phase_step_250     0x20
#define c_phase_step_296875  0x26
#define c_phase_step_375     0x30
#define c_phase_step_500     0x40


// Struct and Constants for data rate dependent settigs.
// See this spreadsheet for the documented settings:  https://ibm.ent.box.com/file/947833398988
// The ordering, bit sizes, and fills of this struct are very important for proper execution and optimal code size.
PK_STATIC_ASSERT(rx_freq_adjust_width == 9);
PK_STATIC_ASSERT(rx_bist_atten_dc_sel_dc_width == 3);
PK_STATIC_ASSERT(rx_bist_atten_ac_sel_dc_width == 3);
PK_STATIC_ASSERT(rx_ctle_peak2_h_sel_width == 2);
PK_STATIC_ASSERT(rx_pr_phase_step_width == 7);
PK_STATIC_ASSERT(rx_bist_freq_adjust_dc_width == 2);
PK_STATIC_ASSERT(rx_bist_lfpath_sel_dc_width == 1);
PK_STATIC_ASSERT(tx_d2_ctrl_width == 2);
PK_STATIC_ASSERT(tx_d2_div_ctrl_width == 2);
PK_STATIC_ASSERT(rx_dl_clk_phase_select_width == 2);
PK_STATIC_ASSERT(rx_clk_phase_select_width == 2);
PK_STATIC_ASSERT(rx_dfe_selftimed_phase_adj_width == 2);
PK_STATIC_ASSERT(rx_pcie_clkgen_div_ictrl_width == 2);
PK_STATIC_ASSERT(rx_pcie_clkgen_div_rctrl_width == 2);
PK_STATIC_ASSERT(rx_dpr_vbn_cal_width == 2);
PK_STATIC_ASSERT(rx_clkgen_cmlmux_irctrl_5nm_width == 3);
PK_STATIC_ASSERT(tx_bank_controls_d2_en_b_alias_width == 1);
PK_STATIC_ASSERT(tx_pcie_cmux_ictrl_dc_width == 2);
PK_STATIC_ASSERT(tx_pcie_cmux_rctrl_dc_width == 2);
PK_STATIC_ASSERT(tx_pcie_cdiv_ictrl_dc_width == 2);
PK_STATIC_ASSERT(tx_pcie_cdiv_rctrl_dc_width == 2);
typedef struct struct_data_rate_settings
{
    // byte 0
    uint8_t  rx_bist_atten_dc_sel_dc      : 3;
    uint8_t  rx_bist_atten_ac_sel_dc      : 3;
    uint8_t  rx_ctle_peak2_h_sel          : 2;

    // byte 1
    uint8_t  rx_bist_freq_adjust_8xx      : 2; // Setting @ 800mV, 850mV
    uint8_t  rx_bist_freq_adjust_9xx      : 2; // Setting @ 900mV, 950mV
    uint8_t  tx_d2_ctrl                   : 2;
    uint8_t  tx_d2_div_ctrl               : 2;

    // byte 2
    uint8_t  rx_dl_clk_phase_select       : 2;
    uint8_t  rx_clk_phase_select          : 2;
    uint8_t  rx_dl_clk_phase_select_ody   : 2;
    uint8_t  rx_clk_phase_select_ody      : 2;

    // byte 3
    uint8_t  tx_pcie_cdiv_ictrl_dc        : 2;
    uint8_t  tx_pcie_cdiv_rctrl_dc        : 2;
    uint8_t  tx_pcie_cmux_ictrl_dc        : 2;
    uint8_t  tx_pcie_cmux_rctrl_dc        : 2;

    // bytes 4-5
    uint8_t  rx_dfe_selftimed_phase_adj   : 2;
    uint8_t  rx_pcie_clkgen_div_ictrl     : 2;
    uint8_t  rx_pcie_clkgen_div_rctrl     : 2;
    uint8_t  tx_d2_en_dc                  : 1;
    uint16_t rx_freq_adjust               : 9;

    // bytes 6-7
    uint8_t  rx_clkgen_cmlmux_irctrl      : 3;
    uint8_t  rx_bist_lfpath_sel_dc        : 1;
    uint8_t  rx_dpr_vbn_cal               : 2;
    uint8_t  fill_data0                   : 3; // Fill space for alignment
    uint8_t  rx_pr_phase_step             : 7;
} __attribute__((packed, aligned(2))) t_data_rate_settings;

// 2.5 Gbps
const t_data_rate_settings c_data_rate_settings_2g =
{
    .rx_freq_adjust               = 0x0030,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = c_phase_step_296875,
    .rx_bist_freq_adjust_8xx      = 2, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_freq_adjust_9xx      = 0,
    .rx_bist_atten_dc_sel_dc      = 0,
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 1,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 0,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 0,
    .tx_d2_en_dc                  = 0,
    .tx_d2_ctrl                   = 0,
    .tx_d2_div_ctrl               = 0,
    .tx_pcie_cmux_ictrl_dc        = 0,
    .tx_pcie_cmux_rctrl_dc        = 0,
    .tx_pcie_cdiv_ictrl_dc        = 0,
    .tx_pcie_cdiv_rctrl_dc        = 0,
};

// 5.0 Gbps
const t_data_rate_settings c_data_rate_settings_5g =
{
    .rx_freq_adjust               = 0x0072,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = c_phase_step_500,
    .rx_bist_freq_adjust_8xx      = 2, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_freq_adjust_9xx      = 0,
    .rx_bist_atten_dc_sel_dc      = 0,
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 1,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 0,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 0,
    .tx_d2_en_dc                  = 0,
    .tx_d2_ctrl                   = 0,
    .tx_d2_div_ctrl               = 0,
    .tx_pcie_cmux_ictrl_dc        = 0,
    .tx_pcie_cmux_rctrl_dc        = 0,
    .tx_pcie_cdiv_ictrl_dc        = 0,
    .tx_pcie_cdiv_rctrl_dc        = 0,
};

// 8.0 Gbps
const t_data_rate_settings c_data_rate_settings_8g =
{
    .rx_freq_adjust               = 0x00B4,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = c_phase_step_500,
    .rx_bist_freq_adjust_8xx      = 2, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_freq_adjust_9xx      = 0,
    .rx_bist_atten_dc_sel_dc      = 0,
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 1,
    .rx_pcie_clkgen_div_ictrl     = 2,
    .rx_pcie_clkgen_div_rctrl     = 2,
    .rx_clkgen_cmlmux_irctrl      = 1,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 0,
    .tx_d2_en_dc                  = 1,
    .tx_d2_ctrl                   = 0,
    .tx_d2_div_ctrl               = 0,
    .tx_pcie_cmux_ictrl_dc        = 0,
    .tx_pcie_cmux_rctrl_dc        = 0,
    .tx_pcie_cdiv_ictrl_dc        = 2,
    .tx_pcie_cdiv_rctrl_dc        = 2,
};

// 16.0 Gbps
const t_data_rate_settings c_data_rate_settings_16g =
{
    .rx_freq_adjust               = 0x00E7,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = c_phase_step_375,
    .rx_bist_freq_adjust_8xx      = 2,
    .rx_bist_freq_adjust_9xx      = 0,
    .rx_bist_atten_dc_sel_dc      = 0,
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 1,
    .rx_pcie_clkgen_div_ictrl     = 2,
    .rx_pcie_clkgen_div_rctrl     = 2,
    .rx_clkgen_cmlmux_irctrl      = 3,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 0,
    .tx_d2_en_dc                  = 1,
    .tx_d2_ctrl                   = 1,
    .tx_d2_div_ctrl               = 1,
    .tx_pcie_cmux_ictrl_dc        = 0,
    .tx_pcie_cmux_rctrl_dc        = 0,
    .tx_pcie_cdiv_ictrl_dc        = 2,
    .tx_pcie_cdiv_rctrl_dc        = 2,
};

// 21.3 Gbps
const t_data_rate_settings c_data_rate_settings_21g =
{
    .rx_freq_adjust               = 0x016A,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = c_phase_step_375,
    .rx_bist_freq_adjust_8xx      = 2,
    .rx_bist_freq_adjust_9xx      = 0,
    .rx_bist_atten_dc_sel_dc      = 0,
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 0,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 5,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 3,
    .tx_d2_en_dc                  = 1, // tx settings copied from 25g settings; as of 5/25/22
    .tx_d2_ctrl                   = 2, //    tx team has not determined settings
    .tx_d2_div_ctrl               = 2, //
    .tx_pcie_cmux_ictrl_dc        = 1, //
    .tx_pcie_cmux_rctrl_dc        = 1, //
    .tx_pcie_cdiv_ictrl_dc        = 0, //
    .tx_pcie_cdiv_rctrl_dc        = 0, //
};

// 25.6 Gbps, 25.78125 Gbps
const t_data_rate_settings c_data_rate_settings_25g =
{
    .rx_freq_adjust               = 0x016A,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = 12,
    .rx_bist_freq_adjust_8xx      = 3,
    .rx_bist_freq_adjust_9xx      = 1, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_atten_dc_sel_dc      = 7, // EWM300844: Correct Rx Bist DC Atten
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 0,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 5,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 3, //0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 3,
    .tx_d2_en_dc                  = 1,
    .tx_d2_ctrl                   = 2,
    .tx_d2_div_ctrl               = 2,
    .tx_pcie_cmux_ictrl_dc        = 1,
    .tx_pcie_cmux_rctrl_dc        = 1,
    .tx_pcie_cdiv_ictrl_dc        = 0,
    .tx_pcie_cdiv_rctrl_dc        = 0,
};

// 32.0 Gbps, 32.5 Gbps
const t_data_rate_settings c_data_rate_settings_32g =
{
    .rx_freq_adjust               = 0x01AC,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = 12,
    .rx_bist_freq_adjust_8xx      = 3, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_freq_adjust_9xx      = 1, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_atten_dc_sel_dc      = 7, // EWM300844: Correct Rx Bist DC Atten
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 0,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 6,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 3, //0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 2,
    .tx_d2_en_dc                  = 1,
    .tx_d2_ctrl                   = 2,
    .tx_d2_div_ctrl               = 2,
    .tx_pcie_cmux_ictrl_dc        = 2,
    .tx_pcie_cmux_rctrl_dc        = 2,
    .tx_pcie_cdiv_ictrl_dc        = 0,
    .tx_pcie_cdiv_rctrl_dc        = 0,
};

// 38.4 Gbps
const t_data_rate_settings c_data_rate_settings_38g =
{
    .rx_freq_adjust               = 0x01EE,
    .rx_ctle_peak2_h_sel          = 0, // H2
    .rx_pr_phase_step             = 12,
    .rx_bist_freq_adjust_8xx      = 3, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_freq_adjust_9xx      = 1, // EWM300848: Correct Rx Bist Freq Adjust
    .rx_bist_atten_dc_sel_dc      = 7, // EWM300844: Correct Rx Bist DC Atten
    .rx_bist_atten_ac_sel_dc      = 0,
    .rx_bist_lfpath_sel_dc        = 0,
    .rx_pcie_clkgen_div_ictrl     = 0,
    .rx_pcie_clkgen_div_rctrl     = 0,
    .rx_clkgen_cmlmux_irctrl      = 6,
    .rx_clk_phase_select          = 2,
    .rx_dl_clk_phase_select       = 2,
    .rx_clk_phase_select_ody      = 3, //0,
    .rx_dl_clk_phase_select_ody   = 0,
    .rx_dfe_selftimed_phase_adj   = 0,
    .rx_dpr_vbn_cal               = 2,
    .tx_d2_en_dc                  = 1,
    .tx_d2_ctrl                   = 3,
    .tx_d2_div_ctrl               = 3,
    .tx_pcie_cmux_ictrl_dc        = 3,
    .tx_pcie_cmux_rctrl_dc        = 3,
    .tx_pcie_cdiv_ictrl_dc        = 0,
    .tx_pcie_cdiv_rctrl_dc        = 0,
};

// Array of pointers to the data rate setting constants
PK_STATIC_ASSERT(ppe_data_rate_width == 3);
const t_data_rate_settings* c_data_rate_settings[8] =
{
    &c_data_rate_settings_2g,  // 0 =  2.5G
    &c_data_rate_settings_5g,  // 1 =  5.0G
    &c_data_rate_settings_8g,  // 2 =  8.0G
    &c_data_rate_settings_16g, // 3 = 16.0G
    &c_data_rate_settings_21g, // 4 = 21.3G
    &c_data_rate_settings_25g, // 5 = 25.6G, 25.78125G
    &c_data_rate_settings_32g, // 6 = 32.0G, 32.5G
    &c_data_rate_settings_38g, // 7 = 38.4G
};

// Function for getting a pointer to data rate dependent settings
static const t_data_rate_settings* get_data_rate_settings(int i_data_rate)
{
    const t_data_rate_settings* o_selected_settings = c_data_rate_settings[i_data_rate];
    return o_selected_settings;
} //get_data_rate_settings


// Assumptions
PK_STATIC_ASSERT(tx_bank_controls_width == 6);
PK_STATIC_ASSERT(rx_a_bank_controls_width == 6);
PK_STATIC_ASSERT(rx_b_bank_controls_width == 6);
PK_STATIC_ASSERT(rx_ab_bank_controls_alias_width == 16);
PK_STATIC_ASSERT(rx_pr_ki_kp_full_reg_width == 16);
PK_STATIC_ASSERT(rx_pr_phase_force_cmd_a_alias_width == 8);
PK_STATIC_ASSERT(rx_ctle_peak_invalid_lock_thresh_alias_width == 16);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_width == 16);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_off_disable_dm_b_addr);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_ctle_config_dc_addr);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_vdac_config_dc_addr);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_clk_phase_select_addr);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_dl_clk_phase_select_addr);
PK_STATIC_ASSERT(rx_dac_cntl8_pl_full_reg_addr == rx_dpr_vbn_cal_addr);
PK_STATIC_ASSERT(rx_dac_cntl11_pl_full_reg_width == 16);
PK_STATIC_ASSERT(rx_dac_cntl11_pl_full_reg_addr == rx_term_pd_addr);
PK_STATIC_ASSERT(rx_dac_cntl11_pl_full_reg_addr == rx_freq_adjust_addr);
PK_STATIC_ASSERT(rx_dac_cntl11_pl_full_reg_addr == rx_dfe_selftimed_phase_adj_addr);
PK_STATIC_ASSERT(rx_dac_cntl11_pl_full_reg_addr == rx_pcie_clkgen_div_ictrl_addr);
PK_STATIC_ASSERT(tx_d2_ctrl_div_ctrl_alias_width == 4);
PK_STATIC_ASSERT(tx_d2_ctrl_div_ctrl_alias_addr == tx_d2_ctrl_addr);
PK_STATIC_ASSERT(tx_d2_ctrl_div_ctrl_alias_addr == tx_d2_div_ctrl_addr);
PK_STATIC_ASSERT(tx_pcie_clk_ctrl_alias_width == 8);
PK_STATIC_ASSERT(tx_pcie_clk_ctrl_alias_addr == tx_pcie_cmux_ictrl_dc_addr);
PK_STATIC_ASSERT(tx_pcie_clk_ctrl_alias_addr == tx_pcie_cmux_rctrl_dc_addr);
PK_STATIC_ASSERT(tx_pcie_clk_ctrl_alias_addr == tx_pcie_cdiv_ictrl_dc_addr);
PK_STATIC_ASSERT(tx_pcie_clk_ctrl_alias_addr == tx_pcie_cdiv_rctrl_dc_addr);


////////////////////////////////////
// Helper Functions
////////////////////////////////////
int determine_data_rate(t_gcr_addr* io_gcr_addr, uint32_t ovr_rate_one_hot)
{
    int l_data_rate;
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if (l_pcie_mode == 1)   // Update ppe_data_rate to reflect PCIe pipe_state_rate
    {
        uint32_t l_pipe_state_rate = (ovr_rate_one_hot > 0) ? (31 - __builtin_clz(ovr_rate_one_hot)) : get_ptr_field(
                                         io_gcr_addr, pipe_state_rate);

        if (l_pipe_state_rate == 4)   // Translate GEN5 (4) to ppe_date_rate 32G (6)
        {
            l_data_rate = 6;
        }
        else
        {
            l_data_rate = l_pipe_state_rate;
        }
    }
    else     //AXO
    {
        l_data_rate = mem_pg_field_get(ppe_data_rate);
    }

    return l_data_rate;
} //determine_data_rate

static const t_data_rate_settings* determine_data_rate_and_get_settings(t_gcr_addr* gcr_addr,
        uint32_t ovr_rate_one_hot)
{
    int l_data_rate = determine_data_rate(gcr_addr, ovr_rate_one_hot);
    const t_data_rate_settings* o_data_rate_settings = get_data_rate_settings(l_data_rate);
    return o_data_rate_settings;
} //determine_data_rate_and_get_settings


//////////////////////////////////////////////////////////////////////////////////////////////
// Write Per-Lane Register Overrides (shared between hw_reg_init and reset_lane).
// Assumes that gcr_addr has the correct group and lane (or broadcast) set.
//////////////////////////////////////////////////////////////////////////////////////////////
void update_tx_rate_dependent_analog_ctrl_pl_regs(t_gcr_addr* gcr_addr, uint32_t ovr_rate_one_hot)
{
    const t_data_rate_settings* l_data_rate_settings = determine_data_rate_and_get_settings(gcr_addr, ovr_rate_one_hot);

    // Tx CML IREF clock control
    int tx_d2_ctrl_div_ctrl_val = (l_data_rate_settings->tx_d2_ctrl     << (tx_d2_ctrl_shift     -
                                   tx_d2_ctrl_div_ctrl_alias_shift)) |
                                  (l_data_rate_settings->tx_d2_div_ctrl << (tx_d2_div_ctrl_shift - tx_d2_ctrl_div_ctrl_alias_shift));
    put_ptr_field(gcr_addr, tx_d2_ctrl_div_ctrl_alias, tx_d2_ctrl_div_ctrl_val, read_modify_write); //pl

    // tx_bank_controls_d2_en_b_alias must be set to 0 to enable d2
    put_ptr_field(gcr_addr, tx_bank_controls_d2_en_b_alias, 1 - l_data_rate_settings->tx_d2_en_dc, read_modify_write); //pl


    int tx_pcie_clk_ctrl_val = (l_data_rate_settings->tx_pcie_cmux_ictrl_dc << (tx_pcie_cmux_ictrl_dc_shift -
                                tx_pcie_clk_ctrl_alias_shift)) |
                               (l_data_rate_settings->tx_pcie_cmux_rctrl_dc << (tx_pcie_cmux_rctrl_dc_shift - tx_pcie_clk_ctrl_alias_shift)) |
                               (l_data_rate_settings->tx_pcie_cdiv_ictrl_dc << (tx_pcie_cdiv_ictrl_dc_shift - tx_pcie_clk_ctrl_alias_shift)) |
                               (l_data_rate_settings->tx_pcie_cdiv_rctrl_dc << (tx_pcie_cdiv_rctrl_dc_shift - tx_pcie_clk_ctrl_alias_shift));
    put_ptr_field(gcr_addr, tx_pcie_clk_ctrl_alias, tx_pcie_clk_ctrl_val, read_modify_write); //pl
} //update_tx_rate_dependent_analog_ctrl_pl_regs


///////////////////////////////////
// clock_change_tx
///////////////////////////////////
PK_STATIC_ASSERT(tx_bank_controls_d2_en_b_alias_endbit == tx_bank_controls_endbit - 3);
PK_STATIC_ASSERT(tx_bank_controls_d2_en_b_alias_addr == tx_bank_controls_addr);
PK_STATIC_ASSERT(tx_bank_controls_d2_en_b_alias_width == 1);
PK_STATIC_ASSERT(tx_bank_controls_width == 6);

void clock_change_tx(t_gcr_addr* gcr_addr,
                     uint32_t rate_one_hot)                                                                                        // Start clock_change_tx function
{
    // Update TX analog clock generator and divider to support requested data rate                                                                         //  Manage TX Clocks
    set_gcr_addr_reg_id(gcr_addr,
                        tx_group);                                                                                                               //  Change addressing to TX group
    // PSL d2_en_val
    uint32_t l_d2_en_val = (rate_one_hot < 4) ? 0b1 : 0b0;
    uint32_t l_tx_eol_mode_disable = get_ptr_field(gcr_addr,
                                     tx_eol_mode_disable);                                                                         //  Get tx_eol_mode_disable value
    uint32_t l_tx_bank_controls = (get_ptr_field(gcr_addr,
                                   tx_bank_controls) & 0x37) | l_d2_en_val <<
                                  3;                                                       //  Get tx_bank_controls and set d2_en to proper value
    uint32_t l_tx_bank_controls_updated = l_tx_bank_controls |
                                          0x04;                                                                                       //  Set tx_fifo_pdwn to 1
    put_ptr_field(gcr_addr, tx_eol_mode_disable, 0b1,
                  read_modify_write);                                                                                  //  Set tx_eol_mode_disable
    put_ptr_field(gcr_addr, tx_bank_controls, l_tx_bank_controls_updated,
                  read_modify_write);                                                              //  Update tx_bank_controls
    put_ptr_field(gcr_addr, tx_pcie_clk_sel, rate_one_hot,
                  read_modify_write);                                                                             //  Change TX clock dividers
    update_tx_rate_dependent_analog_ctrl_pl_regs(gcr_addr,
            rate_one_hot);                                                                                  //  Update TX analog controls
    put_ptr_field(gcr_addr, tx_bank_controls, l_tx_bank_controls,
                  read_modify_write);                                                                      //  Restore tx_bank_controls, possibly updating d2_en
    put_ptr_field(gcr_addr, tx_eol_mode_disable, l_tx_eol_mode_disable,
                  read_modify_write);                                                                //  Restore tx_eol_mode_disable
} //clock_change_tx

void isolate_pipe_interface(t_gcr_addr* gcr_addr)
{
    put_ptr_fast(gcr_addr, pipe_fence_cntl3_pl_addr,   15, 0x8000); // PIPE Fence Input Values
    put_ptr_fast(gcr_addr, pipe_fence_cntl4_pl_addr,   15, 0x0000); // PIPE Fence Output Values
    put_ptr_fast(gcr_addr, pipe_fence_cntl1_pl_addr,   15, 0xFFFF); // PIPE Fence Input Enables
    put_ptr_fast(gcr_addr, pipe_fence_cntl2_pl_addr,   15, 0xFFFF); // PIPE Fence Output Enables
    put_ptr_fast(gcr_addr, pipe_state_cntl3_pl_addr,   15, 0xFFFF); // PIPE Abort Masked
    put_ptr_fast(gcr_addr, pipe_state_0_15_alias_addr, 15, 0x0000); // Clear all PIPE PPE Requests
}

// RX and TX PCIe BIST PL Overrides that can not be broadcast (in write_tx_pl_overrides) because in the multigroup or for some other reason.
// Only called from hw_reg_init (and not io_reset_lane_rx/tx) since PCIe BIST will re-run hw_reg_init (rather than resetting the lanes) between each Gen.
void pcie_bist_no_broadcast_pl_overrides(t_gcr_addr* gcr_addr)
{
    ////////
    // TX //
    ////////
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    // set up grid clock divider ratio mux controls to firmware desired rate
    int l_fw_bist_pcie_rate = fw_field_get(fw_bist_pcie_rate);
    uint32_t l_pipe_nto1_ratio = (15 >> l_fw_bist_pcie_rate); // gen1=15, gen2=7, gen3=3, gen4=1, gen5=0
    put_ptr_field(gcr_addr, pipe_nto1_ratio, l_pipe_nto1_ratio,
                  read_modify_write);               // Override value for grid clock divider ratio controls

    put_ptr_field(gcr_addr, pipe_nto1_ratio_sel, 0b1,
                  read_modify_write);                         // Select PHY Override chip level grid clock divider ratio controls
    put_ptr_field(gcr_addr, pipe_fence_input_value_rate, l_fw_bist_pcie_rate,
                  read_modify_write); // Set pipe_state_rate value to desired rate
    put_ptr_field(gcr_addr, pipe_state_rxactive, 0b1,
                  read_modify_write);                         // Set pipe_state_rxactive to active
    put_ptr_fast(gcr_addr, pipe_state_0_15_alias_addr, 15,
                 0x0000);                               // Clear all PIPE PPE Requests

    restore_tx_dcc_tune_values(gcr_addr,
                               l_fw_bist_pcie_rate);                                    // restore dcc as per desired rate

    ////////
    // RX //
    ////////
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    pipe_preset_rx(gcr_addr, l_fw_bist_pcie_rate,
                   0b1);                                           // preset ctle as per desired rate, restore = true
} //pcie_bist_no_broadcast_pl_overrides


//////////////////////////////////////////////////////////////////////////////////////////////
// Write Per-Lane Register Overrides (shared between hw_reg_init and reset_lane).
// Assumes that gcr_addr has the correct group and lane (or broadcast) set.
//////////////////////////////////////////////////////////////////////////////////////////////
void write_tx_pl_overrides(t_gcr_addr* gcr_addr)
{
    // AXO or BIST Mode, Fence off all PIPE signals to an inactive state
    int l_fw_bist_en = fw_field_get(fw_bist_en);
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if (l_pcie_mode == 0)   // AXO Mode
    {
        isolate_pipe_interface(gcr_addr); // Disable PIPE Interface, in hw_reg_init broadcast to all lanes in multigroup
    }
    else if (l_fw_bist_en == 0)     // PCIe Functional Mode
    {
        put_ptr_fast(gcr_addr, pipe_fence_cntl2_pl_addr,   15,
                     0x0000); // Enable PIPE Interface, clear pipe_fence_global_enable, in hw_reg_init broadcast to all lanes in multigroup
        put_ptr_field(gcr_addr, tx_pcie_ffe_mode_dc, 0b1, read_modify_write); // set pcie FFE mode when in pcie mode
        put_ptr_field(gcr_addr, tx_pcie_eq_calc_enable, 0b1, read_modify_write); // set tx_pcie_eq_calc_enable when in pcie mode
        put_ptr_field(gcr_addr, pipe_config_pmb_timeout_en, 0b1,
                      read_modify_write); // Enable PMB timeouts when in pcie functional mode
    }
    else     // PCIe BIST Mode
    {
        isolate_pipe_interface(gcr_addr); // Disable PIPE Interface, in hw_reg_init broadcast to all lanes in multigroup
        put_ptr_field(gcr_addr, tx_pcie_ffe_mode_dc, 0b1, read_modify_write); // set pcie FFE mode when in pcie mode
        put_ptr_field(gcr_addr, tx_pcie_eq_calc_enable, 0b1, read_modify_write); // set tx_pcie_eq_calc_enable when in pcie mode

        int l_fw_bist_pcie_rate = fw_field_get(fw_bist_pcie_rate);
        int l_fw_bist_pcie_rate_one_hot = (1 << l_fw_bist_pcie_rate);
        pipe_clock_change(gcr_addr,
                          l_fw_bist_pcie_rate_one_hot); // Update clocks per desired rate, writes some rx regs but switches back to tx_grp
    }

    // Issue 282339: Increase time between fifo_init and unload_clock_disable in TX psave
    int l_psave_fifo_init_timer_cfg_val =
        (7 << (tx_psave_fifo_init_timer_val_shift         - tx_psave_fifo_init_timer_cfg_alias_shift)) |
        (1 << (tx_psave_fifo_init_timer_double_mode_shift - tx_psave_fifo_init_timer_cfg_alias_shift));
    put_ptr_field(gcr_addr, tx_psave_fifo_init_timer_cfg_alias, l_psave_fifo_init_timer_cfg_val, read_modify_write); //pl

    // if IOO and not 3to1, set to narrower track and hold pulse for subsequent TDR
#ifdef IOO
    bool l_is_3to1 = is_zm_abus() || is_zm_xbus();

    if (!l_is_3to1)
    {
        put_ptr_field(gcr_addr, tx_tdr_th_pw_sel, 0b10, read_modify_write); // set to narrower pulse for TDR when not a 3to1 bus
    }

#endif

    update_tx_rate_dependent_analog_ctrl_pl_regs(gcr_addr, 0x0);
} //write_tx_pl_overrides


//////////////////////////////////////////////////////////////////////////////////////////////
// Write Per-Lane Register Overrides (shared between hw_reg_init and reset_lane).
// Assumes that gcr_addr has the correct group and lane (or broadcast) set.
//////////////////////////////////////////////////////////////////////////////////////////////
void update_rx_rate_dependent_analog_ctrl_pl_regs(t_gcr_addr* gcr_addr, uint32_t ovr_rate_one_hot)
{
    int l_vio_volt  = img_field_get(ppe_vio_volts);
    const t_data_rate_settings* l_data_rate_settings = determine_data_rate_and_get_settings(gcr_addr, ovr_rate_one_hot);

    // CDR Phase Step (KP)
    put_ptr_field(gcr_addr, rx_pr_phase_step, l_data_rate_settings->rx_pr_phase_step, read_modify_write); //pl

    // RX_FREQ_ADJUST / RX_DFE_SELFTIMED_PHASE_ADJ / RX PCIE_CLKGEN_DIV / RX CLKGEN_CMLCMUX Controls
    put_ptr_field(gcr_addr, rx_pcie_clkgen_div_rctrl, l_data_rate_settings->rx_pcie_clkgen_div_rctrl,
                  read_modify_write); //pl
    put_ptr_field(gcr_addr, rx_clkgen_cmlmux_irctrl_5nm, l_data_rate_settings->rx_clkgen_cmlmux_irctrl,
                  read_modify_write); //pl
    int dac_cntl11_reg_val =
        (0  << rx_term_pd_shift) |
        (l_data_rate_settings->rx_freq_adjust              << rx_freq_adjust_shift) |
        (l_data_rate_settings->rx_dfe_selftimed_phase_adj  << rx_dfe_selftimed_phase_adj_shift) |
        (l_data_rate_settings->rx_pcie_clkgen_div_ictrl    << rx_pcie_clkgen_div_ictrl_shift);
    put_ptr_field(gcr_addr, rx_dac_cntl11_pl_full_reg, dac_cntl11_reg_val, fast_write); //pl

    // RX BIST FREQUENCY ADJUST / RX BIST ATTENUATION Controls
    int bist_freq_adjust;

    // PSL vio_volt_lt_2
    if (l_vio_volt < 2)   // 00 = 950mV, 01 = 900mV
    {
        bist_freq_adjust = l_data_rate_settings->rx_bist_freq_adjust_9xx;
    }
    else     // 10 = 850 mV, 11 = 800 mV
    {
        bist_freq_adjust = l_data_rate_settings->rx_bist_freq_adjust_8xx;
    }

    int bist_atten_dc_ac_sel_fadj_lfpath = (l_data_rate_settings->rx_bist_atten_dc_sel_dc <<
                                            (rx_bist_atten_dc_sel_dc_shift - rx_bist_atten_dc_ac_fadj_lfpath_alias_shift)) |
                                           (l_data_rate_settings->rx_bist_atten_ac_sel_dc << (rx_bist_atten_ac_sel_dc_shift -
                                                   rx_bist_atten_dc_ac_fadj_lfpath_alias_shift)) |
                                           (bist_freq_adjust << (rx_bist_freq_adjust_dc_shift - rx_bist_atten_dc_ac_fadj_lfpath_alias_shift)) |
                                           (l_data_rate_settings->rx_bist_lfpath_sel_dc << rx_bist_lfpath_sel_dc_shift);
    put_ptr_field(gcr_addr, rx_bist_atten_dc_ac_fadj_lfpath_alias, bist_atten_dc_ac_sel_fadj_lfpath,
                  read_modify_write); //pl



    // Set the RX DL/RLM clock phase offset (data and clock relationship). RX_DPR_VBN_CAL / RX VDAC VOLTAGE CONFIGs.
    bool l_is_odyssey = is_odyssey();
    int rlm_clk_phase_sel = l_is_odyssey ? l_data_rate_settings->rx_clk_phase_select_ody    :
                            l_data_rate_settings->rx_clk_phase_select;
    int  dl_clk_phase_sel = l_is_odyssey ? l_data_rate_settings->rx_dl_clk_phase_select_ody :
                            l_data_rate_settings->rx_dl_clk_phase_select;
    int dac_cntl8_reg_val =
        (1           << rx_off_disable_dm_b_shift) |
        (2           << rx_ctle_config_dc_shift) |
        (l_vio_volt  << rx_vdac_config_dc_shift) |
        (rlm_clk_phase_sel  << rx_clk_phase_select_shift) |
        (dl_clk_phase_sel   << rx_dl_clk_phase_select_shift) |
        (l_data_rate_settings->rx_dpr_vbn_cal          << rx_dpr_vbn_cal_shift);
    put_ptr_field(gcr_addr, rx_dac_cntl8_pl_full_reg, dac_cntl8_reg_val, fast_write); //pl
} //update_rx_rate_dependent_analog_ctrl_pl_regs


//////////////////////////////////////////////////////////////////////////////////////////////
// Write Per-Lane Register Overrides (shared between hw_reg_init and reset_lane).
// Assumes that gcr_addr has the correct group and lane (or broadcast) set.
//////////////////////////////////////////////////////////////////////////////////////////////
void write_rx_pl_overrides(t_gcr_addr* gcr_addr)
{
    // Disable the scanclk (opt_gckn) to the circuit/CDR
    put_ptr_field(gcr_addr, rx_ckt_scanclk_force_en, 0b0, read_modify_write); //pl

    // Update RX Rate Dependent Analog Controls
    update_rx_rate_dependent_analog_ctrl_pl_regs(gcr_addr, 0x0);

    // Different settings when Spread Spectrum Clocking (SSC) is disabled
    int spread_en = fw_field_get(fw_spread_en);

    // PSL spread_en
    if ( ! spread_en )
    {
        // HW532326: Set rx_pr_fw_range_sel=1 (by setting rx_pr_fw_inertia_amt_coarse=15) when when spread is disabled.
        // This reduces the max flywheel correction and increases the Bump UI step size.
        put_ptr_field(gcr_addr, rx_pr_fw_inertia_amt_coarse, 15, read_modify_write); //pl

        // Lower fw_inertia (KI) when no spread
        put_ptr_field(gcr_addr, rx_pr_fw_inertia_amt, 1, read_modify_write); //pl
    }

    // Psave returns after powering the bank (do not wait for CDR lock)
    put_ptr_field(gcr_addr, rx_psave_cdrlock_mode_sel, 0b11, read_modify_write); //pl

    /////////////////////////////////////////////////////////////////
    // Set up Per Lane RX Terminations and sigdet power down control
    /////////////////////////////////////////////////////////////////
    // RX Terminator Pull Down Register Value is low for both PCIe and AXO modes. ioreset value is low so no write required.
    // In PCIe mode the RX Terminator Pull Down value to the RX analog is controled by pipe_resetn.
    // RX Terminator Adjust is 1 (42.5ohms) for AXO mode. (via reset)
    // RX Terminator Adjust is 0 (50.0ohms) for PCIe mode.
    /////////////////////////////////////////////////////////////////
    // rx_sigdet_pd is '0' for PCIe
    // rx_sigdet_ps is '1' for AXO (via reset)
    /////////////////////////////////////////////////////////////////
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    if (l_pcie_mode)
    {
        put_ptr_field(gcr_addr, rx_term_adjust, 0, read_modify_write);
        put_ptr_field(gcr_addr, rx_sigdet_pd,   0, read_modify_write);
    }

} //write_rx_pl_overrides


//////////////////////////////////////////////////////////////////////////////////////////////
// Bank Control Sequencing for Circuit Power Up/Down.
// Assumes that gcr_addr has the correct group set.
//////////////////////////////////////////////////////////////////////////////////////////////
#define c_tx_sequence_size 5
const uint8_t c_tx_sequence_power_on[c_tx_sequence_size] =
{
    0b111101, //tx_bank_controls_dc(4)    = tx_pcie_clk_sel
    0b110100, //tx_bank_controls_dc(2)(5) = d2_en_dc,tx_bank_control_pdwn_fifo
    0b100100, //tx_bank_controls_dc(1)    = txc_pwrdwn_dc
    0b100100, //tx_bank_controls_dc(0)    = dcc_comp_en_dc
    0b100000  //tx_bank_controls_dc(3)    = txr_txc_drv_en_p/n_dc
};
const uint8_t c_tx_sequence_power_off[c_tx_sequence_size] =
{
    0b100100, //tx_bank_controls_dc(3)    = txr_txc_drv_en_p/n_dc
    0b100100, //tx_bank_controls_dc(0)    = dcc_comp_en_dc
    0b110100, //tx_bank_controls_dc(1)    = txc_pwrdwn_dc
    0b111101, //tx_bank_controls_dc(2)(5) = d2_en_dc,tx_bank_control_pdwn_fifo
    0b111111  //tx_bank_controls_dc(4)    = tx_pcie_clk_sel

};
void sequence_tx_bank_controls(t_gcr_addr* gcr_addr, t_power_direction i_power_dir)
{
    // Select the sequence to use
    const uint8_t* l_sequence = (i_power_dir == power_on) ? c_tx_sequence_power_on : c_tx_sequence_power_off;

    // If not at the final state, sequence to it
    int l_bank_controls = get_ptr_field (gcr_addr, tx_bank_controls);

    if(l_bank_controls != l_sequence[c_tx_sequence_size - 1])
    {
        int i;

        for (i = 0; i < c_tx_sequence_size; i++)
        {
            put_ptr_field(gcr_addr, tx_bank_controls, l_sequence[i], read_modify_write);
        }
    }
} //sequence_tx_bank_controls


#define c_rx_sequence_size 6
const uint8_t c_rx_sequence_power_on[c_rx_sequence_size] =
{
    0b011111, //DIV, PR, CTLE late                                  ( ABANK_CLK1_PDWN )
    0b001111, //IQGEN, MINI PR                                      ( ABANK_CLK2_PDWN )
    0b001110, //CTLE, VDAC                                          ( ABANK_DATA_PDWN )
    0b000110, //CML2CMOS EDGEO/1                                    ( ABANK_CLK3_PDWN )
    0b000100, //CML2CMOS DATAO, CTLE early, DL_CLK_EN_SYNCHRO_FLUSH ( ABANK_CLK5_PDWN )
    0b000000  //CML2CMOS DATA1                                      ( ABANK_CLK4_PDWN )
};
const uint8_t c_rx_sequence_power_off[c_rx_sequence_size] =
{
    0b000100, //CML2CMOS DATA1                                      ( ABANK_CLK4_PDWN )
    0b000110, //CML2CMOS DATAO, CTLE early, DL_CLK_EN_SYNCHRO_FLUSH ( ABANK_CLK5_PDWN )
    0b001110, //CML2CMOS EDGEO/1                                    ( ABANK_CLK3_PDWN )
    0b001111, //CTLE, VDAC                                          ( ABANK_DATA_PDWN )
    0b011111, //IQGEN, MINI PR                                      ( ABANK_CLK2_PDWN )
    0b111111  //DIV, PR, CTLE late                                  ( ABANK_CLK1_PDWN )
};

// Legacy for Odyssey / 7nm
const uint8_t c_rx_sequence_power_on_ody[c_rx_sequence_size] =
{
    0b011111, //MINI_PR_PDWN( ABANK_CLK1_PDWN )
    0b001111, //IQGEN_PDWN( ABANK_CLK2_PDWN )
    0b001011, //CML2CMOS_DATA1_PDWN( ABANK_CLK4_PDWN )
    0b001001, //CML2CMOS_DATA0_PDWN( ABANK_CLK5_PDWN )
    0b001000, //CTLE_PDWN( ABANK_DATA_PDWN )
    0b000000  //CML2CMOS_EDGE0_PDWN( ABANK_CLK3_PDWN )
};
const uint8_t c_rx_sequence_power_off_ody[c_rx_sequence_size] =
{
    0b001000, //CML2CMOS_EDGE0_PDWN( ABANK_CLK3_PDWN )
    0b001001, //CTLE_PDWN( ABANK_DATA_PDWN )
    0b001011, //CML2CMOS_DATA0_PDWN( ABANK_CLK5_PDWN )
    0b001111, //CML2CMOS_DATA1_PDWN( ABANK_CLK4_PDWN )
    0b011111, //IQGEN_PDWN( ABANK_CLK2_PDWN )
    0b111111  //MINI_PR_PDWN( ABANK_CLK1_PDWN )
};

void sequence_rx_bank_controls(t_gcr_addr* gcr_addr, t_bank i_bank, t_power_direction i_power_dir)
{
    // Select the sequence to use
    bool l_is_odyssey = is_odyssey();

    const uint8_t* l_sequence = l_is_odyssey ?
                                ((i_power_dir == power_on) ? c_rx_sequence_power_on_ody : c_rx_sequence_power_off_ody) :
                                ((i_power_dir == power_on) ? c_rx_sequence_power_on     : c_rx_sequence_power_off    );

    // If not at the final state, sequence to it
    int l_bank_controls = (i_bank == bank_a) ? get_ptr_field (gcr_addr, rx_a_bank_controls) : get_ptr_field (gcr_addr,
                          rx_b_bank_controls);

    if(l_bank_controls != l_sequence[c_rx_sequence_size - 1])
    {
        int i;

        for (i = 0; i < c_rx_sequence_size; i++)
        {
            // PSL bank_a
            if (i_bank == bank_a)
            {
                put_ptr_field(gcr_addr, rx_a_bank_controls, l_sequence[i], read_modify_write);
            }
            else
            {
                put_ptr_field(gcr_addr, rx_b_bank_controls, l_sequence[i], read_modify_write);
            }
        }
    }
} //sequence_rx_bank_controls


//////////////////////////////////
// Initialize HW Regs
//////////////////////////////////
void io_hw_reg_init(t_gcr_addr* gcr_addr)
{
    // Disable thread active time limit for BIST
    // PSL bist_en_active_time
    if (fw_field_get(fw_bist_en))
    {
        img_bit_set(ppe_disable_thread_active_time_check);
    }

    // Larger thread active time limit for AXO (PCIe uses default)
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL axo_mode
    if(!l_pcie_mode)   // AXO Mode
    {
        mem_pg_field_put(ppe_thread_active_time_us_limit, AXO_THREAD_ACTIVE_TIME_LIMIT_US);
    }

    // Settings
    int l_vio_volt          = img_field_get(ppe_vio_volts);
    int num_lanes_rx        = get_num_rx_physical_lanes();
    int source_lane         = num_lanes_rx / 2; // for flywheel snapshot
    int l_data_rate         = mem_pg_field_get(ppe_data_rate);
    const t_data_rate_settings* l_data_rate_settings =  get_data_rate_settings(l_data_rate);


    /////////////////////////////////////////////////////////////////
    // TX PG Registers
    /////////////////////////////////////////////////////////////////
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    bool l_is_odyssey = is_odyssey();

    if (l_is_odyssey)
    {
        put_ptr_field(gcr_addr, tx_iref_vset_dac, l_vio_volt, read_modify_write); //pg
    }
    else
    {
        put_ptr_field(gcr_addr, tx_iref_vset_dac_mg, l_vio_volt, read_modify_write); //mg
    }

    // Shadow fw_pcie_mode to pipe_phy_mode.
    // PSL pcie_mode
    if(l_pcie_mode)   // PCIe Mode
    {
        put_ptr_field(gcr_addr, pipe_config_pg_alias, pipe_phy_mode_mask,
                      fast_write); // PCIe Mode : set pipe_phy_mode, clear pipe_nto1_ratio_sel, clear pipe_nto1_ratio
    }

    /////////////////////////////////////////////////////////////////
    // BROADCAST ALL TX PL Registers
    /////////////////////////////////////////////////////////////////
    set_gcr_addr_lane(gcr_addr, bcast_all_lanes);

    write_tx_pl_overrides(gcr_addr);

    // Clear the BCAST condition
    set_gcr_addr_lane(gcr_addr, 0);

    // Configure the PIPE abort (PL register in the multigroup tx_cplt_mac)
    // This function loops through the lanes in the group and should not use broadcast
    if(l_pcie_mode)   // PCIe Mode
    {
        pipe_abort_config(gcr_addr, PIPE_ABORT_CFG_IDLE); //sets rx_group
    }


    /////////////////////////////////////////////////////////////////
    // RX PG Registers
    /////////////////////////////////////////////////////////////////
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    io_sleep(get_gcr_addr_thread(gcr_addr));

    // CYA for Latch DAC reversal
#ifdef INVERT_DACS
    put_ptr_field(gcr_addr, rx_servo_reverse_latch_dac, 0b1, read_modify_write); //pg
#endif

    // HW544450/HW532468: Select the Peak2 servo correlation bit based on the bus rate (AXO)
    put_ptr_field(gcr_addr, rx_ctle_peak2_h_sel, l_data_rate_settings->rx_ctle_peak2_h_sel, read_modify_write);

    // Allow invalid lock voting on peak2. Enable is set by default for peak1.
    put_ptr_field(gcr_addr, rx_ctle_peak2_invalid_lock_en, 0b1, read_modify_write); //pg
    int ctle_peak_invalid_lock_thresh_val = (2 << rx_ctle_peak1_invalid_lock_thresh_inc_shift) |
                                            (13 << rx_ctle_peak1_invalid_lock_thresh_dec_shift) |
                                            (6 << rx_ctle_peak2_invalid_lock_thresh_inc_shift) | (13 << rx_ctle_peak2_invalid_lock_thresh_dec_shift);
    put_ptr_field(gcr_addr, rx_ctle_peak_invalid_lock_thresh_alias, ctle_peak_invalid_lock_thresh_val,
                  fast_write); //pg, full register

    if (l_is_odyssey)
    {
        put_ptr_field(gcr_addr, rx_iref_vset_dac, l_vio_volt, read_modify_write); //pg
    }
    else
    {
        put_ptr_field(gcr_addr, rx_iref_vset_dac_mg, l_vio_volt, read_modify_write); //mg
    }

    // HW544036: Flywheel snapshot does not work correctly for OpenCAPI / OMI.  Those applications happen to becommon oscillator
    // HW546921: Always set this bit to begin with.  After training when lane reversal data is read from the DL, this can be lowered.
    //put_ptr_field(gcr_addr, rx_psave_fw_valid_bypass, 0b1, read_modify_write); //pg

    // Set up the flywheel snapshot routing for psave (HW476919)
    // This is a don't care for common oscillator interfaces, which will leave rx_psave_fw_valid_bypass=1 forever
    // HW546921: This should be set to 0 after training for P10 P-bus Link Paired mode
    put_ptr_field(gcr_addr, rx_psave_fw_val1_sel, source_lane, read_modify_write); //pg

    // Enable auto recal for PCIe mode on all possible lanes of a pcie group
    if(l_pcie_mode)   // PCIe Mode
    {
        mem_pg_field_put(rx_enable_auto_recal_0_15, 0xFFFF);
    }

    /////////////////////////////////////////////////////////////////
    // BROADCAST ALL RX PL Registers
    //
    // These can be done with a RMW broadcasts because all lanes in a chiplet will have the same values for mode bits below
    // and they are in registers separate from any other mode bits that may be different per-lane.
    // Additionally, hw_reg_init is only called when all lanes are powered down, so changing the mode bits on all lanes is safe.
    /////////////////////////////////////////////////////////////////
    set_gcr_addr_lane(gcr_addr, bcast_all_lanes);

    write_rx_pl_overrides(gcr_addr);

    // Clear the BCAST condition
    set_gcr_addr_lane(gcr_addr, 0);

    /////////////////////////////////////////////////////////////////
    // RX PL Registers
    // Do these AFTER BCAST register operations, or BCAST may overwrite them!
    /////////////////////////////////////////////////////////////////
    io_sleep(get_gcr_addr_thread(gcr_addr));

    /////////////////////////////////////////////////////////////////
    // Set up the flywheel snapshot routing for psave (HW476919)
    // Each chiplet is split evenly into two links.
    // In each link, the lowest numbered lane is used as the flywheel source.
    // The default register settings are rx_psave_fw_val_sel=0b00 and rx_psave_fw_valX_sel=0 so only some lanes need to be updated.
    // The lanes in the lower link continue to select mux0 which defaults to selecting lane 0.
    // The lanes in the upper link are configured to select mux1 which is configured to select the lowest lane in the link.
    //
    // This is a don't care for common oscillator interfaces, which will leave rx_psave_fw_valid_bypass=1 forever
    //
    // HW546921: rx_psave_fw_val1_sel should be set to 0 after training for P10 P-bus Link Paired mode, which will make
    // both the b00 and b01 codes here point to the same lane 0.
    /////////////////////////////////////////////////////////////////
    int i;

    for (i = source_lane; i < num_lanes_rx; i++)
    {
        set_gcr_addr_lane(gcr_addr, i);
        put_ptr_field(gcr_addr, rx_psave_fw_val_sel, 0b01, read_modify_write); //pl
    }


    /////////////////////////////////////////////////////////////////
    // BIST Override Registers
    // Do this LAST
    /////////////////////////////////////////////////////////////////
    // PSL bist_en
    if (fw_field_get(fw_bist_en))
    {
        // PSL pcie_bist_en
        if (l_pcie_mode)
        {
            for (i = 0; i < num_lanes_rx; i++)
            {
                set_gcr_addr_lane(gcr_addr, i);
                pcie_bist_no_broadcast_pl_overrides(gcr_addr); //sets rx_group
            }
        }

        eo_bist_init_ovride(gcr_addr); //sets rx_group
    }

    io_sleep(get_gcr_addr_thread(gcr_addr));
} //io_hw_reg_init


////////////////////////////////////////////////////////////////////
// Reset a lane
// HW508366: This function does NOT power the lane back on
////////////////////////////////////////////////////////////////////
int io_reset_lane_tx(t_gcr_addr* gcr_addr)
{
    // Power off the TX
    int status = io_lane_power_off_tx(gcr_addr);

    // TX Reset
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, tx_ioreset, 0b1, fast_write); //pl, tx_ioreset/tx_iodom_ioreset are only fields in register
    put_ptr_field(gcr_addr, tx_ioreset, 0b0, fast_write); //pl, tx_ioreset/tx_iodom_ioreset are only fields in register

    // TX PL Registers
    write_tx_pl_overrides(gcr_addr);

    // Configure the PIPE abort
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if(l_pcie_mode)   // PCIe Mode
    {
        pipe_abort_config(gcr_addr, PIPE_ABORT_CFG_IDLE); //sets rx_group
    }

    set_gcr_addr_reg_id(gcr_addr, rx_group);
    io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_reset_lane_tx

int io_reset_lane_rx(t_gcr_addr* gcr_addr)
{
    set_gcr_addr_reg_id(gcr_addr, rx_group);
    // Power off the RX (both banks)
    int status = io_lane_power_off_rx(gcr_addr, both_banks);

    // RX Reset
    put_ptr_field(gcr_addr, rx_ioreset,            0b1,
                  fast_write); //pl, rx_ioreset/rx_iodom_ioreset are only fields in register
    put_ptr_field(gcr_addr, rx_ioreset,            0b0,
                  fast_write); //pl, rx_ioreset/rx_iodom_ioreset are only fields in register
    put_ptr_field(gcr_addr, rx_phy_dl_init_done,   0b0, read_modify_write); //pl, in datasm_mac so not reset by rx_ioreset
    put_ptr_field(gcr_addr, rx_phy_dl_recal_done,  0b0, read_modify_write); //pl, in datasm_mac so not reset by rx_ioreset

    // RX PL Registers
    write_rx_pl_overrides(gcr_addr);

    // HW476919: Reconfigure the psave flywheel snapshot 4:1 mux select if lane is in the upper link (see io_hw_reg_init)
    // This is a don't care for common oscillator interfaces, which will leave rx_psave_fw_valid_bypass=1 forever
    int num_lanes_rx = get_num_rx_physical_lanes();
    int lane         = get_gcr_addr_lane(gcr_addr);

    if (lane >= (num_lanes_rx / 2))
    {
        put_ptr_field(gcr_addr, rx_psave_fw_val_sel, 0b01, read_modify_write); //pl
    }

    // Clear per-lane mem_regs (addresses 0x00-0x0F).
    // These are consecutive addresses so we can speed this up by just incrementing after decoding the first address.
    int pl_addr_start = pl_addr(0, lane);
    int i;

    for (i = 0; i < 16; i++)
    {
        mem_regs_u16[pl_addr_start + i] = 0;
    }

    io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_reset_lane_rx


//////////////////////////////////
// Power Up/Down
//////////////////////////////////

// Power up a group (both RX and TX)
void io_group_power_on(t_gcr_addr* gcr_addr)
{
    io_group_power_on_tx(gcr_addr);
    io_group_power_on_rx(gcr_addr);
} //io_group_power_on

void io_group_power_on_tx(t_gcr_addr* gcr_addr)
{
    // TX Registers
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, tx_clkdist_pdwn,     0b0, read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_iref_pdwn_b,      0b1, read_modify_write); //pg
    set_gcr_addr_reg_id(gcr_addr, rx_group);
} //io_group_power_on_tx

void io_group_power_on_rx(t_gcr_addr* gcr_addr)
{
    // RX Registers
    put_ptr_field(gcr_addr, rx_clkdist_pdwn,                0b0, read_modify_write); //pg
    put_ptr_field(gcr_addr, rx_ctl_datasm_clkdist_pdwn,     0b0, read_modify_write); //pg
    put_ptr_field(gcr_addr, rx_iref_pdwn_b,                 0b1, read_modify_write); //pg
} //io_group_power_on_rx

// Power down a group (both RX and TX)
void io_group_power_off(t_gcr_addr* gcr_addr)
{
    io_group_power_off_tx(gcr_addr);
    io_group_power_off_rx(gcr_addr);
} //io_group_power_off

void io_group_power_off_tx(t_gcr_addr* gcr_addr)
{
    // TX Registers
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    put_ptr_field(gcr_addr, tx_clkdist_pdwn,     0b1, read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_iref_pdwn_b,      0b0, read_modify_write); //pg
    set_gcr_addr_reg_id(gcr_addr, rx_group);
} //io_group_power_off_tx

void io_group_power_off_rx(t_gcr_addr* gcr_addr)
{
    // RX Registers
    put_ptr_field(gcr_addr, rx_clkdist_pdwn,                0b1, read_modify_write); //pg
    put_ptr_field(gcr_addr, rx_ctl_datasm_clkdist_pdwn,     0b1, read_modify_write); //pg
    put_ptr_field(gcr_addr, rx_iref_pdwn_b,                 0b0, read_modify_write); //pg
} //io_group_power_off_rx


// Decode banks_sel for use by RX power off/on functions
void decode_banks_sel(t_gcr_addr* gcr_addr, t_power_banks_sel banks_sel, int* en_bank_a, int* en_bank_b)
{
    // PSL both_banks
    if (banks_sel == both_banks)
    {
        *en_bank_a = 1;
        *en_bank_b = 1;
    }
    else
    {
        int main_en = banks_sel & main_only;
        int alt_en  = banks_sel & alt_only;
        int bank_sel_a  = get_ptr_field(gcr_addr, rx_bank_sel_a);
        *en_bank_a = (bank_sel_a == 0) ? alt_en  : main_en;
        *en_bank_b = (bank_sel_a == 0) ? main_en : alt_en;
    }
} //decode_banks_sel


int io_lane_power_on_tx(t_gcr_addr* gcr_addr)
{
    // TX Registers
    //"Power down pins, 0=dcc_comp, 1=tx_pdwn, 2=d2, 3=unused, 4=unused, 5=unused"
    //Need to be staggered
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    //Check that tx psave is quiesced and that req is not = 1 will set fir
    int status = check_for_txpsave_req_sts(gcr_addr);

    // Sequence the bank_controls to the on state
    // PSL sequence_tx_bank_controls
    sequence_tx_bank_controls(gcr_addr, power_on);

    // reset tx io domain - HW504112
    put_ptr_field(gcr_addr, tx_iodom_ioreset, 0b1,
                  fast_write); //pl, reset tx io domain, tx_ioreset/tx_iodom_ioreset are only fields in register
    put_ptr_field(gcr_addr, tx_iodom_ioreset, 0b0,
                  fast_write); //pl, reset tx io domain, tx_ioreset/tx_iodom_ioreset are only fields in register

    set_gcr_addr_reg_id(gcr_addr, rx_group);
    io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_lane_power_on_tx


// Turns on Both Banks, Main Only (Alt is briefly turned on to activate clocks), Or Alt Only (assumes have already turned on Main)
int io_lane_power_on_rx(t_gcr_addr* gcr_addr, t_power_banks_sel banks_sel, bool enable_dl_clk)
{
    // RX Registers
    //power down pins, 0=PR64 and C2DIV, 1=CML2CMOS_NBIAS and MINI_PR and IQGEN, 2=CML2CMOS_EDG, 3=CML2CMOS_DAT, 4=PR64 and C2DIV, 5=VDAC"
    //Bank A and B have be staggered also

    // Make sure the ALT bank is not in psave
    alt_bank_psave_clear_and_wait(gcr_addr);

    // Return if just powering up ALT (assume already went through this to power up main)
    // PSL bank_alt_only
    if (banks_sel == alt_only)
    {
        return rc_no_error;
    }

    // Disable CDR if powering up main bank (initial power on before training)
    if (banks_sel != alt_only)
    {
        put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis, fast_write);
        put_ptr_field(gcr_addr, rx_pr_bit_lock_done_ab_clr_alias, 0b11, fast_write);
    }

    //Check that rx psave is quiesced and that req is not = 1 will set fir
    //If the powerdown lane is asked for but we are doing a init or recal than that a no-no see Mike S -- CQ522215
    int status = check_for_rxpsave_req_sts(gcr_addr);

    // Sequence the bank_controls to the on state for both banks
    // PSL sequence_rx_bank_controls_bank_b
    sequence_rx_bank_controls(gcr_addr, bank_b, power_on);
    // PSL sequence_rx_bank_controls_bank_a
    sequence_rx_bank_controls(gcr_addr, bank_a, power_on);

    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,  0b00,
                  read_modify_write); //pl Deassert to sync c16 and c32 clocks (initializes to 1)

    io_sleep(get_gcr_addr_thread(gcr_addr));

    // (HW519449) Reset flywheel of both banks. Force phase accumulator of bank A.
    // The idea is to shake up the phase relationship between the bank clocks.
    // By doing this, the dl_clk_sel sync lats in the custom should flush out
    // their initial or left-over state.
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b11, read_modify_write);
    put_ptr_field(gcr_addr, rx_pr_fw_reset_ab_alias, 0b00, read_modify_write);
    int phase;

    for (phase = 0; phase <= 0x80; phase = phase + 0x20)
    {
        int phase_force_cmd = 0x80 | phase;
        put_ptr_field(gcr_addr, rx_pr_phase_force_cmd_a_alias, phase_force_cmd, fast_write);
    }

    // PSL enable_dl_clk
    if ( enable_dl_clk )
    {
        put_ptr_field(gcr_addr, rx_dl_clk_en,             0b1,
                      read_modify_write); //pl Enable clock to DL if specified (HW508366)
    }

    // reset rx io domain  - HW504112
    put_ptr_field(gcr_addr, rx_iodom_ioreset, 0b1,
                  fast_write); //pl, reset rx io domain, rx_ioreset/rx_iodom_ioreset are only fields in register
    put_ptr_field(gcr_addr, rx_iodom_ioreset, 0b0,
                  fast_write); //pl, reset rx io domain, rx_ioreset/rx_iodom_ioreset are only fields in register

    // -- HW532825 - Restore flywheel snapshot operation (set to 1 during io_lane_power_off)
    put_ptr_field(gcr_addr, rx_pr_psave_val_ena_ab_alias        , 0b00    , read_modify_write);//pl

    // Turn off ALT bank if were meant to turn on main bank only
    // PSL bank_main_only
    if (banks_sel == main_only)
    {
        put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
    }

    //io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_lane_power_on_rx


int io_lane_power_off_tx(t_gcr_addr* gcr_addr)
{
    // TX Registers
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    //Check that tx psave is quiesced and that req is not = 1 will set fir
    int status = check_for_txpsave_req_sts(gcr_addr);

    // Sequence the bank_controls to the off state
    // PSL sequence_tx_bank_controls
    sequence_tx_bank_controls(gcr_addr, power_off);

    set_gcr_addr_reg_id(gcr_addr, rx_group);
    io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_lane_power_off_tx


// Turns off Alt Only or Both Banks
int io_lane_power_off_rx(t_gcr_addr* gcr_addr, t_power_banks_sel banks_sel)
{
    // Turn off alt bank and return if that is the request
    // PSL bank_alt_only
    if (banks_sel == alt_only)
    {
        put_ptr_field(gcr_addr, rx_psave_req_alt_set, 0b1, fast_write);
        return rc_no_error;
    }

    // No point in just having ALT on, so turn it off also even if request is main only
    // RX Registers
    put_ptr_field(gcr_addr, rx_berpl_count_en, 0b0, read_modify_write); // pl disable BERM logic
    put_ptr_field(gcr_addr, rx_dl_clk_en,               0b0,  read_modify_write); //pl Disable clock to DL
    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias, cdr_a_dis_cdr_b_dis,
                  fast_write);//pl -- HW532825 - Need CDR disabled for the flywheel valid chicken switch to work
    put_ptr_field(gcr_addr, rx_hold_div_clks_ab_alias,  0b11,
                  read_modify_write); //pl Assert hold_div_clock to freeze c16 and c32

    //CQ HW530311 -- When we power off a lane manually, make sure the RX psave state machine
    // is flushed into an all zeros state for that lane.  That implies potentially turning the
    // power back on for the alt bank, before proceeding to turn it off again.
    //
    // However, this also enables the CDR and a CDR lock timer, which we don't want.
    // Disabling the psave cdr state machine will disable the timer but not the CDR itself.
    // Therefore we must also turn off the CDR manually here.
    put_ptr_field(gcr_addr, rx_psave_cdr_disable_sm             , 0b1     , read_modify_write);//pl
    put_ptr_field(gcr_addr, rx_psave_cdrlock_mode_sel           , 0b11    , read_modify_write);//pl
    put_ptr_field(gcr_addr, rx_pr_psave_val_ena_ab_alias        , 0b11    ,
                  read_modify_write);//pl -- HW532825 - Make sure flywheel snapshot cannot make us hang

    alt_bank_psave_clear_and_wait(gcr_addr); // Make sure the ALT bank is not in psave

    put_ptr_field(gcr_addr, rx_pr_edge_track_cntl_ab_alias       , 0b000000, fast_write);//pl
    put_ptr_field(gcr_addr, rx_psave_cdr_disable_sm              , 0b0     , read_modify_write);//pl
    int pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if(!pcie_mode)
    {
        // AXO Mode: psave logic waits on the CDR lock (for dynamic bus powerdown)
        put_ptr_field(gcr_addr, rx_psave_cdrlock_mode_sel            , 0b00    , read_modify_write);//pl
    }

    //Check that rx psave is quiesced and that req is not = 1 will set fir
    //If the powerdown lane is asked for but we are doing a init or recal than that a no-no see Mike S -- CQ522215
    int status = check_for_rxpsave_req_sts(gcr_addr);
    io_sleep(get_gcr_addr_thread(gcr_addr));

    // Sequence the bank_controls to the off state
    // PSL sequence_rx_bank_controls_bank_b
    sequence_rx_bank_controls(gcr_addr, bank_b, power_off);
    // PSL sequence_rx_bank_controls_bank_a
    sequence_rx_bank_controls(gcr_addr, bank_a, power_off);

    io_sleep(get_gcr_addr_thread(gcr_addr));
    return status;
} //io_lane_power_off_rx
