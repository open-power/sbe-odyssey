/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/exp_gettracearray.H $ */
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
//------------------------------------------------------------------------------
/// @file  exp_gettracearray.H
///
/// @brief Collect contents of specified trace array via SCOM.
///
/// Collects contents of specified trace array via SCOM.  Optionally
/// manages chiplet domain trace engine state (start/stop/reset) around
/// trace array data collection.  Trace array data can be collected only
/// when its controlling chiplet trace engine is stopped.
///
/// Trace array entries will be packed into data buffer from
/// oldest->youngest entry.
//------------------------------------------------------------------------------
// *HWP HW Owner        : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP HW Backup Owner : Joe McGill <jmcgill@us.ibm.com>
// *HWP FW Owner        : ???
// *HWP Team            : Perv
// *HWP Level           : 3
// *HWP Consumed by     : FSP
//------------------------------------------------------------------------------

#ifndef _EXP_GETTRACEARRAY_H_
#define _EXP_GETTRACEARRAY_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <fapi2.H>

//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------

// maximum trace array entries supported by P9 chip design
static const uint8_t EXP_TRACEARRAY_NUM_ROWS = 128;
// bits to store per trace array entry
static const uint8_t EXP_TRACEARRAY_BITS_PER_ROW = 128;
// maximum trace array size in bytes
static const uint16_t EXP_TRACEARRAY_MAX_SIZE =
    (EXP_TRACEARRAY_NUM_ROWS* EXP_TRACEARRAY_BITS_PER_ROW) / 8;

enum exp_tracearray_id
{
    EXP_TA_TRACE0 = 0,
    EXP_TA_TRACE1,

    _EXP_TA_COUNT
};

// structure to represent HWP arguments
struct exp_gettracearray_args
{
    exp_tracearray_id trace_array;  ///< The trace array to dump
    bool stop_pre_dump;             ///< Stop the trace array before starting the dump
    bool collect_dump;              ///< Do dump the trace array; useful if you just want to start/stop
    bool reset_post_dump;           ///< Reset the debug logic after dumping
    bool restart_post_dump;         ///< Start the trace array after dumping
};

// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode
(*exp_gettracearray_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>&,
                          const exp_gettracearray_args&,
                          fapi2::variable_buffer& o_ta_data);

extern "C"
{

    /* To find a tracearray start marker, you can include <p9_proc_gettracearray.H> for
     *    static inline fapi2::ReturnCodes p9_tracearray_is_trace_start_marker(
     *        const fapi2::variable_buffer i_buffer, uint32_t i_offset = 0);
     */

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------

    /**
     * @brief Retrieve trace array data from selected trace array via SCOM,
     *        optionally performing trace stop (prior to dump) and/or
     *        trace restart (after dump)
     *
     * @param i_target  Chip or chiplet target.
     * @param i_args    Argument structure with additional parameters
     * @param o_ta_data Trace array data. Will contain all trace entries read
     *                  from the array concatenated, starting with the oldest trace
     *                  entry and ending with the newest.
     * @return FAPI2_RC_SUCCESS
     *             if trace array dump sequence completes successfully,
     *         RC_PROC_GETTRACEARRAY_TRACE_RUNNING
     *             if trace array is running when dump collection is attempted,
     *         else FAPI getscom/putscom return code for failing operation
     */
    fapi2::ReturnCode exp_gettracearray(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                        const exp_gettracearray_args& i_args,
                                        fapi2::variable_buffer& o_ta_data);

} // extern "C"

#endif // _EXP_GETTRACEARRAY_H_
