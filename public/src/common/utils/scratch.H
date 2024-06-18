/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/scratch.H $                           */
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

#ifndef _SCRATCH_H_
#define _SCRATCH_H_

#include "sbetrace.H"

#define CANARY 0xFEEDB0B0ull

class scratch {

private :

uint32_t iv_scratch_bottom_init;
uint32_t iv_scratch_limit;
uint32_t iv_scratch_bottom;

/**
 * @brief Initialize all available scratch space once to make sure we never
 * run into uninitialized memory that might give us parity errors.
 *
 * This must be run after the scratch limit is final lest we overwrite
 * an embedded archive, so we defer this step until the first scratch
 * allocation.
 */
void scratch_init_memory()
{
    SBE_INFO("Initilizing scratch area" );

    for (uint64_t *ptr = (uint64_t *)iv_scratch_limit; ptr < (uint64_t *)iv_scratch_bottom_init; ptr++)
    {
        *ptr = 0;
    }
}

public :

scratch(uint32_t scratch_start, uint32_t scratch_size)
{
    iv_scratch_bottom_init = scratch_start + scratch_size;
    iv_scratch_limit = (uintptr_t)scratch_start;
    iv_scratch_bottom = iv_scratch_bottom_init;

    //Initilize scratch memory
    scratch_init_memory();
}

/**
 * @brief Allocate scratch memory from the top of PIBMEM
 *
 * Scratch space is allocated from the top of PIBMEM going down, and the maximum
 * total amount of memory that can be allocated is limited by the space allocated
 * by the linker (minus engdata images and so on).
 *
 * There is a corresponding scratch_free() function but all scratch space will
 * also implicitly be freed all at once at the end of each chipop or istep.
 *
 * @param size Amount of bytes requested; will be rounded up to a multiple of 8
 * @return A pointer to the allocated scratch space, or
 *         NULL if not enough space was available
 */
void *scratch_alloc(uint32_t size)
{
    const uint32_t rounded_size = (size + 7) & ~7;
    const uint32_t new_bottom = iv_scratch_bottom - rounded_size - 8;
    if (new_bottom < iv_scratch_limit)
    {
        SBE_ERROR("Out of scratch space! rounded_size=0x%X limit=0x%X bottom=0x%X",
                  rounded_size, iv_scratch_limit, iv_scratch_bottom);
        return NULL;
    }

    SBE_INFO("scratch_alloc: rounded_size=0x%X limit=0x%X old_bottom=0x%X new_bottom=0x%X",
              rounded_size, iv_scratch_limit, iv_scratch_bottom, new_bottom);

    uint64_t new_bottom_value = CANARY << 32 | iv_scratch_bottom;
    *(volatile uint64_t *)new_bottom = new_bottom_value;
    iv_scratch_bottom = new_bottom;
    return (void *)(new_bottom + 8);
}

/**
 * @brief Free the most recent chunk of scratch memory
 *
 * Scratch memory can only be freed in strict reverse order of allocation, so the
 * pointer to be freed must point to the most recently allocated chunk of scratch
 * memory that has not been freed yet, otherwise the function will halt the SBE.
 *
 * @param ptr The block of memory to free
 */
void scratch_free(const void *ptr)
{
    uint32_t old_bottom = (uintptr_t)ptr - 8;
    if (old_bottom != iv_scratch_bottom)
    {
        SBE_ERROR("scratch_free: Attempted to free a scratch block that is not at the bottom, halting! "
                  "ptr=%p bottom=0x%X", ptr, iv_scratch_bottom);
        PK_PANIC(SBE::PANIC_SCRATCH);
    }

    uint64_t header = *(uint64_t *)old_bottom;
    if (header >> 32 != CANARY)
    {
        SBE_ERROR("scratch_free: Block header corrupted, halting! ptr=%p header=0x%08X%08X",
                  ptr, header >> 32, header & 0xFFFFFFFF);
        PK_PANIC(SBE::PANIC_SCRATCH);
    }

    uint32_t new_bottom = header & 0xFFFFFFFF;
    SBE_DEBUG("scratch_free: ptr=%p old_bottom=0x%X new_bottom=0x%X", ptr, iv_scratch_bottom, new_bottom);
    iv_scratch_bottom = new_bottom;
}

//TODO: Currently unused and not supported.
//This will be needed for runtime image
/**
 * @brief Free all scratch memory
 *
 * Must be called by chipop and istep handlers at the end of each operation
 */
void scratch_free_all()
{
    iv_scratch_bottom = iv_scratch_bottom_init;
    //SBEVECTORPOOL::clear_scratch_pool();
}

/**
 * @brief Set a new scratch space limit
 *
 * The scratch space limit is the lowest address that may still be allocated to
 * scratch space - so raising the limit will reduce the amount of available
 * scratch space.
 *
 * The limit is initialized to the end of linker allocated space, but can be
 * set higher to account for appended data like engineering data.
 *
 * @param limit The new limit - the lowest address that may still be used for scratch space
 */
void scratch_set_limit(uint32_t limit)
{
    iv_scratch_limit = (limit + 7) & ~7;
}

};

#endif /* _SCRATCH_H_ */
