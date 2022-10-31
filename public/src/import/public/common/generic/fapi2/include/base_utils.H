/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/base_utils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include <byteswap.h>

/// @brief Basic utility macros and functions that are independent of anything in the fapi2 namespace

/// @brief Determine the number of elements in an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace fapi2
{

/// @brief Determine the number of bits set in a constant value
constexpr int _bitCount(const uint64_t x)
{
    return x ? (_bitCount(x >> 1) + (x & 1)) : 0;
}

/// @brief Template variant of _bitCount() for legacy support
template<uint64_t V>
class bitCount
{
    public:
        static const uint8_t count = _bitCount(V);
};

/// @brief Unconditional byte swap, type-overloaded for convenience
static inline uint16_t byte_swap(const uint16_t x)
{
    return bswap_16(x);
}

/// @brief Unconditional byte swap, type-overloaded for convenience
static inline uint32_t byte_swap(const uint32_t x)
{
    return bswap_32(x);
}

/// @brief Unconditional byte swap, type-overloaded for convenience
static inline uint64_t byte_swap(const uint64_t x)
{
    return bswap_64(x);
}

/// @brief return true if the input value is nonzero and a power of two, else return false
constexpr bool is_power_of_two(const uint64_t i_value)
{
    return i_value && (!(i_value & (i_value - 1)));
}

/// @brief return a bit mask with a string of length 1 bits starting at start_bit
constexpr uint64_t bit_mask(const int start_bit, const int length)
{
    return (~0ULL >> (64 - length)) << (64 - start_bit - length);
}

}
