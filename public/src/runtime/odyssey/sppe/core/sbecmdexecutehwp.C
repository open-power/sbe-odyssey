/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdexecutehwp.C $     */
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
#include "sbeFifoMsgUtils.H"
#include "sbecmdexecutehwp.H"
#include "sbeFifoMsgUtils.H"
#include "plat_hwp_data_stream.H"
#include "chipop_handler.H"
#include "target.H"
#include "hwp_table.H"
#include "hwp_includes.H"
#include "hwpWrapper.H"
#include "atefw.H"

using namespace fapi2;

// Refer POZ spec for this table.
// IO HWP Class
static hwpMap_t hwpClass1PtrTbl[] =
          {
               HWP_MAP(istepLoadIOPPEwithOcmb, ody_omi_hss_ppe_load),     // 1.01
               HWP_MAP( istepWithOcmb, ody_omi_hss_config ),              // 1.02
               HWP_MAP( istepWithOcmb, ody_omi_hss_ppe_start),            // 1.03
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_init ),           // 1.04
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_start ),          // 1.05
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_poll ),           // 1.06
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_cleanup),         // 1.07
               HWP_MAP( istepWithOcmb, ody_omi_hss_init ),                // 1.08
               HWP_MAP( istepWithOcmb, ody_omi_hss_dccal_start ),         // 1.09
               HWP_MAP( istepWithOcmb, ody_omi_hss_dccal_poll ),          // 1.10
               HWP_MAP( istepWithOcmb, ody_omi_hss_tx_zcal ),             // 1.11
               HWP_MAP( istepWithOcmb, ody_omi_pretrain_adv ),            // 1.12
               HWP_MAP( istepWithOcmb, ody_omi_setup ),                   // 1.13
               HWP_MAP( istepWithOcmb, ody_omi_train ),                   // 1.14
               HWP_MAP( istepWithOcmb, ody_omi_train_check ),             // 1.15
               HWP_MAP( istepWithOcmb, ody_omi_posttrain_adv ),           // 1.16
          };

// MEM HWP Class
static hwpMap_t hwpClass2PtrTbl[] =
          {
               HWP_MAP( istepWithOcmb, ody_scominit ),              // 2.01
               HWP_MAP( istepWithOcmb, ody_ddrphyinit ),            // 2.02
               HWP_MAP( istepATEPHYReset, NULL ),                   // 2.03 LAB HWPs
               HWP_MAP( istepLoadIMEMwithOcmb, ody_load_imem ),     // 2.04
               HWP_MAP( istepLoadDMEMwithOcmb, ody_load_dmem ),     // 2.05
               HWP_MAP( istepDraminitWithOcmb, ody_sppe_draminit ), // 2.06
               HWP_MAP( istepATEFW, NULL),                          // 2.07 LAB HWPs
               HWP_MAP( istepWithOcmb, ody_load_pie ),              // 2.08
               HWP_MAP( istepWithOcmb, ody_draminit_mc ),           // 2.09
               HWP_MAP( istepWithOcmb, ody_enable_ecc ),            // 2.10
               HWP_MAP( istepWithOcmb, ody_thermal_init ),          // 2.11
          };

hwpTableEntry_t hwpTableEntries[] = {
    HWP_ENTRY(  1, hwpClass1PtrTbl),
    HWP_ENTRY(  2, hwpClass2PtrTbl),
};

REGISTER_HWP_TABLE(hwpTableEntries)

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
    bool isValid = true;
    do
    {
        // Validate the HWP Class.
        SBE_INFO(SBE_FUNC " HWPTable length is 0x%02X", hwpTable.len);
        if(hwpClass > hwpTable.len)
        {
            SBE_ERROR(SBE_FUNC "Invalid HWP Class 0x%02X", hwpClass);
            isValid = false;
            break;
        }

        // Validate the HWP Number.
        auto hwpTableEntry = &hwpTable.hwpClassArr[hwpClass - 1];
        SBE_INFO(SBE_FUNC " HWPTableArray length is 0x%02X for HWP table Array 0x%02X",
                            hwpTableEntry->len, hwpClass);
        if(hwpNum > hwpTableEntry->len)
        {
            SBE_ERROR(SBE_FUNC "Invalid HWP Number 0x%02X", hwpNum);
            isValid = false;
            break;
        }

    }while(0);
    SBE_EXIT(SBE_FUNC);
    return isValid;
    #undef SBE_FUNC
}

uint32_t sbeExecuteHWP(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeExecuteHWP "
    SBE_ENTER(SBE_FUNC);

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
        auto hwpTableEntry = &hwpTable.hwpClassArr[regReqMsg.hwpClass - 1];
        auto hwpMap = &hwpTableEntry->hwpNum[regReqMsg.hwpNum - 1];
        if(hwpMap->hwpWrapper != NULL)
        {
            fapiRc = hwpMap->hwpWrapper(hwpMap->hwp);
            if(fapiRc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " HWP failed for class 0x%02X and number 0x%02X "
                                   "with rc 0x%08X", regReqMsg.hwpClass, regReqMsg.hwpNum, fapiRc);
                hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_HWP_FAILURE);
                ffdc.setRc(fapiRc);
            }
            break;
        }
        // There will be no data sent to upstream FIFO

    }while(0);
    do
    {
        // Build the response header packet.
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
        rc = sbeDsSendRespHdr(hdr, &ffdc, type);
        // will let command processor routine handle the failure.
    }while(0);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
