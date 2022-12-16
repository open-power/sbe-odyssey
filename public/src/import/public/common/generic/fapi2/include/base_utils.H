/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/base_utils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#include <endian.h>

/// @brief Basic utility macros and functions that are independent of anything in the fapi2 namespace

/// @brief Determine the number of elements in an array
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace fapi2
{

/// @brief Determine the number of bits set in a constant value
constexpr unsigned int _bitCount(const uint64_t x)
{
    return x ? static_cast<unsigned int>(_bitCount(x >> 1u) + (x & 1u)) : 0u;
}

/// @brief Template variant of _bitCount() for legacy support
template<uint64_t V>
class bitCount
{
    public:
        static const uint8_t count = static_cast<uint8_t>(_bitCount(V));
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
struct uint8be_t
{
    uint8_t _value;
    inline uint8_t get() const
    {
        return _value;
    }
    inline void set(uint8_t value)
    {
        _value = value;
    }
};

///@brief For storing common types big-endian
struct uint16be_t
{
    uint16_t _value;
    inline uint16_t get() const
    {
        return be16toh(_value);
    }
    inline void set(uint16_t value)
    {
        _value = htobe16(value);
    }
};

struct uint32be_t
{
    uint32_t _value;
    inline uint32_t get() const
    {
        return be32toh(_value);
    }
    inline void set(uint32_t value)
    {
        _value = htobe32(value);
    }
};

struct uint64be_t
{
    uint64_t _value;
    inline uint64_t get() const
    {
        return be64toh(_value);
    }
    inline void set(uint64_t value)
    {
        _value = htobe64(value);
    }
};

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
