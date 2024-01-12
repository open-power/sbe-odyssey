/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/pool.C $                               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2024                        */
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

#include <pool.H>
#include "assert.h"
#include <new>
#include "pk_api.h"

namespace SBEVECTORPOOL
{

vectorMemPool_t * allocMem()
{
    vectorMemPool_t *pool = NULL;
    size_t idx;
    PkMachineContext    ctx;

    //The following operations must be done atomically
    pk_critical_section_enter(&ctx);

    for( idx = 0; idx < g_vectorPoolBlockCount; idx++ )
    {
        if( 0 == g_vectorPool[idx].refCount )
        {
            pool = g_vectorPool + idx;
            new (pool) vectorMemPool_t((uint8_t*)(g_vectorPoolBuffer + idx * g_vectorPoolBlockSize));
            pool->refCount++;
            break;
        }
    }

    //exit the critical section
    pk_critical_section_exit(&ctx);

    if(NULL == pool )
    {
        SBE_ERROR("NULL pool idx:%u", idx);
        pk_halt();
    }
    return pool;
}

void releaseMem( vectorMemPool_t * i_pool )
{
    PkMachineContext    ctx;

    do
    {
        if ( NULL == i_pool )   break;

        // Assert here.  This pool was not supposed to be in use.
        assert( 0 != i_pool->refCount )
        SBE_DEBUG(" Releasing pool 0x%08X", i_pool);

        //The following operations must be done atomically
        pk_critical_section_enter(&ctx);

        i_pool->refCount--;
        SBE_DEBUG(" In releaseMem() RefCount:%u", i_pool->refCount);

        //exit the critical section
        pk_critical_section_exit(&ctx);
    }while(0);
}

} // namespace SBEVECTORPOOL
