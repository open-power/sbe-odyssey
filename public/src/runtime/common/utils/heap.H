/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/utils/heap.H $                      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef _HEAP_H_
#define _HEAP_H_

#include "sbefifo.H"

#define PAK_STACK_SIZE 10

class Heap {

private :

    // @brief - iv_heap_top : top/end of available heap space (highest addr available)
    uint32_t iv_heap_top;
    // @brief - iv_scratch_bottom : bottom/start of used scratch space (lower addrs), from
    // heap top to lower addresses as more scratch space is used
    uint32_t iv_scratch_bottom;
    // @brief - iv_heap_midline : top of used pakstack space, grows from heap bottom
    // to higher addresses as more pakstack space is used
    uint32_t iv_heap_midline;
    // @brief - iv_heap_bottom : bottom of available heap space (lowest addr available)
    uint32_t iv_heap_bottom;
    // @brief - iv_pak_ends : array to keep track of available paks
    uint32_t iv_pak_ends[PAK_STACK_SIZE];
    // @brief - iv_pak_count : count of how many paks are available in the pak stack
    uint8_t  iv_pak_count = 0;

    /**
     * @brief Initialize alloted heap space once to make sure we never
     * run into uninitialized memory that might give us parity errors.
     */
    void heap_init_memory(uint32_t i_ptr);

    /**
     * @brief Get a pointer to where data should be written to at the end of the pakstack
     *
     * @return An 8-byte aligned pointer to the end marker location at the end of the archive
     */
    uint32_t* getPakStackPushPtr();

    /**
     * @brief Return an address to the end of the pakstack archive
     * puskPakStack will use to find the new PAK_END marker after dequeueing
     * data directly into the heap space, and update the heap midline addr
     */
    uint32_t getPakStackEndAddr();

    /**
     * @brief Utility function to revert pakstack to previous state when aborted
     */
    void abortPakPush(uint32_t* write_start_addr);

    /**
     * @brief Constructor for the heap class, assign heap start address and size
     *
     * Making this private since this will be a singleton class.
     */
    Heap(uint32_t i_heap_start, uint32_t i_heap_size);

public :

    /**
     * @brief  Initialize the pakstack and scratch shared memory space
     */
    void initialize();

    /**
     * @brief Return a pointer to the heap midline address
     */
    uint32_t* getHeapMidlinePtr();

    /**
     * @brief   Push a pak to the pakstack from the fifo
     * Add a pak coming in via the fifo to the pak stack. After this call ends successfully,
     * the files from the incoming pak will be available to SBE code
     * via SBE_GLOBALS::embeddedArchive
     */
    uint32_t pushPakStack(sbeFifoType i_type, uint32_t i_size);

    /**
     * @brief   Pop a pak or paks from the pakstack
     * Pop a given amount of paks from the current pakstack, After this call ends successfully,
     * the file from the popped paks will NOT be available to SBE code
     * via SBE_GLOBALS::embeddedArchive
     */
    uint32_t popPakStack(uint8_t pop_count);

    /**
     * @brief Allocate scratch memory from the top of the heap space
     *
     * Scratch space is allocated from the top of the free heap space going down, and the maximum
     * total amount of memory that can be allocated is limited by the space allocated
     * by the linker (minus engdata images and so on).
     *
     * @param size Amount of bytes requested; will be rounded up to a multiple of 8
     * @return A pointer to the allocated scratch space, or
     *         NULL if not enough space was available
     */
    void *scratch_alloc(uint32_t i_size);

    /**
     * @brief Free the most recent chunk of scratch memory
     *
     * Scratch memory can only be freed in strict reverse order of allocation, so the
     * pointer to be freed must point to the most recently allocated chunk of scratch
     * memory that has not been freed yet, otherwise the function will halt the SBE.
     *
     * @param ptr The block of memory to free
     */
    void scratch_free(const void *i_ptr);

    //TODO: Currently unused and not supported.
    //This will be needed for runtime image
    /**
     * @brief Free all scratch memory
     */
    void scratch_free_all();

    /**
     * @brief Static method to get the singleton instance
     */
    static Heap& get_instance();
};

#endif /* _HEAP_H_ */
