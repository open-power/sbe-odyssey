/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/pool.H $                               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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

#include <stddef.h>
#include <stdint.h>

#define G_BLOCKSIZE g_vectorPoolBlockSize

namespace SBEVECTORPOOL
{

// Size of a block  for a vector
extern const size_t g_vectorPoolBlockSize;

//Pool size
extern const size_t g_vectorPoolBlockCount;

extern uint8_t g_vectorPoolBuffer[] __attribute__ ((aligned (8)));

struct vectorMemPool_t
{
    size_t   refCount;
    uint8_t* const data;

    vectorMemPool_t(uint8_t* const i_buf_ptr = NULL):
        refCount(0),
        data(i_buf_ptr)
    {
    }
};

/**
  * @brief Returns memory pool block.
  *
  * @return  Memory block if available, NULL otherwise.
  */
vectorMemPool_t * allocMem();

/**
  * @brief Release memory pool block.
  *
  * @param[in] i_pool pool pointer.
  */
void releaseMem( vectorMemPool_t * i_pool );

} // namespace SBEVECTORPOOL
