/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/spiaccess/sbecmdspi.H $     */
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
/*
 * @file: public/src/runtime/common/chipops/spiaccess/sbecmdspi.H
 *
 * @brief This file contains the Interfaces for the SCOM Access chip-ops
 *
 */

#ifndef __SBEFW_SBECMDSPI_H
#define __SBEFW_SBECMDSPI_H

#include <stdint.h>
#include "spi_base.H"
#include "spi_memory.H"
#include "plat_hwp_data_stream.H"
#include <fapi2.H>

#define WITH_ECC(x)                     (((x)*9)/8)

const uint32_t MAX_BUFFER_SIZE = 0x10000; // 64KB - In bytes always

/**
 * @brief Spi Write Command:
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    RC from the FIFO access utility
 */
uint32_t sbeSpiWrite(uint8_t *i_pArg);

/**
  * @brief spiSpiWriteWrap:  Write data from Upstream FIFO into SEEPROM via SPI driver
  *
  * @param[in]  i_getStream      fifo down stream interface
  * @param[in]  i_putStream      fifo up stream interface
  *
  * @return    Rc from the FIFO access utility
  */
uint32_t sbeSpiWriteWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                         fapi2::sbefifo_hwp_data_ostream& i_putStream);

typedef struct
{
    uint16_t reserved1;
    uint8_t reserved2;
    uint8_t boot_side;
    uint32_t start_addr; // start_addr into SEEPROM to begin writing
} sbeSpiWriteMsgHdr_t;

/**
 * @brief Spi Read Command:
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    RC from the FIFO access utility
 */
uint32_t sbeSpiRead(uint8_t *i_pArg);

/**
  * @brief spiSpiReadWrap:  Read data from SEEPROM via SPI driver and output to Upstream FIFO
  *
  * @param[in]  i_getStream      fifo down stream interface
  * @param[in]  i_putStream      fifo up stream interface
  *
  * @return    Rc from the FIFO access utility
  */

uint32_t sbeSpiReadWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                         fapi2::sbefifo_hwp_data_ostream& i_putStream);

typedef struct
{
    uint8_t reserved1;
    uint8_t boot_side;
    uint16_t len; // length of seeprom to read in bytes
    uint32_t start_addr; // start address into SEEPROM to begin reading
} sbeSpiReadMsgHdr_t;

#endif /* __SBEFW_SBECMDSPI_H */
