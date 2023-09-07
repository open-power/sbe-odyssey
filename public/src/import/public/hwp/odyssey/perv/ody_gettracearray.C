/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_gettracearray.C $ */
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
/// @file  ody_gettracearray.C
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
#include "ody_gettracearray.H"

static const ta_def ta_defs[] =
{
    // bus ids                                                    cplt  mult  pri_setting
    /* PERV */
    { { ODY_TB_PIB},                                             0x01, 0x0000, 5 },
    { { ODY_TB_SBE  , ODY_TB_PIB_ALT },                          0x01, 0x0001, 5 },
    /* CORE */
    { { ODY_TB_MC0},                                             0x08, 0x0000, 5 },
    { { ODY_TB_MC1},                                             0x08, 0x0001, 5 }
};


static const ta_const ta_consts =
{

    TA_DEBUG_BASE_SCOM,
    TA_TRACE_BASE_SCOM,
    TA_EQ_DEBUG_BASE_SCOM,
    TRACEARRAY_NUM_ROWS,

};

const uint16_t PROC_TRACEARRAY_MAX_SIZE  =
    (TRACEARRAY_NUM_ROWS* TRACEARRAY_BITS_PER_ROW) / 8;

//------------------------------------------------------------------------------
// HWP entry point
//------------------------------------------------------------------------------
extern "C" ReturnCode ody_gettracearray(
    const Target<TARGET_TYPE_OCMB_CHIP>& i_target,
    const poz_gettracearray_args& i_args,
    hwp_data_ostream& o_stream)
{
    FAPI_TRY(poz_gettracearray(i_target, i_args, ta_defs, ta_consts, ARRAY_SIZE(ta_defs), o_stream));

    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    return current_err;

}
