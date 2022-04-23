/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdscomaccess.C $     */
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
#include "sbecmdscomaccess.H"
#include "sbefifo.H"
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbescom.H"
#include "sbeutil.H"
#include "sbeFifoMsgUtils.H"
#include "plat_hw_access.H"
#include "sbeglobals.H"
#include "chipop_struct.H"

using namespace fapi2;

//////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeGetScom (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetScom "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeGetScomReqMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Will attempt to dequeue two entries for the scom addresses plus
        // the expected EOT entry at the end
        uint32_t len2dequeue  = sizeof(msg)/sizeof(uint32_t);
        l_rc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, true, false, type);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        uint32_t len2enqueue  = 0;
        uint32_t downFifoRespBuf[2] = {0};

        uint64_t addr = ( (uint64_t)msg.hiAddr << 32) | msg.lowAddr;
        uint64_t scomData = 0;
        SBE_DEBUG(SBE_FUNC "sbeGetScom scomAddr[0x%08X%08X]",
            msg.hiAddr, msg.lowAddr);

        checkIndirectAndDoScom(true, addr, scomData, &hdr, &ffdc, type);

        // scom failed
        if (hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC"sbeGetScom failed, scomAddr[0x%08X%08X]",
                msg.hiAddr, msg.lowAddr);
            break;
        }
        else // successful scom
        {
            downFifoRespBuf[0] = (uint32_t)(scomData>>32);
            downFifoRespBuf[1] = (uint32_t)(scomData);

            SBE_DEBUG(SBE_FUNC"getscom succeeds, scomData[0x%08X%08X]",
                downFifoRespBuf[0], downFifoRespBuf[1]);

            // Push the data into downstream FIFO
            len2enqueue = 2;
            l_rc = sbeDownFifoEnq_mult (len2enqueue, &downFifoRespBuf[0], type);
            if (l_rc)
            {
                // will let command processor routine handle the failure
                break;
            }
        } // end successful scom

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
       // will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

/////////////////////////////////////////////////////
////////////////////////////////////////////////////////
uint32_t sbePutScom (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbePutScom "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbePutScomReqMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Will attempt to dequeue four entries for the scom address
        // (two entries) and the corresponding data (two entries) plus
        // the expected EOT entry at the end

        uint32_t  len2dequeue  = sizeof(msg)/sizeof(uint32_t);
        l_rc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, true, false, type);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        uint64_t scomData = 0;
        // successfully dequeued two entries for
        // scom address followed by the EOT entry

        // Data entry 0 : Scom Register Address (0..31)
        // Data entry 1 : Scom Register Address (32..63)
        // Data entry 2 : Scom Register Data (0..31)
        // Data entry 3 : Scom Register Data (32..63)
        // For Direct SCOM, will ignore entry 0

        scomData = msg.getScomData();

        uint64_t addr = ( (uint64_t)  msg.hiAddr << 32) | msg.lowAddr;
        SBE_DEBUG(SBE_FUNC "sbePutScom scomAddr[0x%08X%08X]",
            msg.hiAddr, msg.lowAddr);

        checkIndirectAndDoScom(false, addr, scomData, &hdr, &ffdc, type);

        // scom failed
        if (hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC"sbePutScom failure data, scomAddr[0x%08X%08X], "
                "scomData[0x%08X%08X]", msg.hiAddr, msg.lowAddr,
                SBE::higher32BWord(scomData), SBE::lower32BWord(scomData));
            break;
        }

    } while(false);

    if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
    {
        // Build the response header packet
        l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
       // will let command processor routine handle the failure
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
