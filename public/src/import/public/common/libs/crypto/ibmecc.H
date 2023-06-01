/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/crypto/ibmecc.H $        */
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
#pragma once

#include <stdint.h>
#include <endian.h>

/// @brief Calculate the ECC byte for a 64bit data value
///
/// This uses the ECC algorithm used in a lot of IBM hardware units,
/// for example I2C, SPI, PIBMEM.
///
/// @param[in] i_data Data value to calculate ECC for
/// @return The ECC byte value
uint8_t ecc64_gen(uint64_t i_data);

/// @brief Calculate the ECC byte for 8 bytes of data
///
/// This wrapper provides a standardized endian interpretation of
/// the data buffer so we don't get different results between
/// platforms.
///
/// @param[in] i_data Pointer to the first of 8 bytes to calculate ECC for
/// @return The ECC byte value
static inline uint8_t ecc64_gen(const uint8_t* i_data)
{
    return ecc64_gen(be64toh(*reinterpret_cast<const uint64_t*>(i_data)));
}

// TODO if needed: check, correct
