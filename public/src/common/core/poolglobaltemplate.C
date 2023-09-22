/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/poolglobaltemplate.C $                 */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2019,2023                        */
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

// This is a source code template for defining pool global variables in the project
// code. This file will not be compiled independently.
// Each project should have a source file as shown below and should compile it.

/*
<project>/core/poolglobal.C
---------------------------

#define VECTOR_POOL_BLOCK_SIZE  1024
#define VECTOR_POOL_BLOCK_COUNT 8

#include "poolglobaltemplate.C"

*/

#include "pool.H"

namespace SBEVECTORPOOL
{
// Size of a block  for a vector
const size_t g_vectorPoolBlockSize = VECTOR_POOL_BLOCK_SIZE;

//Pool count
const size_t g_vectorPoolBlockCount = VECTOR_POOL_BLOCK_COUNT;

uint8_t g_vectorPoolBuffer[g_vectorPoolBlockCount * g_vectorPoolBlockSize];

vectorMemPool_t g_vectorPool[g_vectorPoolBlockCount];
}
