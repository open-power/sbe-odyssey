/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/sramaccess/sbecmdsramaccess.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#ifndef __SBECMDSRAM_H
#define __SBECMDSRAM_H

#include <stdint.h>
#include "plat_hwp_data_stream.H"

static const uint8_t SBE_32BIT_ALIGN_FACTOR = 4;
static const uint8_t SBE_64BIT_ALIGN_FACTOR = 8;

static const uint64_t SBE_FWCTRLFLG3_FSP_ATTACHED = 0X1000000000000000;

/*
 * @brief Granule size in Bytes for 'Get from Hwp'/'Put to Hwp'
 */
static const uint32_t SBE_SRAM_GRANULE = 128;



/**
 * @brief Structure for SBE Get/Put Sram Access ChipOps (0xA403/A404)
 */
typedef struct
{
    uint32_t pervChipletId  : 16;    // Perv Chiplet Id (Supports PAUC, EQ, PCIe and Proc Target for OCC)
    uint32_t mcastBit       :  1;    // M Cast Bit (Only applicable for PAUC, QME and PCIe chiplets)
    uint32_t reserved       :  7;    // reserved
    uint32_t mode           :  8;    // Applicable for OCC & PCI
    uint32_t addressWord0;           // 32-Bit SRAM Memory address word0
    uint32_t addressWord1;           // 32-Bit SRAM Memory address word1
    uint32_t length;                 // 8 byte aligned length (multiple of 8 bytes)

    /**
     * @brief Validate the parameter
     *
     * @return sbeSecondaryResponse Secondary RC
     */
    sbeSecondaryResponse validateParams(void)
    {
        // Validating the length is 8 byte aligned and  address is 8 byte aligned
        return (((((uint64_t)addressWord0 << 32) | addressWord1) % 8)?
                    (SBE_SEC_UNALIGNED_ADDRESS_PASSED) :
                    ( (length % 8)? SBE_SEC_INVALID_LENGTH_PASSED :
                        SBE_SEC_OPERATION_SUCCESSFUL));
    }


    /**
     * @brief execute Get SRAM hardware procedure platform specific
     *
     * @param[in] i_offset SRAM offset address to read/write
     * @param[in] i_bytes Size of data to read/write in bytes
     * @param[in/out] io_data Pointer to data storage
     * @param[in] i_isGetFlag Flag to indicate the sram Access Type
     *                        true  : GetSram ChipOp
     *                        false : PutSram ChipOp
     *
     * @return FAPI_RC_SUCCESS if success, else error code
    */
    fapi2::ReturnCode executeHwp( const uint64_t i_offset,
                                  const uint32_t i_bytes,
                                  uint8_t* io_data,
                                  const bool i_isGetFlag);

}sbeSramAccessReqMsgHdr_t;


/**
  * @brief sbeGetSram : Implements SBE Get Sram ChipOp (Class:0xA4 Command:0x03)
  *
  * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
  *
  * @return    Rc from the FIFO access utility
  */
uint32_t sbeGetSram (uint8_t *i_pArg);


/**
  * @brief sbePutSram : Implements SBE Put Sram ChipOp (Class:0xA4 Command:0x04)
  *
  * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
  *
  * @return    Rc from the FIFO access utility
  */
uint32_t sbePutSram (uint8_t *i_pArg);


/**
  * @brief sbeSramAccessWrap Sram Access Wrapper function
  *
  * @param[in]  i_getStream      up-stream fifo for chip-op /
  *                              memory interface for dump
  * @param[in]  i_putStream      down-stream fifo for chip-op /
  *                              memory interface for dump
  * @param[in]  i_isGetFlag Flag to indicate the sram Access Type
  *             true  : GetSram ChipOp
  *             false : PutSram ChipOp
  *
  * @return     Rc from the FIFO access utility
  */
uint32_t sbeSramAccessWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                            fapi2::sbefifo_hwp_data_ostream& i_putStream,
                            const bool i_isGetFlag );

#endif // __SBECMDSRAM_H