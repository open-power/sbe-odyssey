/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/exechwp/sbecmdexecutehwp.C $ */
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
#include "sbeFifoMsgUtils.H"
#include "sbecmdexecutehwp.H"
#include "sbeFifoMsgUtils.H"
#include "plat_hwp_data_stream.H"
#include "chipop_handler.H"
#include "target.H"
#include "hwptable.H"
#include "hwp_includes.H"
#include "hwpWrapper.H"
#include "sberegaccess.H"
#include "sbeffdc.H"
#include "sbestates.H"
#include "sbecmdexecutehwpseqcmplt.H"

using namespace fapi2;

/**
 * @brief it is used to update the state of sbe to runtime
 *
 * @return void
 */
void poz_exec_hwp_sequence_complete(void)
{
    #define SBE_FUNC " poz_exec_hwp_sequence_complete "
    SBE_ENTER(SBE_FUNC);

    SBE_INFO(SBE_FUNC " Transitioning to runtime state...");
    // Update SBE State to runtime
    (void)SbeRegAccess::theSbeRegAccess().updateSbeState(SBE_STATE_CMN_RUNTIME);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

/**
 * @brief checks if executeHWP chips-op params are valid.
 *
 * @param[in] hwpClass Class of HWP
 * @param[in] hwpNum   HWP Number for the class
 * @return true if valid request, false otherwise
 */
bool isHWPParamsValid(const uint8_t hwpClass, const uint8_t hwpNum)
{
    #define SBE_FUNC " isHWPParamsValid "
    SBE_ENTER(SBE_FUNC);

    // Declare all local variable over here
    bool isValid = false;
    do
    {
        // If there is no HWP then return parameter is not valid
        if( 0 == hwpNum )
        {
            break;
        }

        // Validate the HWP Class and Number.
        SBE_INFO(SBE_FUNC " HWPTable length is 0x%02X", g_hwpTable.len);
        for(size_t entry = 0; entry < g_hwpTable.len; entry++)
        {
            auto hwpTableEntry = &g_hwpTable.hwpClassArr[entry];
            if( hwpClass == hwpTableEntry->hwpClassNum )
            {
                if( hwpNum <= hwpTableEntry->len )
                {
                    isValid = true;
                }
                break;
            }
        }
    } while(false);
    SBE_EXIT(SBE_FUNC);
    return isValid;
    #undef SBE_FUNC
}

uint32_t sbeExecuteHWP(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeExecuteHWP "
    SBE_ENTER(SBE_FUNC);

    // Declare all local variable over here
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    sbeExecuteHwpMsgHdr_t regReqMsg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);
    SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

    fapi2::sbefifo_hwp_data_istream istream(type);

    do
    {
        // Get the executeHWP message header.
        uint32_t len2dequeue  = sizeof(regReqMsg)/sizeof(uint32_t);
        rc = istream.get(len2dequeue, (uint32_t *)&regReqMsg, true, false);

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        SBE_INFO(SBE_FUNC "HWP Class is: [%02X] and HWP Number is: [%02X]", regReqMsg.hwpClass, regReqMsg.hwpNum);

        // Validate the params.
        if(!isHWPParamsValid(regReqMsg.hwpClass, regReqMsg.hwpNum))
        {
            SBE_ERROR(SBE_FUNC "Params passed is not correct. HWP Class [0x%02X] "
                               "HWP Number [0x%02X]", regReqMsg.hwpClass, regReqMsg.hwpNum);
            hdr.setStatus( SBE_PRI_INVALID_DATA,
                           SBE_SEC_INVALID_PARAMS);
            break;
        }

        // Execute the HWP
        for(size_t entry = 0; entry < g_hwpTable.len; entry++)
        {
            auto hwpTableEntry = &g_hwpTable.hwpClassArr[entry];

            uint8_t l_lengthOfHwpNo = hwpTableEntry->len;

            if(( regReqMsg.hwpClass == hwpTableEntry->hwpClassNum ) &&
               ( regReqMsg.hwpNum <=  l_lengthOfHwpNo))
            {
                auto hwpMap = &hwpTableEntry->hwpNum[regReqMsg.hwpNum-1];
                fapiRc = hwpMap->hwpWrapper(hwpMap->hwp);
                if(fapiRc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC " HWP failed for class 0x%02X and number 0x%02X "
                                    "with rc 0x%08X", regReqMsg.hwpClass, regReqMsg.hwpNum, fapiRc);
                    hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                SBE_SEC_HWP_FAILURE);
                    ffdc.setRc(fapiRc);

                    // Update SBE State to dump state
                    (void)SbeRegAccess::theSbeRegAccess().updateSbeState(SBE_STATE_CMN_DUMP);

                }
                break;
            }
        }
        // There will be no data sent to upstream FIFO

    } while( false );

    do
    {
        // Build the response header packet.
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
        rc = sbeDsSendRespHdr(hdr, &ffdc, type);
        // will let command processor routine handle the failure.
    } while( false );

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
