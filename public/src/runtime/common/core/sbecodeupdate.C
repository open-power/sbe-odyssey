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
#include "mbxscratch.H"
#include "errorcodes.H"
#include "chipop_struct.H"
#include "sbeFifoMsgUtils.H"
#include "sbe_sp_intf.H"
#include "pakwrapper.H"
#include "filenames.H"
#include "sha3.H"
#include "codeupdateutils.H"
#include "sbe_sp_plat_intf.H"
#include "sbeutil.H"

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

uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_Partition,
                      uint8_t* o_hashArrayPtr)
{
    #define SBE_FUNC " getImageHash "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ARC_RET_t l_pakRC = ARC_OPERATION_SUCCESSFUL;
    uint32_t l_size = 0;
    uint32_t l_partitionStartAddress = 0;

    do
    {
        //To get partition start address
        getPartitionAddress(i_Partition,l_partitionStartAddress);
        PakWrapper pak((void *)l_partitionStartAddress);

        switch (i_imageType)
        {
            case CU_IMAGES::BOOTLOADER:
                l_pakRC = pak.read_file(bldr_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::RUNTIME:
                l_pakRC = pak.read_file(runtime_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::BMC_OVRD:
                l_pakRC = pak.read_file(bmc_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            case CU_IMAGES::HOST_OVRD:
                l_pakRC = pak.read_file(host_image_hash_file_name,
                                        o_hashArrayPtr,SHA3_DIGEST_LENGTH,NULL,
                                        &l_size);
                break;

            default:
                l_rc = SBE_SEC_CU_INVALID_IMAGE_TYPE;
                SBE_ERROR(SBE_FUNC " Invalid Image  Passed by caller image: %d ",
                                     i_imageType);
                break;
        }

        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        if (l_pakRC != ARC_OPERATION_SUCCESSFUL)
        {
            l_rc = SBE_SEC_CU_FILE_IMAGE_HASH_READ_ERROR;
            SBE_ERROR(SBE_FUNC " ImageType: %d " \
                      "Failed to read image hash Rc:%d",i_imageType,l_pakRC);
            break;
        }
        else if (SHA3_DIGEST_LENGTH != l_size)
        {
            l_rc = SBE_SEC_CU_IMAGE_HASH_SIZE_MISMATCH;
            SBE_ERROR(SBE_FUNC "Failed to read expected hash size of image:%d" \
                               "Expected size: %d,actual size: %d ",
                                i_imageType,SHA3_DIGEST_LENGTH,l_size);
            break;
        }
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
