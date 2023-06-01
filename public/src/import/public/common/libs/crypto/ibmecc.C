/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/crypto/ibmecc.C $        */
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
#include "ibmecc.H"

static const uint64_t ecc64_mask[] =
{
    0xFF0000E8423C0F99ull,
    0x99FF0000E8423C0Full,
    0x0F99FF0000E8423Cull,
    0x3C0F99FF0000E842ull,
    0x423C0F99FF0000E8ull,
    0xE8423C0F99FF0000ull,
    0x00E8423C0F99FF00ull,
    0x0000E8423C0F99FFull
};

uint8_t ecc64_gen(uint64_t i_data)
{
    uint8_t value = 0;

    for (int ecc_bit = 0; ecc_bit < 8; ecc_bit++ )
    {
        uint64_t scratch = ecc64_mask[ecc_bit] & i_data;

        /* The ECC bit is the LSB of the hamming weight, which
         * happens to be the XOR across all bits of the value */
        uint32_t scratch2 = scratch ^ (scratch >> 32);
        scratch2 ^= scratch2 >> 16;
        scratch2 ^= scratch2 >> 8;
        scratch2 ^= scratch2 >> 4;
        scratch2 ^= scratch2 >> 2;
        scratch2 ^= scratch2 >> 1;
        value = (value << 1) | (scratch2 & 1);
    }

    return value;
}
