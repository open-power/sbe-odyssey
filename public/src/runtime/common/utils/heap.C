/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/utils/heap.C $                      */
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

#include "heap.H"
#include "sbetrace.H"
#include "sbeglobals.H"
#include "sbeFifoMsgUtils.H"
#include <endian.h>
#include "fapi2.H"
#include "archive.H"

#define CANARY 0xFEEDB0B0ull

Heap& Heap::get_instance()
{
    static Heap iv_instance(
                reinterpret_cast<uint32_t>(&_heap_space_start_),
                reinterpret_cast<uint32_t>(&_heap_space_end_) -
                    reinterpret_cast<uint32_t>(&_heap_space_start_));

    return iv_instance;
}

void Heap::heap_init_memory(uint32_t i_ptr)
{
    SBE_INFO(" Initializing heap area" );
    memset((uint64_t *)i_ptr, 0, iv_heap_top - i_ptr);
}

uint32_t* Heap::getPakStackPushPtr()
{
    return reinterpret_cast<uint32_t*>(iv_heap_midline - sizeof(PakFileHeaderCore));
}

uint32_t Heap::getPakStackEndAddr()
{
    return reinterpret_cast<uint32_t>(SBE_GLOBAL->embeddedArchive.archive_end());
}

void Heap::abortPakPush(uint32_t* write_start_addr)
{
    *(write_start_addr) = PAK_END;
    iv_pak_ends[iv_pak_count] = 0;
    --iv_pak_count;
}

Heap::Heap(uint32_t i_heap_start, uint32_t i_heap_size)
{
    iv_heap_top = i_heap_start + i_heap_size;
    iv_scratch_bottom = iv_heap_top;
    iv_heap_midline = i_heap_start;
    iv_heap_bottom = i_heap_start;
}

void Heap::initialize()
{
    if(iv_pak_count != 0)
    {
        SBE_ERROR("Intializing heap object after pushPakStack() has been called, halting");
        PK_PANIC(SBE::PANIC_ASSERT);
    }
    // Move heap midline to after paks loaded by the bootloader if present
    // See if you can pull a pak from the given heap space start addr
    if ( * reinterpret_cast<uint64_t*>(iv_heap_midline) != 0)
    {
        // Walk full pak and update midline with end ptr
        SBE_INFO(" iv_heap_midline before walking pakstack to find endptr: 0x%X", iv_heap_midline);
        iv_heap_midline = getPakStackEndAddr();
        if(!iv_heap_midline)
        {
            SBE_ERROR("Existing data in the heap space during heap intialization is not of "
                    "the correct pak format. Halting");
            PK_PANIC(SBE::PANIC_ASSERT);
        }
    }
    SBE_INFO(" Available heap to start at addr: 0x%X, of size: 0x%X", iv_heap_midline,
            iv_heap_top-iv_heap_midline);
    //Initialize heap memory (after sppe_rt.pak if present)
    heap_init_memory(iv_heap_midline);

    if (iv_heap_midline == iv_heap_bottom)
    {
        // No sppe_rt.pak present when initializing the heap pakstack
        // Write a pak end marker at the start of the pakstack,
        // SBE_GLOBAL->embeddedArchive.append_file() (specifically _locate_file) will be
        // very unhappy if it doesnt find a pak magic marker
        *(getHeapMidlinePtr()) = htobe32(PAK_END);

        // Move the midline to after the magic marker for append_file to find and remove
        iv_heap_midline += sizeof(PakFileHeaderCore);
    }
}

uint32_t* Heap::getHeapMidlinePtr()
{
    return reinterpret_cast<uint32_t*>(iv_heap_midline);
}

uint32_t Heap::pushPakStack(sbeFifoType i_type, uint32_t i_size)
{
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do{
        if(iv_pak_count >= PAK_STACK_SIZE)
        {
            SBE_ERROR("Appending a pak past the exceeded pak stack size %d. Exiting!", PAK_STACK_SIZE);
            l_rc = SBE_PRI_USER_ERROR;
            break;
        }
        if(i_size + iv_heap_midline > iv_scratch_bottom)
        {
            SBE_ERROR("Not enough room to push pak into heap space. Exiting");
            l_rc = SBE_SEC_HEAP_SPACE_FULL_FAILURE;
            break;
        }

        // Append the previous midline address to the iv_pak_ends array
        iv_pak_ends[iv_pak_count] = iv_heap_midline;
        SBE_INFO(" PUSH: Before push, appending iv_pak_ends array at index (iv_pak_count): %d, "
                "with heap_midline: 0x%X", iv_pak_count, iv_heap_midline);
        // Increment iv_pak_count by 1
        ++iv_pak_count;

        uint32_t* write_start_addr = getPakStackPushPtr();
        SBE_INFO(" PUSH: Payload length [%d] words, [0x%X] bytes, write_start_addr: 0x%X", i_size,
                i_size*4, write_start_addr);
        l_rc = sbeUpFifoDeq_mult(i_size, write_start_addr, true, false, i_type);
        if(l_rc)
        {
            SBE_ERROR("Failure while dequeueing fifo into pakspace. Rewriting PAK_END at previous "
                    "pak end and exiting");
            abortPakPush(write_start_addr);
            break;
        }

        // Update midline ptr to be the same as the archive.archive_end()
        // archive_end() returns the address AFTER the PAK_END marker
        uint32_t end_ptr = getPakStackEndAddr();

        if(reinterpret_cast<uint32_t>(write_start_addr) + (i_size * 4) != end_ptr)
        {
            SBE_ERROR("Pushed pak was not of the correct pak format. Please review pak data for "
                    "potential corruption. Rewriting PAK_END at previous pak end and exiting");
            abortPakPush(write_start_addr);
            l_rc = SBE_SEC_GENERIC_BAD_PAK;
            break;
        }
        iv_heap_midline = end_ptr;
        SBE_INFO(" PUSH: New heap_midline: 0x%X", iv_heap_midline);
    } while(0);
    return l_rc;
}

uint32_t Heap::popPakStack(uint8_t pop_count)
{
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do{
        if(pop_count > iv_pak_count)
        {
            SBE_ERROR("Attempting to pop more paks than in the current pakstack. pop_count: %d, "
                    "iv_pak_count: %d", pop_count, iv_pak_count);
            l_rc = SBE_SEC_POP_UNAVAILABE_PAKS;
            break;
        }
        iv_pak_count = iv_pak_count - pop_count;

        // Revert iv_heap_midline to previous pak end
        iv_heap_midline = iv_pak_ends[iv_pak_count];
        SBE_INFO(" POP: Previous end tile ptr, now the new midline - 0x%X, iv_pak_count: %d",
                iv_heap_midline, iv_pak_count);

        // Write a PAK_END marker to the end of the pakstack
        *(getHeapMidlinePtr() - 2) = htobe32(PAK_END);
        *(getHeapMidlinePtr() - 1) = iv_heap_midline - iv_heap_bottom;

        uint32_t end_ptr = getPakStackEndAddr();
        SBE_INFO(" POP: getPakStackEndAddr 0x%X", end_ptr);

        if(!end_ptr || iv_heap_midline != end_ptr)
        {
            SBE_ERROR("PakStackEndPtr does not match heap midline. Something went wrong.");
            PK_PANIC(SBE::PANIC_ASSERT);
        }
    } while(0);
    return l_rc;
}

void* Heap::scratch_alloc(uint32_t i_size)
{
    const uint32_t rounded_size = (i_size + 7) & ~7;
    const uint32_t new_bottom = iv_scratch_bottom - rounded_size - 8;
    // The midline, determined by the pakstack usage, is the limit on what can be allocated
    if (new_bottom < iv_heap_midline)
    {
        SBE_ERROR("Out of scratch space. rounded_size=0x%X limit=0x%X bottom=0x%X",
                  rounded_size, iv_heap_midline, iv_scratch_bottom);
        return NULL;
    }

    SBE_INFO(" scratch_alloc: rounded_size=0x%X limit=0x%X old_bottom=0x%X new_bottom=0x%X",
              rounded_size, iv_heap_midline, iv_scratch_bottom, new_bottom);

    uint64_t new_bottom_value = CANARY << 32 | iv_scratch_bottom;
    *(volatile uint64_t *)new_bottom = new_bottom_value;
    iv_scratch_bottom = new_bottom;
    return (void *)(new_bottom + 8);
}

void Heap::scratch_free(const void *i_ptr)
{

    if(i_ptr != NULL)
    {
        uint32_t old_bottom = (uintptr_t)i_ptr - 8;
        if (old_bottom != iv_scratch_bottom)
        {
            SBE_ERROR("scratch_free: Attempted to free a scratch block that is not at the bottom. "
                    "ptr=%p bottom=0x%X", i_ptr, iv_scratch_bottom);
            PK_PANIC(SBE::PANIC_ASSERT);
        }

        uint64_t header = *(uint64_t *)old_bottom;
        if (header >> 32 != CANARY)
        {
            SBE_ERROR("scratch_free: Block header corrupted, halting. ptr=%p header=0x%08X%08X",
                    i_ptr, header >> 32, header & 0xFFFFFFFF);
            PK_PANIC(SBE::PANIC_ASSERT);
        }

        uint32_t new_bottom = header & 0xFFFFFFFF;
        SBE_DEBUG("scratch_free: ptr=%p old_bottom=0x%X new_bottom=0x%X", i_ptr, iv_scratch_bottom,
                new_bottom);
        iv_scratch_bottom = new_bottom;
    }
    else
    {
        SBE_ERROR("scratch_free : Null pointer passed. Nothing to free...");
    }
}

void Heap::scratch_free_all()
{
    iv_scratch_bottom = iv_heap_top;
}

size_t Heap::getFreeHeapSize()
{
    size_t freeHeapSpace = iv_scratch_bottom - iv_heap_midline;
    if(freeHeapSpace > 8)
    {
        freeHeapSpace -= 0x8;
        SBE_DEBUG("Available free heap space is 0x%08x", freeHeapSpace);
    }
    else
    {
        freeHeapSpace = 0x0;
        SBE_INFO("No free heap space");
    }

    return freeHeapSpace;
}
