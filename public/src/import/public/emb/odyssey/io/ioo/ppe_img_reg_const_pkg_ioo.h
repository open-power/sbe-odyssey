/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ppe_img_reg_const_pkg_ioo.h $ */
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

#ifndef _PPE_IMG_REG_CONST_IOO_PKG_H_
#define _PPE_IMG_REG_CONST_IOO_PKG_H_



#define ppe_current_thread_addr                    0b000000100
#define ppe_current_thread_startbit                0
#define ppe_current_thread_width                   4
#define ppe_current_thread_endbit                  3
#define ppe_current_thread_shift                   12
#define ppe_current_thread_mask                    0xf000


#define ppe_debug_log_num_addr                     0b000000100
#define ppe_debug_log_num_startbit                 7
#define ppe_debug_log_num_width                    7
#define ppe_debug_log_num_endbit                   13
#define ppe_debug_log_num_shift                    2
#define ppe_debug_log_num_mask                     0x1fc


#define ppe_disable_thread_active_time_check_addr   0b000000001
#define ppe_disable_thread_active_time_check_startbit   1
#define ppe_disable_thread_active_time_check_width   1
#define ppe_disable_thread_active_time_check_endbit   1
#define ppe_disable_thread_active_time_check_shift   14
#define ppe_disable_thread_active_time_check_mask   0x4000


#define ppe_num_threads_addr                       0b000000000
#define ppe_num_threads_startbit                   0
#define ppe_num_threads_width                      4
#define ppe_num_threads_endbit                     3
#define ppe_num_threads_shift                      12
#define ppe_num_threads_mask                       0xf000


#define ppe_sim_speedup_addr                       0b000000001
#define ppe_sim_speedup_startbit                   2
#define ppe_sim_speedup_width                      1
#define ppe_sim_speedup_endbit                     2
#define ppe_sim_speedup_shift                      13
#define ppe_sim_speedup_mask                       0x2000


#define ppe_vio_volts_addr                         0b000000000
#define ppe_vio_volts_startbit                     8
#define ppe_vio_volts_width                        2
#define ppe_vio_volts_endbit                       9
#define ppe_vio_volts_shift                        6
#define ppe_vio_volts_mask                         0xc0


#define ppe_watchdog_select_sim_mode_addr          0b000000000
#define ppe_watchdog_select_sim_mode_startbit      4
#define ppe_watchdog_select_sim_mode_width         4
#define ppe_watchdog_select_sim_mode_endbit        7
#define ppe_watchdog_select_sim_mode_shift         8
#define ppe_watchdog_select_sim_mode_mask          0xf00


#define ucontroller_test_en_addr                   0b000000001
#define ucontroller_test_en_startbit               0
#define ucontroller_test_en_width                  1
#define ucontroller_test_en_endbit                 0
#define ucontroller_test_en_shift                  15
#define ucontroller_test_en_mask                   0x8000


#define ucontroller_test_stat_addr                 0b000000100
#define ucontroller_test_stat_startbit             4
#define ucontroller_test_stat_width                1
#define ucontroller_test_stat_endbit               4
#define ucontroller_test_stat_shift                11
#define ucontroller_test_stat_mask                 0x800


#endif //_PPE_IMG_REG_CONST_IOO_PKG_H_
