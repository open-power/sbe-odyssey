/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_gettracearray.H $ */
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
//------------------------------------------------------------------------------
/// @file  ody_gettracearray.H
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

#ifndef _ODY_GETTRACEARRAY_H_
#define _ODY_GETTRACEARRAY_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include <fapi2.H>
#include "hwp_data_stream.H"
#include "poz_gettracearray.H"
using namespace fapi2;

//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------

enum ody_tracearray_bus_id
{
    NO_TB          = 0x00,
    ODY_TB_PIB     = 0x01,
    ODY_TB_SBE     = 0x02,
    ODY_TB_PIB_ALT = 0x03,
    ODY_TB_MC0     = 0x81,
    ODY_TB_MC1     = 0x82,
};




const uint32_t TA_DEBUG_BASE_SCOM      = 0x000107C0;
const uint32_t TA_EQ_DEBUG_BASE_SCOM   = 0x000183E0;
const uint32_t TA_TRACE_BASE_SCOM      = 0x00010400;
// maximum trace array entries supported by Odyssey chip design
const uint32_t TRACEARRAY_NUM_ROWS = 128;

// function pointer typedef definition for HWP call support
typedef ReturnCode
(*ody_gettracearray_FP_t)(const Target<TARGET_TYPE_OCMB_CHIP>&,
                          const poz_gettracearray_args&,
                          hwp_data_ostream&);

extern "C"
{


//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------

    /**
     * @brief Retrieve trace array data from selected trace array via SCOM,
     *        optionally performing trace stop (prior to dump) and/or
     *        trace restart (after dump)
     *
     * @param[in] i_target  Chip or chiplet target.
     * @param[in] i_args    Argument structure with additional parameters
     * @param[out] o_stream Trace array stream. Will contain all trace entries read
     *                  from the array concatenated, starting with the oldest trace
     *                  entry and ending with the newest.
     * @return FAPI2_RC_SUCCESS
     *             if trace array dump sequence completes successfully,
     *         RC_PROC_GETTRACEARRAY_TRACE_RUNNING
     *             if trace array is running when dump collection is attempted,
     *         else FAPI getscom/putscom return code for failing operation
     */
    ReturnCode ody_gettracearray(const Target<TARGET_TYPE_OCMB_CHIP>& i_target,
                                 const poz_gettracearray_args& i_args,
                                 hwp_data_ostream& o_stream);

} // extern "C"

#endif // _ODY_GETTRACEARRAY_H_
