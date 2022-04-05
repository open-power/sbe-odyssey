/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdringaccess.C $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
/*
 * @file: ppe/sbe/sbefw/sbecmdringaccess.C
 *
 * @brief This file contains the SBE Ring Access chipOps
 *
 */

#include "sbecmdringaccess.H"
#include "sbefifo.H"
#include "sbeSpMsg.H"
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbeFifoMsgUtils.H"
#include "sbeutil.H"
#include "target.H"
#include "plat_hw_access.H"
#include "plat_hwp_data_stream.H"
#include "sbeglobals.H"
#include "chipop_handler.H"

using namespace fapi2;

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeGetRingWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                        fapi2::sbefifo_hwp_data_ostream& i_putStream)
{
    #define SBE_FUNC " sbeGetRingWrap "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeGetRingAccessMsgHdr_t l_reqMsg;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t l_ffdc;
    ReturnCode l_fapiRc;
    uint32_t l_len = 0;
    // Note-Read operation flow is SHIFT and then READ.
    // First time , the shift count will be 0.. because of the way it
    // works. if we shift 64bits in the very first iteration then we
    // loose first 64 bit. But still we should update l_bitSentCnt
    // because we are sending back this data
    uint32_t l_bitSentCnt = 64;

    do
    {
        // Get the ring access header
        l_len  = sizeof(sbeGetRingAccessMsgHdr_t)/sizeof(uint32_t);
        l_rc = i_getStream.get(l_len, (uint32_t *)&l_reqMsg);

        // If FIFO access failure
        //CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        SBE_INFO(SBE_FUNC "Ring Address 0x%08X User Ring Mode 0x%04X "
            "Length in Bits 0x%08X",
            (uint32_t)l_reqMsg.ringAddr,
            (uint32_t)l_reqMsg.ringMode,
            (uint32_t)l_reqMsg.ringLenInBits);

    }while(false);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( (SBE_SEC_OPERATION_SUCCESSFUL == l_rc) &&
         (i_putStream.isStreamRespHeader( respHdr.rcStatus(),l_ffdc.getRc())) )
    {
        l_rc  = i_putStream.put(l_bitSentCnt);
        if( (SBE_SEC_OPERATION_SUCCESSFUL == l_rc) )
        {
            l_rc = sbeDsSendRespHdr( respHdr, &l_ffdc,
                                     i_getStream.getFifoType() );
        }
    }
    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeGetRing(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetRing "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbeGetRingWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbePutRing(uint8_t *i_pArg)
{
#define SBE_FUNC " sbePutRing "
    SBE_ENTER(SBE_FUNC);

    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    ReturnCode fapiRc;
    sbePutRingMsgHdr_t hdr;
    uint32_t len = 0;
    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);
    sbefifo_hwp_data_istream istream(type);
    do
    {
        len = sizeof(sbePutRingMsgHdr_t)/sizeof(uint32_t);
        rc = istream.get(len, (uint32_t *)&hdr, false);
    }while(false);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( SBE_SEC_OPERATION_SUCCESSFUL == rc )
    {
        rc = sbeDsSendRespHdr( respHdr, &ffdc, istream.getFifoType());
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
#undef SBE_FUNC
}
