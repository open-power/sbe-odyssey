/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/stopclocks/sbecmdstopclocks.C $ */
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

#include "sbecmdstopclocks.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_handler.H"
#include "plat_hwp_data_stream.H"

using namespace fapi2;


uint32_t sbeStopClocks(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeStopClocks "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    l_rc = sbeStopClocksWrap (istream, ostream);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t sbeStopClocksWrap (fapi2::sbefifo_hwp_data_istream& i_getStream,
                            fapi2::sbefifo_hwp_data_ostream& io_putStream )
{
    #define SBE_FUNC " sbeStopClocksWrap: "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t fapiRc = FAPI2_RC_SUCCESS;
    uint32_t len = 0;
    sbeResponseFfdc_t ffdc;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeStopClocksReqMsgHdr_t reqMsg = {0};

    do
    {
        // Get the TargetType and Instance Id from the command message
        // Get the stop clock header length
        len  = sizeof(sbeStopClocksReqMsgHdr_t)/sizeof(uint32_t);
        l_rc = i_getStream.get(len, (uint32_t *)&reqMsg);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        SBE_INFO(SBE_FUNC "LogTargetType 0x%02X, Instance Id 0x%02X",
                    (uint8_t)reqMsg.iv_logTargetType,
                    (uint8_t)reqMsg.iv_instanceId);

        // validate the chipop target parameter
        if ( reqMsg.validateParams() != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            respHdr.setStatus( SBE_PRI_INVALID_DATA, reqMsg.validateParams());
            break;
        }

        SBE_DEBUG(SBE_FUNC "Calling stopclocks HWP with log target : 0x%x",
                            reqMsg.iv_logTargetType);
        /* Executing stopclock HWP */
        fapiRc = reqMsg.executeHwp( );
        if(fapiRc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "Failed in stopclocks(), fapiRc=0x%.8x, "
                                "Log Target type: [0x%04X], Instance Id [0x%02X]",
                                fapiRc, (uint16_t)reqMsg.iv_logTargetType,
                                (uint8_t)reqMsg.iv_instanceId );
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_STOP_CLOCK_FAILED );
            ffdc.setRc(fapiRc);
        }

    }while(0);

    // In case of stopclock chipop SBE will send the response
    if(io_putStream.isStreamRespHeader(respHdr.rcStatus(),ffdc.getRc()))
    {
        // If there was a FIFO error, will skip sending the response,
        // instead give the control back to the command processor thread
        if(SBE_SEC_OPERATION_SUCCESSFUL == l_rc)
        {
            // Create the Response to caller
            l_rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType());
        }
    }
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

