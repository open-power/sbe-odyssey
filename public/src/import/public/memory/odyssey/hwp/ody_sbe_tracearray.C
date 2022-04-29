/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/ody_sbe_tracearray.C $ */
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
/// @file  ody_sbe_tracearray.C
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
#include "ody_sbe_tracearray.H"

//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------

const uint32_t DEBUG_TRACE_CONTROL = 0x080107d0;
const uint32_t DEBUG_TRACE_CONTROL_START = 0;
const uint32_t DEBUG_TRACE_CONTROL_STOP  = 1;
const uint32_t DEBUG_TRACE_CONTROL_RESET = 2;

const uint32_t TRACE0_SCOM_BASE = 0x08010400;
const uint32_t TRACE1_SCOM_BASE = 0x0801040a;

const uint32_t TRACE_HI_DATA_OFS         = 0x0;
const uint32_t TRACE_LO_DATA_OFS         = 0x1;
const uint32_t TRACE_TRCTRL_CONFIG_OFS   = 0x2;
const uint32_t TRCTRL_HOLD_OFF           = 18;
const uint32_t TRCTRL_RUN_STATUS         = 19;
const uint32_t TRCTRL_CLOCK_ENABLE       = 22;

//------------------------------------------------------------------------------
// HWP entry point
//------------------------------------------------------------------------------
extern "C" fapi2::ReturnCode ody_sbe_tracearray(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const ody_gettracearray_args& i_args,
    uint64_t* o_ta_data,
    const uint16_t i_first_row,
    const uint16_t i_num_rows)
{
    fapi2::ReturnCode l_fapiRc = fapi2::FAPI2_RC_SUCCESS;
    const uint32_t TRACE_SCOM_BASE = (i_args.trace_array == ODY_TA_TRACE0) ? TRACE0_SCOM_BASE : TRACE1_SCOM_BASE;
    const bool first_granule = (0 == i_first_row);
    const bool last_granule  = ((i_first_row + i_num_rows) >= ODY_TRACEARRAY_NUM_ROWS);

    // mark HWP entry
    FAPI_INF("Entering ...");

    FAPI_ASSERT(i_args.trace_array < _ODY_TA_COUNT,
                fapi2::ODY_GETTRACEARRAY_INVALID_ARRAY()
                .set_TARGET(i_target).set_TRACE_ARRAY(i_args.trace_array),
                "Invalid trace array ID specified: 0x%X", i_args.trace_array);

    if (i_args.stop_pre_dump && first_granule)
    {
        FAPI_DBG("Stopping trace arrays");
        fapi2::buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_STOP>();
        FAPI_TRY(fapi2::putScom(i_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to stop chiplet domain trace arrays");
    }

    if (i_args.collect_dump && first_granule)
    {
        fapi2::buffer<uint64_t> l_trace_ctrl;
        FAPI_TRY(fapi2::getScom(i_target, TRACE_SCOM_BASE + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
                 "Failed to read current trace control setting");

        FAPI_ASSERT(l_trace_ctrl.getBit<TRCTRL_CLOCK_ENABLE>(),
                    fapi2::ODY_GETTRACEARRAY_CLOCKS_OFF()
                    .set_TARGET(i_target).set_TRACE_ARRAY(i_args.trace_array)
                    .set_TRCTRL_REG(l_trace_ctrl),
                    "The trace array's clocks are not enabled; you likely didn't capture any data."
                    "Make sure bit 22 in the trace control register is set while tracing and dumping.");

        FAPI_ASSERT(!l_trace_ctrl.getBit<TRCTRL_RUN_STATUS>(),
                    fapi2::ODY_GETTRACEARRAY_TRACE_RUNNING()
                    .set_TARGET(i_target).set_TRACE_ARRAY(i_args.trace_array)
                    .set_TRCTRL_REG(l_trace_ctrl),
                    "Trace array is still running -- If you think you stopped it, "
                    "maybe the controlling debug macro is slaved to another debug macro?");

        l_trace_ctrl.setBit<TRCTRL_HOLD_OFF>();
        FAPI_TRY(fapi2::putScom(i_target, TRACE_SCOM_BASE + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
                 "Failed to enable holding trace_run off; a possible reason is that "
                 "tracing was started just before the SCOM access");
    }

    if (i_args.collect_dump)
    {
        fapi2::buffer<uint64_t> l_trace_data;

        // dump high-order data entries from specified array
        // Loop through all addresses so we end on the same array address as we started,
        // but only store the first i_num_rows ones.
        for (int i = 0; i < ODY_TRACEARRAY_NUM_ROWS; i++)
        {
            FAPI_TRY(getScom(i_target, TRACE_SCOM_BASE + TRACE_HI_DATA_OFS, l_trace_data),
                     "Error dumping high-order data entries from trace array");

            if (i < i_num_rows)
            {
                o_ta_data[2 * i] = l_trace_data;
            }
        }

        // dump low-order data entries from specified array
        for (int i = 0; i < i_num_rows; i++)
        {
            FAPI_TRY(getScom(i_target, TRACE_SCOM_BASE + TRACE_LO_DATA_OFS, l_trace_data),
                     "Error dumping low-order data entries from trace array");
            o_ta_data[2 * i + 1] = l_trace_data;
        }
    }

    if (i_args.collect_dump && last_granule)
    {
        fapi2::buffer<uint64_t> l_trace_ctrl;
        FAPI_TRY(fapi2::getScom(i_target, TRACE_SCOM_BASE + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
                 "Failed to read current trace control setting");
        l_trace_ctrl.clearBit<TRCTRL_HOLD_OFF>();
        FAPI_TRY(fapi2::putScom(i_target, TRACE_SCOM_BASE + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
                 "Failed to disable holding trace_run off");
    }

    if (i_args.reset_post_dump && last_granule)
    {
        FAPI_DBG("Resetting trace arrays");
        fapi2::buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_RESET>();
        FAPI_TRY(fapi2::putScom(i_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to reset chiplet domain trace arrays");
    }

    if (i_args.restart_post_dump && last_granule)
    {
        FAPI_DBG("Starting trace arrays");
        fapi2::buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_START>();
        FAPI_TRY(fapi2::putScom(i_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to restart chiplet domain trace arrays");
    }

    // mark HWP exit
    FAPI_INF("Success");
    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}
