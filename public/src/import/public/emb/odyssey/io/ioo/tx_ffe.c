/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_ffe.c $        */
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
// *! FILENAME    : tx_ffe.c
// *! TITLE       :
// *! DESCRIPTION : Set tx ffe registers
// *!
// *! OWNER NAME  : Gary Peterson       Email: garyp@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *! DETAILED DESCRIPTION :
// *!   This reads the zcal results from the ffe enable registers and uses
// *!   the pre2, pre1, and post control values
// *!   read from mem_regs to calculate the needed select and enable values,
// *!   and finally writes the results.
// *!   Prior to running this code, the zcal results are stored in the high
// *!   speed enable registers as if there were no ffe applied.
// *!   The starting select settings are ignored.
// *!   The coeficients are input as desired coefficients multiplied by 128 and
// *!   rounded to the nearest integer.
// *!   Calculated values are as follows:
// *!      if pseg zcal is less than a valid value, it is set to a default;
// *!        likewise for nseg zcal
// *!      if pseg zcal is out of bounds, it is set to the nearest boundary value
// *!        likewise for nseg zcal
// *!      pseg enabled[2r] are set to pseg_zcal[2r]
// *!        likewise for nseg enabled; this represents the total number of pseg
// *!        or nseg 2r-equivalent segments which will be enabled
// *!      pseg pre1, pre2, and post selects are calculated as
// *!        round_to_nearest((coefficient * pseg enabled[2r])/128)
// *!      likewise for nseg pre1, pre2, and post (5nm) selects
// *!      Note: the current bounds and register sizes are such that the
// *!        registers cannot overflow using this method
// *!      The pre1, pre2, and post (5nm) enables and selects, and main enables are
// *!        calculated as follows:
// *!          the 7nm design has a constraint such that if a pseg and corresponding
// *!            nseg are both enabled, they must have the same select setting
// *!          the 5nm design has a constraint such that the select must be the same
// *!            between a pseg and the corresponding nseg.
// *!          to accomplish this, the same nseg and pseg segments are reserved
// *!            for FFE. Specific segments used for ffe are enabled and selected.
// *!            for 7nm segments reserved, but not used are not enabled or selected
// *!            for 5nm segments reserved, but not used are not enabled
// *!          pre1, pre2, and post (5nm) bank segments not reserved for ffe are used for main
// *!          after the pre1, pre2 and post (5nm) bank main segments are determined,
// *!            remaining segments are supplied by the main bank
// *!          Note: the bank register width and the allowed zcal and coefficient
// *!            are such that the resulting computation will not overflow the
// *!            ffe bank register controls and the remaining segments for the main bank
// *!            are bounded to the available main segments.
// *!   After the needed number of enables and selects are calculated, they
// *!     are converted to appropriate thermometer code and written to the
// *!     high speed ffe setting registers
// *!   Note on data structure: the data structure allows, e.g. pre1 pseg and
// *!     nseg to have different numbers of main segments enabled. With the
// *!     current implementation, this will not happen. But, if we later
// *!     had a situation where not all extra pre1 segments were used, this
// *!     could happen.
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// gap23040400 |gap     | EWM302057 bound main segments to prevent therm value overflow
// gap22102600 |gap     | EWM293106 update io_sleep to solve tx_ffe thread active time exceeded
// gap22100300 |gap     | Fix bug in main alloc that limited bound ranges, add logging when using bound
// gap22020500 |gap     | Add 5nm post, follow 5nm constraints
// mbs22021000 |mbs     | Updates to reduce code size
// bja22012500 |bja     | Support 5nm and 7nm segments
// bja21101800 |bja     | Use newly available PRE1 segs
// vbr21101100 |vbr     | Added additional sleep
// bja21061802 |bja     | No need to clear psegs before setting nsegs because select is shared in 5nm
// bja21061801 |bja     | Carefully readminister use of fast_write after register reorganization
// bja21061800 |bja     | Removed high_bits_l and low_bits_l from debug lines
// bja21061300 |bja     | Limit pre1 to 5 of the 9 bits
// bja21060802 |bja     | Do away with _hs designation
// bja21060801 |bja     | Combine p/n selects for pre1 and pre2
// bja21060800 |bja     | Remove tx_[pn]seg_main_16_24_hs_en
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// gap19112100 |gap     | Created
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "tx_ffe.h"
#include "io_logger.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"

// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
// If this is less than or equal to IO_DEBUG_LEVEL in ppe_common/img_defs.mk, debug states will be written, current
// value is 2
#define TX_FFE_DBG_LVL 3

void tx_ffe(t_gcr_addr* gcr_addr_i)
{
    set_debug_state(0xC200); // tx_ffe begin

    io_sleep(get_gcr_addr_thread(gcr_addr_i));
    uint32_t pre2_coef_x128_l  = mem_pg_field_get(tx_ffe_pre2_coef);
    uint32_t pre1_coef_x128_l  = mem_pg_field_get(tx_ffe_pre1_coef);
    uint32_t post_coef_x128_l  = mem_pg_field_get(tx_ffe_post_coef);

    bool is_5nm_l = !is_odyssey();

    uint32_t zcal_result_nseg_l = tx_ffe_get_zcal(gcr_addr_i, true, is_5nm_l);
    uint32_t zcal_result_pseg_l = tx_ffe_get_zcal(gcr_addr_i, false, is_5nm_l);

    t_tx_ffe_cntl seg_values_l;
    io_sleep(get_gcr_addr_thread(gcr_addr_i));

    tx_ffe_calc_ffe(gcr_addr_i, pre2_coef_x128_l, pre1_coef_x128_l, post_coef_x128_l, zcal_result_nseg_l,
                    zcal_result_pseg_l, is_5nm_l, &seg_values_l);
    io_sleep(get_gcr_addr_thread(gcr_addr_i));
    tx_ffe_write_ffe(gcr_addr_i, is_5nm_l, &seg_values_l);
    io_sleep(get_gcr_addr_thread(gcr_addr_i));

    set_debug_state(0xC2FF); // tx_ffe end
}


/* find total number of 2r equivalent enabled nsegs or psegs
*/
uint32_t tx_ffe_get_zcal(t_gcr_addr* gcr_addr_i, bool is_nseg_i, bool is_5nm_i)
{
    set_debug_state(0xC210, TX_FFE_DBG_LVL); // tx_ffe_get_zcal begin

    uint32_t seg_en_2r_l = 0;
    uint32_t main_high_l = 0;
    uint32_t main_low_l  = 0;
    uint32_t pre1_width_l = is_5nm_i ? tx_pseg_pre1_en_width : tx_pseg_pre1_hs_en_width ;

    // using if rather than case to save space
    // PSL nseg
    if (is_nseg_i)
    {
        set_debug_state(0xC211, TX_FFE_DBG_LVL); // get en nseg

        // PSL nseg_5nm
        if (is_5nm_i)   //unused bits not guaranteed to be 0, thus cannot read entire reg
        {
            main_high_l = get_ptr_field(gcr_addr_i, tx_nseg_main_en);
            seg_en_2r_l = tx_ffe_fromVecWithHalf(main_high_l, tx_nseg_main_en_width);
        }
        else
        {
            main_low_l = get_ptr_field(gcr_addr_i, tx_nseg_main_16_24_hs_en);
            main_high_l = get_ptr_field(gcr_addr_i, tx_nseg_main_0_15_hs_en);
            seg_en_2r_l = tx_ffe_fromVecWithHalf(main_low_l | (main_high_l << 9), 25);
        }

        seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i,
                                              tx_nseg_pre1_en) >> (tx_nseg_pre1_en_width - pre1_width_l), pre1_width_l); // 5nm and 7nm fields overlap
        seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i, tx_nseg_pre2_en),
                                              tx_nseg_pre2_en_width); // 5nm and 7nm share pre2 reg

        if (is_5nm_i)
        {
            seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i, tx_nseg_post_en), tx_nseg_post_en_width);
        }
    }
    else
    {
        set_debug_state(0xC212, TX_FFE_DBG_LVL); // get en pseg

        // PSL pseg_5nm
        if (is_5nm_i)   //unused bits not guaranteed to be 0, thus cannot read entire reg
        {
            main_high_l = get_ptr_field(gcr_addr_i, tx_pseg_main_en);
            seg_en_2r_l = tx_ffe_fromVecWithHalf(main_high_l, tx_pseg_main_en_width);
        }
        else
        {
            main_low_l = get_ptr_field(gcr_addr_i, tx_pseg_main_16_24_hs_en);
            main_high_l = get_ptr_field(gcr_addr_i, tx_pseg_main_0_15_hs_en);
            seg_en_2r_l = tx_ffe_fromVecWithHalf(main_low_l | (main_high_l << 9), 25);
        }

        seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i,
                                              tx_pseg_pre1_en) >> (tx_pseg_pre1_en_width - pre1_width_l), pre1_width_l); // 5nm and 7nm fields overlap
        seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i, tx_pseg_pre2_en),
                                              tx_pseg_pre2_en_width); // 5nm and 7nm share pre2 reg

        if (is_5nm_i)
        {
            seg_en_2r_l += tx_ffe_fromVecWithHalf(get_ptr_field(gcr_addr_i, tx_pseg_post_en), tx_pseg_post_en_width);
        }
    }

    set_debug_state(0xC21F, TX_FFE_DBG_LVL); // tx_ffe_get_zcal end
    return seg_en_2r_l;
} // tx_ffe_get_zcal


/* compute number of 2R-equivalent pre1, pre2, and post segments used for ffe and main, and number of 2R equivalent
 * main bank segments to enable using zcal results and pre-cursor coefficients
 */
void tx_ffe_calc_ffe(t_gcr_addr* gcr_addr_i, uint32_t pre2_coef_x128_i, uint32_t pre1_coef_x128_i,
                     uint32_t post_coef_x128_i,
                     uint32_t zcal_result_nseg_i, uint32_t zcal_result_pseg_i, bool is_5nm_i, t_tx_ffe_cntl* seg_values_o)
{
    // constants in this routine
    uint32_t pre2_max_2r_l = tx_nseg_pre2_en_width * 2 - 1 ;
    uint32_t pre1_max_2r_l = (is_5nm_i ? tx_pseg_pre1_en_width : tx_pseg_pre1_hs_en_width) * 2 - 1 ;
    uint32_t post_max_2r_l = is_5nm_i ? tx_nseg_post_en_width * 2 - 1 : 0;
    set_tx_dcc_debug_tx_ffe(0xC250, pre2_max_2r_l );
    set_tx_dcc_debug_tx_ffe(0xC251, pre1_max_2r_l );
    set_tx_dcc_debug_tx_ffe(0xC252, post_max_2r_l );

    // ----------------------------------------
    // adjust zcal results
    //  (1) change a too-low value to the default
    //  (2) bound extreme min and max to the expected range
    // ----------------------------------------
    tx_ffe_bound_zcal(gcr_addr_i, &zcal_result_nseg_i);
    tx_ffe_bound_zcal(gcr_addr_i, &zcal_result_pseg_i);
    set_tx_dcc_debug_tx_ffe(0xC253, zcal_result_nseg_i );
    set_tx_dcc_debug_tx_ffe(0xC254, zcal_result_pseg_i );

    // ----------------------------------------
    // set total enabled segments needed
    //   these are reduced by amounts enabled in pre1 and pre2 banks, with the remaining satisfied by
    //   the main bank
    // ----------------------------------------
    seg_values_o->nseg.main = zcal_result_nseg_i ;
    seg_values_o->pseg.main = zcal_result_pseg_i ;

    // ----------------------------------------
    // calculate pre2 and pre1 bank segments selected as ffe and then allocated between pseg and nseg for ffe
    // the remaining main segments are reduced by the number used for ffe
    //   without a bound on the zcal result, the max value for ffe pre1 and post here is (68*32 + 64)/128, rounded down = 17;
    //       for pre2, it is (68*15 + 64)/128, rounded down = 8; the pre2, pre1, and post segments are sufficient
    //       to supply these.
    // ----------------------------------------
    seg_values_o->nseg.pre2.ffe = tx_ffe_calc_sel(seg_values_o->nseg.main, pre2_coef_x128_i) ;
    seg_values_o->pseg.pre2.ffe = tx_ffe_calc_sel(seg_values_o->pseg.main, pre2_coef_x128_i) ;
    seg_values_o->nseg.pre1.ffe = tx_ffe_calc_sel(seg_values_o->nseg.main, pre1_coef_x128_i) ;
    seg_values_o->pseg.pre1.ffe = tx_ffe_calc_sel(seg_values_o->pseg.main, pre1_coef_x128_i) ;
    seg_values_o->nseg.post.ffe = tx_ffe_calc_sel(seg_values_o->nseg.main, post_coef_x128_i) ;
    seg_values_o->pseg.post.ffe = tx_ffe_calc_sel(seg_values_o->pseg.main, post_coef_x128_i) ;

    seg_values_o->nseg.main -= (seg_values_o->nseg.pre2.ffe + seg_values_o->nseg.pre1.ffe + seg_values_o->nseg.post.ffe) ;
    seg_values_o->pseg.main -= (seg_values_o->pseg.pre2.ffe + seg_values_o->pseg.pre1.ffe + seg_values_o->pseg.post.ffe) ;

    set_tx_dcc_debug_tx_ffe(0xC255, seg_values_o->nseg.pre2.ffe);
    set_tx_dcc_debug_tx_ffe(0xC256, seg_values_o->pseg.pre2.ffe);
    set_tx_dcc_debug_tx_ffe(0xC257, seg_values_o->nseg.pre1.ffe);
    set_tx_dcc_debug_tx_ffe(0xC258, seg_values_o->pseg.pre1.ffe);
    set_tx_dcc_debug_tx_ffe(0xC259, seg_values_o->nseg.post.ffe);
    set_tx_dcc_debug_tx_ffe(0xC25A, seg_values_o->pseg.post.ffe);
    set_tx_dcc_debug_tx_ffe(0xC25B, seg_values_o->nseg.main);
    set_tx_dcc_debug_tx_ffe(0xC25C, seg_values_o->pseg.main);

    // use in opposite order of tx_zcal_tdr; tx_zcal_tdr disables main, then post, then pre2, then pre1
    tx_ffe_alloc_main(&seg_values_o->nseg.main, &seg_values_o->pseg.main, seg_values_o->nseg.pre1.ffe,
                      seg_values_o->pseg.pre1.ffe,
                      &seg_values_o->nseg.pre1.main, &seg_values_o->pseg.pre1.main, pre1_max_2r_l) ;
    tx_ffe_alloc_main(&seg_values_o->nseg.main, &seg_values_o->pseg.main, seg_values_o->nseg.pre2.ffe,
                      seg_values_o->pseg.pre2.ffe,
                      &seg_values_o->nseg.pre2.main, &seg_values_o->pseg.pre2.main, pre2_max_2r_l) ;
    tx_ffe_alloc_main(&seg_values_o->nseg.main, &seg_values_o->pseg.main, seg_values_o->nseg.post.ffe,
                      seg_values_o->pseg.post.ffe,
                      &seg_values_o->nseg.post.main, &seg_values_o->pseg.post.main, post_max_2r_l) ;

    set_tx_dcc_debug_tx_ffe(0xC25D, seg_values_o->nseg.pre2.main);
    set_tx_dcc_debug_tx_ffe(0xC25E, seg_values_o->pseg.pre2.main);
    set_tx_dcc_debug_tx_ffe(0xC25F, seg_values_o->nseg.pre1.main);
    set_tx_dcc_debug_tx_ffe(0xC260, seg_values_o->pseg.pre1.main);
    set_tx_dcc_debug_tx_ffe(0xC261, seg_values_o->nseg.post.main);
    set_tx_dcc_debug_tx_ffe(0xC262, seg_values_o->pseg.post.main);
    set_tx_dcc_debug_tx_ffe(0xC263, seg_values_o->nseg.main);
    set_tx_dcc_debug_tx_ffe(0xC264, seg_values_o->pseg.main);

} // tx_ffe_calc_ffe


/* write all bank control values
*/
void tx_ffe_write_ffe(t_gcr_addr* gcr_addr_i, bool is_5nm_i, t_tx_ffe_cntl* seg_values_i)
{
    tx_ffe_write_main_en(gcr_addr_i, seg_values_i->nseg.main, true, is_5nm_i) ;
    tx_ffe_write_main_en(gcr_addr_i, seg_values_i->pseg.main, false, is_5nm_i) ;

    tx_ffe_write_ffe_en_sel(gcr_addr_i, seg_values_i->nseg.pre2.main, seg_values_i->nseg.pre2.ffe,
                            seg_values_i->pseg.pre2.main,
                            seg_values_i->pseg.pre2.ffe, TX_FFE_BANKTYPE_PRE2, is_5nm_i) ;
    tx_ffe_write_ffe_en_sel(gcr_addr_i, seg_values_i->nseg.pre1.main, seg_values_i->nseg.pre1.ffe,
                            seg_values_i->pseg.pre1.main,
                            seg_values_i->pseg.pre1.ffe, TX_FFE_BANKTYPE_PRE1, is_5nm_i) ;

    // PSL 5nm
    if (is_5nm_i)
    {
        tx_ffe_write_ffe_en_sel(gcr_addr_i, seg_values_i->nseg.post.main, seg_values_i->nseg.post.ffe,
                                seg_values_i->pseg.post.main,
                                seg_values_i->pseg.post.ffe, TX_FFE_BANKTYPE_POST, is_5nm_i) ;
    }
} // tx_ffe_write_ffe


/* adjust the zcal result if below valid range or outside of bounds
*/
void tx_ffe_bound_zcal(t_gcr_addr* gcr_addr_i, uint32_t* zcal_2r_io)
{
    set_debug_state(0xC220, TX_FFE_DBG_LVL); // tx_ffe_bound_zcal begin

    // PSL 2r_lt_valid_min
    if (*zcal_2r_io < tx_ffe_zcal_valid_min_2r_c)
    {
        set_debug_state(0xC221, TX_FFE_DBG_LVL); // tx_ffe_bound_zcal at or below valid minimum
        ADD_LOG(DEBUG_TX_ZCAL_LIMIT, gcr_addr_i, *zcal_2r_io);
        *zcal_2r_io = tx_ffe_zcal_default_2r_c ;
    }

    // PSL 2r_lt_bound_min
    if (*zcal_2r_io < tx_ffe_zcal_bound_min_2r_c)
    {
        set_debug_state(0xC222, TX_FFE_DBG_LVL); // tx_ffe_bound_zcal below min bound
        ADD_LOG(DEBUG_TX_ZCAL_LIMIT, gcr_addr_i, *zcal_2r_io);
        *zcal_2r_io = tx_ffe_zcal_bound_min_2r_c;
    }

    // PSL 2r_gt_bound_max
    if (*zcal_2r_io > tx_ffe_zcal_bound_max_2r_c)
    {
        set_debug_state(0xC223, TX_FFE_DBG_LVL); // tx_ffe_bound_zcal above max bound
        ADD_LOG(DEBUG_TX_ZCAL_LIMIT, gcr_addr_i, *zcal_2r_io);
        *zcal_2r_io = tx_ffe_zcal_bound_max_2r_c;
    }

    set_debug_state(0xC22F, TX_FFE_DBG_LVL); // tx_ffe_bound_zcal end
}


/* calculate ffe select values given an impedance calibration value and a coeficient
 * the coeficient is an integer representing the decimal coeficient multiplied by 128
 * and rounded to the nearest value
 * the calibration value would typically by the 2R equivalent resistance, though this
 *  function would work with any convention
 * the result is rounded to the nearest integer
 * we intentionally round 1/2 up to reduce the impact of alg limitation w/high zcal results
 * the lower result is odd and the higher result is even
 */
uint32_t tx_ffe_calc_sel(uint32_t zcal_i, uint32_t ffe_coef_x128_i)
{
    return ((zcal_i * ffe_coef_x128_i) + 64) >> 7 ;
}


/* calculate ffe bank segments used for main
 */
void tx_ffe_alloc_main(uint32_t* nseg_remain, uint32_t* pseg_remain, uint32_t nseg_ffe_i, uint32_t pseg_ffe_i,
                       uint32_t* nseg_main, uint32_t* pseg_main, uint32_t bank_max_2r_i)
{
    // 1)  calculate amount allocated to ffe; this is different than the max of pseg and nseg ffe because
    // one could use the 2r segment and the other might not. for example, pseg could require 4 segments
    // for ffe and nseg 3; a total of 5 2r equivalents would be required to be reserved for ffe -- the
    // 2-1R segments used for pseg and the 1-2R segment used up by nseg
    // ----------------------------------------
    // This operates by recognizing that this additional segment is only needed
    // when the maximum is even; thus, we can 'or' in the segment when needed rather
    // than doing an addition operation.
    uint32_t used_for_ffe_2r_l = max(nseg_ffe_i, pseg_ffe_i) | (nseg_ffe_i & 0x1) | (pseg_ffe_i & 0x1) ;
    // 2) allocate remaining segments to main and subtract main segments needed
    tx_ffe_alloc_main_phase(nseg_remain, nseg_main, bank_max_2r_i, used_for_ffe_2r_l);
    tx_ffe_alloc_main_phase(pseg_remain, pseg_main, bank_max_2r_i, used_for_ffe_2r_l);
} // tx_ffe_alloc_main

/* calculate ffe bank segments used for main for one phase
 */
void tx_ffe_alloc_main_phase(uint32_t* seg_remain, uint32_t* seg_main, uint32_t bank_max_2r_i,
                             uint32_t used_for_ffe_2r_i)
{
    // segments not allocated for ffe are used for main up to amount needed
    *seg_main = min(*seg_remain, bank_max_2r_i - used_for_ffe_2r_i) ;

    // the main segments and ffe segments cannot both be odd
    // PSL used_for_ffe_2r
    if ((used_for_ffe_2r_i & 0x1) == 1)
    {
        *seg_main &= 0xFFFE;
    }

    *seg_remain -= *seg_main;
} // tx_ffe_alloc_main_phase


/* write the main-bank enable after converting a number of 2R-equivalent segments to therm code and splitting between
 * high and low registers
 */
void tx_ffe_write_main_en(t_gcr_addr* gcr_addr_i, uint32_t num_2r_equiv_i, bool is_nseg_i, bool is_5nm_i)
{
    set_debug_state(0xC230, TX_FFE_DBG_LVL); // tx_ffe_write_main_en begin

    uint32_t high_bits_l;
    uint32_t low_bits_l;

    /* There are scenarios or possible scenarios where there are not enough main bank segments to meet
     * requirements. This happens when the zcal result is near max and shared selects between nseg and
     * pseg ffe banks force some unused ffe bank segments. For example, the n leg may want 7 pre1
     * segments selected the p leg 8. Both n and p will select 4 1R and 1 2R segment. n leg will enable
     * 3 1R and 1 2R segment while the p leg will enable 4 1R segments. The n leg will sacrifice 1 1R
     * segment to share a select and the p leg will sacrifice 1 2R segment. This results in up to
     * 1-1R segment per ffe bank unused. If the result of zcal is a large value and these segments
     * are unused, the leftover segments for the main bank may be too great. Thus we go to the work
     * of bounding the the remaining segments. In this scenario, the zcal result is relatively large
     * so the error of reducing the enabled segments vs the calibrated value is small proportionally.
     * */
    uint32_t therm_width_l;

    if (is_5nm_i)
    {
        therm_width_l = tx_nseg_main_en_width;
    }
    else
    {
        therm_width_l = tx_nseg_main_0_15_hs_en_width + tx_nseg_main_16_24_hs_en_width;
    }

    /*  This code is shared by 5nm and 7nm since the therm code uses msb bits first and the msb of the 5nm
     *  main bank reg is the same as the msb of the 7nm main bank reg */

    uint32_t full_therm_l = tx_ffe_toThermWithHalfRev(min(num_2r_equiv_i, (therm_width_l << 1) - 1),
                            tx_nseg_main_0_15_hs_en_width + tx_nseg_main_16_24_hs_en_width);
    high_bits_l = full_therm_l >> (tx_nseg_main_16_24_hs_en_width);
    low_bits_l = full_therm_l & ((0x1 << tx_nseg_main_16_24_hs_en_width) - 1); // for 5nm will always be 0

    // PSL nseg
    if (is_nseg_i)
    {
        set_debug_state(0xC231, TX_FFE_DBG_LVL); // write main_nseg

        // PSL nseg_5nm
        if (!is_5nm_i)
        {
            put_ptr_field_fast(gcr_addr_i, tx_nseg_main_16_24_hs_en, low_bits_l );
        }

        put_ptr_field_fast(gcr_addr_i, tx_nseg_main_0_15_hs_en, high_bits_l);
    }
    else   // TX_FFE_SEGTYPE_MAIN_PSEG
    {
        set_debug_state(0xC232, TX_FFE_DBG_LVL); // write main_pseg

        // PSL nseg_5nm
        if (!is_5nm_i)
        {
            put_ptr_field_fast(gcr_addr_i, tx_pseg_main_16_24_hs_en, low_bits_l );
        }

        put_ptr_field_fast(gcr_addr_i, tx_pseg_main_0_15_hs_en, high_bits_l);
    }

    set_debug_state(0xC23F, TX_FFE_DBG_LVL); // tx_ffe_write_main_en end
} // tx_ffe_write_main_en


/* write ffe enables; for the ffe banks we want to treat unused segments differently between
 * 5nm and 7nm; for 5nm, we select but don't enable unused segments; for 7nm we don't select
 * or enable unused segments.
 */
void tx_ffe_write_ffe_en_sel(t_gcr_addr* gcr_addr_i, uint32_t num_2r_equiv_nseg_main_en_i,
                             uint32_t num_2r_equiv_nseg_ffe_en_i,
                             uint32_t num_2r_equiv_pseg_main_en_i, uint32_t num_2r_equiv_pseg_ffe_en_i, t_tx_ffe_ffe_banktype banktype_i,
                             bool is_5nm_i)
{
    set_debug_state(0xC240, TX_FFE_DBG_LVL); // tx_ffe_write_ffe_en_sel begin
    uint32_t bank_width = 0;
    uint32_t nseg_sel_l;
    uint32_t nseg_en_l;
    uint32_t pseg_sel_l;
    uint32_t pseg_en_l;

    // using if/else rather than case to save space
    // PSL banktype_pre2
    if (banktype_i == TX_FFE_BANKTYPE_PRE2)
    {
        bank_width = tx_pre2_sel_width;
    }
    // PSL banktype_pre1
    else if (banktype_i == TX_FFE_BANKTYPE_PRE1)
    {
        bank_width = is_5nm_i ? tx_pseg_pre1_en_width : tx_pseg_pre1_hs_en_width ;
    }
    else   // TX_FFE_BANKTYPE_POST
    {
        bank_width = tx_post_sel_width;
    }

    // calculate nseg and pseg en and sel register values given number of 2r segments needed for
    // main and ffe; ffe segments use the lsbs, main uses the msbs
    nseg_sel_l = tx_ffe_toThermWithHalf(num_2r_equiv_nseg_ffe_en_i, bank_width);
    nseg_en_l = nseg_sel_l | tx_ffe_toThermWithHalfRev(num_2r_equiv_nseg_main_en_i, bank_width);

    pseg_sel_l = tx_ffe_toThermWithHalf(num_2r_equiv_pseg_ffe_en_i, bank_width);
    pseg_en_l = pseg_sel_l | tx_ffe_toThermWithHalfRev(num_2r_equiv_pseg_main_en_i, bank_width);

    // write the results after adjusting for 5nm, 7nm reg differences
    // PSL banktype_pre2
    if (banktype_i == TX_FFE_BANKTYPE_PRE2)   // pre2 en is the same for 5nm and 7nm
    {
        set_debug_state(0xC241, TX_FFE_DBG_LVL); // write pre2 en and sel

        // PSL pre2_5nm
        if ( is_5nm_i )
        {
            put_ptr_field_fast(gcr_addr_i, tx_pre2_sel, nseg_sel_l | pseg_sel_l);
        }
        else
        {
            put_ptr_field_fast(gcr_addr_i, tx_nseg_pre2_hs_sel, nseg_sel_l);
            put_ptr_field_fast(gcr_addr_i, tx_pseg_pre2_hs_sel, pseg_sel_l);
        }

        // sel must be written before enable to avoid main and ffe enabled and selected on same segment
        put_ptr_field_fast(gcr_addr_i, tx_nseg_pre2_en, nseg_en_l);
        put_ptr_field_fast(gcr_addr_i, tx_pseg_pre2_en, pseg_en_l);
    }
    // PSL banktype_pre1
    else if (banktype_i == TX_FFE_BANKTYPE_PRE1)
    {
        set_debug_state(0xC242, TX_FFE_DBG_LVL); // write pre1 en and sel

        // sel must be written before enable to avoid main and ffe enabled and selected on same segment
        // PSL pre1_5nm
        if ( is_5nm_i )   // for 5nm we select, but don't enable unused segments
        {
            put_ptr_field_fast(gcr_addr_i, tx_pre1_sel, nseg_sel_l | pseg_sel_l);
        }
        else     // for 7nm we don't select or enable unused segments
        {
            put_ptr_field_fast(gcr_addr_i, tx_nseg_pre1_hs_sel, nseg_sel_l);
            put_ptr_field_fast(gcr_addr_i, tx_pseg_pre1_hs_sel, pseg_sel_l);
        }

        // 5nm and 7nm pre1 are different widths; the earlier code uses the width to
        // use the low bits for ffe and high bits for main, and leaves the result
        // in the low order bits of the uint32_t value. So here we shift that
        // result and use the full register on the write.

        put_ptr_field_fast(gcr_addr_i, tx_nseg_pre1_en, nseg_en_l << (tx_nseg_pre1_en_width - bank_width));
        put_ptr_field_fast(gcr_addr_i, tx_pseg_pre1_en, pseg_en_l << (tx_pseg_pre1_en_width - bank_width));
    }
    else   // TX_FFE_BANKTYPE_POST
    {
        set_debug_state(0xC243, TX_FFE_DBG_LVL); // write post en and sel

        // sel must be written before enable to avoid main and ffe enabled and selected on same segment
        put_ptr_field_fast(gcr_addr_i, tx_post_sel, nseg_sel_l | pseg_sel_l);
        put_ptr_field_fast(gcr_addr_i, tx_nseg_post_en, nseg_en_l);
        put_ptr_field_fast(gcr_addr_i, tx_pseg_post_en, pseg_en_l);
    }

    set_debug_state(0xC24F, TX_FFE_DBG_LVL); // tx_ffe_write_ffe_en_sel end
} // tx_ffe_write_ffe_en_sel
