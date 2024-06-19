/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/tx_seg_test.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
// *! FILENAME    : tx_seg_test.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Brian Albertson     Email: brian.j.albertson@ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// gap22082400 |gap     | Add iot support
// gap22080200 |gap     | Added P11 support
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Support 5nm and 7nm segments
// bja21060800 |bja     | Renumber shift register for 5nm segment counts
// gap20021800 |gap     | Changed from IO_DISABLE_DEBUG to IO_DEBUG_LEVEL
// gap19121000 |gap     | Created
// -----------------------------------------------------------------------------

#ifndef _TX_SEG_TEST_H_
#define _TX_SEG_TEST_H_

// Use this to set debug_state levels for testing to enable deep debug
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk,
// deep debug info will be generated; the current value of IO_DEBUG_LEVEL is 2
// You will likely need to decrease $free_space_limit in ppe/obj/analyze_size.pl
// TX_SEG_TEST_DBG_LVL is a related define in *.c
#define TX_SEG_TEST_DEEP_DBG_LVL 3

// Set this to 1 to use a blockout mask to short circuit most of the shift register. This is
// used in designer sim to shorten runtime and still capture the critical shifting of 2r to
// 1r main regions and 1r to 2r ffe regions to validate the various modes.
// Set to 0 for production
#define TX_SEG_TEST_FAST 0

// Use this to log and stop on fail; 1 is normal operation; 0 is used to
// see shift register complete even if individual seg test fails
#define TX_SEG_TEST_LOG_FAIL 1

////           7nm                  5nm IOO               IOT
//// bit 00    unused               unused                unused
//// bit 01    unused               post main 2r 0        unused
//// bit 02    pre1 main 2r 0       pre1 main 2r 0        unused
//// bit 03    pre2 main 2r 0       pre2 main 2r 0        unused
//// bit 04    main 2r 0            main 2r 0             main 2r 0
//// bit 05-08 pre1 main 1r 1-4     pre1 main 1r 1-4      unused
//// bit 09-12 unused               pre1 main 1r 5-8      unused
//// bit 13-16 pre2 main 1r 1-4     pre2 main 1r 1-4      unused
//// bit 17    main 1r 16           unused                main 1r 16
//// bit 18-25 main 1r 17-24        post main 1r 1-8      main 1r 17-24
//// bit 26-37 main 1r 1-12         main 1r 1-12          main 1r 1-12
//// bit 38-40 main 1r 13-15        unused                main 1r 13-15
//// bit 41-48 unused               post ffe 1r 1-8       unused
//// bit 49-52 pre1 ffe 1r 1-4      pre1 ffe 1r 1-4       unused
//// bit 53-56 unused               pre1 ffe 1r 5-8       unused
//// bit 57-60 pre2 ffe 1r 1-4      pre2 ffe 1r 1-4       unused
//// bit 61    pre1 ffe 2r 0        pre1 ffe 2r 0         unused
//// bit 62    pre2 ffe 2r 0        pre2 ffe 2r 0         unused
//// bit 63    unused               post ffe 2r 0         unused

#define sr_post_main_2r_endbit  1
#define sr_post_main_2r_width   1
#define sr_pre1_main_2r_endbit  2
#define sr_pre1_main_2r_width   1
#define sr_pre2_main_2r_endbit  3
#define sr_pre2_main_2r_width   1
#define sr_main_2r_endbit       4
#define sr_main_2r_width        1
#define sr_pre1_main_1r_endbit  12
#define sr_pre1_main_1r_width   8
#define sr_pre2_main_1r_endbit  16
#define sr_pre2_main_1r_width   4
#define sr_main_16_24_endbit    25
#define sr_main_16_24_width     9
#define sr_post_main_1r_endbit  25
#define sr_post_main_1r_width   8
#define sr_main_0_15_1r_endbit  40
#define sr_main_0_15_1r_width   15
#define sr_post_ffe_1r_endbit   48
#define sr_post_ffe_1r_width    8
#define sr_pre1_ffe_1r_endbit   56
#define sr_pre1_ffe_1r_width    8
#define sr_pre2_ffe_1r_endbit   60
#define sr_pre2_ffe_1r_width    4
#define sr_pre1_ffe_2r_endbit   61
#define sr_pre1_ffe_2r_width    1
#define sr_pre2_ffe_2r_endbit   62
#define sr_pre2_ffe_2r_width    1
#define sr_post_ffe_2r_endbit   63
#define sr_post_ffe_2r_width    1

//                                   1122233444556
//                                0482604826048260
#define sr_post_main_2r_mask    0x4000000000000000
#define sr_pre1_main_2r_mask    0x2000000000000000
#define sr_pre2_main_2r_mask    0x1000000000000000
#define sr_main_2r_mask         0x0800000000000000
#define sr_pre1_main_1r_mask    0x07F8000000000000
#define sr_pre2_main_1r_mask    0x0007800000000000
#define sr_post_main_1r_mask    0x00003FC000000000
#define sr_pre2_ffe_1r_mask     0x0000000000000078
#define sr_pre1_ffe_2r_mask     0x0000000000000004
#define sr_pre2_ffe_2r_mask     0x0000000000000002

#if TX_SEG_TEST_FAST
    #define sr_5nm_blockout_mask    0x807FFFFFFFFFFF80
#else
    #define sr_5nm_blockout_mask    0x8000400003800000
#endif

#define sr_5nm_ffe_2r_mask      0x0000000000000007
#define sr_5nm_main_1r_mask     0x07FFFFFFFC000000

#if TX_SEG_TEST_FAST
    #define sr_iot_blockout_mask    0xF7FF808FF87FFFFF
#else
    #define sr_iot_blockout_mask    0xF7FF8000007FFFFF
#endif

#if TX_SEG_TEST_FAST
    #define sr_7nm_blockout_mask    0xC07FFFFFFFFFFF81
#else
    #define sr_7nm_blockout_mask    0xC0780000007F8781
#endif

#define sr_7nm_ffe_2r_mask      0x0000000000000006
#define sr_7nm_main_1r_mask     0x07FFFFFFFF800000
#define sr_iot_main_1r_mask     0x00007FFFFF800000
#define sr_any_1r_mask          0x07FFFFFFFFFFFFF8
#define sr_any_2r_mask          0x7800000000000007
#define sr_any_ffe_2r_mask      0x0000000000000007
#define sr_any_main_2r_mask     0x7800000000000000

// updates segment register data structure for particular register to match shift register
// update_reg(<seg_reg_struct_ptr>, <end_of_fld>, <width_of_field>, <shift_reg_value>, <shift_reg_fld_endbit>
//                 [ <orig_val> & (~mask bits to update                          ) ]| [(new valuue shifted) & (mask bits to update                            )]
#define update_reg(uint_ptr, fld_endbit, fld_width, sr, sr_endbit) { \
        uint_ptr = ((uint_ptr & (~(((1 << fld_width) - 1) << (15 - (fld_endbit))))) | (((sr>>(63-sr_endbit)) & ((1 << fld_width) - 1)) << (15 - (fld_endbit))));\
    }

// shift only the masked portion 1 right; shifting off the end
#define shift_mask(sr, mask) { \
        sr =(sr & (~mask))|(((mask & sr)>>1)&mask);\
    }

#define last_mask_bit_set(sr, mask) (sr & mask & (-mask))

// these are each the full 16 bit gcr register containing the field
typedef struct TX_SEG_REGS
{
    uint16_t pre1_sel           ;
    uint16_t pre1_en            ;
    uint16_t pre2_sel           ;
    uint16_t pre2_en            ;
    uint16_t post_sel           ;
    uint16_t main_16_24_post_en ;
    uint16_t main_0_15_en       ;
} t_tx_seg_regs;

int tx_seg_test(t_gcr_addr* gcr_addr_i);

void     tx_seg_test_setup(t_gcr_addr* gcr_addr_i);
bool     tx_seg_test_next_sr(int64_t* sr_io, uint32_t fix_2r_i, uint32_t num_2r_i, uint32_t num_1r_equiv_i);
void     tx_seg_test_sr_to_reg(int64_t sr_i, t_tx_seg_regs* seg_regs_io);
void     tx_seg_test_update_seg_gcr( t_gcr_addr* gcr_addr_i, t_tx_seg_regs* prev_seg_regs_i,
                                     t_tx_seg_regs* new_seg_regs_i, bool update_all_i);
void     tx_seg_test_restore(t_gcr_addr* gcr_addr_i);
void     shift_mask_up_to_end(int64_t* sr_io, int64_t mask_i);
void     shift_mask_blockout(int64_t* sr_io, int64_t mask_i, int64_t blockout_i);
int      tx_seg_test_value(t_gcr_addr* gcr_addr_i, uint32_t val_i);

////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
// Some functions and macros to help in debugging.
// These are light weight but the code size and performance hit can add up,
// so allow for a compiler option to disable ().
////////////////////////////////////////////////////////////////////////////////////////////
// share with dcc since these are not run at the same time
// If this threshold changes, also need to update c code #if
#if TX_SEG_TEST_DEEP_DBG_LVL > IO_DEBUG_LEVEL
    #define set_tx_dcc_debug_tx_seg_test(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug_tx_seg_test(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif //IO_DEBUG_LEVEL

#endif //_TX_SEG_TEST_H_
