/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/odyssey/bldr/build/linker/linker.cmd $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
#define MINIMUM_STACK_SIZE 256
#define DOUBLE_WORD_SIZE   8

OUTPUT_FORMAT(elf32-powerpc);

MEMORY {
    sram    :   ORIGIN = BOOTLOADER_ORIGIN, LENGTH = BOOTLOADER_SIZE
}

SECTIONS {
    /* Text, read only data and other permanent read-only sections. */
    . = BOOTLOADER_ORIGIN;
    _bootloader_start = .;

    /* Vectors should be at top of memory. */
    .pk_vectors . : {
        KEEP(*(.vectors));
    }

    . = BOOTLOADER_ORIGIN + VECTOR_SIZE;
    .bldr_metadata . : {
        KEEP(*(.bldr_metadata));
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
    _code_size = . - BOOTLOADER_ORIGIN;

    /* Read-write data */

    /* Small data area 2 sections .sdata and .sbss must be adjacent to each
     * other.
     */
    _sda2_start = .;
    .sdata2 . : {
        *(.sdata2)
        *(.sdata2.*)
        . = ALIGN(DOUBLE_WORD_SIZE);
    }
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
        _ram_consumed = . - _bootloader_start;

        /* Assert if more RAM consumed than allocated. */
        ASSERT ((_ram_consumed < BOOTLOADER_SIZE), "Error: Not enough RAM \
        space.");
    }

    /* Image size will contain code segment and initialised data sections. */
    _sbe_image_size = _code_size + _data_size + _sdata_size + _sdata2_size;

    /* Stack segment */
    PROVIDE (_stack_size = BOOTLOADER_SIZE - _ram_consumed - HASH_LIST_SIZE);
    .stack . :
    {
        /* Assert if stack space less than MINIMUM_STACK_SIZE. */
        ASSERT ((_stack_size > MINIMUM_STACK_SIZE), "Error: Not enough stack \
        space.");
        . = ALIGN(DOUBLE_WORD_SIZE);
        _PK_INITIAL_STACK_LIMIT = .;
        . = . + _stack_size;
        _PK_INITIAL_STACK = . - 1;
    }

    /* Hash List*/
    . = HASH_LIST_START_OFFSET;
    /* Hash List should be at constant location i.e end of sram. */
    .hashlist . : {
        KEEP(*(.hash_list));
    }

    .g_cross_image_data TRACE_BUF_PTR_OFFSET : {
        *(.g_pk_trace_buf)
        *(.g_metadata_ptr)
    }
}
