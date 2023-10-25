/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/codeupdate/codeupdateutils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2024                        */
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
/*                                                                        */
/* This header file is Code-update specific that declares all libraries   */
/* need to be implemented for each project for respective customizations  */

#ifndef CODE_UPDATE_UTILS_H
#define CODE_UPDATE_UTILS_H

#include "sbe_sp_intf.H"
#include "poz_sbe_spi_cmd.H"
#include "target.H"
#include "spi_memory.H"

using namespace fapi2;
using spi::SPIPort;

/*
 * @brief Structure for storage device info
 */
struct storageDevStruct_t
{
    // For storage device
    // The base address of the storage device
    uint32_t storageDevBaseAddress;
    // Mask for storage base address for higher 8-nibble
    uint32_t storageDevBaseAddressMask;
    // Size for each side in storage device
    uint32_t storageDevSideSize;
    // This is for aligning the given address to sub-sector size
    uint32_t storageSectorBoundaryAlign;
    // Mask for sub-sector size check
    uint32_t storageSubSectorCheckMask;
    // Storage sub-sector size
    uint32_t storageSubSectorSize;

    // Buffer size used to receive data from fifo to write to storage device
    uint32_t maxBufferSize;

    // This refer to engine attach to storage device
    uint8_t devEngineNum;
    uint8_t memId;

    uint8_t reserved[2];
};

/*
 * @brief Structure for code-update
 */
struct codeUpdateCtrlStruct_t
{
    // Storage related params
    storageDevStruct_t storageDevStruct;

    // Update Image related variables as per partition table
    uint32_t imageStartAddr;
    uint32_t imageSizeMax;

    // Incoming image info as per chip-op
    uint32_t imageSizeInWords;
    uint16_t imageType;

    // Used to maintain running and non-running sides in storage device
    uint8_t nonRunSideIndex;
    uint8_t runSideIndex;

    // Constructor
    codeUpdateCtrlStruct_t() {};
};

/*
 * @brief Structure for RAS - CE/UE
 */
typedef struct
{
    uint8_t numOfCE;
    uint8_t numOfUE;
}memDeviceStatus_t;

// A SPI port for RAS support
class RASSPIPort : public SPIPort
{
public:
    RASSPIPort(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
               const uint32_t i_base_address, const uint8_t i_resp_select,
               const spi::ecc_mode i_ecc_mode) :
        SPIPort(i_target, i_base_address, i_resp_select, i_ecc_mode)
        {}

    memDeviceStatus_t iv_ioMemDeviceStatus;

private:
    //TODO:PFSBE-691 - SPI driver changes
    fapi2::ReturnCode handle_spi_errors(
            uint64_t &io_status) const override
    {
        return current_err;
    }
};

/*
 * @brief Project specific code to instantiate a memory device handle
 *
 * @param[in]  i_boot_side        Boot side of the memory device to talk to (0=PRIMARY, 1=SECONDARY)
 * @param[in]  i_memory_id        Memory ID (from partition table)
 * @param[in]  i_use_ecc          Generate ECC on write, strip ECC on read; raw mode if false
 * @param[in]  i_intent_to_write  Prepare device for write access
 * @param[out] o_mem_device       Allocated and initialized memory device
 *
 * @return SBE_SEC return code
 */
uint32_t createMemoryDevice(
    uint8_t i_boot_side,
    uint8_t i_memory_id,
    bool i_use_ecc,
    bool i_intent_to_write,
    spi::AbstractMemoryDevice *&o_mem_device);

/*
 * @brief Free a memory device allocated by createMemoryDevice
 *
 * @param[in]  i_mem_device    Memory device to free
 */
void freeMemoryDevice(spi::AbstractMemoryDevice *i_mem_device);

#define PARTITION_ENTRY_NAME_MAX_CHAR  16
namespace CU
{
    /*
     * @brief map for image::partition name in partition table
     */
    typedef struct
    {
        CU_IMAGES imageNum;
        char      imageName[PARTITION_ENTRY_NAME_MAX_CHAR];
    } imgMap_t;
}

/* @brief : This API is used to get the running side and
 *          non-running side number from the storage device
 *
 * @param[out] : o_runningSide used to pass running side number
 * @param[out] : o_nonRunningSide used to pass non-running side number
 *
 * @return : void
 */
void getSideInfo(uint8_t &o_runningSide,
                 uint8_t &o_nonRunningSide);


/* @brief : This API is used to get side start address in storage device
 *
 * @param[in]  : i_side Used to pass side information to get its start address
 *
 * @param[out] : o_sideStartAddress Used to return the given side start address
 *
 * @return : void
 */
void getSideAddress(const uint8_t i_side,
                    uint32_t &o_sideStartAddress);


/* @brief : This API is used to get the code update needed parameters
 *          populated from platform data
 *
 * @param[inout] : io_codeUpdateCtrlStruct struct used to contains various
 *                 parameters needed for code-update
 *
 * @return : void
 */
void getCodeUpdateParams(codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct);


/* @brief : This API is used to perform erase in device
 *
 * @param[in] : i_handle : SPI controller handle
 * @param[in] : i_eraseStartAddress : erase start address in device.
 *              This is physical address incl ECC.
 * @param[in] : i_eraseEndAddress : erase end address
 *              This is physical address incl ECC.
 *
 * @return : fapiRc
 */
fapi2::ReturnCode deviceErase(SpiControlHandle& i_handle,
                              uint32_t i_eraseStartAddress,
                              uint32_t i_eraseEndAddress);


/* @brief : This API is used to perform write in device
 *
 * @param[in] : i_handle : SPI controller handle
 * @param[in] : i_writeLength : Length of data to write to device
 * @param[in] : i_buffer : buffer containing data
 * @param[in] : i_writeAddress : address to write buffer data in device.
 *              -This function when used to write raw data (no ecc byte
 *               calculated and included by driver) the address passed should
 *               be phyical address and i_ecc should be set to false.
 *              -This function when used to write plain data (with ecc
 *               byte calculated and included by driver) the address passed
 *               should be logical address and i_ecc should be set to true.
 * @param[in] : i_ecc : true/false : write data with/wo ecc byte included
 *              as explained in i_writeAddress above
 *
 * @return : fapiRc
 */
fapi2::ReturnCode deviceWrite(SpiControlHandle& i_handle,
                              uint32_t i_writeAddress,
                              uint32_t i_writeLength,
                              void *i_buffer,
                              bool i_ecc);

/* @brief : This API is used to perform read from device
 *
 * @param[in] : i_handle : SPI controller handle
 * @param[in] : i_readAddress : Address to read data from device
 * @param[in] : i_readLength : Length of data to read from device
 * @param[in] : i_eccStatus :  ECC byte(RAW_BYTE_ACCESS, STANDARD_ECC_ACCESS,
 *                                      DISCARD_ECC_ACCESS)
 * @param[out] : o_buffer : buffer to store the data after read
 *
 * @return : fapiRc
 */
fapi2::ReturnCode deviceRead(SpiControlHandle& i_handle,
                             const uint32_t i_readAddress,
                             const uint32_t i_readLength,
                             const SPI_ECC_CONTROL_STATUS i_eccStatus,
                             void *o_buffer
                            );

/*
 * @brief checkSignature : Check signature of partition (by looking at hash.list and secure.hdr)
 *
 * @param[in] i_imageType : image type received
 * @param[in] i_pakStartAddrInPibMem : pointer to image pak start address in incoming binary file.
 *            Valid for incoming pak file in pibmem buffer
 * @param[in] io_codeUpdateCtrlStruct struct used to get/set/use various
 *            context needed for code-update
 *
 * @return rc
 */
uint32_t checkSignature(const CU_IMAGES i_imageType,
			void *i_pakStartAddrInPibMem,
			codeUpdateCtrlStruct_t &i_codeUpdateCtrlStruct);

/*@brief - Get image hash for an image in NOR
 *
 * @param[in]  -const CU_IMAGES i_imageType :i_imageType passed to the function
 * @param[in]  -uint8_t i_Partition : passed to the function
 * @param[in]  -uint32_t i_storageDevSideSize : Storage device side size
 * @param[out] -uint8_t* o_hashArrayPtr :64-bytes image hash
 *
 * @return - success/ fail
 */
uint32_t getImageHash(const CU_IMAGES i_imageType,
                      const uint8_t i_side,
                      codeUpdateCtrlStruct_t &io_codeUpdateCtrlStruct,
                      uint8_t* o_hashArrayPtr);

/* @brief : This API is used to get side & device start address in storage device
 *
 * @param[in]  i_side : side of memory to scrub on
 * @param[in]  i_devId: device per side of memory to scrub on to get its start address
 * @param[out] o_sideStartAddress : return the device start address
 * @param[out] o_sideMaxSize : return the device max size
 *
 * @return : rc
 */
uint32_t getSideStartAddressAndSize(const uint8_t i_side,
                                    const uint8_t i_devId,
                                    uint32_t &o_sideStartAddress,
                                    uint32_t &o_sideMaxSize);

#endif //CODE_UPDATE_UTILS_H
