/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ppe_mem_reg_const_pkg_ioo.h $ */
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
//- *!***************************************************************************
//- *! FILENAME    : ppe_reg_const_pkg_ioo.h
//- *! TITLE       : PPE Register Constants Package Header
//- *! DESCRIPTION : Constants containing register info for use by PPE
//- *!
//- *!
//- *! OWNER NAME  : John Rell III       Email: jgrell@us.ibm.com
//- *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
//- *!
//- *!***************************************************************************

#ifndef _PPE_MEM_REG_CONST_IOO_PKG_H_
#define _PPE_MEM_REG_CONST_IOO_PKG_H_



#define amp_setting_ovr_enb_addr                   0b110100000
#define amp_setting_ovr_enb_startbit               10
#define amp_setting_ovr_enb_width                  1
#define amp_setting_ovr_enb_endbit                 10
#define amp_setting_ovr_enb_shift                  5
#define amp_setting_ovr_enb_mask                   0x20


#define bist_in_hold_loop_addr                     0b111001011
#define bist_in_hold_loop_startbit                 1
#define bist_in_hold_loop_width                    1
#define bist_in_hold_loop_endbit                   1
#define bist_in_hold_loop_shift                    14
#define bist_in_hold_loop_mask                     0x4000


#define bist_in_progress_addr                      0b111001011
#define bist_in_progress_startbit                  0
#define bist_in_progress_width                     1
#define bist_in_progress_endbit                    0
#define bist_in_progress_shift                     15
#define bist_in_progress_mask                      0x8000


#define bist_internal_error_addr                   0b111001011
#define bist_internal_error_startbit               6
#define bist_internal_error_width                  1
#define bist_internal_error_endbit                 6
#define bist_internal_error_shift                  9
#define bist_internal_error_mask                   0x200


#define bist_other_fail_addr                       0b111001011
#define bist_other_fail_startbit                   4
#define bist_other_fail_width                      1
#define bist_other_fail_endbit                     4
#define bist_other_fail_shift                      11
#define bist_other_fail_mask                       0x800


#define bist_overall_pass_addr                     0b111001011
#define bist_overall_pass_startbit                 5
#define bist_overall_pass_width                    1
#define bist_overall_pass_endbit                   5
#define bist_overall_pass_shift                    10
#define bist_overall_pass_mask                     0x400


#define bist_rx_fail_addr                          0b111001011
#define bist_rx_fail_startbit                      2
#define bist_rx_fail_width                         1
#define bist_rx_fail_endbit                        2
#define bist_rx_fail_shift                         13
#define bist_rx_fail_mask                          0x2000


#define bist_spare_0_addr                          0b111001011
#define bist_spare_0_startbit                      7
#define bist_spare_0_width                         1
#define bist_spare_0_endbit                        7
#define bist_spare_0_shift                         8
#define bist_spare_0_mask                          0x100


#define bist_spare_1_addr                          0b111001011
#define bist_spare_1_startbit                      8
#define bist_spare_1_width                         1
#define bist_spare_1_endbit                        8
#define bist_spare_1_shift                         7
#define bist_spare_1_mask                          0x80


#define bist_tx_fail_addr                          0b111001011
#define bist_tx_fail_startbit                      3
#define bist_tx_fail_width                         1
#define bist_tx_fail_endbit                        3
#define bist_tx_fail_shift                         12
#define bist_tx_fail_mask                          0x1000


#define jump_table_used_addr                       0b000001011
#define jump_table_used_startbit                   4
#define jump_table_used_width                      1
#define jump_table_used_endbit                     4
#define jump_table_used_shift                      11
#define jump_table_used_mask                       0x800


#define loff_setting_ovr_enb_addr                  0b110100000
#define loff_setting_ovr_enb_startbit              9
#define loff_setting_ovr_enb_width                 1
#define loff_setting_ovr_enb_endbit                9
#define loff_setting_ovr_enb_shift                 6
#define loff_setting_ovr_enb_mask                  0x40


#define pcie_gen3_lane0_saved_tx_dcc_i_tune_addr   0b011110000
#define pcie_gen3_lane0_saved_tx_dcc_i_tune_startbit   0
#define pcie_gen3_lane0_saved_tx_dcc_i_tune_width   8
#define pcie_gen3_lane0_saved_tx_dcc_i_tune_endbit   7
#define pcie_gen3_lane0_saved_tx_dcc_i_tune_shift   8
#define pcie_gen3_lane0_saved_tx_dcc_i_tune_mask   0xff00


#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_addr   0b011110001
#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_startbit   10
#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_width   6
#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_endbit   15
#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_shift   0
#define pcie_gen3_lane0_saved_tx_dcc_iq_tune_mask   0x3f


#define pcie_gen3_lane0_saved_tx_dcc_q_tune_addr   0b011110000
#define pcie_gen3_lane0_saved_tx_dcc_q_tune_startbit   8
#define pcie_gen3_lane0_saved_tx_dcc_q_tune_width   8
#define pcie_gen3_lane0_saved_tx_dcc_q_tune_endbit   15
#define pcie_gen3_lane0_saved_tx_dcc_q_tune_shift   0
#define pcie_gen3_lane0_saved_tx_dcc_q_tune_mask   0xff


#define pcie_lane0_gen1_preset_rx_ctle_peak1_addr   0b100011000
#define pcie_lane0_gen1_preset_rx_ctle_peak1_startbit   0
#define pcie_lane0_gen1_preset_rx_ctle_peak1_width   4
#define pcie_lane0_gen1_preset_rx_ctle_peak1_endbit   3
#define pcie_lane0_gen1_preset_rx_ctle_peak1_shift   12
#define pcie_lane0_gen1_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_addr   0b100011000
#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane0_gen1_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane0_gen1_preset_rx_ctle_peak2_addr   0b100011000
#define pcie_lane0_gen1_preset_rx_ctle_peak2_startbit   4
#define pcie_lane0_gen1_preset_rx_ctle_peak2_width   4
#define pcie_lane0_gen1_preset_rx_ctle_peak2_endbit   7
#define pcie_lane0_gen1_preset_rx_ctle_peak2_shift   8
#define pcie_lane0_gen1_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane0_gen1_preset_rx_lte_gain_addr    0b100011000
#define pcie_lane0_gen1_preset_rx_lte_gain_startbit   9
#define pcie_lane0_gen1_preset_rx_lte_gain_width   3
#define pcie_lane0_gen1_preset_rx_lte_gain_endbit   11
#define pcie_lane0_gen1_preset_rx_lte_gain_shift   4
#define pcie_lane0_gen1_preset_rx_lte_gain_mask    0x70


#define pcie_lane0_gen1_preset_rx_lte_zero_addr    0b100011000
#define pcie_lane0_gen1_preset_rx_lte_zero_startbit   13
#define pcie_lane0_gen1_preset_rx_lte_zero_width   3
#define pcie_lane0_gen1_preset_rx_lte_zero_endbit   15
#define pcie_lane0_gen1_preset_rx_lte_zero_shift   0
#define pcie_lane0_gen1_preset_rx_lte_zero_mask    0x7


#define pcie_lane0_gen2_preset_rx_ctle_peak1_addr   0b100011001
#define pcie_lane0_gen2_preset_rx_ctle_peak1_startbit   0
#define pcie_lane0_gen2_preset_rx_ctle_peak1_width   4
#define pcie_lane0_gen2_preset_rx_ctle_peak1_endbit   3
#define pcie_lane0_gen2_preset_rx_ctle_peak1_shift   12
#define pcie_lane0_gen2_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_addr   0b100011001
#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane0_gen2_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane0_gen2_preset_rx_ctle_peak2_addr   0b100011001
#define pcie_lane0_gen2_preset_rx_ctle_peak2_startbit   4
#define pcie_lane0_gen2_preset_rx_ctle_peak2_width   4
#define pcie_lane0_gen2_preset_rx_ctle_peak2_endbit   7
#define pcie_lane0_gen2_preset_rx_ctle_peak2_shift   8
#define pcie_lane0_gen2_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane0_gen2_preset_rx_lte_gain_addr    0b100011001
#define pcie_lane0_gen2_preset_rx_lte_gain_startbit   9
#define pcie_lane0_gen2_preset_rx_lte_gain_width   3
#define pcie_lane0_gen2_preset_rx_lte_gain_endbit   11
#define pcie_lane0_gen2_preset_rx_lte_gain_shift   4
#define pcie_lane0_gen2_preset_rx_lte_gain_mask    0x70


#define pcie_lane0_gen2_preset_rx_lte_zero_addr    0b100011001
#define pcie_lane0_gen2_preset_rx_lte_zero_startbit   13
#define pcie_lane0_gen2_preset_rx_lte_zero_width   3
#define pcie_lane0_gen2_preset_rx_lte_zero_endbit   15
#define pcie_lane0_gen2_preset_rx_lte_zero_shift   0
#define pcie_lane0_gen2_preset_rx_lte_zero_mask    0x7


#define pcie_lane0_gen3_preset_rx_ctle_peak1_addr   0b100011010
#define pcie_lane0_gen3_preset_rx_ctle_peak1_startbit   0
#define pcie_lane0_gen3_preset_rx_ctle_peak1_width   4
#define pcie_lane0_gen3_preset_rx_ctle_peak1_endbit   3
#define pcie_lane0_gen3_preset_rx_ctle_peak1_shift   12
#define pcie_lane0_gen3_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_addr   0b100011010
#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane0_gen3_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane0_gen3_preset_rx_ctle_peak2_addr   0b100011010
#define pcie_lane0_gen3_preset_rx_ctle_peak2_startbit   4
#define pcie_lane0_gen3_preset_rx_ctle_peak2_width   4
#define pcie_lane0_gen3_preset_rx_ctle_peak2_endbit   7
#define pcie_lane0_gen3_preset_rx_ctle_peak2_shift   8
#define pcie_lane0_gen3_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane0_gen3_preset_rx_lte_gain_addr    0b100011010
#define pcie_lane0_gen3_preset_rx_lte_gain_startbit   9
#define pcie_lane0_gen3_preset_rx_lte_gain_width   3
#define pcie_lane0_gen3_preset_rx_lte_gain_endbit   11
#define pcie_lane0_gen3_preset_rx_lte_gain_shift   4
#define pcie_lane0_gen3_preset_rx_lte_gain_mask    0x70


#define pcie_lane0_gen3_preset_rx_lte_zero_addr    0b100011010
#define pcie_lane0_gen3_preset_rx_lte_zero_startbit   13
#define pcie_lane0_gen3_preset_rx_lte_zero_width   3
#define pcie_lane0_gen3_preset_rx_lte_zero_endbit   15
#define pcie_lane0_gen3_preset_rx_lte_zero_shift   0
#define pcie_lane0_gen3_preset_rx_lte_zero_mask    0x7


#define pcie_lane0_gen4_preset_rx_ctle_peak1_addr   0b100011011
#define pcie_lane0_gen4_preset_rx_ctle_peak1_startbit   0
#define pcie_lane0_gen4_preset_rx_ctle_peak1_width   4
#define pcie_lane0_gen4_preset_rx_ctle_peak1_endbit   3
#define pcie_lane0_gen4_preset_rx_ctle_peak1_shift   12
#define pcie_lane0_gen4_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_addr   0b100011011
#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane0_gen4_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane0_gen4_preset_rx_ctle_peak2_addr   0b100011011
#define pcie_lane0_gen4_preset_rx_ctle_peak2_startbit   4
#define pcie_lane0_gen4_preset_rx_ctle_peak2_width   4
#define pcie_lane0_gen4_preset_rx_ctle_peak2_endbit   7
#define pcie_lane0_gen4_preset_rx_ctle_peak2_shift   8
#define pcie_lane0_gen4_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane0_gen4_preset_rx_lte_gain_addr    0b100011011
#define pcie_lane0_gen4_preset_rx_lte_gain_startbit   9
#define pcie_lane0_gen4_preset_rx_lte_gain_width   3
#define pcie_lane0_gen4_preset_rx_lte_gain_endbit   11
#define pcie_lane0_gen4_preset_rx_lte_gain_shift   4
#define pcie_lane0_gen4_preset_rx_lte_gain_mask    0x70


#define pcie_lane0_gen4_preset_rx_lte_zero_addr    0b100011011
#define pcie_lane0_gen4_preset_rx_lte_zero_startbit   13
#define pcie_lane0_gen4_preset_rx_lte_zero_width   3
#define pcie_lane0_gen4_preset_rx_lte_zero_endbit   15
#define pcie_lane0_gen4_preset_rx_lte_zero_shift   0
#define pcie_lane0_gen4_preset_rx_lte_zero_mask    0x7


#define pcie_lane0_gen5_preset_rx_ctle_peak1_addr   0b100011100
#define pcie_lane0_gen5_preset_rx_ctle_peak1_startbit   0
#define pcie_lane0_gen5_preset_rx_ctle_peak1_width   4
#define pcie_lane0_gen5_preset_rx_ctle_peak1_endbit   3
#define pcie_lane0_gen5_preset_rx_ctle_peak1_shift   12
#define pcie_lane0_gen5_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_addr   0b100011100
#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane0_gen5_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane0_gen5_preset_rx_ctle_peak2_addr   0b100011100
#define pcie_lane0_gen5_preset_rx_ctle_peak2_startbit   4
#define pcie_lane0_gen5_preset_rx_ctle_peak2_width   4
#define pcie_lane0_gen5_preset_rx_ctle_peak2_endbit   7
#define pcie_lane0_gen5_preset_rx_ctle_peak2_shift   8
#define pcie_lane0_gen5_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane0_gen5_preset_rx_lte_gain_addr    0b100011100
#define pcie_lane0_gen5_preset_rx_lte_gain_startbit   9
#define pcie_lane0_gen5_preset_rx_lte_gain_width   3
#define pcie_lane0_gen5_preset_rx_lte_gain_endbit   11
#define pcie_lane0_gen5_preset_rx_lte_gain_shift   4
#define pcie_lane0_gen5_preset_rx_lte_gain_mask    0x70


#define pcie_lane0_gen5_preset_rx_lte_zero_addr    0b100011100
#define pcie_lane0_gen5_preset_rx_lte_zero_startbit   13
#define pcie_lane0_gen5_preset_rx_lte_zero_width   3
#define pcie_lane0_gen5_preset_rx_lte_zero_endbit   15
#define pcie_lane0_gen5_preset_rx_lte_zero_shift   0
#define pcie_lane0_gen5_preset_rx_lte_zero_mask    0x7


#define pcie_lane1_gen1_preset_rx_ctle_peak1_addr   0b100100000
#define pcie_lane1_gen1_preset_rx_ctle_peak1_startbit   0
#define pcie_lane1_gen1_preset_rx_ctle_peak1_width   4
#define pcie_lane1_gen1_preset_rx_ctle_peak1_endbit   3
#define pcie_lane1_gen1_preset_rx_ctle_peak1_shift   12
#define pcie_lane1_gen1_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_addr   0b100100000
#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane1_gen1_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane1_gen1_preset_rx_ctle_peak2_addr   0b100100000
#define pcie_lane1_gen1_preset_rx_ctle_peak2_startbit   4
#define pcie_lane1_gen1_preset_rx_ctle_peak2_width   4
#define pcie_lane1_gen1_preset_rx_ctle_peak2_endbit   7
#define pcie_lane1_gen1_preset_rx_ctle_peak2_shift   8
#define pcie_lane1_gen1_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane1_gen1_preset_rx_lte_gain_addr    0b100100000
#define pcie_lane1_gen1_preset_rx_lte_gain_startbit   9
#define pcie_lane1_gen1_preset_rx_lte_gain_width   3
#define pcie_lane1_gen1_preset_rx_lte_gain_endbit   11
#define pcie_lane1_gen1_preset_rx_lte_gain_shift   4
#define pcie_lane1_gen1_preset_rx_lte_gain_mask    0x70


#define pcie_lane1_gen1_preset_rx_lte_zero_addr    0b100100000
#define pcie_lane1_gen1_preset_rx_lte_zero_startbit   13
#define pcie_lane1_gen1_preset_rx_lte_zero_width   3
#define pcie_lane1_gen1_preset_rx_lte_zero_endbit   15
#define pcie_lane1_gen1_preset_rx_lte_zero_shift   0
#define pcie_lane1_gen1_preset_rx_lte_zero_mask    0x7


#define pcie_lane1_gen2_preset_rx_ctle_peak1_addr   0b100100001
#define pcie_lane1_gen2_preset_rx_ctle_peak1_startbit   0
#define pcie_lane1_gen2_preset_rx_ctle_peak1_width   4
#define pcie_lane1_gen2_preset_rx_ctle_peak1_endbit   3
#define pcie_lane1_gen2_preset_rx_ctle_peak1_shift   12
#define pcie_lane1_gen2_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_addr   0b100100001
#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane1_gen2_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane1_gen2_preset_rx_ctle_peak2_addr   0b100100001
#define pcie_lane1_gen2_preset_rx_ctle_peak2_startbit   4
#define pcie_lane1_gen2_preset_rx_ctle_peak2_width   4
#define pcie_lane1_gen2_preset_rx_ctle_peak2_endbit   7
#define pcie_lane1_gen2_preset_rx_ctle_peak2_shift   8
#define pcie_lane1_gen2_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane1_gen2_preset_rx_lte_gain_addr    0b100100001
#define pcie_lane1_gen2_preset_rx_lte_gain_startbit   9
#define pcie_lane1_gen2_preset_rx_lte_gain_width   3
#define pcie_lane1_gen2_preset_rx_lte_gain_endbit   11
#define pcie_lane1_gen2_preset_rx_lte_gain_shift   4
#define pcie_lane1_gen2_preset_rx_lte_gain_mask    0x70


#define pcie_lane1_gen2_preset_rx_lte_zero_addr    0b100100001
#define pcie_lane1_gen2_preset_rx_lte_zero_startbit   13
#define pcie_lane1_gen2_preset_rx_lte_zero_width   3
#define pcie_lane1_gen2_preset_rx_lte_zero_endbit   15
#define pcie_lane1_gen2_preset_rx_lte_zero_shift   0
#define pcie_lane1_gen2_preset_rx_lte_zero_mask    0x7


#define pcie_lane1_gen3_preset_rx_ctle_peak1_addr   0b100100010
#define pcie_lane1_gen3_preset_rx_ctle_peak1_startbit   0
#define pcie_lane1_gen3_preset_rx_ctle_peak1_width   4
#define pcie_lane1_gen3_preset_rx_ctle_peak1_endbit   3
#define pcie_lane1_gen3_preset_rx_ctle_peak1_shift   12
#define pcie_lane1_gen3_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_addr   0b100100010
#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane1_gen3_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane1_gen3_preset_rx_ctle_peak2_addr   0b100100010
#define pcie_lane1_gen3_preset_rx_ctle_peak2_startbit   4
#define pcie_lane1_gen3_preset_rx_ctle_peak2_width   4
#define pcie_lane1_gen3_preset_rx_ctle_peak2_endbit   7
#define pcie_lane1_gen3_preset_rx_ctle_peak2_shift   8
#define pcie_lane1_gen3_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane1_gen3_preset_rx_lte_gain_addr    0b100100010
#define pcie_lane1_gen3_preset_rx_lte_gain_startbit   9
#define pcie_lane1_gen3_preset_rx_lte_gain_width   3
#define pcie_lane1_gen3_preset_rx_lte_gain_endbit   11
#define pcie_lane1_gen3_preset_rx_lte_gain_shift   4
#define pcie_lane1_gen3_preset_rx_lte_gain_mask    0x70


#define pcie_lane1_gen3_preset_rx_lte_zero_addr    0b100100010
#define pcie_lane1_gen3_preset_rx_lte_zero_startbit   13
#define pcie_lane1_gen3_preset_rx_lte_zero_width   3
#define pcie_lane1_gen3_preset_rx_lte_zero_endbit   15
#define pcie_lane1_gen3_preset_rx_lte_zero_shift   0
#define pcie_lane1_gen3_preset_rx_lte_zero_mask    0x7


#define pcie_lane1_gen4_preset_rx_ctle_peak1_addr   0b100100011
#define pcie_lane1_gen4_preset_rx_ctle_peak1_startbit   0
#define pcie_lane1_gen4_preset_rx_ctle_peak1_width   4
#define pcie_lane1_gen4_preset_rx_ctle_peak1_endbit   3
#define pcie_lane1_gen4_preset_rx_ctle_peak1_shift   12
#define pcie_lane1_gen4_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_addr   0b100100011
#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane1_gen4_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane1_gen4_preset_rx_ctle_peak2_addr   0b100100011
#define pcie_lane1_gen4_preset_rx_ctle_peak2_startbit   4
#define pcie_lane1_gen4_preset_rx_ctle_peak2_width   4
#define pcie_lane1_gen4_preset_rx_ctle_peak2_endbit   7
#define pcie_lane1_gen4_preset_rx_ctle_peak2_shift   8
#define pcie_lane1_gen4_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane1_gen4_preset_rx_lte_gain_addr    0b100100011
#define pcie_lane1_gen4_preset_rx_lte_gain_startbit   9
#define pcie_lane1_gen4_preset_rx_lte_gain_width   3
#define pcie_lane1_gen4_preset_rx_lte_gain_endbit   11
#define pcie_lane1_gen4_preset_rx_lte_gain_shift   4
#define pcie_lane1_gen4_preset_rx_lte_gain_mask    0x70


#define pcie_lane1_gen4_preset_rx_lte_zero_addr    0b100100011
#define pcie_lane1_gen4_preset_rx_lte_zero_startbit   13
#define pcie_lane1_gen4_preset_rx_lte_zero_width   3
#define pcie_lane1_gen4_preset_rx_lte_zero_endbit   15
#define pcie_lane1_gen4_preset_rx_lte_zero_shift   0
#define pcie_lane1_gen4_preset_rx_lte_zero_mask    0x7


#define pcie_lane1_gen5_preset_rx_ctle_peak1_addr   0b100100100
#define pcie_lane1_gen5_preset_rx_ctle_peak1_startbit   0
#define pcie_lane1_gen5_preset_rx_ctle_peak1_width   4
#define pcie_lane1_gen5_preset_rx_ctle_peak1_endbit   3
#define pcie_lane1_gen5_preset_rx_ctle_peak1_shift   12
#define pcie_lane1_gen5_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_addr   0b100100100
#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane1_gen5_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane1_gen5_preset_rx_ctle_peak2_addr   0b100100100
#define pcie_lane1_gen5_preset_rx_ctle_peak2_startbit   4
#define pcie_lane1_gen5_preset_rx_ctle_peak2_width   4
#define pcie_lane1_gen5_preset_rx_ctle_peak2_endbit   7
#define pcie_lane1_gen5_preset_rx_ctle_peak2_shift   8
#define pcie_lane1_gen5_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane1_gen5_preset_rx_lte_gain_addr    0b100100100
#define pcie_lane1_gen5_preset_rx_lte_gain_startbit   9
#define pcie_lane1_gen5_preset_rx_lte_gain_width   3
#define pcie_lane1_gen5_preset_rx_lte_gain_endbit   11
#define pcie_lane1_gen5_preset_rx_lte_gain_shift   4
#define pcie_lane1_gen5_preset_rx_lte_gain_mask    0x70


#define pcie_lane1_gen5_preset_rx_lte_zero_addr    0b100100100
#define pcie_lane1_gen5_preset_rx_lte_zero_startbit   13
#define pcie_lane1_gen5_preset_rx_lte_zero_width   3
#define pcie_lane1_gen5_preset_rx_lte_zero_endbit   15
#define pcie_lane1_gen5_preset_rx_lte_zero_shift   0
#define pcie_lane1_gen5_preset_rx_lte_zero_mask    0x7


#define pcie_lane2_gen1_preset_rx_ctle_peak1_addr   0b100101000
#define pcie_lane2_gen1_preset_rx_ctle_peak1_startbit   0
#define pcie_lane2_gen1_preset_rx_ctle_peak1_width   4
#define pcie_lane2_gen1_preset_rx_ctle_peak1_endbit   3
#define pcie_lane2_gen1_preset_rx_ctle_peak1_shift   12
#define pcie_lane2_gen1_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_addr   0b100101000
#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane2_gen1_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane2_gen1_preset_rx_ctle_peak2_addr   0b100101000
#define pcie_lane2_gen1_preset_rx_ctle_peak2_startbit   4
#define pcie_lane2_gen1_preset_rx_ctle_peak2_width   4
#define pcie_lane2_gen1_preset_rx_ctle_peak2_endbit   7
#define pcie_lane2_gen1_preset_rx_ctle_peak2_shift   8
#define pcie_lane2_gen1_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane2_gen1_preset_rx_lte_gain_addr    0b100101000
#define pcie_lane2_gen1_preset_rx_lte_gain_startbit   9
#define pcie_lane2_gen1_preset_rx_lte_gain_width   3
#define pcie_lane2_gen1_preset_rx_lte_gain_endbit   11
#define pcie_lane2_gen1_preset_rx_lte_gain_shift   4
#define pcie_lane2_gen1_preset_rx_lte_gain_mask    0x70


#define pcie_lane2_gen1_preset_rx_lte_zero_addr    0b100101000
#define pcie_lane2_gen1_preset_rx_lte_zero_startbit   13
#define pcie_lane2_gen1_preset_rx_lte_zero_width   3
#define pcie_lane2_gen1_preset_rx_lte_zero_endbit   15
#define pcie_lane2_gen1_preset_rx_lte_zero_shift   0
#define pcie_lane2_gen1_preset_rx_lte_zero_mask    0x7


#define pcie_lane2_gen2_preset_rx_ctle_peak1_addr   0b100101001
#define pcie_lane2_gen2_preset_rx_ctle_peak1_startbit   0
#define pcie_lane2_gen2_preset_rx_ctle_peak1_width   4
#define pcie_lane2_gen2_preset_rx_ctle_peak1_endbit   3
#define pcie_lane2_gen2_preset_rx_ctle_peak1_shift   12
#define pcie_lane2_gen2_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_addr   0b100101001
#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane2_gen2_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane2_gen2_preset_rx_ctle_peak2_addr   0b100101001
#define pcie_lane2_gen2_preset_rx_ctle_peak2_startbit   4
#define pcie_lane2_gen2_preset_rx_ctle_peak2_width   4
#define pcie_lane2_gen2_preset_rx_ctle_peak2_endbit   7
#define pcie_lane2_gen2_preset_rx_ctle_peak2_shift   8
#define pcie_lane2_gen2_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane2_gen2_preset_rx_lte_gain_addr    0b100101001
#define pcie_lane2_gen2_preset_rx_lte_gain_startbit   9
#define pcie_lane2_gen2_preset_rx_lte_gain_width   3
#define pcie_lane2_gen2_preset_rx_lte_gain_endbit   11
#define pcie_lane2_gen2_preset_rx_lte_gain_shift   4
#define pcie_lane2_gen2_preset_rx_lte_gain_mask    0x70


#define pcie_lane2_gen2_preset_rx_lte_zero_addr    0b100101001
#define pcie_lane2_gen2_preset_rx_lte_zero_startbit   13
#define pcie_lane2_gen2_preset_rx_lte_zero_width   3
#define pcie_lane2_gen2_preset_rx_lte_zero_endbit   15
#define pcie_lane2_gen2_preset_rx_lte_zero_shift   0
#define pcie_lane2_gen2_preset_rx_lte_zero_mask    0x7


#define pcie_lane2_gen3_preset_rx_ctle_peak1_addr   0b100101010
#define pcie_lane2_gen3_preset_rx_ctle_peak1_startbit   0
#define pcie_lane2_gen3_preset_rx_ctle_peak1_width   4
#define pcie_lane2_gen3_preset_rx_ctle_peak1_endbit   3
#define pcie_lane2_gen3_preset_rx_ctle_peak1_shift   12
#define pcie_lane2_gen3_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_addr   0b100101010
#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane2_gen3_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane2_gen3_preset_rx_ctle_peak2_addr   0b100101010
#define pcie_lane2_gen3_preset_rx_ctle_peak2_startbit   4
#define pcie_lane2_gen3_preset_rx_ctle_peak2_width   4
#define pcie_lane2_gen3_preset_rx_ctle_peak2_endbit   7
#define pcie_lane2_gen3_preset_rx_ctle_peak2_shift   8
#define pcie_lane2_gen3_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane2_gen3_preset_rx_lte_gain_addr    0b100101010
#define pcie_lane2_gen3_preset_rx_lte_gain_startbit   9
#define pcie_lane2_gen3_preset_rx_lte_gain_width   3
#define pcie_lane2_gen3_preset_rx_lte_gain_endbit   11
#define pcie_lane2_gen3_preset_rx_lte_gain_shift   4
#define pcie_lane2_gen3_preset_rx_lte_gain_mask    0x70


#define pcie_lane2_gen3_preset_rx_lte_zero_addr    0b100101010
#define pcie_lane2_gen3_preset_rx_lte_zero_startbit   13
#define pcie_lane2_gen3_preset_rx_lte_zero_width   3
#define pcie_lane2_gen3_preset_rx_lte_zero_endbit   15
#define pcie_lane2_gen3_preset_rx_lte_zero_shift   0
#define pcie_lane2_gen3_preset_rx_lte_zero_mask    0x7


#define pcie_lane2_gen4_preset_rx_ctle_peak1_addr   0b100101011
#define pcie_lane2_gen4_preset_rx_ctle_peak1_startbit   0
#define pcie_lane2_gen4_preset_rx_ctle_peak1_width   4
#define pcie_lane2_gen4_preset_rx_ctle_peak1_endbit   3
#define pcie_lane2_gen4_preset_rx_ctle_peak1_shift   12
#define pcie_lane2_gen4_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_addr   0b100101011
#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane2_gen4_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane2_gen4_preset_rx_ctle_peak2_addr   0b100101011
#define pcie_lane2_gen4_preset_rx_ctle_peak2_startbit   4
#define pcie_lane2_gen4_preset_rx_ctle_peak2_width   4
#define pcie_lane2_gen4_preset_rx_ctle_peak2_endbit   7
#define pcie_lane2_gen4_preset_rx_ctle_peak2_shift   8
#define pcie_lane2_gen4_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane2_gen4_preset_rx_lte_gain_addr    0b100101011
#define pcie_lane2_gen4_preset_rx_lte_gain_startbit   9
#define pcie_lane2_gen4_preset_rx_lte_gain_width   3
#define pcie_lane2_gen4_preset_rx_lte_gain_endbit   11
#define pcie_lane2_gen4_preset_rx_lte_gain_shift   4
#define pcie_lane2_gen4_preset_rx_lte_gain_mask    0x70


#define pcie_lane2_gen4_preset_rx_lte_zero_addr    0b100101011
#define pcie_lane2_gen4_preset_rx_lte_zero_startbit   13
#define pcie_lane2_gen4_preset_rx_lte_zero_width   3
#define pcie_lane2_gen4_preset_rx_lte_zero_endbit   15
#define pcie_lane2_gen4_preset_rx_lte_zero_shift   0
#define pcie_lane2_gen4_preset_rx_lte_zero_mask    0x7


#define pcie_lane2_gen5_preset_rx_ctle_peak1_addr   0b100101100
#define pcie_lane2_gen5_preset_rx_ctle_peak1_startbit   0
#define pcie_lane2_gen5_preset_rx_ctle_peak1_width   4
#define pcie_lane2_gen5_preset_rx_ctle_peak1_endbit   3
#define pcie_lane2_gen5_preset_rx_ctle_peak1_shift   12
#define pcie_lane2_gen5_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_addr   0b100101100
#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane2_gen5_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane2_gen5_preset_rx_ctle_peak2_addr   0b100101100
#define pcie_lane2_gen5_preset_rx_ctle_peak2_startbit   4
#define pcie_lane2_gen5_preset_rx_ctle_peak2_width   4
#define pcie_lane2_gen5_preset_rx_ctle_peak2_endbit   7
#define pcie_lane2_gen5_preset_rx_ctle_peak2_shift   8
#define pcie_lane2_gen5_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane2_gen5_preset_rx_lte_gain_addr    0b100101100
#define pcie_lane2_gen5_preset_rx_lte_gain_startbit   9
#define pcie_lane2_gen5_preset_rx_lte_gain_width   3
#define pcie_lane2_gen5_preset_rx_lte_gain_endbit   11
#define pcie_lane2_gen5_preset_rx_lte_gain_shift   4
#define pcie_lane2_gen5_preset_rx_lte_gain_mask    0x70


#define pcie_lane2_gen5_preset_rx_lte_zero_addr    0b100101100
#define pcie_lane2_gen5_preset_rx_lte_zero_startbit   13
#define pcie_lane2_gen5_preset_rx_lte_zero_width   3
#define pcie_lane2_gen5_preset_rx_lte_zero_endbit   15
#define pcie_lane2_gen5_preset_rx_lte_zero_shift   0
#define pcie_lane2_gen5_preset_rx_lte_zero_mask    0x7


#define pcie_lane3_gen1_preset_rx_ctle_peak1_addr   0b100110000
#define pcie_lane3_gen1_preset_rx_ctle_peak1_startbit   0
#define pcie_lane3_gen1_preset_rx_ctle_peak1_width   4
#define pcie_lane3_gen1_preset_rx_ctle_peak1_endbit   3
#define pcie_lane3_gen1_preset_rx_ctle_peak1_shift   12
#define pcie_lane3_gen1_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_addr   0b100110000
#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane3_gen1_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane3_gen1_preset_rx_ctle_peak2_addr   0b100110000
#define pcie_lane3_gen1_preset_rx_ctle_peak2_startbit   4
#define pcie_lane3_gen1_preset_rx_ctle_peak2_width   4
#define pcie_lane3_gen1_preset_rx_ctle_peak2_endbit   7
#define pcie_lane3_gen1_preset_rx_ctle_peak2_shift   8
#define pcie_lane3_gen1_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane3_gen1_preset_rx_lte_gain_addr    0b100110000
#define pcie_lane3_gen1_preset_rx_lte_gain_startbit   9
#define pcie_lane3_gen1_preset_rx_lte_gain_width   3
#define pcie_lane3_gen1_preset_rx_lte_gain_endbit   11
#define pcie_lane3_gen1_preset_rx_lte_gain_shift   4
#define pcie_lane3_gen1_preset_rx_lte_gain_mask    0x70


#define pcie_lane3_gen1_preset_rx_lte_zero_addr    0b100110000
#define pcie_lane3_gen1_preset_rx_lte_zero_startbit   13
#define pcie_lane3_gen1_preset_rx_lte_zero_width   3
#define pcie_lane3_gen1_preset_rx_lte_zero_endbit   15
#define pcie_lane3_gen1_preset_rx_lte_zero_shift   0
#define pcie_lane3_gen1_preset_rx_lte_zero_mask    0x7


#define pcie_lane3_gen2_preset_rx_ctle_peak1_addr   0b100110001
#define pcie_lane3_gen2_preset_rx_ctle_peak1_startbit   0
#define pcie_lane3_gen2_preset_rx_ctle_peak1_width   4
#define pcie_lane3_gen2_preset_rx_ctle_peak1_endbit   3
#define pcie_lane3_gen2_preset_rx_ctle_peak1_shift   12
#define pcie_lane3_gen2_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_addr   0b100110001
#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane3_gen2_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane3_gen2_preset_rx_ctle_peak2_addr   0b100110001
#define pcie_lane3_gen2_preset_rx_ctle_peak2_startbit   4
#define pcie_lane3_gen2_preset_rx_ctle_peak2_width   4
#define pcie_lane3_gen2_preset_rx_ctle_peak2_endbit   7
#define pcie_lane3_gen2_preset_rx_ctle_peak2_shift   8
#define pcie_lane3_gen2_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane3_gen2_preset_rx_lte_gain_addr    0b100110001
#define pcie_lane3_gen2_preset_rx_lte_gain_startbit   9
#define pcie_lane3_gen2_preset_rx_lte_gain_width   3
#define pcie_lane3_gen2_preset_rx_lte_gain_endbit   11
#define pcie_lane3_gen2_preset_rx_lte_gain_shift   4
#define pcie_lane3_gen2_preset_rx_lte_gain_mask    0x70


#define pcie_lane3_gen2_preset_rx_lte_zero_addr    0b100110001
#define pcie_lane3_gen2_preset_rx_lte_zero_startbit   13
#define pcie_lane3_gen2_preset_rx_lte_zero_width   3
#define pcie_lane3_gen2_preset_rx_lte_zero_endbit   15
#define pcie_lane3_gen2_preset_rx_lte_zero_shift   0
#define pcie_lane3_gen2_preset_rx_lte_zero_mask    0x7


#define pcie_lane3_gen3_preset_rx_ctle_peak1_addr   0b100110010
#define pcie_lane3_gen3_preset_rx_ctle_peak1_startbit   0
#define pcie_lane3_gen3_preset_rx_ctle_peak1_width   4
#define pcie_lane3_gen3_preset_rx_ctle_peak1_endbit   3
#define pcie_lane3_gen3_preset_rx_ctle_peak1_shift   12
#define pcie_lane3_gen3_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_addr   0b100110010
#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane3_gen3_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane3_gen3_preset_rx_ctle_peak2_addr   0b100110010
#define pcie_lane3_gen3_preset_rx_ctle_peak2_startbit   4
#define pcie_lane3_gen3_preset_rx_ctle_peak2_width   4
#define pcie_lane3_gen3_preset_rx_ctle_peak2_endbit   7
#define pcie_lane3_gen3_preset_rx_ctle_peak2_shift   8
#define pcie_lane3_gen3_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane3_gen3_preset_rx_lte_gain_addr    0b100110010
#define pcie_lane3_gen3_preset_rx_lte_gain_startbit   9
#define pcie_lane3_gen3_preset_rx_lte_gain_width   3
#define pcie_lane3_gen3_preset_rx_lte_gain_endbit   11
#define pcie_lane3_gen3_preset_rx_lte_gain_shift   4
#define pcie_lane3_gen3_preset_rx_lte_gain_mask    0x70


#define pcie_lane3_gen3_preset_rx_lte_zero_addr    0b100110010
#define pcie_lane3_gen3_preset_rx_lte_zero_startbit   13
#define pcie_lane3_gen3_preset_rx_lte_zero_width   3
#define pcie_lane3_gen3_preset_rx_lte_zero_endbit   15
#define pcie_lane3_gen3_preset_rx_lte_zero_shift   0
#define pcie_lane3_gen3_preset_rx_lte_zero_mask    0x7


#define pcie_lane3_gen4_preset_rx_ctle_peak1_addr   0b100110011
#define pcie_lane3_gen4_preset_rx_ctle_peak1_startbit   0
#define pcie_lane3_gen4_preset_rx_ctle_peak1_width   4
#define pcie_lane3_gen4_preset_rx_ctle_peak1_endbit   3
#define pcie_lane3_gen4_preset_rx_ctle_peak1_shift   12
#define pcie_lane3_gen4_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_addr   0b100110011
#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane3_gen4_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane3_gen4_preset_rx_ctle_peak2_addr   0b100110011
#define pcie_lane3_gen4_preset_rx_ctle_peak2_startbit   4
#define pcie_lane3_gen4_preset_rx_ctle_peak2_width   4
#define pcie_lane3_gen4_preset_rx_ctle_peak2_endbit   7
#define pcie_lane3_gen4_preset_rx_ctle_peak2_shift   8
#define pcie_lane3_gen4_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane3_gen4_preset_rx_lte_gain_addr    0b100110011
#define pcie_lane3_gen4_preset_rx_lte_gain_startbit   9
#define pcie_lane3_gen4_preset_rx_lte_gain_width   3
#define pcie_lane3_gen4_preset_rx_lte_gain_endbit   11
#define pcie_lane3_gen4_preset_rx_lte_gain_shift   4
#define pcie_lane3_gen4_preset_rx_lte_gain_mask    0x70


#define pcie_lane3_gen4_preset_rx_lte_zero_addr    0b100110011
#define pcie_lane3_gen4_preset_rx_lte_zero_startbit   13
#define pcie_lane3_gen4_preset_rx_lte_zero_width   3
#define pcie_lane3_gen4_preset_rx_lte_zero_endbit   15
#define pcie_lane3_gen4_preset_rx_lte_zero_shift   0
#define pcie_lane3_gen4_preset_rx_lte_zero_mask    0x7


#define pcie_lane3_gen5_preset_rx_ctle_peak1_addr   0b100110100
#define pcie_lane3_gen5_preset_rx_ctle_peak1_startbit   0
#define pcie_lane3_gen5_preset_rx_ctle_peak1_width   4
#define pcie_lane3_gen5_preset_rx_ctle_peak1_endbit   3
#define pcie_lane3_gen5_preset_rx_ctle_peak1_shift   12
#define pcie_lane3_gen5_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_addr   0b100110100
#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane3_gen5_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane3_gen5_preset_rx_ctle_peak2_addr   0b100110100
#define pcie_lane3_gen5_preset_rx_ctle_peak2_startbit   4
#define pcie_lane3_gen5_preset_rx_ctle_peak2_width   4
#define pcie_lane3_gen5_preset_rx_ctle_peak2_endbit   7
#define pcie_lane3_gen5_preset_rx_ctle_peak2_shift   8
#define pcie_lane3_gen5_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane3_gen5_preset_rx_lte_gain_addr    0b100110100
#define pcie_lane3_gen5_preset_rx_lte_gain_startbit   9
#define pcie_lane3_gen5_preset_rx_lte_gain_width   3
#define pcie_lane3_gen5_preset_rx_lte_gain_endbit   11
#define pcie_lane3_gen5_preset_rx_lte_gain_shift   4
#define pcie_lane3_gen5_preset_rx_lte_gain_mask    0x70


#define pcie_lane3_gen5_preset_rx_lte_zero_addr    0b100110100
#define pcie_lane3_gen5_preset_rx_lte_zero_startbit   13
#define pcie_lane3_gen5_preset_rx_lte_zero_width   3
#define pcie_lane3_gen5_preset_rx_lte_zero_endbit   15
#define pcie_lane3_gen5_preset_rx_lte_zero_shift   0
#define pcie_lane3_gen5_preset_rx_lte_zero_mask    0x7


#define pcie_lane4_gen1_preset_rx_ctle_peak1_addr   0b100111000
#define pcie_lane4_gen1_preset_rx_ctle_peak1_startbit   0
#define pcie_lane4_gen1_preset_rx_ctle_peak1_width   4
#define pcie_lane4_gen1_preset_rx_ctle_peak1_endbit   3
#define pcie_lane4_gen1_preset_rx_ctle_peak1_shift   12
#define pcie_lane4_gen1_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_addr   0b100111000
#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane4_gen1_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane4_gen1_preset_rx_ctle_peak2_addr   0b100111000
#define pcie_lane4_gen1_preset_rx_ctle_peak2_startbit   4
#define pcie_lane4_gen1_preset_rx_ctle_peak2_width   4
#define pcie_lane4_gen1_preset_rx_ctle_peak2_endbit   7
#define pcie_lane4_gen1_preset_rx_ctle_peak2_shift   8
#define pcie_lane4_gen1_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane4_gen1_preset_rx_lte_gain_addr    0b100111000
#define pcie_lane4_gen1_preset_rx_lte_gain_startbit   9
#define pcie_lane4_gen1_preset_rx_lte_gain_width   3
#define pcie_lane4_gen1_preset_rx_lte_gain_endbit   11
#define pcie_lane4_gen1_preset_rx_lte_gain_shift   4
#define pcie_lane4_gen1_preset_rx_lte_gain_mask    0x70


#define pcie_lane4_gen1_preset_rx_lte_zero_addr    0b100111000
#define pcie_lane4_gen1_preset_rx_lte_zero_startbit   13
#define pcie_lane4_gen1_preset_rx_lte_zero_width   3
#define pcie_lane4_gen1_preset_rx_lte_zero_endbit   15
#define pcie_lane4_gen1_preset_rx_lte_zero_shift   0
#define pcie_lane4_gen1_preset_rx_lte_zero_mask    0x7


#define pcie_lane4_gen2_preset_rx_ctle_peak1_addr   0b100111001
#define pcie_lane4_gen2_preset_rx_ctle_peak1_startbit   0
#define pcie_lane4_gen2_preset_rx_ctle_peak1_width   4
#define pcie_lane4_gen2_preset_rx_ctle_peak1_endbit   3
#define pcie_lane4_gen2_preset_rx_ctle_peak1_shift   12
#define pcie_lane4_gen2_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_addr   0b100111001
#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane4_gen2_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane4_gen2_preset_rx_ctle_peak2_addr   0b100111001
#define pcie_lane4_gen2_preset_rx_ctle_peak2_startbit   4
#define pcie_lane4_gen2_preset_rx_ctle_peak2_width   4
#define pcie_lane4_gen2_preset_rx_ctle_peak2_endbit   7
#define pcie_lane4_gen2_preset_rx_ctle_peak2_shift   8
#define pcie_lane4_gen2_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane4_gen2_preset_rx_lte_gain_addr    0b100111001
#define pcie_lane4_gen2_preset_rx_lte_gain_startbit   9
#define pcie_lane4_gen2_preset_rx_lte_gain_width   3
#define pcie_lane4_gen2_preset_rx_lte_gain_endbit   11
#define pcie_lane4_gen2_preset_rx_lte_gain_shift   4
#define pcie_lane4_gen2_preset_rx_lte_gain_mask    0x70


#define pcie_lane4_gen2_preset_rx_lte_zero_addr    0b100111001
#define pcie_lane4_gen2_preset_rx_lte_zero_startbit   13
#define pcie_lane4_gen2_preset_rx_lte_zero_width   3
#define pcie_lane4_gen2_preset_rx_lte_zero_endbit   15
#define pcie_lane4_gen2_preset_rx_lte_zero_shift   0
#define pcie_lane4_gen2_preset_rx_lte_zero_mask    0x7


#define pcie_lane4_gen3_preset_rx_ctle_peak1_addr   0b100111010
#define pcie_lane4_gen3_preset_rx_ctle_peak1_startbit   0
#define pcie_lane4_gen3_preset_rx_ctle_peak1_width   4
#define pcie_lane4_gen3_preset_rx_ctle_peak1_endbit   3
#define pcie_lane4_gen3_preset_rx_ctle_peak1_shift   12
#define pcie_lane4_gen3_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_addr   0b100111010
#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane4_gen3_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane4_gen3_preset_rx_ctle_peak2_addr   0b100111010
#define pcie_lane4_gen3_preset_rx_ctle_peak2_startbit   4
#define pcie_lane4_gen3_preset_rx_ctle_peak2_width   4
#define pcie_lane4_gen3_preset_rx_ctle_peak2_endbit   7
#define pcie_lane4_gen3_preset_rx_ctle_peak2_shift   8
#define pcie_lane4_gen3_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane4_gen3_preset_rx_lte_gain_addr    0b100111010
#define pcie_lane4_gen3_preset_rx_lte_gain_startbit   9
#define pcie_lane4_gen3_preset_rx_lte_gain_width   3
#define pcie_lane4_gen3_preset_rx_lte_gain_endbit   11
#define pcie_lane4_gen3_preset_rx_lte_gain_shift   4
#define pcie_lane4_gen3_preset_rx_lte_gain_mask    0x70


#define pcie_lane4_gen3_preset_rx_lte_zero_addr    0b100111010
#define pcie_lane4_gen3_preset_rx_lte_zero_startbit   13
#define pcie_lane4_gen3_preset_rx_lte_zero_width   3
#define pcie_lane4_gen3_preset_rx_lte_zero_endbit   15
#define pcie_lane4_gen3_preset_rx_lte_zero_shift   0
#define pcie_lane4_gen3_preset_rx_lte_zero_mask    0x7


#define pcie_lane4_gen4_preset_rx_ctle_peak1_addr   0b100111011
#define pcie_lane4_gen4_preset_rx_ctle_peak1_startbit   0
#define pcie_lane4_gen4_preset_rx_ctle_peak1_width   4
#define pcie_lane4_gen4_preset_rx_ctle_peak1_endbit   3
#define pcie_lane4_gen4_preset_rx_ctle_peak1_shift   12
#define pcie_lane4_gen4_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_addr   0b100111011
#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane4_gen4_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane4_gen4_preset_rx_ctle_peak2_addr   0b100111011
#define pcie_lane4_gen4_preset_rx_ctle_peak2_startbit   4
#define pcie_lane4_gen4_preset_rx_ctle_peak2_width   4
#define pcie_lane4_gen4_preset_rx_ctle_peak2_endbit   7
#define pcie_lane4_gen4_preset_rx_ctle_peak2_shift   8
#define pcie_lane4_gen4_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane4_gen4_preset_rx_lte_gain_addr    0b100111011
#define pcie_lane4_gen4_preset_rx_lte_gain_startbit   9
#define pcie_lane4_gen4_preset_rx_lte_gain_width   3
#define pcie_lane4_gen4_preset_rx_lte_gain_endbit   11
#define pcie_lane4_gen4_preset_rx_lte_gain_shift   4
#define pcie_lane4_gen4_preset_rx_lte_gain_mask    0x70


#define pcie_lane4_gen4_preset_rx_lte_zero_addr    0b100111011
#define pcie_lane4_gen4_preset_rx_lte_zero_startbit   13
#define pcie_lane4_gen4_preset_rx_lte_zero_width   3
#define pcie_lane4_gen4_preset_rx_lte_zero_endbit   15
#define pcie_lane4_gen4_preset_rx_lte_zero_shift   0
#define pcie_lane4_gen4_preset_rx_lte_zero_mask    0x7


#define pcie_lane4_gen5_preset_rx_ctle_peak1_addr   0b100111100
#define pcie_lane4_gen5_preset_rx_ctle_peak1_startbit   0
#define pcie_lane4_gen5_preset_rx_ctle_peak1_width   4
#define pcie_lane4_gen5_preset_rx_ctle_peak1_endbit   3
#define pcie_lane4_gen5_preset_rx_ctle_peak1_shift   12
#define pcie_lane4_gen5_preset_rx_ctle_peak1_mask   0xf000


#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_addr   0b100111100
#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_startbit   0
#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_width   8
#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_endbit   7
#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_shift   8
#define pcie_lane4_gen5_preset_rx_ctle_peak1_peak2_mask   0xff00


#define pcie_lane4_gen5_preset_rx_ctle_peak2_addr   0b100111100
#define pcie_lane4_gen5_preset_rx_ctle_peak2_startbit   4
#define pcie_lane4_gen5_preset_rx_ctle_peak2_width   4
#define pcie_lane4_gen5_preset_rx_ctle_peak2_endbit   7
#define pcie_lane4_gen5_preset_rx_ctle_peak2_shift   8
#define pcie_lane4_gen5_preset_rx_ctle_peak2_mask   0xf00


#define pcie_lane4_gen5_preset_rx_lte_gain_addr    0b100111100
#define pcie_lane4_gen5_preset_rx_lte_gain_startbit   9
#define pcie_lane4_gen5_preset_rx_lte_gain_width   3
#define pcie_lane4_gen5_preset_rx_lte_gain_endbit   11
#define pcie_lane4_gen5_preset_rx_lte_gain_shift   4
#define pcie_lane4_gen5_preset_rx_lte_gain_mask    0x70


#define pcie_lane4_gen5_preset_rx_lte_zero_addr    0b100111100
#define pcie_lane4_gen5_preset_rx_lte_zero_startbit   13
#define pcie_lane4_gen5_preset_rx_lte_zero_width   3
#define pcie_lane4_gen5_preset_rx_lte_zero_endbit   15
#define pcie_lane4_gen5_preset_rx_lte_zero_shift   0
#define pcie_lane4_gen5_preset_rx_lte_zero_mask    0x7


#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_addr   0b100011000
#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_width   16
#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen1_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_addr   0b100011001
#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_width   16
#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen2_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_addr   0b100011010
#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_width   16
#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen3_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_addr   0b100011011
#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_width   16
#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen4_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_addr   0b100011100
#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_width   16
#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane0_gen5_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_addr   0b100100000
#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_width   16
#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen1_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_addr   0b100100001
#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_width   16
#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen2_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_addr   0b100100010
#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_width   16
#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen3_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_addr   0b100100011
#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_width   16
#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen4_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_addr   0b100100100
#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_width   16
#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane1_gen5_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_addr   0b100101000
#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_width   16
#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen1_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_addr   0b100101001
#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_width   16
#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen2_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_addr   0b100101010
#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_width   16
#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen3_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_addr   0b100101011
#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_width   16
#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen4_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_addr   0b100101100
#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_width   16
#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane2_gen5_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_addr   0b100110000
#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_width   16
#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen1_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_addr   0b100110001
#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_width   16
#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen2_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_addr   0b100110010
#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_width   16
#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen3_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_addr   0b100110011
#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_width   16
#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen4_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_addr   0b100110100
#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_width   16
#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane3_gen5_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_addr   0b100111000
#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_width   16
#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen1_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_addr   0b100111001
#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_width   16
#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen2_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_addr   0b100111010
#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_width   16
#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen3_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_addr   0b100111011
#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_width   16
#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen4_mask   0xffff


#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_addr   0b100111100
#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_startbit   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_width   16
#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_endbit   15
#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_shift   0
#define pcie_ppe_preset_rx_peak_lte_lane4_gen5_mask   0xffff


#define pcie_ppe_saved_rx_ad_loff_lane_0_addr      0b001010000
#define pcie_ppe_saved_rx_ad_loff_lane_0_startbit   0
#define pcie_ppe_saved_rx_ad_loff_lane_0_width     8
#define pcie_ppe_saved_rx_ad_loff_lane_0_endbit    7
#define pcie_ppe_saved_rx_ad_loff_lane_0_shift     8
#define pcie_ppe_saved_rx_ad_loff_lane_0_mask      0xff00


#define pcie_ppe_saved_rx_ad_loff_lane_1_addr      0b001110000
#define pcie_ppe_saved_rx_ad_loff_lane_1_startbit   0
#define pcie_ppe_saved_rx_ad_loff_lane_1_width     8
#define pcie_ppe_saved_rx_ad_loff_lane_1_endbit    7
#define pcie_ppe_saved_rx_ad_loff_lane_1_shift     8
#define pcie_ppe_saved_rx_ad_loff_lane_1_mask      0xff00


#define pcie_ppe_saved_rx_ad_loff_lane_2_addr      0b010010000
#define pcie_ppe_saved_rx_ad_loff_lane_2_startbit   0
#define pcie_ppe_saved_rx_ad_loff_lane_2_width     8
#define pcie_ppe_saved_rx_ad_loff_lane_2_endbit    7
#define pcie_ppe_saved_rx_ad_loff_lane_2_shift     8
#define pcie_ppe_saved_rx_ad_loff_lane_2_mask      0xff00


#define pcie_ppe_saved_rx_ad_loff_lane_3_addr      0b010110000
#define pcie_ppe_saved_rx_ad_loff_lane_3_startbit   0
#define pcie_ppe_saved_rx_ad_loff_lane_3_width     8
#define pcie_ppe_saved_rx_ad_loff_lane_3_endbit    7
#define pcie_ppe_saved_rx_ad_loff_lane_3_shift     8
#define pcie_ppe_saved_rx_ad_loff_lane_3_mask      0xff00


#define pcie_ppe_saved_rx_ad_loff_lane_4_addr      0b011010000
#define pcie_ppe_saved_rx_ad_loff_lane_4_startbit   0
#define pcie_ppe_saved_rx_ad_loff_lane_4_width     8
#define pcie_ppe_saved_rx_ad_loff_lane_4_endbit    7
#define pcie_ppe_saved_rx_ad_loff_lane_4_shift     8
#define pcie_ppe_saved_rx_ad_loff_lane_4_mask      0xff00


#define pcie_ppe_saved_rx_bd_loff_lane_0_addr      0b001100000
#define pcie_ppe_saved_rx_bd_loff_lane_0_startbit   0
#define pcie_ppe_saved_rx_bd_loff_lane_0_width     8
#define pcie_ppe_saved_rx_bd_loff_lane_0_endbit    7
#define pcie_ppe_saved_rx_bd_loff_lane_0_shift     8
#define pcie_ppe_saved_rx_bd_loff_lane_0_mask      0xff00


#define pcie_ppe_saved_rx_bd_loff_lane_1_addr      0b010000000
#define pcie_ppe_saved_rx_bd_loff_lane_1_startbit   0
#define pcie_ppe_saved_rx_bd_loff_lane_1_width     8
#define pcie_ppe_saved_rx_bd_loff_lane_1_endbit    7
#define pcie_ppe_saved_rx_bd_loff_lane_1_shift     8
#define pcie_ppe_saved_rx_bd_loff_lane_1_mask      0xff00


#define pcie_ppe_saved_rx_bd_loff_lane_2_addr      0b010100000
#define pcie_ppe_saved_rx_bd_loff_lane_2_startbit   0
#define pcie_ppe_saved_rx_bd_loff_lane_2_width     8
#define pcie_ppe_saved_rx_bd_loff_lane_2_endbit    7
#define pcie_ppe_saved_rx_bd_loff_lane_2_shift     8
#define pcie_ppe_saved_rx_bd_loff_lane_2_mask      0xff00


#define pcie_ppe_saved_rx_bd_loff_lane_3_addr      0b011000000
#define pcie_ppe_saved_rx_bd_loff_lane_3_startbit   0
#define pcie_ppe_saved_rx_bd_loff_lane_3_width     8
#define pcie_ppe_saved_rx_bd_loff_lane_3_endbit    7
#define pcie_ppe_saved_rx_bd_loff_lane_3_shift     8
#define pcie_ppe_saved_rx_bd_loff_lane_3_mask      0xff00


#define pcie_ppe_saved_rx_bd_loff_lane_4_addr      0b011100000
#define pcie_ppe_saved_rx_bd_loff_lane_4_startbit   0
#define pcie_ppe_saved_rx_bd_loff_lane_4_width     8
#define pcie_ppe_saved_rx_bd_loff_lane_4_endbit    7
#define pcie_ppe_saved_rx_bd_loff_lane_4_shift     8
#define pcie_ppe_saved_rx_bd_loff_lane_4_mask      0xff00


#define pcie_ppe_saved_tx_dcc_lane_0_addr          0b011110000
#define pcie_ppe_saved_tx_dcc_lane_0_startbit      0
#define pcie_ppe_saved_tx_dcc_lane_0_width         16
#define pcie_ppe_saved_tx_dcc_lane_0_endbit        15
#define pcie_ppe_saved_tx_dcc_lane_0_shift         0
#define pcie_ppe_saved_tx_dcc_lane_0_mask          0xffff


#define pcie_ppe_saved_tx_dcc_lane_1_addr          0b011111000
#define pcie_ppe_saved_tx_dcc_lane_1_startbit      0
#define pcie_ppe_saved_tx_dcc_lane_1_width         16
#define pcie_ppe_saved_tx_dcc_lane_1_endbit        15
#define pcie_ppe_saved_tx_dcc_lane_1_shift         0
#define pcie_ppe_saved_tx_dcc_lane_1_mask          0xffff


#define pcie_ppe_saved_tx_dcc_lane_2_addr          0b100000000
#define pcie_ppe_saved_tx_dcc_lane_2_startbit      0
#define pcie_ppe_saved_tx_dcc_lane_2_width         16
#define pcie_ppe_saved_tx_dcc_lane_2_endbit        15
#define pcie_ppe_saved_tx_dcc_lane_2_shift         0
#define pcie_ppe_saved_tx_dcc_lane_2_mask          0xffff


#define pcie_ppe_saved_tx_dcc_lane_3_addr          0b100001000
#define pcie_ppe_saved_tx_dcc_lane_3_startbit      0
#define pcie_ppe_saved_tx_dcc_lane_3_width         16
#define pcie_ppe_saved_tx_dcc_lane_3_endbit        15
#define pcie_ppe_saved_tx_dcc_lane_3_shift         0
#define pcie_ppe_saved_tx_dcc_lane_3_mask          0xffff


#define pcie_ppe_saved_tx_dcc_lane_4_addr          0b100010000
#define pcie_ppe_saved_tx_dcc_lane_4_startbit      0
#define pcie_ppe_saved_tx_dcc_lane_4_width         16
#define pcie_ppe_saved_tx_dcc_lane_4_endbit        15
#define pcie_ppe_saved_tx_dcc_lane_4_shift         0
#define pcie_ppe_saved_tx_dcc_lane_4_mask          0xffff


#define pcie_ppe_txdetrx_dac_cntl_lane_0_addr      0b101000001
#define pcie_ppe_txdetrx_dac_cntl_lane_0_startbit   0
#define pcie_ppe_txdetrx_dac_cntl_lane_0_width     8
#define pcie_ppe_txdetrx_dac_cntl_lane_0_endbit    7
#define pcie_ppe_txdetrx_dac_cntl_lane_0_shift     8
#define pcie_ppe_txdetrx_dac_cntl_lane_0_mask      0xff00


#define pcie_ppe_txdetrx_dac_cntl_lane_1_addr      0b101000011
#define pcie_ppe_txdetrx_dac_cntl_lane_1_startbit   0
#define pcie_ppe_txdetrx_dac_cntl_lane_1_width     8
#define pcie_ppe_txdetrx_dac_cntl_lane_1_endbit    7
#define pcie_ppe_txdetrx_dac_cntl_lane_1_shift     8
#define pcie_ppe_txdetrx_dac_cntl_lane_1_mask      0xff00


#define pcie_ppe_txdetrx_dac_cntl_lane_2_addr      0b101000101
#define pcie_ppe_txdetrx_dac_cntl_lane_2_startbit   0
#define pcie_ppe_txdetrx_dac_cntl_lane_2_width     8
#define pcie_ppe_txdetrx_dac_cntl_lane_2_endbit    7
#define pcie_ppe_txdetrx_dac_cntl_lane_2_shift     8
#define pcie_ppe_txdetrx_dac_cntl_lane_2_mask      0xff00


#define pcie_ppe_txdetrx_dac_cntl_lane_3_addr      0b101000111
#define pcie_ppe_txdetrx_dac_cntl_lane_3_startbit   0
#define pcie_ppe_txdetrx_dac_cntl_lane_3_width     8
#define pcie_ppe_txdetrx_dac_cntl_lane_3_endbit    7
#define pcie_ppe_txdetrx_dac_cntl_lane_3_shift     8
#define pcie_ppe_txdetrx_dac_cntl_lane_3_mask      0xff00


#define pcie_ppe_txdetrx_dac_cntl_lane_4_addr      0b101001001
#define pcie_ppe_txdetrx_dac_cntl_lane_4_startbit   0
#define pcie_ppe_txdetrx_dac_cntl_lane_4_width     8
#define pcie_ppe_txdetrx_dac_cntl_lane_4_endbit    7
#define pcie_ppe_txdetrx_dac_cntl_lane_4_shift     8
#define pcie_ppe_txdetrx_dac_cntl_lane_4_mask      0xff00


#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_addr   0b101000000
#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_startbit   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_width   16
#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_endbit   15
#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_shift   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_0_mask   0xffff


#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_addr   0b101000010
#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_startbit   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_width   16
#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_endbit   15
#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_shift   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_1_mask   0xffff


#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_addr   0b101000100
#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_startbit   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_width   16
#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_endbit   15
#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_shift   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_2_mask   0xffff


#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_addr   0b101000110
#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_startbit   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_width   16
#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_endbit   15
#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_shift   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_3_mask   0xffff


#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_addr   0b101001000
#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_startbit   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_width   16
#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_endbit   15
#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_shift   0
#define pcie_ppe_txdetrx_idle_samp_timer_lane_4_mask   0xffff


#define pcie_ppe_txdetrx_idle_timer_lane_0_addr    0b101000000
#define pcie_ppe_txdetrx_idle_timer_lane_0_startbit   0
#define pcie_ppe_txdetrx_idle_timer_lane_0_width   3
#define pcie_ppe_txdetrx_idle_timer_lane_0_endbit   2
#define pcie_ppe_txdetrx_idle_timer_lane_0_shift   13
#define pcie_ppe_txdetrx_idle_timer_lane_0_mask    0xe000


#define pcie_ppe_txdetrx_idle_timer_lane_1_addr    0b101000010
#define pcie_ppe_txdetrx_idle_timer_lane_1_startbit   0
#define pcie_ppe_txdetrx_idle_timer_lane_1_width   3
#define pcie_ppe_txdetrx_idle_timer_lane_1_endbit   2
#define pcie_ppe_txdetrx_idle_timer_lane_1_shift   13
#define pcie_ppe_txdetrx_idle_timer_lane_1_mask    0xe000


#define pcie_ppe_txdetrx_idle_timer_lane_2_addr    0b101000100
#define pcie_ppe_txdetrx_idle_timer_lane_2_startbit   0
#define pcie_ppe_txdetrx_idle_timer_lane_2_width   3
#define pcie_ppe_txdetrx_idle_timer_lane_2_endbit   2
#define pcie_ppe_txdetrx_idle_timer_lane_2_shift   13
#define pcie_ppe_txdetrx_idle_timer_lane_2_mask    0xe000


#define pcie_ppe_txdetrx_idle_timer_lane_3_addr    0b101000110
#define pcie_ppe_txdetrx_idle_timer_lane_3_startbit   0
#define pcie_ppe_txdetrx_idle_timer_lane_3_width   3
#define pcie_ppe_txdetrx_idle_timer_lane_3_endbit   2
#define pcie_ppe_txdetrx_idle_timer_lane_3_shift   13
#define pcie_ppe_txdetrx_idle_timer_lane_3_mask    0xe000


#define pcie_ppe_txdetrx_idle_timer_lane_4_addr    0b101001000
#define pcie_ppe_txdetrx_idle_timer_lane_4_startbit   0
#define pcie_ppe_txdetrx_idle_timer_lane_4_width   3
#define pcie_ppe_txdetrx_idle_timer_lane_4_endbit   2
#define pcie_ppe_txdetrx_idle_timer_lane_4_shift   13
#define pcie_ppe_txdetrx_idle_timer_lane_4_mask    0xe000


#define pcie_ppe_txdetrx_samp_timer_lane_0_addr    0b101000000
#define pcie_ppe_txdetrx_samp_timer_lane_0_startbit   8
#define pcie_ppe_txdetrx_samp_timer_lane_0_width   8
#define pcie_ppe_txdetrx_samp_timer_lane_0_endbit   15
#define pcie_ppe_txdetrx_samp_timer_lane_0_shift   0
#define pcie_ppe_txdetrx_samp_timer_lane_0_mask    0xff


#define pcie_ppe_txdetrx_samp_timer_lane_1_addr    0b101000010
#define pcie_ppe_txdetrx_samp_timer_lane_1_startbit   8
#define pcie_ppe_txdetrx_samp_timer_lane_1_width   8
#define pcie_ppe_txdetrx_samp_timer_lane_1_endbit   15
#define pcie_ppe_txdetrx_samp_timer_lane_1_shift   0
#define pcie_ppe_txdetrx_samp_timer_lane_1_mask    0xff


#define pcie_ppe_txdetrx_samp_timer_lane_2_addr    0b101000100
#define pcie_ppe_txdetrx_samp_timer_lane_2_startbit   8
#define pcie_ppe_txdetrx_samp_timer_lane_2_width   8
#define pcie_ppe_txdetrx_samp_timer_lane_2_endbit   15
#define pcie_ppe_txdetrx_samp_timer_lane_2_shift   0
#define pcie_ppe_txdetrx_samp_timer_lane_2_mask    0xff


#define pcie_ppe_txdetrx_samp_timer_lane_3_addr    0b101000110
#define pcie_ppe_txdetrx_samp_timer_lane_3_startbit   8
#define pcie_ppe_txdetrx_samp_timer_lane_3_width   8
#define pcie_ppe_txdetrx_samp_timer_lane_3_endbit   15
#define pcie_ppe_txdetrx_samp_timer_lane_3_shift   0
#define pcie_ppe_txdetrx_samp_timer_lane_3_mask    0xff


#define pcie_ppe_txdetrx_samp_timer_lane_4_addr    0b101001000
#define pcie_ppe_txdetrx_samp_timer_lane_4_startbit   8
#define pcie_ppe_txdetrx_samp_timer_lane_4_width   8
#define pcie_ppe_txdetrx_samp_timer_lane_4_endbit   15
#define pcie_ppe_txdetrx_samp_timer_lane_4_shift   0
#define pcie_ppe_txdetrx_samp_timer_lane_4_mask    0xff


#define poff_avg_a_addr                            0b000000110
#define poff_avg_a_startbit                        0
#define poff_avg_a_width                           8
#define poff_avg_a_endbit                          7
#define poff_avg_a_shift                           8
#define poff_avg_a_mask                            0xff00


#define poff_avg_b_addr                            0b000000110
#define poff_avg_b_startbit                        8
#define poff_avg_b_width                           8
#define poff_avg_b_endbit                          15
#define poff_avg_b_shift                           0
#define poff_avg_b_mask                            0xff


#define ppe_channel_loss_addr                      0b110000100
#define ppe_channel_loss_startbit                  12
#define ppe_channel_loss_width                     2
#define ppe_channel_loss_endbit                    13
#define ppe_channel_loss_shift                     2
#define ppe_channel_loss_mask                      0xc


#define ppe_data_rate_addr                         0b110000100
#define ppe_data_rate_startbit                     9
#define ppe_data_rate_width                        3
#define ppe_data_rate_endbit                       11
#define ppe_data_rate_shift                        4
#define ppe_data_rate_mask                         0x70


#define ppe_debug_state_addr                       0b110110000
#define ppe_debug_state_startbit                   0
#define ppe_debug_state_width                      16
#define ppe_debug_state_endbit                     15
#define ppe_debug_state_shift                      0
#define ppe_debug_state_mask                       0xffff


#define ppe_debug_stopwatch_time_us_addr           0b111001010
#define ppe_debug_stopwatch_time_us_startbit       0
#define ppe_debug_stopwatch_time_us_width          16
#define ppe_debug_stopwatch_time_us_endbit         15
#define ppe_debug_stopwatch_time_us_shift          0
#define ppe_debug_stopwatch_time_us_mask           0xffff


#define ppe_eoff_edge_hysteresis_addr              0b110001011
#define ppe_eoff_edge_hysteresis_startbit          3
#define ppe_eoff_edge_hysteresis_width             3
#define ppe_eoff_edge_hysteresis_endbit            5
#define ppe_eoff_edge_hysteresis_shift             10
#define ppe_eoff_edge_hysteresis_mask              0x1c00


#define ppe_error_lane_addr                        0b110111001
#define ppe_error_lane_startbit                    8
#define ppe_error_lane_width                       5
#define ppe_error_lane_endbit                      12
#define ppe_error_lane_shift                       3
#define ppe_error_lane_mask                        0xf8


#define ppe_error_state_addr                       0b110111000
#define ppe_error_state_startbit                   0
#define ppe_error_state_width                      16
#define ppe_error_state_endbit                     15
#define ppe_error_state_shift                      0
#define ppe_error_state_mask                       0xffff


#define ppe_error_thread_addr                      0b110111001
#define ppe_error_thread_startbit                  4
#define ppe_error_thread_width                     4
#define ppe_error_thread_endbit                    7
#define ppe_error_thread_shift                     8
#define ppe_error_thread_mask                      0xf00


#define ppe_error_type_addr                        0b110111001
#define ppe_error_type_startbit                    1
#define ppe_error_type_width                       3
#define ppe_error_type_endbit                      3
#define ppe_error_type_shift                       12
#define ppe_error_type_mask                        0x7000


#define ppe_error_valid_addr                       0b110111001
#define ppe_error_valid_startbit                   0
#define ppe_error_valid_width                      1
#define ppe_error_valid_endbit                     0
#define ppe_error_valid_shift                      15
#define ppe_error_valid_mask                       0x8000


#define ppe_init_time_us_warning_addr              0b110110100
#define ppe_init_time_us_warning_startbit          2
#define ppe_init_time_us_warning_width             1
#define ppe_init_time_us_warning_endbit            2
#define ppe_init_time_us_warning_shift             13
#define ppe_init_time_us_warning_mask              0x2000


#define ppe_last_cal_time_us_addr                  0b111000111
#define ppe_last_cal_time_us_startbit              0
#define ppe_last_cal_time_us_width                 16
#define ppe_last_cal_time_us_endbit                15
#define ppe_last_cal_time_us_shift                 0
#define ppe_last_cal_time_us_mask                  0xffff


#define ppe_loff_offset_applied_a_addr             0b000000101
#define ppe_loff_offset_applied_a_startbit         4
#define ppe_loff_offset_applied_a_width            1
#define ppe_loff_offset_applied_a_endbit           4
#define ppe_loff_offset_applied_a_shift            11
#define ppe_loff_offset_applied_a_mask             0x800


#define ppe_loff_offset_applied_b_addr             0b000000101
#define ppe_loff_offset_applied_b_startbit         5
#define ppe_loff_offset_applied_b_width            1
#define ppe_loff_offset_applied_b_endbit           5
#define ppe_loff_offset_applied_b_shift            10
#define ppe_loff_offset_applied_b_mask             0x400


#define ppe_loff_offset_d_override_addr            0b111101111
#define ppe_loff_offset_d_override_startbit        0
#define ppe_loff_offset_d_override_width           7
#define ppe_loff_offset_d_override_endbit          6
#define ppe_loff_offset_d_override_shift           9
#define ppe_loff_offset_d_override_mask            0xfe00


#define ppe_loff_offset_e_override_addr            0b111101111
#define ppe_loff_offset_e_override_startbit        8
#define ppe_loff_offset_e_override_width           7
#define ppe_loff_offset_e_override_endbit          14
#define ppe_loff_offset_e_override_shift           1
#define ppe_loff_offset_e_override_mask            0xfe


#define ppe_loff_offset_pause_addr                 0b111101111
#define ppe_loff_offset_pause_startbit             15
#define ppe_loff_offset_pause_width                1
#define ppe_loff_offset_pause_endbit               15
#define ppe_loff_offset_pause_shift                0
#define ppe_loff_offset_pause_mask                 0x1


#define ppe_lte_gain_disable_addr                  0b110001011
#define ppe_lte_gain_disable_startbit              13
#define ppe_lte_gain_disable_width                 1
#define ppe_lte_gain_disable_endbit                13
#define ppe_lte_gain_disable_shift                 2
#define ppe_lte_gain_disable_mask                  0x4


#define ppe_lte_gain_zero_disable_alias_addr       0b110001011
#define ppe_lte_gain_zero_disable_alias_startbit   13
#define ppe_lte_gain_zero_disable_alias_width      2
#define ppe_lte_gain_zero_disable_alias_endbit     14
#define ppe_lte_gain_zero_disable_alias_shift      1
#define ppe_lte_gain_zero_disable_alias_mask       0x6


#define ppe_lte_hysteresis_addr                    0b110001011
#define ppe_lte_hysteresis_startbit                0
#define ppe_lte_hysteresis_width                   3
#define ppe_lte_hysteresis_endbit                  2
#define ppe_lte_hysteresis_shift                   13
#define ppe_lte_hysteresis_mask                    0xe000


#define ppe_lte_zero_disable_addr                  0b110001011
#define ppe_lte_zero_disable_startbit              14
#define ppe_lte_zero_disable_width                 1
#define ppe_lte_zero_disable_endbit                14
#define ppe_lte_zero_disable_shift                 1
#define ppe_lte_zero_disable_mask                  0x2


#define ppe_margin_offset_overlay_addr             0b000000101
#define ppe_margin_offset_overlay_startbit         2
#define ppe_margin_offset_overlay_width            4
#define ppe_margin_offset_overlay_endbit           5
#define ppe_margin_offset_overlay_shift            10
#define ppe_margin_offset_overlay_mask             0x3c00


#define ppe_offset_margin_mode_addr                0b110101010
#define ppe_offset_margin_mode_startbit            11
#define ppe_offset_margin_mode_width               2
#define ppe_offset_margin_mode_endbit              12
#define ppe_offset_margin_mode_shift               3
#define ppe_offset_margin_mode_mask                0x18


#define ppe_pipe_current_lane_addr                 0b110110110
#define ppe_pipe_current_lane_startbit             8
#define ppe_pipe_current_lane_width                2
#define ppe_pipe_current_lane_endbit               9
#define ppe_pipe_current_lane_shift                6
#define ppe_pipe_current_lane_mask                 0xc0


#define ppe_pipe_ei_reset_inactive_phase_alias_addr   0b110110111
#define ppe_pipe_ei_reset_inactive_phase_alias_startbit   0
#define ppe_pipe_ei_reset_inactive_phase_alias_width   16
#define ppe_pipe_ei_reset_inactive_phase_alias_endbit   15
#define ppe_pipe_ei_reset_inactive_phase_alias_shift   0
#define ppe_pipe_ei_reset_inactive_phase_alias_mask   0xffff


#define ppe_pipe_lane_stress_mode_0_3_addr         0b110000110
#define ppe_pipe_lane_stress_mode_0_3_startbit     8
#define ppe_pipe_lane_stress_mode_0_3_width        4
#define ppe_pipe_lane_stress_mode_0_3_endbit       11
#define ppe_pipe_lane_stress_mode_0_3_shift        4
#define ppe_pipe_lane_stress_mode_0_3_mask         0xf0


#define ppe_pipe_margin_mode_0_3_addr              0b111001000
#define ppe_pipe_margin_mode_0_3_startbit          0
#define ppe_pipe_margin_mode_0_3_width             4
#define ppe_pipe_margin_mode_0_3_endbit            3
#define ppe_pipe_margin_mode_0_3_shift             12
#define ppe_pipe_margin_mode_0_3_mask              0xf000


#define ppe_pipe_reset_active_phase_0_3_addr       0b110110110
#define ppe_pipe_reset_active_phase_0_3_startbit   0
#define ppe_pipe_reset_active_phase_0_3_width      4
#define ppe_pipe_reset_active_phase_0_3_endbit     3
#define ppe_pipe_reset_active_phase_0_3_shift      12
#define ppe_pipe_reset_active_phase_0_3_mask       0xf000


#define ppe_pipe_reset_active_txdetectrx_phase_alias_addr   0b110110110
#define ppe_pipe_reset_active_txdetectrx_phase_alias_startbit   0
#define ppe_pipe_reset_active_txdetectrx_phase_alias_width   8
#define ppe_pipe_reset_active_txdetectrx_phase_alias_endbit   7
#define ppe_pipe_reset_active_txdetectrx_phase_alias_shift   8
#define ppe_pipe_reset_active_txdetectrx_phase_alias_mask   0xff00


#define ppe_pipe_reset_inactive_phase_0_3_addr     0b110110111
#define ppe_pipe_reset_inactive_phase_0_3_startbit   8
#define ppe_pipe_reset_inactive_phase_0_3_width    8
#define ppe_pipe_reset_inactive_phase_0_3_endbit   15
#define ppe_pipe_reset_inactive_phase_0_3_shift    0
#define ppe_pipe_reset_inactive_phase_0_3_mask     0xff


#define ppe_pipe_rx_ei_inactive_phase_0_3_addr     0b110110111
#define ppe_pipe_rx_ei_inactive_phase_0_3_startbit   0
#define ppe_pipe_rx_ei_inactive_phase_0_3_width    8
#define ppe_pipe_rx_ei_inactive_phase_0_3_endbit   7
#define ppe_pipe_rx_ei_inactive_phase_0_3_shift    8
#define ppe_pipe_rx_ei_inactive_phase_0_3_mask     0xff00


#define ppe_pipe_txdetectrx_phase_0_3_addr         0b110110110
#define ppe_pipe_txdetectrx_phase_0_3_startbit     4
#define ppe_pipe_txdetectrx_phase_0_3_width        4
#define ppe_pipe_txdetectrx_phase_0_3_endbit       7
#define ppe_pipe_txdetectrx_phase_0_3_shift        8
#define ppe_pipe_txdetectrx_phase_0_3_mask         0xf00


#define ppe_pr_offset_applied_a_addr               0b000000101
#define ppe_pr_offset_applied_a_startbit           2
#define ppe_pr_offset_applied_a_width              1
#define ppe_pr_offset_applied_a_endbit             2
#define ppe_pr_offset_applied_a_shift              13
#define ppe_pr_offset_applied_a_mask               0x2000


#define ppe_pr_offset_applied_ab_alias_addr        0b000000101
#define ppe_pr_offset_applied_ab_alias_startbit    2
#define ppe_pr_offset_applied_ab_alias_width       2
#define ppe_pr_offset_applied_ab_alias_endbit      3
#define ppe_pr_offset_applied_ab_alias_shift       12
#define ppe_pr_offset_applied_ab_alias_mask        0x3000


#define ppe_pr_offset_applied_b_addr               0b000000101
#define ppe_pr_offset_applied_b_startbit           3
#define ppe_pr_offset_applied_b_width              1
#define ppe_pr_offset_applied_b_endbit             3
#define ppe_pr_offset_applied_b_shift              12
#define ppe_pr_offset_applied_b_mask               0x1000


#define ppe_pr_offset_d_override_addr              0b110101010
#define ppe_pr_offset_d_override_startbit          0
#define ppe_pr_offset_d_override_width             5
#define ppe_pr_offset_d_override_endbit            4
#define ppe_pr_offset_d_override_shift             11
#define ppe_pr_offset_d_override_mask              0xf800


#define ppe_pr_offset_e_override_addr              0b110101010
#define ppe_pr_offset_e_override_startbit          5
#define ppe_pr_offset_e_override_width             5
#define ppe_pr_offset_e_override_endbit            9
#define ppe_pr_offset_e_override_shift             6
#define ppe_pr_offset_e_override_mask              0x7c0


#define ppe_pr_offset_pause_addr                   0b110101010
#define ppe_pr_offset_pause_startbit               10
#define ppe_pr_offset_pause_width                  1
#define ppe_pr_offset_pause_endbit                 10
#define ppe_pr_offset_pause_shift                  5
#define ppe_pr_offset_pause_mask                   0x20


#define ppe_recal_not_run_disable_addr             0b110000100
#define ppe_recal_not_run_disable_startbit         14
#define ppe_recal_not_run_disable_width            1
#define ppe_recal_not_run_disable_endbit           14
#define ppe_recal_not_run_disable_shift            1
#define ppe_recal_not_run_disable_mask             0x2


#define ppe_recal_not_run_sim_mode_addr            0b110000100
#define ppe_recal_not_run_sim_mode_startbit        4
#define ppe_recal_not_run_sim_mode_width           4
#define ppe_recal_not_run_sim_mode_endbit          7
#define ppe_recal_not_run_sim_mode_shift           8
#define ppe_recal_not_run_sim_mode_mask            0xf00


#define ppe_servo_status0_addr                     0b110111010
#define ppe_servo_status0_startbit                 0
#define ppe_servo_status0_width                    16
#define ppe_servo_status0_endbit                   15
#define ppe_servo_status0_shift                    0
#define ppe_servo_status0_mask                     0xffff


#define ppe_servo_status1_addr                     0b110111011
#define ppe_servo_status1_startbit                 0
#define ppe_servo_status1_width                    16
#define ppe_servo_status1_endbit                   15
#define ppe_servo_status1_shift                    0
#define ppe_servo_status1_mask                     0xffff


#define ppe_thread_active_time_us_limit_addr       0b110110110
#define ppe_thread_active_time_us_limit_startbit   10
#define ppe_thread_active_time_us_limit_width      6
#define ppe_thread_active_time_us_limit_endbit     15
#define ppe_thread_active_time_us_limit_shift      0
#define ppe_thread_active_time_us_limit_mask       0x3f


#define ppe_thread_last_active_time_us_addr        0b110110101
#define ppe_thread_last_active_time_us_startbit    0
#define ppe_thread_last_active_time_us_width       16
#define ppe_thread_last_active_time_us_endbit      15
#define ppe_thread_last_active_time_us_shift       0
#define ppe_thread_last_active_time_us_mask        0xffff


#define ppe_thread_lock_sim_mode_addr              0b110000100
#define ppe_thread_lock_sim_mode_startbit          0
#define ppe_thread_lock_sim_mode_width             4
#define ppe_thread_lock_sim_mode_endbit            3
#define ppe_thread_lock_sim_mode_shift             12
#define ppe_thread_lock_sim_mode_mask              0xf000


#define ppe_thread_loop_count_addr                 0b110110001
#define ppe_thread_loop_count_startbit             0
#define ppe_thread_loop_count_width                16
#define ppe_thread_loop_count_endbit               15
#define ppe_thread_loop_count_shift                0
#define ppe_thread_loop_count_mask                 0xffff


#define ppe_thread_time_stress_mode_addr           0b110000100
#define ppe_thread_time_stress_mode_startbit       8
#define ppe_thread_time_stress_mode_width          1
#define ppe_thread_time_stress_mode_endbit         8
#define ppe_thread_time_stress_mode_shift          7
#define ppe_thread_time_stress_mode_mask           0x80


#define rx_a_bad_dfe_conv_addr                     0b000000000
#define rx_a_bad_dfe_conv_startbit                 8
#define rx_a_bad_dfe_conv_width                    1
#define rx_a_bad_dfe_conv_endbit                   8
#define rx_a_bad_dfe_conv_shift                    7
#define rx_a_bad_dfe_conv_mask                     0x80


#define rx_a_before_loff_n000_addr                 0b000001110
#define rx_a_before_loff_n000_startbit             0
#define rx_a_before_loff_n000_width                8
#define rx_a_before_loff_n000_endbit               7
#define rx_a_before_loff_n000_shift                8
#define rx_a_before_loff_n000_mask                 0xff00


#define rx_a_ber_done_addr                         0b000000010
#define rx_a_ber_done_startbit                     12
#define rx_a_ber_done_width                        1
#define rx_a_ber_done_endbit                       12
#define rx_a_ber_done_shift                        3
#define rx_a_ber_done_mask                         0x8


#define rx_a_ctle_gain_done_addr                   0b000000010
#define rx_a_ctle_gain_done_startbit               1
#define rx_a_ctle_gain_done_width                  1
#define rx_a_ctle_gain_done_endbit                 1
#define rx_a_ctle_gain_done_shift                  14
#define rx_a_ctle_gain_done_mask                   0x4000


#define rx_a_ctle_peak1_done_addr                  0b000000010
#define rx_a_ctle_peak1_done_startbit              4
#define rx_a_ctle_peak1_done_width                 1
#define rx_a_ctle_peak1_done_endbit                4
#define rx_a_ctle_peak1_done_shift                 11
#define rx_a_ctle_peak1_done_mask                  0x800


#define rx_a_ctle_peak1_hyst_addr                  0b000000100
#define rx_a_ctle_peak1_hyst_startbit              0
#define rx_a_ctle_peak1_hyst_width                 4
#define rx_a_ctle_peak1_hyst_endbit                3
#define rx_a_ctle_peak1_hyst_shift                 12
#define rx_a_ctle_peak1_hyst_mask                  0xf000


#define rx_a_ctle_peak1_peak2_hyst_alias_addr      0b000000100
#define rx_a_ctle_peak1_peak2_hyst_alias_startbit   0
#define rx_a_ctle_peak1_peak2_hyst_alias_width     8
#define rx_a_ctle_peak1_peak2_hyst_alias_endbit    7
#define rx_a_ctle_peak1_peak2_hyst_alias_shift     8
#define rx_a_ctle_peak1_peak2_hyst_alias_mask      0xff00


#define rx_a_ctle_peak2_done_addr                  0b000000010
#define rx_a_ctle_peak2_done_startbit              5
#define rx_a_ctle_peak2_done_width                 1
#define rx_a_ctle_peak2_done_endbit                5
#define rx_a_ctle_peak2_done_shift                 10
#define rx_a_ctle_peak2_done_mask                  0x400


#define rx_a_ctle_peak2_hyst_addr                  0b000000100
#define rx_a_ctle_peak2_hyst_startbit              4
#define rx_a_ctle_peak2_hyst_width                 4
#define rx_a_ctle_peak2_hyst_endbit                7
#define rx_a_ctle_peak2_hyst_shift                 8
#define rx_a_ctle_peak2_hyst_mask                  0xf00


#define rx_a_ddc_done_addr                         0b000000010
#define rx_a_ddc_done_startbit                     11
#define rx_a_ddc_done_width                        1
#define rx_a_ddc_done_endbit                       11
#define rx_a_ddc_done_shift                        4
#define rx_a_ddc_done_mask                         0x10


#define rx_a_ddc_hyst_left_edge_addr               0b000001000
#define rx_a_ddc_hyst_left_edge_startbit           0
#define rx_a_ddc_hyst_left_edge_width              5
#define rx_a_ddc_hyst_left_edge_endbit             4
#define rx_a_ddc_hyst_left_edge_shift              11
#define rx_a_ddc_hyst_left_edge_mask               0xf800


#define rx_a_ddc_hyst_right_edge_addr              0b000001000
#define rx_a_ddc_hyst_right_edge_startbit          5
#define rx_a_ddc_hyst_right_edge_width             5
#define rx_a_ddc_hyst_right_edge_endbit            9
#define rx_a_ddc_hyst_right_edge_shift             6
#define rx_a_ddc_hyst_right_edge_mask              0x7c0


#define rx_a_dfe_done_addr                         0b000000010
#define rx_a_dfe_done_startbit                     10
#define rx_a_dfe_done_width                        1
#define rx_a_dfe_done_endbit                       10
#define rx_a_dfe_done_shift                        5
#define rx_a_dfe_done_mask                         0x20


#define rx_a_dfe_h1_done_addr                      0b000000010
#define rx_a_dfe_h1_done_startbit                  9
#define rx_a_dfe_h1_done_width                     1
#define rx_a_dfe_h1_done_endbit                    9
#define rx_a_dfe_h1_done_shift                     6
#define rx_a_dfe_h1_done_mask                      0x40


#define rx_a_eoff_done_addr                        0b000000010
#define rx_a_eoff_done_startbit                    2
#define rx_a_eoff_done_width                       1
#define rx_a_eoff_done_endbit                      2
#define rx_a_eoff_done_shift                       13
#define rx_a_eoff_done_mask                        0x2000


#define rx_a_lane_hist_min_eye_width_addr          0b000001000
#define rx_a_lane_hist_min_eye_width_startbit      10
#define rx_a_lane_hist_min_eye_width_width         6
#define rx_a_lane_hist_min_eye_width_endbit        15
#define rx_a_lane_hist_min_eye_width_shift         0
#define rx_a_lane_hist_min_eye_width_mask          0x3f


#define rx_a_latch_offset_done_addr                0b000000010
#define rx_a_latch_offset_done_startbit            0
#define rx_a_latch_offset_done_width               1
#define rx_a_latch_offset_done_endbit              0
#define rx_a_latch_offset_done_shift               15
#define rx_a_latch_offset_done_mask                0x8000


#define rx_a_lte_gain_done_addr                    0b000000010
#define rx_a_lte_gain_done_startbit                6
#define rx_a_lte_gain_done_width                   1
#define rx_a_lte_gain_done_endbit                  6
#define rx_a_lte_gain_done_shift                   9
#define rx_a_lte_gain_done_mask                    0x200


#define rx_a_lte_zero_done_addr                    0b000000010
#define rx_a_lte_zero_done_startbit                7
#define rx_a_lte_zero_done_width                   1
#define rx_a_lte_zero_done_endbit                  7
#define rx_a_lte_zero_done_shift                   8
#define rx_a_lte_zero_done_mask                    0x100


#define rx_a_quad_phase_done_addr                  0b000000010
#define rx_a_quad_phase_done_startbit              3
#define rx_a_quad_phase_done_width                 1
#define rx_a_quad_phase_done_endbit                3
#define rx_a_quad_phase_done_shift                 12
#define rx_a_quad_phase_done_mask                  0x1000


#define rx_a_sigdet_done_addr                      0b000000010
#define rx_a_sigdet_done_startbit                  8
#define rx_a_sigdet_done_width                     1
#define rx_a_sigdet_done_endbit                    8
#define rx_a_sigdet_done_shift                     7
#define rx_a_sigdet_done_mask                      0x80


#define rx_a_step_done_alias_addr                  0b000000010
#define rx_a_step_done_alias_startbit              0
#define rx_a_step_done_alias_width                 13
#define rx_a_step_done_alias_endbit                12
#define rx_a_step_done_alias_shift                 3
#define rx_a_step_done_alias_mask                  0xfff8


#define rx_amp_gain_cnt_max_addr                   0b110000011
#define rx_amp_gain_cnt_max_startbit               8
#define rx_amp_gain_cnt_max_width                  4
#define rx_amp_gain_cnt_max_endbit                 11
#define rx_amp_gain_cnt_max_shift                  4
#define rx_amp_gain_cnt_max_mask                   0xf0


#define rx_b_before_loff_n000_addr                 0b000001110
#define rx_b_before_loff_n000_startbit             8
#define rx_b_before_loff_n000_width                8
#define rx_b_before_loff_n000_endbit               15
#define rx_b_before_loff_n000_shift                0
#define rx_b_before_loff_n000_mask                 0xff


#define rx_b_ber_done_addr                         0b000000011
#define rx_b_ber_done_startbit                     12
#define rx_b_ber_done_width                        1
#define rx_b_ber_done_endbit                       12
#define rx_b_ber_done_shift                        3
#define rx_b_ber_done_mask                         0x8


#define rx_b_ctle_gain_done_addr                   0b000000011
#define rx_b_ctle_gain_done_startbit               1
#define rx_b_ctle_gain_done_width                  1
#define rx_b_ctle_gain_done_endbit                 1
#define rx_b_ctle_gain_done_shift                  14
#define rx_b_ctle_gain_done_mask                   0x4000


#define rx_b_ctle_peak1_done_addr                  0b000000011
#define rx_b_ctle_peak1_done_startbit              4
#define rx_b_ctle_peak1_done_width                 1
#define rx_b_ctle_peak1_done_endbit                4
#define rx_b_ctle_peak1_done_shift                 11
#define rx_b_ctle_peak1_done_mask                  0x800


#define rx_b_ctle_peak1_hyst_addr                  0b000000100
#define rx_b_ctle_peak1_hyst_startbit              8
#define rx_b_ctle_peak1_hyst_width                 4
#define rx_b_ctle_peak1_hyst_endbit                11
#define rx_b_ctle_peak1_hyst_shift                 4
#define rx_b_ctle_peak1_hyst_mask                  0xf0


#define rx_b_ctle_peak1_peak2_hyst_alias_addr      0b000000100
#define rx_b_ctle_peak1_peak2_hyst_alias_startbit   8
#define rx_b_ctle_peak1_peak2_hyst_alias_width     8
#define rx_b_ctle_peak1_peak2_hyst_alias_endbit    15
#define rx_b_ctle_peak1_peak2_hyst_alias_shift     0
#define rx_b_ctle_peak1_peak2_hyst_alias_mask      0xff


#define rx_b_ctle_peak2_done_addr                  0b000000011
#define rx_b_ctle_peak2_done_startbit              5
#define rx_b_ctle_peak2_done_width                 1
#define rx_b_ctle_peak2_done_endbit                5
#define rx_b_ctle_peak2_done_shift                 10
#define rx_b_ctle_peak2_done_mask                  0x400


#define rx_b_ctle_peak2_hyst_addr                  0b000000100
#define rx_b_ctle_peak2_hyst_startbit              12
#define rx_b_ctle_peak2_hyst_width                 4
#define rx_b_ctle_peak2_hyst_endbit                15
#define rx_b_ctle_peak2_hyst_shift                 0
#define rx_b_ctle_peak2_hyst_mask                  0xf


#define rx_b_ddc_done_addr                         0b000000011
#define rx_b_ddc_done_startbit                     11
#define rx_b_ddc_done_width                        1
#define rx_b_ddc_done_endbit                       11
#define rx_b_ddc_done_shift                        4
#define rx_b_ddc_done_mask                         0x10


#define rx_b_ddc_hyst_left_edge_addr               0b000001001
#define rx_b_ddc_hyst_left_edge_startbit           0
#define rx_b_ddc_hyst_left_edge_width              5
#define rx_b_ddc_hyst_left_edge_endbit             4
#define rx_b_ddc_hyst_left_edge_shift              11
#define rx_b_ddc_hyst_left_edge_mask               0xf800


#define rx_b_ddc_hyst_right_edge_addr              0b000001001
#define rx_b_ddc_hyst_right_edge_startbit          5
#define rx_b_ddc_hyst_right_edge_width             5
#define rx_b_ddc_hyst_right_edge_endbit            9
#define rx_b_ddc_hyst_right_edge_shift             6
#define rx_b_ddc_hyst_right_edge_mask              0x7c0


#define rx_b_dfe_done_addr                         0b000000011
#define rx_b_dfe_done_startbit                     10
#define rx_b_dfe_done_width                        1
#define rx_b_dfe_done_endbit                       10
#define rx_b_dfe_done_shift                        5
#define rx_b_dfe_done_mask                         0x20


#define rx_b_dfe_h1_done_addr                      0b000000011
#define rx_b_dfe_h1_done_startbit                  9
#define rx_b_dfe_h1_done_width                     1
#define rx_b_dfe_h1_done_endbit                    9
#define rx_b_dfe_h1_done_shift                     6
#define rx_b_dfe_h1_done_mask                      0x40


#define rx_b_eoff_done_addr                        0b000000011
#define rx_b_eoff_done_startbit                    2
#define rx_b_eoff_done_width                       1
#define rx_b_eoff_done_endbit                      2
#define rx_b_eoff_done_shift                       13
#define rx_b_eoff_done_mask                        0x2000


#define rx_b_lane_hist_min_eye_width_addr          0b000001001
#define rx_b_lane_hist_min_eye_width_startbit      10
#define rx_b_lane_hist_min_eye_width_width         6
#define rx_b_lane_hist_min_eye_width_endbit        15
#define rx_b_lane_hist_min_eye_width_shift         0
#define rx_b_lane_hist_min_eye_width_mask          0x3f


#define rx_b_latch_offset_done_addr                0b000000011
#define rx_b_latch_offset_done_startbit            0
#define rx_b_latch_offset_done_width               1
#define rx_b_latch_offset_done_endbit              0
#define rx_b_latch_offset_done_shift               15
#define rx_b_latch_offset_done_mask                0x8000


#define rx_b_lte_gain_done_addr                    0b000000011
#define rx_b_lte_gain_done_startbit                6
#define rx_b_lte_gain_done_width                   1
#define rx_b_lte_gain_done_endbit                  6
#define rx_b_lte_gain_done_shift                   9
#define rx_b_lte_gain_done_mask                    0x200


#define rx_b_lte_zero_done_addr                    0b000000011
#define rx_b_lte_zero_done_startbit                7
#define rx_b_lte_zero_done_width                   1
#define rx_b_lte_zero_done_endbit                  7
#define rx_b_lte_zero_done_shift                   8
#define rx_b_lte_zero_done_mask                    0x100


#define rx_b_quad_phase_done_addr                  0b000000011
#define rx_b_quad_phase_done_startbit              3
#define rx_b_quad_phase_done_width                 1
#define rx_b_quad_phase_done_endbit                3
#define rx_b_quad_phase_done_shift                 12
#define rx_b_quad_phase_done_mask                  0x1000


#define rx_b_reserved_addr                         0b000000011
#define rx_b_reserved_startbit                     8
#define rx_b_reserved_width                        1
#define rx_b_reserved_endbit                       8
#define rx_b_reserved_shift                        7
#define rx_b_reserved_mask                         0x80


#define rx_b_step_done_alias_addr                  0b000000011
#define rx_b_step_done_alias_startbit              0
#define rx_b_step_done_alias_width                 13
#define rx_b_step_done_alias_endbit                12
#define rx_b_step_done_alias_shift                 3
#define rx_b_step_done_alias_mask                  0xfff8


#define rx_bad_eye_opt_height_addr                 0b000000001
#define rx_bad_eye_opt_height_startbit             1
#define rx_bad_eye_opt_height_width                1
#define rx_bad_eye_opt_height_endbit               1
#define rx_bad_eye_opt_height_shift                14
#define rx_bad_eye_opt_height_mask                 0x4000


#define rx_bad_eye_opt_width_addr                  0b000000001
#define rx_bad_eye_opt_width_startbit              0
#define rx_bad_eye_opt_width_width                 1
#define rx_bad_eye_opt_width_endbit                0
#define rx_bad_eye_opt_width_shift                 15
#define rx_bad_eye_opt_width_mask                  0x8000


#define rx_ber_fail_addr                           0b000000001
#define rx_ber_fail_startbit                       15
#define rx_ber_fail_width                          1
#define rx_ber_fail_endbit                         15
#define rx_ber_fail_shift                          0
#define rx_ber_fail_mask                           0x1


#define rx_ber_timer_sel_bist_addr                 0b110000101
#define rx_ber_timer_sel_bist_startbit             0
#define rx_ber_timer_sel_bist_width                4
#define rx_ber_timer_sel_bist_endbit               3
#define rx_ber_timer_sel_bist_shift                12
#define rx_ber_timer_sel_bist_mask                 0xf000


#define rx_berm_bank_addr                          0b111100001
#define rx_berm_bank_startbit                      0
#define rx_berm_bank_width                         1
#define rx_berm_bank_endbit                        0
#define rx_berm_bank_shift                         15
#define rx_berm_bank_mask                          0x8000


#define rx_berm_depth_addr                         0b111100001
#define rx_berm_depth_startbit                     1
#define rx_berm_depth_width                        2
#define rx_berm_depth_endbit                       2
#define rx_berm_depth_shift                        13
#define rx_berm_depth_mask                         0x6000


#define rx_berm_hoffset_addr                       0b111100010
#define rx_berm_hoffset_startbit                   0
#define rx_berm_hoffset_width                      7
#define rx_berm_hoffset_endbit                     6
#define rx_berm_hoffset_shift                      9
#define rx_berm_hoffset_mask                       0xfe00


#define rx_berm_pattern_sel_addr                   0b111100001
#define rx_berm_pattern_sel_startbit               3
#define rx_berm_pattern_sel_width                  3
#define rx_berm_pattern_sel_endbit                 5
#define rx_berm_pattern_sel_shift                  10
#define rx_berm_pattern_sel_mask                   0x1c00


#define rx_berm_voffset_addr                       0b111100010
#define rx_berm_voffset_startbit                   7
#define rx_berm_voffset_width                      9
#define rx_berm_voffset_endbit                     15
#define rx_berm_voffset_shift                      0
#define rx_berm_voffset_mask                       0x1ff


#define rx_clr_eye_height_width_addr               0b110100000
#define rx_clr_eye_height_width_startbit           12
#define rx_clr_eye_height_width_width              1
#define rx_clr_eye_height_width_endbit             12
#define rx_clr_eye_height_width_shift              3
#define rx_clr_eye_height_width_mask               0x8


#define rx_clr_lane_recal_cnt_addr                 0b110100000
#define rx_clr_lane_recal_cnt_startbit             11
#define rx_clr_lane_recal_cnt_width                1
#define rx_clr_lane_recal_cnt_endbit               11
#define rx_clr_lane_recal_cnt_shift                4
#define rx_clr_lane_recal_cnt_mask                 0x10


#define rx_cmd_init_done_addr                      0b000000000
#define rx_cmd_init_done_startbit                  6
#define rx_cmd_init_done_width                     1
#define rx_cmd_init_done_endbit                    6
#define rx_cmd_init_done_shift                     9
#define rx_cmd_init_done_mask                      0x200


#define rx_ctle_force_peak1_peak2_cal_enable_addr   0b111100000
#define rx_ctle_force_peak1_peak2_cal_enable_startbit   11
#define rx_ctle_force_peak1_peak2_cal_enable_width   3
#define rx_ctle_force_peak1_peak2_cal_enable_endbit   13
#define rx_ctle_force_peak1_peak2_cal_enable_shift   2
#define rx_ctle_force_peak1_peak2_cal_enable_mask   0x1c


#define rx_ctle_gain_fail_addr                     0b000000001
#define rx_ctle_gain_fail_startbit                 2
#define rx_ctle_gain_fail_width                    1
#define rx_ctle_gain_fail_endbit                   2
#define rx_ctle_gain_fail_shift                    13
#define rx_ctle_gain_fail_mask                     0x2000


#define rx_ctle_gain_max_check_addr                0b110100001
#define rx_ctle_gain_max_check_startbit            7
#define rx_ctle_gain_max_check_width               4
#define rx_ctle_gain_max_check_endbit              10
#define rx_ctle_gain_max_check_shift               5
#define rx_ctle_gain_max_check_mask                0x1e0


#define rx_ctle_gain_min_check_addr                0b110100001
#define rx_ctle_gain_min_check_startbit            11
#define rx_ctle_gain_min_check_width               4
#define rx_ctle_gain_min_check_endbit              14
#define rx_ctle_gain_min_check_shift               1
#define rx_ctle_gain_min_check_mask                0x1e


#define rx_ctle_peak1_cal_disable_addr             0b110001110
#define rx_ctle_peak1_cal_disable_startbit         9
#define rx_ctle_peak1_cal_disable_width            1
#define rx_ctle_peak1_cal_disable_endbit           9
#define rx_ctle_peak1_cal_disable_shift            6
#define rx_ctle_peak1_cal_disable_mask             0x40


#define rx_ctle_peak1_fail_addr                    0b000000001
#define rx_ctle_peak1_fail_startbit                6
#define rx_ctle_peak1_fail_width                   1
#define rx_ctle_peak1_fail_endbit                  6
#define rx_ctle_peak1_fail_shift                   9
#define rx_ctle_peak1_fail_mask                    0x200


#define rx_ctle_peak1_hyst_delta_mode_addr         0b110000111
#define rx_ctle_peak1_hyst_delta_mode_startbit     6
#define rx_ctle_peak1_hyst_delta_mode_width        1
#define rx_ctle_peak1_hyst_delta_mode_endbit       6
#define rx_ctle_peak1_hyst_delta_mode_shift        9
#define rx_ctle_peak1_hyst_delta_mode_mask         0x200


#define rx_ctle_peak1_hyst_limit_addr              0b110000111
#define rx_ctle_peak1_hyst_limit_startbit          0
#define rx_ctle_peak1_hyst_limit_width             3
#define rx_ctle_peak1_hyst_limit_endbit            2
#define rx_ctle_peak1_hyst_limit_shift             13
#define rx_ctle_peak1_hyst_limit_mask              0xe000


#define rx_ctle_peak1_max_check_addr               0b110100100
#define rx_ctle_peak1_max_check_startbit           0
#define rx_ctle_peak1_max_check_width              5
#define rx_ctle_peak1_max_check_endbit             4
#define rx_ctle_peak1_max_check_shift              11
#define rx_ctle_peak1_max_check_mask               0xf800


#define rx_ctle_peak1_min_check_addr               0b110100100
#define rx_ctle_peak1_min_check_startbit           5
#define rx_ctle_peak1_min_check_width              5
#define rx_ctle_peak1_min_check_endbit             9
#define rx_ctle_peak1_min_check_shift              6
#define rx_ctle_peak1_min_check_mask               0x7c0


#define rx_ctle_peak1_peak2_cal_disable_alias_addr   0b110001110
#define rx_ctle_peak1_peak2_cal_disable_alias_startbit   9
#define rx_ctle_peak1_peak2_cal_disable_alias_width   2
#define rx_ctle_peak1_peak2_cal_disable_alias_endbit   10
#define rx_ctle_peak1_peak2_cal_disable_alias_shift   5
#define rx_ctle_peak1_peak2_cal_disable_alias_mask   0x60


#define rx_ctle_peak2_cal_disable_addr             0b110001110
#define rx_ctle_peak2_cal_disable_startbit         10
#define rx_ctle_peak2_cal_disable_width            1
#define rx_ctle_peak2_cal_disable_endbit           10
#define rx_ctle_peak2_cal_disable_shift            5
#define rx_ctle_peak2_cal_disable_mask             0x20


#define rx_ctle_peak2_fail_addr                    0b000000001
#define rx_ctle_peak2_fail_startbit                7
#define rx_ctle_peak2_fail_width                   1
#define rx_ctle_peak2_fail_endbit                  7
#define rx_ctle_peak2_fail_shift                   8
#define rx_ctle_peak2_fail_mask                    0x100


#define rx_ctle_peak2_hyst_delta_mode_addr         0b110000111
#define rx_ctle_peak2_hyst_delta_mode_startbit     7
#define rx_ctle_peak2_hyst_delta_mode_width        1
#define rx_ctle_peak2_hyst_delta_mode_endbit       7
#define rx_ctle_peak2_hyst_delta_mode_shift        8
#define rx_ctle_peak2_hyst_delta_mode_mask         0x100


#define rx_ctle_peak2_hyst_limit_addr              0b110000111
#define rx_ctle_peak2_hyst_limit_startbit          3
#define rx_ctle_peak2_hyst_limit_width             3
#define rx_ctle_peak2_hyst_limit_endbit            5
#define rx_ctle_peak2_hyst_limit_shift             10
#define rx_ctle_peak2_hyst_limit_mask              0x1c00


#define rx_ctle_peak2_max_check_addr               0b110101001
#define rx_ctle_peak2_max_check_startbit           0
#define rx_ctle_peak2_max_check_width              5
#define rx_ctle_peak2_max_check_endbit             4
#define rx_ctle_peak2_max_check_shift              11
#define rx_ctle_peak2_max_check_mask               0xf800


#define rx_ctle_peak2_min_check_addr               0b110101001
#define rx_ctle_peak2_min_check_startbit           5
#define rx_ctle_peak2_min_check_width              5
#define rx_ctle_peak2_min_check_endbit             9
#define rx_ctle_peak2_min_check_shift              6
#define rx_ctle_peak2_min_check_mask               0x7c0


#define rx_ctle_peak_hyst_settings_full_reg_addr   0b110000111
#define rx_ctle_peak_hyst_settings_full_reg_startbit   0
#define rx_ctle_peak_hyst_settings_full_reg_width   16
#define rx_ctle_peak_hyst_settings_full_reg_endbit   15
#define rx_ctle_peak_hyst_settings_full_reg_shift   0
#define rx_ctle_peak_hyst_settings_full_reg_mask   0xffff


#define rx_ctle_start_at_zero_disable_addr         0b110001011
#define rx_ctle_start_at_zero_disable_startbit     15
#define rx_ctle_start_at_zero_disable_width        1
#define rx_ctle_start_at_zero_disable_endbit       15
#define rx_ctle_start_at_zero_disable_shift        0
#define rx_ctle_start_at_zero_disable_mask         0x1


#define rx_current_cal_lane_addr                   0b110110100
#define rx_current_cal_lane_startbit               3
#define rx_current_cal_lane_width                  5
#define rx_current_cal_lane_endbit                 7
#define rx_current_cal_lane_shift                  8
#define rx_current_cal_lane_mask                   0x1f00


#define rx_dc_enable_full_reg_addr                 0b110001101
#define rx_dc_enable_full_reg_startbit             0
#define rx_dc_enable_full_reg_width                16
#define rx_dc_enable_full_reg_endbit               15
#define rx_dc_enable_full_reg_shift                0
#define rx_dc_enable_full_reg_mask                 0xffff


#define rx_dc_enable_latch_offset_cal_addr         0b110001101
#define rx_dc_enable_latch_offset_cal_startbit     0
#define rx_dc_enable_latch_offset_cal_width        1
#define rx_dc_enable_latch_offset_cal_endbit       0
#define rx_dc_enable_latch_offset_cal_shift        15
#define rx_dc_enable_latch_offset_cal_mask         0x8000


#define rx_dc_enable_loff_a_addr                   0b110110100
#define rx_dc_enable_loff_a_startbit               8
#define rx_dc_enable_loff_a_width                  1
#define rx_dc_enable_loff_a_endbit                 8
#define rx_dc_enable_loff_a_shift                  7
#define rx_dc_enable_loff_a_mask                   0x80


#define rx_dc_enable_loff_ab_alias_addr            0b110110100
#define rx_dc_enable_loff_ab_alias_startbit        8
#define rx_dc_enable_loff_ab_alias_width           2
#define rx_dc_enable_loff_ab_alias_endbit          9
#define rx_dc_enable_loff_ab_alias_shift           6
#define rx_dc_enable_loff_ab_alias_mask            0xc0


#define rx_dc_enable_loff_ab_data_edge_alias_addr   0b110110100
#define rx_dc_enable_loff_ab_data_edge_alias_startbit   8
#define rx_dc_enable_loff_ab_data_edge_alias_width   4
#define rx_dc_enable_loff_ab_data_edge_alias_endbit   11
#define rx_dc_enable_loff_ab_data_edge_alias_shift   4
#define rx_dc_enable_loff_ab_data_edge_alias_mask   0xf0


#define rx_dc_enable_loff_b_addr                   0b110110100
#define rx_dc_enable_loff_b_startbit               9
#define rx_dc_enable_loff_b_width                  1
#define rx_dc_enable_loff_b_endbit                 9
#define rx_dc_enable_loff_b_shift                  6
#define rx_dc_enable_loff_b_mask                   0x40


#define rx_dc_enable_loff_data_addr                0b110110100
#define rx_dc_enable_loff_data_startbit            10
#define rx_dc_enable_loff_data_width               1
#define rx_dc_enable_loff_data_endbit              10
#define rx_dc_enable_loff_data_shift               5
#define rx_dc_enable_loff_data_mask                0x20


#define rx_dc_enable_loff_data_edge_alias_addr     0b110110100
#define rx_dc_enable_loff_data_edge_alias_startbit   10
#define rx_dc_enable_loff_data_edge_alias_width    2
#define rx_dc_enable_loff_data_edge_alias_endbit   11
#define rx_dc_enable_loff_data_edge_alias_shift    4
#define rx_dc_enable_loff_data_edge_alias_mask     0x30


#define rx_dc_enable_loff_edge_addr                0b110110100
#define rx_dc_enable_loff_edge_startbit            11
#define rx_dc_enable_loff_edge_width               1
#define rx_dc_enable_loff_edge_endbit              11
#define rx_dc_enable_loff_edge_shift               4
#define rx_dc_enable_loff_edge_mask                0x10


#define rx_dc_step_cntl_opt_alias_addr             0b110001101
#define rx_dc_step_cntl_opt_alias_startbit         0
#define rx_dc_step_cntl_opt_alias_width            2
#define rx_dc_step_cntl_opt_alias_endbit           1
#define rx_dc_step_cntl_opt_alias_shift            14
#define rx_dc_step_cntl_opt_alias_mask             0xc000


#define rx_dccal_done_addr                         0b000000000
#define rx_dccal_done_startbit                     1
#define rx_dccal_done_width                        1
#define rx_dccal_done_endbit                       1
#define rx_dccal_done_shift                        14
#define rx_dccal_done_mask                         0x4000


#define rx_ddc_ber_period_sel_addr                 0b110000101
#define rx_ddc_ber_period_sel_startbit             8
#define rx_ddc_ber_period_sel_width                2
#define rx_ddc_ber_period_sel_endbit               9
#define rx_ddc_ber_period_sel_shift                6
#define rx_ddc_ber_period_sel_mask                 0xc0


#define rx_ddc_fail_addr                           0b000000001
#define rx_ddc_fail_startbit                       14
#define rx_ddc_fail_width                          1
#define rx_ddc_fail_endbit                         14
#define rx_ddc_fail_shift                          1
#define rx_ddc_fail_mask                           0x2


#define rx_ddc_hysteresis_addr                     0b110001110
#define rx_ddc_hysteresis_startbit                 14
#define rx_ddc_hysteresis_width                    2
#define rx_ddc_hysteresis_endbit                   15
#define rx_ddc_hysteresis_shift                    0
#define rx_ddc_hysteresis_mask                     0x3


#define rx_ddc_measure_limited_addr                0b000000000
#define rx_ddc_measure_limited_startbit            12
#define rx_ddc_measure_limited_width               1
#define rx_ddc_measure_limited_endbit              12
#define rx_ddc_measure_limited_shift               3
#define rx_ddc_measure_limited_mask                0x8


#define rx_ddc_min_err_lim_addr                    0b110001110
#define rx_ddc_min_err_lim_startbit                11
#define rx_ddc_min_err_lim_width                   3
#define rx_ddc_min_err_lim_endbit                  13
#define rx_ddc_min_err_lim_shift                   2
#define rx_ddc_min_err_lim_mask                    0x1c


#define rx_ddc_small_eye_warning_addr              0b000000000
#define rx_ddc_small_eye_warning_startbit          11
#define rx_ddc_small_eye_warning_width             1
#define rx_ddc_small_eye_warning_endbit            11
#define rx_ddc_small_eye_warning_shift             4
#define rx_ddc_small_eye_warning_mask              0x10


#define rx_dfe_ap_addr                             0b000000101
#define rx_dfe_ap_startbit                         8
#define rx_dfe_ap_width                            8
#define rx_dfe_ap_endbit                           15
#define rx_dfe_ap_shift                            0
#define rx_dfe_ap_mask                             0xff


#define rx_dfe_clkadj_coeff_addr                   0b110001011
#define rx_dfe_clkadj_coeff_startbit               6
#define rx_dfe_clkadj_coeff_width                  7
#define rx_dfe_clkadj_coeff_endbit                 12
#define rx_dfe_clkadj_coeff_shift                  3
#define rx_dfe_clkadj_coeff_mask                   0x3f8


#define rx_dfe_coef_1_6_addr                       0b000001010
#define rx_dfe_coef_1_6_startbit                   8
#define rx_dfe_coef_1_6_width                      8
#define rx_dfe_coef_1_6_endbit                     15
#define rx_dfe_coef_1_6_shift                      0
#define rx_dfe_coef_1_6_mask                       0xff


#define rx_dfe_coef_3_4_addr                       0b000001101
#define rx_dfe_coef_3_4_startbit                   8
#define rx_dfe_coef_3_4_width                      8
#define rx_dfe_coef_3_4_endbit                     15
#define rx_dfe_coef_3_4_shift                      0
#define rx_dfe_coef_3_4_mask                       0xff


#define rx_dfe_coef_5_2_addr                       0b000001101
#define rx_dfe_coef_5_2_startbit                   0
#define rx_dfe_coef_5_2_width                      8
#define rx_dfe_coef_5_2_endbit                     7
#define rx_dfe_coef_5_2_shift                      8
#define rx_dfe_coef_5_2_mask                       0xff00


#define rx_dfe_coef_5_2_3_4_alias_addr             0b000001101
#define rx_dfe_coef_5_2_3_4_alias_startbit         0
#define rx_dfe_coef_5_2_3_4_alias_width            16
#define rx_dfe_coef_5_2_3_4_alias_endbit           15
#define rx_dfe_coef_5_2_3_4_alias_shift            0
#define rx_dfe_coef_5_2_3_4_alias_mask             0xffff


#define rx_dfe_coef_7_0_addr                       0b000001010
#define rx_dfe_coef_7_0_startbit                   0
#define rx_dfe_coef_7_0_width                      8
#define rx_dfe_coef_7_0_endbit                     7
#define rx_dfe_coef_7_0_shift                      8
#define rx_dfe_coef_7_0_mask                       0xff00


#define rx_dfe_coef_7_0_1_6_alias_addr             0b000001010
#define rx_dfe_coef_7_0_1_6_alias_startbit         0
#define rx_dfe_coef_7_0_1_6_alias_width            16
#define rx_dfe_coef_7_0_1_6_alias_endbit           15
#define rx_dfe_coef_7_0_1_6_alias_shift            0
#define rx_dfe_coef_7_0_1_6_alias_mask             0xffff


#define rx_dfe_coef_h1_addr                        0b000001100
#define rx_dfe_coef_h1_startbit                    0
#define rx_dfe_coef_h1_width                       8
#define rx_dfe_coef_h1_endbit                      7
#define rx_dfe_coef_h1_shift                       8
#define rx_dfe_coef_h1_mask                        0xff00


#define rx_dfe_debug_addr                          0b111001001
#define rx_dfe_debug_startbit                      0
#define rx_dfe_debug_width                         16
#define rx_dfe_debug_endbit                        15
#define rx_dfe_debug_shift                         0
#define rx_dfe_debug_mask                          0xffff


#define rx_dfe_fail_addr                           0b000000001
#define rx_dfe_fail_startbit                       13
#define rx_dfe_fail_width                          1
#define rx_dfe_fail_endbit                         13
#define rx_dfe_fail_shift                          2
#define rx_dfe_fail_mask                           0x4


#define rx_dfe_fast_h1_adj_addr                    0b110000110
#define rx_dfe_fast_h1_adj_startbit                0
#define rx_dfe_fast_h1_adj_width                   3
#define rx_dfe_fast_h1_adj_endbit                  2
#define rx_dfe_fast_h1_adj_shift                   13
#define rx_dfe_fast_h1_adj_mask                    0xe000


#define rx_dfe_full_h1_adj_addr                    0b110000110
#define rx_dfe_full_h1_adj_startbit                3
#define rx_dfe_full_h1_adj_width                   3
#define rx_dfe_full_h1_adj_endbit                  5
#define rx_dfe_full_h1_adj_shift                   10
#define rx_dfe_full_h1_adj_mask                    0x1c00


#define rx_dfe_full_max_error_disable_addr         0b110000110
#define rx_dfe_full_max_error_disable_startbit     7
#define rx_dfe_full_max_error_disable_width        1
#define rx_dfe_full_max_error_disable_endbit       7
#define rx_dfe_full_max_error_disable_shift        8
#define rx_dfe_full_max_error_disable_mask         0x100


#define rx_dfe_full_mode_addr                      0b110000110
#define rx_dfe_full_mode_startbit                  6
#define rx_dfe_full_mode_width                     1
#define rx_dfe_full_mode_endbit                    6
#define rx_dfe_full_mode_shift                     9
#define rx_dfe_full_mode_mask                      0x200


#define rx_dfe_full_quad_addr                      0b000000000
#define rx_dfe_full_quad_startbit                  13
#define rx_dfe_full_quad_width                     2
#define rx_dfe_full_quad_endbit                    14
#define rx_dfe_full_quad_shift                     1
#define rx_dfe_full_quad_mask                      0x6


#define rx_dfe_h1_fail_addr                        0b000000001
#define rx_dfe_h1_fail_startbit                    12
#define rx_dfe_h1_fail_width                       1
#define rx_dfe_h1_fail_endbit                      12
#define rx_dfe_h1_fail_shift                       3
#define rx_dfe_h1_fail_mask                        0x8


#define rx_dfe_h1_max_check_addr                   0b110101011
#define rx_dfe_h1_max_check_startbit               0
#define rx_dfe_h1_max_check_width                  7
#define rx_dfe_h1_max_check_endbit                 6
#define rx_dfe_h1_max_check_shift                  9
#define rx_dfe_h1_max_check_mask                   0xfe00


#define rx_dfe_h1_min_check_addr                   0b110101011
#define rx_dfe_h1_min_check_startbit               7
#define rx_dfe_h1_min_check_width                  7
#define rx_dfe_h1_min_check_endbit                 13
#define rx_dfe_h1_min_check_shift                  2
#define rx_dfe_h1_min_check_mask                   0x1fc


#define rx_dfe_hyst_enabled_addr                   0b000000000
#define rx_dfe_hyst_enabled_startbit               15
#define rx_dfe_hyst_enabled_width                  1
#define rx_dfe_hyst_enabled_endbit                 15
#define rx_dfe_hyst_enabled_shift                  0
#define rx_dfe_hyst_enabled_mask                   0x1


#define rx_dfe_hyst_min_recal_cnt_addr             0b110001110
#define rx_dfe_hyst_min_recal_cnt_startbit         4
#define rx_dfe_hyst_min_recal_cnt_width            5
#define rx_dfe_hyst_min_recal_cnt_endbit           8
#define rx_dfe_hyst_min_recal_cnt_shift            7
#define rx_dfe_hyst_min_recal_cnt_mask             0xf80


#define rx_dfe_max_check_addr                      0b110100110
#define rx_dfe_max_check_startbit                  0
#define rx_dfe_max_check_width                     8
#define rx_dfe_max_check_endbit                    7
#define rx_dfe_max_check_shift                     8
#define rx_dfe_max_check_mask                      0xff00


#define rx_dfe_min_check_addr                      0b110100111
#define rx_dfe_min_check_startbit                  0
#define rx_dfe_min_check_width                     8
#define rx_dfe_min_check_endbit                    7
#define rx_dfe_min_check_shift                     8
#define rx_dfe_min_check_mask                      0xff00


#define rx_disable_bank_pdwn_addr                  0b110000011
#define rx_disable_bank_pdwn_startbit              12
#define rx_disable_bank_pdwn_width                 1
#define rx_disable_bank_pdwn_endbit                12
#define rx_disable_bank_pdwn_shift                 3
#define rx_disable_bank_pdwn_mask                  0x8


#define rx_enable_auto_recal_0_15_addr             0b110101110
#define rx_enable_auto_recal_0_15_startbit         0
#define rx_enable_auto_recal_0_15_width            16
#define rx_enable_auto_recal_0_15_endbit           15
#define rx_enable_auto_recal_0_15_shift            0
#define rx_enable_auto_recal_0_15_mask             0xffff


#define rx_enable_auto_recal_16_23_addr            0b110101111
#define rx_enable_auto_recal_16_23_startbit        0
#define rx_enable_auto_recal_16_23_width           8
#define rx_enable_auto_recal_16_23_endbit          7
#define rx_enable_auto_recal_16_23_shift           8
#define rx_enable_auto_recal_16_23_mask            0xff00


#define rx_eo_converged_end_count_addr             0b110000011
#define rx_eo_converged_end_count_startbit         0
#define rx_eo_converged_end_count_width            4
#define rx_eo_converged_end_count_endbit           3
#define rx_eo_converged_end_count_shift            12
#define rx_eo_converged_end_count_mask             0xf000


#define rx_eo_enable_bank_sync_addr                0b110001001
#define rx_eo_enable_bank_sync_startbit            6
#define rx_eo_enable_bank_sync_width               1
#define rx_eo_enable_bank_sync_endbit              6
#define rx_eo_enable_bank_sync_shift               9
#define rx_eo_enable_bank_sync_mask                0x200


#define rx_eo_enable_ctle_peak_cal_addr            0b110001001
#define rx_eo_enable_ctle_peak_cal_startbit        2
#define rx_eo_enable_ctle_peak_cal_width           1
#define rx_eo_enable_ctle_peak_cal_endbit          2
#define rx_eo_enable_ctle_peak_cal_shift           13
#define rx_eo_enable_ctle_peak_cal_mask            0x2000


#define rx_eo_enable_ddc_addr                      0b110001001
#define rx_eo_enable_ddc_startbit                  5
#define rx_eo_enable_ddc_width                     1
#define rx_eo_enable_ddc_endbit                    5
#define rx_eo_enable_ddc_shift                     10
#define rx_eo_enable_ddc_mask                      0x400


#define rx_eo_enable_dfe_cal_addr                  0b110001001
#define rx_eo_enable_dfe_cal_startbit              4
#define rx_eo_enable_dfe_cal_width                 1
#define rx_eo_enable_dfe_cal_endbit                4
#define rx_eo_enable_dfe_cal_shift                 11
#define rx_eo_enable_dfe_cal_mask                  0x800


#define rx_eo_enable_edge_offset_cal_addr          0b110001001
#define rx_eo_enable_edge_offset_cal_startbit      1
#define rx_eo_enable_edge_offset_cal_width         1
#define rx_eo_enable_edge_offset_cal_endbit        1
#define rx_eo_enable_edge_offset_cal_shift         14
#define rx_eo_enable_edge_offset_cal_mask          0x4000


#define rx_eo_enable_full_reg_addr                 0b110001001
#define rx_eo_enable_full_reg_startbit             0
#define rx_eo_enable_full_reg_width                16
#define rx_eo_enable_full_reg_endbit               15
#define rx_eo_enable_full_reg_shift                0
#define rx_eo_enable_full_reg_mask                 0xffff


#define rx_eo_enable_lte_cal_addr                  0b110001001
#define rx_eo_enable_lte_cal_startbit              3
#define rx_eo_enable_lte_cal_width                 1
#define rx_eo_enable_lte_cal_endbit                3
#define rx_eo_enable_lte_cal_shift                 12
#define rx_eo_enable_lte_cal_mask                  0x1000


#define rx_eo_enable_quad_phase_cal_addr           0b110001001
#define rx_eo_enable_quad_phase_cal_startbit       7
#define rx_eo_enable_quad_phase_cal_width          1
#define rx_eo_enable_quad_phase_cal_endbit         7
#define rx_eo_enable_quad_phase_cal_shift          8
#define rx_eo_enable_quad_phase_cal_mask           0x100


#define rx_eo_enable_vga_cal_addr                  0b110001001
#define rx_eo_enable_vga_cal_startbit              0
#define rx_eo_enable_vga_cal_width                 1
#define rx_eo_enable_vga_cal_endbit                0
#define rx_eo_enable_vga_cal_shift                 15
#define rx_eo_enable_vga_cal_mask                  0x8000


#define rx_eo_phase_select_0_2_addr                0b110110100
#define rx_eo_phase_select_0_2_startbit            12
#define rx_eo_phase_select_0_2_width               3
#define rx_eo_phase_select_0_2_endbit              14
#define rx_eo_phase_select_0_2_shift               1
#define rx_eo_phase_select_0_2_mask                0xe


#define rx_eo_step_cntl_opt_alias_addr             0b110001001
#define rx_eo_step_cntl_opt_alias_startbit         0
#define rx_eo_step_cntl_opt_alias_width            8
#define rx_eo_step_cntl_opt_alias_endbit           7
#define rx_eo_step_cntl_opt_alias_shift            8
#define rx_eo_step_cntl_opt_alias_mask             0xff00


#define rx_eo_vga_ctle_loop_not_converged_addr     0b000000000
#define rx_eo_vga_ctle_loop_not_converged_startbit   7
#define rx_eo_vga_ctle_loop_not_converged_width    1
#define rx_eo_vga_ctle_loop_not_converged_endbit   7
#define rx_eo_vga_ctle_loop_not_converged_shift    8
#define rx_eo_vga_ctle_loop_not_converged_mask     0x100


#define rx_eoff_fail_addr                          0b000000001
#define rx_eoff_fail_startbit                      4
#define rx_eoff_fail_width                         1
#define rx_eoff_fail_endbit                        4
#define rx_eoff_fail_shift                         11
#define rx_eoff_fail_mask                          0x800


#define rx_eoff_max_check_addr                     0b110100011
#define rx_eoff_max_check_startbit                 0
#define rx_eoff_max_check_width                    8
#define rx_eoff_max_check_endbit                   7
#define rx_eoff_max_check_shift                    8
#define rx_eoff_max_check_mask                     0xff00


#define rx_eoff_min_check_addr                     0b110100011
#define rx_eoff_min_check_startbit                 8
#define rx_eoff_min_check_width                    8
#define rx_eoff_min_check_endbit                   15
#define rx_eoff_min_check_shift                    0
#define rx_eoff_min_check_mask                     0xff


#define rx_eoff_poff_fail_addr                     0b000000001
#define rx_eoff_poff_fail_startbit                 5
#define rx_eoff_poff_fail_width                    1
#define rx_eoff_poff_fail_endbit                   5
#define rx_eoff_poff_fail_shift                    10
#define rx_eoff_poff_fail_mask                     0x400


#define rx_epoff_max_check_addr                    0b110101000
#define rx_epoff_max_check_startbit                0
#define rx_epoff_max_check_width                   8
#define rx_epoff_max_check_endbit                  7
#define rx_epoff_max_check_shift                   8
#define rx_epoff_max_check_mask                    0xff00


#define rx_epoff_min_check_addr                    0b110101000
#define rx_epoff_min_check_startbit                8
#define rx_epoff_min_check_width                   8
#define rx_epoff_min_check_endbit                  15
#define rx_epoff_min_check_shift                   0
#define rx_epoff_min_check_mask                    0xff


#define rx_esd_abs_val_check_addr                  0b110100101
#define rx_esd_abs_val_check_startbit              6
#define rx_esd_abs_val_check_width                 7
#define rx_esd_abs_val_check_endbit                12
#define rx_esd_abs_val_check_shift                 3
#define rx_esd_abs_val_check_mask                  0x3f8


#define rx_esd_fail_addr                           0b000000001
#define rx_esd_fail_startbit                       8
#define rx_esd_fail_width                          1
#define rx_esd_fail_endbit                         8
#define rx_esd_fail_shift                          7
#define rx_esd_fail_mask                           0x80


#define rx_esd_shift_val_check_addr                0b110100101
#define rx_esd_shift_val_check_startbit            0
#define rx_esd_shift_val_check_width               6
#define rx_esd_shift_val_check_endbit              5
#define rx_esd_shift_val_check_shift               10
#define rx_esd_shift_val_check_mask                0xfc00


#define rx_eye_height_min_check_addr               0b110100000
#define rx_eye_height_min_check_startbit           0
#define rx_eye_height_min_check_width              7
#define rx_eye_height_min_check_endbit             6
#define rx_eye_height_min_check_shift              9
#define rx_eye_height_min_check_mask               0xfe00


#define rx_eye_width_min_check_addr                0b110100001
#define rx_eye_width_min_check_startbit            0
#define rx_eye_width_min_check_width               7
#define rx_eye_width_min_check_endbit              6
#define rx_eye_width_min_check_shift               9
#define rx_eye_width_min_check_mask                0xfe00


#define rx_fail_flag_addr                          0b111001011
#define rx_fail_flag_startbit                      12
#define rx_fail_flag_width                         1
#define rx_fail_flag_endbit                        12
#define rx_fail_flag_shift                         3
#define rx_fail_flag_mask                          0x8


#define rx_hist_min_eye_height_addr                0b110110011
#define rx_hist_min_eye_height_startbit            8
#define rx_hist_min_eye_height_width               7
#define rx_hist_min_eye_height_endbit              14
#define rx_hist_min_eye_height_shift               1
#define rx_hist_min_eye_height_mask                0xfe


#define rx_hist_min_eye_height_lane_addr           0b110110010
#define rx_hist_min_eye_height_lane_startbit       11
#define rx_hist_min_eye_height_lane_width          5
#define rx_hist_min_eye_height_lane_endbit         15
#define rx_hist_min_eye_height_lane_shift          0
#define rx_hist_min_eye_height_lane_mask           0x1f


#define rx_hist_min_eye_height_valid_addr          0b110110010
#define rx_hist_min_eye_height_valid_startbit      8
#define rx_hist_min_eye_height_valid_width         1
#define rx_hist_min_eye_height_valid_endbit        8
#define rx_hist_min_eye_height_valid_shift         7
#define rx_hist_min_eye_height_valid_mask          0x80


#define rx_hist_min_eye_width_addr                 0b110110011
#define rx_hist_min_eye_width_startbit             0
#define rx_hist_min_eye_width_width                8
#define rx_hist_min_eye_width_endbit               7
#define rx_hist_min_eye_width_shift                8
#define rx_hist_min_eye_width_mask                 0xff00


#define rx_hist_min_eye_width_lane_addr            0b110110010
#define rx_hist_min_eye_width_lane_startbit        3
#define rx_hist_min_eye_width_lane_width           5
#define rx_hist_min_eye_width_lane_endbit          7
#define rx_hist_min_eye_width_lane_shift           8
#define rx_hist_min_eye_width_lane_mask            0x1f00


#define rx_hist_min_eye_width_mode_addr            0b110000011
#define rx_hist_min_eye_width_mode_startbit        4
#define rx_hist_min_eye_width_mode_width           2
#define rx_hist_min_eye_width_mode_endbit          5
#define rx_hist_min_eye_width_mode_shift           10
#define rx_hist_min_eye_width_mode_mask            0xc00


#define rx_hist_min_eye_width_valid_addr           0b110110010
#define rx_hist_min_eye_width_valid_startbit       0
#define rx_hist_min_eye_width_valid_width          1
#define rx_hist_min_eye_width_valid_endbit         0
#define rx_hist_min_eye_width_valid_shift          15
#define rx_hist_min_eye_width_valid_mask           0x8000


#define rx_init_done_addr                          0b000000000
#define rx_init_done_startbit                      0
#define rx_init_done_width                         1
#define rx_init_done_endbit                        0
#define rx_init_done_shift                         15
#define rx_init_done_mask                          0x8000


#define rx_lane_busy_addr                          0b000000000
#define rx_lane_busy_startbit                      2
#define rx_lane_busy_width                         1
#define rx_lane_busy_endbit                        2
#define rx_lane_busy_shift                         13
#define rx_lane_busy_mask                          0x2000


#define rx_lane_fail_0_15_addr                     0b111000100
#define rx_lane_fail_0_15_startbit                 0
#define rx_lane_fail_0_15_width                    16
#define rx_lane_fail_0_15_endbit                   15
#define rx_lane_fail_0_15_shift                    0
#define rx_lane_fail_0_15_mask                     0xffff


#define rx_lane_fail_16_23_addr                    0b111000101
#define rx_lane_fail_16_23_startbit                0
#define rx_lane_fail_16_23_width                   8
#define rx_lane_fail_16_23_endbit                  7
#define rx_lane_fail_16_23_shift                   8
#define rx_lane_fail_16_23_mask                    0xff00


#define rx_lane_fail_cnt_addr                      0b111001011
#define rx_lane_fail_cnt_startbit                  13
#define rx_lane_fail_cnt_width                     2
#define rx_lane_fail_cnt_endbit                    14
#define rx_lane_fail_cnt_shift                     1
#define rx_lane_fail_cnt_mask                      0x6


#define rx_lane_hist_min_eye_height_addr           0b000001011
#define rx_lane_hist_min_eye_height_startbit       9
#define rx_lane_hist_min_eye_height_width          7
#define rx_lane_hist_min_eye_height_endbit         15
#define rx_lane_hist_min_eye_height_shift          0
#define rx_lane_hist_min_eye_height_mask           0x7f


#define rx_lane_hist_min_eye_height_bank_addr      0b000001011
#define rx_lane_hist_min_eye_height_bank_startbit   5
#define rx_lane_hist_min_eye_height_bank_width     1
#define rx_lane_hist_min_eye_height_bank_endbit    5
#define rx_lane_hist_min_eye_height_bank_shift     10
#define rx_lane_hist_min_eye_height_bank_mask      0x400


#define rx_lane_hist_min_eye_height_latch_addr     0b000001011
#define rx_lane_hist_min_eye_height_latch_startbit   6
#define rx_lane_hist_min_eye_height_latch_width    3
#define rx_lane_hist_min_eye_height_latch_endbit   8
#define rx_lane_hist_min_eye_height_latch_shift    7
#define rx_lane_hist_min_eye_height_latch_mask     0x380


#define rx_lane_hist_min_eye_height_quad_addr      0b000000101
#define rx_lane_hist_min_eye_height_quad_startbit   6
#define rx_lane_hist_min_eye_height_quad_width     2
#define rx_lane_hist_min_eye_height_quad_endbit    7
#define rx_lane_hist_min_eye_height_quad_shift     8
#define rx_lane_hist_min_eye_height_quad_mask      0x300


#define rx_lane_hist_min_eye_height_valid_addr     0b000000101
#define rx_lane_hist_min_eye_height_valid_startbit   0
#define rx_lane_hist_min_eye_height_valid_width    1
#define rx_lane_hist_min_eye_height_valid_endbit   0
#define rx_lane_hist_min_eye_height_valid_shift    15
#define rx_lane_hist_min_eye_height_valid_mask     0x8000


#define rx_lane_hist_min_eye_width_valid_addr      0b000000101
#define rx_lane_hist_min_eye_width_valid_startbit   1
#define rx_lane_hist_min_eye_width_valid_width     1
#define rx_lane_hist_min_eye_width_valid_endbit    1
#define rx_lane_hist_min_eye_width_valid_shift     14
#define rx_lane_hist_min_eye_width_valid_mask      0x4000


#define rx_lane_recal_cnt_addr                     0b000000111
#define rx_lane_recal_cnt_startbit                 0
#define rx_lane_recal_cnt_width                    16
#define rx_lane_recal_cnt_endbit                   15
#define rx_lane_recal_cnt_shift                    0
#define rx_lane_recal_cnt_mask                     0xffff


#define rx_lanes_pon_00_15_addr                    0b111001110
#define rx_lanes_pon_00_15_startbit                0
#define rx_lanes_pon_00_15_width                   16
#define rx_lanes_pon_00_15_endbit                  15
#define rx_lanes_pon_00_15_shift                   0
#define rx_lanes_pon_00_15_mask                    0xffff


#define rx_lanes_pon_16_23_addr                    0b111001111
#define rx_lanes_pon_16_23_startbit                0
#define rx_lanes_pon_16_23_width                   8
#define rx_lanes_pon_16_23_endbit                  7
#define rx_lanes_pon_16_23_shift                   8
#define rx_lanes_pon_16_23_mask                    0xff00


#define rx_latch_offset_fail_addr                  0b000000001
#define rx_latch_offset_fail_startbit              3
#define rx_latch_offset_fail_width                 1
#define rx_latch_offset_fail_endbit                3
#define rx_latch_offset_fail_shift                 12
#define rx_latch_offset_fail_mask                  0x1000


#define rx_latchoff_max_check_addr                 0b110100010
#define rx_latchoff_max_check_startbit             0
#define rx_latchoff_max_check_width                8
#define rx_latchoff_max_check_endbit               7
#define rx_latchoff_max_check_shift                8
#define rx_latchoff_max_check_mask                 0xff00


#define rx_latchoff_min_check_addr                 0b110100010
#define rx_latchoff_min_check_startbit             8
#define rx_latchoff_min_check_width                8
#define rx_latchoff_min_check_endbit               15
#define rx_latchoff_min_check_shift                0
#define rx_latchoff_min_check_mask                 0xff


#define rx_linklayer_done_addr                     0b111001011
#define rx_linklayer_done_startbit                 11
#define rx_linklayer_done_width                    1
#define rx_linklayer_done_endbit                   11
#define rx_linklayer_done_shift                    4
#define rx_linklayer_done_mask                     0x10


#define rx_linklayer_fail_addr                     0b111001011
#define rx_linklayer_fail_startbit                 9
#define rx_linklayer_fail_width                    1
#define rx_linklayer_fail_endbit                   9
#define rx_linklayer_fail_shift                    6
#define rx_linklayer_fail_mask                     0x40


#define rx_loff_ad_n000_addr                       0b000001100
#define rx_loff_ad_n000_startbit                   8
#define rx_loff_ad_n000_width                      8
#define rx_loff_ad_n000_endbit                     15
#define rx_loff_ad_n000_shift                      0
#define rx_loff_ad_n000_mask                       0xff


#define rx_loff_ad_n000_valid_addr                 0b000000000
#define rx_loff_ad_n000_valid_startbit             9
#define rx_loff_ad_n000_valid_width                1
#define rx_loff_ad_n000_valid_endbit               9
#define rx_loff_ad_n000_valid_shift                6
#define rx_loff_ad_n000_valid_mask                 0x40


#define rx_lte_gain_fail_addr                      0b000000001
#define rx_lte_gain_fail_startbit                  9
#define rx_lte_gain_fail_width                     1
#define rx_lte_gain_fail_endbit                    9
#define rx_lte_gain_fail_shift                     6
#define rx_lte_gain_fail_mask                      0x40


#define rx_margin_timing_offset_addr               0b000001111
#define rx_margin_timing_offset_startbit           8
#define rx_margin_timing_offset_width              8
#define rx_margin_timing_offset_endbit             15
#define rx_margin_timing_offset_shift              0
#define rx_margin_timing_offset_mask               0xff


#define rx_margin_voltage_offset_addr              0b000001111
#define rx_margin_voltage_offset_startbit          0
#define rx_margin_voltage_offset_width             8
#define rx_margin_voltage_offset_endbit            7
#define rx_margin_voltage_offset_shift             8
#define rx_margin_voltage_offset_mask              0xff00


#define rx_min_recal_cnt_addr                      0b110001110
#define rx_min_recal_cnt_startbit                  0
#define rx_min_recal_cnt_width                     4
#define rx_min_recal_cnt_endbit                    3
#define rx_min_recal_cnt_shift                     12
#define rx_min_recal_cnt_mask                      0xf000


#define rx_min_recal_cnt_reached_addr              0b000000000
#define rx_min_recal_cnt_reached_startbit          4
#define rx_min_recal_cnt_reached_width             1
#define rx_min_recal_cnt_reached_endbit            4
#define rx_min_recal_cnt_reached_shift             11
#define rx_min_recal_cnt_reached_mask              0x800


#define rx_qpa_hysteresis_addr                     0b111100000
#define rx_qpa_hysteresis_startbit                 8
#define rx_qpa_hysteresis_width                    3
#define rx_qpa_hysteresis_endbit                   10
#define rx_qpa_hysteresis_shift                    5
#define rx_qpa_hysteresis_mask                     0xe0


#define rx_qpa_hysteresis_enable_addr              0b111100000
#define rx_qpa_hysteresis_enable_startbit          6
#define rx_qpa_hysteresis_enable_width             1
#define rx_qpa_hysteresis_enable_endbit            6
#define rx_qpa_hysteresis_enable_shift             9
#define rx_qpa_hysteresis_enable_mask              0x200


#define rx_qpa_pattern_addr                        0b111100000
#define rx_qpa_pattern_startbit                    1
#define rx_qpa_pattern_width                       5
#define rx_qpa_pattern_endbit                      5
#define rx_qpa_pattern_shift                       10
#define rx_qpa_pattern_mask                        0x7c00


#define rx_qpa_pattern_enable_addr                 0b111100000
#define rx_qpa_pattern_enable_startbit             0
#define rx_qpa_pattern_enable_width                1
#define rx_qpa_pattern_enable_endbit               0
#define rx_qpa_pattern_enable_shift                15
#define rx_qpa_pattern_enable_mask                 0x8000


#define rx_quad_ph_adj_max_check_addr              0b110100110
#define rx_quad_ph_adj_max_check_startbit          8
#define rx_quad_ph_adj_max_check_width             6
#define rx_quad_ph_adj_max_check_endbit            13
#define rx_quad_ph_adj_max_check_shift             2
#define rx_quad_ph_adj_max_check_mask              0xfc


#define rx_quad_ph_adj_min_check_addr              0b110100111
#define rx_quad_ph_adj_min_check_startbit          8
#define rx_quad_ph_adj_min_check_width             6
#define rx_quad_ph_adj_min_check_endbit            13
#define rx_quad_ph_adj_min_check_shift             2
#define rx_quad_ph_adj_min_check_mask              0xfc


#define rx_quad_phase_fail_addr                    0b000000001
#define rx_quad_phase_fail_startbit                11
#define rx_quad_phase_fail_width                   1
#define rx_quad_phase_fail_endbit                  11
#define rx_quad_phase_fail_shift                   4
#define rx_quad_phase_fail_mask                    0x10


#define rx_rc_enable_bank_sync_addr                0b110001010
#define rx_rc_enable_bank_sync_startbit            7
#define rx_rc_enable_bank_sync_width               1
#define rx_rc_enable_bank_sync_endbit              7
#define rx_rc_enable_bank_sync_shift               8
#define rx_rc_enable_bank_sync_mask                0x100


#define rx_rc_enable_ctle_peak_cal_addr            0b110001010
#define rx_rc_enable_ctle_peak_cal_startbit        2
#define rx_rc_enable_ctle_peak_cal_width           1
#define rx_rc_enable_ctle_peak_cal_endbit          2
#define rx_rc_enable_ctle_peak_cal_shift           13
#define rx_rc_enable_ctle_peak_cal_mask            0x2000


#define rx_rc_enable_ddc_addr                      0b110001010
#define rx_rc_enable_ddc_startbit                  5
#define rx_rc_enable_ddc_width                     1
#define rx_rc_enable_ddc_endbit                    5
#define rx_rc_enable_ddc_shift                     10
#define rx_rc_enable_ddc_mask                      0x400


#define rx_rc_enable_dfe_cal_addr                  0b110001010
#define rx_rc_enable_dfe_cal_startbit              4
#define rx_rc_enable_dfe_cal_width                 1
#define rx_rc_enable_dfe_cal_endbit                4
#define rx_rc_enable_dfe_cal_shift                 11
#define rx_rc_enable_dfe_cal_mask                  0x800


#define rx_rc_enable_edge_offset_cal_addr          0b110001010
#define rx_rc_enable_edge_offset_cal_startbit      1
#define rx_rc_enable_edge_offset_cal_width         1
#define rx_rc_enable_edge_offset_cal_endbit        1
#define rx_rc_enable_edge_offset_cal_shift         14
#define rx_rc_enable_edge_offset_cal_mask          0x4000


#define rx_rc_enable_full_reg_addr                 0b110001010
#define rx_rc_enable_full_reg_startbit             0
#define rx_rc_enable_full_reg_width                16
#define rx_rc_enable_full_reg_endbit               15
#define rx_rc_enable_full_reg_shift                0
#define rx_rc_enable_full_reg_mask                 0xffff


#define rx_rc_enable_lte_cal_addr                  0b110001010
#define rx_rc_enable_lte_cal_startbit              3
#define rx_rc_enable_lte_cal_width                 1
#define rx_rc_enable_lte_cal_endbit                3
#define rx_rc_enable_lte_cal_shift                 12
#define rx_rc_enable_lte_cal_mask                  0x1000


#define rx_rc_enable_quad_phase_cal_addr           0b110001010
#define rx_rc_enable_quad_phase_cal_startbit       8
#define rx_rc_enable_quad_phase_cal_width          1
#define rx_rc_enable_quad_phase_cal_endbit         8
#define rx_rc_enable_quad_phase_cal_shift          7
#define rx_rc_enable_quad_phase_cal_mask           0x80


#define rx_rc_enable_vga_cal_addr                  0b110001010
#define rx_rc_enable_vga_cal_startbit              0
#define rx_rc_enable_vga_cal_width                 1
#define rx_rc_enable_vga_cal_endbit                0
#define rx_rc_enable_vga_cal_shift                 15
#define rx_rc_enable_vga_cal_mask                  0x8000


#define rx_rc_step_cntl_opt_alias_addr             0b110001010
#define rx_rc_step_cntl_opt_alias_startbit         0
#define rx_rc_step_cntl_opt_alias_width            9
#define rx_rc_step_cntl_opt_alias_endbit           8
#define rx_rc_step_cntl_opt_alias_shift            7
#define rx_rc_step_cntl_opt_alias_mask             0xff80


#define rx_recal_abort_0_15_addr                   0b110101100
#define rx_recal_abort_0_15_startbit               0
#define rx_recal_abort_0_15_width                  16
#define rx_recal_abort_0_15_endbit                 15
#define rx_recal_abort_0_15_shift                  0
#define rx_recal_abort_0_15_mask                   0xffff


#define rx_recal_abort_16_23_addr                  0b110101101
#define rx_recal_abort_16_23_startbit              0
#define rx_recal_abort_16_23_width                 8
#define rx_recal_abort_16_23_endbit                7
#define rx_recal_abort_16_23_shift                 8
#define rx_recal_abort_16_23_mask                  0xff00


#define rx_recal_before_init_addr                  0b000000000
#define rx_recal_before_init_startbit              5
#define rx_recal_before_init_width                 1
#define rx_recal_before_init_endbit                5
#define rx_recal_before_init_shift                 10
#define rx_recal_before_init_mask                  0x400


#define rx_recal_done_addr                         0b000000000
#define rx_recal_done_startbit                     3
#define rx_recal_done_width                        1
#define rx_recal_done_endbit                       3
#define rx_recal_done_shift                        12
#define rx_recal_done_mask                         0x1000


#define rx_recal_run_or_unused_0_15_addr           0b111001100
#define rx_recal_run_or_unused_0_15_startbit       0
#define rx_recal_run_or_unused_0_15_width          16
#define rx_recal_run_or_unused_0_15_endbit         15
#define rx_recal_run_or_unused_0_15_shift          0
#define rx_recal_run_or_unused_0_15_mask           0xffff


#define rx_recal_run_or_unused_16_23_addr          0b111001101
#define rx_recal_run_or_unused_16_23_startbit      0
#define rx_recal_run_or_unused_16_23_width         8
#define rx_recal_run_or_unused_16_23_endbit        7
#define rx_recal_run_or_unused_16_23_shift         8
#define rx_recal_run_or_unused_16_23_mask          0xff00


#define rx_running_eq_eval_addr                    0b110110100
#define rx_running_eq_eval_startbit                1
#define rx_running_eq_eval_width                   1
#define rx_running_eq_eval_endbit                  1
#define rx_running_eq_eval_shift                   14
#define rx_running_eq_eval_mask                    0x4000


#define rx_running_recal_addr                      0b110110100
#define rx_running_recal_startbit                  0
#define rx_running_recal_width                     1
#define rx_running_recal_endbit                    0
#define rx_running_recal_shift                     15
#define rx_running_recal_mask                      0x8000


#define rx_sigdet_fail_addr                        0b000000001
#define rx_sigdet_fail_startbit                    10
#define rx_sigdet_fail_width                       1
#define rx_sigdet_fail_endbit                      10
#define rx_sigdet_fail_shift                       5
#define rx_sigdet_fail_mask                        0x20


#define rx_spare_max_check_addr                    0b110100100
#define rx_spare_max_check_startbit                10
#define rx_spare_max_check_width                   5
#define rx_spare_max_check_endbit                  14
#define rx_spare_max_check_shift                   1
#define rx_spare_max_check_mask                    0x3e


#define rx_step_fail_alias_addr                    0b000000001
#define rx_step_fail_alias_startbit                2
#define rx_step_fail_alias_width                   14
#define rx_step_fail_alias_endbit                  15
#define rx_step_fail_alias_shift                   0
#define rx_step_fail_alias_mask                    0x3fff


#define rx_vga_amax_target_addr                    0b110010000
#define rx_vga_amax_target_startbit                8
#define rx_vga_amax_target_width                   8
#define rx_vga_amax_target_endbit                  15
#define rx_vga_amax_target_shift                   0
#define rx_vga_amax_target_mask                    0xff


#define rx_vga_converged_addr                      0b000001011
#define rx_vga_converged_startbit                  0
#define rx_vga_converged_width                     4
#define rx_vga_converged_endbit                    3
#define rx_vga_converged_shift                     12
#define rx_vga_converged_mask                      0xf000


#define rx_vga_debug_addr                          0b111000011
#define rx_vga_debug_startbit                      0
#define rx_vga_debug_width                         16
#define rx_vga_debug_endbit                        15
#define rx_vga_debug_shift                         0
#define rx_vga_debug_mask                          0xffff


#define rx_vga_jump_target_addr                    0b110010000
#define rx_vga_jump_target_startbit                0
#define rx_vga_jump_target_width                   8
#define rx_vga_jump_target_endbit                  7
#define rx_vga_jump_target_shift                   8
#define rx_vga_jump_target_mask                    0xff00


#define rx_vga_recal_max_target_addr               0b110010001
#define rx_vga_recal_max_target_startbit           0
#define rx_vga_recal_max_target_width              8
#define rx_vga_recal_max_target_endbit             7
#define rx_vga_recal_max_target_shift              8
#define rx_vga_recal_max_target_mask               0xff00


#define rx_vga_recal_min_target_addr               0b110010001
#define rx_vga_recal_min_target_startbit           8
#define rx_vga_recal_min_target_width              8
#define rx_vga_recal_min_target_endbit             15
#define rx_vga_recal_min_target_shift              0
#define rx_vga_recal_min_target_mask               0xff


#define tx_bist_dcc_i_max_addr                     0b110000001
#define tx_bist_dcc_i_max_startbit                 8
#define tx_bist_dcc_i_max_width                    8
#define tx_bist_dcc_i_max_endbit                   15
#define tx_bist_dcc_i_max_shift                    0
#define tx_bist_dcc_i_max_mask                     0xff


#define tx_bist_dcc_i_min_addr                     0b110000001
#define tx_bist_dcc_i_min_startbit                 0
#define tx_bist_dcc_i_min_width                    8
#define tx_bist_dcc_i_min_endbit                   7
#define tx_bist_dcc_i_min_shift                    8
#define tx_bist_dcc_i_min_mask                     0xff00


#define tx_bist_dcc_iq_max_addr                    0b110001100
#define tx_bist_dcc_iq_max_startbit                8
#define tx_bist_dcc_iq_max_width                   8
#define tx_bist_dcc_iq_max_endbit                  15
#define tx_bist_dcc_iq_max_shift                   0
#define tx_bist_dcc_iq_max_mask                    0xff


#define tx_bist_dcc_iq_min_addr                    0b110001100
#define tx_bist_dcc_iq_min_startbit                0
#define tx_bist_dcc_iq_min_width                   8
#define tx_bist_dcc_iq_min_endbit                  7
#define tx_bist_dcc_iq_min_shift                   8
#define tx_bist_dcc_iq_min_mask                    0xff00


#define tx_bist_dcc_q_max_addr                     0b110000010
#define tx_bist_dcc_q_max_startbit                 8
#define tx_bist_dcc_q_max_width                    8
#define tx_bist_dcc_q_max_endbit                   15
#define tx_bist_dcc_q_max_shift                    0
#define tx_bist_dcc_q_max_mask                     0xff


#define tx_bist_dcc_q_min_addr                     0b110000010
#define tx_bist_dcc_q_min_startbit                 0
#define tx_bist_dcc_q_min_width                    8
#define tx_bist_dcc_q_min_endbit                   7
#define tx_bist_dcc_q_min_shift                    8
#define tx_bist_dcc_q_min_mask                     0xff00


#define tx_bist_fail_0_15_addr                     0b111010000
#define tx_bist_fail_0_15_startbit                 0
#define tx_bist_fail_0_15_width                    16
#define tx_bist_fail_0_15_endbit                   15
#define tx_bist_fail_0_15_shift                    0
#define tx_bist_fail_0_15_mask                     0xffff


#define tx_bist_fail_16_23_addr                    0b111010001
#define tx_bist_fail_16_23_startbit                0
#define tx_bist_fail_16_23_width                   8
#define tx_bist_fail_16_23_endbit                  7
#define tx_bist_fail_16_23_shift                   8
#define tx_bist_fail_16_23_mask                    0xff00


#define tx_bist_hs_dac_thresh_max_addr             0b110001111
#define tx_bist_hs_dac_thresh_max_startbit         8
#define tx_bist_hs_dac_thresh_max_width            8
#define tx_bist_hs_dac_thresh_max_endbit           15
#define tx_bist_hs_dac_thresh_max_shift            0
#define tx_bist_hs_dac_thresh_max_mask             0xff


#define tx_bist_hs_dac_thresh_min_addr             0b110001111
#define tx_bist_hs_dac_thresh_min_startbit         0
#define tx_bist_hs_dac_thresh_min_width            8
#define tx_bist_hs_dac_thresh_min_endbit           7
#define tx_bist_hs_dac_thresh_min_shift            8
#define tx_bist_hs_dac_thresh_min_mask             0xff00


#define tx_dc_enable_dcc_addr                      0b110001101
#define tx_dc_enable_dcc_startbit                  1
#define tx_dc_enable_dcc_width                     1
#define tx_dc_enable_dcc_endbit                    1
#define tx_dc_enable_dcc_shift                     14
#define tx_dc_enable_dcc_mask                      0x4000


#define tx_dcc_debug_addr                          0b111000110
#define tx_dcc_debug_startbit                      0
#define tx_dcc_debug_width                         16
#define tx_dcc_debug_endbit                        15
#define tx_dcc_debug_shift                         0
#define tx_dcc_debug_mask                          0xffff


#define tx_dcc_main_min_samples_addr               0b110010100
#define tx_dcc_main_min_samples_startbit           8
#define tx_dcc_main_min_samples_width              8
#define tx_dcc_main_min_samples_endbit             15
#define tx_dcc_main_min_samples_shift              0
#define tx_dcc_main_min_samples_mask               0xff


#define tx_detectrx_ovr_en_0_15_addr               0b110001000
#define tx_detectrx_ovr_en_0_15_startbit           0
#define tx_detectrx_ovr_en_0_15_width              16
#define tx_detectrx_ovr_en_0_15_endbit             15
#define tx_detectrx_ovr_en_0_15_shift              0
#define tx_detectrx_ovr_en_0_15_mask               0xffff


#define tx_detectrx_ovr_en_16_23_addr              0b110010011
#define tx_detectrx_ovr_en_16_23_startbit          8
#define tx_detectrx_ovr_en_16_23_width             8
#define tx_detectrx_ovr_en_16_23_endbit            15
#define tx_detectrx_ovr_en_16_23_shift             0
#define tx_detectrx_ovr_en_16_23_mask              0xff


#define tx_detectrx_ovr_val_0_15_addr              0b110010010
#define tx_detectrx_ovr_val_0_15_startbit          0
#define tx_detectrx_ovr_val_0_15_width             16
#define tx_detectrx_ovr_val_0_15_endbit            15
#define tx_detectrx_ovr_val_0_15_shift             0
#define tx_detectrx_ovr_val_0_15_mask              0xffff


#define tx_detectrx_ovr_val_16_23_addr             0b110010011
#define tx_detectrx_ovr_val_16_23_startbit         0
#define tx_detectrx_ovr_val_16_23_width            8
#define tx_detectrx_ovr_val_16_23_endbit           7
#define tx_detectrx_ovr_val_16_23_shift            8
#define tx_detectrx_ovr_val_16_23_mask             0xff00


#define tx_ffe_post_coef_addr                      0b110010101
#define tx_ffe_post_coef_startbit                  10
#define tx_ffe_post_coef_width                     6
#define tx_ffe_post_coef_endbit                    15
#define tx_ffe_post_coef_shift                     0
#define tx_ffe_post_coef_mask                      0x3f


#define tx_ffe_pre1_coef_addr                      0b110010101
#define tx_ffe_pre1_coef_startbit                  4
#define tx_ffe_pre1_coef_width                     6
#define tx_ffe_pre1_coef_endbit                    9
#define tx_ffe_pre1_coef_shift                     6
#define tx_ffe_pre1_coef_mask                      0xfc0


#define tx_ffe_pre2_coef_addr                      0b110010101
#define tx_ffe_pre2_coef_startbit                  0
#define tx_ffe_pre2_coef_width                     4
#define tx_ffe_pre2_coef_endbit                    3
#define tx_ffe_pre2_coef_shift                     12
#define tx_ffe_pre2_coef_mask                      0xf000


#define tx_lanes_pon_00_15_addr                    0b111010010
#define tx_lanes_pon_00_15_startbit                0
#define tx_lanes_pon_00_15_width                   16
#define tx_lanes_pon_00_15_endbit                  15
#define tx_lanes_pon_00_15_shift                   0
#define tx_lanes_pon_00_15_mask                    0xffff


#define tx_lanes_pon_16_23_addr                    0b111010011
#define tx_lanes_pon_16_23_startbit                0
#define tx_lanes_pon_16_23_width                   8
#define tx_lanes_pon_16_23_endbit                  7
#define tx_lanes_pon_16_23_shift                   8
#define tx_lanes_pon_16_23_mask                    0xff00


#define tx_pcie_idle_del_sel_1_addr                0b110010110
#define tx_pcie_idle_del_sel_1_startbit            0
#define tx_pcie_idle_del_sel_1_width               4
#define tx_pcie_idle_del_sel_1_endbit              3
#define tx_pcie_idle_del_sel_1_shift               12
#define tx_pcie_idle_del_sel_1_mask                0xf000


#define tx_pcie_idle_del_sel_2_addr                0b110010110
#define tx_pcie_idle_del_sel_2_startbit            8
#define tx_pcie_idle_del_sel_2_width               4
#define tx_pcie_idle_del_sel_2_endbit              11
#define tx_pcie_idle_del_sel_2_shift               4
#define tx_pcie_idle_del_sel_2_mask                0xf0


#define tx_pcie_idle_del_sel_3_addr                0b110010111
#define tx_pcie_idle_del_sel_3_startbit            0
#define tx_pcie_idle_del_sel_3_width               4
#define tx_pcie_idle_del_sel_3_endbit              3
#define tx_pcie_idle_del_sel_3_shift               12
#define tx_pcie_idle_del_sel_3_mask                0xf000


#define tx_pcie_idle_del_sel_4_addr                0b110010111
#define tx_pcie_idle_del_sel_4_startbit            8
#define tx_pcie_idle_del_sel_4_width               4
#define tx_pcie_idle_del_sel_4_endbit              11
#define tx_pcie_idle_del_sel_4_shift               4
#define tx_pcie_idle_del_sel_4_mask                0xf0


#define tx_pcie_idle_del_sel_5_addr                0b110011000
#define tx_pcie_idle_del_sel_5_startbit            0
#define tx_pcie_idle_del_sel_5_width               4
#define tx_pcie_idle_del_sel_5_endbit              3
#define tx_pcie_idle_del_sel_5_shift               12
#define tx_pcie_idle_del_sel_5_mask                0xf000


#define tx_pcie_idle_loz_del_sel_1_alias_addr      0b110010110
#define tx_pcie_idle_loz_del_sel_1_alias_startbit   0
#define tx_pcie_idle_loz_del_sel_1_alias_width     8
#define tx_pcie_idle_loz_del_sel_1_alias_endbit    7
#define tx_pcie_idle_loz_del_sel_1_alias_shift     8
#define tx_pcie_idle_loz_del_sel_1_alias_mask      0xff00


#define tx_pcie_idle_loz_del_sel_2_alias_addr      0b110010110
#define tx_pcie_idle_loz_del_sel_2_alias_startbit   8
#define tx_pcie_idle_loz_del_sel_2_alias_width     8
#define tx_pcie_idle_loz_del_sel_2_alias_endbit    15
#define tx_pcie_idle_loz_del_sel_2_alias_shift     0
#define tx_pcie_idle_loz_del_sel_2_alias_mask      0xff


#define tx_pcie_idle_loz_del_sel_3_alias_addr      0b110010111
#define tx_pcie_idle_loz_del_sel_3_alias_startbit   0
#define tx_pcie_idle_loz_del_sel_3_alias_width     8
#define tx_pcie_idle_loz_del_sel_3_alias_endbit    7
#define tx_pcie_idle_loz_del_sel_3_alias_shift     8
#define tx_pcie_idle_loz_del_sel_3_alias_mask      0xff00


#define tx_pcie_idle_loz_del_sel_4_alias_addr      0b110010111
#define tx_pcie_idle_loz_del_sel_4_alias_startbit   8
#define tx_pcie_idle_loz_del_sel_4_alias_width     8
#define tx_pcie_idle_loz_del_sel_4_alias_endbit    15
#define tx_pcie_idle_loz_del_sel_4_alias_shift     0
#define tx_pcie_idle_loz_del_sel_4_alias_mask      0xff


#define tx_pcie_idle_loz_del_sel_5_alias_addr      0b110011000
#define tx_pcie_idle_loz_del_sel_5_alias_startbit   0
#define tx_pcie_idle_loz_del_sel_5_alias_width     8
#define tx_pcie_idle_loz_del_sel_5_alias_endbit    7
#define tx_pcie_idle_loz_del_sel_5_alias_shift     8
#define tx_pcie_idle_loz_del_sel_5_alias_mask      0xff00


#define tx_pcie_loz_del_sel_1_addr                 0b110010110
#define tx_pcie_loz_del_sel_1_startbit             4
#define tx_pcie_loz_del_sel_1_width                4
#define tx_pcie_loz_del_sel_1_endbit               7
#define tx_pcie_loz_del_sel_1_shift                8
#define tx_pcie_loz_del_sel_1_mask                 0xf00


#define tx_pcie_loz_del_sel_2_addr                 0b110010110
#define tx_pcie_loz_del_sel_2_startbit             12
#define tx_pcie_loz_del_sel_2_width                4
#define tx_pcie_loz_del_sel_2_endbit               15
#define tx_pcie_loz_del_sel_2_shift                0
#define tx_pcie_loz_del_sel_2_mask                 0xf


#define tx_pcie_loz_del_sel_3_addr                 0b110010111
#define tx_pcie_loz_del_sel_3_startbit             4
#define tx_pcie_loz_del_sel_3_width                4
#define tx_pcie_loz_del_sel_3_endbit               7
#define tx_pcie_loz_del_sel_3_shift                8
#define tx_pcie_loz_del_sel_3_mask                 0xf00


#define tx_pcie_loz_del_sel_4_addr                 0b110010111
#define tx_pcie_loz_del_sel_4_startbit             12
#define tx_pcie_loz_del_sel_4_width                4
#define tx_pcie_loz_del_sel_4_endbit               15
#define tx_pcie_loz_del_sel_4_shift                0
#define tx_pcie_loz_del_sel_4_mask                 0xf


#define tx_pcie_loz_del_sel_5_addr                 0b110011000
#define tx_pcie_loz_del_sel_5_startbit             4
#define tx_pcie_loz_del_sel_5_width                4
#define tx_pcie_loz_del_sel_5_endbit               7
#define tx_pcie_loz_del_sel_5_shift                8
#define tx_pcie_loz_del_sel_5_mask                 0xf00


#define tx_rc_enable_dcc_addr                      0b110001010
#define tx_rc_enable_dcc_startbit                  6
#define tx_rc_enable_dcc_width                     1
#define tx_rc_enable_dcc_endbit                    6
#define tx_rc_enable_dcc_shift                     9
#define tx_rc_enable_dcc_mask                      0x200


#define tx_seg_test_1r_segs_addr                   0b110010100
#define tx_seg_test_1r_segs_startbit               3
#define tx_seg_test_1r_segs_width                  2
#define tx_seg_test_1r_segs_endbit                 4
#define tx_seg_test_1r_segs_shift                  11
#define tx_seg_test_1r_segs_mask                   0x1800


#define tx_seg_test_2r_seg_addr                    0b110010100
#define tx_seg_test_2r_seg_startbit                2
#define tx_seg_test_2r_seg_width                   1
#define tx_seg_test_2r_seg_endbit                  2
#define tx_seg_test_2r_seg_shift                   13
#define tx_seg_test_2r_seg_mask                    0x2000


#define tx_seg_test_en_addr                        0b110010100
#define tx_seg_test_en_startbit                    0
#define tx_seg_test_en_width                       1
#define tx_seg_test_en_endbit                      0
#define tx_seg_test_en_shift                       15
#define tx_seg_test_en_mask                        0x8000


#define tx_seg_test_fail_addr                      0b111010100
#define tx_seg_test_fail_startbit                  0
#define tx_seg_test_fail_width                     1
#define tx_seg_test_fail_endbit                    0
#define tx_seg_test_fail_shift                     15
#define tx_seg_test_fail_mask                      0x8000


#define tx_seg_test_frc_2r_addr                    0b110010100
#define tx_seg_test_frc_2r_startbit                1
#define tx_seg_test_frc_2r_width                   1
#define tx_seg_test_frc_2r_endbit                  1
#define tx_seg_test_frc_2r_shift                   14
#define tx_seg_test_frc_2r_mask                    0x4000


#endif //_PPE_MEM_REG_CONST_IOO_PKG_H_
