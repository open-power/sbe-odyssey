/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/pmtiming_common.h $ */
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
#include <stdint.h>
#include <ppe42_spr.h>


//
// CETSEL register defines for Watchdog and FIT timers
//
#define CETSEL_WDT_SEL_MASK   0xf0000000  //Mask of WDT bits
#define CETSEL_FIT_SEL_MASK   0x0f000000  //Mask of FIT bits
#define CETSEL_FIT_SEL_66US   0x0c000000  //Corresponds to a ~66us @ 2.0G or 218us @ 2.4G 
#define CETSEL_FIT_SEL_262US  0x0a000000  //Corresponds to a ~262us @ 2.0G or 218us @ 2.4G 

//
// TBR defines
//
#define TBR_VAL_MAX       (uint32_t)0xffffffff

//
// TBR time stamp structure to capture OCC beacons
//
// Notes:
// - Typically, these beacon will come from the 405 sending either doorbells to CEs on
//   the Tap chips or sending IPCs to the GPEs on the Spinal chip.
// - Initialize as follows: max_period=0 and min_period=TBR_VAL_MAX
// - The data in this struct are not valid until
//   - min_period < TBR_VAL_MAX && max_period > 0
//
typedef struct
{
    uint32_t state;         // See OCC_BEACON_STATE enum below
    uint32_t this_ts;
    uint32_t prev_ts;
    uint32_t this_period;   // This [latest] duration = (this_ts - prev_ts)
    uint32_t max_period;
    uint32_t min_period;
} OccBeacon_t;

extern OccBeacon_t g_occ_beacon;

enum OCC_BEACON_STATE
{
    BEACON_STATE_UNDEFINED = 0x1,
    BEACON_STATE_NORMAL    = 0x2,
    BEACON_STATE_INIT      = 0x3,
};

//
// Calculate the duration betwen two TBR measurements
//
// Notes:
// - Algorithm only works for durations <= TBR_VAL_MAX
//
static inline uint32_t calc_tbr_diff( uint32_t i_tbr_begin, uint32_t i_tbr_end)
{
    if (i_tbr_end > i_tbr_begin)
    {
        return (i_tbr_end - i_tbr_begin);
    }
    else
    {
        return (TBR_VAL_MAX - i_tbr_begin + 1 + i_tbr_end);
    }
}


//
// Calculate the average TBR value
//
// Notes:
// - Algorithm only works for durations <= TBR_VAL_MAX
//
static inline uint32_t calc_tbr_avg( uint32_t i_tbr_begin, uint32_t i_tbr_end)
{
    if (i_tbr_end > i_tbr_begin)
    {
        return ( (i_tbr_begin >> 1) + (i_tbr_end >> 1) );
    }
    else
    {
        // The below expression could overflow, and it's OK, as following example shows:
        //   i_tbr_begin = 0xfffffff0
        //   i_tbr_end   = 0x000000c0
        //   TBR_VAL_MAX = 0xffffffff
        // From below expression, we get:
        //   0x7ffffff8 + 0x00000060 + 0x7fffffff = 0x1_00000057
        // where the 32-bit var will ignore 0x1_00000000 and produce 0x00000057.
        return ( (i_tbr_begin >> 1) + (i_tbr_end >> 1) + (TBR_VAL_MAX >> 1) );
    }
}


//
// dec_kickoff()
//
// Description: Common DEC kickoff function
//
// Notes:       Follows the procedure outlined in "PPE42 OpenPOWER Embedded Processor Core" UM doc
//
// Params:      uint32_t i_delay; // DEC delay value supplied in [2ns] ticks
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
