/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_txdetrx_bist.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : tx_txdetrx_bist.c
// *! TITLE       :
// *! DESCRIPTION : TX Signal Detect Circuit BIST.
// *!             :
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// gap23030700 |gap     | Issue 300655: change detrx controls from PG to PL for Pll
// mbs22082601 |mbs     | Updated with PSL comments
// gap22052300 |gap     | change set_debug to save a few bytes
// vbr22061500 |vbr     | Added returning of fail status for ext commands
// gap22042700 |gap     | change tx_rxdet_enable to tx_pcie_rxdet_en_dc
// mwh22032500 |mwh     | set more register to default value after test
// mwh22032200 |mwh     | update code based on input by Gary P
// mwh22032000 |mwh     | Initial Code
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"
#include "txbist_main.h"

#define DBG_LVL 3 // debug on each branch

//This are the controls for bist
//tx_bist_txdetrx_alias pl
//tx_bist_txdetrx_en    pl
//tx_bist_txdetrx_fail  pl
//tx_bist_txdetrx_done  pl

//==============================================//
//see the doc in workbook

// registesr               |type| Default   | test 1   | test 2
//-------------------------|----|-----------|----------|---------------|
//tx_tdr_dac_cntl_pl       |pl  | 00000000  | 11000000 | 11000000=192  |0  8 tx_cntl15_pl
//tx_tdr_clamp_disable     |pg  | 0         | 1        | 1             |8  1 tx_ctl_cntl4_pg
//tx_tdr_phase_sel         |pg  | 0         | 0        | 0             |9  1 tx_ctl_cntl4_pg
//tx_tdr_offset_ctrl       |pg  | 0         | 0        | 0             |10 1 tx_ctl_cntl4_pg
//tx_tdr_enable            |pl  | 0         | 0        | 0             |0  1 tx_cntl3_pl
//tx_pcie_rxdet_en_dc      |pl  | 0         | 1        | 0             |4  1 tx_cntl3_pl **
//tx_bist_hs_en            |pg  | 0         | 0        | 0             |8  1 tx_ctl_mode1_pg
//tx_bank_controls         |pl  | 111111    | 000000   | 000000        |0  6 tx_cntl14_pl
//tx_idle_mode_ovr_en      |pl  | 1         | 1        | 1             |3  1 tx_mode2_pl
//tx_idle_mode_ovr_val     |pl  | 0         | 0        | 0             |4  1 tx_mode2_pl
//tx_pcie_rxdet_mode_dc    |pl  | 0         | 1        | 1             |15 1 tx_cntl30_pl
//tx_pcie_idle_del_sel_1   |pl  | 0100      | 0000     | 0000          |0  4 tx_ppe_mode10_pg
//tx_pcie_loz_del_sel_1    |pl  | 0011      | 0000     | 0000          |4  4 tx_ppe_mode10_pg
//tx_detrx_n_comp          |pl  | read      | 1        | 0             |13 1 tx_stat1_pl
//tx_detrx_p_comp          |pl  | read      | 1        | 0             |14 1 tx_stat1_pl



//------------------------------------------------------------------------------
int tx_txdetrx_bist(t_gcr_addr* gcr_addr, int tx_bist_enable_ls, int tx_bist_enable_hs)
{
    //start tx_txdetrx_bist
    set_debug_state(0x0120); // DEBUG : Start of txdetrx bist

    int status = rc_no_error;
    int sticky_ls = 0;
    int sticky_hs = 0;
    int sticky_fail_test1 = 0;
    int tx_detrx_n_comp_int = 1;
    int tx_detrx_p_comp_int = 1;

    //Need this since tx_bist_en_alias in override set this high
    //can not be high for this test, we will reset to 1 at end
    // PSL tx_bist_enable_ls
    if (tx_bist_enable_ls == 1 )
    {
        sticky_ls = 1;
        put_ptr_field(gcr_addr, tx_bist_ls_en, 0b0, read_modify_write);
    }

    //Need this since tx_bist_en_alias in override set this high
    //can not be high for this test, we will reset to 1 at end
    // PSL tx_bist_enable_hs
    if (tx_bist_enable_hs == 1)
    {
        sticky_hs = 1;
        put_ptr_field(gcr_addr, tx_bist_hs_en, 0b0, read_modify_write);
    }

    //Set the value in the table
    //tx_tdr_dac_cntl = C0 bits tx_tdr_clamp_disable = 0 tx_tdr_phase_sel=0 tx_tdr_offset_ctr=0
    put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl, 0b11000000, read_modify_write);//pl
    put_ptr_field(gcr_addr, tx_tdr_cntl_alias, 0b100, read_modify_write);//pg

    // Set the value in the table
    put_ptr_field(gcr_addr, tx_pcie_rxdet_mode_dc, 0b1, read_modify_write); //pl

    //Set the value in the table
    put_ptr_field(gcr_addr, tx_pcie_idle_del_sel_1, 0b0000, read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_pcie_loz_del_sel_1, 0b0000, read_modify_write); //pg


    //set tx_pcie_rxdet_en_dc,0b1 TEST 1
    put_ptr_field(gcr_addr, tx_pcie_rxdet_en_dc, 0b1, read_modify_write); //pl


    io_spin_us(5);//5 micro-sec for setting of circuit

    //Reading value out of IOOCC_TX_RXDET_COMP.v (circuit) excepting 0
    tx_detrx_n_comp_int = get_ptr_field(gcr_addr, tx_detrx_n_comp); //pl
    tx_detrx_p_comp_int = get_ptr_field(gcr_addr, tx_detrx_p_comp); //pl

    //Record fail for test 1 expecting 1
    // PSL fail_test1
    if ((tx_detrx_n_comp_int == 0) || ( tx_detrx_p_comp_int == 0))
    {
        sticky_fail_test1 = 1;
    }

    set_debug_state(0x0121, DBG_LVL); // DEBUG : End test1  start test 2 of txdetrx bist


    //set tx_pcie_rxdet_en_dc,0b0 TEST 2
    put_ptr_field(gcr_addr, tx_pcie_rxdet_en_dc, 0b0, read_modify_write); //pl

    io_spin_us(5);//5 micro-sec for setting of circuit

    //Reading value out of IOOCC_TX_RXDET_COMP.v (circuit) excepting 0
    tx_detrx_n_comp_int = get_ptr_field(gcr_addr, tx_detrx_n_comp); //pl
    tx_detrx_p_comp_int = get_ptr_field(gcr_addr, tx_detrx_p_comp); //pl

    //Record fail for test 2 expecting 0 and updating logger for fail
    // PSL fail_test2_or_sticky_fail_test1
    if ((tx_detrx_n_comp_int == 1) || (tx_detrx_p_comp_int == 1) || (sticky_fail_test1 == 1))
    {
        txbist_main_set_bist_fail(gcr_addr);
        put_ptr_field(gcr_addr, tx_bist_txdetrx_fail, 0b1, read_modify_write); //pl
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TXDETRX_FAIL, gcr_addr, 0x0);
        status = error_code;
    }

    set_debug_state(0x0122, DBG_LVL); // DEBUG : End test2 and error setting of txdetrx bist

    //Reseting hs and ls to 1 if they were set to 1.
    // PSL sticky_ls_eq_1
    if (sticky_ls == 1)
    {
        put_ptr_field(gcr_addr, tx_bist_ls_en, 0b1, read_modify_write);
    }

    //Reseting hs and ls to 1 if they were set to 1.
    // PSL sticky_hs_eq_1
    if (sticky_hs == 1)
    {
        put_ptr_field(gcr_addr, tx_bist_hs_en, 0b1, read_modify_write);
    }

    //Need to be set back to 0 if not will interfer with functional operation of txrxdet
    put_ptr_field(gcr_addr, tx_bist_txdetrx_en, 0b0, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_pcie_idle_del_sel_1, 0b0100, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_pcie_loz_del_sel_1, 0b0011, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl, 0b00000000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_tdr_cntl_alias, 0b000 , read_modify_write); //pg

    //setting done,
    put_ptr_field(gcr_addr, tx_bist_txdetrx_done, 0b1, read_modify_write); //pl


    set_debug_state(0x0123); // DEBUG : Finished of txdetrx bist
    return status;
}//end tx_txdetrx_bist
