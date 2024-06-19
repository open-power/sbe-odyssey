/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_seg_test.c $ */
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
// *! FILENAME    : tx_seg_test.c
// *! TITLE       :
// *! DESCRIPTION : Run tx segment test
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Brian Albertson     Email: brian.j.albertson@ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mbs22082601 |mbs     | Updated with PSL comments
// gap22082400 |gap     | Add iot support
// gap22081000 |gap     | Refactored to reduce size, set additional tx pins during setup
// gap22080200 |gap     | Added P11 support
// gap22071800 |gap     | EWM284157 tx_pattern_sel changed to read_modify_write as reg now has additional bits
// jjb22070700 |jjb     | tx_pattern_en must be set high prior to tx_pattern_sel for proper gear ratio sync operation
// vbr22061500 |vbr     | Added returning of fail status for ext commands
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Support 5nm and 7nm segments
// bja21061800 |bja     | Explicitly clear FFE selects to maintain intent of fast writes
// bja21060800 |bja     | Rename segment enable and select registers
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// gap20052800 |gap     | Added more modes
// mwh20022401 |mwh     | Added Debug Logs
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// gap19121900 |gap     | Created
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include "io_lib.h"
#include "pk.h"
#include "tx_seg_test.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"

// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value is 2
#define TX_SEG_TEST_DBG_LVL 3

//---------
// Note on debug code:
//   debug code has been commented out with '//- '
//   this is because debug code is extensive, but will be very helpful
//   should the need arise.

////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test
//
// Run segment test on a lane
// gcr_addr is set to tx_group and lane before this is called
////////////////////////////////////////////////////////////////////////////////////

int tx_seg_test(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xC300); // tx_seg_test begin
    int status = rc_no_error;

    int64_t sr = 0; // emulated shift register
    t_tx_seg_regs prev_seg_regs = {0, 0, 0, 0, 0, 0, 0}; // intermediate data structure, previous
    t_tx_seg_regs new_seg_regs = {0, 0, 0, 0, 0, 0, 0};  // intermediate data structure, new

    tx_seg_test_setup(gcr_addr_i);

    uint32_t tx_seg_test_frc_2r_l = mem_pg_field_get(tx_seg_test_frc_2r); // 0 false, or 1 true
    uint32_t tx_seg_test_2r_seg_l = mem_pg_field_get(tx_seg_test_2r_seg); // 0 or 1
    uint32_t tx_seg_test_1r_segs_l = mem_pg_field_get(tx_seg_test_1r_segs); // 0 to 3

    set_tx_dcc_debug_tx_seg_test(0xC39D, tx_seg_test_frc_2r_l    );
    set_tx_dcc_debug_tx_seg_test(0xC39E, tx_seg_test_2r_seg_l    );
    set_tx_dcc_debug_tx_seg_test(0xC39F, tx_seg_test_1r_segs_l   );

    uint32_t first_loop = true;

    // PSL seg_test_next
    while(tx_seg_test_next_sr(&sr, tx_seg_test_frc_2r_l, tx_seg_test_2r_seg_l, tx_seg_test_1r_segs_l))
    {
        io_sleep(get_gcr_addr_thread(gcr_addr_i));
        tx_seg_test_sr_to_reg(sr, &new_seg_regs);
        tx_seg_test_update_seg_gcr(gcr_addr_i, &prev_seg_regs,  &new_seg_regs, first_loop);

#if defined(IOO) && TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
        uint32_t tx_pseg_pre1_en_val         = get_ptr(gcr_addr_i, tx_pseg_pre1_en_addr        , 0, 15);
        uint32_t tx_pre1_sel_val             = get_ptr(gcr_addr_i, tx_pre1_sel_addr            , 0, 15);
        uint32_t tx_nseg_pre1_en_val         = get_ptr(gcr_addr_i, tx_nseg_pre1_en_addr        , 0, 15);
        uint32_t tx_nseg_pre1_sel_val        = get_ptr(gcr_addr_i, tx_nseg_pre1_sel_addr       , 0, 15);
        uint32_t tx_pseg_pre2_en_val         = get_ptr(gcr_addr_i, tx_pseg_pre2_en_addr        , 0, 15);
        uint32_t tx_pre2_sel_val             = get_ptr(gcr_addr_i, tx_pre2_sel_addr            , 0, 15);
        uint32_t tx_nseg_pre2_en_val         = get_ptr(gcr_addr_i, tx_nseg_pre2_en_addr        , 0, 15);
        uint32_t tx_nseg_pre2_sel_val        = get_ptr(gcr_addr_i, tx_nseg_pre2_sel_addr       , 0, 15);
        uint32_t tx_post_sel_val             = get_ptr(gcr_addr_i, tx_post_sel_addr            , 0, 15);
        set_tx_dcc_debug_tx_seg_test(0xC3A0, tx_pseg_pre1_en_val         );
        set_tx_dcc_debug_tx_seg_test(0xC3A1, tx_pre1_sel_val             );
        set_tx_dcc_debug_tx_seg_test(0xC3A2, tx_nseg_pre1_en_val         );
        set_tx_dcc_debug_tx_seg_test(0xC3A3, tx_nseg_pre1_sel_val        );
        set_tx_dcc_debug_tx_seg_test(0xC3A4, tx_pseg_pre2_en_val         );
        set_tx_dcc_debug_tx_seg_test(0xC3A5, tx_pre2_sel_val             );
        set_tx_dcc_debug_tx_seg_test(0xC3A6, tx_nseg_pre2_en_val         );
        set_tx_dcc_debug_tx_seg_test(0xC3A7, tx_nseg_pre2_sel_val        );
        set_tx_dcc_debug_tx_seg_test(0xC3AA, tx_post_sel_val             );
#endif
#if TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
        uint32_t tx_nseg_main_16_24_hs_en_val = get_ptr(gcr_addr_i, tx_nseg_main_16_24_hs_en_addr , 0, 15);
        uint32_t tx_pseg_main_16_24_hs_en_val = get_ptr(gcr_addr_i, tx_pseg_main_16_24_hs_en_addr , 0, 15);
        uint32_t tx_nseg_main_0_15_hs_en_val  = get_ptr(gcr_addr_i, tx_nseg_main_0_15_hs_en_addr  , 0, 15);
        uint32_t tx_pseg_main_0_15_hs_en_val  = get_ptr(gcr_addr_i, tx_pseg_main_0_15_hs_en_addr  , 0, 15);

        set_tx_dcc_debug_tx_seg_test(0xC3A8, tx_nseg_main_16_24_hs_en_val );
        set_tx_dcc_debug_tx_seg_test(0xC3A9, tx_pseg_main_16_24_hs_en_val );
        set_tx_dcc_debug_tx_seg_test(0xC3AB, tx_nseg_main_0_15_hs_en_val );
        set_tx_dcc_debug_tx_seg_test(0xC3AC, tx_pseg_main_0_15_hs_en_val );
#endif
        status |= tx_seg_test_value(gcr_addr_i, 0xFFFF);
        status |= tx_seg_test_value(gcr_addr_i, 0x0000);
        prev_seg_regs = new_seg_regs;
        first_loop = false;

    } // while tx_seg_test_next_sr true

    tx_seg_test_restore(gcr_addr_i);

    set_debug_state(0xC3FF); // tx_seg_test end
    return status;
} // tx_seg_test

////////////////////////////////////////////////////////////////////////////////////
// Mode setting summary
//
//  fix_2r       == tx_seg_test_frc_2r
//  num_2r       == tx_seg_test_2r_seg
//  num_1r_equiv == tx_seg_test_1r_segs
//
//  reg settings:               | 2r_equiv     Segments
//  fix_2r num_2r  num_1r_equiv | during test  during test      Comment
//  ------ ------  ------------ | -----------  -----------      -------
//                              |
//    0      0            0     |    1-2       1-2r or 1-1r     Only 1 segment on
//    0      0            1     |    2         2-2R or 1-1R
//    1      1            1     |    3         1-2R and 1-1R    Legacy default mode
//    0      0            2     |    4         2-2R and 1-1R
//                              |              or 0-2r and 2-1R
//    1      1            2     |    5         1-2R and 2-1R
//    0      0            3     |    6         2-2R and 2-1R
//                              |              or 0-2r and 3-1R
//    1      1            3     |    7         1-2R and 3-1R
//    1      0            1     |    2         0-2R and 1-1R    Legacy Override_2r mode
//    1      0            2     |    4         0-2R and 2-1R    Legacy Override_1r mode
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// shift register (sr) considerations:
//
//   2R segments are grouped as much as possible to allow some normal SR operation
//   between segment tests.
//   pre1/pre2/post main and pre1/pre2/post ffe are separated to avoid enabling
//   main and ffe at the same time
//   The math/algorithm depends on this being an int, vs a uint, so the actual
//   contents can never use the msb/sign bit; if we would ever need the msb, the
//   algorithm pieces that look for whether the lsb of a range is set would need
//   to change.
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test_setup
//
// Per lane setup; near complement is tx_seg_test_restore
////////////////////////////////////////////////////////////////////////////////////

void tx_seg_test_setup(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xC310, TX_SEG_TEST_DBG_LVL); // tx_seg_test_setup begin

    // also clears tx_rxdet_enable and tx_rxdet_pulse to 0
    put_ptr_field(gcr_addr_i, tx_pattern_enable,        0x1,
                  fast_write);        // pl, single bit, other bits in reg can or should be 0
    put_ptr_field(gcr_addr_i, tx_pattern_sel,           0x1, read_modify_write); // pg, other bits in reg

    put_ptr_field(gcr_addr_i, tx_rctrl,                 0x1, read_modify_write); // pg, single bit, must use rmw
    put_ptr_field(gcr_addr_i, tx_seg_test_leakage_ctrl, 0x1, read_modify_write); // pl, single bit, must use rmw

    // in p10_tx_testmode_controls.xlsx, DD1 TX50 Mode Controls tab, Single Segment Test col; but might not be absolutely required
    put_ptr_field(gcr_addr_i, tx_unload_clk_disable   , 0x0, read_modify_write); // pl, single bit, must use rmw

#ifdef IOO
    put_ptr_field(gcr_addr_i, tx_boost_en,               0x0, read_modify_write); //pl
    put_ptr_field(gcr_addr_i, tx_d2_ctrl_div_ctrl_alias, 0x0,
                  read_modify_write); //pl this is the default in regdef; will not restore
    put_ptr_field(gcr_addr_i, tx_tdr_clamp_disable,      0x1, read_modify_write); //pg
#endif

    //tx_pdwn is bit 1
    //put_ptr_field(gcr_addr_i, tx_bank_controls_d2_en_b_alias, 0x1, read_modify_write); //pl bit 2
    //bit 4 put_ptr_field(gcr_addr_i, tx_pcie_clk_sel,           0x10, read_modify_write); //pl bit 4
    put_ptr_field(gcr_addr_i, tx_bank_controls, 0x3A, read_modify_write); //pl
} // tx_seg_test_setup


////////////////////////////////////////////////////////////////////////////////////
// bool tx_seg_test_next_sr
//
// Updates shift register contents in place, returning false if complete
//
//   args are: <sr_io>, <fix_2r_i>, <num_2r>, <num_1r_equiv>
// *_mask variables are #defines from the tx_seg_test.h file
////////////////////////////////////////////////////////////////////////////////////

bool tx_seg_test_next_sr(int64_t* sr_io, uint32_t fix_2r_i, uint32_t num_2r_i, uint32_t num_1r_equiv_i)
{
    set_debug_state(0xC320, TX_SEG_TEST_DBG_LVL); // tx_seg_test_next_sr begin

    bool updated = true;

    // PSL sr_io
    if (*sr_io)   // after first loop, try to shift
    {
        set_tx_dcc_debug_tx_seg_test(0xC3B4, num_1r_equiv_i); // try_to_shift,  value arbitrary, for code tracing only
        uint64_t main_1r_mask;
        uint64_t ffe_2r_mask;
        uint64_t blockout;
        // PSL sr_io_odyssey
#ifdef IOO

        if (is_odyssey())
        {
            set_debug_state(0xC326, TX_SEG_TEST_DBG_LVL); // is_odyssey
            main_1r_mask = sr_7nm_main_1r_mask;
            ffe_2r_mask = sr_7nm_ffe_2r_mask;
            blockout = sr_7nm_blockout_mask;
        }
        else
        {
            set_debug_state(0xC327, TX_SEG_TEST_DBG_LVL); // not is_odyssey
            main_1r_mask = sr_5nm_main_1r_mask;
            ffe_2r_mask = sr_5nm_ffe_2r_mask;
            blockout = sr_5nm_blockout_mask;
        }

        // PSL sr_io_fix_2r
#else
        main_1r_mask = sr_iot_main_1r_mask;
        ffe_2r_mask = 0;
        blockout = sr_iot_blockout_mask;
#endif

        if (fix_2r_i)
        {
            // if fix_2r_i, num_2r_i must be 0 or 1 --> at most 1 2r bit set
            set_tx_dcc_debug_tx_seg_test(0xC3B5, num_1r_equiv_i); // fix_2r_true, value arbitrary, for code tracing only
#ifdef IOO
            updated = !last_mask_bit_set(*sr_io, sr_pre2_ffe_1r_mask); // finished when last 1r seg is 1
            // PSL sr_io_fix_2r_updated
#else
            updated = !last_mask_bit_set(*sr_io, sr_iot_main_1r_mask); // updated when when last main 1r is not 1
#endif

            if (updated)
            {
                set_tx_dcc_debug_tx_seg_test(0xC3B6, num_1r_equiv_i); // updated, value arbitrary, for code tracing only

                // PSL sr_io_fix_2r_num_2r_gt_0
                if (num_2r_i > 0)   // need to shift 2r enable; 1 2r always enabled; at least 1 1r always enabled
                {
                    // PSL sr_io_fix_2r_num_2r_gt_0_last_mask_set
                    if (last_mask_bit_set(*sr_io, main_1r_mask))  // move upper 2r segs to lower
                    {
                        set_tx_dcc_debug_tx_seg_test(0xC3B7,
                                                     num_1r_equiv_i); // shift_2r from_main_to_ffe, value arbitrary, for code tracing only
                        *sr_io = (*sr_io & ~(sr_any_main_2r_mask)) | sr_pre1_ffe_2r_mask;
                    }
                    // PSL sr_io_fix_2r_num_2r_gt_0_main_1r
                    else if (*sr_io & main_1r_mask)  // keep 2r segs in pre1 main, pre2 main, post main, or main
                    {
                        set_tx_dcc_debug_tx_seg_test(0xC3B8, num_1r_equiv_i); // shift_2r leave_in_main, value arbitrary, for code tracing only
                        shift_mask_up_to_end(sr_io, sr_any_main_2r_mask);
                    }
                    else   // keep 2r segs in pre1 ffe, pre2 ffe, or post ffe
                    {
                        set_tx_dcc_debug_tx_seg_test(0xC3B9, num_1r_equiv_i); // shift_2r to_end_of_main, value arbitrary, for code tracing only
                        shift_mask_up_to_end(sr_io, ffe_2r_mask);
                    }
                } // num_2r_i > 0

                shift_mask_blockout(sr_io, sr_any_1r_mask, blockout); // shift 1r enable
            } // updated
        } // fix_2r_i

        // PSL sr_io_1r
#ifdef IOO
        else if (num_1r_equiv_i >
                 0)   // when traversing 2r/1r boundary, need to ensure correct number of segs are enabled; will always have 0 or 2 2r segs active
        {
            updated = !last_mask_bit_set(*sr_io, ffe_2r_mask); // updated when last seg is not 1
            set_tx_dcc_debug_tx_seg_test(0xC3BA, num_1r_equiv_i); // try_to_shift_1r_equiv, value arbitrary, for code tracing only

            // PSL sr_io_1r_updated
            if (updated)
            {
                set_tx_dcc_debug_tx_seg_test(0xC3BB, num_1r_equiv_i); // shift_1r_equiv, value arbitrary, for code tracing only

                // PSL sr_io_1r_end_2r
                if (*sr_io & sr_main_2r_mask)   // end of upper 2r segs; enable 1 1r seg
                {
                    set_tx_dcc_debug_tx_seg_test(0xC3BC,
                                                 num_1r_equiv_i); // shift_1r_equiv convert_2_2r_to_1_1r, value arbitrary, for code tracing only
                    *sr_io >>= 1; // shift all; will add a 1r seg
                    *sr_io &= ~sr_main_2r_mask; // clear main_2r; which is only 2r set at this point
                }
                // PSL sr_io_1r_end_1r
                else if (last_mask_bit_set(*sr_io, sr_pre2_ffe_1r_mask)
                         && !(*sr_io & sr_pre2_ffe_2r_mask))   // end of 1r segs; enable 2 2r seg
                {
                    set_tx_dcc_debug_tx_seg_test(0xC3BD,
                                                 num_1r_equiv_i); // shift_1r_equiv convert_1_1r_to_2_2r, value arbitrary, for code tracing only
                    *sr_io >>= 1; // shift all; will add a 2r seg
                    *sr_io |= sr_pre2_ffe_2r_mask; // turn on last 2r bits
                }
                // PSL sr_io_1r_shift_main_2r
                else if (*sr_io & sr_any_main_2r_mask)   // left 2r bits set; need to shift these first to keep set bits together
                {
                    shift_mask_up_to_end(sr_io, sr_any_main_2r_mask);
                }
                // PSL sr_io_1r_shift_2r
                else if  (*sr_io & ffe_2r_mask)   // left 2r bits set; need to shift these first to keep set bits together
                {
                    shift_mask_up_to_end(sr_io, ffe_2r_mask);
                }
                else   // shift left 2r and shift 1r
                {
                    set_tx_dcc_debug_tx_seg_test(0xC3BE,
                                                 num_1r_equiv_i); // shift_1r_equiv shift_2r_and_1r, value arbitrary, for code tracing only
                    shift_mask_blockout(sr_io, sr_any_1r_mask, blockout); // shift 1r enable
                }
            } // updated
        } // num_1r_equiv_i > 0

#endif
        else   // special case, single segment shifted across
        {
            set_debug_state(0xC3BF, TX_SEG_TEST_DBG_LVL); // tx_seg_test_next_sr shift_single_segment main_branch
#ifdef IOO
            updated = !last_mask_bit_set(*sr_io, ffe_2r_mask); // updated when last seg is not 1
            // PSL special_case_updated
#else
            updated = !last_mask_bit_set(*sr_io, sr_iot_main_1r_mask); // updated when when last main 1r is not 1
#endif

            if (updated)
            {
                shift_mask_blockout(sr_io, (sr_any_1r_mask | sr_any_2r_mask), blockout); // shift 1r enable
            }
        } // single segment shifted
    } // shift reg update branch
    else   // first loop; init
    {
        uint32_t num_1r_to_set = num_1r_equiv_i;
        set_tx_dcc_debug_tx_seg_test(0xC3C1, num_1r_equiv_i); // init_sr, value arbitrary, for code tracing only

        // PSL not_sr_io_set_one_2r
        if ((fix_2r_i && num_2r_i) || (!fix_2r_i && !num_1r_equiv_i)) // set 1 2r segment
        {
            set_tx_dcc_debug_tx_seg_test(0xC3C2, num_1r_equiv_i); // init_sr set_1_2r_seg, value arbitrary, for code tracing only
            *sr_io |= sr_pre1_main_2r_mask;
        }

        // PSL not_sr_io_set_two_2r
#ifdef IOO
        else if (!fix_2r_i && num_1r_equiv_i)   // set 2 2r segments
        {
            set_tx_dcc_debug_tx_seg_test(0xC3C3, num_1r_equiv_i); // init_sr set_2_2r_seg, value arbitrary, for code tracing only
            *sr_io |= (sr_pre1_main_2r_mask | sr_pre2_main_2r_mask);
            num_1r_to_set--;
        }

        // PSL not_sr_io_odyssey
        if (!is_odyssey())  // 5nm starts 1 bit earlier
        {
            *sr_io <<= 1;
        }

        *sr_io |= (((~sr_pre1_main_1r_mask) >> num_1r_to_set)&sr_pre1_main_1r_mask); // set 1r segments
#else
        *sr_io >>= 2; // IOT starts 2 bits later than 7nm
        *sr_io |= (((~sr_iot_main_1r_mask) >> num_1r_to_set)&sr_iot_main_1r_mask); // set 1r segments
#endif

    } // shift reg initialize branch

#if TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
    uint16_t sr_io_0_15, sr_io_16_31, sr_io_32_47, sr_io_48_63;
    sr_io_0_15 = *sr_io >> 48;
    sr_io_16_31 = (*sr_io & 0xFFFF00000000) >> 32;
    sr_io_32_47 = (*sr_io & 0xFFFF0000) >> 16;
    sr_io_48_63 = (*sr_io & 0xFFFF);

    // the set_tx_dcc_debug_tx_seg_test macro has a check for DEBUG LEVEL, so this is only
    // 'instantiated' when IO_DEBUG_LEVEL >= 3
    set_tx_dcc_debug_tx_seg_test(0xC3C3, sr_io_0_15);
    set_tx_dcc_debug_tx_seg_test(0xC3C4, sr_io_16_31);
    set_tx_dcc_debug_tx_seg_test(0xC3C5, sr_io_32_47);
    set_tx_dcc_debug_tx_seg_test(0xC3C6, sr_io_48_63);

    set_tx_dcc_debug_tx_seg_test(0xC3C7, updated);
#endif

    return updated;
} // tx_seg_test_next_sr


////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test_sr_to_reg
//
// Helper function that updates the seg_regs structure from the shift register
//
// update_reg is a define that updates a field in memory with a portion of the
//   shift register
//   args to update_reg are: uint_ptr, fld_endbit, fld_width, sr, sr_endbit
//
// *_mask, *_width, and *_endbit are defines from the tx_seg_test.h file
////////////////////////////////////////////////////////////////////////////////////

void tx_seg_test_sr_to_reg(int64_t sr_i, t_tx_seg_regs* seg_regs_io)
{
    set_debug_state(0xC330, TX_SEG_TEST_DBG_LVL); // tx_seg_test_sr_to_reg begin

    // PSL pre1_main_enabled
#ifdef IOO

    if ((sr_pre1_main_2r_mask | sr_pre1_main_1r_mask) & sr_i) // pre1 main enabled
    {
        set_debug_state(0xC331, TX_SEG_TEST_DBG_LVL); // pre1_main seg update
        update_reg(seg_regs_io->pre1_en, tx_nseg_pre1_en_endbit - sr_pre1_main_1r_width,    sr_pre1_main_2r_width,  sr_i,
                   sr_pre1_main_2r_endbit );
        update_reg(seg_regs_io->pre1_en, tx_nseg_pre1_en_endbit,                            sr_pre1_main_1r_width,  sr_i,
                   sr_pre1_main_1r_endbit );
        seg_regs_io->pre1_sel &= ~tx_pre1_sel_mask; // turn off selects to select main
    }
    else   // pre1 ffe enabled or no pre1 en
    {
        set_debug_state(0xC332, TX_SEG_TEST_DBG_LVL); // pre1_ffe seg update
        update_reg(seg_regs_io->pre1_en, tx_nseg_pre1_en_endbit - sr_pre1_ffe_1r_width,     sr_pre1_ffe_2r_width,  sr_i,
                   sr_pre1_ffe_2r_endbit  );
        update_reg(seg_regs_io->pre1_en, tx_nseg_pre1_en_endbit,                            sr_pre1_ffe_1r_width,  sr_i,
                   sr_pre1_ffe_1r_endbit  );
        seg_regs_io->pre1_sel |= tx_pre1_sel_mask; // turn on selects to select ffe
    }

    // PSL pre2_enabled
    if ((sr_pre2_ffe_2r_mask | sr_pre2_ffe_1r_mask) & sr_i) // pre2 ffe enabled or no pre2 en
    {
        // follows different pattern in case we want to set exp_val_0 only when pre2_ffe enabled
        set_debug_state(0xC333, TX_SEG_TEST_DBG_LVL); // pre2_ffe seg update
        update_reg(seg_regs_io->pre2_en, tx_nseg_pre2_en_endbit - sr_pre2_ffe_1r_width,     sr_pre2_ffe_2r_width,  sr_i,
                   sr_pre2_ffe_2r_endbit  );
        update_reg(seg_regs_io->pre2_en, tx_nseg_pre2_en_endbit,                            sr_pre2_ffe_1r_width,  sr_i,
                   sr_pre2_ffe_1r_endbit  );
        seg_regs_io->pre2_sel |= tx_pre2_sel_mask; // turn on selects to select ffe
    }
    else   // pre2 main enabled
    {
        set_debug_state(0xC334, TX_SEG_TEST_DBG_LVL); // pre2_main seg update
        update_reg(seg_regs_io->pre2_en, tx_nseg_pre2_en_endbit - sr_pre2_main_1r_width,    sr_pre2_main_2r_width,  sr_i,
                   sr_pre2_main_2r_endbit );
        update_reg(seg_regs_io->pre2_en, tx_nseg_pre2_en_endbit,                            sr_pre2_main_1r_width,  sr_i,
                   sr_pre2_main_1r_endbit );
        seg_regs_io->pre2_sel &= ~tx_pre2_sel_mask; // turn off selects to select main
    }

#endif

    update_reg(seg_regs_io->main_0_15_en, tx_nseg_main_0_15_hs_en_endbit - sr_main_0_15_1r_width, sr_main_2r_width     ,
               sr_i, sr_main_2r_endbit      );
    update_reg(seg_regs_io->main_0_15_en, tx_nseg_main_0_15_hs_en_endbit,                         sr_main_0_15_1r_width,
               sr_i, sr_main_0_15_1r_endbit );

    // PSL is_odyssey
#ifdef IOO

    if(is_odyssey())
    {
        set_debug_state(0xC335, TX_SEG_TEST_DBG_LVL); // odyssey main_16_24
#endif
        update_reg(seg_regs_io->main_16_24_post_en, tx_nseg_main_16_24_hs_en_endbit, sr_main_16_24_width, sr_i,
                   sr_main_16_24_endbit );
#ifdef IOO
    }
    else
    {
        // PSL post_main_enabled
        if ((sr_post_main_2r_mask | sr_post_main_1r_mask) & sr_i)   // post main enabled
        {
            set_debug_state(0xC336, TX_SEG_TEST_DBG_LVL); // post_main seg update
            update_reg(seg_regs_io->main_16_24_post_en, tx_nseg_post_en_endbit - sr_post_main_1r_width,    sr_post_main_2r_width,
                       sr_i, sr_post_main_2r_endbit );
            update_reg(seg_regs_io->main_16_24_post_en, tx_nseg_post_en_endbit,                            sr_post_main_1r_width,
                       sr_i, sr_post_main_1r_endbit );
            seg_regs_io->post_sel &= ~tx_post_sel_mask; // turn off selects to select main
        }
        else
        {
            set_debug_state(0xC337, TX_SEG_TEST_DBG_LVL); // post_ffe seg update
            update_reg(seg_regs_io->main_16_24_post_en, tx_nseg_post_en_endbit - sr_post_main_1r_width,    sr_post_ffe_2r_width,
                       sr_i, sr_post_ffe_2r_endbit );
            update_reg(seg_regs_io->main_16_24_post_en, tx_nseg_post_en_endbit,                            sr_post_ffe_1r_width,
                       sr_i, sr_post_ffe_1r_endbit );
            seg_regs_io->post_sel |= tx_post_sel_mask; // turn on selects to select ffe
        }
    }

#endif

#if defined(IOO) && TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
    set_tx_dcc_debug_tx_seg_test(0xC3D8, seg_regs_io->pre1_sel          );
    set_tx_dcc_debug_tx_seg_test(0xC3D9, seg_regs_io->pre1_en           );
    set_tx_dcc_debug_tx_seg_test(0xC3DA, seg_regs_io->pre2_sel          );
    set_tx_dcc_debug_tx_seg_test(0xC3DB, seg_regs_io->pre2_en           );
    set_tx_dcc_debug_tx_seg_test(0xC3DC, seg_regs_io->post_sel          );
#endif
#if TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
    set_tx_dcc_debug_tx_seg_test(0xC3DD, seg_regs_io->main_16_24_post_en);
    set_tx_dcc_debug_tx_seg_test(0xC3DE, seg_regs_io->main_0_15_en      );
#endif
} //tx_seg_test_sr_to_reg


////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test_update_seg_gcr
//
// Helper function that updates the hardware with changed contents found by
// comparing a previous structure to a new one.
// If update_all_i is true, updates all registers to the new value regardless of
//   the previous value
// FFE segment constraint:
//   for corresponding segment/reg position, if p_en and n_en,
//      then p_sel must match n_sel
//   for 7nm, This constraint is satisfied by examining the selects. If the prev and new
//      selects differ or update_all_i is true, the p enables are cleared
//      before writing the n selects and enables and finally, the p selects and
//      enables. This takes advantage that the p and n selects are always the
//      same after the initial update_all.
//   for 5nm, this constraint is satisfied by definition because the same select
//      is used for p as n
// *_mask variables are defines from the tx_seg_test.h file
////////////////////////////////////////////////////////////////////////////////////

void tx_seg_test_update_seg_gcr( t_gcr_addr* gcr_addr_i, t_tx_seg_regs* prev_seg_regs_i,  t_tx_seg_regs* new_seg_regs_i,
                                 bool update_all_i)
{
    set_debug_state(0xC340, TX_SEG_TEST_DBG_LVL); // tx_seg_test_update_seg_gcr begin

    // PSL update_pre1
#ifdef IOO

    if ((prev_seg_regs_i->pre1_en ^ new_seg_regs_i->pre1_en) || (prev_seg_regs_i->pre1_sel ^ new_seg_regs_i->pre1_sel)
        || update_all_i)
    {
        set_debug_state(0xC341, TX_SEG_TEST_DBG_LVL); // update pre1
        put_ptr_fast(gcr_addr_i, tx_pseg_pre1_en_addr, 15, 0x0);
        put_ptr_fast(gcr_addr_i, tx_nseg_pre1_sel_addr, 15, new_seg_regs_i->pre1_sel);  // this is don't care for 5nm
        put_ptr_fast(gcr_addr_i, tx_nseg_pre1_en_addr, 15, new_seg_regs_i->pre1_en);
        put_ptr_fast(gcr_addr_i, tx_pre1_sel_addr, 15,     new_seg_regs_i->pre1_sel);   // this is pseg sel for 7nm
        put_ptr_fast(gcr_addr_i, tx_pseg_pre1_en_addr, 15, new_seg_regs_i->pre1_en);
    }

    // PSL update_pre2
    if ((prev_seg_regs_i->pre2_en ^ new_seg_regs_i->pre2_en) || (prev_seg_regs_i->pre2_sel ^ new_seg_regs_i->pre2_sel)
        || update_all_i)
    {
        set_debug_state(0xC343, TX_SEG_TEST_DBG_LVL); // update pre2
        put_ptr_fast(gcr_addr_i, tx_pseg_pre2_en_addr, 15, 0x0);
        put_ptr_fast(gcr_addr_i, tx_nseg_pre2_sel_addr, 15, new_seg_regs_i->pre2_sel);  // this is don't care for 5nm
        put_ptr_fast(gcr_addr_i, tx_nseg_pre2_en_addr, 15, new_seg_regs_i->pre2_en);
        put_ptr_fast(gcr_addr_i, tx_pre2_sel_addr, 15,     new_seg_regs_i->pre2_sel);   // this is pseg sel for 7nm
        put_ptr_fast(gcr_addr_i, tx_pseg_pre2_en_addr, 15, new_seg_regs_i->pre2_en);
    }

    // this is don't care for 7nm
    // PSL update_post_sel
    if ((prev_seg_regs_i->post_sel ^ new_seg_regs_i->post_sel) || update_all_i)
    {
        set_debug_state(0xC345, TX_SEG_TEST_DBG_LVL); // update post_sel
        put_ptr_fast(gcr_addr_i, tx_post_sel_addr, 15, new_seg_regs_i->post_sel);
    }

#endif

    // PSL update_post_en
    if ((prev_seg_regs_i->main_16_24_post_en ^ new_seg_regs_i->main_16_24_post_en) || update_all_i)
    {
        set_debug_state(0xC347, TX_SEG_TEST_DBG_LVL); // update post_en
        put_ptr_fast(gcr_addr_i, tx_nseg_main_16_24_hs_en_addr, 15, new_seg_regs_i->main_16_24_post_en);
        put_ptr_fast(gcr_addr_i, tx_pseg_main_16_24_hs_en_addr, 15, new_seg_regs_i->main_16_24_post_en);
    }

    // PSL update_main_0_15
    if ((prev_seg_regs_i->main_0_15_en ^ new_seg_regs_i->main_0_15_en) || update_all_i)
    {
        set_debug_state(0xC349, TX_SEG_TEST_DBG_LVL); // update main_0_15
        put_ptr_fast(gcr_addr_i, tx_nseg_main_0_15_hs_en_addr, 15, new_seg_regs_i->main_0_15_en);
        put_ptr_fast(gcr_addr_i, tx_pseg_main_0_15_hs_en_addr, 15, new_seg_regs_i->main_0_15_en);
    }

} // tx_seg_test_update_seg_gcr


////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test_restore
//
// Per lane restore; near complement of tx_seg_test_setup
////////////////////////////////////////////////////////////////////////////////////

void tx_seg_test_restore(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xC360, TX_SEG_TEST_DBG_LVL); // tx_seg_test_restore begin

    put_ptr_field(gcr_addr_i, tx_pattern_sel,           0x0, read_modify_write); // pg, other bits in reg

    // also clears tx_rxdet_enable and tx_rxdet_pulse to 0
    put_ptr_field(gcr_addr_i, tx_pattern_enable,        0x0,
                  fast_write);        // pl, single bit, other bits in reg can or should be 0

    put_ptr_field(gcr_addr_i, tx_rctrl,                 0x0, read_modify_write); // pg, single bit, must use rmw
    put_ptr_field(gcr_addr_i, tx_seg_test_leakage_ctrl, 0x0, read_modify_write); // pl, single bit, must use rmw

#ifdef IOO
    put_ptr_field(gcr_addr_i, tx_boost_en,               0x1, read_modify_write); //pl
    put_ptr_field(gcr_addr_i, tx_tdr_clamp_disable,      0x0, read_modify_write); //pg
#endif

    //tx_pdwn is bit 1
    //put_ptr_field(gcr_addr_i, tx_bank_controls_d2_en_b_alias, 0x0, read_modify_write); //pl bit 2
    // bit 4 put_ptr_field(gcr_addr_i, tx_pcie_clk_sel,           0x10, read_modify_write); //pl bit 4
    put_ptr_field(gcr_addr_i, tx_bank_controls, 0x20, read_modify_write); //pl
} //tx_seg_test_restore


////////////////////////////////////////////////////////////////////////////////////
// shift_mask_blockout
//
// Shifts region of shift register defined by mask right until no bits are in the blockout region
//
//   args are: <sr_io>, <mask>, <blockout>
//
// all arguments are expected to be in the range 0 to 2^63 - 1 so that the sign bit
//   of an int64 is not set
////////////////////////////////////////////////////////////////////////////////////

void shift_mask_blockout(int64_t* sr_io, int64_t mask_i, int64_t blockout_i)
{
    set_debug_state(0xC370, TX_SEG_TEST_DBG_LVL); // shift_mask_blockout begin

    // PSL blockout
    do
    {
        set_debug_state(0xC371, TX_SEG_TEST_DBG_LVL); // shift_mask_blockout before_shift
        shift_mask(*sr_io, mask_i);
        set_debug_state(0xC372, TX_SEG_TEST_DBG_LVL); // shift_mask_blockout after_shift
    }
    while (*sr_io & blockout_i);
} // shift_mask_blockout


////////////////////////////////////////////////////////////////////////////////////
// shift_mask_up_to_end
//
// Shifts region of shift register defined by mask 1 right if lsb is not set
//
//   args are: <sr_io>, <mask>
//
// sr and mask are expected to be in the range 0 to 2^63 - 1 so that the sign bit
//   of an int64 is not set
////////////////////////////////////////////////////////////////////////////////////

void shift_mask_up_to_end(int64_t* sr_io,
                          int64_t mask_i)   // shift only the masked portion 1 right only if there is room in the masked region
{
    // PSL last_bit_not_set
    if ((mask_i & (-mask_i)& *sr_io) == 0) // last bit of masked region not set
    {
        shift_mask(*sr_io, mask_i);
    }
} // shift_mask_up_to_end


////////////////////////////////////////////////////////////////////////////////////
// tx_seg_test_value
////////////////////////////////////////////////////////////////////////////////////

int tx_seg_test_value(t_gcr_addr* gcr_addr_i, uint32_t val_i)
{
    // for 32:1 interface, bits 0 and 32 must be driven to a 0; could reduce this to two writes in that case
    // for 16:1 interface, bits 0, 16, 32, and 48 must be driven to a 0
    int status = rc_no_error;
    put_ptr_field(gcr_addr_i, tx_pattern_0_15,   val_i,  fast_write);
    put_ptr_field(gcr_addr_i, tx_pattern_16_31,  val_i,  fast_write);
    put_ptr_field(gcr_addr_i, tx_pattern_32_47,  val_i,  fast_write);
    put_ptr_field(gcr_addr_i, tx_pattern_48_63,  val_i,  fast_write);

    set_debug_state(0xC354, TX_SEG_TEST_DBG_LVL); // tx_seg_test_value
#if TX_SEG_TEST_DEEP_DBG_LVL < IO_DEBUG_LEVEL
    set_tx_dcc_debug_tx_seg_test(0xC355, val_i);
    uint32_t debug_tx_seg_test_status_l = get_ptr_field(gcr_addr_i, tx_seg_test_status);
    set_tx_dcc_debug_tx_seg_test(0xC357, debug_tx_seg_test_status_l);
#endif

    // PSL tx_seg_status
    if (get_ptr_field(gcr_addr_i, tx_seg_test_status) != (val_i ? 2 : 1))
    {
        uint32_t new_lane_bad;
        uint32_t lane = get_gcr_addr_lane(gcr_addr_i);

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

        mem_pg_bit_set(tx_seg_test_fail); // single bit, per group reg
        set_debug_state(0xC358, TX_SEG_TEST_DBG_LVL); // tx_seg_test_value fail
#if TX_SEG_TEST_LOG_FAIL
        ADD_LOG(DEBUG_BIST_TXSEG, gcr_addr_i, new_lane_bad);
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning); // set DFT FIR bit
        status = error_code;
#endif
    }

    return status;
} //tx_seg_test_value
