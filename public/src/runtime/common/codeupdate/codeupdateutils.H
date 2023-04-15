/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/codeupdate/codeupdateutils.H $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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

using namespace fapi2;

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

    uint8_t reserved[3];
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
 * @param[in]  : i_side
 * @param[out] : o_sideStartAddress used to pass side start address
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
 * @param[in] : i_writeAddress : address to write buffer data.
 *              This is logical address that may (i_ecc=true)/may not
 *              (i_ecc=false) be converted to physical address (incl ECC)
 *              by driver during write opr.
 * @param[in] : i_ecc : true/false : write data with/wo ecc included
 *
 * @return : fapiRc
 */
fapi2::ReturnCode deviceWrite(SpiControlHandle& i_handle,
                              uint32_t i_writeAddress,
                              uint32_t i_writeLength,
                              void *i_buffer,
                              bool i_ecc);

#endif //CODE_UPDATE_UTILS_H
