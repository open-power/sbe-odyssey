/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ppe_fw_reg_const_pkg_ioo.h $ */
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

#ifndef _PPE_FW_REG_CONST_IOO_PKG_H_
#define _PPE_FW_REG_CONST_IOO_PKG_H_



#define ext_cmd_done_00_15_addr                    0b000001000
#define ext_cmd_done_00_15_startbit                0
#define ext_cmd_done_00_15_width                   16
#define ext_cmd_done_00_15_endbit                  15
#define ext_cmd_done_00_15_shift                   0
#define ext_cmd_done_00_15_mask                    0xffff


#define ext_cmd_done_00_31_addr                    0b000001000
#define ext_cmd_done_00_31_startbit                0
#define ext_cmd_done_00_31_width                   32
#define ext_cmd_done_00_31_endbit                  31
#define ext_cmd_done_00_31_shift                   0
#define ext_cmd_done_00_31_mask                    0xffffffff


#define ext_cmd_done_16_31_addr                    0b000001001
#define ext_cmd_done_16_31_startbit                0
#define ext_cmd_done_16_31_width                   16
#define ext_cmd_done_16_31_endbit                  15
#define ext_cmd_done_16_31_shift                   0
#define ext_cmd_done_16_31_mask                    0xffff


#define ext_cmd_done_bist_final_pl_addr            0b000001000
#define ext_cmd_done_bist_final_pl_startbit        13
#define ext_cmd_done_bist_final_pl_width           1
#define ext_cmd_done_bist_final_pl_endbit          13
#define ext_cmd_done_bist_final_pl_shift           2
#define ext_cmd_done_bist_final_pl_mask            0x4


#define ext_cmd_done_dccal_pl_addr                 0b000001000
#define ext_cmd_done_dccal_pl_startbit             3
#define ext_cmd_done_dccal_pl_width                1
#define ext_cmd_done_dccal_pl_endbit               3
#define ext_cmd_done_dccal_pl_shift                12
#define ext_cmd_done_dccal_pl_mask                 0x1000


#define ext_cmd_done_hw_reg_init_pg_addr           0b000001000
#define ext_cmd_done_hw_reg_init_pg_startbit       0
#define ext_cmd_done_hw_reg_init_pg_width          1
#define ext_cmd_done_hw_reg_init_pg_endbit         0
#define ext_cmd_done_hw_reg_init_pg_shift          15
#define ext_cmd_done_hw_reg_init_pg_mask           0x8000


#define ext_cmd_done_ioreset_pl_addr               0b000001000
#define ext_cmd_done_ioreset_pl_startbit           1
#define ext_cmd_done_ioreset_pl_width              1
#define ext_cmd_done_ioreset_pl_endbit             1
#define ext_cmd_done_ioreset_pl_shift              14
#define ext_cmd_done_ioreset_pl_mask               0x4000


#define ext_cmd_done_lab_code_pg_addr              0b000001001
#define ext_cmd_done_lab_code_pg_startbit          1
#define ext_cmd_done_lab_code_pg_width             1
#define ext_cmd_done_lab_code_pg_endbit            1
#define ext_cmd_done_lab_code_pg_shift             14
#define ext_cmd_done_lab_code_pg_mask              0x4000


#define ext_cmd_done_lab_code_pl_addr              0b000001001
#define ext_cmd_done_lab_code_pl_startbit          0
#define ext_cmd_done_lab_code_pl_width             1
#define ext_cmd_done_lab_code_pl_endbit            0
#define ext_cmd_done_lab_code_pl_shift             15
#define ext_cmd_done_lab_code_pl_mask              0x8000


#define ext_cmd_done_power_off_pl_addr             0b000001000
#define ext_cmd_done_power_off_pl_startbit         6
#define ext_cmd_done_power_off_pl_width            1
#define ext_cmd_done_power_off_pl_endbit           6
#define ext_cmd_done_power_off_pl_shift            9
#define ext_cmd_done_power_off_pl_mask             0x200


#define ext_cmd_done_power_on_pl_addr              0b000001000
#define ext_cmd_done_power_on_pl_startbit          7
#define ext_cmd_done_power_on_pl_width             1
#define ext_cmd_done_power_on_pl_endbit            7
#define ext_cmd_done_power_on_pl_shift             8
#define ext_cmd_done_power_on_pl_mask              0x100


#define ext_cmd_done_recal_pl_addr                 0b000001000
#define ext_cmd_done_recal_pl_startbit             12
#define ext_cmd_done_recal_pl_width                1
#define ext_cmd_done_recal_pl_endbit               12
#define ext_cmd_done_recal_pl_shift                3
#define ext_cmd_done_recal_pl_mask                 0x8


#define ext_cmd_done_rx_bist_tests_pl_addr         0b000001000
#define ext_cmd_done_rx_bist_tests_pl_startbit     9
#define ext_cmd_done_rx_bist_tests_pl_width        1
#define ext_cmd_done_rx_bist_tests_pl_endbit       9
#define ext_cmd_done_rx_bist_tests_pl_shift        6
#define ext_cmd_done_rx_bist_tests_pl_mask         0x40


#define ext_cmd_done_rx_detect_pl_addr             0b000001000
#define ext_cmd_done_rx_detect_pl_startbit         2
#define ext_cmd_done_rx_detect_pl_width            1
#define ext_cmd_done_rx_detect_pl_endbit           2
#define ext_cmd_done_rx_detect_pl_shift            13
#define ext_cmd_done_rx_detect_pl_mask             0x2000


#define ext_cmd_done_train_pl_addr                 0b000001000
#define ext_cmd_done_train_pl_startbit             11
#define ext_cmd_done_train_pl_width                1
#define ext_cmd_done_train_pl_endbit               11
#define ext_cmd_done_train_pl_shift                4
#define ext_cmd_done_train_pl_mask                 0x10


#define ext_cmd_done_tx_bist_tests_pl_addr         0b000001000
#define ext_cmd_done_tx_bist_tests_pl_startbit     10
#define ext_cmd_done_tx_bist_tests_pl_width        1
#define ext_cmd_done_tx_bist_tests_pl_endbit       10
#define ext_cmd_done_tx_bist_tests_pl_shift        5
#define ext_cmd_done_tx_bist_tests_pl_mask         0x20


#define ext_cmd_done_tx_ffe_pl_addr                0b000001000
#define ext_cmd_done_tx_ffe_pl_startbit            5
#define ext_cmd_done_tx_ffe_pl_width               1
#define ext_cmd_done_tx_ffe_pl_endbit              5
#define ext_cmd_done_tx_ffe_pl_shift               10
#define ext_cmd_done_tx_ffe_pl_mask                0x400


#define ext_cmd_done_tx_fifo_init_pl_addr          0b000001000
#define ext_cmd_done_tx_fifo_init_pl_startbit      8
#define ext_cmd_done_tx_fifo_init_pl_width         1
#define ext_cmd_done_tx_fifo_init_pl_endbit        8
#define ext_cmd_done_tx_fifo_init_pl_shift         7
#define ext_cmd_done_tx_fifo_init_pl_mask          0x80


#define ext_cmd_done_tx_zcal_pl_addr               0b000001000
#define ext_cmd_done_tx_zcal_pl_startbit           4
#define ext_cmd_done_tx_zcal_pl_width              1
#define ext_cmd_done_tx_zcal_pl_endbit             4
#define ext_cmd_done_tx_zcal_pl_shift              11
#define ext_cmd_done_tx_zcal_pl_mask               0x800


#define ext_cmd_fail_00_15_addr                    0b000001100
#define ext_cmd_fail_00_15_startbit                0
#define ext_cmd_fail_00_15_width                   16
#define ext_cmd_fail_00_15_endbit                  15
#define ext_cmd_fail_00_15_shift                   0
#define ext_cmd_fail_00_15_mask                    0xffff


#define ext_cmd_fail_00_31_addr                    0b000001100
#define ext_cmd_fail_00_31_startbit                0
#define ext_cmd_fail_00_31_width                   32
#define ext_cmd_fail_00_31_endbit                  31
#define ext_cmd_fail_00_31_shift                   0
#define ext_cmd_fail_00_31_mask                    0xffffffff


#define ext_cmd_fail_16_31_addr                    0b000001101
#define ext_cmd_fail_16_31_startbit                0
#define ext_cmd_fail_16_31_width                   16
#define ext_cmd_fail_16_31_endbit                  15
#define ext_cmd_fail_16_31_shift                   0
#define ext_cmd_fail_16_31_mask                    0xffff


#define ext_cmd_fail_bist_final_pl_addr            0b000001100
#define ext_cmd_fail_bist_final_pl_startbit        13
#define ext_cmd_fail_bist_final_pl_width           1
#define ext_cmd_fail_bist_final_pl_endbit          13
#define ext_cmd_fail_bist_final_pl_shift           2
#define ext_cmd_fail_bist_final_pl_mask            0x4


#define ext_cmd_fail_dccal_pl_addr                 0b000001100
#define ext_cmd_fail_dccal_pl_startbit             3
#define ext_cmd_fail_dccal_pl_width                1
#define ext_cmd_fail_dccal_pl_endbit               3
#define ext_cmd_fail_dccal_pl_shift                12
#define ext_cmd_fail_dccal_pl_mask                 0x1000


#define ext_cmd_fail_hw_reg_init_pg_addr           0b000001100
#define ext_cmd_fail_hw_reg_init_pg_startbit       0
#define ext_cmd_fail_hw_reg_init_pg_width          1
#define ext_cmd_fail_hw_reg_init_pg_endbit         0
#define ext_cmd_fail_hw_reg_init_pg_shift          15
#define ext_cmd_fail_hw_reg_init_pg_mask           0x8000


#define ext_cmd_fail_ioreset_pl_addr               0b000001100
#define ext_cmd_fail_ioreset_pl_startbit           1
#define ext_cmd_fail_ioreset_pl_width              1
#define ext_cmd_fail_ioreset_pl_endbit             1
#define ext_cmd_fail_ioreset_pl_shift              14
#define ext_cmd_fail_ioreset_pl_mask               0x4000


#define ext_cmd_fail_lab_code_pg_addr              0b000001101
#define ext_cmd_fail_lab_code_pg_startbit          1
#define ext_cmd_fail_lab_code_pg_width             1
#define ext_cmd_fail_lab_code_pg_endbit            1
#define ext_cmd_fail_lab_code_pg_shift             14
#define ext_cmd_fail_lab_code_pg_mask              0x4000


#define ext_cmd_fail_lab_code_pl_addr              0b000001101
#define ext_cmd_fail_lab_code_pl_startbit          0
#define ext_cmd_fail_lab_code_pl_width             1
#define ext_cmd_fail_lab_code_pl_endbit            0
#define ext_cmd_fail_lab_code_pl_shift             15
#define ext_cmd_fail_lab_code_pl_mask              0x8000


#define ext_cmd_fail_power_off_pl_addr             0b000001100
#define ext_cmd_fail_power_off_pl_startbit         6
#define ext_cmd_fail_power_off_pl_width            1
#define ext_cmd_fail_power_off_pl_endbit           6
#define ext_cmd_fail_power_off_pl_shift            9
#define ext_cmd_fail_power_off_pl_mask             0x200


#define ext_cmd_fail_power_on_pl_addr              0b000001100
#define ext_cmd_fail_power_on_pl_startbit          7
#define ext_cmd_fail_power_on_pl_width             1
#define ext_cmd_fail_power_on_pl_endbit            7
#define ext_cmd_fail_power_on_pl_shift             8
#define ext_cmd_fail_power_on_pl_mask              0x100


#define ext_cmd_fail_recal_pl_addr                 0b000001100
#define ext_cmd_fail_recal_pl_startbit             12
#define ext_cmd_fail_recal_pl_width                1
#define ext_cmd_fail_recal_pl_endbit               12
#define ext_cmd_fail_recal_pl_shift                3
#define ext_cmd_fail_recal_pl_mask                 0x8


#define ext_cmd_fail_rx_bist_tests_pl_addr         0b000001100
#define ext_cmd_fail_rx_bist_tests_pl_startbit     9
#define ext_cmd_fail_rx_bist_tests_pl_width        1
#define ext_cmd_fail_rx_bist_tests_pl_endbit       9
#define ext_cmd_fail_rx_bist_tests_pl_shift        6
#define ext_cmd_fail_rx_bist_tests_pl_mask         0x40


#define ext_cmd_fail_rx_detect_pl_addr             0b000001100
#define ext_cmd_fail_rx_detect_pl_startbit         2
#define ext_cmd_fail_rx_detect_pl_width            1
#define ext_cmd_fail_rx_detect_pl_endbit           2
#define ext_cmd_fail_rx_detect_pl_shift            13
#define ext_cmd_fail_rx_detect_pl_mask             0x2000


#define ext_cmd_fail_train_pl_addr                 0b000001100
#define ext_cmd_fail_train_pl_startbit             11
#define ext_cmd_fail_train_pl_width                1
#define ext_cmd_fail_train_pl_endbit               11
#define ext_cmd_fail_train_pl_shift                4
#define ext_cmd_fail_train_pl_mask                 0x10


#define ext_cmd_fail_tx_bist_tests_pl_addr         0b000001100
#define ext_cmd_fail_tx_bist_tests_pl_startbit     10
#define ext_cmd_fail_tx_bist_tests_pl_width        1
#define ext_cmd_fail_tx_bist_tests_pl_endbit       10
#define ext_cmd_fail_tx_bist_tests_pl_shift        5
#define ext_cmd_fail_tx_bist_tests_pl_mask         0x20


#define ext_cmd_fail_tx_ffe_pl_addr                0b000001100
#define ext_cmd_fail_tx_ffe_pl_startbit            5
#define ext_cmd_fail_tx_ffe_pl_width               1
#define ext_cmd_fail_tx_ffe_pl_endbit              5
#define ext_cmd_fail_tx_ffe_pl_shift               10
#define ext_cmd_fail_tx_ffe_pl_mask                0x400


#define ext_cmd_fail_tx_fifo_init_pl_addr          0b000001100
#define ext_cmd_fail_tx_fifo_init_pl_startbit      8
#define ext_cmd_fail_tx_fifo_init_pl_width         1
#define ext_cmd_fail_tx_fifo_init_pl_endbit        8
#define ext_cmd_fail_tx_fifo_init_pl_shift         7
#define ext_cmd_fail_tx_fifo_init_pl_mask          0x80


#define ext_cmd_fail_tx_zcal_pl_addr               0b000001100
#define ext_cmd_fail_tx_zcal_pl_startbit           4
#define ext_cmd_fail_tx_zcal_pl_width              1
#define ext_cmd_fail_tx_zcal_pl_endbit             4
#define ext_cmd_fail_tx_zcal_pl_shift              11
#define ext_cmd_fail_tx_zcal_pl_mask               0x800


#define ext_cmd_lanes_rx_00_15_addr                0b000000000
#define ext_cmd_lanes_rx_00_15_startbit            0
#define ext_cmd_lanes_rx_00_15_width               16
#define ext_cmd_lanes_rx_00_15_endbit              15
#define ext_cmd_lanes_rx_00_15_shift               0
#define ext_cmd_lanes_rx_00_15_mask                0xffff


#define ext_cmd_lanes_rx_00_31_addr                0b000000000
#define ext_cmd_lanes_rx_00_31_startbit            0
#define ext_cmd_lanes_rx_00_31_width               32
#define ext_cmd_lanes_rx_00_31_endbit              31
#define ext_cmd_lanes_rx_00_31_shift               0
#define ext_cmd_lanes_rx_00_31_mask                0xffffffff


#define ext_cmd_lanes_rx_16_31_addr                0b000000001
#define ext_cmd_lanes_rx_16_31_startbit            0
#define ext_cmd_lanes_rx_16_31_width               16
#define ext_cmd_lanes_rx_16_31_endbit              15
#define ext_cmd_lanes_rx_16_31_shift               0
#define ext_cmd_lanes_rx_16_31_mask                0xffff


#define ext_cmd_lanes_tx_00_15_addr                0b000000010
#define ext_cmd_lanes_tx_00_15_startbit            0
#define ext_cmd_lanes_tx_00_15_width               16
#define ext_cmd_lanes_tx_00_15_endbit              15
#define ext_cmd_lanes_tx_00_15_shift               0
#define ext_cmd_lanes_tx_00_15_mask                0xffff


#define ext_cmd_lanes_tx_00_31_addr                0b000000010
#define ext_cmd_lanes_tx_00_31_startbit            0
#define ext_cmd_lanes_tx_00_31_width               32
#define ext_cmd_lanes_tx_00_31_endbit              31
#define ext_cmd_lanes_tx_00_31_shift               0
#define ext_cmd_lanes_tx_00_31_mask                0xffffffff


#define ext_cmd_lanes_tx_16_31_addr                0b000000011
#define ext_cmd_lanes_tx_16_31_startbit            0
#define ext_cmd_lanes_tx_16_31_width               16
#define ext_cmd_lanes_tx_16_31_endbit              15
#define ext_cmd_lanes_tx_16_31_shift               0
#define ext_cmd_lanes_tx_16_31_mask                0xffff


#define ext_cmd_power_banks_sel_addr               0b000000110
#define ext_cmd_power_banks_sel_startbit           9
#define ext_cmd_power_banks_sel_width              2
#define ext_cmd_power_banks_sel_endbit             10
#define ext_cmd_power_banks_sel_shift              5
#define ext_cmd_power_banks_sel_mask               0x60


#define ext_cmd_req_00_15_addr                     0b000000100
#define ext_cmd_req_00_15_startbit                 0
#define ext_cmd_req_00_15_width                    16
#define ext_cmd_req_00_15_endbit                   15
#define ext_cmd_req_00_15_shift                    0
#define ext_cmd_req_00_15_mask                     0xffff


#define ext_cmd_req_00_31_addr                     0b000000100
#define ext_cmd_req_00_31_startbit                 0
#define ext_cmd_req_00_31_width                    32
#define ext_cmd_req_00_31_endbit                   31
#define ext_cmd_req_00_31_shift                    0
#define ext_cmd_req_00_31_mask                     0xffffffff


#define ext_cmd_req_16_31_addr                     0b000000101
#define ext_cmd_req_16_31_startbit                 0
#define ext_cmd_req_16_31_width                    16
#define ext_cmd_req_16_31_endbit                   15
#define ext_cmd_req_16_31_shift                    0
#define ext_cmd_req_16_31_mask                     0xffff


#define ext_cmd_req_bist_final_pl_addr             0b000000100
#define ext_cmd_req_bist_final_pl_startbit         13
#define ext_cmd_req_bist_final_pl_width            1
#define ext_cmd_req_bist_final_pl_endbit           13
#define ext_cmd_req_bist_final_pl_shift            2
#define ext_cmd_req_bist_final_pl_mask             0x4


#define ext_cmd_req_dccal_pl_addr                  0b000000100
#define ext_cmd_req_dccal_pl_startbit              3
#define ext_cmd_req_dccal_pl_width                 1
#define ext_cmd_req_dccal_pl_endbit                3
#define ext_cmd_req_dccal_pl_shift                 12
#define ext_cmd_req_dccal_pl_mask                  0x1000


#define ext_cmd_req_hw_reg_init_pg_addr            0b000000100
#define ext_cmd_req_hw_reg_init_pg_startbit        0
#define ext_cmd_req_hw_reg_init_pg_width           1
#define ext_cmd_req_hw_reg_init_pg_endbit          0
#define ext_cmd_req_hw_reg_init_pg_shift           15
#define ext_cmd_req_hw_reg_init_pg_mask            0x8000


#define ext_cmd_req_ioreset_pl_addr                0b000000100
#define ext_cmd_req_ioreset_pl_startbit            1
#define ext_cmd_req_ioreset_pl_width               1
#define ext_cmd_req_ioreset_pl_endbit              1
#define ext_cmd_req_ioreset_pl_shift               14
#define ext_cmd_req_ioreset_pl_mask                0x4000


#define ext_cmd_req_lab_code_pg_addr               0b000000101
#define ext_cmd_req_lab_code_pg_startbit           1
#define ext_cmd_req_lab_code_pg_width              1
#define ext_cmd_req_lab_code_pg_endbit             1
#define ext_cmd_req_lab_code_pg_shift              14
#define ext_cmd_req_lab_code_pg_mask               0x4000


#define ext_cmd_req_lab_code_pl_addr               0b000000101
#define ext_cmd_req_lab_code_pl_startbit           0
#define ext_cmd_req_lab_code_pl_width              1
#define ext_cmd_req_lab_code_pl_endbit             0
#define ext_cmd_req_lab_code_pl_shift              15
#define ext_cmd_req_lab_code_pl_mask               0x8000


#define ext_cmd_req_power_off_pl_addr              0b000000100
#define ext_cmd_req_power_off_pl_startbit          6
#define ext_cmd_req_power_off_pl_width             1
#define ext_cmd_req_power_off_pl_endbit            6
#define ext_cmd_req_power_off_pl_shift             9
#define ext_cmd_req_power_off_pl_mask              0x200


#define ext_cmd_req_power_on_pl_addr               0b000000100
#define ext_cmd_req_power_on_pl_startbit           7
#define ext_cmd_req_power_on_pl_width              1
#define ext_cmd_req_power_on_pl_endbit             7
#define ext_cmd_req_power_on_pl_shift              8
#define ext_cmd_req_power_on_pl_mask               0x100


#define ext_cmd_req_recal_pl_addr                  0b000000100
#define ext_cmd_req_recal_pl_startbit              12
#define ext_cmd_req_recal_pl_width                 1
#define ext_cmd_req_recal_pl_endbit                12
#define ext_cmd_req_recal_pl_shift                 3
#define ext_cmd_req_recal_pl_mask                  0x8


#define ext_cmd_req_rx_bist_tests_pl_addr          0b000000100
#define ext_cmd_req_rx_bist_tests_pl_startbit      9
#define ext_cmd_req_rx_bist_tests_pl_width         1
#define ext_cmd_req_rx_bist_tests_pl_endbit        9
#define ext_cmd_req_rx_bist_tests_pl_shift         6
#define ext_cmd_req_rx_bist_tests_pl_mask          0x40


#define ext_cmd_req_rx_detect_pl_addr              0b000000100
#define ext_cmd_req_rx_detect_pl_startbit          2
#define ext_cmd_req_rx_detect_pl_width             1
#define ext_cmd_req_rx_detect_pl_endbit            2
#define ext_cmd_req_rx_detect_pl_shift             13
#define ext_cmd_req_rx_detect_pl_mask              0x2000


#define ext_cmd_req_train_pl_addr                  0b000000100
#define ext_cmd_req_train_pl_startbit              11
#define ext_cmd_req_train_pl_width                 1
#define ext_cmd_req_train_pl_endbit                11
#define ext_cmd_req_train_pl_shift                 4
#define ext_cmd_req_train_pl_mask                  0x10


#define ext_cmd_req_tx_bist_tests_pl_addr          0b000000100
#define ext_cmd_req_tx_bist_tests_pl_startbit      10
#define ext_cmd_req_tx_bist_tests_pl_width         1
#define ext_cmd_req_tx_bist_tests_pl_endbit        10
#define ext_cmd_req_tx_bist_tests_pl_shift         5
#define ext_cmd_req_tx_bist_tests_pl_mask          0x20


#define ext_cmd_req_tx_ffe_pl_addr                 0b000000100
#define ext_cmd_req_tx_ffe_pl_startbit             5
#define ext_cmd_req_tx_ffe_pl_width                1
#define ext_cmd_req_tx_ffe_pl_endbit               5
#define ext_cmd_req_tx_ffe_pl_shift                10
#define ext_cmd_req_tx_ffe_pl_mask                 0x400


#define ext_cmd_req_tx_fifo_init_pl_addr           0b000000100
#define ext_cmd_req_tx_fifo_init_pl_startbit       8
#define ext_cmd_req_tx_fifo_init_pl_width          1
#define ext_cmd_req_tx_fifo_init_pl_endbit         8
#define ext_cmd_req_tx_fifo_init_pl_shift          7
#define ext_cmd_req_tx_fifo_init_pl_mask           0x80


#define ext_cmd_req_tx_zcal_pl_addr                0b000000100
#define ext_cmd_req_tx_zcal_pl_startbit            4
#define ext_cmd_req_tx_zcal_pl_width               1
#define ext_cmd_req_tx_zcal_pl_endbit              4
#define ext_cmd_req_tx_zcal_pl_shift               11
#define ext_cmd_req_tx_zcal_pl_mask                0x800


#define ext_cmd_status_clear_mode_addr             0b000000111
#define ext_cmd_status_clear_mode_startbit         15
#define ext_cmd_status_clear_mode_width            1
#define ext_cmd_status_clear_mode_endbit           15
#define ext_cmd_status_clear_mode_shift            0
#define ext_cmd_status_clear_mode_mask             0x1


#define fw_bist_en_addr                            0b000000110
#define fw_bist_en_startbit                        14
#define fw_bist_en_width                           1
#define fw_bist_en_endbit                          14
#define fw_bist_en_shift                           1
#define fw_bist_en_mask                            0x2


#define fw_bist_pcie_rate_addr                     0b000000110
#define fw_bist_pcie_rate_startbit                 11
#define fw_bist_pcie_rate_width                    3
#define fw_bist_pcie_rate_endbit                   13
#define fw_bist_pcie_rate_shift                    2
#define fw_bist_pcie_rate_mask                     0x1c


#define fw_debug_addr                              0b000001110
#define fw_debug_startbit                          0
#define fw_debug_width                             16
#define fw_debug_endbit                            15
#define fw_debug_shift                             0
#define fw_debug_mask                              0xffff


#define fw_gcr_bus_id_addr                         0b000000110
#define fw_gcr_bus_id_startbit                     0
#define fw_gcr_bus_id_width                        6
#define fw_gcr_bus_id_endbit                       5
#define fw_gcr_bus_id_shift                        10
#define fw_gcr_bus_id_mask                         0xfc00


#define fw_num_lanes_rx_addr                       0b000000111
#define fw_num_lanes_rx_startbit                   0
#define fw_num_lanes_rx_width                      5
#define fw_num_lanes_rx_endbit                     4
#define fw_num_lanes_rx_shift                      11
#define fw_num_lanes_rx_mask                       0xf800


#define fw_num_lanes_tx_addr                       0b000000111
#define fw_num_lanes_tx_startbit                   5
#define fw_num_lanes_tx_width                      5
#define fw_num_lanes_tx_endbit                     9
#define fw_num_lanes_tx_shift                      6
#define fw_num_lanes_tx_mask                       0x7c0


#define fw_pcie_mode_addr                          0b000000110
#define fw_pcie_mode_startbit                      8
#define fw_pcie_mode_width                         1
#define fw_pcie_mode_endbit                        8
#define fw_pcie_mode_shift                         7
#define fw_pcie_mode_mask                          0x80


#define fw_serdes_16_to_1_mode_addr                0b000000110
#define fw_serdes_16_to_1_mode_startbit            6
#define fw_serdes_16_to_1_mode_width               1
#define fw_serdes_16_to_1_mode_endbit              6
#define fw_serdes_16_to_1_mode_shift               9
#define fw_serdes_16_to_1_mode_mask                0x200


#define fw_spread_en_addr                          0b000000110
#define fw_spread_en_startbit                      7
#define fw_spread_en_width                         1
#define fw_spread_en_endbit                        7
#define fw_spread_en_shift                         8
#define fw_spread_en_mask                          0x100


#define fw_stop_thread_addr                        0b000000110
#define fw_stop_thread_startbit                    15
#define fw_stop_thread_width                       1
#define fw_stop_thread_endbit                      15
#define fw_stop_thread_shift                       0
#define fw_stop_thread_mask                        0x1


#define fw_thread_stopped_addr                     0b000001111
#define fw_thread_stopped_startbit                 0
#define fw_thread_stopped_width                    1
#define fw_thread_stopped_endbit                   0
#define fw_thread_stopped_shift                    15
#define fw_thread_stopped_mask                     0x8000


#endif //_PPE_FW_REG_CONST_IOO_PKG_H_
