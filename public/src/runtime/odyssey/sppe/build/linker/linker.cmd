/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/build/linker/linker.cmd $     */
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

// Need to do this so that elf32-powerpc is not modified!
#undef powerpc

//TODO:This is a basic linker.Update as per needs/design

#include "odysseylink.H"
#define INITIAL_STACK_SIZE 256

OUTPUT_FORMAT(elf32-powerpc);

MEMORY
{
 sram(rw)  : ORIGIN = SRAM_ORIGIN, LENGTH = SRAM_SIZE
}

SECTIONS
{
    . = SRAM_ORIGIN;

    //pk_vector should be at top of pibmem memory
    .pk_vectors . : {
      KEEP(*(.vectors));
    } > sram

    ////////////////////////////////
    // Read-only Data
    ////////////////////////////////

    . = ALIGN(8);
    _RODATA_SECTION_BASE = .;

   .text . : {
       *(.text)

        . = ALIGN(8);
        ctor_start_address = .;
        *(.ctors) *(.ctors.*)
        ctor_end_address = .;
        *(rodata*) *(.got2) } > sram

    ////////////////////////////////
    // Read-write Data
    ////////////////////////////////


    // SDA2 constant sections .sdata2 and .sbss2 must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets.

    . = ALIGN(8);

   .sdata2 . : { *(.sdata2*) } > sram
    _SDA2_BASE_ = .;
   .sbss2  . : { *(.sbss2*) } > sram

    . = ALIGN(8);
    _DATA_SECTION_BASE = .;

    // SDA sections .sdata and .sbss must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets.

    .sdata  . : { *(.data*) *(.sdata*)  } > sram

    . = ALIGN(8);

    _SDA_BASE_ = .;

    . = ALIGN(8);

    _sbe_image_size = . - SRAM_ORIGIN;

    _sbss_start = .;
    .sbss   . : { *(.sbss*) *(.bss*)  } > sram

    . = ALIGN(8);

    _sbss_end = .;

    . = ALIGN(8);
   _PK_INITIAL_STACK_LIMIT = .;
   . = . + INITIAL_STACK_SIZE;
   _PK_INITIAL_STACK = . - 1;
}
