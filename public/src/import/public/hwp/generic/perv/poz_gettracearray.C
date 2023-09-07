/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_gettracearray.C $ */
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
/// @file  p10_poz_gettracearray.C
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

//
//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------

#include <poz_gettracearray.H>


//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------
const uint32_t DEBUG_TRACE_CONTROL_OFS = 0x10;

const uint32_t DEBUG_TRACE_CONTROL_START = 0;
const uint32_t DEBUG_TRACE_CONTROL_STOP  = 1;
const uint32_t DEBUG_TRACE_CONTROL_RESET = 2;

const uint32_t TRACE_HI_DATA_OFS       = 0;
const uint32_t TRACE_LO_DATA_OFS       = 1;
const uint32_t TRACE_TRCTRL_CONFIG_OFS = 2;

const uint32_t TRACE_LO_DATA_RUNNING     = 53;

const uint32_t TRA0_TR0_CONFIG_TRACE_RUN_STATUS = 19;
const uint32_t TRA0_TR0_CONFIG_TRACE_RUN_HOLD_OFF =   18;
const uint32_t TRA0_TR0_CONFIG_TRACE_SELECT_CONTROL = 14;
const uint32_t TRA0_TR0_CONFIG_TRACE_SELECT_CONTROL_LEN = 4;
const uint32_t TRA0_TR0_CONFIG_TRCTRL_CLOCK_ENABLE = 23;

//------------------------------------------------------------------------------
// Helper methods
//------------------------------------------------------------------------------
/*
 * @brief Determine if the secondary trace mux is used.
 *
 * @param pri_setting The primary mux setting if the secondary mux is used
 *
 * @return if pri_setting is < 5 then the secondary mux setting should be used
 *
 */
static inline bool use_sec_mux(uint8_t pri_setting)
{
    return (pri_setting < 4);
}

//------------------------------------------------------------------------------
// Table of known trace arrays
//------------------------------------------------------------------------------


class TraceArrayFinder
{
    public:
        bool valid;
        uint8_t mux_num;
        uint32_t pri_mux_sel;
        uint32_t sec_mux_sel;
        uint32_t debug_scom_base;
        uint32_t trace_scom_base;

        TraceArrayFinder(tracearray_bus_id i_trace_bus, Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, const ta_def* ta_defs,
                         const ta_const ta_consts, uint8_t ta_defs_len) :
            valid(false), mux_num(0), pri_mux_sel(0), sec_mux_sel(0), debug_scom_base(0),
            trace_scom_base(0)
        {
            FAPI_DBG("Looking up trace bus");
            const ta_def* l_ta_def;

            for(uint8_t u8cnt = 0; u8cnt < ta_defs_len; u8cnt++)
            {
                l_ta_def = (ta_defs + (u8cnt));

                for(size_t sel = 0; sel < TRACE_MUX_POSITIONS; sel++)
                {

                    if(l_ta_def->bus_ids[sel] == i_trace_bus)
                    {
                        uint32_t l_buffer = 0;
                        l_buffer |= l_ta_def->chiplet << 24;

                        trace_scom_base = l_buffer |
                                          (ta_consts.trace_base_scom
                                           + (TA_BASE_SCOM_MULTIPLIER
                                              *  l_ta_def->base_multiplier));
                        debug_scom_base = l_buffer | ta_consts.debug_base_scom;

                        FAPI_DBG("Chiplet: 0x%x; Multiplier: 0x%x", l_ta_def->chiplet, l_ta_def->base_multiplier);

                        // Determine mux position of requested bus
                        pri_mux_sel = sel;

                        FAPI_DBG("Assigning secondary mux");

                        // Traces with a pri setting <4 also use a secondary select to control one of the traces
                        // connected to the primary mux.
                        if (( use_sec_mux(l_ta_def->pri_setting)))
                        {
                            pri_mux_sel = l_ta_def->pri_setting;
                            sec_mux_sel = sel;
                        }
                        else
                        {
                            sec_mux_sel = 5;
                        }

                        valid = true;
                        FAPI_DBG("Returning found information");
                        return;
                    }
                }
            }
        }
};

//-----------------------------------------------------------------------------
// Function definitions
//-----------------------------------------------------------------------------

/**
 * @brief Perform the actual dumping of the trace array
 *
 * Make sure trace_run is held off during the dump, and check that the trace is no longer running after
 * we enabled holding it off (holding it off only makes sure it doesn't turn back on during the trace,
 * it will not force it off if it's currently running).
 * The whole Tracearray data will be written to a stream.
 *
 * The error handling makes sure that the hold_off bit is cleared even in case of an error.
 *
 * @param i_target      The target to run SCOMs against
 * @param i_args        Tracearray arguments for trace bus information
 * @param i_scom_base   Tracearray base addr
 * @param ta_consts     Structure containing the tracebus addr information
 * @param o_stream      The destination data stream
 *
 * @return RC_PROC_GETTRACEARRAY_TRACE_RUNNING if trace is still running, else FAPI2_RC_SUCCESS
 */
static ReturnCode do_dump(
    const Target < TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const poz_gettracearray_args& i_args,
    const ta_const ta_consts,
    const uint32_t i_scom_base,
    hwp_data_ostream& o_stream)
{
    buffer<uint64_t> l_trace_ctrl;
    buffer<uint64_t> buf = 0;
    uint32_t u32p_single_trace_data[4] = {};
    ReturnCode l_rc = FAPI2_RC_SUCCESS;

    FAPI_TRY(getScom(i_target, i_scom_base + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
             "Failed to read current trace control setting");

    FAPI_DBG("Trace Cntrl: 0x%x with data 0x%x", i_scom_base + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl);
    FAPI_ASSERT(l_trace_ctrl.getBit<TRA0_TR0_CONFIG_TRCTRL_CLOCK_ENABLE>(),
                GETTRACEARRAY_CLOCKS_OFF()
                .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus)
                .set_TRCTRL_REG(l_trace_ctrl),
                "The trace array's clocks are not enabled; you likely didn't capture any data."
                "Make sure bit 23 in the trace control register is set while tracing and dumping.");

    FAPI_ASSERT(!l_trace_ctrl.getBit<TRA0_TR0_CONFIG_TRACE_RUN_STATUS>(),
                GETTRACEARRAY_TRACE_RUNNING()
                .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus)
                .set_TRCTRL_REG(l_trace_ctrl),
                "Trace array is still running -- If you think you stopped it, "
                "maybe the controlling debug macro is chained to another debug macro?");

    l_trace_ctrl.setBit<TRA0_TR0_CONFIG_TRACE_RUN_HOLD_OFF>();
    FAPI_TRY(putScom(i_target, i_scom_base + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl),
             "Failed to enable holding trace_run off; a possible reason is that "
             "tracing was started just before the SCOM access");

    /* Dump the Tracearray to o_stream */
    for (uint32_t i = 0; i < ta_consts.ta_num_rows; i++)
    {
        buf = 0;
        FAPI_TRY(getScom(i_target, i_scom_base + TRACE_LO_DATA_OFS, buf),
                 "Failed to read trace array low data register,"
                 " iteration %d", i);
        u32p_single_trace_data[2] = (buf >> 32) & 0xFFFFFFFF;
        u32p_single_trace_data[3] =  buf & 0xFFFFFFFF;

        FAPI_TRY(getScom(i_target, i_scom_base + TRACE_HI_DATA_OFS, buf),
                 "Failed to read trace array high data register, "
                 "iteration %d", i);
        u32p_single_trace_data[0] = (buf >> 32) & 0xFFFFFFFF;
        u32p_single_trace_data[1] =  buf & 0xFFFFFFFF;

        for (uint8_t j = 0; j < 4; j++)
        {
            o_stream.put(u32p_single_trace_data[j]);
        }

    }

fapi_try_exit:

    l_trace_ctrl.clearBit<TRA0_TR0_CONFIG_TRACE_RUN_HOLD_OFF>();
    l_rc = putScom(i_target, i_scom_base + TRACE_TRCTRL_CONFIG_OFS, l_trace_ctrl);

    if (l_rc != FAPI2_RC_SUCCESS)
    {
        FAPI_ERR("Failed to stop holding trace_run off");

        if (current_err == FAPI2_RC_SUCCESS)
        {
            current_err = l_rc;
        }
    }

    return current_err;
}


extern "C" ReturnCode poz_gettracearray(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const poz_gettracearray_args& i_args,
    const ta_def* ta_defs,
    const ta_const ta_consts,
    uint8_t ta_defs_len,
    hwp_data_ostream& o_stream
)
{
    FAPI_INF("Start");

    Target < TARGET_TYPE_ANY_POZ_CHIP > l_trctrl_target = i_target;
    Target < TARGET_TYPE_ANY_POZ_CHIP > l_dbg_target = i_target;
    fapi2::ReturnCode l_rc = FAPI2_RC_SUCCESS;

    TraceArrayFinder l_ta_finder(i_args.trace_bus, l_trctrl_target, ta_defs, ta_consts, ta_defs_len);
    FAPI_DBG("Assigning targets");
    TargetType arg_type = l_trctrl_target.getType();
    //TargetType ta_type = poz_gettracearray_target_type(i_args.trace_bus);


    FAPI_DBG("Assignment of trace control and trace base");

    uint32_t DEBUG_TRACE_CONTROL = l_ta_finder.debug_scom_base;
    const uint32_t TRACE_SCOM_BASE     = l_ta_finder.trace_scom_base;
    DEBUG_TRACE_CONTROL = l_ta_finder.debug_scom_base +
                          DEBUG_TRACE_CONTROL_OFS;


    FAPI_DBG("Bus Checking");
    FAPI_ASSERT(l_rc != FAPI2_RC_INVALID_PARAMETER,
                GETTRACEARRAY_INVALID_BUS()
                .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus),
                "Specified trace bus could not be converted to required target type 0x%X, the supplied"
                //" target is of type 0x%X", ta_type, arg_type);
                " target is of type 0x%X", TARGET_TYPE_ANY_POZ_CHIP, arg_type);

    FAPI_ASSERT(l_ta_finder.valid, GETTRACEARRAY_INVALID_BUS()
                .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus),
                "Invalid trace bus specified: 0x%X", i_args.trace_bus);

    /*FAPI_ASSERT((arg_type & ta_type) != 0, GETTRACEARRAY_INVALID_BUS()
                .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus),
                "Specified trace bus requires target type 0x%X, but the supplied target is of type 0x%X",
                ta_type, arg_type); */

    FAPI_DBG("TRACE BUS : 0x%08x", i_args.trace_bus);
    FAPI_DBG("DEBUG ADDR: 0x%08x", DEBUG_TRACE_CONTROL);
    FAPI_DBG("TRACE BASE: 0x%08x", TRACE_SCOM_BASE);

    /* confirm the mux setting unless we are not dumping, or explicitly
       instructed to skip the check */
    if (!i_args.ignore_mux_setting &&
        i_args.collect_dump)
    {
        buffer<uint64_t> buf;
        uint32_t cur_sel = 0;
        FAPI_TRY(getScom(l_trctrl_target,
                         (TRACE_SCOM_BASE + TRACE_TRCTRL_CONFIG_OFS),
                         buf),
                 "Failed to read current trace mux setting");
        buf.extractToRight<TRA0_TR0_CONFIG_TRACE_SELECT_CONTROL, TRA0_TR0_CONFIG_TRACE_SELECT_CONTROL_LEN>(cur_sel);
        FAPI_DBG("TRACE SEL : 0x%08x", cur_sel);

        // If the secondary mux select is <4 then this trace must use both muxes; determine if both muxes are set
        // to receive data from the requseted trace bus. Otherwise only check primary mux.
        // The config stores the mux0 in bits 14:15 and the mux1 in bits 16:17 .
        if ( use_sec_mux(l_ta_finder.sec_mux_sel))
        {
            FAPI_ASSERT((cur_sel == ((l_ta_finder.pri_mux_sel << 2) | (l_ta_finder.sec_mux_sel))),
                        GETTRACEARRAY_TRACE_MUX_INCORRECT()
                        .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus).set_MUX_SELECT(cur_sel),
                        "Secondary trace mux is set to %d, but %d is needed for requested trace bus\n"
                        "Primary trace mux is set to %d, but %d is needed for requested trace bus",
                        (cur_sel >> 2), l_ta_finder.sec_mux_sel, (cur_sel & 0x3), l_ta_finder.pri_mux_sel);
        }
        else
        {
            // Muxes that do not have selects will always be 0
            FAPI_ASSERT(cur_sel >> 2 == l_ta_finder.pri_mux_sel,
                        GETTRACEARRAY_TRACE_MUX_INCORRECT()
                        .set_TARGET(i_target).set_TRACE_BUS(i_args.trace_bus).set_MUX_SELECT(cur_sel),
                        "Primary trace mux is set to %d, but %d is needed for requested trace bus",
                        cur_sel >> 2, l_ta_finder.pri_mux_sel);
        }
    }


    if (i_args.stop_pre_dump)
    {
        FAPI_DBG("Stopping trace arrays");
        buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_STOP>();

        FAPI_TRY(putScom(l_dbg_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to stop chiplet domain trace arrays");
    }

    if (i_args.collect_dump)
    {

        /* Run the do_dump subroutine, turn the TRACE_RUNNING return code into FFDC */
        l_rc = do_dump(l_trctrl_target,
                       i_args,
                       ta_consts,
                       TRACE_SCOM_BASE,
                       o_stream);

        FAPI_TRY(l_rc);
    }

    if (i_args.reset_post_dump)
    {
        FAPI_DBG("Resetting trace arrays");
        buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_RESET>();

        FAPI_TRY(putScom(l_dbg_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to reset chiplet domain trace arrays");
    }

    if (i_args.restart_post_dump)
    {
        FAPI_DBG("Starting trace arrays");
        buffer<uint64_t> buf;
        buf.flush<0>().setBit<DEBUG_TRACE_CONTROL_START>();

        FAPI_TRY(putScom(l_dbg_target, DEBUG_TRACE_CONTROL, buf),
                 "Failed to restart chiplet domain trace arrays");
    }

    // mark HWP exit
    FAPI_INF("Success");
    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    return current_err;

}
