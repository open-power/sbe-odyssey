/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_seg_test.h $   */
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
// *! *** IBM Confidential ***
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
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Support 5nm and 7nm segments
// bja21060800 |bja     | Renumber shift register for 5nm segment counts
// gap20021800 |gap     | Changed from IO_DISABLE_DEBUG to IO_DEBUG_LEVEL
// gap19121000 |gap     | Created
// -----------------------------------------------------------------------------

#ifndef _TX_SEG_TEST_H_
#define _TX_SEG_TEST_H_

//// bit 00    sign bit, never use
///  bit 01-04 unused
//// bit 05    post main 2r 0 -- sinkless in 7nm
//// bit 07    pre1 main 2r 0
//// bit 07    pre2 main 2r 0
//// bit 08    main 2r 0
//// bit 09-16 post main 1r 1-8
//// bit 17-24 pre1 main 1r 1-8
//// bit 25-28 pre2 main 1r 1-4
//// bit 29-40 main 1r 1-12
//// bit 41-48 post ffe 1r 1-8
//// bit 49-56 pre1 ffe 1r 1-8
//// bit 57-60 pre2 ffe 1r 1-4
//// bit 61    post ffe 2r 0 -- sinkless in 7nm
//// bit 62    pre1 ffe 2r 0
//// bit 63    pre2 ffe 2r 0
#define sr_post_main_2r_endbit  5
#define sr_post_main_2r_mask    0x0400000000000000
#define sr_post_main_2r_width   1
#define sr_pre1_main_2r_endbit  6
#define sr_pre1_main_2r_mask    0x0200000000000000
#define sr_pre1_main_2r_width   1
#define sr_pre2_main_2r_endbit  7
#define sr_pre2_main_2r_mask    0x0100000000000000
#define sr_pre2_main_2r_width   1
#define sr_main_2r_endbit       8
#define sr_main_2r_mask         0x0080000000000000
#define sr_main_2r_width        1
#define sr_1r_mask              0x007FFFFFFFFFFFF8
#define sr_post_main_1r_endbit  16
#define sr_post_main_1r_mask    0x007F800000000000
#define sr_post_main_1r_width   8
#define sr_pre1_main_1r_endbit  24
#define sr_pre1_main_1r_mask    0x00007F8000000000
#define sr_pre1_main_1r_width   8
#define sr_pre2_main_1r_endbit  28
#define sr_pre2_main_1r_mask    0x0000007800000000
#define sr_pre2_main_1r_width   4
#define sr_main_1r_endbit       40
#define sr_main_1r_mask         0x00000007FF800000
#define sr_main_1r_width        12
#define sr_post_ffe_1r_endbit   48
#define sr_post_ffe_1r_mask     0x00000000007F8000
#define sr_post_ffe_1r_width    8
#define sr_pre1_ffe_1r_endbit   56
#define sr_pre1_ffe_1r_mask     0x0000000000007F80
#define sr_pre1_ffe_1r_width    8
#define sr_pre2_ffe_1r_endbit   60
#define sr_pre2_ffe_1r_mask     0x0000000000000078
#define sr_pre2_ffe_1r_width    4
#define sr_post_ffe_2r_endbit   61
#define sr_post_ffe_2r_mask     0x0000000000000004
#define sr_post_ffe_2r_width    1
#define sr_pre1_ffe_2r_endbit   62
#define sr_pre1_ffe_2r_mask     0x0000000000000002
#define sr_pre1_ffe_2r_width    1
#define sr_pre2_ffe_2r_endbit   63
#define sr_pre2_ffe_2r_mask     0x0000000000000001
#define sr_pre2_ffe_2r_width    1

// updates segment register data structure for particular register to match shift register
// update_reg(<seg_reg_struct_ptr>, <width_of_field>, <shift_reg_value>, <shift_reg_fld_endbit>
#define update_reg(uint_ptr, fld_endbit, fld_width, sr, sr_endbit) { \
        uint_ptr = ((uint_ptr & (~(((1 << fld_width) - 1) << (15 - (fld_endbit))))) | (((sr>>(63-sr_endbit)) & ((1 << fld_width) - 1)) << (15 - (fld_endbit))));\
    }

typedef struct TX_SEG_REGS
{
    uint16_t pre1_reg   ; // full register, en & sel
    uint8_t  pre2_reg   ; // full register, en & sel
    uint16_t post_reg   ; // full register, en & sel
    uint16_t main  ;
} t_tx_seg_regs;

int tx_seg_test(t_gcr_addr* gcr_addr_i);
int tx_seg_test_test_seg(t_gcr_addr* gcr_addr_i);

void tx_seg_test_setup(t_gcr_addr* gcr_addr_i);
bool tx_seg_test_next_sr(int64_t* sr_io, uint32_t fix_2r_i, uint32_t num_2r_i, uint32_t num_1r_equiv_i);
void tx_seg_test_sr_to_reg(int64_t sr_i, t_tx_seg_regs* seg_regs_io);
void tx_seg_test_update_seg_gcr( t_gcr_addr* gcr_addr_i, t_tx_seg_regs* prev_seg_regs_i,  t_tx_seg_regs* new_seg_regs_i,
                                 bool update_all_i);
void tx_seg_test_restore(t_gcr_addr* gcr_addr_i);
void shift_mask(int64_t* sr_io, int64_t mask_i);
void shift_mask_up_to_end(int64_t* sr_io, int64_t mask_i);
void tx_seg_test_0_pattern(t_gcr_addr* gcr_addr_i);
void tx_seg_test_1_pattern(t_gcr_addr* gcr_addr_i);
void tx_seg_test_set_segtest_fail(t_gcr_addr* gcr_addr_i);


////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
// Some functions and macros to help in debugging.
// These are light weight but the code size and performance hit can add up,
// so allow for a compiler option to disable (IO_DEBUG_LEVEL).
////////////////////////////////////////////////////////////////////////////////////////////
// share with dcc since these are not run at the same time
// If this threshold changes, also need to update c code #if
#if IO_DEBUG_LEVEL < 3
    #define set_tx_dcc_debug_tx_seg_test(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug_tx_seg_test(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif //IO_DEBUG_LEVEL

#endif //_TX_SEG_TEST_H_
