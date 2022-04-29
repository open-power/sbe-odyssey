/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/exp_gettracearray.C $ */
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
/// @file  exp_gettracearray.C
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

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include "exp_gettracearray.H"
#include "exp_sbe_tracearray.H"

//------------------------------------------------------------------------------
// HWP entry point
//------------------------------------------------------------------------------
extern "C" fapi2::ReturnCode exp_gettracearray(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const exp_gettracearray_args& i_args,
    fapi2::variable_buffer& o_ta_data)
{
    o_ta_data.resize(EXP_TRACEARRAY_NUM_ROWS * EXP_TRACEARRAY_BITS_PER_ROW);
    uint64_t l_data_buffer[EXP_TRACEARRAY_NUM_ROWS * EXP_TRACEARRAY_BITS_PER_ROW / 8 / sizeof(uint64_t)];

    FAPI_TRY(exp_sbe_tracearray(i_target, i_args, l_data_buffer, 0, EXP_TRACEARRAY_NUM_ROWS));

    for(int i = 0; i < 2 * EXP_TRACEARRAY_NUM_ROWS; i++)
    {
        FAPI_TRY(o_ta_data.set<uint64_t>(l_data_buffer[i], i),
                 "Failed to insert data into trace buffer");
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;

}
