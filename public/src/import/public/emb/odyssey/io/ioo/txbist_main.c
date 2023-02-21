/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/txbist_main.c $   */
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
// *! FILENAME    : txbist_main.c
// *! TITLE       :
// *! DESCRIPTION : Run tx duty cycle correction
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// jjb22121500 |jjb     | needed to change tx_pattern_sel to non-zero value that is different than desired value to enable fastx2 clocks
// jjb22110700 |jjb     | moved ioo clk_sel, bus_width_sel, and gear_ratio set up code to reduce code space
// jjb22101300 |jjb     | Updated hs and ls tx bist support for pcie mode
// jjb22101100 |jjb     | TX HS BIST updated to run all 4 patterns
// mbs22082601 |mbs     | Updated with PSL comments
// jjb22070700 |jjb     | tx_pattern_enable must be set high prior to tx_pattern_sel for gear ratio sync to operate properly
// gap22060100 |gap     | Add 1us wait between changing select and reading result EWM256180
// vbr22061500 |vbr     | Added returning of fail status for ext commands
// bja21101800 |bja     | Unify DCC tune handling for 5nm and 7nm
// bja21060800 |bja     | Use modified dcc gray code functions
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// gap20082500 |gap     | HW542315 correct repeating pattern when in half-width mode
// gap20060900 |gap     | HW524816 made debug before set_fir unique
// gap20060901 |gap     | Removed redundant set_fir calls; removed extra set_debug calls
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// cws20011400 |cws     | Added Debug Logs
// gap19052100 |gap     | Removed high speed post test
// mwh19043000 |mwh     | Add set_fir(fir_code_dft_error) to fail if's
// bja19040400 |bja     | Set DFT FIR bit on fail
// gap19031200 |gap     | Changed i_tune, q_tune and iq_tune to customized gray code
// gap18112700 |gap     | Update field names, change min and max from defines to mem_reg's
// gap18101100 |gap     | Created
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "txbist_main.h"
#include "tx_txdetrx_bist.h"
#include "tx_txidle_bist.h"

#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"
#include "tx_dcc_tune_constants.h"

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run steps depending on enables
//   gcr_addr is set to tx_group before this is called
////////////////////////////////////////////////////////////////////////////////////
int txbist_main (t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0x0100); // txbist_main start
    int status = rc_no_error;

    // Cal Step: TX Duty Cycle Correction checking
    int tx_bist_enable_dcc = get_ptr_field(gcr_addr_i, tx_bist_dcc_en);
    int tx_bist_enable_ls = get_ptr_field(gcr_addr_i, tx_bist_ls_en);
    int tx_bist_enable_hs = get_ptr_field(gcr_addr_i, tx_bist_hs_en);

    int l_tx_bist_txdetrx_en = get_ptr_field(gcr_addr_i, tx_bist_txdetrx_en);//pl
    int l_tx_bist_txidle_pulse_mask = get_ptr_field(gcr_addr_i, tx_bist_txidle_pulse_mask); //pl

    int l_pcie_mode = fw_field_get(fw_pcie_mode);
    int l_rate = get_ptr_field(gcr_addr_i, pipe_state_rate);// Gen1=0...Gen5=4

#ifdef IOO

    // PSL pcie_mode
    if (l_pcie_mode == 1)   // Update ppe_data_rate to reflect PCIe pipe_state_rate
    {
        uint32_t l_tx_pcie_clk_sel = get_ptr_field(gcr_addr_i, tx_pcie_clk_sel); // 1 hot; bits 27-31 --> gen5-gen1

        // PSL pcie_gen1or2
        if (l_tx_pcie_clk_sel < 3)   // GEN1/2
        {
            put_ptr_field(gcr_addr_i, tx_pattern_bus_width_sel, 0b1 , read_modify_write);  // 40 bit mode
        }
        else     // GEN3/4/5
        {
            put_ptr_field(gcr_addr_i, tx_pattern_bus_width_sel, 0b0 , read_modify_write);  // 32 bit mode
        }

        uint32_t l_tx_pattern_gear_ratio = 15 >> (31 - __builtin_clz(l_tx_pcie_clk_sel))
                                           ; // uint32 0x10, 0x08, 0x04, 0x02, 0x01--> 0, 1, 3, 7, 15
        put_ptr_field(gcr_addr_i, tx_pattern_gear_ratio, l_tx_pattern_gear_ratio , read_modify_write);
    }

#endif

    // PSL txdetrx_and_pcie_and_rate0
    if ((l_tx_bist_txdetrx_en == 1) && (l_pcie_mode == 1) && (l_rate == 0))
    {
        status |= tx_txdetrx_bist(gcr_addr_i, tx_bist_enable_ls, tx_bist_enable_hs);
    }

    // PSL txidlepulse_and_pcie_and_rate0
    if ((l_tx_bist_txidle_pulse_mask == 1) && (l_pcie_mode == 1) && (l_rate == 0))
    {
        status |= tx_txidle_bist(gcr_addr_i, tx_bist_enable_ls, tx_bist_enable_hs);
    }


    // PSL enable_dcc
    if (tx_bist_enable_dcc)
    {
        status |= txbist_main_dcc(gcr_addr_i);
    }

    // Cal Step: TX BIST low speed
    // PSL enable_ls
    if (tx_bist_enable_ls)
    {
        status |= txbist_main_ls(gcr_addr_i);
    }

    // Cal Step: TX BIST high speed
    // PSL enable_hs
    if (tx_bist_enable_hs)
    {
        status |= txbist_main_hs(gcr_addr_i);
    }

    set_debug_state(0x01FF); // txbist_main end
    return status;
} // txbist_main

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run Duty cycle correction checking
////////////////////////////////////////////////////////////////////////////////////
int txbist_main_dcc(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0x0110); // txbist_main_dcc start
    int status = rc_no_error;

    int16_t bist_dcc_i_min_l  = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_i_min),  tx_bist_dcc_i_min_width);
    int16_t bist_dcc_i_max_l  = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_i_max),  tx_bist_dcc_i_max_width);
    int16_t bist_dcc_q_min_l  = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_q_min),  tx_bist_dcc_q_min_width);
    int16_t bist_dcc_q_max_l  = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_q_max),  tx_bist_dcc_q_max_width);
    int16_t bist_dcc_iq_min_l = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_iq_min), tx_bist_dcc_iq_min_width);
    int16_t bist_dcc_iq_max_l = TwosCompToInt(mem_pg_field_get(tx_bist_dcc_iq_max), tx_bist_dcc_iq_max_width);

    SET_TX_TUNE_CONSTANTS
    int32_t dcc_i_tune_l  = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_i_tune),  tx_dcc_i_tune_width_const,
                                            tx_dcc_i_tune_offset_const)  ;
    int32_t dcc_q_tune_l  = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_q_tune),  tx_dcc_q_tune_width_const,
                                            tx_dcc_q_tune_offset_const)  ;
    int32_t dcc_iq_tune_l = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_iq_tune), tx_dcc_iq_tune_width_const,
                                            tx_dcc_iq_tune_offset_const)   ;

    // PSL itune_lt_imin
    if(dcc_i_tune_l < bist_dcc_i_min_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0111); // txbist_main_dcc fail i low
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_I_FAIL, gcr_addr_i, dcc_i_tune_l);
        status = error_code;
    }

    // PSL itune_gt_imax
    if(dcc_i_tune_l > bist_dcc_i_max_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0112); // txbist_main_dcc fail i high
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_I_FAIL, gcr_addr_i, dcc_i_tune_l);
        status = error_code;
    }

    // PSL qtune_lt_imin
    if(dcc_q_tune_l < bist_dcc_q_min_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0113); // txbist_main_dcc fail q low
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_Q_FAIL, gcr_addr_i, dcc_q_tune_l);
        status = error_code;
    }

    // PSL qtune_gt_imax
    if(dcc_q_tune_l > bist_dcc_q_max_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0114); // txbist_main_dcc fail q high
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_Q_FAIL, gcr_addr_i, dcc_q_tune_l);
        status = error_code;
    }

    // PSL iqtune_lt_iqmin
    if(dcc_iq_tune_l < bist_dcc_iq_min_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0115); // txbist_main_dcc fail iq low
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_IQ_FAIL, gcr_addr_i, dcc_iq_tune_l);
        status = error_code;
    }

    // PSL iqtune_gt_iqmax
    if(dcc_iq_tune_l > bist_dcc_iq_max_l)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_dcc_fail, 0b1, read_modify_write);
        set_debug_state(0x0116); // txbist_main_dcc fail iq high
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_DCC_IQ_FAIL, gcr_addr_i, dcc_iq_tune_l);
        status = error_code;
    }

    return status;
} //txbist_main_dcc

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run low speed bist
////////////////////////////////////////////////////////////////////////////////////
int txbist_main_ls(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0x0130); // txbist_main_ls start
    int status = rc_no_error;

    put_ptr_field(gcr_addr_i, tx_pattern_enable,   0b1,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_sel,      0b001,
                  read_modify_write); // needed to induce change in tx_pattern_sel to start clocks
    put_ptr_field(gcr_addr_i, tx_pattern_sel,      0b010,  read_modify_write);
    put_ptr_field(gcr_addr_i, tx_bist_prbs_enable, 0b1,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_bist_prbs_clear,  0b1,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_bist_prbs_clear,  0b0,    read_modify_write);

    // PSL prbs_stat
    if(get_ptr_field(gcr_addr_i, tx_bist_prbs_stat_alias) != 1)
    {
        set_debug_state(0x0131); // txbist_main_ls fail
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_ls_fail, 0b1, read_modify_write);
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_LS_FAIL, gcr_addr_i, 0x0);
        status = error_code;
    }

    put_ptr_field(gcr_addr_i, tx_bist_prbs_enable, 0b0,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_enable,   0b0,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_sel,      0b000,  read_modify_write);

    return status;
} //txbist_main_ls

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run high speed bist
////////////////////////////////////////////////////////////////////////////////////
int txbist_main_hs(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0x0150); // txbist_main_hs start
    int status = rc_no_error;

    put_ptr_field(gcr_addr_i, tx_tdr_enable,      0b0,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_bist_hs_cust_en, 0b1,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_enable,  0b1,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_sel,     0b010,
                  read_modify_write); // needed to induce change in tx_pattern_sel to start clocks
    put_ptr_field(gcr_addr_i, tx_pattern_sel,     0b001,  read_modify_write);

    status |= txbist_main_hs_pat(gcr_addr_i, 0b0011);
    status |= txbist_main_hs_pat(gcr_addr_i, 0b1100);
    status |= txbist_main_hs_pat(gcr_addr_i, 0b0110);
    status |= txbist_main_hs_pat(gcr_addr_i, 0b1001);
    // two failing cases for testing
//   txbist_main_hs_pat(gcr_addr_i, 0b1110);
//   txbist_main_hs_pat(gcr_addr_i, 0b0001);

    put_ptr_field(gcr_addr_i, tx_pattern_sel,     0b000,  read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_enable,  0b0,    read_modify_write);
    put_ptr_field(gcr_addr_i, tx_bist_hs_cust_en, 0b0,    read_modify_write);

    return status;
} //txbist_main_hs

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run high speed bist for particular clock pattern
////////////////////////////////////////////////////////////////////////////////////
int txbist_main_hs_pat(t_gcr_addr* gcr_addr_i, uint8_t clk_pattern_i)
{
    int status = rc_no_error;

    tx_write_4_bit_pat(gcr_addr_i, clk_pattern_i);

    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b000); // Main N
    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b001); // Main P
    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b100); // Pre1 N
    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b101); // Pre1 P
    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b110); // Pre2 N
    status |= txbist_main_hs_pat_sel(gcr_addr_i, clk_pattern_i, 0b111); // Pre2 P

    return status;
} //txbist_main_hs_pat

////////////////////////////////////////////////////////////////////////////////////
// TXBIST
// Run high speed bist for particular hs select; clock setting is passed to log error, if any
////////////////////////////////////////////////////////////////////////////////////
int txbist_main_hs_pat_sel(t_gcr_addr* gcr_addr_i, uint8_t clk_pattern_i, uint8_t hs_sel_i)
{
    set_debug_state(0x0152, 3); // txbist_main_hs_pat_sel  start
    int status = rc_no_error;

    uint16_t dac_thresh_max_l = mem_pg_field_get(tx_bist_hs_dac_thresh_max);
    uint16_t dac_thresh_min_l = mem_pg_field_get(tx_bist_hs_dac_thresh_min);

    put_ptr_field(gcr_addr_i, tx_bist_hs_cust_sel,     hs_sel_i,    read_modify_write);
    io_wait_us(get_gcr_addr_thread(gcr_addr_i), 1); // wait at least 700nS from sel to compare

    put_ptr_field(gcr_addr_i, tx_tdr_dac_cntl,    dac_thresh_max_l,  read_modify_write);

    // PSL tdr_capt_ne_0
    if(get_ptr_field(gcr_addr_i, tx_tdr_capt_val) != 0)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_hs_fail, 0b1, read_modify_write);
        set_debug_state(0x0153); // txbist_main_hs_pat_sel fail high
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_HS_FAIL, gcr_addr_i, clk_pattern_i);
        status = error_code;
    }

    put_ptr_field(gcr_addr_i, tx_tdr_dac_cntl,    dac_thresh_min_l,  read_modify_write);

    // PSL tdr_capt_ne_1
    if(get_ptr_field(gcr_addr_i, tx_tdr_capt_val) != 1)
    {
        txbist_main_set_bist_fail(gcr_addr_i);
        put_ptr_field(gcr_addr_i, tx_bist_hs_fail, 0b1, read_modify_write);
        set_debug_state(0x0154); // txbist_main_hs_pat_sel fail low
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TX_HS_FAIL, gcr_addr_i, clk_pattern_i);
        status = error_code;
    }

    return status;
} //txbist_main_hs_pat_sel

void txbist_main_set_bist_fail(t_gcr_addr* gcr_addr_i)
{
    uint32_t new_lane_bad;
    uint8_t lane = get_gcr_addr_lane(gcr_addr_i);

    // PSL lane_lt_16
    if (lane < 16)
    {
        new_lane_bad = mem_pg_field_get(tx_bist_fail_0_15) | (0b1 << (15 - lane));
        mem_pg_field_put(tx_bist_fail_0_15, new_lane_bad);
    }
    else
    {
        new_lane_bad = mem_pg_field_get(tx_bist_fail_16_23) | (0b1 << (23 - lane));
        mem_pg_field_put(tx_bist_fail_16_23, new_lane_bad);
    }
} // txbist_main_set_bist_fail
