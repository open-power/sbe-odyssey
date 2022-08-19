/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_ffe.h $        */
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
// *! FILENAME    : tx_ffe.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// gap22020509 |gap     | Add 5nm post, follow 5nm constraints
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Support 5nm and 7nm segments
// bja21101800 |bja     | Use newly available PRE1 segments
// vbr19120900 |vbr     | Initial implementation of debug levels
// gap19100400 |gap     | Created
// -----------------------------------------------------------------------------

#ifndef _TX_FFE_H_
#define _TX_FFE_H_

// Design parameters
#define tx_ffe_zcal_valid_min_2r_c     33 /* results < this will be replaced with default             */
#define tx_ffe_zcal_default_2r_c       50 /* value used when zcal result is invalid                   */
#define tx_ffe_zcal_bound_min_2r_c     40 /* zcal result will be clipped to this minimum              */
// 7nm
// Main:  1 2r + 24 1r = 49 2r
// Pre1:  1 2r +  4 1r =  9 2r
// Pre2:  1 2r +  4 1r =  9 2r
// ---------------------------
// Total: 3 2r + 32 1r = 67 2r
//
// 5nm
// Main:  1 2r + 12 1r = 25 2r
// Pre1:  1 2r +  8 1r = 17 2r
// Pre2:  1 2r +  4 1r =  9 2r
// Post:  1 2r +  8 1r = 17 2r
// ---------------------------
// Total: 4 2r + 32 1r = 68 2r
//
// Because of the way the algorithm works, a 2r segment used for one phase ffe cannot be used
//   for main for the other phase. We can have situations where one phase that requires more
//   segments uses 8 ffe segments and another that uses fewer uses 7. This means that the
//   2r segment is not available for the phase that wants more segments. This limits the
//   enabled segments to the total 2r equivalent minus the 2r segments from the ffe banks.
//   for 7nm, this is 67 total - 2 from ffe banks = 65 2R equivalents. For 5nm, this is
//   68 total - 3 from ffe banks = 65 2R equivalents. So the mas upper bound is 65 in both
//   scenarios.
#define tx_ffe_zcal_bound_max_2r_c     65 /* zcal result will be clipped to this maximum              */

// types of sst segments
typedef enum
{
    TX_FFE_BANKTYPE_PRE1,
    TX_FFE_BANKTYPE_PRE2,
    TX_FFE_BANKTYPE_POST,
} t_tx_ffe_ffe_banktype;

typedef struct TX_FFE_FFE_CNTL_SEG_STRUCT
{
    uint32_t main ;
    uint32_t ffe  ;
} t_tx_ffe_ffe_seg;

typedef struct TX_FFE_CNTL_SEG_STRUCT
{
    t_tx_ffe_ffe_seg pre1;
    t_tx_ffe_ffe_seg pre2;
    t_tx_ffe_ffe_seg post;
    uint32_t main;
} t_tx_ffe_seg;

typedef struct TX_FFE_CNTL_STRUCT
{
    t_tx_ffe_seg pseg ;
    t_tx_ffe_seg nseg ;
} t_tx_ffe_cntl;

void     tx_ffe(t_gcr_addr* gcr_addr_i);
uint32_t tx_ffe_get_zcal(t_gcr_addr* gcr_addr_i,  bool is_nseg_i, bool is_5nm);
void     tx_ffe_calc_ffe(uint32_t pre2_coef_x128_i, uint32_t pre1_coef_x128_i, uint32_t post_coef_x128_i,
                         uint32_t zcal_result_nseg_i, uint32_t zcal_result_pseg_i, bool is_5nm_i, t_tx_ffe_cntl* seg_values_o);
void     tx_ffe_write_ffe(t_gcr_addr* gcr_addr_i, bool is_5nm, t_tx_ffe_cntl* seg_values_i);
void     tx_ffe_bound_zcal(uint32_t* zcal_2r_l);
uint32_t tx_ffe_calc_sel(uint32_t zcal_i, uint32_t ffe_coef_x128_i) ;
void     tx_ffe_alloc_main(uint32_t* nseg_remain, uint32_t* pseg_remain, uint32_t nseg_ffe_i, uint32_t pseg_ffe_i,
                           uint32_t* nseg_main,
                           uint32_t* pseg_main, uint32_t bank_max_2r_i);
void     tx_ffe_write_main_en(t_gcr_addr* gcr_addr_i, uint32_t num_2r_equiv_i, bool is_nseg_i, bool is_5nm_i);
void     tx_ffe_write_ffe_en_sel(t_gcr_addr* gcr_addr_i, uint32_t num_2r_equiv_nseg_main_en_i,
                                 uint32_t num_2r_equiv_nseg_ffe_en_i,
                                 uint32_t num_2r_equiv_pseg_main_en_i, uint32_t num_2r_equiv_pseg_ffe_en_i, t_tx_ffe_ffe_banktype banktype_i,
                                 bool is_5nm_i);


////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FUNCTIONS
// Some functions and macros to help in debugging.
// These are light weight but the code size and performance hit can add up,
// so allow for a compiler option to enable/disable based on debug level.
////////////////////////////////////////////////////////////////////////////////////////////
// share with dcc since these are not run at the same time
#if IO_DEBUG_LEVEL < 3
    #define set_tx_dcc_debug_tx_ffe(marker, value) {}
#else
    // This writes a "marker" followed by a value "value" to the mem_regs which can be used for tracking execution value.
    #define set_tx_dcc_debug_tx_ffe(marker, value) { mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (marker);  mem_regs_u16[pg_addr(tx_dcc_debug_addr)] = (value); }
#endif //IO_DEBUG_LEVEL
#endif // _TX_FFE_H_
