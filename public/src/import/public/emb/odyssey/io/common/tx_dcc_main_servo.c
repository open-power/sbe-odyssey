/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_dcc_main_servo.c $ */
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
// gap23012500 |gap     | Issue 293532 log when tune bit hits min or max bound
// gap22090800 |gap     | Update iot offset
// mbs22083000 |mbs     | PSL comment updates
// gap22080300 |gap     | Update debug controls
// bja21101800 |bja     | Unify DCC tune handling between 5nm and 7nm
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
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


#include "tx_dcc_main_servo.h"
#include "tx_dcc_tune_constants.h"
#include "io_logger.h"

// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value is 2
#define TX_DCC_MAIN_SERVO_DBG_LVL 3

////////////////////////////////////////////////////////////////////////////////////
// DCC servo
// Run Duty cycle servo to move towards or over edge with finer steps downto a step size of 1
////////////////////////////////////////////////////////////////////////////////////
void tx_dcc_main_servo(t_gcr_addr* gcr_addr_i, uint32_t step_size_i, int32_t dir_i, t_servoop op_i, int32_t min_tune_i,
                       int32_t max_tune_i, uint32_t min_samples_i, int32_t ratio_thresh_i)
{

    set_tx_dcc_debug(0xD071, step_size_i); //step_size_i
    set_tx_dcc_debug(0xD072, dir_i); //temp debug
    set_tx_dcc_debug(0xD073, min_tune_i); //temp debug
    set_tx_dcc_debug(0xD074, max_tune_i); //temp debug
    set_tx_dcc_debug(0xD075, op_i); //temp debug

    t_comp_result comp_decision_l = COMP_RESULT_P_NEAR_N;
    t_comp_result initial_comp_decision_l = COMP_RESULT_P_NEAR_N;
    int dcc_next_tune_l = 0;
    int dcc_last_tune_l = 0;
    int adj_l = 0;
    int step_l = 0;
    int step_size_l = 0;
    bool done = false;
    bool saved_compare = false;
    int thread_l = 0;

    // Notes on bit alignment for clk_pat pattern gen
    //    The bits are swizzled at the main mux as follows:
    //       data_in[1:3,0]<->D4[0:3]
    //    So, in order to obtain a desired D4[0:3] pattern, the
    //       same as the applicable tx_dcc_pat, we must swizzle
    //       the repeating data/clock pattern as follows:
    //         data_in[0:3]<->D4[3,0:2]
    //
    //    BUS   Phase     Desired control      Repeating clk pattern
    //    IOO     I           0b0110                 0011
    //    IOO     Q           0b0011                 1001
    //    IOO     IQ          0b0101                 1010
    //    IOT     I           0b0101                 1010

#ifdef IOT

    switch(op_i)
    {
        case SERVOOP_I:
            // PSL servoop_i
            set_debug_state(0xD031, TX_DCC_MAIN_SERVO_DBG_LVL); // servo init i
            tx_write_4_bit_pat(gcr_addr_i, 0b1010);
            dcc_last_tune_l = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_tune), tx_dcc_tune_width, tx_dcc_tune_offset_iot) ;
            break;
    }

#endif

#ifdef IOO
    SET_TX_TUNE_CONSTANTS

    switch(op_i)
    {
        case SERVOOP_I:
            // PSL servoop_i
            set_debug_state(0xD031, TX_DCC_MAIN_SERVO_DBG_LVL); // servo init i
            tx_write_4_bit_pat(gcr_addr_i, 0b0011);
            dcc_last_tune_l = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_i_tune), tx_dcc_i_tune_width_const,
                                              tx_dcc_i_tune_offset_const) ;
            break;

        case SERVOOP_Q:
            // PSL servoop_q
            set_debug_state(0xD032, TX_DCC_MAIN_SERVO_DBG_LVL); // servo init q
            tx_write_4_bit_pat(gcr_addr_i, 0b1001);
            dcc_last_tune_l = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_q_tune), tx_dcc_q_tune_width_const,
                                              tx_dcc_q_tune_offset_const) ;
            break;

        case SERVOOP_IQ:
            // PSL servoop_iq
            set_debug_state(0xD033, TX_DCC_MAIN_SERVO_DBG_LVL); // servo init iq
            tx_write_4_bit_pat(gcr_addr_i, 0b1010);
            dcc_last_tune_l = GrayToIntOffset(get_ptr_field(gcr_addr_i, tx_dcc_iq_tune), tx_dcc_iq_tune_width_const,
                                              tx_dcc_iq_tune_offset_const) ;
            break;
    }

#endif

    thread_l = get_gcr_addr_thread(gcr_addr_i);
    step_size_l = step_size_i;
    saved_compare = false;
    done = false;

    uint16_t loop_count = 0;

    do
    {
        loop_count++;
        set_tx_dcc_debug(0xFAFA, loop_count);
        //lcl_put(scom_ppe_work1_lcl_addr, scom_ppe_work1_width, loop_count);
        //lcl_put(scom_ppe_work2_lcl_addr, scom_ppe_work2_width, loop_count+1);
        set_debug_state(0xD034); // servo do loop start

        io_wait_us(thread_l, tx_dcc_main_wait_tune_us_c);
        //io_sleep(thread_l);
        comp_decision_l = tx_dcc_main_compare_result(gcr_addr_i, min_samples_i, ratio_thresh_i);

        // PSL saved_compare
        if (!saved_compare)
        {
            initial_comp_decision_l = comp_decision_l;
            saved_compare = true;
        }

        step_l = step_size_l * dir_i;

        switch(comp_decision_l)
        {
            case COMP_RESULT_P_GT_N:
                // PSL comp_result_p_gt_n
                set_debug_state(0xD035, TX_DCC_MAIN_SERVO_DBG_LVL); // servo prelim reduce
                adj_l = -1 * step_l;
                break;

            case COMP_RESULT_P_LT_N:
                // PSL comp_result_p_lt_n
                set_debug_state(0xD036, TX_DCC_MAIN_SERVO_DBG_LVL); // servo prelim increase
                adj_l = step_l;
                break;

            case COMP_RESULT_P_NEAR_N:
                // PSL comp_result_p_near_n
                set_debug_state(0xD037, TX_DCC_MAIN_SERVO_DBG_LVL); // servo no change - done
                done = true;
                break;
        }

        set_tx_dcc_debug(0xD089, adj_l);
        set_tx_dcc_debug(0xD08A, dcc_last_tune_l);
        set_tx_dcc_debug(0xD08B, max_tune_i);
        set_tx_dcc_debug(0xD08C, min_tune_i);
        set_tx_dcc_debug(0xD08D, step_l);

        // PSL not_done_comp_decision
        if (!done)
        {
            // PSL adj_dcc_last_tune_gt_max
            if ((adj_l + dcc_last_tune_l) > max_tune_i)
            {
                // PSL dcc_last_tune_eq_max
                if (dcc_last_tune_l == max_tune_i)
                {
                    set_debug_state(0xD038, TX_DCC_MAIN_SERVO_DBG_LVL); // servo at max limit - done
                    done = true;
                }
                else
                {
                    set_debug_state(0xD039, TX_DCC_MAIN_SERVO_DBG_LVL); // servo tune to max
                    // there is a suboptimization issue with this
                    // in normal scenarios, one could see, with a step_size_i of 4,
                    // for example, this sequence of tune values:
                    //   max_tune_i - 1; max_tune_i; max_tune_i - 2; max_tune_i - 1
                    // With some work adjusting step_size_l, we could avoid this
                    //   though adjusting properly and generally, may be non-trivial
                    //   that approach needs to consider the possibilty that
                    //   dcc_last_tune_l may have started out of range
                    dcc_next_tune_l = max_tune_i;
                }
            }
            // PSL adj_dcc_last_tune_eq_max
            else if ((adj_l + dcc_last_tune_l) < min_tune_i)
            {
                // PSL dcc_last_tune_eq_min
                if (dcc_last_tune_l == min_tune_i)
                {
                    set_debug_state(0xD03A, TX_DCC_MAIN_SERVO_DBG_LVL); // servo at min limit - done
                    done = true;
                }
                else
                {
                    set_debug_state(0xD03B, TX_DCC_MAIN_SERVO_DBG_LVL); // servo tune to min
                    // see above for a suboptimization issue that is also present here
                    dcc_next_tune_l = min_tune_i;
                }
            }
            else
            {
                set_debug_state(0xD03C, TX_DCC_MAIN_SERVO_DBG_LVL); // servo use prelim adj
                dcc_next_tune_l = dcc_last_tune_l + adj_l;
            }
        }



        // PSL not_done_tune
        if (!done)
        {

#ifdef IOT

            switch(op_i)
            {
                case SERVOOP_I:
                    set_debug_state(0xD03D, TX_DCC_MAIN_SERVO_DBG_LVL); // servo update i
                    put_ptr_field(gcr_addr_i, tx_dcc_tune,   IntToGrayOffset(dcc_next_tune_l, tx_dcc_tune_width, tx_dcc_tune_offset_iot),
                                  read_modify_write);
                    break;
            }

#endif

#ifdef IOO

            switch(op_i)
            {
                case SERVOOP_I:
                    set_debug_state(0xD03D, TX_DCC_MAIN_SERVO_DBG_LVL); // servo update i
                    put_ptr_field(gcr_addr_i, tx_dcc_i_tune, IntToGrayOffset(dcc_next_tune_l, tx_dcc_i_tune_width_const,
                                  tx_dcc_i_tune_offset_const), read_modify_write);
                    break;

                case SERVOOP_Q:
                    set_debug_state(0xD03E, TX_DCC_MAIN_SERVO_DBG_LVL); // servo update q
                    put_ptr_field(gcr_addr_i, tx_dcc_q_tune, IntToGrayOffset(dcc_next_tune_l, tx_dcc_q_tune_width_const,
                                  tx_dcc_q_tune_offset_const), read_modify_write);
                    break;

                case SERVOOP_IQ:
                    set_debug_state(0xD041, TX_DCC_MAIN_SERVO_DBG_LVL); // servo update iq
                    put_ptr_field(gcr_addr_i, tx_dcc_iq_tune, IntToGrayOffset(dcc_next_tune_l, tx_dcc_iq_tune_width_const,
                                  tx_dcc_iq_tune_offset_const), read_modify_write);
                    break;
            }

#endif

            // PSL init_comp_decision_neq
            if((initial_comp_decision_l != comp_decision_l) | (step_size_l < step_size_i) | (step_size_l <= 1))
            {
                step_size_l = step_size_l >> 1;
                set_tx_dcc_debug(0xD091, step_size_l); //temp debug
            }

            dcc_last_tune_l = dcc_next_tune_l;
        }

        // PSL step_size_gt_0
    }
    while ((step_size_l > 0) & !done);

    // LOG when DCC result is at bounds
    //log data arg uint16_t; current op_i max 2 bits; current tune max 8 bits
    // logged word is:  binary SSGGGGG0BBBBBBBB
    // where SS is 00: SERVOOP_I; 01: SERVOOP_Q; 10: SERVOOP_IQ
    //       GGGGG    is 1 hot gen 54321; only for IOO
    //       BBBBBBBB is abs(bound), bound is min or max depending on fail
    if (dcc_last_tune_l >= max_tune_i)   // should never be > max_tune, but handling just in case
    {
        // PSL tune_at_high_bound
        set_debug_state(0xD042, TX_DCC_MAIN_SERVO_DBG_LVL); // log tune_at_high_bound
        uint32_t log_val_l = (op_i << 14) | max_tune_i;
#ifdef IOO
        log_val_l |= (get_ptr_field(gcr_addr_i, tx_pcie_clk_sel) << 9); // 1 hot; bits 27-31 --> gen5-gen1
#endif
        ADD_LOG(DEBUG_BIST_TX_DCC_MAX_FAIL, gcr_addr_i, log_val_l);
        set_tx_dcc_debug(0xD097, log_val_l);
    }
    else if (dcc_last_tune_l <= min_tune_i)   // should never be < min_tune, but handling just in case
    {
        // PSL tune_at_low_bound
        set_debug_state(0xD043, TX_DCC_MAIN_SERVO_DBG_LVL); //  log tune_at_low_bound
        uint32_t log_val_l = (op_i << 14) | (-1 * min_tune_i);
#ifdef IOO
        log_val_l |= (get_ptr_field(gcr_addr_i, tx_pcie_clk_sel) << 9); // 1 hot; bits 27-31 --> gen5-gen1
#endif
        ADD_LOG(DEBUG_BIST_TX_DCC_MIN_FAIL, log_val_l);
        set_tx_dcc_debug(0xD098, log_val_l);
    }

} //tx_dcc_main_servo

////////////////////////////////////////////////////////////////////////////////////
// DCC compare result
// Run compare state machine and return whether P >, <, or near N
////////////////////////////////////////////////////////////////////////////////////
t_comp_result tx_dcc_main_compare_result(t_gcr_addr* gcr_addr_i, uint32_t min_samples_i, int32_t ratio_thresh_i)
{

    t_comp_result comp_result_l = COMP_RESULT_P_NEAR_N;
    uint32_t vote_p_gt_n_l = 0;
    uint32_t vote_p_lt_n_l = 0;
    uint32_t vote_l = 0;
    uint32_t vote_total_l = 0;

    put_ptr_field(gcr_addr_i, tx_dcc_cmp_run, 1, read_modify_write);

    set_debug_state(0xD051); // compare_result loop begin

    do
    {
        io_sleep(get_gcr_addr_thread(gcr_addr_i));

        vote_l = get_ptr_field(gcr_addr_i, tx_dcc_cmp_cnt_alias) ; // up followed by down

        vote_p_gt_n_l = vote_l >> tx_dcc_cmp_down_cnt_width; // assumes alias will only contain up and down
        vote_p_lt_n_l = vote_l & ((0x1 << tx_dcc_cmp_down_cnt_width) - 1);
        vote_total_l = vote_p_lt_n_l + vote_p_gt_n_l;

        set_tx_dcc_debug(0xD099, vote_l);
        set_tx_dcc_debug(0xD09A, vote_p_lt_n_l);
        set_tx_dcc_debug(0xD09B, vote_p_gt_n_l);
        set_tx_dcc_debug(0xD09C, vote_total_l);
        set_tx_dcc_debug(0xD09D, min_samples_i);
        set_tx_dcc_debug(0xD09E, ratio_thresh_i);

        // PSL vote_lt_min
    }
    while (vote_total_l < min_samples_i);

    // PSL compare_p_lt_n
    if ((vote_p_lt_n_l >> ratio_thresh_i) > vote_p_gt_n_l)
    {
        set_debug_state(0xD052, TX_DCC_MAIN_SERVO_DBG_LVL); // compare_result p_lt_n
        comp_result_l = COMP_RESULT_P_LT_N;
    }
    // PSL compare_p_gt_n
    else if ((vote_p_gt_n_l >> ratio_thresh_i) > vote_p_lt_n_l)
    {
        set_debug_state(0xD053, TX_DCC_MAIN_SERVO_DBG_LVL); // compare_result p_gt_n
        comp_result_l = COMP_RESULT_P_GT_N;
    }
    else
    {
        set_debug_state(0xD054, TX_DCC_MAIN_SERVO_DBG_LVL); // compare_result p_near_n
        comp_result_l = COMP_RESULT_P_NEAR_N;
    }

    put_ptr_field(gcr_addr_i, tx_dcc_cmp_run, 0, read_modify_write);

    return comp_result_l;
}
