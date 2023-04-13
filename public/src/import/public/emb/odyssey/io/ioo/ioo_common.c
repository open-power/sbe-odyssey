/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_common.c $    */
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
// *! FILENAME    : ioo_common.c
// *! TITLE       :
// *! DESCRIPTION : IOO Common functions
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23030200 |vbr     | Issue 300456: Only check specific banks in wait for cdr lock
// vbr23020300 |vbr     | Issue 298086: new function to remove edge pr offset on PIPE rate change or rx eq eval
// vbr22120900 |vbr     | Only allow lanes 0-3 for PCIe (instead of 0-4)
// jjb22112200 |jjb     | reduced if statements in eo_update_poff_avg
// vbr22120600 |vbr     | Fixed the wait on done in apply_rx_dac_offset()
// jfg22111800 |jfg     | Remove extra comment code
// jfg22111701 |jfg     | Add read_active_pr and related defines
// vbr22111700 |vbr     | Use latch_dac_double_mode for saving/restoring latch dacs
// vbr22111501 |vbr     | New function to back out the DFE instead of a restore. Do not sleep during psave wait.
// vbr22111500 |vbr     | Updated pipe_abort configs to only include rxactive in recal
// vbr22110300 |vbr     | Issue 293889: Can not use bcast for setting the pipe abort masks
// vbr22110200 |vbr     | Issue 293622: Fixed conditionals in wait for cdr lock and updated pcie exit check
// vbr22100401 |vbr     | Issue 291616: Check for pipe abort in dccal/init; functions to change pipe_abort sources for dccal/init vs recal
// vbr22100400 |vbr     | Issue 290234: exit wait_for_cdr_lock() on a pcie pipe reset
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22062800 |vbr     | Common function for taking alt bank out of psave
// vbr22061500 |vbr     | Add external command fail reporting
// vbr22060200 |vbr     | Added attribute to ensure proper data alignment of packed struct
// vbr22041200 |vbr     | Issue 277473 - account for data mini PR being right of center and thus a negative adjust is needed in restore function
// mbs22021000 |mbs     | Updates to reduce code size
// vbr22012801 |vbr     | Added common functions for DAC accelerator
// vbr22012800 |vbr     | Updated some functions to use less reads
// mwh21101900 |mwh     | Add poff_avg code make common for all three methods of path offset
// vbr21102600 |vbr     | Added rx peak/lte preset function
// vbr21102200 |vbr     | Added error condition checking on the save/restore functions that sets fatal fir
// vbr21102100 |vbr     | Switched from u8 to u32 accesses for latch dac save/restore to improve speed; added resetting of dfe clk_adj
// vbr21101900 |vbr     | Added tx dcc save/restore functions
// vbr21101200 |vbr     | Added latch dac save/restore functions
// mwh21092300 |mwh     | Moved tx_write_4_bit_pat and in_tx_half_width_mode into libs because need be common for dcc iot and ioo
// vbr21061700 |vbr     | DL recal_abort is abort_error, bump_ui_mode and pcie_cmd are abort_clean
// vbr21061000 |vbr     | Added bump_ui to recal abort check
// vbr21060800 |vbr     | Updates to checking for recal abort
// vbr21052200 |vbr     | Added sleeps to reduce thread blocking
// mwh21040100 |mwh     | Removed rx_eo_loff_servo_setup eoff has it own registers now
// mwh21032600 |mwh     | change function check_for_txpsave_req_sts to use per lane tx req/sts
// vbr21020500 |vbr     | Moved common ioo/iot functions to io_lib. Removed eo_copy_lane_cal.
// vbr21012200 |vbr     | Added a loff servo setup function for loff/eoff
// vbr21012500 |vbr     | Updated mem_reg recal abort
// vbr20043001 |vbr     | Made amp servo setup more efficient (less writes)
// vbr20043000 |vbr     | Copied efficient servo queue check from eo_lte() into common function for use by other steps
// vbr20111800 |vbr     | HW552111: Added escape to wait_for_cdr_lock()
// gap20100800 |gap     | Remove is_p10_dd1
//-------------|P11 ----|-------------------------------------------------------
// gap20091500 |gap     | HW542315: updated to use is_p10_dd1
// bja20090800 |bja     | HW542315: make in_tx_half_width_mode() return false for p10 dd1 omi
// gap20082500 |gap     | HW542315: add tx_write_4_bit_pat to handle half/full width modes
// vbr20101201 |vbr     | HW549006: Removed the abort check on recal_req since that doesn't work with auto recal and command interface recals.
// vbr20101200 |vbr     | HW549006: DL asserting psave_req or lowering recal_req when entering degraded mode is treated as a recal abort.
// gap20091500 |gap     | HW542315: updated to use is_p10_dd1
// bja20090800 |bja     | HW542315: make in_tx_half_width_mode() return false for p10 dd1 omi
// gap20082500 |gap     | HW542315: add tx_write_4_bit_pat to handle half/full width modes
// vbr20030500 |vbr     | HW523782/HW523779: When fw_spread_en, min cdr lock wait time is 5us.
// mwh20040201 |mwh     | Fix a issue with having cap Int change to int -- add ; in debug_state in fir.
// mwh20040200 |mwh     | Fix a comment issue had tx when should of been rx
// mwh20040100 |mwh     | Add in debug state for check_for_rxpsave_req_sts and check_for_txpsave_req_sts
// mwh20022400 |mwh     | Add in function check_for_rxpsave_req_sts and check_for_txpsave_req_sts
// vbr20021100 |vbr     | HW522731: smarter lane cal copy based on flywheel lane.
// vbr20020600 |vbr     | HW522210: Check for lane_bad before copying lane cal; added set/clr_rx_lane_bad() functions.
// bja20021300 |bja     | Don't override tx_fifo_l2u_dly bc reset val is appropriate
// bja20020500 |bja     | Add tx_fifo_init()
// cws20011400 |cws     | Added Debug Logs
// jfg19091200 |jfg     | Experimental ONLY change for DFE filter parms RE ALLEYEOPT DAC inconsistency
// vbr19092601 |vbr     | Added option to enable/disable copying of LTE settings between lanes
// mbs19072500 |mbs     | Added amp_setting_ovr_enb
// mbs19051600 |mbs     | HW491617: Separated servo setup for dfe_fast and dfe_full
// vbr19050500 |vbr     | Copy LTE settings between lanes
// vbr19041500 |vbr     | Updated register names
// mbs19030400 |mbs     | Added set_fir call in check_rx_abort
// vbr19012200 |vbr     | Only check recal abort when init_done.
// cws19011300 |cws     | Updated dfe rx_eo_servo_setup inc2/dec2 to latest defaults
// vbr18111400 |vbr     | Updated calls to set_fir.
// vbr18101700 |vbr     | Updated wait_for_cdr_lock() to use the timescaling (not needed if we always run at same frequency, but safer to include).
// vbr16072600 |vbr     | Replaced Debruijn with __builtin_ctz (significantly faster, ~248B saved).
// vbr16030400 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "io_logger.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS / PROCEDURES
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Take ALT bank out of psave and wait for it to finish powering up
void alt_bank_psave_clear_and_wait(t_gcr_addr* gcr_addr)
{
    put_ptr_field(gcr_addr, rx_psave_req_alt_clr, 0b1, fast_write);
    int psave_sts = 1;

    while (psave_sts)
    {
        psave_sts = get_ptr_field(gcr_addr, rx_psave_sts_alt);
        //if (psave_sts) { io_sleep(get_gcr_addr_thread(gcr_addr)); }
    }
} //alt_bank_psave_clear_and_wait


// Wait for lock on both banks.
// Set FIR if not locked after X checks.
// Lock could take relatively long due to spread spectrum, so use sleep in between checks.
PK_STATIC_ASSERT(rx_pr_locked_ab_alias_width == 2);
int wait_for_cdr_lock(t_gcr_addr* gcr_addr, t_bank check_bank, bool set_fir_on_error)
{
    PkTimebase start_time = pk_timebase_get();

    // HW523782/HW523779: When spread is enabled, we wait a minumum of 5 us before checking for lock.
    // This is to prevent advancing on the false lock that may occur when the flywheel is far from the correct value.
    // Since this function is called both right after enabling the CDR and again at the end of cal before switching banks,
    // this minimum wait will be incurred twice in each calibration.
    //

    // PSL fw_spread_en
    if (fw_field_get(fw_spread_en))
    {
        io_wait_us(get_gcr_addr_thread(gcr_addr), 5); // sleep until 5us has elapsed
    }

    int pcie_mode = fw_field_get(fw_pcie_mode);
    int bank_mask = bank_to_bitfield_ab_mask(check_bank);
    int loop_count = 0;

    while ( true )
    {
        loop_count++;
        // Check CDR status
        int locked_ab = get_ptr_field(gcr_addr, rx_pr_locked_ab_alias);

        // PSL locked_ab
        if ( (locked_ab & bank_mask) == bank_mask )
        {
            return pass_code; // Exit when locked
        }
        else
        {
            // PCIe check for abort
            if (pcie_mode)
            {
                int pipe_abort = check_rx_abort(gcr_addr);

                // PSL pipe_abort
                if (pipe_abort)
                {
                    return pass_code;  // Exit on a PCIe PIPE Abort (Issue 290234/293622)
                }
            } //pcie_mode

            // Check timeout if not locked as yet. Check if hit a loop limit (in case timer is broke HW552111).
            bool timeout = (pk_timebase_get() - start_time) > (32 * scaled_microsecond);
            bool escape  = (loop_count >= 32); // 32 loops is much more that 32 us with min thread sleep times

            if (timeout || escape)
            {
                if(set_fir_on_error)
                {
                    // PSL set_fir_bad_lane_warning
                    set_fir(fir_code_bad_lane_warning);
                } // Set FIR on a timeout.

                ADD_LOG(DEBUG_RX_CDR_LOCK_TIMEOUT, gcr_addr, locked_ab);
                return warning_code; // Exit when timeout
            }

            // Sleep before polling again
#if PK_THREAD_SUPPORT
            io_sleep(get_gcr_addr_thread(gcr_addr));
#else
            io_spin_us(polling_interval_us);
#endif
        } //if...else...
    } //while
} //wait_for_cdr_lock


// Abort check
// Called after servo ops to check for an abort condition. Returns abort_*_code if an abort code is detected; returns pass_code otherwise.
PK_STATIC_ASSERT(rx_dl_phy_ro_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_dl_phy_ro_full_reg_alias_addr == rx_dl_phy_recal_abort_sticky_addr);
PK_STATIC_ASSERT(rx_dl_phy_ro_full_reg_alias_addr == rx_dl_phy_bump_recal_abort_sticky_addr);
int check_rx_abort(t_gcr_addr* gcr_addr)
{
    int ret_val = pass_code;

    // PCIe always checks for a pipe abort
    int pcie_mode = fw_field_get(fw_pcie_mode);

    // PSL pcie_mode
    if (pcie_mode)
    {
        // Check for any PIPE command as a clean abort
        int pipe_ifc_recal_abort = get_ptr_field(gcr_addr, rx_pipe_ifc_recal_abort);

        // PSL pcie_recal_abort
        if (pipe_ifc_recal_abort)
        {
            ret_val |= abort_clean_code;
        }
    }

    // Only perform when running in recal.
    int recal = mem_pg_field_get(rx_running_recal);

    // PSL recal_running
    if (recal)
    {
        // Check the recal_abort mem_reg
        int lane = get_gcr_addr_lane(gcr_addr);
        int abort_val0 = (0x80000000 >> lane) & ( (mem_pg_field_get(rx_recal_abort_0_15) << 16) | (mem_pg_field_get(
                             rx_recal_abort_16_23) << (16 - rx_recal_abort_16_23_width)) );

        if (abort_val0)
        {
            ret_val |= abort_error_code;
        }

        // PSL axo_mode
        if (!pcie_mode)
        {
            // Check the DL sticky abort and DL sticky bump abort signals when AXO mode
            int rx_dl_phy_ro_full_reg = get_ptr_field(gcr_addr, rx_dl_phy_ro_full_reg_alias);
            int recal_abort_error = rx_dl_phy_ro_full_reg & rx_dl_phy_recal_abort_sticky_mask;
            int recal_abort_clean = rx_dl_phy_ro_full_reg & rx_dl_phy_bump_recal_abort_sticky_mask;

            // HW549006: DL may assert psave_req and lower recal_req in the middle of a recal when entering degraded mode; treat this as an abort.
            // Only checking psave_req and not recal_req since recal_req will not be asserted in an auto recal or command interface recal.
            int psave_req = get_ptr_field(gcr_addr, rx_psave_req_dl);
            recal_abort_error |= psave_req;

            // PSL axo_recal_abort_error
            if (recal_abort_error)
            {
                ret_val |= abort_error_code;
            }

            // PSL axo_recal_abort_clean
            if (recal_abort_clean)
            {
                ret_val |= abort_clean_code;
            }
        }

        if (ret_val & abort_error_code)
        {
            // Set FIR on an abort_error
            // PSL set_fir_recal_abort
            set_fir(fir_code_recal_abort);
            ADD_LOG(DEBUG_RECAL_ABORT, gcr_addr, 0x0);
        }
    } //if(recal)

    return ret_val;
} //check_rx_abort


// Function for setting the PIPE Abort source depending on recal or dccal/init.
// Only needed for PCIe mode, but doesn't hurt to call in AXO mode
PK_STATIC_ASSERT(pipe_recal_abort_mask_0_15_alias_width == 16);
void pipe_abort_config(t_gcr_addr* gcr_addr, t_pipe_abort_cfg pipe_abort_cfg)
{

    // When not training (idle) want all abort sources selected (mask==0) except rxactive on all lanes.
    // In recal want all abort sources selected (mask==0) except rxactive on all lanes; but include rxactive on the recal lane.
    // In dccal/init want all abort sources unselected (mask==1) on all lanes except on current lane (enable the pipe_reset only).
    int all_lane_mask_val, cur_lane_mask_val;

    if (pipe_abort_cfg == PIPE_ABORT_CFG_IDLE)
    {
        // Do not need to bypass other things to handle PIPE commands on a lane being inactive.
        // Enable bypassing for all other PIPE commands on any lane.
        all_lane_mask_val = pipe_recal_abort_mask_rxactive_mask;
        cur_lane_mask_val = pipe_recal_abort_mask_rxactive_mask;
    }
    else if (pipe_abort_cfg == PIPE_ABORT_CFG_RECAL)
    {
        // Need to interrupt recal on the current lane being inactive (none of the other lanes).
        // Need to interrupt recal on any PIPE command on any lane.
        all_lane_mask_val = pipe_recal_abort_mask_rxactive_mask;
        cur_lane_mask_val = 0x0000;
    }
    else     // PIPE_ABORT_CFG_DCCAL || PIPE_ABORT_CFG_INITCAL
    {
        // Need to interrupt dccal/initial training  if the current lane is reset (none of the other lanes).
        // Do not interrupt on any other PIPE command on any lane.
        all_lane_mask_val = 0xFFFF;
        cur_lane_mask_val = 0xFFFF & ~pipe_recal_abort_mask_resetn_active_mask;
    }

    // Write desired mask to all lanes in the group.
    // Can not use a bcast because that will write to all lanes in the multigroup.
    int num_lanes = get_num_rx_physical_lanes(); //use rx instead of tx so ignore the extra zcal lane
    int saved_lane = get_gcr_addr_lane(gcr_addr);
    set_gcr_addr_reg_id(gcr_addr, tx_group);
    int lane;

    for (lane = 0; lane < num_lanes; lane++)
    {
        set_gcr_addr_lane(gcr_addr, lane);
        int reg_val =  (lane == saved_lane) ? cur_lane_mask_val : all_lane_mask_val;
        put_ptr_field_fast(gcr_addr, pipe_recal_abort_mask_0_15_alias, reg_val);
    }

    set_gcr_addr_reg_id_lane(gcr_addr, rx_group, saved_lane);
} //pipe_abort_config


// The ordering of this struct is very important. It must match the regdef bitfield definitions.
PK_STATIC_ASSERT(rx_amp_servo_mask_h0_width == 1);
PK_STATIC_ASSERT(rx_amp_servo_vote_bias_inc_width == 1);
PK_STATIC_ASSERT(rx_amp_servo_vote_bias_dec_width == 1);
PK_STATIC_ASSERT(rx_amp_servo_mask_h0_shift == rx_amp_servo_vote_bias_inc_shift + 1);
PK_STATIC_ASSERT(rx_amp_servo_mask_h0_shift == rx_amp_servo_vote_bias_dec_shift + 2);
PK_STATIC_ASSERT(rx_amp_filter_depth_inc0_shift == 12);
PK_STATIC_ASSERT(rx_amp_filter_depth_dec0_shift == 8);
PK_STATIC_ASSERT(rx_amp_filter_depth_inc1_shift == 4);
PK_STATIC_ASSERT(rx_amp_filter_depth_dec1_shift == 0);
PK_STATIC_ASSERT(rx_amp_filter_depth_inc2_shift == 12);
PK_STATIC_ASSERT(rx_amp_filter_depth_dec2_shift == 8);
PK_STATIC_ASSERT(rx_amp_filter_depth_inc3_shift == 4);
PK_STATIC_ASSERT(rx_amp_filter_depth_dec3_shift == 0);
typedef struct struct_amp_servo_settings
{
    // rx_ctl_cntl2_pg
    unsigned int dummy_data_0               : 13;
    unsigned int rx_amp_servo_mask_h0       :  1;
    unsigned int rx_amp_servo_vote_bias_inc :  1;
    unsigned int rx_amp_servo_vote_bias_dec :  1;

    // rx_ctl_mode4_pg
    unsigned int rx_amp_filter_depth_inc0   :  4;
    unsigned int rx_amp_filter_depth_dec0   :  4;
    unsigned int rx_amp_filter_depth_inc1   :  4;
    unsigned int rx_amp_filter_depth_dec1   :  4;

    // rx_ctl_mode9_pg
    unsigned int rx_amp_filter_depth_inc2   :  4;
    unsigned int rx_amp_filter_depth_dec2   :  4;
    unsigned int rx_amp_filter_depth_inc3   :  4;
    unsigned int rx_amp_filter_depth_dec3   :  4;
} __attribute__((packed, aligned(2))) t_amp_servo_settings;

// VGA: H0=X (don't care), Outer Eye (Amax)
static const t_amp_servo_settings c_vga_settings =
{
    .dummy_data_0 = 0,
    .rx_amp_servo_mask_h0        = 0,
    .rx_amp_servo_vote_bias_inc  = 1,
    .rx_amp_servo_vote_bias_dec  = 0,

    .rx_amp_filter_depth_inc0    = 0,
    .rx_amp_filter_depth_dec0    = 4,

    .rx_amp_filter_depth_inc1    = 0,
    .rx_amp_filter_depth_dec1    = 4,

    .rx_amp_filter_depth_inc2    = 0,
    .rx_amp_filter_depth_dec2    = 4,

    .rx_amp_filter_depth_inc3    = 0,
    .rx_amp_filter_depth_dec3    = 6,
};

// DFE Fast: H0=1, Slight Inner Eye Bias
const t_amp_servo_settings c_dfe_fast_settings =
{
    .dummy_data_0 = 0,
    .rx_amp_servo_mask_h0        = 1,
    .rx_amp_servo_vote_bias_inc  = 0,
    .rx_amp_servo_vote_bias_dec  = 0,

    .rx_amp_filter_depth_inc0    = 3,
    .rx_amp_filter_depth_dec0    = 3,

    .rx_amp_filter_depth_inc1    = 2,
    .rx_amp_filter_depth_dec1    = 2,

    .rx_amp_filter_depth_inc2    = 2,
    .rx_amp_filter_depth_dec2    = 2,

    .rx_amp_filter_depth_inc3    = 5,
    .rx_amp_filter_depth_dec3    = 4,
};

// DFE Full: H0=1, Inner Eye (Amin)
const t_amp_servo_settings c_dfe_full_settings =
{
    .dummy_data_0 = 0,
    .rx_amp_servo_mask_h0        = 1,
    .rx_amp_servo_vote_bias_inc  = 0,
    .rx_amp_servo_vote_bias_dec  = 1,

    .rx_amp_filter_depth_inc0    = 5,
    .rx_amp_filter_depth_dec0    = 0,

    .rx_amp_filter_depth_inc1    = 5,
    .rx_amp_filter_depth_dec1    = 1,

    .rx_amp_filter_depth_inc2    = 4,
    .rx_amp_filter_depth_dec2    = 0,

    .rx_amp_filter_depth_inc3    = 6,
    .rx_amp_filter_depth_dec3    = 0,
};

// Write the selected setting from above to the Amp Servo Settings
PK_STATIC_ASSERT(rx_amp_filter_depth_inc_dec_0_1_alias_width == 16);
PK_STATIC_ASSERT(rx_amp_filter_depth_inc_dec_2_3_alias_width == 16);
void rx_eo_amp_servo_setup(t_gcr_addr* i_tgt, const t_amp_servo_setup i_servo_setup)
{
    // Select the settings to use
    //t_amp_servo_settings selected_settings = c_dfe_fast_settings;
    uint16_t* selected_settings = (uint16_t*)(&c_vga_settings);

    switch(i_servo_setup)
    {
        case SERVO_SETUP_VGA:
            // PSL vga_break
            selected_settings = (uint16_t*)(&c_vga_settings);
            break;

        case SERVO_SETUP_DFE_FULL:
            // PSL dfe_full_break
            selected_settings = (uint16_t*)(&c_dfe_full_settings);
            break;

        case SERVO_SETUP_DFE_FAST:
            // PSL dfe_fast_break
            selected_settings = (uint16_t*)(&c_dfe_fast_settings);
            break;
    }

    // Always write the settings in rx_ctl_cntl2_pg; there are other settings in this register
    uint16_t amp_servo_mask_h0_vote_bias_inc_dec_val = selected_settings[0];
    //(selected_settings.rx_amp_servo_mask_h0       << (rx_amp_servo_mask_h0_shift       - rx_amp_servo_maskh0_bias_incdec_alias_shift)) |
    //(selected_settings.rx_amp_servo_vote_bias_inc << (rx_amp_servo_vote_bias_inc_shift - rx_amp_servo_maskh0_bias_incdec_alias_shift)) |
    //(selected_settings.rx_amp_servo_vote_bias_dec << (rx_amp_servo_vote_bias_dec_shift - rx_amp_servo_maskh0_bias_incdec_alias_shift));
    put_ptr_field(i_tgt, rx_amp_servo_maskh0_bias_incdec_alias, amp_servo_mask_h0_vote_bias_inc_dec_val, read_modify_write);

    // Switch for sim or lab to disable settings overrides
    int amp_setting_ovr = mem_pg_field_get(amp_setting_ovr_enb);

    if (amp_setting_ovr == 0)
    {
        // There is nothing else in rx_ctl_mode4_pg so can use fast_write
        uint16_t amp_filter_depth_inc_dec_0_1_val = selected_settings[1];
        //(selected_settings.rx_amp_filter_depth_inc0 << rx_amp_filter_depth_inc0_shift) |
        //(selected_settings.rx_amp_filter_depth_dec0 << rx_amp_filter_depth_dec0_shift) |
        //(selected_settings.rx_amp_filter_depth_inc1 << rx_amp_filter_depth_inc1_shift) |
        //(selected_settings.rx_amp_filter_depth_dec1 << rx_amp_filter_depth_dec1_shift);
        put_ptr_field(i_tgt, rx_amp_filter_depth_inc_dec_0_1_alias, amp_filter_depth_inc_dec_0_1_val, fast_write);

        // There is nothing else in rx_ctl_mode9_pg so can use fast_write
        uint16_t amp_filter_depth_inc_dec_2_3_val = selected_settings[2];
        //(selected_settings.rx_amp_filter_depth_inc2 << rx_amp_filter_depth_inc2_shift) |
        //(selected_settings.rx_amp_filter_depth_dec2 << rx_amp_filter_depth_dec2_shift) |
        //(selected_settings.rx_amp_filter_depth_inc3 << rx_amp_filter_depth_inc3_shift) |
        //(selected_settings.rx_amp_filter_depth_dec3 << rx_amp_filter_depth_dec3_shift);
        put_ptr_field(i_tgt, rx_amp_filter_depth_inc_dec_2_3_alias, amp_filter_depth_inc_dec_2_3_val, fast_write);
    } //!amp_setting_ovr

    return;
} //rx_eo_amp_servo_setup


// The ordering of this struct is very important. It must match the regdef bitfield definitions.
//PK_STATIC_ASSERT(rx_loff_filter_depth0_shift == 12);
//PK_STATIC_ASSERT(rx_loff_filter_depth1_shift == 8);
//PK_STATIC_ASSERT(rx_loff_filter_depth2_shift == 4);
//PK_STATIC_ASSERT(rx_loff_filter_depth3_shift == 0);
//PK_STATIC_ASSERT(rx_loff_inc_dec_amt0_shift == 13);
//PK_STATIC_ASSERT(rx_loff_thresh1_shift == 8);
//PK_STATIC_ASSERT(rx_loff_inc_dec_amt1_shift == 5);
//PK_STATIC_ASSERT(rx_loff_thresh2_shift == 0);
//PK_STATIC_ASSERT(rx_loff_inc_dec_amt2_shift == 13);
//PK_STATIC_ASSERT(rx_loff_thresh3_shift == 8);
//PK_STATIC_ASSERT(rx_loff_inc_dec_amt3_shift == 5);
//PK_STATIC_ASSERT(rx_loff_thresh4_shift == 0);
//typedef struct struct_loff_servo_settings {
//  // rx_ctl_mode15_pg
//  unsigned int rx_loff_filter_depth0   :  4;
//  unsigned int rx_loff_filter_depth1   :  4;
//  unsigned int rx_loff_filter_depth2   :  4;
//  unsigned int rx_loff_filter_depth3   :  4;
//
//  // rx_ctl_mode16_pg
//  unsigned int rx_loff_inc_dec_amt0    :  3;
//  unsigned int rx_loff_thresh1         :  5;
//  unsigned int rx_loff_inc_dec_amt1    :  3;
//  unsigned int rx_loff_thresh2         :  5;
//
//  // rx_ctl_mode17_pg
//  unsigned int rx_loff_inc_dec_amt2    :  3;
//  unsigned int rx_loff_thresh3         :  5;
//  unsigned int rx_loff_inc_dec_amt3    :  3;
//  unsigned int rx_loff_thresh4         :  5;
//}__attribute__((packed), aligned(2)) t_loff_servo_settings;

//// Latch Offset (DC Cal, Fenced)
//static const int  c_loff_hyst_start = 5;
//static const t_loff_servo_settings c_loff_settings = {
//  .rx_loff_filter_depth0   = 3,
//  .rx_loff_filter_depth1   = 3,
//  .rx_loff_filter_depth2   = 3,
//  .rx_loff_filter_depth3   = 4,
//
//  .rx_loff_inc_dec_amt0    = 4,
//  .rx_loff_thresh1         = 1,
//
//  .rx_loff_inc_dec_amt1    = 2,
//  .rx_loff_thresh2         = 2,
//
//  .rx_loff_inc_dec_amt2    = 2,
//  .rx_loff_thresh3         = 3,
//
//  .rx_loff_inc_dec_amt3    = 0,
//  .rx_loff_thresh4         = 7
//};
//
//// Edge Offset (Live Data)
//static const int  c_eoff_hyst_start = 1;
//static const t_loff_servo_settings c_eoff_settings = {
//  .rx_loff_filter_depth0   = 10,
//  .rx_loff_filter_depth1   = 10,
//  .rx_loff_filter_depth2   = 10,
//  .rx_loff_filter_depth3   = 10,
//
//  .rx_loff_inc_dec_amt0    = 0,
//  .rx_loff_thresh1         = 0,
//
//  .rx_loff_inc_dec_amt1    = 0,
//  .rx_loff_thresh2         = 0,
//
//  .rx_loff_inc_dec_amt2    = 0,
//  .rx_loff_thresh3         = 0,
//
//  .rx_loff_inc_dec_amt3    = 0,
//  .rx_loff_thresh4         = 3
//};
//
//// Write the selected setting from above to the Amp Servo Settings
//PK_STATIC_ASSERT(rx_loff_filter_depth_0_3_alias_width == 16);
//PK_STATIC_ASSERT(rx_loff_inc_dec_0_1_thresh_1_2_alias_width == 16);
//PK_STATIC_ASSERT(rx_loff_inc_dec_2_3_thresh_3_4_alias_width == 16);
//void rx_eo_loff_servo_setup(t_gcr_addr *i_tgt, const t_loff_servo_setup i_servo_setup)
//{
//  // Select the settings to use
//  uint16_t *selected_settings = (uint16_t*)(&c_loff_settings);
//  int selected_hyst_start = c_loff_hyst_start;
//  switch(i_servo_setup)
//  {
//    case SERVO_SETUP_LOFF:
//      selected_settings = (uint16_t*)(&c_loff_settings);
//      selected_hyst_start = c_loff_hyst_start;
//      break;
//
//    case SERVO_SETUP_EOFF:
//      selected_settings = (uint16_t*)(&c_eoff_settings);
//      selected_hyst_start = c_eoff_hyst_start;
//      break;
//  }
//
//  // Always write this register to select between live data mode (EOFF) or not (LOFF); there may be other settings in this register
//  int live_edge_mode = (i_servo_setup == SERVO_SETUP_LOFF) ? 0b0 : 0b1;
//  put_ptr_field(i_tgt, rx_loff_livedge_mode, live_edge_mode, read_modify_write);
//
//  // Switch for sim or lab to disable settings overrides
//  int loff_setting_ovr = mem_pg_field_get(loff_setting_ovr_enb);
//  if (loff_setting_ovr == 0) {
//    // There are other settings in this register
//    put_ptr_field(i_tgt, rx_loff_hyst_start, selected_hyst_start, read_modify_write);
//
//    // There is nothing else in rx_ctl_mode15_pg so can use a fast_write
//    uint16_t loff_filter_depth_0_3_val = selected_settings[0];
//    put_ptr_field(i_tgt, rx_loff_filter_depth_0_3_alias, loff_filter_depth_0_3_val, fast_write);
//
//    // There is nothing else in rx_ctl_mode16_pg so can use a fast_write
//    uint16_t loff_inc_dec_0_1_thresh_1_2_val = selected_settings[1];
//    put_ptr_field(i_tgt, rx_loff_inc_dec_0_1_thresh_1_2_alias, loff_inc_dec_0_1_thresh_1_2_val, fast_write);
//
//    // There is nothing else in rx_ctl_mode17_pg so can use a fast_write
//    uint16_t loff_inc_dec_2_3_thresh_3_4_val = selected_settings[2];
//    put_ptr_field(i_tgt, rx_loff_inc_dec_2_3_thresh_3_4_alias, loff_inc_dec_2_3_thresh_3_4_val, fast_write);
//  } //!loff_setting_ovr
//
//  return;
//} //rx_eo_loff_servo_setup


//Check that tx psave is quiesced and that req is not = 1 will set fir
int check_for_txpsave_req_sts(t_gcr_addr* gcr_addr)  //start void
{
    set_debug_state(0x0300); //start check_for_txpsave_req_sts
    set_gcr_addr_reg_id(gcr_addr, tx_group);

    // Wait for the lane to has req = sts
    int phy_dl_tx_psave_sts, phy_dl_tx_psave_req;

    int loop_count = 0;

    do
    {
        int reg_val = get_ptr_field(gcr_addr, tx_psave_req_dl_sts_phy_alias);//pl
        phy_dl_tx_psave_sts = (reg_val & 0b01);
        phy_dl_tx_psave_req = (reg_val & 0b10) >> 1;

        loop_count++;

        if ((loop_count % 2) == 0)
        {
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // PSL tx_psave_sts_ne_req
    }
    while (phy_dl_tx_psave_sts ^ phy_dl_tx_psave_req);   // bit xoring

    set_debug_state(0x0302); //out of do loop check_for_txpsave_req_sts

    //If the powerdown lane req is high after check that req=sts -- error -- see Mike S -- CQ522215
    int status = rc_no_error;

    if (phy_dl_tx_psave_req)
    {
        // PSL set_fir_fatal_error
        set_debug_state(0x0303);
        set_fir(fir_code_fatal_error);
        status = rc_error;
    }

    return status;
}//check_for_txpsave_req_sts


//Check that rx psave is quiesced and that req is not = 1 will set fir
int check_for_rxpsave_req_sts(t_gcr_addr* gcr_addr)  //begin void
{
    set_debug_state(0x0310); //start check_for_rxpsave_req_sts
    // Wait for the lane to has req = sts
    set_gcr_addr_reg_id(gcr_addr, rx_group);

    //If the powerdown lane is asked for but we are doing a init or recal than that a no-no see Mike S -- CQ522215
    int rx_dl_phy_stat_full_reg      = get_ptr_field(gcr_addr, rx_dl_phy_ro_full_reg_alias);
    int rx_dl_phy_run_lane_recal_req = rx_dl_phy_stat_full_reg & (rx_dl_phy_run_lane_mask | rx_dl_phy_recal_req_mask);

    if (rx_dl_phy_run_lane_recal_req)
    {
        set_debug_state(0x0314);
        set_fir(fir_code_fatal_error);
    }

    int rx_psave_sts_alt_int , rx_psave_req_alt_int;
    int rx_psave_sts_phy_int, rx_psave_req_dl_int;

    int loop_count = 0;

    do
    {

        int reg_val = get_ptr_field(gcr_addr, rx_psave_alt_req_sts_alias);//pl
        rx_psave_sts_alt_int = (reg_val & 0b01);
        rx_psave_req_alt_int = (reg_val & 0b10) >> 1;

        reg_val = get_ptr_field(gcr_addr, rx_psave_req_dl_sts_phy_alias);//pl
        rx_psave_sts_phy_int = (reg_val & 0b01);
        rx_psave_req_dl_int  = (reg_val & 0b10) >> 1;

        loop_count++;

        if ((loop_count % 2) == 0)
        {
            io_sleep(get_gcr_addr_thread(gcr_addr));
        }

        // PSL rx_psave_sts_ne_req
    }
    while ( (rx_psave_sts_alt_int ^ rx_psave_req_alt_int) | (rx_psave_sts_phy_int ^ rx_psave_req_dl_int) );

    set_debug_state(0x0312); //out of do loop check_for_rxpsave_req_sts

    //If the powerdown lane req is high after check that req=sts -- error -- see Mike S -- CQ522215
    int status = rc_no_error;

    if (rx_psave_req_alt_int | rx_psave_req_dl_int)
    {
        // PSL set_fir_fatal_error
        set_debug_state(0x0313);
        set_fir(fir_code_fatal_error);
        status = rc_error;
    }

    return status;
}//check_for_rxpsave_req_sts


//for updating poff_avg_a/b across all 3 diff methods
void eo_update_poff_avg ( t_gcr_addr* gcr_addr, int poff_value, t_bank bank, int lane )
{
    //begin eo_update_poff_avg common
    //int lane = get_gcr_addr_lane(gcr_addr);
    //for getting rel path offset need add previous value or if enough runs go 0
    int poff_avg_before = 0;

    // PSL bank_a
    if (bank == bank_a )
    {
        //bank A is alt B is main
        poff_avg_before = TwosCompToInt(mem_pl_field_get(poff_avg_a, lane), poff_avg_a_width); //pl
        mem_pl_field_put(poff_avg_a, lane, IntToTwosComp((poff_value + poff_avg_before), poff_avg_a_width)); //pl
    }
    else
    {
        //bank B is alt A is main
        poff_avg_before = TwosCompToInt(mem_pl_field_get(poff_avg_b, lane), poff_avg_b_width); //pl
        mem_pl_field_put(poff_avg_b, lane, IntToTwosComp((poff_value + poff_avg_before), poff_avg_b_width)); //pl
    }

    return;
}//end eo_update_poff_avg

// Use the DAC accelerator to apply an offset to the Data and/or Edge DACs in a Bank
// Waits for the SM to complete before returning.
PK_STATIC_ASSERT(rx_apply_poff_ad_run_startbit  == rx_apply_poff_ae_run_startbit + 1);
PK_STATIC_ASSERT(rx_apply_poff_ae_done_startbit == rx_apply_poff_ae_run_startbit + 2);
PK_STATIC_ASSERT(rx_apply_poff_ad_done_startbit == rx_apply_poff_ae_run_startbit + 3);
PK_STATIC_ASSERT(rx_apply_poff_be_run_startbit  == rx_apply_poff_ae_run_startbit + 4);
PK_STATIC_ASSERT(rx_apply_poff_bd_run_startbit  == rx_apply_poff_ae_run_startbit + 5);
PK_STATIC_ASSERT(rx_apply_poff_be_done_startbit == rx_apply_poff_ae_run_startbit + 6);
PK_STATIC_ASSERT(rx_apply_poff_bd_done_startbit == rx_apply_poff_ae_run_startbit + 7);
void apply_rx_dac_offset(t_gcr_addr* gcr_addr, t_data_edge_dac_sel dac_sel, t_bank bank, int offset)
{
    // Mask int32 to correct field width and then write
    // OK to overwrite other field in this register (h3_adj)
    int poff_adj_regval = IntToTwosComp(offset, rx_poff_adj_width);
    put_ptr_field(gcr_addr, rx_poff_adj, poff_adj_regval, fast_write);

    // Clear any existing DAC accelerator errors (shouldn't be necessary, but just in case)
    //put_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky_clr, 0b1, fast_write); //strobe bit

    // Run on selected DACs for a single bank
    // Register contains only rx_apply_*_run/done. OK to overwrite other bits to 0.
    // PSL bank_a
    uint32_t run_done_wr_val = (bank == bank_a) ? dac_sel << 4 :
                               dac_sel; // bits[0:7]: ae_run, ad_run, ae_done, ad_done, be_run, bd_run, be_done, bd_done
    put_ptr_field(gcr_addr, rx_apply_poff_ab_run_done_alias, run_done_wr_val, fast_write);

    // Wait for done
    uint32_t done_mask = run_done_wr_val >> 2;
    uint32_t apply_done;

    // PSL apply_done
    do
    {
        apply_done = done_mask & get_ptr_field(gcr_addr, rx_apply_poff_ab_run_done_alias);
    }
    while (apply_done != done_mask);

    // Check for DAC Accelerator errors
    /*if (get_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky)) {
      // Clear the error and set a fir
      put_ptr_field(gcr_addr, rx_dac_accel_rollover_sticky_clr, 0b1, fast_write); //strobe bit
      set_fir(fir_code_bad_lane_warning);
    }*/
} //apply_rx_dac_offset


// Common function for reading active PR by bank
// It relies on common vector definition using prmask_* defines
void read_active_pr (t_gcr_addr* gcr_addr, t_bank bank, int* pr_vals)
{
    uint32_t bank_pr[2];

    // Load ****both**** data and edge values on read. Assumes in same reg address in data + edge order
    // PSL bank_a
    if (bank == bank_a)
    {
        bank_pr[0] = get_ptr(gcr_addr, rx_a_pr_ns_data_addr,  rx_a_pr_ns_data_startbit, rx_a_pr_ns_edge_endbit);
        bank_pr[1] = get_ptr(gcr_addr, rx_a_pr_ew_data_addr,  rx_a_pr_ew_data_startbit, rx_a_pr_ew_edge_endbit);
    }
    else
    {
        bank_pr[0] = get_ptr(gcr_addr, rx_b_pr_ns_data_addr,  rx_b_pr_ns_data_startbit, rx_b_pr_ns_edge_endbit);
        bank_pr[1] = get_ptr(gcr_addr, rx_b_pr_ew_data_addr,  rx_b_pr_ew_data_startbit, rx_b_pr_ew_edge_endbit);
    }

    pr_vals[prDns_i] = prmask_Dns(bank_pr[0]);
    pr_vals[prEns_i] = prmask_Ens(bank_pr[0]);
    pr_vals[prDew_i] = prmask_Dew(bank_pr[1]);
    pr_vals[prEew_i] = prmask_Eew(bank_pr[1]);

    return;
}


/////////////////////////
// PCIe Only Functions //
/////////////////////////

PK_STATIC_ASSERT(saved_rx_ad_loff_lane_offset == saved_rx_bd_loff_lane_offset);
PK_STATIC_ASSERT(rx_ad_latch_dac_w111_addr == rx_ad_latch_dac_n000_addr + 31);
PK_STATIC_ASSERT(rx_bd_latch_dac_w111_addr == rx_bd_latch_dac_n000_addr + 31);
PK_STATIC_ASSERT(rx_ad_latch_dac_n000_width == 8);

// PCIe Only: Save to the mem_regs the current data latch dac values for all data latches in a bank
// Must set the gcr reg_id to rx_group before calling this
void save_rx_data_latch_dac_values(t_gcr_addr* gcr_addr, t_bank target_bank)
{
    int lane = get_gcr_addr_lane(gcr_addr);
    // PSL save_bank_a
    int dac_addr = (target_bank == bank_a) ? rx_ad_latch_dac_n000_alias_addr :
                   rx_bd_latch_dac_n000_alias_addr; // PCIe only, so can ignore Issue 296947 (Odyssey address adjust)
    int mem_addr = (target_bank == bank_a) ? saved_rx_ad_loff_addr : saved_rx_bd_loff_addr;

    // Error condition checking: Only support PCIe and lanes 0-3
    if (lane > 3)
    {
        set_debug_state(0x400A); // Illegal parameters for save_rx_data_latch_dac_values()
        // PSL set_fir_fatal_error
        set_fir(fir_code_fatal_error);
        return;
    }

    // Create u32 pointer to mem_regs target section
    // Byte access is simpler and less code, but word access saves ~1.5 us
    int mem_reg_index = mem_addr + (lane * saved_rx_ad_loff_lane_offset);
    uint32_t* mem_reg_ptr = (uint32_t*)(&mem_regs_u16[mem_reg_index]);

    // Read and store each data latch dac value
    // Use the rx_latch_dac_double_mode to read consecutive registers in a single read
    put_ptr_field(gcr_addr, rx_latch_dac_double_mode, 1, fast_write); // fast_write OK, only field in register
    int index;

    for (index = 0; index < 8; index++, dac_addr = dac_addr + 4)
    {
        uint32_t read10, read32;
        read10 = get_ptr(gcr_addr, dac_addr + 0, 0, 15);
        read32 = get_ptr(gcr_addr, dac_addr + 2, 0, 15);

        uint32_t reg_val;
        reg_val  = (read10 & 0x00FF) << 24;
        reg_val |= (read10 & 0xFF00) << 8;
        reg_val |= (read32 & 0x00FF) << 8;
        reg_val |= (read32 & 0xFF00) >> 8;

        mem_reg_ptr[index] = reg_val;
    }

    put_ptr_field(gcr_addr, rx_latch_dac_double_mode, 0, fast_write); // fast_write OK, only field in register
} //save_rx_data_latch_dac_values


PK_STATIC_ASSERT(rx_mini_pr_step_run_done_full_reg_width == 16);
PK_STATIC_ASSERT(rx_mini_pr_step_a_ns_data_run_startbit == 0);
PK_STATIC_ASSERT(rx_mini_pr_step_a_ew_data_run_startbit == 1);
PK_STATIC_ASSERT(rx_mini_pr_step_a_ns_edge_run_startbit == 2);
PK_STATIC_ASSERT(rx_mini_pr_step_a_ew_edge_run_startbit == 3);
PK_STATIC_ASSERT(rx_mini_pr_step_b_ns_data_run_startbit == 8);
PK_STATIC_ASSERT(rx_mini_pr_step_b_ew_data_run_startbit == 9);
PK_STATIC_ASSERT(rx_mini_pr_step_b_ns_edge_run_startbit == 10);
PK_STATIC_ASSERT(rx_mini_pr_step_b_ew_edge_run_startbit == 11);

// PCIe Only: Write the data latches in a bank with the saved value + the recorded path offset
// MUST set the gcr reg_id to rx_group before calling this
// ASSUMPTION: Not waiting for mini PR stepper to complete since expect it to be faster (~200ns) than subsequent code that would access the rx_data_dac_*_regs.
void restore_rx_data_latch_dac_values(t_gcr_addr* gcr_addr, t_bank target_bank)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // PSL restore_bank_a
    int dac_addr = (target_bank == bank_a) ? rx_ad_latch_dac_n000_alias_addr :
                   rx_bd_latch_dac_n000_alias_addr; // PCIe only, so can ignore Issue 296947 (Odyssey address adjust)
    int mem_addr = (target_bank == bank_a) ? saved_rx_ad_loff_addr : saved_rx_bd_loff_addr;

    // Error condition checking: Only support PCIe and lanes 0-3
    if (lane > 3)
    {
        set_debug_state(0x400B); // Illegal parameters for restore_rx_data_latch_dac_values()
        // PSL set_fir_fatal_error
        set_fir(fir_code_fatal_error);
        return;
    }

    // Read the saved path offset and convert to 2s complement
    // PSL path_offset_bank_a
    int path_offset = (target_bank == bank_a) ? mem_pl_field_get(poff_avg_a, lane) : mem_pl_field_get(poff_avg_b, lane);
    path_offset = TwosCompToInt(path_offset, poff_avg_a_width);

    // Create u32 pointer to mem_regs target section
    // Byte access is simpler and less code, but word access saves ~4 us
    int mem_reg_index = mem_addr + (lane * saved_rx_ad_loff_lane_offset);
    uint32_t* mem_reg_ptr = (uint32_t*)(&mem_regs_u16[mem_reg_index]);

    // Read each stored data latch dac value, apply the path offset, and write the result to the dac
    // Use the rx_latch_dac_double_mode to write consecutive registers in a single write
    put_ptr_field(gcr_addr, rx_latch_dac_double_mode, 1, fast_write); // fast_write OK, only field in register
    int index;

    for (index = 0; index < 8; index++, dac_addr = dac_addr + 4)
    {
        int dac_val0, dac_val1, wr_val;
        uint32_t reg_val = mem_reg_ptr[index];

        dac_val0 = IntToLatchDac( LatchDacToInt((reg_val & 0xff000000) >> 24) + path_offset );
        dac_val1 = IntToLatchDac( LatchDacToInt((reg_val & 0x00ff0000) >> 16) + path_offset );
        wr_val   = (dac_val1 << 8) | dac_val0;
        put_ptr_fast(gcr_addr, dac_addr + 0, 15, wr_val);

        dac_val0 = IntToLatchDac( LatchDacToInt((reg_val & 0x0000ff00) >> 8) + path_offset );
        dac_val1 = IntToLatchDac( LatchDacToInt((reg_val & 0x000000ff) >> 0) + path_offset );
        wr_val   = (dac_val1 << 8) | dac_val0;
        put_ptr_fast(gcr_addr, dac_addr + 2, 15, wr_val);
    }

    put_ptr_field(gcr_addr, rx_latch_dac_double_mode, 0, fast_write); // fast_write OK, only field in register

    // Reset DFE Fast
    // ASSUMPTION: Not waiting for mini PR stepper to complete since expect it to be faster (~200ns) than subsequent code that would access the rx_data_dac_*_regs.
    int clk_adj;
    int run_done_wr_val;

    // PSL dfe_bank_a
    if (target_bank == bank_a)
    {
        // Clear the mem_regs holding the coefficients and latch offset
        mem_pl_field_put(rx_dfe_coef_7_0_1_6_alias, lane, 0);
        mem_pl_field_put(rx_dfe_coef_5_2_3_4_alias, lane, 0);
        mem_pl_bit_clr(rx_loff_ad_n000_valid, lane);

        // Read current clock adjust (account for QPA by taking average of NS and EW)
        clk_adj = 16 - ((get_ptr_field(gcr_addr, rx_a_pr_ns_data) + get_ptr_field(gcr_addr, rx_a_pr_ew_data)) / 2);

        // Select the mini pr stepper run bits
        run_done_wr_val = (rx_mini_pr_step_a_ns_data_run_mask | rx_mini_pr_step_a_ew_data_run_mask);
    }
    else     //bank_b
    {
        // Read current clock adjust (account for QPA by taking average of NS and EW)
        clk_adj = 16 - ((get_ptr_field(gcr_addr, rx_b_pr_ns_data) + get_ptr_field(gcr_addr, rx_b_pr_ew_data)) / 2);

        // Select the mini pr stepper run bits
        run_done_wr_val = (rx_mini_pr_step_b_ns_data_run_mask | rx_mini_pr_step_b_ew_data_run_mask);
    }

    // Step the mini PR back to nominal (16)
    int clk_adj_regval = IntToTwosComp(clk_adj, rx_mini_pr_step_data_adj_width);
    put_ptr_field(gcr_addr, rx_mini_pr_step_data_adj, clk_adj_regval,
                  fast_write); // fast_write OK, rx_mini_pr_step_data/edge_adj are only fields in register
    put_ptr_field(gcr_addr, rx_mini_pr_step_run_done_full_reg, run_done_wr_val,
                  fast_write); // OK to write 0 to rest of register

    // Wait for PR stepper to complete
    // ASSUMPTION: Not waiting for mini PR stepper to complete since expect it to be faster (~200ns) than subsequent code that would access the rx_data_dac_*_regs.
    //do { step_done = get_ptr_field(gcr_addr, rx_mini_pr_step_a/b_done_alias); } while (step_done != 0b1100); // bits[0:1] = ns_data_done, ew_data_done
} //restore_rx_data_latch_dac_values


// Clear the RX DFE from both Banks latches and Mini PRs
// Must set the gcr reg_id to rx_group before calling this
// ASSUMPTION: Not waiting for DAC accelerator to complete since expect it to be faster (~800ns) than subsequent code that would access the rx_data_dac_*_regs
PK_STATIC_ASSERT(rx_mini_pr_step_a_done_alias_width == 4);
void clear_rx_dfe(t_gcr_addr* gcr_addr)
{
    // Read current clock adjust (account for QPA by taking average of NS and EW)
    int clk_adj_a = 16 - ((get_ptr_field(gcr_addr, rx_a_pr_ns_data) + get_ptr_field(gcr_addr, rx_a_pr_ew_data)) / 2);
    int clk_adj_b = 16 - ((get_ptr_field(gcr_addr, rx_b_pr_ns_data) + get_ptr_field(gcr_addr, rx_b_pr_ew_data)) / 2);

    // Bank A Step the mini PR back to nominal (16)
    int clk_adj_regval = IntToTwosComp(clk_adj_a, rx_mini_pr_step_data_adj_width);
    put_ptr_field(gcr_addr, rx_mini_pr_step_data_adj, clk_adj_regval,
                  fast_write); // fast_write OK, rx_mini_pr_step_data/edge_adj are only fields in register
    put_ptr_field(gcr_addr, rx_mini_pr_step_run_done_full_reg,
                  rx_mini_pr_step_a_ns_data_run_mask | rx_mini_pr_step_a_ew_data_run_mask,
                  fast_write); // OK to write 0 to rest of register

    // Poll for stepper done before setup and run of bank B
    int step_done;

    do
    {
        step_done = get_ptr_field(gcr_addr, rx_mini_pr_step_a_done_alias);
    }
    while (step_done != 0b1100);   // bits[0:1] = ns_data_done, ew_data_done

    // Bank B Step the mini PR back to nominal (16)
    // ASSUMPTION: Not waiting for Mini PR stepper to complete since expect it to be faster (~200ns) than subsequent code that would access the rx_data_dac_*_regs
    clk_adj_regval = IntToTwosComp(clk_adj_b, rx_mini_pr_step_data_adj_width);
    put_ptr_field(gcr_addr, rx_mini_pr_step_data_adj, clk_adj_regval,
                  fast_write); // fast_write OK, rx_mini_pr_step_data/edge_adj are only fields in register
    put_ptr_field(gcr_addr, rx_mini_pr_step_run_done_full_reg,
                  rx_mini_pr_step_b_ns_data_run_mask | rx_mini_pr_step_b_ew_data_run_mask,
                  fast_write); // OK to write 0 to rest of register

    // Read the DFE coefficients
    int lane = get_gcr_addr_lane(gcr_addr);
    int coef_7_0_val = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_7_0, lane), rx_dfe_coef_7_0_width);
    int coef_1_6_val = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_1_6, lane), rx_dfe_coef_1_6_width);
    int coef_5_2_val = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_5_2, lane), rx_dfe_coef_5_2_width);
    int coef_3_4_val = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_3_4, lane), rx_dfe_coef_3_4_width);

    // Invert the coefficients for the adjusment. Mask int32 to correct field width and shift into correct bit positions.
    int coef_7_0_coef_1_6_adj_regval = ( IntToTwosComp(-coef_7_0_val, rx_coef_7_0_adj_width) << rx_coef_7_0_adj_shift ) |
                                       ( IntToTwosComp(-coef_1_6_val, rx_coef_1_6_adj_width) << rx_coef_1_6_adj_shift );
    int coef_5_2_coef_3_4_adj_regval = ( IntToTwosComp(-coef_5_2_val, rx_coef_5_2_adj_width) << rx_coef_5_2_adj_shift ) |
                                       ( IntToTwosComp(-coef_3_4_val, rx_coef_3_4_adj_width) << rx_coef_3_4_adj_shift );

    // OK to fast_write (no other fields in the adjust registers)
    put_ptr_field(gcr_addr, rx_coef_7_0_coef_1_6_adj_full_reg, coef_7_0_coef_1_6_adj_regval, fast_write);
    put_ptr_field(gcr_addr, rx_coef_5_2_coef_3_4_adj_full_reg, coef_5_2_coef_3_4_adj_regval, fast_write);

    // Apply DFE coefficients to both banks
    put_ptr_field(gcr_addr, rx_apply_dfe_v2_ab_run_done_alias, 0b1010,
                  fast_write); // bits[0:3]: a_run, a_done, b_run, b_done

    // Clear the mem_regs holding the coefficients and latch offset
    mem_pl_field_put(rx_dfe_coef_7_0_1_6_alias, lane, 0);
    mem_pl_field_put(rx_dfe_coef_5_2_3_4_alias, lane, 0);
    mem_pl_bit_clr(rx_loff_ad_n000_valid, lane);

    // Wait for DAC accelerator to finish
    // ASSUMPTION: Not waiting for DAC accelerator to complete since expect it to be faster (~800ns) than subsequent code that would access the rx_data_dac_*_regs
    //do { apply_done = get_ptr_field(gcr, rx_apply_dfe_v2_ab_run_done_alias); } while (apply_done != 0b1111);
} //clear_rx_dfe


// Remove the configured Edge PR Offset from whichever banks have it applied
// Must set the gcr reg_id to rx_group before calling this
// ASSUMPTION: Not waiting for Mini PR stepper to complete since expect it to be faster (~400ns) than subsequent code that would access the rx_data_dac_*_regs
void remove_edge_pr_offset(t_gcr_addr* gcr_addr)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // Exit if a PR offset is not applied to either bank
    int pr_offset_applied_ab = mem_pl_field_get(ppe_pr_offset_applied_ab_alias, lane);

    if (pr_offset_applied_ab == 0)
    {
        return;
    }

    // Determine which banks to run the mini PR stepepr on and the run command
    int run_done_wr_val = 0;

    if (pr_offset_applied_ab & 0b10)
    {
        run_done_wr_val |= (rx_mini_pr_step_a_ns_edge_run_mask | rx_mini_pr_step_a_ew_edge_run_mask);
    }

    if (pr_offset_applied_ab & 0b01)
    {
        run_done_wr_val |= (rx_mini_pr_step_b_ns_edge_run_mask | rx_mini_pr_step_b_ew_edge_run_mask);
    }

    // Run the PR stepper to undo the edge PR offset
    int pr_offset_e = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_e_override), ppe_pr_offset_e_override_width);
    int step_adj_regval = IntToTwosComp(-pr_offset_e, rx_mini_pr_step_edge_adj_width);
    put_ptr_field(gcr_addr, rx_mini_pr_step_edge_adj, step_adj_regval,
                  fast_write); // fast_write OK, rx_mini_pr_step_data/edge_adj are only fields in register
    put_ptr_field(gcr_addr, rx_mini_pr_step_run_done_full_reg, run_done_wr_val,
                  fast_write); // OK to write 0 to rest of register

    // Clear pr_offset_applied (assuming the data offset has already been removed)
    mem_pl_field_put(ppe_pr_offset_applied_ab_alias, lane, 0);

    // Wait for PR stepper to complete
    // ASSUMPTION: Not waiting for Mini PR stepper to complete since expect it to be faster (~400ns) than subsequent code that would access the rx_data_dac_*_regs
    //do { step_done = get_ptr_field(gcr_addr, rx_mini_pr_step_a/b_done_alias); } while (step_done != 0b1100); // bits[0:1] = ns_data_done, ew_data_done
} //remove_edge_pr_offset


PK_STATIC_ASSERT(tx_dcc_i_q_tune_full_reg_alias_width == 16);
PK_STATIC_ASSERT(tx_dcc_i_tune_width == pcie_gen3_lane0_saved_tx_dcc_i_tune_width);
PK_STATIC_ASSERT(tx_dcc_q_tune_width == pcie_gen3_lane0_saved_tx_dcc_q_tune_width);
PK_STATIC_ASSERT(tx_dcc_i_tune_startbit < tx_dcc_q_tune_startbit);
PK_STATIC_ASSERT(pcie_gen3_lane0_saved_tx_dcc_i_tune_startbit < pcie_gen3_lane0_saved_tx_dcc_q_tune_startbit);
PK_STATIC_ASSERT(tx_dcc_iq_tune_width == 6);
PK_STATIC_ASSERT(tx_dcc_iq_tune_width == pcie_gen3_lane0_saved_tx_dcc_iq_tune_width);

// PCIe Only: Save to the mem_regs the TX DCC settings for the current PCIe GenX
// Must set the gcr reg_id to tx_group before calling this
void save_tx_dcc_tune_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // Error condition checking: Only support PCIe Gen3-5 and lanes 0-3
    if ( (lane > 3) || (cal_mode < C_PCIE_GEN3_CAL) || (cal_mode == C_AXO_CAL) )
    {
        set_debug_state(0xD01A); // Illegal parameters for save_tx_dcc_tune_values()
        // PSL set_fir_fatal_error
        set_fir(fir_code_fatal_error);
        return;
    }

    // Calculate the mem_reg offset where the settings will be stored
    int genx_index    = cal_mode - C_PCIE_GEN3_CAL;
    int mem_reg_index = saved_tx_dcc_addr + (lane * saved_tx_dcc_lane_offset) + (genx_index * saved_tx_dcc_genx_offset);

    // Read and store the TX DCC settings
    mem_regs_u16[mem_reg_index + 0] = get_ptr_field(gcr_addr, tx_dcc_i_q_tune_full_reg_alias);
    mem_regs_u16[mem_reg_index + 1] = get_ptr_field(gcr_addr, tx_dcc_iq_tune);
} //save_tx_dcc_tune_values


// PCIe Only: Write the TX DCC config with the saved values for the current PCIe GenX
// Must set the gcr reg_id to tx_group before calling this
void restore_tx_dcc_tune_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // Error condition checking: Only support PCIe and lanes 0-3
    if ( (lane > 3) || (cal_mode == C_AXO_CAL) )
    {
        set_debug_state(0xD01B); // Illegal parameters for restore_tx_dcc_tune_values()
        // PSL set_fir_fatal_error
        set_fir(fir_code_fatal_error);
        return;
    }

    // For Gen1 and Gen2, just write the default values to the tune controls
    if (cal_mode < C_PCIE_GEN3_CAL)
    {
        put_ptr_field(gcr_addr, tx_dcc_i_q_tune_full_reg_alias, 0xC0C0,   fast_write);
        put_ptr_field(gcr_addr, tx_dcc_iq_tune,                 0b010000, read_modify_write);
        return;
    }

    // Calculate the mem_reg offset where the settings are stored
    int genx_index    = cal_mode - C_PCIE_GEN3_CAL;
    int mem_reg_index = saved_tx_dcc_addr + (lane * saved_tx_dcc_lane_offset) + (genx_index * saved_tx_dcc_genx_offset);

    // Read the stored settings and write to the circuit
    put_ptr_field(gcr_addr, tx_dcc_i_q_tune_full_reg_alias, mem_regs_u16[mem_reg_index + 0], fast_write);
    put_ptr_field(gcr_addr, tx_dcc_iq_tune,                 mem_regs_u16[mem_reg_index + 1], read_modify_write);
} //restore_tx_dcc_tune_values


PK_STATIC_ASSERT(rx_a_ctle_peak1_peak2_alias_width == 8);
PK_STATIC_ASSERT(rx_b_ctle_peak1_peak2_alias_width == 8);
PK_STATIC_ASSERT(rx_a_ctle_peak1_startbit < rx_a_ctle_peak2_startbit);
PK_STATIC_ASSERT(rx_b_ctle_peak1_startbit < rx_b_ctle_peak2_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak1_startbit == rx_b_ctle_peak1_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak2_startbit == rx_b_ctle_peak2_startbit);
PK_STATIC_ASSERT(rx_a_ctle_peak1_width == pcie_lane0_gen1_preset_rx_ctle_peak1_width);
PK_STATIC_ASSERT(rx_a_ctle_peak2_width == pcie_lane0_gen1_preset_rx_ctle_peak2_width);
PK_STATIC_ASSERT(rx_a_ctle_peak1_peak2_alias_width == pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_width);
PK_STATIC_ASSERT(pcie_lane0_gen1_preset_rx_ctle_peak1_startbit < pcie_lane0_gen1_preset_rx_ctle_peak2_startbit);
PK_STATIC_ASSERT(rx_a_lte_gain_zero_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_b_lte_gain_zero_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_a_lte_gain_zero_alias_width == 6);
PK_STATIC_ASSERT(rx_b_lte_gain_zero_alias_width == 6);
PK_STATIC_ASSERT(rx_a_lte_gain_startbit < rx_a_lte_zero_startbit);
PK_STATIC_ASSERT(rx_b_lte_gain_startbit < rx_b_lte_zero_startbit);
PK_STATIC_ASSERT(rx_a_lte_gain_startbit == rx_b_lte_gain_startbit);
PK_STATIC_ASSERT(rx_a_lte_zero_startbit == rx_b_lte_zero_startbit);
PK_STATIC_ASSERT(rx_a_lte_gain_width == pcie_lane0_gen1_preset_rx_lte_gain_width);
PK_STATIC_ASSERT(rx_a_lte_zero_width == pcie_lane0_gen1_preset_rx_lte_zero_width);

// PCIe Only: Write the CTLE Peak1/Peak2 and LTE Gain/Zero registers with the presets for the current PCIe GenX
// Must set the gcr reg_id to rx_group before calling this
void preset_rx_peak_lte_values(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode)
{
    int lane = get_gcr_addr_lane(gcr_addr);

    // Error condition checking: Only support PCIe and lanes 0-3
    if ( (lane > 3) || (cal_mode == C_AXO_CAL) )
    {
        set_debug_state(0x6033); // Illegal parameters for preset_rx_peak_lte_values()
        // PSL set_fir_fatal_error
        set_fir(fir_code_fatal_error);
        return;
    }

    // Calculate the mem_reg offset where the settings are stored
    int genx_index    = cal_mode - C_PCIE_GEN1_CAL;
    int mem_reg_index = preset_rx_peak_lte_addr + (lane * preset_rx_peak_lte_lane_offset) +
                        (genx_index * preset_rx_peak_lte_genx_offset);

    // Read the mem_reg and parse
    int reg_val           = mem_regs_u16[mem_reg_index];
    int peak1_peak2_val   = (reg_val & preset_rx_ctle_peak1_peak2_mask) >> preset_rx_ctle_peak1_peak2_shift;
    int lte_gain_val      = (reg_val & preset_rx_ctle_lte_gain_mask) >> preset_rx_ctle_lte_gain_shift;
    int lte_zero_val      = (reg_val & preset_rx_ctle_lte_zero_mask) >> preset_rx_ctle_lte_zero_shift;
    int lte_gain_zero_val = (lte_gain_val << (rx_a_lte_gain_shift - rx_a_lte_zero_shift)) | lte_zero_val;

    // Write values to both banks
    put_ptr_field(gcr_addr, rx_a_lte_gain_zero_alias, lte_gain_zero_val, fast_write); // only field in register
    put_ptr_field(gcr_addr, rx_b_lte_gain_zero_alias, lte_gain_zero_val, fast_write); // only field in register

    put_ptr_field(gcr_addr, rx_a_ctle_peak1_peak2_alias, peak1_peak2_val, read_modify_write); // ctle_gain also in register
    put_ptr_field(gcr_addr, rx_b_ctle_peak1_peak2_alias, peak1_peak2_val, read_modify_write); // ctle_gain also in register
} //preset_rx_peak_lte_values

/**
 * @brief Converts a vector code with a MSB 1/2 strength bit to a decimal value
 * @param[in] i_vec Value
 * @param[in] i_width Width of Register
 * @retval decimal Value
 */
uint32_t tx_ffe_fromVecWithHalf(const uint32_t i_vec, const uint32_t i_width)
{
    uint32_t l_dec = 0;
    int8_t j;

    for ( j = i_width - 2; j >= 0; j-- )
    {
        l_dec += ((i_vec >> j) & 0x1) << 1; // add 2 for each bit set except msb
    }

    l_dec += i_vec >> (i_width - 1) & 0x1; // add 1 for msb set
    return l_dec;
} // tx_ffe_fromVecWithHalf

/**
 * @brief Converts a decimal value to a thermometer code
 * @param[in] i_dec Decimal Value
 * @retval    Thermometer Value
 */
uint32_t tx_ffe_toTherm(const uint32_t i_dec)
{
    return (( 0x1 <<  i_dec ) - 1 );
} // tx_ffe_toTherm

/**
 * @brief Converts a decimal value to a thermometer code with reverse bit significance
 * @param[in] i_dec Decimal Value; must be <= i_width; this is not checked
 * @param[in] i_width Decimal Value width of therm val
 * @retval    Thermometer Value
 */
uint32_t tx_ffe_toThermRev(const uint32_t i_dec, const uint32_t i_width)
{
    return ((( 0x1 <<  i_dec ) - 1) << (i_width - i_dec));
} // tx_ffe_toThermRev

/**
 * @brief Converts a decimal value to a thermometer code with a MSB 1/2 strength bit
 * @param[in] i_dec   Decimal Value; must be < 2*i_width; this is not checked
 * @param[in] i_width Width of Register
 * @retval Thermometer Value
 */
uint32_t tx_ffe_toThermWithHalf(const uint32_t i_dec, const uint32_t i_width)
{
    // If the LSB of the 2r equivalent is on, then we need to set the 2r bit (MSB)
    uint32_t halfOn = ( i_dec & 0x1 ) << ( i_width - 1 );

    // Shift the 2r equivalent to a 1r value and convert to a thermometer code.
    uint32_t x1Equivalent = tx_ffe_toTherm(i_dec >> 0x1);

    // combine 1r equivalent thermometer code + the 2r MSB value.
    return halfOn | x1Equivalent ;
} // tx_ffe_toThermWithHalf

/**
 * @brief Converts a decimal value to a thermometer code with a MSB 1/2 strength bit
 * @brief and the full-strength segments reversed
 * @brief i_dec < i_width*2
 * @param[in] i_dec   Decimal Value; must be < 2*i_width; this is not checked
 * @param[in] i_width Width of Register
 * @retval Thermometer Value
 */
uint32_t tx_ffe_toThermWithHalfRev(const uint32_t i_dec, const uint32_t i_width)
{
    // If the LSB of the 2r equivalent is on, then we need to set the 2r bit (MSB)
    uint32_t halfOn = ( i_dec & 0x1 ) << ( i_width - 1 );

    // Shift the 2r equivalent to a 1r value and convert to a thermometer code.
    uint32_t x1Equivalent = tx_ffe_toThermRev( i_dec >> 0x1, i_width - 1 );

    // combine 1r equivalent thermometer code + the 2r MSB value.
    return halfOn | x1Equivalent ;
} // tx_ffe_toThermWithHalfRev
