/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbecodeupdate.C $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
#include "sbecodeupdate.H"
#include "cmnglobals.H"
#include "ppe42_string.h"
#include "sbetrace.H"
#include "poz_scom_perv_cfam.H"
#include "poz_ppe.H"
#include "mbxscratch.H"
#include "errorcodes.H"
#include "sbefifo.H"
#include "chipop_struct.H"
#include "sbeFifoMsgUtils.H"

////////////////////////////////////////////////////////
//////// Get Code Levels Chip-op ///////////////////////
////////////////////////////////////////////////////////
uint32_t sbeGetCodeLevels (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeGetCodeLevels "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    //TODO:PFSBE163 to be uncommented during implementation
    //uint32_t fapiRc = fapi2::FAPI2_RC_SUCCESS;
    //getCodeLevelsRespMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

#if 0   //TODO: to be enabled during implementation
        //No attempt to read FIFO as no input params expected
        // call get code levels function
        fapiRc = getCodeLevels(&msg, type);
        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = CU_RC_CODE_LEVELS_HWP_FAILURE;
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
            ffdc.setRc(l_rc);
            break;
        }
#endif
    } while(false);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC"Failed. rc[0x%X]",l_rc);
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

////////////////////////////////////////////////////////
//////// Update Image Chip-op //////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeUpdateImage (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeUpdateImage "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    //TODO:PFSBE-20 to be uncommented during implementation
    //uint32_t fapiRc = fapi2::FAPI2_RC_SUCCESS;
    //getUpdateImageRespMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

#if 0   //TODO: to be enabled during implementation
        // Will attempt to dequeue three entries for the update image
        // plus the expected EOT entry at the end
        uint32_t len2dequeue = sizeof(msg)/sizeof(uint32_t);
        l_rc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, false, false, type);
        if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }

        // call update image function
        fapiRc = updateImage(&msg, type);
        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = CU_RC_UPDATE_IMAGE_HWP_FAILURE;
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
            ffdc.setRc(l_rc);
        }
#endif

        // Build the response header packet
        l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
    } while(false);

    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC"Failed. rc[0x%X]",l_rc);
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

////////////////////////////////////////////////////////
//////// Sync Partition Chip-op ////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeSyncPartition (uint8_t *i_pArg)
{
#define SBE_FUNC " sbeSyncPartition "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    //TODO:PFSBE-21 to be uncommented during implementation
    //uint32_t fapiRc = fapi2::FAPI2_RC_SUCCESS;
    //syncPartitionRespMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;

    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

#if 0   //TODO: to be enabled during implementation
        //No attempt to read FIFO as no input params expected
        // call sync partition function
        fapiRc = syncPartition(&msg, type);
        if (fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            l_rc = CU_RC_SYNC_PARTITION_HWP_FAILURE;
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           l_rc );
            ffdc.setRc(l_rc);
            break;
        }
#endif
    } while(false);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr(hdr, &ffdc, type);
    if(l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC"Failed. rc[0x%X]",l_rc);
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

uint8_t getBootedPartition(void)
{
    return PARTITION_0_INDEX;
}

