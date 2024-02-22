/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/pmtiming_common.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#ifndef __PMTIMING_COMMON_H__
#define __PMTIMING_COMMON_H__

#include <stdint.h>
#include <ppe42_spr.h>


//
// TSEL register defines for PPE Watchdog and FIT timers
//
#define TSEL_WDT_SEL_MASK   0xf0000000  //Mask of WDT bits
#define TSEL_FIT_SEL_MASK   0x0f000000  //Mask of FIT bits
#define TSEL_FIT_SEL_66US   0x0c000000  //Corresponds to a ~66us @ 2.0G or 55us @ 2.4G 
#define TSEL_FIT_SEL_262US  0x0a000000  //Corresponds to a ~262us @ 2.0G or 218us @ 2.4G 
#define TSEL_FIT_SEL_524US  0x09000000  //Corresponds to a ~524us @ 2.0G or 437us @ 2.4G 

//
// TBR defines
//
#define TBR_VAL_MAX       (uint32_t)0xffffffff
#define TBR_VAL_HALFMAX   (uint32_t)0x80000000

//
// TBR time stamp structure to capture notification times/intervals of any sort.
//
// Notes:
// - Typically, these notifications will come from the 405 sending either
//   doorbells to CEs or IPCs to the GPEs.
// - Initialize as follows: max_period=0 and min_period=TBR_VAL_MAX
// - The data in this struct are not valid until
//   - min_period < TBR_VAL_MAX && max_period > 0
//
typedef struct
{
    uint32_t state;         // See NOTIFICATION_STATE enum below
    uint32_t this_ts;
    uint32_t prev_ts;
    uint32_t this_period;   // This [latest] interval = (this_ts - prev_ts)
    uint32_t max_period;
    uint32_t min_period;
} Notification_t;

enum NOTIFICATION_STATE
{
    NOTIFICATION_STATE_UNDEFINED = 0x1,
    NOTIFICATION_STATE_NORMAL    = 0x2,
    NOTIFICATION_STATE_INIT      = 0x3,
};


//
// dec_kickoff()
//
// Description: Common DEC kickoff function
//
// Notes:       Follows the procedure outlined in "PPE42 OpenPOWER Embedded Processor Core" UM doc
//
// Params:      uint32_t i_delay; // DEC delay value supplied in PPE clock ticks
//
// Return:      None
//
//
static inline void dec_kickoff( uint32_t i_delay)
{
    uint32_t tcr_val = mfspr(SPRN_TCR);
    tcr_val &= ~TCR_DIE;
    mtspr(SPRN_TCR, tcr_val);         //Disable DEC interrupt

    mtspr(SPRN_DEC, 0xffffffff);      //Write 0xffffffff
    mtspr(SPRN_TSR, TSR_DIS);         //Clear any pending DEC interrupt
    mtspr(SPRN_DEC, i_delay);         //Write poll duration

    tcr_val = mfspr(SPRN_TCR);
    tcr_val |= TCR_DIE;
    mtspr(SPRN_TCR, tcr_val);         //Enable DEC interrupt
}


//
// Description:
// Calculate the duration betwen two TBR values w/compensation for overrun of End time
//
// Notes to user:
// - The algorithm is designed to work with 32-bit Time Base Register (TBR)
//   readings.
// - *Do not* use this function to calculate the difference of any two "ordinary"
//   uint32 variables.  Because the End time, E, is assumed to represent a time
//   later than the Begin time, B. Thus, when E overruns MAX (ie, TBR_VAL_MAX),
//   the effective value of E is actually treated as E=E+MAX+1 until B overruns
//   MAX as well.
// - The algorithm should only be used for durations E-B<=MAX
//
static inline uint32_t calc_tbr_diff( uint32_t i_begin, uint32_t i_end)
{
    if (i_end > i_begin)
    {
        return (i_end - i_begin);
    }
    else
    {
        return (TBR_VAL_MAX - i_begin + 1 + i_end);
    }
}


//
// Description:
// Calculate the average TBR value w/compensation for overrun of End time
//
// Notes to the user:
// - The algorithm is designed to work with 32-bit Time Base Register (TBR)
//   readings.
// - *Do not* use this function to calculate the average of any two "ordinary"
//   uint32 variables.  Because the End time, E, is assumed to represent a time
//   later than the Begin time, B. Thus, when E overruns MAX (ie, TBR_VAL_MAX),
//   the effective value of E is actually treated as E=E+MAX+1 until B overruns
//   MAX as well.
// - The algorithm should only be used for durations E-B<=MAX.
//
// Discussion of particular implementation:
// The algorithm below, denoted SHIFT, offers better performance than other
// algorithms wrt speed and precision. In the following we will compare it
// against the popular EB12=(E+B+1)/2 where E and B represent the End and Begin
// times as well as AXOR=(E&B)+((E^B)+1)/2. Though due to chance of overrun of
// E+B+1 and (E^B)+1 the algorithms have to be modified.  Please see the
// following Refs for examples for some background and how the algorithms can
// be implemented:
//
// [1] https://devblogs.microsoft.com/oldnewthing/20220207-00/?p=106223
// [2] https://ibm.box.com/s/zlpqsq30rjfudrpt6muy5wd1hhbrmqu4
//
// Logic table wrt Precision:
// The following logic table summarizes the precision of the returned result
// when LSb(E) != LSb(B), where LSb means the Least Significant bit. The
// precision is measured as the deviation from the true float result.
//
//    Inputs     |          Precision of Result
// LSb(E) LSb(B) | SHIFT(E>B) SHIFT(E<B)  EB12     AXOR
// ==============|======================================
//    0     0    |      0         0         0        0
//    0     1    |    +0.5      -0.5      +0.5     +0.5
//    1     0    |    -0.5      +0.5      +0.5     +0.5
//    1     1    |      0         0         0        0
// =====================================================
//
// Speed performance:
// The number of OPs, incl all conditionals, can be seen in more detail in
// [Ref2]. The following table summarizes the performances:
//
//         Number of OPs (w/SWAR proc)
// Domain |  SHIFT    EB12    AXOR
// =======|===========================
// E > B  |   5(4)    6(6)    7(6)
// E < B  |   7(5)    7(6)    8(6)
// ===================================
//
// Conclusion:
// In summary SHIFT is slightly faster than the other algorithms while also
// being more "fair", though it's unpredictable in whether it returns a result
// that has been rounded up or down. This "fairness" aspect results in that the
// long term accuracy of multiple returned results from SHIFT will exhibit a
// precision of zero while the other algorithms will exhibit a precision of
// 0.25 (considering all possible input values (E,B)).
//
static inline uint32_t calc_tbr_avg( uint32_t i_begin, uint32_t i_end)
{
    if (i_end > i_begin)
    {
        return ( ((i_begin + 1) >> 1) + (i_end >> 1) );
    }
    else if (i_end < i_begin)
    {
        // The below expression could overflow, but it's OK as following example shows:
        //   i_begin = 0xfffffff0
        //   i_end   = 0x000000c0
        //   TBR_VAL_MAX = 0xffffffff
        // From below expression, we get:
        //   0x7ffffff8 + 0x00000060 + 0x80000000 = 0x1_00000058
        // where the 32-bit var will ignore 0x1_00000000 and produce 0x00000058.
        return ( (i_begin >> 1) + ((i_end + 1) >> 1) + TBR_VAL_HALFMAX );
    }
    else // End==Begin
    {
        return ( i_end );
    }
}

#endif  /* __PMTIMING_COMMON_H__ */
