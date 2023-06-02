/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_dfe.c $        */
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
// *! FILENAME    : eo_dfe.c
// *! TITLE       :
// *! DESCRIPTION : Run DFE H1-3
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Chris Steffen       Email: cwsteffen@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr23041700 |vbr     | EWM 302758: skip DFE Fast BIST checks in RxEqEval.
// vbr23033000 |vbr     | EWM 302249: Moved sleep in DFE Full latch update loop and added quick abort from loop
// vbr23031300 |vbr     | Updated sleeps in dfe full for EWM301148.
// vbr23030100 |vbr     | Allow running DFE Full in PCIe Gen 1/2 recal with no H3-H1 pattern matching
// vbr23021700 |vbr     | Issue 299509: Added sleep to fix thread active violation between bank sync and dfe full in recal
// vbr23010400 |vbr     | Issue 296947: Adjusted latch_dac accesses for different addresses on Odyssey vs P11/ZMetis
// vbr22092700 |vbr     | Issue 290398: Add ppe config to disable servo min/max errors for DFE Full
// mbs22082601 |mbs     | Updated with PSL comments
// vbr22060900 |vbr     | Always increment the DFE Full quad after bank B regardless of run_all_quads
// vbr22051700 |vbr     | Issue 280487: Limit DFE Fast ClkAdj to 12
// vbr21022300 |vbr     | Added H1 adjust to DFE Fast. Other improvements.
// vbr22022500 |vbr     | Removed reporting of last eye height in DFE full
// vbr21080900 |vbr     | Cleaned up checking of dfe full results
// vbr21052600 |vbr     | Updating quad mode for dfe full
// mwh21070900 |mwh     | Add registers rx_dfe_h1_min/max_check so can program limits for dft_h1 fast(should had this in P9)
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// vbr21052600 |vbr     | Updated debug states
// vbr21052700 |vbr     | Rename sl*ve_mode to external_mode
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// vbr21051000 |vbr     | Story 236077: wait for dac accel to finish
// vbr21050700 |vbr     | Switch to DFE Fast Method 2 implementation
// vbr21042000 |vbr     | Update Hx coeff mem_regs each loop for debugging.
// vbr21040800 |vbr     | Added reading of  previous clock adjust (assuming no NS/EW QPA) and cleaned up BIST check.
// vbr21040700 |vbr     | Added way to select to run H1 or H1-H3 for DFE fast (re-enabled H2, H3).
// vbr21012800 |vbr     | Skip eye height checks when clr_eye_height_width is set
// vbr20121100 |vbr     | Removed P10 dd1 code
// vbr20111301 |vbr     | DFE clkadj for bank b is more correct now, but still needs work.  Added some more comments
// vbr20111300 |vbr     | Removed the error check on max iterations in dfe fast
// vbr20082600 |vbr     | A lot of changes to speed up dfe_fast: H1 only, 2 loops max, write coeffs at end only, removed polling/error checks.
// vbr20043000 |vbr     | Switched to common function for servo queues empty check
// bja20120100 |bja     | HW553981: In DFE clock adjust, set error if Ap is less than zero
// vbr20050500 |vbr     | Using Mini PR stepper for setting clock adjust
// vbr20040900 |vbr     | Added checking of DAC accelerator error
// vbr20040800 |vbr     | Fixed clkadj calc and added store/read of old DFE coefficients
// vbr20040700 |vbr     | Switched to using DAC accelerator for applying DFE Fast. Not a final implementation since not re-runnable.
//-------------|P11     |-------------------------------------------------------
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// vbr21030800 |vbr     | HW560155: clk_adj=0 is valid and should not give an error
// mwh20012100 |mwh     | Add in code to set rx_dfe_fail if servo op has issue
// mbs20111800 |mbs     | Reversed dfe H1 adjust for dd1 dac swizzle
// bja20120100 |bja     | HW553981: In DFE clock adjust, set error if Ap is less than zero
// mbs20092800 |mbs     | Updated hysteresis for DFE
// vbr20091600 |vbr     | HW546085: Added observation mem_regs for DFE Fast measured/calculated AP
// vbr20091100 |vbr     | HW546085: Added observation mem_regs for DFE Fast coefficients
// bja20090900 |bja     | Use common is_p10_dd1() check
// mbs20080500 |mbs     | HW539048- Added dfe_quad_mode to allow dfe full training of only one quadrant
// mbs20073000 |mbs     | LAB - Updated dfe h1 check to error at -20 instead of 0
// mbs20073000 |mbs     | LAB - Updated dfrx_pr_fw_inertia_amt_coarsee clkadj to only adjust for h1 > 0
// mbs20073000 |mbs     | LAB - Updated dfe_fast H coefficient calculations for DAC swizzle bug
// mbs20073000 |mbs     | LAB - Updated dfe_full servo op call to twist servo_op patterns for DAC swizzle bug
// mbs20031000 |mbs     | HW525009 - Switch external mode to bank B when copying results for fast DFE
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// vbr20021300 |vbr     | Added Min Eye Height code
// cws20011400 |cws     | Added Debug Logs
// vbr19111500 |vbr     | Initial implementation of debug levels
// mbs19110600 |mbs     | HW468344 - Move check for servo queues empty to beginning of functions
// mbs19110600 |mbs     | HW478019 - Set fir warning when DAC limit is reached
// vbr19081300 |vbr     | Removed mult_int16 (not needed for ppe42x)
// bja19082900 |bja     | Rename rx_dfe_h1_coeff to rx_dfe_clkadj_coeff
// bja19062100 |bja     | Read K from rx_dfe_h1_coeff
// mbs19072500 |mbs     | Removed several calls to set_debug_state to reduce code size
// mbs19062700 |mbs     | HW493492 - Set DFE hysteresis values from 3/6 to 1/3 due to VDAC change
// mbs19062500 |mbs     | HW493492 - Set DFE_FULL_H1_ADJ to 1 due to VDAC change
// mbs19062000 |mbs     | HW493492 - Set DFE_FULL_H1_ADJ to 2
// mbs19061400 |mbs     | HW493492 - Restore DFE_FULL_H1_ADJ to 0
// mbs19060800 |mbs     | HW493492 - Added DFE_FULL_H1_ADJ and set to 2
// cws19053000 |cws     | Changed several function calls to static to make them non global calls
// vbr19051700 |vbr     | HW491892: Updated DAC limits
// vbr19051400 |vbr     | HW491892: Change VDAC from 9-bit SM to 8-bit twos_comp
// mbs19051700 |mbs     | HW491617 - changed K value from 16 to 20
// mbs19051600 |mbs     | HW491617 - separated servo setup for dfe_fast and dfe_full
// mwh19043000 |mwh     | add in set_fir(fir_code_dft_error);
// mbs19041001 |mbs     | Added recal flag to dfe_full function (HW486784)
// vbr19040100 |vbr     | HW483082: Step mini-pr by 1 (<4) when applying dfe clock adjust
// mbs19040201 |mbs     | HW483129 Added assignment of l_h1_vals[0]
// mbs19030500 |mbs     | Changed abort state from 711A to 701F
// vbr19012400 |vbr     | Added debug state for abort
// vbr19012200 |vbr     | Updated function calls
// cws19011300 |cws     | Added servo queue empty check and changed debug states
// mwh19011100 |mwh     | Add () because the || could could cause compare to wrong thing
// vbr18120700 |mwh     | Added rxbist for h1 and dfe max and min and done,
// vbr18111400 |vbr     | Added call to set_fir
// cws18071000 |cws     | Initial Rev
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "servo_ops.h"
#include "ioo_common.h"
#include "eo_dfe.h"
#include "io_logger.h"

#include "io_config.h"

//------------------------------------------------------------------------------
// Constant Definitions
//------------------------------------------------------------------------------
#define DAC_BASE_ADDR get_latch_dac_addr(rx_ad_latch_dac_n000)  // Issue 296947 Workaround
#define DAC_STARTBIT  rx_ad_latch_dac_n000_startbit
#define DAC_ENDBIT    rx_ad_latch_dac_n000_endbit

#define SET_DFE_DEBUG(...) SET_DFE_DEBUG_IMPL(DFE_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

// These defines will return the number of arguments
#define DFE_VA_NARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define DFE_VA_NARGS(...) DFE_VA_NARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1)

// Uses the found number of arguments to call the correct pound define
#define SET_DFE_DEBUG_IMPL2(count, ...) SET_DFE_DEBUG ## count (__VA_ARGS__)
#define SET_DFE_DEBUG_IMPL(count, ...) SET_DFE_DEBUG_IMPL2(count, __VA_ARGS__)

// Defines based upon the argument count.
#define SET_DFE_DEBUG1 set_debug_state
#define SET_DFE_DEBUG2 SET_DFE_DEBUG_WITH_VALUE

// Sets the debug state and also a mem regs debug dfe address
#if IO_DEBUG_LEVEL < 3
    #define SET_DFE_DEBUG_WITH_VALUE(i_debug_state, i_value) {}
#else
    #define SET_DFE_DEBUG_WITH_VALUE(i_debug_state, i_value) { set_debug_state(i_debug_state);  mem_regs_u16[pg_addr(rx_dfe_debug_addr)] = (i_value); }
#endif



// Amp Meas Servo Op
// AAAA BBBB BCDD DEEE
// A = Mask (Does not include H0) [H3][H2][H1][H0][H-1]
// B = Pattern [H3][H2][H1][H0][H-1]
// C = Bank (A=0, B=1)
// D = Quad (N=0, E=1, S=2, W=3)
// E = Latch (L000=0 ... L111=7)

// Declare servo op arrays as static globals so they are placed in static memory thus reducing code size and complexity.

#define SERVO_OP_MASK_ALL     0xF000 // Allows H3,H2,H1,H-1 to be used in pattern matching
#define SERVO_OP_MASK_H321_X  0x1000 // Allows H-1 to be used in pattern matching while H3,H2,H1 are don't care
#define SERVO_OP_AP           0x0100 // Sets H0(1) H-1(0)
#define SERVO_OP_AN           0x0080 // Sets H0(0) H-1(1)
#define SERVO_OP_FILT_AX000XX 0x0000
#define SERVO_OP_FILT_AX001XX 0x0200
#define SERVO_OP_FILT_AX010XX 0x0400
#define SERVO_OP_FILT_AX011XX 0x0600
#define SERVO_OP_FILT_AX100XX 0x0800
#define SERVO_OP_FILT_AX101XX 0x0A00
#define SERVO_OP_FILT_AX110XX 0x0C00
#define SERVO_OP_FILT_AX111XX 0x0E00

typedef enum
{
    QUAD_NORTH = 0x0000,
    QUAD_EAST  = 0x0008,
    QUAD_SOUTH = 0x0010,
    QUAD_WEST  = 0x0018
} QUADRANT;

typedef enum
{
    L000       = 0,
    L001       = 1,
    L010       = 2,
    L011       = 3,
    L100       = 4,
    L101       = 5,
    L110       = 6,
    L111       = 7,
    LATCH_SIZE = 8,
    LXX1_MASK  = 1,
    LXX1_MASK_PDD1 = 4
} LATCH;

typedef enum
{
    BANK_A    = 0,
    BANK_B    = 1,
    BANK_SIZE = 2
} BANK;

typedef enum
{
    PR_A_NS_DATA = 0,
    PR_A_EW_DATA = 1,
    PR_B_NS_DATA = 2,
    PR_B_EW_DATA = 3,
    PR_DATA_SIZE = 4
} PR_DATA;

const static uint16_t SERVO_OPS[8] =
{
    SERVO_OP_FILT_AX000XX,
    SERVO_OP_FILT_AX001XX,
    SERVO_OP_FILT_AX010XX,
    SERVO_OP_FILT_AX011XX,
    SERVO_OP_FILT_AX100XX,
    SERVO_OP_FILT_AX101XX,
    SERVO_OP_FILT_AX110XX,
    SERVO_OP_FILT_AX111XX
};

static uint16_t dfe_fast_servo_ops_a[8] =
{
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX100XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX101XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX010XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX110XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX111XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX000XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX011XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX001XX | SERVO_OP_AP | (BANK_A << 6) | QUAD_NORTH | L000
};

/*static uint16_t dfe_fast_servo_ops_b[8] =
{
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX100XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX101XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX010XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX110XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX111XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX000XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX011XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000,
    SERVO_OP_MASK_ALL | SERVO_OP_FILT_AX001XX | SERVO_OP_AP | (BANK_B << 6) | QUAD_NORTH | L000
};*/


//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------

static uint32_t rx_eo_dfe_check_dac_limits(t_gcr_addr* i_tgt, const int32_t i_new_val)
{
    int l_check_dfe_min =  TwosCompToInt(mem_pg_field_get(rx_dfe_min_check), rx_dfe_min_check_width);
    int l_check_dfe_max =  TwosCompToInt(mem_pg_field_get(rx_dfe_max_check), rx_dfe_max_check_width);

    if ( (i_new_val < l_check_dfe_min) || (i_new_val > l_check_dfe_max) )
    {
        uint32_t l_lane = get_gcr_addr_lane(i_tgt);
        mem_pl_bit_set(rx_dfe_fail, l_lane);
        // PSL set_fir_dft_error_and_bad_lane_warning
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_DFE_DAC_LIMIT, i_tgt, i_new_val);
        return warning_code;
    }
    else
    {
        return rc_no_error;
    }
}//rx_eo_dfe_check_dac_limits


/**
 * @brief Round to reduce bias
 * If the result is positive then we want to add 1 to the rounding, if the result is neg then do nothing
 * - This is due to the twos complement of the negative number. in order to reduce bias
 *
 * @param[in]  i_tgt      Reference to Bus Target
 * @param[out] o_pr_data  Read Mini PR Data Positions
 *
 * @return void
 */
static inline int32_t eo_dfe_round(const int32_t i_val)
{
    // Find the rounded value of (i_val / 2)
    return ((i_val >= 0) ? (i_val + 1) : i_val) >> 1;
}


/**
 * @brief Calculate DFE Clock Adjust
 *
 * @param[in] i_tgt   Reference to Bus Target
 * @param[in] i_h1    Calculated H1 Value
 * @param[in] i_ap1   AP1 Servo Op Result
 * @param[in] i_ap0   AP0 Servo Op Result
 *
 * @return void
 */
static inline uint32_t  rx_eo_dfe_calc_clk_adj(t_gcr_addr* i_tgt, int32_t i_h1, int32_t i_ap1, int32_t i_ap0,
        int32_t* io_clk_adj)
{
    const int32_t K             = TwosCompToInt(mem_pg_field_get(rx_dfe_clkadj_coeff), rx_dfe_clkadj_coeff_width);
    int32_t       l_ap          = eo_dfe_round(i_ap1 + i_ap0); // Average AP1 & AP0 = AP
    int32_t       l_new_clk_adj = 0;
    uint32_t      l_rc          = pass_code;

    // Report AP and H1
    int32_t l_lane = get_gcr_addr_lane(i_tgt);
    mem_pl_field_put(rx_dfe_ap,      l_lane, l_ap);
    mem_pl_field_put(rx_dfe_coef_h1, l_lane, i_h1);

    // Never divide by ZERO or negative
    if (l_ap <= 0)
    {
        set_debug_state(0x7030);
        // PSL set_fir_bad_lane_warning_div_by_zero
        set_fir(fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_RX_DFE_AP_ZERO_FAIL, i_tgt, i_ap1);
        l_rc = warning_code;
    }
    // PSL h1_gt_0
    else if ( i_h1 > 0 )
    {
        // Only allow the clock adjust to grow.
        // - This will prevent the DFE Values + Clk Adjust from oscillating
        // - Cap the clock adjust at 12
        l_new_clk_adj = div_uint32(K * i_h1, l_ap); // (K * i_h1) / l_ap

        // PSL new_clk_adj_gt_12
        if (l_new_clk_adj > 12)
        {
            l_new_clk_adj = 12;
        }

        // PSL new_clk_adj_gt_prev
        if (l_new_clk_adj > *io_clk_adj)
        {
            *io_clk_adj = l_new_clk_adj;
        }
    }

    return l_rc;
}

/**
 * @brief Applying DFE Clock Adjust as a Differential from the Previous DFE Clock Adjust. Must be a positive differential.
 *        Surprisingly, leaving this as inline is slightly less code size (01.28.2022).
 *
 * @param[in]     i_tgt           Reference to Bus Target
 * @param[in]     i_prev_clk_adj  Previous clock adjust
 * @param[in]     i_new_clk_adj   New clock adjust
 * @param[in]     i_bank          Bank to adjust
 *
 * @return void
 */
static inline void rx_eo_dfe_set_clock_adj(t_gcr_addr* i_tgt, int32_t i_prev_clk_adj, int32_t i_new_clk_adj,
        const t_bank i_bank)
{
    // Only apply new clock adjust if it has increased (never go smaller to prevent oscillations)
    // PSL new_clk_adj_gt_prev
    if (i_new_clk_adj > i_prev_clk_adj)
    {
        // Step by 1 from old to new clock adjust.
        // clk_adj is positive but this corresponds to negative Data PR adjustment
        int clk_adj_diff = IntToTwosComp(i_prev_clk_adj - i_new_clk_adj, rx_mini_pr_step_data_adj_width);
        put_ptr_field(i_tgt, rx_mini_pr_step_data_adj, clk_adj_diff,
                      fast_write); // fast_write OK, rx_mini_pr_step_data/edge_adj are only fields in register

        // Only touch bank A or bank B according to the parameter (HW525009)
        // The stepper is fast enough that don't need to wait for done.
        //int l_step_done;
        // PSL put_mini_pr_step_done_bank_a
        if ( i_bank == bank_a )
        {
            put_ptr_field(i_tgt, rx_mini_pr_step_a_run_done_alias, 0b11000000,
                          fast_write); // OK to write 0 to rest of register, bits[0:1] = ns_data_run, ew_data_run; bits[4:5] = ns_data_done, ew_data_done
            //do { l_step_done = get_ptr_field(i_tgt, rx_mini_pr_step_a_done_alias); } while (l_step_done != 0b1100); // bits[0:1] = ns_data_done, ew_data_done
        }
        else       //bank_b
        {
            put_ptr_field(i_tgt, rx_mini_pr_step_b_run_done_alias, 0b11000000,
                          fast_write); // OK to write 0 to rest of register, bits[0:1] = ns_data_run, ew_data_run; bits[4:5] = ns_data_done, ew_data_done
            //do { l_step_done = get_ptr_field(i_tgt, rx_mini_pr_step_b_done_alias); } while (l_step_done != 0b1100); // bits[0:1] = ns_data_done, ew_data_done
        }
    } //if(i_new_clk_adj > i_prev_clk_adj)

    return;
}



/**
 * @brief DFE Fast Servo Measurements (4 Ops)
 *
 * @param[in]  i_tgt      Reference to Bus Target
 * @param[in]  i_loff     Previous DAC value from latch offset
 * @param[out] o_coefs    Found Coefficient Values (7_0, 1_6, 5_2, 3_4)
 * @param[io]  io_clk_adj Clk Adjust
 * @param[in]  i_h1_only  True = H1 only (method 1).  False = H1-H3 (method 2).
 *
 * @return uint32_t. pass_code if success, else error code.
 */
static uint32_t rx_eo_dfe_fast_servo(t_gcr_addr* i_tgt, int32_t i_loff, int32_t o_coefs[4], int32_t* io_clk_adj,
                                     bool i_h1_only)
{
    SET_DFE_DEBUG(0x7020); // Enter DFE Fast Servos
    const uint32_t AP10010       = 0; //needed for h1 in method 1
    const uint32_t AP10110       = 1; //needed for h1 in method 1
    const uint32_t AP01010       = 2; //needed for hx in method 1
    const uint32_t AP11010       = 3; //needed for hx in method 1
    const uint32_t AP11110       = 4;
    const uint32_t AP00010       = 5;
    const uint32_t AP01110       = 6;
    const uint32_t AP00110       = 7;
    uint32_t       l_rc          = pass_code;

    // Run Servo Ops
    int32_t  l_servo_results[8];
    uint16_t* l_servo_ops = dfe_fast_servo_ops_a;
    int l_num_servo_ops   = i_h1_only ? 2 : 8;
    l_rc = run_servo_ops_and_get_results(
               i_tgt,
               c_servo_queue_amp_meas,
               l_num_servo_ops,
               l_servo_ops,
               l_servo_results);

    // PSL servo_error
    if (l_rc)
    {
        goto function_exit;
    }

    // H1 Adjust to help with measurement bias
    uint32_t dfe_fast_h1_adj   = TwosCompToInt(mem_pg_field_get(rx_dfe_fast_h1_adj), rx_dfe_fast_h1_adj_width);

    // Use the AP measurements to calculate the DFE coefficients
    //       H1 = (Ap10110 - Ap10010)/2
    //       H2 = (Ap11010 - Ap10010)/2
    //       H3 = (Ap11010 - Ap01010)/2
    // coef_7_0 = (Ap11110 - Ap00010)/2 =  H3 + H2 + H1
    // coef_1_6 = (Ap00110 - Ap11010)/2 = -H3 - H2 + H1
    // coef_5_2 = (Ap10110 - Ap01010)/2 =  H3 - H2 + H1
    // coef_3_4 = (Ap01110 - Ap10010)/2 = -H3 + H2 + H1
    int l_coef_7_0, l_coef_1_6, l_coef_5_2, l_coef_3_4;
    int l_h1      = eo_dfe_round(l_servo_results[AP10110] - l_servo_results[AP10010]);

    // PSL h1_only
    if (i_h1_only)
    {
        l_coef_7_0  = dfe_fast_h1_adj + l_h1;
        l_coef_1_6  = dfe_fast_h1_adj + l_h1;
        l_coef_5_2  = dfe_fast_h1_adj + l_h1;
        l_coef_3_4  = dfe_fast_h1_adj + l_h1;
    }
    else
    {
        l_coef_7_0  = dfe_fast_h1_adj + eo_dfe_round(l_servo_results[AP11110] - l_servo_results[AP00010]);
        l_coef_1_6  = dfe_fast_h1_adj + eo_dfe_round(l_servo_results[AP00110] - l_servo_results[AP11010]);
        l_coef_5_2  = dfe_fast_h1_adj + eo_dfe_round(l_servo_results[AP10110] - l_servo_results[AP01010]);
        l_coef_3_4  = dfe_fast_h1_adj + eo_dfe_round(l_servo_results[AP01110] - l_servo_results[AP10010]);
    }

    o_coefs[0] = l_coef_7_0;
    o_coefs[1] = l_coef_1_6;
    o_coefs[2] = l_coef_5_2;
    o_coefs[3] = l_coef_3_4;

    // Calculate DFE Clock Adjust
    int32_t l_ap1 = l_servo_results[AP10110] - i_loff;
    int32_t l_ap0 = l_servo_results[AP10010] - i_loff;
    l_rc = rx_eo_dfe_calc_clk_adj(i_tgt, l_h1, l_ap1, l_ap0, io_clk_adj);

function_exit:
    SET_DFE_DEBUG(0x7025); // Exit DFE Fast Looper
    return l_rc;
}


/**
 * @brief Check DFE Coefficients
 *
 * @param[in]  i_tgt        Reference to Bus Target
 *
 * @return uint32_t. pass_code if success, else error code.
 */
static int32_t rx_eo_dfe_check_hvals(t_gcr_addr* i_tgt)
{
    int32_t l_rc = pass_code;
    int32_t l_lane = get_gcr_addr_lane(i_tgt);

    // Rxbist check of H1 value
    int rx_dfe_h1_check_en_int = (mem_pg_field_get(rx_running_eq_eval) == 1) ? 0 : get_ptr_field(i_tgt, rx_dfe_h1_check_en);
    uint32_t l_fir_code = rx_dfe_h1_check_en_int ? (fir_code_dft_error | fir_code_bad_lane_warning) :
                          fir_code_bad_lane_warning;

    int32_t l_h1 = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_h1, l_lane), rx_dfe_coef_h1_width);

    int32_t rx_dfe_h1_max_check = TwosCompToInt(mem_pg_field_get(rx_dfe_h1_max_check), rx_dfe_h1_max_check_width);
    int32_t rx_dfe_h1_min_check = TwosCompToInt(mem_pg_field_get(rx_dfe_h1_min_check), rx_dfe_h1_min_check_width);

    if ( (l_h1 < rx_dfe_h1_min_check) || (l_h1 > rx_dfe_h1_max_check) )
    {
        set_debug_state(0x7060);
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(l_fir_code);
        ADD_LOG(DEBUG_RX_DFE_H1_LIMIT, i_tgt, l_h1);

        if(rx_dfe_h1_check_en_int)
        {
            mem_pl_bit_set(rx_dfe_h1_fail, l_lane);
        }

        l_rc = warning_code;
    }

    //RXBIST: applies to both A and B banks at sametime
    mem_pl_bit_set(rx_a_dfe_h1_done, l_lane);
    mem_pl_bit_set(rx_b_dfe_h1_done, l_lane);

    return l_rc;
}


/**
 * @brief Applys hysteresis and averaging to new result
 *   - If delta is <= uncertainty, average the new & prev result
 *   - If delta is >  uncertainty, take new result
 *
 * @param[in] i_new   New result
 * @param[in] i_prev  Prev result
 *
 * @return Latest Dac Result
 *
 */
static inline int32_t rx_eo_dfe_hysteresis(const int32_t i_new, const int32_t i_prev, bool i_hyst_en)
{
    const int32_t UNCERTAINTY_AVG  = 4;  // If a change is larger than this value, the result changes to the new value
    const int32_t UNCERTAINTY_HYST =
        2;  // If a change is larger than this value, the result changes to the average of the new and prev values, else it doesn't change

    int32_t result  = i_new;

    // PSL hyst_en
    if ( i_hyst_en )
    {
        int32_t delta = abs(i_new - i_prev);

        // PSL hyst_result
        result = (delta <= UNCERTAINTY_HYST) ? i_prev :    // no change
                 (delta <= UNCERTAINTY_AVG ) ? eo_dfe_round(i_new + i_prev) :  // small change
                 i_new; // large change
    }

    return result;
}

/**
 * @brief Full DFE (32 Measurements per bank)
 *
 * @param[in] i_tgt                    Reference to Bus Target
 * @param[in] i_bank                   Target bank to calibration
 * @param[in] i_run_all_quads          True to force running all 4 quadrants; otherwise follow rx_dfe_full_mode
 * @param[in] i_hyst_en                True when this is being run with hysteresis used
 * @param[in] i_enable_min_eye_height  True when the min eye height checking and logging are enabled
 *
 * @return uint32_t. pass_code if success, else error code.
 */
uint32_t rx_eo_dfe_full(t_gcr_addr* i_tgt, const t_bank i_bank, bool i_run_all_quads, bool i_hyst_en,
                        bool i_enable_min_eye_height, bool i_disable_pattern_filter)
{
    // l_bank - enumerated to index and for servo ops + safe if t_bank ever changes
    // PSL bank_a
    const uint32_t l_bank      = (i_bank == bank_a) ? BANK_A : BANK_B;
    uint16_t l_servo_ops_full[16];                     // Servo Ops Fully Qualified
    uint16_t* l_ap_servo_ops   = &l_servo_ops_full[0]; // AP Servo Ops
    uint16_t* l_an_servo_ops   = &l_servo_ops_full[8]; // AN Servo Ops
    int32_t  l_ax_results[16];                         // AP & AN Servo Op Results
    int32_t*  l_ap_results     = &l_ax_results[0];     // AP Servo Op Results
    int32_t*  l_an_results     = &l_ax_results[8];     // AN Servo Op Results
    uint32_t l_latch           = L000;                 // Latch Looper
    uint32_t l_rc              = pass_code;              // Return Code
    uint32_t l_dac_addr        = 0;
    int32_t l_dac_array[LATCH_SIZE];
    uint32_t l_lane            = get_gcr_addr_lane(i_tgt);
    int32_t dfe_full_h1_adj    = TwosCompToInt(mem_pg_field_get(rx_dfe_full_h1_adj), rx_dfe_full_h1_adj_width);


    SET_DFE_DEBUG(0x7100); // Enter Full DFE

    //SET_DFE_DEBUG(0x7101); // Initialize Settings
    rx_eo_amp_servo_setup(i_tgt, SERVO_SETUP_DFE_FULL);
    int rx_dfe_check_en_int = get_ptr(i_tgt, rx_dfe_check_en_addr  , rx_dfe_check_en_startbit  ,
                                      rx_dfe_check_en_endbit); //ppe pl
    int l_servo_min_max_error_dis = mem_pg_field_get(rx_dfe_full_max_error_disable);

    if (l_servo_min_max_error_dis)
    {
        // Disable servo status for result at min/max
        servo_errors_disable_only_result_at_min_or_max(i_tgt);
    }

    // Initialize Min Eye Height tracking before quadrant loop
    int32_t  l_min_height = 127;
    uint32_t l_min_quad   = 0;
    uint32_t l_min_latch  = 0;

    // Quadrant Loop
    // rx_dfe_full_mode: 0 = Run on all quadrants, 1 = Run on single quadrant and rotate each time
    uint32_t l_quad     = QUAD_NORTH;
    uint32_t l_quad_end = QUAD_WEST;
    uint32_t l_dfe_full_quad = mem_pl_field_get(rx_dfe_full_quad, l_lane); // x00, x01, x02, or x03
    uint32_t l_dfe_full_mode = mem_pg_field_get(rx_dfe_full_mode);

    // PSL dfe_full_mode_one_quad
    if ( l_dfe_full_mode && !i_run_all_quads )
    {
        l_quad = (l_dfe_full_quad << 3); // x00, x08, x10, or x18
        l_quad_end = l_quad; // Only run one quad
    }

    // Switch the quadrant only after bank B. Always do this regardless of i_run_all_quads
    // so that bank A still advances when bank B is running all quads because of a peak change.
    if (i_bank == bank_b)
    {
        l_dfe_full_quad = (l_dfe_full_quad + 1) & 0x3; // Rotate to next quadrant on the next call to dfe_full
        mem_pl_field_put(rx_dfe_full_quad, l_lane, l_dfe_full_quad); // Store the next quad value for this lane
    }

    io_sleep(get_gcr_addr_thread(i_tgt)); //EWM299509

    // PCIe Gen1 and Gen2 do not match on the H3-H1 pattern since may be sparse with 8b/10b. All other rates pattern match on H3-H1.
    uint32_t l_servo_op_mask = i_disable_pattern_filter ? SERVO_OP_MASK_H321_X : SERVO_OP_MASK_ALL;

    int l_dac_bank_addr = DAC_BASE_ADDR + (l_bank << 5);

    for (; l_quad <= l_quad_end; l_quad += 8)
    {
        //SET_DFE_DEBUG(0x7102, l_quad); // Quadrant Loop
        l_dac_addr = l_dac_bank_addr + l_quad;

        // Customize Servo Ops for Specific Quadrant and Bank
        for (l_latch = L000; l_latch <= L111; ++l_latch)
        {
            l_ap_servo_ops[l_latch] = SERVO_OPS[l_latch] | l_servo_op_mask | SERVO_OP_AP | ( l_bank << 6) | l_quad | l_latch;
            l_an_servo_ops[l_latch] = SERVO_OPS[l_latch] | l_servo_op_mask | SERVO_OP_AN | ( l_bank << 6) | l_quad | l_latch;

            l_dac_array[l_latch] = LatchDacToInt(get_ptr(i_tgt, (l_dac_addr + l_latch), DAC_STARTBIT, DAC_ENDBIT));
        }

        SET_DFE_DEBUG(0x7103); // Run Servo Ops

        l_rc = run_servo_ops_and_get_results(
                   i_tgt,
                   c_servo_queue_amp_meas,
                   16,
                   l_servo_ops_full,
                   l_ax_results);

        // PSL servo_error
        if (l_rc)
        {
            goto function_exit;
        }

        // Check for recal abort
        l_rc = check_rx_abort(i_tgt);

        // PSL recal_abort
        if (l_rc)
        {
            SET_DFE_DEBUG(0x710F); // DFE Recal Abort
            goto function_exit;
        }

        // Calculate Result
        SET_DFE_DEBUG(0x7109); // DFE Full Begin Latch Loop

        for (l_latch = L000; l_latch <= L111; ++l_latch)
        {
            //SET_DFE_DEBUG(0x7104, l_latch); // Latch Loop

            // Thread Sleep and quick exit on abort: See EWM 302249, 301148, 302043
            if (l_latch == 3)
            {
                l_rc = check_rx_abort(i_tgt);

                // PSL recal_abort_calc
                if (l_rc)
                {
                    SET_DFE_DEBUG(0x710E); // DFE Recal Abort 2
                    goto function_exit;
                }

                io_sleep(get_gcr_addr_thread(i_tgt));
            }

            // Calculate Results :: DAC Value = (AP???10 + AN???01) / 2
            //SET_DFE_DEBUG(0x7105, l_ap_results[l_latch]); // Calcualte DAC Value
            //SET_DFE_DEBUG(0x7105, l_an_results[l_latch]); // Calcualte DAC Value
            int32_t l_new_val = eo_dfe_round(l_ap_results[l_latch] + l_an_results[l_latch]);

            // Calculate the eye height for this latch and save the relevant info if it is the new minimum eye height for this DFE run
            int32_t l_height  = (l_ap_results[l_latch] - l_an_results[l_latch]) / 2; // truncated (round-down)

            // PSL height_lt_min_height
            if (l_height < l_min_height)
            {
                l_min_height = l_height;
                l_min_quad   = l_quad;
                l_min_latch  = l_latch;
            }

            // HW493492: This dfe full method tends to set DFE H1 a couple steps low.  So for LXX1 latches, add DFE_FULL_H1_ADJ,
            //           and for LXX0 latches, subtract DFE_FULL_H1_ADJ
            l_new_val = (l_latch & LXX1_MASK     ) ? (l_new_val + dfe_full_h1_adj) : (l_new_val - dfe_full_h1_adj);

            //SET_DFE_DEBUG(0x7105, l_new_val); // Calcualte DAC Value

            // Get the previous value of the hw dac
            int32_t l_prev_val = l_dac_array[l_latch];

            // Apply a hysteresis to account for uncertainty of servo ops
            //SET_DFE_DEBUG(0x7106, l_prev_val); // Apply Hysteresis
            l_new_val = rx_eo_dfe_hysteresis(l_new_val, l_prev_val, i_hyst_en);
            //SET_DFE_DEBUG(0x7106, l_new_val); // Apply Hysteresis

            // Rxbist Check (Could double as a runtime error check)
            if(rx_dfe_check_en_int)
            {
                l_rc = rx_eo_dfe_check_dac_limits(i_tgt, l_new_val);

                // PSL rx_bist_dac_limit_fail
                if (l_rc)
                {
                    //SET_DFE_DEBUG(0x710F); // DFE DAC Limit Fail
                    goto function_exit;
                }
            } //rx_dfe_check_en

            // PSL new_val_ne_prev_val
            if (l_new_val != l_prev_val)
            {
                // Convert to dac format
                int32_t l_dac_val = IntToLatchDac(l_new_val);
                //SET_DFE_DEBUG(0x7107, l_dac_val); // Update Latch Dac
                put_ptr_fast(i_tgt, (l_dac_addr + l_latch), DAC_ENDBIT, l_dac_val);
            }
            else
            {
                //SET_DFE_DEBUG(0x7108, 0x0); // No Update Latch Dac
            }

        }//end calculate result - for(latch)

    } // for(quad)

    // Check and log the min eye height as needed
    int l_clr_eye_height_width  = mem_pg_field_get(rx_clr_eye_height_width);

    // PSL enable_min_eye_height
    if (i_enable_min_eye_height && !l_clr_eye_height_width)
    {
        SET_DFE_DEBUG(0x710B); // Min Eye Height Checks

        // Check if new min eye height for this lane (valid not set or is < old_min_height)
        // PSL new_lane_hist_min_eye_height
        if ( !mem_pl_field_get(rx_lane_hist_min_eye_height_valid, l_lane)
             || (l_min_height < mem_pl_field_get(rx_lane_hist_min_eye_height, l_lane)) )
        {
            mem_pl_field_put(rx_lane_hist_min_eye_height,       l_lane, l_min_height);
            mem_pl_field_put(rx_lane_hist_min_eye_height_bank,  l_lane, l_bank);  //BANK_A=0, BANK_B=1
            mem_pl_field_put(rx_lane_hist_min_eye_height_latch, l_lane, l_min_latch);
            mem_pl_field_put(rx_lane_hist_min_eye_height_quad,  l_lane, (l_min_quad >> 3));
            mem_pl_bit_set(rx_lane_hist_min_eye_height_valid,   l_lane);
        }

        // Check if new min eye height for group (valid not set or is < old_min_height)
        // PSL new_group_hist_min_eye_height
        if ( !mem_pg_field_get(rx_hist_min_eye_height_valid) || (l_min_height < mem_pg_field_get(rx_hist_min_eye_height)) )
        {
            mem_pg_field_put(rx_hist_min_eye_height, l_min_height);
            mem_pg_field_put(rx_hist_min_eye_height_lane, l_lane);
            mem_pg_bit_set(rx_hist_min_eye_height_valid);
        }

        // Bad Lane and DFT checks
        if (l_min_height < mem_pg_field_get(rx_eye_height_min_check))
        {
            mem_pl_bit_set(rx_bad_eye_opt_height, l_lane);
            uint32_t l_fir_code = rx_dfe_check_en_int ? (fir_code_dft_error | fir_code_bad_lane_warning) :
                                  fir_code_bad_lane_warning;
            // PSL set_fir_bad_lane_warning_and_dft_error
            set_fir(l_fir_code);
            ADD_LOG(DEBUG_RX_EYE_HEIGHT_FAIL, i_tgt, l_min_height);
            l_rc = warning_code;
        }
    } //if(i_enable_min_eye_height && !l_clr_eye_height_width)

    // Valid is cleared in ioo_thread, but could clear it here too
    //if (l_clr_eye_height_width) {
    //  mem_pl_bit_clr(rx_lane_hist_min_eye_height_valid,   l_lane);
    //  mem_pg_bit_clr(rx_hist_min_eye_height_valid);
    //}

function_exit:

    if (l_servo_min_max_error_dis)
    {
        // Re-enable servo status for result at min/max
        servo_errors_enable_all(i_tgt);
    }

    if (i_bank == bank_a)
    {
        mem_pl_bit_set(rx_a_dfe_done, l_lane);
    }
    else
    {
        mem_pl_bit_set(rx_b_dfe_done, l_lane);
    }

    if (l_rc & rc_warning )
    {
        mem_pl_field_put(rx_dfe_fail, l_lane, 0b1);    //ppe pl
    }

    SET_DFE_DEBUG(0x710A); // Exit Full DFE
    return l_rc;
}



/**
 * @brief DFE Fast. Performs 8 servo ops, calculates the DFE Method 2
 *   coefficients, and then will apply the result to both Bank A & B.
 *
 * @param[in] i_gcr_addr   References the unit target
 * @param[in] i_cal_mode   Used to determine GenX and whether to run H1 or H1-H3
 *
 * @return uint32_t. pass_code if success, else error code.
 *
 */
uint32_t rx_eo_dfe_fast(t_gcr_addr* i_tgt, t_init_cal_mode i_cal_mode)
{
    const uint32_t ITERATIONS              = 2;       // Fixed number of iterations instead of converge
    uint32_t       l_iter                  = 0;       // Current Iteration Number
    uint32_t       l_rc                    = pass_code; // Return Code
    int32_t        l_coefs[4]              = {0, 0, 0, 0};
    int32_t        l_coef_7_0_vals[2]      = {0, 0};
    int32_t        l_coef_1_6_vals[2]      = {0, 0};
    int32_t        l_coef_5_2_vals[2]      = {0, 0};
    int32_t        l_coef_3_4_vals[2]      = {0, 0};
    int32_t        l_clk_adj_a[2]          = {0, 0};
    int32_t        l_clk_adj_b[2]          = {0, 0};

    SET_DFE_DEBUG(0x7000); // Enter DFE Fast

    // Run H1 in Gen 3/4 and H1=H3 in Gen 5
    bool l_h1_only = (i_cal_mode == C_PCIE_GEN3_CAL) || (i_cal_mode == C_PCIE_GEN4_CAL);

    // Read current DFE coefficients
    int32_t l_lane = get_gcr_addr_lane(i_tgt);
    l_coef_7_0_vals[1] = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_7_0, l_lane),
                                       rx_dfe_coef_7_0_width); // [1] is previous coefficient value calculation
    l_coef_1_6_vals[1] = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_1_6, l_lane),
                                       rx_dfe_coef_1_6_width); // [1] is previous coefficient value calculation
    l_coef_5_2_vals[1] = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_5_2, l_lane),
                                       rx_dfe_coef_5_2_width); // [1] is previous coefficient value calculation
    l_coef_3_4_vals[1] = TwosCompToInt(mem_pl_field_get(rx_dfe_coef_3_4, l_lane),
                                       rx_dfe_coef_3_4_width); // [1] is previous coefficient value calculation

    // Set Servo Settings
    rx_eo_amp_servo_setup(i_tgt, SERVO_SETUP_DFE_FAST);

    // Read current clock adjust for A and B (account for QPA by taking average of NS and EW)
    l_clk_adj_a[0] = 16 - ((get_ptr_field(i_tgt, rx_a_pr_ns_data) + get_ptr_field(i_tgt, rx_a_pr_ew_data)) / 2);
    l_clk_adj_b[0] = 16 - ((get_ptr_field(i_tgt, rx_b_pr_ns_data) + get_ptr_field(i_tgt, rx_b_pr_ew_data)) / 2);

    // Read LOFF of latch that servos are run on
    int32_t l_loff;

    // PSL saved_loff_valid
    if (mem_pl_field_get(rx_loff_ad_n000_valid, l_lane))
    {
        // Use the saved latch offset when it is valid
        l_loff = TwosCompToInt(mem_pl_field_get(rx_loff_ad_n000, l_lane), rx_loff_ad_n000_width);
    }
    else     //!rx_loff_ad_n000_valid
    {
        // Save the latch offset for future use since will be overwriting the latch DAC
        uint32_t latch_addr = get_latch_dac_addr(rx_ad_latch_dac_n000); // Issue 296947 Workaround
        l_loff = LatchDacToInt(get_ptr(i_tgt, latch_addr, rx_ad_latch_dac_n000_startbit, rx_ad_latch_dac_n000_endbit));
        mem_pl_field_put(rx_loff_ad_n000, l_lane, l_loff);
        mem_pl_bit_set(rx_loff_ad_n000_valid, l_lane);
    }

    // Loop Servo ops and ClkAdj
    for (l_iter = 0; l_iter < ITERATIONS; ++l_iter)
    {
        // Run Servo Ops and calculate ClkAdj and Coefficients
        l_clk_adj_a[1] = l_clk_adj_a[0];
        l_rc = rx_eo_dfe_fast_servo(i_tgt, l_loff, l_coefs, &l_clk_adj_a[0], l_h1_only);

        // PSL servo_error
        if (l_rc)
        {
            goto function_exit;
        }

        // Set DFE Clock Adjust Bank A
        rx_eo_dfe_set_clock_adj(i_tgt, l_clk_adj_a[1], l_clk_adj_a[0], bank_a);

        // Save new coefficients
        l_coef_7_0_vals[0] = l_coefs[0]; // [0] is current coef_7_0 value calculation
        l_coef_1_6_vals[0] = l_coefs[1]; // [1] is current coef_1_6 value calculation
        l_coef_5_2_vals[0] = l_coefs[2]; // [2] is current coef_5_2 value calculation
        l_coef_3_4_vals[0] = l_coefs[3]; // [3] is current coef_3_4 value calculation
        mem_pl_field_put(rx_dfe_coef_7_0, l_lane, l_coef_7_0_vals[0]);
        mem_pl_field_put(rx_dfe_coef_1_6, l_lane, l_coef_1_6_vals[0]);
        mem_pl_field_put(rx_dfe_coef_5_2, l_lane, l_coef_5_2_vals[0]);
        mem_pl_field_put(rx_dfe_coef_3_4, l_lane, l_coef_3_4_vals[0]);
    } //for l_iter

    // Only check and apply Coef adjustment after last iteration
    l_rc = rx_eo_dfe_check_hvals(i_tgt);

    // PSL check_hvals_error
    if (l_rc)
    {
        goto function_exit;
    }

    // Calculate adjustment (new - original)
    int l_coef_7_0_adj_int = l_coef_7_0_vals[0] - l_coef_7_0_vals[1];
    int l_coef_1_6_adj_int = l_coef_1_6_vals[0] - l_coef_1_6_vals[1];
    int l_coef_5_2_adj_int = l_coef_5_2_vals[0] - l_coef_5_2_vals[1];
    int l_coef_3_4_adj_int = l_coef_3_4_vals[0] - l_coef_3_4_vals[1];

    // Mask int32 to correct field width and shift into correct bit positions.
    int l_coef_7_0_coef_1_6_adj_regval = ( IntToTwosComp(l_coef_7_0_adj_int,
                                           rx_coef_7_0_adj_width) << rx_coef_7_0_adj_shift ) |
                                         ( IntToTwosComp(l_coef_1_6_adj_int, rx_coef_1_6_adj_width) << rx_coef_1_6_adj_shift );
    int l_coef_5_2_coef_3_4_adj_regval = ( IntToTwosComp(l_coef_5_2_adj_int,
                                           rx_coef_5_2_adj_width) << rx_coef_5_2_adj_shift ) |
                                         ( IntToTwosComp(l_coef_3_4_adj_int, rx_coef_3_4_adj_width) << rx_coef_3_4_adj_shift );

    // OK to fast_write (no other fields in the adjust registers)
    put_ptr_field(i_tgt, rx_coef_7_0_coef_1_6_adj_full_reg, l_coef_7_0_coef_1_6_adj_regval, fast_write);
    put_ptr_field(i_tgt, rx_coef_5_2_coef_3_4_adj_full_reg, l_coef_5_2_coef_3_4_adj_regval, fast_write);

    // Clear any existing DAC accelerator errors (shouldn't be necessary, but just in case)
    //put_ptr_field(i_tgt, rx_dac_accel_rollover_sticky_clr, 0b1, fast_write); //strobe bit

    // Apply DFE coefficients to Bank A and wait for accelerator to finish
    // Control register contains only rx_apply_*_run/done. OK to overwrite other bits to 0.
    int l_apply_done;
    put_ptr_field(i_tgt, rx_apply_dfe_v2_a_run_done_alias, 0b10, fast_write); // bits[0:1]: a_run, a_done

    do
    {
        l_apply_done = get_ptr_field(i_tgt, rx_apply_dfe_v2_a_done);
    }
    while (!l_apply_done);

    // Put bank B into CDR External mode temporarily while we write its DAC registers (HW525009)
    // Disable external mode on bank A, and enable bank B external mode (after bank A DACs writes complete).
    put_ptr_field(i_tgt, rx_pr_edge_track_cntl_ab_alias, cdr_a_lcl_cdr_b_ext, fast_write);

    // Apply DFE coefficients to Bank B but don't wait for accelerator to finish as yet
    // Control register contains only rx_apply_*_run/done. OK to overwrite other bits to 0.
    put_ptr_field(i_tgt, rx_apply_dfe_v2_b_run_done_alias, 0b10, fast_write); // bits[0:1]: b_run, b_done
    //do { l_apply_done = get_ptr_field(i_tgt, rx_apply_dfe_v2_b_done); } while (!l_apply_done);

    // Set DFE Clock Adjust Bank B - the Mini PR Stepper will hold off until after the DFE coefficients have been applied
    l_clk_adj_b[1] = l_clk_adj_b[0]; //original clkadj
    l_clk_adj_b[0] = l_clk_adj_a[0]; //new clkadj (same as bank A)
    rx_eo_dfe_set_clock_adj(i_tgt, l_clk_adj_b[1], l_clk_adj_b[0], bank_b);

    // Wait for the DAC Accelerator to finish. The Mini PR Stepper will run and complete soon after
    do
    {
        l_apply_done = get_ptr_field(i_tgt, rx_apply_dfe_v2_b_done);
    }
    while (!l_apply_done);

    // Restore CDR External mode on bank A (HW525009)
    // Disable external mode on bank B, and enable bank A external mode (after bank B DACs writes complete).
    put_ptr_field(i_tgt, rx_pr_edge_track_cntl_ab_alias, cdr_a_ext_cdr_b_lcl, fast_write);

    // Check for DAC Accelerator errors (on either bank).
    /*if (get_ptr_field(i_tgt, rx_dac_accel_rollover_sticky)) {
      // Clear the error, set a fir, and return warning
      set_debug_state(0x700E); // DFE Fast DAC Accelerator overflow
      put_ptr_field(i_tgt, rx_dac_accel_rollover_sticky_clr, 0b1, fast_write); //strobe bit
      set_fir(fir_code_bad_lane_warning);
      ADD_LOG(DEBUG_RX_DFE_DAC_LIMIT, i_tgt, 0x0);
      l_rc |= warning_code;
    }*/

function_exit:

    // PSL error_restore_dfe_coeff
    if (l_rc)
    {
        // On an error, restore the DFE coefficients since the new ones were not applied. Leave ClkAdj where it is.
        mem_pl_field_put(rx_dfe_coef_7_0, l_lane, l_coef_7_0_vals[1]);
        mem_pl_field_put(rx_dfe_coef_1_6, l_lane, l_coef_1_6_vals[1]);
        mem_pl_field_put(rx_dfe_coef_5_2, l_lane, l_coef_5_2_vals[1]);
        mem_pl_field_put(rx_dfe_coef_3_4, l_lane, l_coef_3_4_vals[1]);
    }

    SET_DFE_DEBUG(0x700A); // Exit DFE Fast
    return l_rc;
}
