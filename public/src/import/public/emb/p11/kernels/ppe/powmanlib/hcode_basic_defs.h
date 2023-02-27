/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/hcode_basic_defs.h $ */
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
#include <stdint.h>

#define UNDEFINED_MAX_8   (uint8_t )0xff
#define UNDEFINED_MAX_16  (uint16_t)0xffff
#define UNDEFINED_MAX_32  (uint32_t)0xffffffff
#define UNDEFINED_MAX_64  (uint64_t)0xffffffffffffffff
#define UNDEFINED_ZERO    (uint32_t)0x00000000

static inline uint16_t endian_swap16( uint16_t i_data)
{
    return (i_data >> 8 | i_data << 8);
}

#if 0
void memcpy64( uint64_t* i_dst, uint64_t* i_src, uint32_t i_bytesize);

void memcpy64( uint64_t* i_dst, uint64_t* i_src, uint32_t i_bytesize)
{
    uint32_t i;
    uint32_t dw_size = i_bytesize / 8;

    if(((i_bytesize | (uint32_t)(i_dst) | (uint32_t)(i_src)) & 0x7) != 0)
    {
        PK_TRACE("memcpy64 Attempt to copy un-aligned memory");
        IOTA_PANIC(MEMCPY64_DATA_NOT_ALIGNED);
    }

    for(i = 0; i < dw_size; ++i)
    {
        i_dst[i] = i_src[i];
    }
}
#endif
