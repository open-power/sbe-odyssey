/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbecmdgenericffdc.C $          */
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

#include "sbeFifoMsgUtils.H"
#include "chipop_struct.H"
#include "sbeffdc.H"

using namespace fapi2;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
uint32_t sbeGetFfdc (uint8_t *i_pArg)
{
    #define SBE_FUNC "sbeGetFfdc "
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t len = 0;
    sbeFifoType type;
    sbeRespGenHdr_t respHdr;
    respHdr.init();

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_INFO(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Dequeue the EOT entry as no more data is expected.
        rc = sbeUpFifoDeq_mult (len, NULL, true, false, type);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        sbeResponseFfdc_t ffdc;
        uint32_t dumpFieldsConfig = 0;
        ffdc.setRc(fapi2::g_FfdcData.fapiRc);
        SBE_INFO(SBE_FUNC"FAPI RC is %x", g_FfdcData.fapiRc);

        //Send the HW Package and Plat Package data over FIFO.
        dumpFieldsConfig = SBE_FFDC_ALL_DATA;

        //Send the FFDC data over FIFO.
        uint32_t wordCount = 0;
        rc = sendFFDCOverFIFO(dumpFieldsConfig, wordCount, true);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        rc = sbeDsSendRespHdr(respHdr, NULL, type);
        if (rc)
        {
            SBE_ERROR(SBE_FUNC "Failed in sbeDsSendRespHdr, rc[0x%08X]", rc);
            break;
        }

    }while(0);

    if( rc )
    {
        SBE_ERROR(SBE_FUNC "Failed. rc[0x%X]", rc);
    }
    return rc;
    #undef SBE_FUNC
}
//////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
