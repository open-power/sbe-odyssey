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
#define MAX_BUFFER_SIZE                 0x1000 // 4KB - In bytes always
#define SPI_ENGINE_NOR                  0
#define SPIM_BASEADDR_PIB               0x70000
#define NOR_DEVICE_MAX_SIZE             0x1000000 // 16MB

using spi::SPIPort;
using spi::FlashDevice;

#define SECTOR_NUM_CHECK(sector, address)\
        ((address >= (sector * SMALLEST_ERASE_BLOCK_SIZE)) && (address < ((sector + 1) * SMALLEST_ERASE_BLOCK_SIZE)))
#define SECTOR_00 0x00
#define SECTOR_72 0x48

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

// A flash device with a built-in offset to encapsulate the boot side selection
class OdyFlashDevice : public FlashDevice
{
public:
    OdyFlashDevice(const spi::AbstractPort& i_port,
                   device_type i_devtype,
                   uint32_t i_raw_size,
                   uint8_t* i_erase_buffer) :
        FlashDevice(i_port, i_devtype, i_raw_size, i_erase_buffer)
        {}

private:
    fapi2::ReturnCode read_extended_status(uint32_t i_address, extended_status& o_status) const override
    {
        FAPI_TRY(FlashDevice::read_extended_status(i_address, o_status));
        if ((o_status & (ES_ERASE_FAIL | ES_PROG_FAIL)) &&
            (SECTOR_NUM_CHECK(SECTOR_00, i_address) || SECTOR_NUM_CHECK(SECTOR_72, i_address)))
        {
            // Ignore erase/write errors on debug partition
            o_status = ES_NONE;
        }

    fapi_try_exit:
        return current_err;
    }
};

// A struct to hold all objects associated with a memory device in one place
// for easy allocation and freeing
struct OdyMemoryDevice {
    OdyFlashDevice flash;
    SPIPort port;
    uint8_t erase_buffer[0] __attribute__((aligned(8)));
};

uint32_t createMemoryDevice(
    uint8_t i_boot_side,
    uint8_t i_memory_id,
    bool i_use_ecc,
    bool i_intent_to_write,
    spi::AbstractMemoryDevice *&o_mem_device)
{
    #define SBE_FUNC " createMemoryDevice "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        if ((i_boot_side != SIDE_0_INDEX) && (i_boot_side != SIDE_1_INDEX))
        {
            // Fail if i_boot_side is any value other than 0 & 1 (excl. golden)
            SBE_ERROR(SBE_FUNC "Invalid boot side:[%d]", i_boot_side);
            l_rc = SBE_SEC_CU_INVALID_BOOT_SIDE;
            break;
        }

        if (i_memory_id != 0)
        {
            // Fail if i_memory_id != 0 since that is all we support for Odyssey
            SBE_ERROR(SBE_FUNC "Memory dev Id:[%d] not supported", i_memory_id);
            l_rc = SBE_SEC_CU_MEM_DEV_ID_NOT_SUPPORTED;
            break;
        }

        if (i_use_ecc == false)
        {
            // Fail if ECC use is set to false
            SBE_ERROR(SBE_FUNC "ECC should be enabled[%d]", i_use_ecc);
            l_rc = SBE_SEC_CU_ECC_USE_DISABLED;
            break;
        }

        //
        // Allocate driver objects and eventual erase buffer from scratch
        //
        const uint32_t l_alloc_size = sizeof(OdyMemoryDevice) + (i_intent_to_write ? FlashDevice::ERASE_BUFFER_SIZE : 0);
        auto const l_memblock = static_cast<OdyMemoryDevice *>(Heap::get_instance().scratch_alloc(l_alloc_size));
        if (!l_memblock)
        {
            // Handle alloc error
            SBE_ERROR(SBE_FUNC "Allocation of buffer size [0x%08x] failed", l_alloc_size);
            l_rc = SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
            break;
        }

        //
        // Initialize SPI port object using placement new
        //
        new (&l_memblock->port) SPIPort((fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&)g_platTarget->plat_getChipTarget(),
                                        SPIM_BASEADDR_PIB, SPI_ENGINE_NOR,
                                        i_use_ecc ? spi::ECC_ENABLED : spi::ECC_DISCARD);

        //
        // Detect Flash device type
        //
        static bool l_dev_type_known = false;
        static FlashDevice::device_type l_dev_type;
        if (!l_dev_type_known)
        {
            fapi2::ReturnCode l_fapiRc = FlashDevice::detect_device(l_memblock->port, l_dev_type);
            if (l_fapiRc != fapi2::FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "Unknown device, id=[0x%08x]", l_dev_type);
                l_rc = SBE_SEC_CU_UNKNOWN_DEVICE;
                break;
            }
            SBE_INFO(SBE_FUNC "Device type:[%d]", l_dev_type);
            l_dev_type_known = true;
        }

        //
        // Initialize Flash driver object using placement new
        //
        uint8_t * const l_erase_buffer = i_intent_to_write ? l_memblock->erase_buffer : NULL;
        new (&l_memblock->flash) OdyFlashDevice(l_memblock->port, l_dev_type, NOR_DEVICE_MAX_SIZE, l_erase_buffer);

        // Return the pointer to the Flash device, which is at the beginning of our memory
        // block so we can use that same pointer to free later.
        o_mem_device = &l_memblock->flash;
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

void freeMemoryDevice(spi::AbstractMemoryDevice *i_mem_device)
{
    // Cast the pointer back to our original memory block type
    auto const l_memblock = reinterpret_cast<OdyMemoryDevice *>(i_mem_device);

    // Destroy the objects we created
    l_memblock->flash.~OdyFlashDevice();
    l_memblock->port.~SPIPort();

    // Free the memory block regardless of whether the previous call failed or not
    Heap::get_instance().scratch_free(l_memblock);
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
