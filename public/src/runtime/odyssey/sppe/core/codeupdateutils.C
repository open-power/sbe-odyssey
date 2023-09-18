/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/codeupdateutils.C $      */
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
#include "cmnglobals.H"
#include "odysseylink.H"
#include "sberegaccess.H"
#include "sbetrace.H"
#include "codeupdateutils.H"
#include "pakwrapper.H"
#include "filenames.H"

#define NOR_BASE_ADDRESS_MASK           0xFF000000
#define NOR_FLASH_SECTOR_BOUNDARY_ALIGN 0xFFFFF000
#define MAX_BUFFER_SIZE                 0x10000 // 64KB - In bytes always
#define SPI_ENGINE_NOR                  0


void getSideInfo(uint8_t &o_runningSide,
                 uint8_t &o_nonRunningSide)
{
    #define SBE_FUNC " getSideInfo "
    SBE_ENTER(SBE_FUNC);

    // to get running side
    o_runningSide = SbeRegAccess::theSbeRegAccess().getBootSelection();

    // to get non-running side
    if (o_runningSide == SIDE_0_INDEX)
    {
        // if booting from primary then making non-running is secondary
        o_nonRunningSide = SIDE_1_INDEX;
    }
    else if (o_runningSide == SIDE_1_INDEX)
    {
        // if booting from secondary then making non-running is primary
        o_nonRunningSide = SIDE_0_INDEX;
    }
    else if (o_runningSide == GOLDEN_SIDE_INDEX)
    {
        // if booting from golden then making non-running is primary
        o_nonRunningSide = SIDE_0_INDEX;
    }

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

void getSideAddress(const uint8_t i_side, uint32_t &o_sideStartAddress)
{
    #define SBE_FUNC " getSideAddress "
    SBE_ENTER(SBE_FUNC);

    // Get the side start offset
    o_sideStartAddress = getAbsPartitionAddr(i_side);
    SBE_INFO(SBE_FUNC "For Side: [0x%02x] Side Start Address:[0x%08x]",
             i_side, o_sideStartAddress);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

void getCodeUpdateParams(codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct)
{
    #define SBE_FUNC " getCodeUpdateParams "
    SBE_ENTER(SBE_FUNC);

    // Get storage dev params
    io_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddress      = NOR_SIDE_0_START_ADDR;
    io_codeUpdateCtrlStruct.storageDevStruct.storageDevBaseAddressMask  = NOR_BASE_ADDRESS_MASK;
    io_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize         = NOR_SIDE_SIZE;
    io_codeUpdateCtrlStruct.storageDevStruct.storageSectorBoundaryAlign = NOR_FLASH_SECTOR_BOUNDARY_ALIGN;
    io_codeUpdateCtrlStruct.storageDevStruct.storageSubSectorCheckMask  = NOR_FLASH_SUB_SECTOR_BOUNDARY_CHECK_MASK;
    io_codeUpdateCtrlStruct.storageDevStruct.storageSubSectorSize       = NOR_FLASH_SUB_SECTOR_SIZE;
    io_codeUpdateCtrlStruct.storageDevStruct.devEngineNum               = SPI_ENGINE_NOR;
    io_codeUpdateCtrlStruct.storageDevStruct.maxBufferSize              = MAX_BUFFER_SIZE;

    // Get side info from booted device
    getSideInfo(io_codeUpdateCtrlStruct.runSideIndex,
                io_codeUpdateCtrlStruct.nonRunSideIndex);

    SBE_INFO(SBE_FUNC "RunSide:[%d] NonRunSide:[%d] MaxBuf:[0x%08x]",
             io_codeUpdateCtrlStruct.runSideIndex,
             io_codeUpdateCtrlStruct.nonRunSideIndex,
             io_codeUpdateCtrlStruct.storageDevStruct.maxBufferSize);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}


fapi2::ReturnCode deviceErase(SpiControlHandle& i_handle,
                              uint32_t i_eraseStartAddress,
                              uint32_t i_eraseEndAddress)
{
    return spi_erase_and_no_preserve(i_handle,
                                     i_eraseStartAddress,
                                     i_eraseEndAddress);
}


fapi2::ReturnCode deviceWrite(SpiControlHandle& i_handle,
                              uint32_t i_writeAddress,
                              uint32_t i_writeLength,
                              void *i_buffer,
                              bool i_ecc)
{
    if (i_ecc == true)
    {
        return spi_write_ecc(i_handle,
                             i_writeAddress,
                             i_writeLength,
                             (uint8_t *)i_buffer,
                             i_ecc);
    }
    else
    {
        return spi_write(i_handle,
                         i_writeAddress,
                         i_writeLength,
                         (uint8_t *)i_buffer);
    }
}

fapi2::ReturnCode deviceRead(SpiControlHandle& i_handle,
                             const uint32_t i_readAddress,
                             const uint32_t i_readLength,
                             const SPI_ECC_CONTROL_STATUS i_eccStatus,
                             void *o_buffer
                            )
{
    return spi_read(i_handle,
                    i_readAddress,
                    i_readLength,
                    i_eccStatus,
                    (uint8_t *)o_buffer);
}

uint32_t checkSignature(const CU_IMAGES i_imageType,
			void *i_pakStartAddrInPibMem,
			codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct)
{
    return SBE_SEC_OPERATION_SUCCESSFUL;
}

uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_side,
                      codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct,
                      uint8_t* o_hashArrayPtr)
{
    #define SBE_FUNC " getImageHash "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ARC_RET_t l_pakRC = ARC_OPERATION_SUCCESSFUL;
    uint32_t l_size = 0;
    uint32_t l_sideStartAddress = 0;

    do
    {
        //To get side start address
        getSideAddress(i_side,l_sideStartAddress);
        PakWrapper pak((void *)l_sideStartAddress, (void *)(l_sideStartAddress + io_codeUpdateCtrlStruct.storageDevStruct.storageDevSideSize));

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

        if (SHA3_DIGEST_LENGTH != l_size)
        {
            l_rc = SBE_SEC_CU_IMAGE_HASH_SIZE_MISMATCH;
            SBE_ERROR(SBE_FUNC "Failed to read expected hash size of image:%d" \
                               "Expected size: %d,actual size: %d ",
                                i_imageType,SHA3_DIGEST_LENGTH,l_size);
            break;
        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
