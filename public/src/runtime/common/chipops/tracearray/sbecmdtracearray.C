/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/tracearray/sbecmdtracearray.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "sbecmdtracearray.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_handler.H"
#include "sbeutil.H"


using namespace fapi2;

uint32_t sbeControlTraceArray(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeControlTraceArray: "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);

    l_rc = sbeControlTraceArrayWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

// Preparing the Trace Array HWP structure arg
poz_gettracearray_args sbeControlTraceArrayCmd_t::getArg( void )
{
    // Trace Array HWP structure arg
    poz_gettracearray_args l_args = {};
    // Fill trace array Id
    l_args.trace_bus =  iv_traceArrayId;
    // Fill control arguments
    // Reset the debug logic after dumping
    l_args.reset_post_dump      = ( iv_operation &
                                    SBE_TA_OPERATION_RESET);
    // Start the trace array after dumping
    l_args.restart_post_dump    = ( iv_operation &
                                    SBE_TA_OPERATION_RESTART);
    // Stop the trace array before starting the dump
    l_args.stop_pre_dump        = ( iv_operation & SBE_TA_OPERATION_STOP);
    // Do dump the trace array; useful if you just want to start/stop
    l_args.collect_dump         = ( iv_operation &
                                    SBE_TA_OPERATION_COLLECT_DUMP);
    // Do not fail if the primary trace mux is set to a different bus
    l_args.ignore_mux_setting   = ( iv_operation &
                                    SBE_TA_OPERATION_IGNORE_MUX_SETTING);

    return l_args;
}


// Trace Array wrapper function
uint32_t sbeControlTraceArrayWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                                  fapi2::sbefifo_hwp_data_ostream& i_putStream)
{
    #define SBE_FUNC " sbeControlTraceArrayWrap "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc           = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t fapiRc         = FAPI2_RC_SUCCESS;
    uint32_t l_len          = 0;

    sbeControlTraceArrayCmd_t l_req = {};
    sbeResponseFfdc_t ffdc;
    sbeRespGenHdr_t respHdr;
    respHdr.init();

    do
    {
        l_len = sizeof(sbeControlTraceArrayCmd_t)/sizeof(uint32_t);
        l_rc = i_getStream.get(l_len, (uint32_t *)&l_req);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        SBE_INFO(SBE_FUNC "traceArrayId [0x%04X] operation [0x%04X]",
                          l_req.iv_traceArrayId,
                          l_req.iv_operation);

        // Execute Trace Array HWP
        fapiRc = l_req.executeHwp ( i_putStream );
        if (fapiRc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "tracearray_hwp failed fapirc: 0x%0.8x", fapiRc);
            // Respond with HWP FFDC
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_HWP_FAILURE );
            ffdc.setRc(fapiRc);
        }

    } while (false);

    // In case of trace array chipop SBE will send the response
    if (i_putStream.isStreamRespHeader(respHdr.rcStatus(),ffdc.getRc()))
    {
        l_rc  = i_putStream.put(i_putStream.words_written());
        // If there was a FIFO error, will skip sending the response,
        // instead give the control back to the command processor thread
        if(SBE_SEC_OPERATION_SUCCESSFUL == l_rc)
        {
            // Create the Response to caller
            l_rc = sbeDsSendRespHdr(respHdr, &ffdc, i_getStream.getFifoType());
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

