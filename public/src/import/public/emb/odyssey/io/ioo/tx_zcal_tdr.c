/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_zcal_tdr.c $   */
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
// *! FILENAME    : tx_zcal_tdr.c
// *! TITLE       :
// *! DESCRIPTION : Run tx duty cycle correction
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *! DETAILED DESCRIPTION :
// *!   Description of shared functions between odyssey and 5nm applications
// *!    In general, this code does not assume the segment control registers
// *!     are in a particular state upon entry. Therefor, all select and
// *!     enable bits must be written to a 1 which selects all available
// *!     segments for main
// *!   For odyssey, the segments should be disabled in main, followed by
// *!     pre2, then pre1
// *!   For 5nm, the segments may be disabled in any order
// *!   Odyssey can be thought of as main & pre2 & pre1, with each of
// *!     these having an msb 2r segment, followed by a number of 1r segments
// *!     main is composed of main_0_15 & main_16_24
// *!   5nm can be thought of as (main & post) & pre2 & pre1
// *!     main is simply main in this case however, with some special
// *!     handling of the post 2r segment we will share code
// *!   code sharing takes advantage of:
// *!     main(5nm) = main_0_15[0-12](odyssey)
// *!     post(5nm) = main_16_24(odyssey) with the first post segment 2r
// *!        rather than 1r
// *!     pre1(odyssey) = pre1[0-4](5nm)
// *!     pre2(5nm) = pre2(odyssey)
// *!     each register field is in its own 16 bit register with unused
// *!        bits of a particular design unconnected
// *!   chart of aliases, partial aliases for enables:
// *!     odyssey            5nm
// *!     -------            ---
// *!     main_0_15[0-12]    main
// *!     main_16_24         post, except post bit 0 is 2r
// *!     pre2               pre2
// *!     pre1               pre1[0-4]
// *!   chart of aliases, partial aliases for selects:
// *!     odyssey            5nm
// *!     -------            ---
// *!     na                 post * no overlap or equivalent odyssey
// *!     pseg_pre2          pre2
// *!     pseg_pre1          pre1[0-4]
// *!     nseg_pre2          na * no overlap or equivalent 5nm
// *!     nseg_pre1          na * no overlap or equivalent 5nm
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// jjb23020100 |jjb     | Issue 298467: Made io_waits conditional on ppe_sim_speedup.
// gap22101300 |gap     | If IOO, set to wide track and hold pulse during zcal
// mbs22082601 |mbs     | Updated with PSL comments
// gap22071800 |gap     | Remove test PSL_* controls
// gap22052300 |gap     | Minor changes to reduce code size
// gap22030100 |gap     | Update to restore odyssey function
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Use 5nm regs in a way that makes all segments available for zcal
// bja21101801 |bja     | Remove half_width_mode=true case
// bja21101800 |bja     | Use all available PRE1 segs
// bja21061802 |bja     | RMW pre2 enables because p and n segments share the same register
// bja21061801 |bja     | RMW tx_tdr_phase_sel because it shares a reg with more fields now
// bja21061800 |bja     | Clear FFE selects before starting
// bja21060800 |bja     | Remove tx_pseg_main_16_24_hs_en, which simplifies code
// gap21021800 |gap     | Add test PSL_* controls
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// gap21012800 |gap     | Change tx_zcal_tdr_split_main_therm HW558264
// gap20092200 |gap     | Update for 32:1 in 16:1 mode
// gap20040100 |gap     | Commented out some superfluous set_debug_state statements
// gap20032600 |gap     | Added wait time for th after changing segments
// gap19041000 |gap     | Created
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "tx_zcal_tdr.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "io_config.h"

#define DBG_LVL 3 // debug on each branch
#define DEEP_DBG_LVL 3 // debug on each write

void tx_zcal_tdr (t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xC100); // tx_zcal_tdr begin
    int thread_l = 0;
    bool is_5nm_l = !is_odyssey();
    thread_l = get_gcr_addr_thread(gcr_addr_i);

    // setup tdr; offset reg value is a function of what is being called half_width_mode
    uint32_t tdr_offset_l = 16 * tx_zcal_tdr_sample_position_c;

    // if IOO, save tx_tdr_th_pw_sel and overwrite to widest width
#ifdef IOO
    uint32_t tx_tdr_th_pw_sel_save_l = get_ptr_field(gcr_addr_i, tx_tdr_th_pw_sel);
    put_ptr_field(gcr_addr_i, tx_tdr_th_pw_sel, 0b00, read_modify_write); // set to wider pulse for zcal
#endif

    set_debug_state(0xC160, DBG_LVL); // tx_tdr_pulse_offset
    put_ptr_field(gcr_addr_i, tx_tdr_pulse_offset, tdr_offset_l, read_modify_write);
    set_debug_state(0xC161, DEEP_DBG_LVL); // tx_tdr_pulse_width
    put_ptr_field(gcr_addr_i, tx_tdr_pulse_width, tx_zcal_tdr_pulse_width_c , read_modify_write);
    set_debug_state(0xC162, DEEP_DBG_LVL); // tx_tdr_enable
    put_ptr_field(gcr_addr_i, tx_tdr_enable, 0b1, read_modify_write);

    // *!   chart of aliases, partial aliases for selects:
    // *!     odyssey            5nm
    // *!     -------            ---
    // *!     na                 post * no overlap or equivalent odyssey
    // *!     pseg_pre2          pre2
    // *!     pseg_pre1          pre1[0-4]
    // *!     nseg_pre2          na * no overlap or equivalent 5nm
    // *!     nseg_pre1          na * no overlap or equivalent 5nm
    // For this code we always want the selects to be written to 0
    set_debug_state(0xC163, DEEP_DBG_LVL); //     tx_post_sel
    put_ptr_field_fast(gcr_addr_i, tx_post_sel, 0x0);
    set_debug_state(0xC164, DEEP_DBG_LVL); //     tx_pre1_sel
    put_ptr_field_fast(gcr_addr_i, tx_pre1_sel, 0x0);
    set_debug_state(0xC165, DEEP_DBG_LVL); //     tx_pre2_sel
    put_ptr_field_fast(gcr_addr_i, tx_pre2_sel, 0x0);
    set_debug_state(0xC166, DEEP_DBG_LVL); //     tx_nseg_pre1_hs_sel
    put_ptr_field_fast(gcr_addr_i, tx_nseg_pre1_hs_sel, 0x0);
    set_debug_state(0xC167, DEEP_DBG_LVL); //     tx_nseg_pre2_hs_sel
    put_ptr_field_fast(gcr_addr_i, tx_nseg_pre2_hs_sel, 0x0);

    uint32_t current_pseg_pre2_l = tx_pseg_pre2_en_width * 2 - 1 ;
    uint32_t current_pseg_pre1_l = tx_pseg_pre1_hs_en_width * 2 - 1 ;
    uint32_t current_pseg_main_l = (tx_pseg_main_0_15_hs_en_width * 2 - 1) + (tx_pseg_main_16_24_hs_en_width * 2);

    // PSL 5nm
    if (is_5nm_l)
    {
        current_pseg_main_l -= 7 ; // 5nm is 3 1r bits shorter for main and post's first segment is 2r
        current_pseg_pre1_l += 8 ; // 5nm is 4 1r bits longer for pre1 than odyssey
    }

    uint32_t current_nseg_main_l = current_pseg_main_l ;
    uint32_t current_nseg_pre2_l = current_pseg_pre2_l ;
    uint32_t current_nseg_pre1_l = current_pseg_pre1_l ;

    set_tx_dcc_debug_tx_zcal_tdr(0xC1C1, current_pseg_main_l) ; //   current_pseg_main_l
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C2, current_nseg_main_l) ; //   current_nseg_main_l
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C3, current_pseg_pre1_l) ; //   current_pseg_pre1_l
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C4, current_nseg_pre1_l) ; //   current_nseg_pre1_l
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C5, current_pseg_pre2_l) ; //   current_pseg_pre2_l
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C6, current_nseg_pre2_l) ; //   current_nseg_pre2_l
    tx_zcal_tdr_write_en(gcr_addr_i, current_pseg_main_l, SEGTYPE_MAIN_PSEG, is_5nm_l);
    tx_zcal_tdr_write_en(gcr_addr_i, current_nseg_main_l, SEGTYPE_MAIN_NSEG, is_5nm_l);
    tx_zcal_tdr_write_en(gcr_addr_i, current_pseg_pre1_l, SEGTYPE_PRE1_PSEG, is_5nm_l);
    tx_zcal_tdr_write_en(gcr_addr_i, current_nseg_pre1_l, SEGTYPE_PRE1_NSEG, is_5nm_l);
    tx_zcal_tdr_write_en(gcr_addr_i, current_pseg_pre2_l, SEGTYPE_PRE2_PSEG, is_5nm_l);
    tx_zcal_tdr_write_en(gcr_addr_i, current_nseg_pre2_l, SEGTYPE_PRE2_NSEG, is_5nm_l);

    // do until !updated_pu_or_pd = 0
    bool updated_pu_or_pd_l = false;
    bool done_l = false;

    do
    {
        set_debug_state(0xC111, DEEP_DBG_LVL); // begin update loop
        //   updated_pu_or_pd = 0
        updated_pu_or_pd_l = false;
        //   set upper dac level
        //   set p phase
        //   done = false
        //   do until done
        //     if comparator matches 1 required number of times,
        //       if(tx_zcal_tdr_decrement_bank()), updated_pu_or_pd = 1
        //       else done = 1
        //     else done = 1
        //   set p phase
        //   set initial dac level
        //
        // written to 0 below put_ptr_field(gcr_addr_i, tx_tdr_phase_sel, 0, read_modify_write);
        set_debug_state(0xC168, DEEP_DBG_LVL); //    tx_tdr_dac_cntl
        put_ptr_field(gcr_addr_i, tx_tdr_dac_cntl, tx_zcal_tdr_dac_75percent_vio_c,
                      fast_write); // only other field in reg is tx_ , which we want 0 == P
        uint32_t l_ppe_sim_speedup = img_field_get(ppe_sim_speedup);

        // PSL ppe_sim_speedup_wait_1
        if (!l_ppe_sim_speedup)
        {
            io_wait_us(thread_l, tx_zcal_tdr_sw_wait_us_c);
        }

        done_l = false;

        do
        {
            // PSL pulled_too_high
            if (tx_zcal_tdr_capt_match_mult_rds(gcr_addr_i, 1,
                                                tx_zcal_tdr_matches_needed_c))   // pulled too high; need to reduce pullups
            {
                // PSL pullup_dec_success
                if (tx_zcal_tdr_decrement_bank(gcr_addr_i, SEGTYPE_MAIN_PSEG, is_5nm_l, &current_pseg_pre2_l, &current_pseg_pre1_l,
                                               &current_pseg_main_l))
                {
                    set_debug_state(0xC123, DBG_LVL); // pullup decrement successful
                    updated_pu_or_pd_l = true;

                    // PSL ppe_sim_speedup_wait_2
                    if (!l_ppe_sim_speedup)
                    {
                        io_wait_us(thread_l, tx_zcal_tdr_seg_wait_us_c);
                    }
                }
                else
                {
                    set_debug_state(0xC124, DBG_LVL); // pullup decrement unsuccessful
                    done_l = true;
                }
            }
            else
            {
                set_debug_state(0xC125, DBG_LVL); // pullup crossed threshold
                done_l = true;
            }
        } // pull up

        while (!done_l);

        //   set n phase
        //   set lower dac level
        //   done = false
        //   do until done
        //     if comparator matches 0 required number of times,
        //       if(tx_zcal_tdr_decrement_bank()), updated_pu_or_pd = 1
        //       else done = 1
        //     else done = 1
        //
        //     writing phase_sel first because it gives track and hold more time to track after the large change in level
        set_debug_state(0xC169, DEEP_DBG_LVL); //    tx_tdr_phase_sel
        put_ptr_field(gcr_addr_i, tx_tdr_phase_sel, 1, read_modify_write);
        set_debug_state(0xC16A, DEEP_DBG_LVL); //    tx_tdr_dac_cntl
        put_ptr_field(gcr_addr_i, tx_tdr_dac_cntl, tx_zcal_tdr_dac_25percent_vio_c, read_modify_write);

        // PSL ppe_sim_speedup_wait_3
        if (!l_ppe_sim_speedup)
        {
            io_wait_us(thread_l, tx_zcal_tdr_sw_wait_us_c);
        }

        done_l = false;

        do
        {
            // PSL pulled_too_low
            if (tx_zcal_tdr_capt_match_mult_rds(gcr_addr_i, 0,
                                                tx_zcal_tdr_matches_needed_c))   // pulled too low; need to decrease pulldowns
            {
                // PSL pulldown_dec_success
                if (tx_zcal_tdr_decrement_bank(gcr_addr_i, SEGTYPE_MAIN_NSEG, is_5nm_l, &current_nseg_pre2_l, &current_nseg_pre1_l,
                                               &current_nseg_main_l))
                {
                    set_debug_state(0xC133, DBG_LVL); // pulldown decrement successful
                    updated_pu_or_pd_l = true;

                    // PSL ppe_sim_speedup_wait_4
                    if (!l_ppe_sim_speedup)
                    {
                        io_wait_us(thread_l, tx_zcal_tdr_seg_wait_us_c);
                    }
                }
                else
                {
                    set_debug_state(0xC134, DBG_LVL); // pulldown decrement unsuccessful
                    done_l = true;
                }
            }
            else
            {
                set_debug_state(0xC135, DBG_LVL); // pulldown crossed threshold
                done_l = true;
            }
        } // pull down

        while (!done_l);
    } // outer update loop

    while (updated_pu_or_pd_l);

    //   disable tdr; some of these may not be necessary
    set_debug_state(0xC16B, DEEP_DBG_LVL); //  tx_tdr_enable
    put_ptr_field(gcr_addr_i, tx_tdr_enable, 0b0, read_modify_write);
    set_debug_state(0xC16C, DEEP_DBG_LVL); //  tx_tdr_pulse_width
    put_ptr_field(gcr_addr_i, tx_tdr_pulse_width, 0 , read_modify_write);
    set_debug_state(0xC16D, DEEP_DBG_LVL); //   tx_tdr_pulse_offset
    put_ptr_field(gcr_addr_i, tx_tdr_pulse_offset, 0, read_modify_write);

    // if IOO, restore tx_tdr_th_pw_sel
#ifdef IOO
    put_ptr_field(gcr_addr_i, tx_tdr_th_pw_sel, tx_tdr_th_pw_sel_save_l, read_modify_write);
#endif

    set_debug_state(0xC1FF); // tx_zcal_tdr end
} // tx_zcal_tdr


// In general, the enables are written here
// Also, in general, we are always either setting all segments or decreasing the
// number of segments; we can take advantage of this to save some writes
void tx_zcal_tdr_write_en (t_gcr_addr* gcr_addr_i, uint32_t num_2r_equiv_i, t_segtype segtype_i, bool is_5nm_i)
{
    uint32_t high_bits_l = 0;
    uint32_t low_bits_l = 0;

    switch(segtype_i)
    {
        case SEGTYPE_MAIN_PSEG:
            set_debug_state(0xC141, DBG_LVL); // write main_pseg

            // PSL segtype_main_pseg_16_24
            if (tx_zcal_tdr_split_main_therm (num_2r_equiv_i, tx_pseg_main_0_15_hs_en_width, tx_pseg_main_16_24_hs_en_width,
                                              is_5nm_i, &high_bits_l, &low_bits_l))  // write 16_24
            {
                set_debug_state(0xC142, DBG_LVL); // write main_pseg low bits
                put_ptr_field_fast(gcr_addr_i, tx_pseg_main_16_24_hs_en, low_bits_l);
            }

            set_debug_state(0xC16E, DEEP_DBG_LVL); //   tx_pseg_main_0_15_hs_en
            put_ptr_field_fast(gcr_addr_i, tx_pseg_main_0_15_hs_en, high_bits_l); // write odyssey regs, which are a superset of 5nm
            break;

        case SEGTYPE_MAIN_NSEG:
            set_debug_state(0xC143, DBG_LVL); // write main_nseg

            // PSL segtype_main_nseg_16_24
            if (tx_zcal_tdr_split_main_therm (num_2r_equiv_i, tx_nseg_main_0_15_hs_en_width, tx_nseg_main_16_24_hs_en_width,
                                              is_5nm_i, &high_bits_l, &low_bits_l))  // write 16_24
            {
                set_debug_state(0xC144, DBG_LVL); // write main_nseg low bits
                put_ptr_field_fast(gcr_addr_i, tx_nseg_main_16_24_hs_en, low_bits_l);
            }

            set_debug_state(0xC16F, DEEP_DBG_LVL); //             tx_nseg_main_0_15_hs_en
            put_ptr_field_fast(gcr_addr_i, tx_nseg_main_0_15_hs_en, high_bits_l); // write odyssey regs, which are a superset of 5nm
            break;

        case SEGTYPE_PRE2_PSEG:
            // PSL segtype_pre2_pseg_break
            set_debug_state(0xC149, DBG_LVL); // write pre2 pseg
            put_ptr_field_fast(gcr_addr_i, tx_pseg_pre2_hs_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_pseg_pre2_en_width));
            break;

        case SEGTYPE_PRE2_NSEG:
            // PSL segtype_pre2_nseg_break
            set_debug_state(0xC14A, DBG_LVL); // write pre2 nseg
            put_ptr_field_fast(gcr_addr_i, tx_nseg_pre2_hs_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_nseg_pre2_en_width));
            break;

        case SEGTYPE_PRE1_PSEG:
            set_debug_state(0xC14B, DBG_LVL); // write pre1 pseg

            // PSL segtype_pre1_pseg_5nm
            if (is_5nm_i)   // pre1 5nm and odyssey have different widths
            {
                set_debug_state(0xC170, DEEP_DBG_LVL); //                 tx_pseg_pre1_en
                put_ptr_field_fast(gcr_addr_i, tx_pseg_pre1_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_pseg_pre1_en_width));
            }
            else
            {
                set_debug_state(0xC171, DEEP_DBG_LVL); //                 tx_pseg_pre1_hs_en
                put_ptr_field_fast(gcr_addr_i, tx_pseg_pre1_hs_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_pseg_pre1_hs_en_width));
            }

            break;

        case SEGTYPE_PRE1_NSEG:
            set_debug_state(0xC14C, DBG_LVL); // write pre1 nseg

            // PSL segtype_pre1_nseg_5nm
            if (is_5nm_i)   // pre1 5nm and odyssey have different widths
            {
                set_debug_state(0xC172, DEEP_DBG_LVL); //                 tx_nseg_pre1_en
                put_ptr_field_fast(gcr_addr_i, tx_nseg_pre1_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_nseg_pre1_en_width));
            }
            else
            {
                set_debug_state(0xC173, DEEP_DBG_LVL); //                 tx_nseg_pre1_hs_en
                put_ptr_field_fast(gcr_addr_i, tx_nseg_pre1_hs_en, tx_ffe_toThermWithHalf(num_2r_equiv_i, tx_nseg_pre1_hs_en_width));
            }

            break;
    } // switch segtype_i
} // tx_zcal_tdr_write_en

// P or N is passed in as SEGTYPE_MAIN_PSEG or  SEGTYPE_MAIN_NSEG
// remove main, then pre2, then pre1
bool tx_zcal_tdr_decrement_bank(t_gcr_addr* gcr_addr_i, t_segtype segtype_i, bool is_5nm_i, uint32_t* current_pre2_io,
                                uint32_t* current_pre1_io, uint32_t* current_main_io)
{
    bool value_updated_l = false ;

    // PSL main_gt_0
    if (*current_main_io > 0)
    {
        set_debug_state(0xC151, DBG_LVL); // decrement main bank
        *current_main_io -= 1;
        tx_zcal_tdr_write_en(gcr_addr_i, *current_main_io, segtype_i, is_5nm_i);
        value_updated_l = true ;
    }
    // PSL pre2_gt_0
    else if (*current_pre2_io > 0)
    {
        set_debug_state(0xC152, DBG_LVL); // decrement pre2 bank
        *current_pre2_io -= 1 ;
        tx_zcal_tdr_write_en(gcr_addr_i, *current_pre2_io,
                             (segtype_i == SEGTYPE_MAIN_PSEG) ? SEGTYPE_PRE2_PSEG : SEGTYPE_PRE2_NSEG, is_5nm_i);
        value_updated_l = true ;
    }
    // PSL pre1_gt_0
    else if (*current_pre1_io > 0)
    {
        set_debug_state(0xC153, DBG_LVL); // decrement pre1 bank
        *current_pre1_io -= 1 ;
        tx_zcal_tdr_write_en(gcr_addr_i, *current_pre1_io,
                             (segtype_i == SEGTYPE_MAIN_PSEG) ? SEGTYPE_PRE1_PSEG : SEGTYPE_PRE1_NSEG, is_5nm_i);
        value_updated_l = true ;
    }
    else
    {
        set_debug_state(0xC154, DBG_LVL); // could not decrement bank
        value_updated_l = false;
    }

    return value_updated_l;
} // tx_zcal_tdr_decrement_bank

bool tx_zcal_tdr_capt_match_mult_rds(t_gcr_addr* gcr_addr_i, uint32_t match_value_i, uint32_t times_i)   //PSL NAMEX
{
    bool value_matched_l = true ;
    uint32_t times_matched_l = 0;

    for (times_matched_l = 0; times_matched_l < times_i; times_matched_l++)   //PSL
    {
        set_debug_state(0xC1C0, DBG_LVL); // get  tx_tdr_capt_val
        value_matched_l = (get_ptr_field(gcr_addr_i, tx_tdr_capt_val) == match_value_i);

        // PSL value_matched
        if (!value_matched_l)   // doing explicit conditional to allow coverage testing
        {
            // sim model generally does not include noise, so, unless we are lucky, we will see consistent
            // values on all reads in this loop; thus the debug_state is not very informative even in sim
            break;
        }
    }

    return value_matched_l;
} // tx_zcal_tdr_capt_match_mult_rds

/**
 * helper routine that determines whether to write to both main therm-code seg enable fields
 *   and the values to write
 * Assumes two fields, when concatenated, form a therm code with msb a value of '1',
 * all other bits a value of 2 with msb first cleared/last set.
 * As currently implemented, the first field is *_0_15_* and second is *_16_24_*
 * Returns true if second field is to be written
 *
 * This assumes we are either enabling all segments or decrementing segments
 * with this assumption, we only need to write the lsb's when we are writing
 * all segments or when some of the low order segments are disabled and the
 * value is odd.
 *
 * Note, when decrementing and some low order segments are disabled and the
 * value is odd; this operation will set the 2r bit in the high register
 * and clear a bit in the low register. Here is the sequence of values assuming
 * widths of 16 and 9:
 *
 *  high             low       low written
 * 1111111111111111 111111111 <<
 * 0111111111111111 111111111
 * 1011111111111111 111111111
 * ...
 * 1000000000000000 111111111
 * 0000000000000000 111111111
 * 1000000000000000 011111111 <<
 * 0000000000000000 011111111
 * 1000000000000000 001111111 <<
 * ...
 * 1000000000000000 000000001 <<
 * 0000000000000000 000000000
 */
bool tx_zcal_tdr_split_main_therm (const uint32_t num_2r_equiv_i, uint32_t high_width_i, uint32_t low_width_i,
                                   bool is_5nm_i, uint32_t* high_bits_io, uint32_t* low_bits_io)
{
    bool write_low_bits_l;
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C7, num_2r_equiv_i) ; // num_2r_equiv_i
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C8, high_width_i) ;   // high_width_i
    set_tx_dcc_debug_tx_zcal_tdr(0xC1C9, low_width_i) ;    // low_width_i
    set_tx_dcc_debug_tx_zcal_tdr(0xC1CA, is_5nm_i) ;       // is_5nm_i
    set_tx_dcc_debug_tx_zcal_tdr(0xC1CB, high_width_i) ;   // high_width_i

    // PSL 5nm
    if (is_5nm_i)
    {
        uint32_t low_val_l = min(low_width_i * 2 - 1, num_2r_equiv_i);
        uint32_t high_val_l = num_2r_equiv_i - low_val_l;
        set_tx_dcc_debug_tx_zcal_tdr(0xC1CC, low_val_l) ;   // low_val_l
        set_tx_dcc_debug_tx_zcal_tdr(0xC1CD, high_val_l) ;  // high_val_l

        // PSL 5nm_high_val_gt_0
        if (high_val_l > 0)
        {
            high_val_l += 6; // this is equivalent to shifting 3 1's in from the right if the original value is non-zero
        }

        set_tx_dcc_debug_tx_zcal_tdr(0xC1CE, high_val_l) ;  // high_val_l
        *high_bits_io = tx_ffe_toThermWithHalf(high_val_l, high_width_i);
        *low_bits_io = tx_ffe_toThermWithHalf(low_val_l, low_width_i);
        set_tx_dcc_debug_tx_zcal_tdr(0xC1CF, *high_bits_io) ;  // *high_bits_io
        set_tx_dcc_debug_tx_zcal_tdr(0xC1D0, *low_bits_io) ;   // *low_bits_io
        // HW558264
        // high_width_i and low_width_i represent numbers of 2r segments, with the exception of 1 bit of high_width_i
        // Thus, in the computation below, these terms are multiplied by 2 to convert to 2r-equivalent segments before comparison
        // with the 2r_equiv_i desired result.
        // write low bits if either all bits are written or low bits are written to something other than all 1's
        write_low_bits_l = (num_2r_equiv_i == ((high_width_i + low_width_i) * 2) - 8)
                           || (num_2r_equiv_i < ((low_width_i * 2) - 1));
    }
    else
    {
        uint32_t full_therm_l = 0;
        full_therm_l = tx_ffe_toThermWithHalf(num_2r_equiv_i, low_width_i + high_width_i);
        *high_bits_io = full_therm_l >> low_width_i;
        *low_bits_io = full_therm_l & ((0x1 << low_width_i) - 1);
        // HW558264
        // high_width_i and low_width_i represent numbers of 2r segments, with the exception of 1 bit of high_width_i
        // Thus, in the computation below, these terms are multiplied by 2 to convert to 2r-equivalent segments before comparison
        // with the 2r_equiv_i desired result.
        // write low bits if either all bits are written or (low bits are written to something other than all 1's and value is odd)
        write_low_bits_l = (num_2r_equiv_i == ((high_width_i + low_width_i) * 2) - 1) || ((num_2r_equiv_i < (low_width_i * 2))
                           && ((num_2r_equiv_i & 0x1) == 1)) ;
    }

    return write_low_bits_l;
} //tx_zcal_tdr_split_main_therm
