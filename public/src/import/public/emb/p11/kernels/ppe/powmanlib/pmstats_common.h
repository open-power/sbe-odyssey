/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/pmstats_common.h $ */
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
#ifndef __PMSTATS_COMMON_H__
#define __PMSTATS_COMMON_H__

#include <iota.h>
#include <iota_trace.h>
#include "pmtiming_common.h"


//******************************************************************************
// What this API does
// ------------------
// - Calculates the duration between two TBR time samples, saves it away in a
//   circular buffer, managed by the API, and finds the avg/min/max of the 16
//   most recent, ie "running", time samples.
// - The updated avg/min/max of the running samples are put into a measurement
//   statistics data structure, MeasStats_t, supplied by the caller.
// - For every 16 time samples two other sets of avg/min/max are updated in the
//   data structure:
//   1) the previous set of running values and
//   2) the lifetime set of all other avg/min/max values that ever were captured
//      in all other previous sets of previous data (see Characteristics below).
// - It will gradually ramp up the running sample count during the gathering of
//   the very first 16 samples. After this it will enter the steady state phase.
//
// How to use this API
// -------------------
// - Use meas_stats_init() to initialize your local MeasStats_t data block.
// - Use meas_stats_update() to update your local MeasStats_t data block.
// - Do *not* modify any of the fields of MeasStats_t data in your local code.
//
// Characteristics
// ---------------
// - The three data sets captured in the MeasStats_t structure represent
//   statistical information of the following time zones (given in TBR ticks
//   of increasing age):
//   - Running set  :  1-16
//   - Previous set :  2-17(*) -> 17-32(**)
//   - Lifetime set : 18-oo(*) -> 33-oo(**)
// - The previous data set ("prev") in the MeasStats_t struct is a snapshot of
//   the running data set ("run") and is updated every 16 samples.
// - Also on every 16 samples, the lifetime data set ("life") is updated with
//   the content of the previous data set. However, importantly, it's updated
//   **before** the previous data set is being updated with the snapshot of
//   the running data.
//
// Assumptions of this API
// -----------------------
// - Time stamps are in TBR ticks.
// - Durations are captured in 32-bit variables.
//   - Assuming a 32 ns TBR tick and a running sample size of 16, the maximum
//     span between two time stamps supplied to the API should not exceed 8 sec.
// - Only this API can update the content of the MeasStats_t data block.
//
//******************************************************************************


#define DURATION_MAX           (uint32_t)0xffffffff
#define RUNNING_SAMPLES_SIZE   (uint16_t)16

//
// Structure for collecting measurement duration statistics
// Notes:
// - Three types of duration stats are collected:
//   - "run": The most recent [running] stats are updated while accumulating the most
//     recent [running] duration measurements.
//   - "prev": The previous stats are replaced with a snapshot of the most recent
//     [running] stats every time the sample_count_life equals a multiple of the
//     RUNNING_SAMPLE_SIZE.
//   - "life": The lifetime stats are updated every time we replace the duration_prev
//     values. Note that the lifetime stats are a reflection of the past and does
//     *NOT* take into consideration the running values (until they show up in the
//     previous values). This is so that, if something goes wrong, that the possibly
//     increased duration_run max and avg values can be distinguished from the
//     possibly more normal operation depicted in the duration_life values.
//
typedef struct
{
    uint32_t duration_run_latest; // Most recent sample
    uint32_t duration_run_buf[RUNNING_SAMPLES_SIZE]; // FIFO circular buffer
    uint32_t run_buf_pos;         // Position of oldest value in FIFO (to be over written)
    uint32_t duration_run_acc;    // Accummulated (summed) value of most recent values in FIFO
    uint32_t duration_run_avg;    // Avg value of most recent values in FIFO
    uint32_t duration_run_min;    // Min value of most recent values in FIFO
    uint32_t duration_run_max;    // Max value of most recent values in FIFO
    uint32_t duration_prev_avg;   // Avg value of prev RUNNING_SAMPLES_SIZE set of recent values
    uint32_t duration_prev_min;   // Min value of prev RUNNING_SAMPLES_SIZE set of recent values
    uint32_t duration_prev_max;   // Max value of prev RUNNING_SAMPLES_SIZE set of recent values
    uint32_t duration_life_min;   // Min lifetime val of all prev sets (excl current run vals)
    uint32_t duration_life_max;   // Max lifetime val of all prev sets (excl current run vals)
    uint32_t sample_count_life;   // Number of lifetime samples
} MeasStats_t;


/**
 * @function   meas_stats_init()
 * @brief      Initializes measurement duration statistics struct settings
 * @param[out] o_stats //Measurement statistics struct
 * @return     None
 */
static inline void meas_stats_init(MeasStats_t* o_stats)
{
    uint8_t i;

    for (i = 0; i < RUNNING_SAMPLES_SIZE; i++)
    {
        o_stats->duration_run_buf[i] = 0;
    }

    o_stats->run_buf_pos           = 0;
    o_stats->duration_run_acc      = 0;
    o_stats->duration_run_min      = DURATION_MAX;
    o_stats->duration_run_max      = 0;
    o_stats->duration_prev_min     = DURATION_MAX;
    o_stats->duration_prev_max     = 0;
    o_stats->duration_life_min     = DURATION_MAX;
    o_stats->duration_life_max     = 0;
    o_stats->sample_count_life     = 0;
}


/**
 * @function   meas_stats_update
 * @brief      Updates measuremet duration statistics
 * @param[in]  i_tbr_begin  //Begining TBR value (at begining of measurements)
 * @param[in]  i_tbr_end    //Ending TBR value (at end of measurements)
 * @param[in/out] io_stats  //Measurement duration statistics struct
 * @return     None
 */
static inline void meas_stats_update( uint32_t i_tbr_begin, uint32_t i_tbr_end, MeasStats_t* io_stats)
{
    // Update sample count
    io_stats->sample_count_life++;

    // Copy buffer position to local var, pos (for easier referencing later)
    uint32_t pos = io_stats->run_buf_pos;

    // Calculate duration and save to local var, duration (for easier referencing later)
    uint32_t duration = calc_tbr_diff( i_tbr_begin, i_tbr_end);

    // Update the accumulated duration using the outgoing and incoming duration
    // values.  Then overwrite, in the FIFO buffer, the outgoing duration value
    // with the incoming value and also make a note of the latest duration
    // value in the struct.
    io_stats->duration_run_acc += duration;
    io_stats->duration_run_acc -= io_stats->duration_run_buf[pos];

    io_stats->duration_run_buf[pos] = duration;

    io_stats->duration_run_latest = duration;

    //
    // Update duration_prev and duration_life values depending on the stats phase
    // and adjust the sample_count accordingly which is to be used in the subsequent
    // calculattion of the duration_running values.  The statistics phases are:
    // - Ramp-up phase : Represents the initial start-up phase at initialization time.
    // - Steady state phase : The post ramp-up phase.
    //
    uint16_t sample_count = 0; //Phase specific number of samples (to accomodate ramp-up)

    if (io_stats->sample_count_life < RUNNING_SAMPLES_SIZE)
    {
        /*
         *  Ramp-up phase
         */

        // Ramp up the sample count (until we hit RUNNING_SAMPLES_SIZE)
        sample_count = io_stats->sample_count_life;
    }
    else
    {
        /*
         *  Steady-state phase
         */

        // Whenever sample_count_life hits a multiple of RUNNING_SAMPLE_SIZE update
        // the stats values in the following order:
        // 1. Update the duration_life values with duration_prev values.
        // 2. Take a snapshot of the duration_run values and save them in the
        //    duration_prev values.
        if ((io_stats->sample_count_life % RUNNING_SAMPLES_SIZE) == 0)
        {
            // Update duration_life values with content from duration_prev values
            if (io_stats->duration_prev_min < io_stats->duration_life_min)
            {
                io_stats->duration_life_min = io_stats->duration_prev_min;
            }

            if (io_stats->duration_prev_max > io_stats->duration_life_max)
            {
                io_stats->duration_life_max = io_stats->duration_prev_max;
            }

            // Update duration_prev value with snapshot of duration_run values
            io_stats->duration_prev_avg = io_stats->duration_run_avg;
            io_stats->duration_prev_min = io_stats->duration_run_min;
            io_stats->duration_prev_max = io_stats->duration_run_max;

            // Reset duration_run min and max values
            io_stats->duration_run_min = DURATION_MAX;
            io_stats->duration_run_max = 0;
        }

        // The sample count is fixed in steady state
        sample_count = RUNNING_SAMPLES_SIZE;
    }

    //
    // Update duration_run_{avg,min,max} values
    //

    // Avg

    if (sample_count > 0)
    {
        io_stats->duration_run_avg = (io_stats->duration_run_acc + sample_count / 2) / sample_count;
    }
    else
    {
        //CMO-TODO: Trace out and create error log.
        PK_TRACE_ERR("CODE BUG: sample_count=0 is illegal");
        //APPCFG_PANIC(<code>);
    }

    // Min

    if (io_stats->duration_run_buf[pos] < io_stats->duration_run_min)
    {
        io_stats->duration_run_min = io_stats->duration_run_buf[pos];
    }

    // Max

    if (io_stats->duration_run_buf[pos] > io_stats->duration_run_max)
    {
        io_stats->duration_run_max = io_stats->duration_run_buf[pos];
    }


    //
    // Point to where next duration measurement goes in circular buffer
    //
    pos++;

    if (pos == RUNNING_SAMPLES_SIZE)
    {
        //End of buffer case. Wrap run_pos to beginning.
        io_stats->run_buf_pos = 0;
    }
    else
    {
        io_stats->run_buf_pos = pos;
    }
}

#endif  /* __PMSTATS_COMMON_H__ */
