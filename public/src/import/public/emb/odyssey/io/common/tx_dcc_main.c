/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_dcc_main.c $ */
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
// *!---------------------------------------------------------------------------
// *! FILENAME    : tx_dcc_main.c
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
// gap23040400 |gap     | Removed gen1, 2 branch since dcc is not used in pcie gen1 or 2
// vbr22111700 |vbr     | Added a sleep
// gap22090800 |gap     | Updated range of tune bits for iot
// mbs22083000 |mbs     | PSL comment updates
// gap22080300 |gap     | EWM285666 io_sleep added to fix thread_active_timeout on back-to-back calls
// gap22071800 |gap     | EWM284157 change reg read to determine tx speed
// jjb22062700 |jjb     | Removed 5nm qualifiers from tx patgen code
// jjb22062100 |jjb     | Updated tx pattern generator pcie controls
// vbr22011800 |vbr     | Added a status return that needs to be updated with pass/fail
// bja21101801 |bja     | Unused variables are an error, so separate IOO and IOT tune variables
// bja21101800 |bja     | Use macros to unify DCC tune handling between 5nm and 7nm
// mwh21092300 |mwh     | Moved to common to and removed out of tx_dcc_main.c to support iot and ioo
// bja21060800 |bja     | Modified gray code functions for 5nm
// vbr21052100 |vbr     | Added sleeps to reduce thread blocking
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr21020900 |vbr     | Removed debug writes and went back to io_wait
// vbr20111300 |vbr     | HW552111: Replaced 1us io_wait with a sleep. Added some debug writes that should probably be removed when issue is fully resolved.
// bja20092800 |bja     | Use shared constants for TX register overrides
// gap20100800 |gap     | Backout repmux for P11
// bja20091500 |bja     | HW544277: Use main path by default for dcc
// bja20091400 |bja     | HW544277: Prevent replica path in anything but p10 dd1
// bja20090900 |bja     | Use common is_p10_dd1() check
// gap20082500 |gap     | HW542315 correct repeating pattern when in half-width mode
// gap20052100 |gap     | Added power up/down controls for dd2, repmux path select dd1 cq521314
// gap20050200 |gap     | Add main and pad dcc error propagation
// gap19091000 |gap     | Change rx_dcc_debug to tx_dcc_debug HW503432
// mbs19090500 |mbs     | Updated tx_dcc_main_min_samples mem_reg for sim speedup
// vbr19081300 |vbr     | Removed mult_int16 (not needed for ppe42x)
// gap19073000 |gap     | Updated to use modified dcdet circuitry and associated state machine
// gap19061800 |gap     | Changed from tx_dcc_out bit to tx_dcc_out_vec, still using one bit
// vbr19051700 |vbr     | Updated multiply by -1 to not use multiplier.
// gap19061300 |gap     | Changed from io_wait to io_wait_us
// gap19061300 |gap     | Added wait time for auto zero
// gap19031300 |gap     | Rename TwosCompTo* to IntTo*
// gap19030600 |gap     | Changed i_tune, q_tune and iq_tune to customized gray code
// vbr18081500 |vbr     | Including ioo_common.h for return codes.
// gap18042700 |gap     | Created
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "tx_dcc_main.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

#include "tx_dcc_main_servo.h"

////////////////////////////////////////////////////////////////////////////////////
// DCC
// Run Duty cycle correction initialization
////////////////////////////////////////////////////////////////////////////////////
int tx_dcc_main_init(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xD010); // init start

    uint32_t tx_dcc_main_min_samples_int = mem_pg_field_get(tx_dcc_main_min_samples);

#ifdef IOT
    put_ptr_field(gcr_addr_i, tx_dcc_tune,     IntToGrayOffset(0, tx_dcc_tune_width, tx_dcc_tune_offset_iot),
                  read_modify_write);
#endif

#ifdef IOO
    SET_TX_TUNE_CONSTANTS
    put_ptr_field(gcr_addr_i, tx_dcc_i_tune,  IntToGrayOffset(0, tx_dcc_i_tune_width_const,  tx_dcc_i_tune_offset_const),
                  read_modify_write);
    put_ptr_field(gcr_addr_i, tx_dcc_q_tune,  IntToGrayOffset(0, tx_dcc_q_tune_width_const,  tx_dcc_q_tune_offset_const),
                  read_modify_write);
    put_ptr_field(gcr_addr_i, tx_dcc_iq_tune, IntToGrayOffset(0, tx_dcc_iq_tune_width_const, tx_dcc_iq_tune_offset_const),
                  read_modify_write);
#endif

    // power on dcc
    put_ptr_field(gcr_addr_i, tx_bank_controls_dcc_alias,  0b0, read_modify_write); //pl power-on, active low

    io_sleep(get_gcr_addr_thread(gcr_addr_i));

#ifdef IOO
    put_ptr_field(gcr_addr_i, tx_tdr_enable,      0b0,     read_modify_write);
    int l_pcie_mode = fw_field_get(fw_pcie_mode);

    // for pcie, dcc only runs in gen3, 4, or 5; so, gen1 and 2 are not enabled or supported
    // PSL pcie_mode
    if (l_pcie_mode == 1)   // Update ppe_data_rate to reflect PCIe pipe_state_rate
    {
        // writing tx_pattern_bus_width_sel in case bist was last run in gen1 or gen2 mode before running dcc
        put_ptr_field(gcr_addr_i, tx_pattern_bus_width_sel, 0b0 , read_modify_write);  // 32 bit mode
        uint32_t l_tx_pcie_clk_sel = get_ptr_field(gcr_addr_i, tx_pcie_clk_sel); // 1 hot; bits 27-31 --> gen5-gen1
        uint32_t l_tx_pattern_gear_ratio = 15 >> (31 - __builtin_clz(l_tx_pcie_clk_sel))
                                           ; // uint32 0x10, 0x08, 0x04, 0x02, 0x01--> 0, 1, 3, 7, 15
        put_ptr_field(gcr_addr_i, tx_pattern_gear_ratio, l_tx_pattern_gear_ratio , read_modify_write);
    }

#endif

    put_ptr_field(gcr_addr_i, tx_pattern_enable,  0b1,     read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_sel,     0b001,   read_modify_write);

#ifdef IOT
    tx_dcc_main_servo(gcr_addr_i, tx_dcc_main_max_step_i_c,  tx_dcc_main_dir_i_c,  SERVOOP_I, tx_dcc_main_min_i_iot,
                      tx_dcc_main_max_i_iot,  tx_dcc_main_min_samples_int, tx_dcc_main_ratio_thresh_c);
#endif

#ifdef IOO
    SET_TUNE_LIMITS
    tx_dcc_main_servo(gcr_addr_i, tx_dcc_main_max_step_i_c,  tx_dcc_main_dir_i_c,  SERVOOP_I, tx_dcc_main_min_i_c,
                      tx_dcc_main_max_i_c,  tx_dcc_main_min_samples_int, tx_dcc_main_ratio_thresh_c);
    tx_dcc_main_servo(gcr_addr_i, tx_dcc_main_max_step_q_c,  tx_dcc_main_dir_q_c,  SERVOOP_Q, tx_dcc_main_min_q_c,
                      tx_dcc_main_max_q_c,  tx_dcc_main_min_samples_int, tx_dcc_main_ratio_thresh_c);
    tx_dcc_main_servo(gcr_addr_i, tx_dcc_main_max_step_iq_c, tx_dcc_main_dir_iq_c, SERVOOP_IQ, tx_dcc_main_min_iq_c,
                      tx_dcc_main_max_iq_c, tx_dcc_main_min_samples_int, tx_dcc_main_ratio_thresh_c);
#endif

    put_ptr_field(gcr_addr_i, tx_pattern_enable,  0b0,     read_modify_write);
    put_ptr_field(gcr_addr_i, tx_pattern_sel,     0b000,   read_modify_write);

    // power off dcc
    put_ptr_field(gcr_addr_i, tx_bank_controls_dcc_alias,  0b1, read_modify_write); //pl power-on, active low

    set_debug_state(0xD01F); // init end
    io_sleep(get_gcr_addr_thread(gcr_addr_i)); // sleep to fix thread_active_timeout on back-to-back calls
    return pass_code; // Per Issue 267096 - there is no fail condition for TX DCC
} //tx_dcc_main_init
