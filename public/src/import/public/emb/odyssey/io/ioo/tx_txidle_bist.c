/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_txidle_bist.c $ */
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
// *! FILENAME    : tx_txidle_bist.c
// *! TITLE       :
// *! DESCRIPTION : TX Signal Detect Circuit BIST Hiz and Loz.
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
// jjb23031500 |jjb     | Issue 301224: restore default idle_mode and idle_loz values at end of test
// gap23030700 |gap     | Issue 300655: change detrx controls from PG to PL for Pll
// mbs22082601 |mbs     | Updated with PSL comments
// gap22052300 |gap     | Change set_debug_state to save a few bytes
// vbr22061500 |vbr     | Added returning of fail status for ext commands
// mwh22032400 |mwh     | Initial Code
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
#include "tx_txidle_bist.h"

#define DBG_LVL 3 // debug on each branch


//This are the controls for bist
//tx_bist_txidle_fail  pl -- show fails
//tx_bist_txidle_done  pl -- show it done
//tx_tdr_enable        pl -- enable the tdr and allow readout
//tx_bist_txidle_pulse_mask pl -- mask pulse_t to high and force output

//==============================================//
//see the doc in workbook
//force one or 0 on pad to see that compartor flips -- stuck at fault.
//                                             IDLE_LOZ=1     |     IDLE_HIZ = 0
//                                       |pleg |nleg|pleg|nleg|pleg|nleg|pleg|nleg|
// registesr               |type| Default| T1  | T2 | T3 | T4 | T5 | T6 | T7 | T8 |
//-------------------------|----|--------|-----|----|----|----|----|----|----|----|
//tx_bist_txidle_pulse_mask|pl  | 0      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |
//tx_tdr_dac_cntl_pl       |pl  | 0x00   | 0x66|0x66|0x9A|0x9A|0x66|0x66|0x9A|0x9A|0  8
//tx_tdr_clamp_disable     |pg  | 0      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |8  1 tx_ctl_cntl4_pg
//tx_tdr_phase_sel         |pg  | 0      | 0   | 1  | 0  | 1  | 0  | 1  | 0  | 1  |9  1 tx_ctl_cntl4_pg
//tx_tdr_offset_ctrl       |pg  | 0      | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 0  |10 1 tx_ctl_cntl4_pg
//tx_tdr_enable            |pl  | 0      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |0  1 tx_cntl3_pl
//tx_rxdet_enable          |pl  | 0      | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 0  |4  1 tx_cntl3_pl **
//tx_bist_hs_en            |pg  | 0      | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 0  |8  1 tx_ctl_mode1_pg
//tx_bank_controls         |pl  | 0x3F   | 0x00|0x00|0x00|0x00|0x00|0x00|0x00|0x00|0  6 tx_cntl14_pl
//tx_pcie_rxdet_mode_dc    |pl  | 0      | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 0  |15 1 tx_cntl30_pl
//tx_idle_mode_ovr_en      |pl  | 1      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |3  1 tx_mode2_pl
//tx_idle_mode_ovr_val     |pl  | 0      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |4  1 tx_mode2_pl idle = 1
//tx_idle_loz_ovr_en       |pl  | 0      | 1   | 1  | 1  | 1  | 1  | 1  | 1  | 1  |5  1 tx_mode2_pl
//tx_idle_loz_ovr_val      |pl  | 0      | 1   | 1  | 1  | 1  | 0  | 0  | 0  | 0  |6  1 tx_mode2_pl loz = 1 than 0
//tx_pcie_idle_del_sel_pl  |pl  | 0x4    | n/a | n/a|n/a |n/a |n/a |n/a |n/a |n/a |0  4 tx_cntl23_pl
//tx_pcie_loz_del_sel_pl   |pl  | 0x3    | n/a | n/a|n/a |n/a |n/a |n/a |n/a |n/a |4  4 tx_cntl23_pl
//tx_tdr_capt_val          |pl  | read   |  1  | 1  | 0  | 0  | 1  | 1  | 0  | 0  |


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// idle test function
//------------------------------------------------------------------------------

uint16_t tx_idle_tests(t_gcr_addr* gcr_addr, uint16_t tx_tdr_dac_cntl_in, uint16_t tx_tdr_cntl_alias_in,
                       uint16_t  expect, uint16_t tx_idle_ovr_alias, uint16_t fail_mask)
{
    //start tx_idle_tests
    set_debug_state(0x0125, DBG_LVL); // DEBUG : Start of idle test
    uint16_t fail = 0;
    uint16_t tx_tdr_capt_val_int = 0;

    put_ptr_field(gcr_addr, tx_idle_ovr_alias, tx_idle_ovr_alias, read_modify_write); //pl

    //write cntl based on chart
    put_ptr_field(gcr_addr, tx_tdr_cntl_alias, tx_tdr_cntl_alias_in , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl, tx_tdr_dac_cntl_in , read_modify_write); //pg

    //setting time for circuit
    io_spin(5);

    //read compare value out circuit
    tx_tdr_capt_val_int = get_ptr_field(gcr_addr, tx_tdr_capt_val); //pl

    //Record fail
    // PSL set_fail
    if (tx_tdr_capt_val_int != expect )
    {
        fail = fail_mask ;
    }

    set_tx_dcc_debug_tx_idle_bist(0x0130, tx_tdr_cntl_alias_in );
    set_tx_dcc_debug_tx_idle_bist(0x0131, tx_idle_ovr_alias);
    set_tx_dcc_debug_tx_idle_bist(0x0132, expect);
    set_tx_dcc_debug_tx_idle_bist(0x0133, tx_tdr_capt_val_int );
    set_tx_dcc_debug_tx_idle_bist(0x0134, fail);
    set_tx_dcc_debug_tx_idle_bist(0x0135, tx_tdr_cntl_in );

    set_debug_state(0x0126, DBG_LVL); // DEBUG : end of idle test

    return fail;
}//end tx_idle_tests


int tx_txidle_bist(t_gcr_addr* gcr_addr, int tx_bist_enable_ls, int tx_bist_enable_hs)
{
    //start tx_txidle_bist
    set_debug_state(0x0124); // DEBUG : Start of idle bist

    //int settings
    uint16_t over_all_fail = 0;
    int sticky_ls = 0;
    int sticky_hs = 0;


    //Need this since tx_bist_en_alias in override set this high
    //can not be high for this test, we will reset to 1 at end
    // PSL enable_ls_eq_1
    if (tx_bist_enable_ls == 1 )
    {
        sticky_ls = 1;
        put_ptr_field(gcr_addr, tx_bist_ls_en, 0b0, read_modify_write);
    }

    //Need this since tx_bist_en_alias in override set this high
    //can not be high for this test, we will reset to 1 at end
    // PSL enable_hs_eq_1
    if (tx_bist_enable_hs == 1)
    {
        sticky_hs = 1;
        put_ptr_field(gcr_addr, tx_bist_hs_en, 0b0, read_modify_write);
    }

    //Setting based on chart
    put_ptr_field(gcr_addr, tx_tdr_enable , 0b1, read_modify_write); //pl

    //Setting based on chart
    put_ptr_field(gcr_addr, tx_rxdet_enable , 0b0, read_modify_write); //pl

    //Setting based on chart
    put_ptr_field(gcr_addr, tx_pcie_rxdet_mode_dc, 0b0, read_modify_write); //pl


    //Test 1 LOZ=1 expect 0 on read, p pad selected
    set_debug_state(0x0141, DBG_LVL); // test1  102_p_idle_1111_exp1
    over_all_fail = tx_idle_tests(gcr_addr, tdr_cntl_low, tdr_cntl_102_clp_padp_offset , expect_1 ,
                                  tx_idle_mode_ovr_alias_1111, t1_p_pad_fail_exp_1);

    //Test 2 LOZ=1 expect 0 on read, n pad selected
    set_debug_state(0x0142, DBG_LVL); // test2  102_n_idle_1111_exp1
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_low, tdr_cntl_102_clp_padn_offset , expect_1 ,
                    tx_idle_mode_ovr_alias_1111, t2_n_pad_fail_exp_1);

    //Test 3 LOZ=1 expect 1 on read, p pad selected
    set_debug_state(0x0143, DBG_LVL); // test3  154_p_idle_1111_exp0
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_high, tdr_cntl_154_clp_padp_offset , expect_0 ,
                    tx_idle_mode_ovr_alias_1111, t3_p_pad_fail_exp_0);

    //Test 4 LOZ=1 expect 1 on read, n pad selected
    set_debug_state(0x0144, DBG_LVL); // test4  154_n_idle_1111_exp0
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_high, tdr_cntl_154_clp_padn_offset , expect_0 ,
                    tx_idle_mode_ovr_alias_1111, t4_n_pad_fail_exp_0);

    //Test 5 HIZ=1 expect 1 on read, p pad selected
    set_debug_state(0x0145, DBG_LVL); // test5  102_p_idle_1110_exp1
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_low, tdr_cntl_102_clp_padp_offset , expect_1 ,
                    tx_idle_mode_ovr_alias_1110, t5_p_pad_fail_exp_1);

    //Test 6 HIZ=1 expect 1 on read, n pad selected
    set_debug_state(0x0146, DBG_LVL); // test6  102_n_idle_1110_exp1
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_low, tdr_cntl_102_clp_padn_offset , expect_1 ,
                    tx_idle_mode_ovr_alias_1110, t6_n_pad_fail_exp_1);

    //Test 7 HIZ=1 expect 1 on read, p pad selected
    set_debug_state(0x0147, DBG_LVL); // test7  154_p_idle_1110_exp0
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_high, tdr_cntl_154_clp_padp_offset , expect_0 ,
                    tx_idle_mode_ovr_alias_1110, t7_p_pad_fail_exp_0);

    //Test 8 HIZ=1 expect 1 on read, n pad selected
    set_debug_state(0x0148, DBG_LVL); // test8  154_n_idle_1110_exp0
    over_all_fail = over_all_fail | tx_idle_tests(gcr_addr, tdr_cntl_high, tdr_cntl_154_clp_padn_offset , expect_0 ,
                    tx_idle_mode_ovr_alias_1110, t8_n_pad_fail_exp_0);

    //Record fail  updating logger for fail
    // PSL over_all_fail
    if (over_all_fail)
    {
        txbist_main_set_bist_fail(gcr_addr);
        put_ptr_field(gcr_addr, tx_bist_txidle_fail, over_all_fail, read_modify_write); //pl
        // PSL set_fir_bad_lane_warning_and_dft_fir
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TXIDLE_FAIL, gcr_addr, 0x0);
    }

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
    put_ptr_field(gcr_addr, tx_tdr_enable, 0b0, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_bist_txidle_pulse_mask, 0b0, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_tdr_cntl_alias, 0b000 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl, 0b00000000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_idle_ovr_alias, 0b1010, read_modify_write);//pl

    //setting done,
    put_ptr_field(gcr_addr, tx_bist_txidle_done, 0b1, read_modify_write); //pl

    set_debug_state(0x0127); // DEBUG : Finished of idle bist
    return (over_all_fail ? error_code : pass_code);
}//end tx_idle_bist
