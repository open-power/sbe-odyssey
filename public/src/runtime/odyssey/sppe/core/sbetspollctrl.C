/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspollctrl.C $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "sbetspollctrl.H"
#include "sbeSpMsg.H"
#include "sbe_sp_intf.H"
#include "plat_hwp_data_stream.H"
#include "chipop_handler.H"
#include "fapi2.H"

using namespace fapi2;

uint32_t sbectrltspoll(uint8_t *i_pArg)
{
#define SBE_FUNC " sbectrltspoll "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    ReturnCode fapiRc;
    sbeCtrlTSandDQSpollReqMsg_t reqMsg;
    uint32_t len = 0;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_istream istream(type);

    do{

        len = sizeof(sbeCtrlTSandDQSpollReqMsg_t)/sizeof(uint32_t);
        fifoRc = istream.get(len, (uint32_t *)&reqMsg);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        SBE_DEBUG(SBE_FUNC "Updating ATTR_ODY_SENSOR_POLLING_PERIOD_MS to 0x%08x MilliSec",
                            reqMsg.tempSensorPollIntervalMs);

        if(!(reqMsg.validateInputParams()))
        {
            SBE_ERROR(SBE_FUNC "Invalid params: ATTR_ODY_SENSOR_POLLING_PERIOD_MS: 0x%08x \
                                             & ATTR_ODY_DQS_TRACKING_PERIOD: 0x%08x ",
                                reqMsg.tempSensorPollIntervalMs, reqMsg.dqsInterval);
            respHdr.setStatus( SBE_PRI_INVALID_DATA,
                               SBE_SEC_INVALID_PARAMS);
            break;
        }

        fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD_MS = reqMsg.tempSensorPollIntervalMs;
        fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_DQS_TRACKING_PERIOD = reqMsg.dqsInterval;

        SBE_INFO(SBE_FUNC "Updated ATTR_ODY_SENSOR_POLLING_PERIOD_MS & ATTR_ODY_DQS_TRACKING_PERIOD \
                            to 0x%08x & 0x%08x",
                            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD_MS,
                            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_DQS_TRACKING_PERIOD);

    }while(false);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( SBE_SEC_OPERATION_SUCCESSFUL == fifoRc )
    {
        fifoRc = sbeDsSendRespHdr( respHdr, &ffdc, istream.getFifoType());
    }

    SBE_EXIT(SBE_FUNC);
    return fifoRc;
#undef SBE_FUNC
}