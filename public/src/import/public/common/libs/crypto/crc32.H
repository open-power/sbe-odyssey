/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/crypto/crc32.H $         */
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
/// @file crc32.H
/// @brief Implement crc32
//
// *HWP HWP Owner: Ben Gass bgass@us.ibm.com
// *HWP FW Owner: Daniel Crowell dcrowell@us.ibm.com
// *HWP Team: Nest
// *HWP Level: 3
// *HWP Consumed by: HB
//

#pragma once

#include <stdint.h>
#include <vector>
#include <algorithm>

// CRC32 lookup table to match the table used in Microchip firmware
// Note that this table is known as CRC32_BZIP2 in some texts
extern const uint32_t crc32_lookup_table[];

///
/// @brief Generates a CRC on a range of data
/// @param[in] i_data data on which to generate the CRC32
/// @param[in] i_len the length of the data
/// @return the CRC32 for the input data
///
uint32_t crc32_gen(const uint8_t* i_data, uint64_t i_len);

///
/// @brief Generates a CRC on a vector of data
/// @param[in] i_data data on which to generate the CRC32
/// @param[in] i_len allows the user to specify a length of data to generate CRC - used for response structure - defaulted to max (use whole array)
/// @return the CRC32 for the inputted vector
///
static inline uint32_t crc32_gen(const std::vector<uint8_t>& i_data, const uint64_t i_len = ~0)
{
    return crc32_gen(i_data.data(), std::min(i_data.size(), i_len));
}
