/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/build/linker/linker.cmd $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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

 /* Need to do this so that elf32-powerpc is not modified! */
#undef powerpc

#include "odysseylink.H"

#ifndef INITIAL_STACK_SIZE
#define INITIAL_STACK_SIZE 256
#endif
#define DOUBLE_WORD_SIZE   8

OUTPUT_FORMAT(elf32-powerpc);

MEMORY {
    sram    :   ORIGIN = SPPE_ORIGIN, LENGTH = SPPE_MAX_SIZE
}

SECTIONS {
    /* Text, read only data and other permanent read-only sections. */
    . = SPPE_ORIGIN;
    _sppe_start = .;

    /* Vectors should be at top of memory. */
    .pk_vectors . : {
        KEEP(*(.vectors));
    }

    . = SPPE_ORIGIN + VECTOR_SIZE;
    .sppe_metadata . : {
        KEEP(*(.sppe_metadata));
    }

    /* Text section */
   .text . : {
        *(.text)
        *(.text.*)
        *(.dtors)
        *(.dtors.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
    }

   /* Other read-only data */
   .rodata . :
   {
        ctor_start_address = .;
        KEEP(*(.ctors));
        KEEP(*(.ctors.*));
        ctor_end_address = .;
        KEEP(*(.rodata*));
    }
    . = ALIGN(DOUBLE_WORD_SIZE);
    _code_size = . - SPPE_ORIGIN;

    /* Read-write data */

    /* Small data area 2 sections .sdata and .sbss must be adjacent to each
     * other.
     */
    _sda2_start = .;
    .sdata2 . : {
        // Used for dumping attributes
        _attrs_start_ = .;
        KEEP(*(.attrs))
        KEEP(*(.attrs.*))
        // Used for dumping attributes
        _attrs_end_ = .;

        *(.sdata2)
        *(.sdata2.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
    }
    _attrs_size_ = . - _attrs_start_;

    /* Size of initialised data section i.e. .sdata2 */
    _sdata2_size = . - _sda2_start;

    _sbss2_start = .;
    .sbss2 . : {
        *(.sbss2)
        *(.sbss2.*)
        ASSERT ((_sbss2_start == .), "Error: Small data area 2 containing \
        uninitialized data.");
    }
    . = ALIGN(DOUBLE_WORD_SIZE);

    /* _SDA2_BASE_ should point to the center of SDA2, so that whole
     * SDA2 can be addressed with 16-bit signed offsets.
     */
    _SDA2_BASE_ = _sda2_start + ((. - _sda2_start) / 2);

    /* Data section. */
    _data_start = .;
    .data . : {
        *(.data)
        *(.data.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
    }
    /* Size of initialised data section i.e. .data */
    _data_size = . - _data_start;

    /* SDA sections .sdata and .sbss must be adjacent to each
     * other.
     */
    _sda_start = .;

    .sdata . : {
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
    }
    /* Size of initialised data section i.e. .sdata */
    _sdata_size = . - _sda_start;

    /* Start of sbss section is also the end of data section. */
    _sbss_start = .;
    .sbss . : {
        *(.sbss)
        *(.sbss.*)
    }
    . = ALIGN(DOUBLE_WORD_SIZE);

    /* _SDA_BASE_ should point to the center of SDA, so that whole SDA
     * can be addressed with 16-bit signed offsets.
     */
    _SDA_BASE_ = _sda_start + ((. - _sda_start) / 2);

    /* BSS section. */
    .bss . : {
        *(.bss)
        *(.bss.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
        /* Total uninitialised data is .sbss + .bss sections. */
        _bss_end = .;
        _ram_consumed = . - _sppe_start;
    }

    /* Image size will contain code segment and initialised data sections. */
    _sbe_image_size = _code_size + _data_size + _sdata_size + _sdata2_size;

    /* Stack segment */
    .stack . :
    {
        . = ALIGN(DOUBLE_WORD_SIZE);
        _PK_INITIAL_STACK_LIMIT = .;
        . = . + INITIAL_STACK_SIZE;
        _PK_INITIAL_STACK = . - 1;
    }
    . = ALIGN(DOUBLE_WORD_SIZE);
    /*
     * Pibmem address to the beginning of the heap space
     * Ultimately separated into two spaces inside of the heap:
     *     pak stack: the pak stack grows from the bottom upwards (low addr to high addr)
     *         holds all paks
     *     scratch: allocations grow from the top downwards (high addr to low addr)
     *         temporariy holds decompressed images
     *         also can be used for other FW use-cases like storing the training data from dram/omi
     * 1. Loader has to put sppe_rt.pak file to the start of this heap space.
     * 2. When this fw trying to decompress some Image, it can use this scratch area
     *    for decompressing.
     * 3. pushpop chipop will push the incoming pak files to the pak stack area.
     */
    _heap_space_start_ = .;
    _heap_space_end_ = SPPE_TRACE_START_OFFSET;
    _heap_space_size_ = _heap_space_end_ - _heap_space_start_;

    /* Hash List*/
    g_hash_list = HASH_LIST_START_OFFSET;

    .g_cross_image_data TRACE_BUF_PTR_OFFSET : {
        *(.g_pk_trace_buf)
        *(.g_metadata_ptr)
    }
}
