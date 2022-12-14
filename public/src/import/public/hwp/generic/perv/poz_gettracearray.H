/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_gettracearray.H $ */
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
/// @file  p10_poz_gettracearray.H
///
/// @brief Collect contents of specified trace array via SCOM.
///
/// Collects contents of specified trace array via SCOM.  Optionally
/// manages chiplet domain trace engine state (start/stop/reset) around
/// trace array data collection.  Trace array data can be collected only
/// when its controlling chiplet trace engine is stopped.
///
/// Request number of Trace array entries will be packed into data buffer from
/// oldest->youngest entry.
///
/// Calling code is expected to pass the proper target type based on the
/// desired trace resource; a convenience function is provided to find out
/// the expected target type for a given trace resource.
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Nicholas Landi <nlandi@ibm.com>
// *HWP FW Maintainer   : Raja Das <rajadas2@in.ibm.com>
// *HWP Consumed by     : Cronus, SBE
//------------------------------------------------------------------------------

#ifndef _POZ_GETTRACEARRAY_H
#define _POZ_GETTRACEARRAY_H
//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <fapi2.H>
#include "hwp_data_stream.H"


using namespace fapi2;
//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------
const uint32_t TA_BASE_SCOM_MULTIPLIER = 0x00000010;
const uint32_t TA_DEBUG_BASE_SCOM      = 0x000107C0;
const uint32_t TA_EQ_DEBUG_BASE_SCOM   = 0x000183E0;
const uint32_t TA_TRACE_BASE_SCOM      = 0x00010400;

const uint32_t TRACE_MUX_POSITIONS       = 16;

// maximum trace array entries supported by Odyssey chip design
static const uint8_t TRACEARRAY_NUM_ROWS = 128;
// bits to store per trace array entry
static const uint8_t TRACEARRAY_BITS_PER_ROW = 128;
// maximum trace array size in bytes
static const uint16_t PROC_TRACEARRAY_MAX_SIZE =
    (TRACEARRAY_NUM_ROWS* TRACEARRAY_BITS_PER_ROW) / 8;


typedef uint8_t tracearray_bus_id;

struct ta_def
{
    /* One entry per mux setting; value of 0 means N/A */
    tracearray_bus_id bus_ids[TRACE_MUX_POSITIONS];
    const uint8_t chiplet;
    const uint16_t base_multiplier;
    const uint8_t pri_setting; // setting of primary mux if secondary mux is used
};

// structure to represent HWP arguments
struct poz_gettracearray_args
{
    tracearray_bus_id trace_bus;    ///< The trace bus whose associated trace array should be dumped
    bool stop_pre_dump;             ///< Stop the trace array before starting the dump
    bool ignore_mux_setting;        ///< Do not fail if the primary trace mux is set to a different bus
    bool collect_dump;              ///< Do dump the trace array; useful if you just want to start/stop
    bool reset_post_dump;           ///< Reset the debug logic after dumping
    bool restart_post_dump;         ///< Start the trace array after dumping
};

//function pointer typedef definition for HWP call support
typedef ReturnCode (*poz_gettracearray_FP_t) (
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const poz_gettracearray_args& i_args,
    const ta_def* ta_defs,
    uint8_t ta_defs_len,
    hwp_data_ostream& o_stream
);

/* @brief Retrieve trace array data from selected trace array via SCOM.
*         Trace array data is written to a hwp_data_ostream.
 *        Optionally performing trace stop (prior to dump) and/or
 *        trace restart, reset (after dump).
 *
 * @param i_target      Chip or chiplet target.
 * @param i_args        Argument structure with additional parameters
 * @param ta_defs       Structure containing the tracebus information
 * @param ta_defs_len   Number of tracebusses hold in ta_defs
 * @param o_stream      Stream to write Trace array to.
 *
 * @return FAPI2_RC_SUCCESS
 *             if trace array dump sequence completes successfully,
 */
extern "C" ReturnCode poz_gettracearray(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const poz_gettracearray_args& i_args,
    const ta_def* ta_defs,
    uint8_t ta_defs_len,
    hwp_data_ostream& o_stream
);


#endif //_POZ_GETTRACEARRAY_H
