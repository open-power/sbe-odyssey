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

// Need to do this so that elf32-powerpc is not modified!
#undef powerpc

//TODO:This is a basic linker.Update as per needs/design

#include "odysseylink.H"
#define INITIAL_STACK_SIZE 256
#define BASE_LOADER_STACK_SIZE 65536

OUTPUT_FORMAT(elf32-powerpc);

MEMORY
{
 rom(rx)   : ORIGIN = SROM_ORIGIN, LENGTH = SROM_SIZE
 sram(rw)  : ORIGIN = SRAM_ORIGIN, LENGTH = SRAM_SIZE
}

SECTIONS
{
    . = SROM_ORIGIN;
    _origin = .;

    //pkVector should be at top of pibmem memory
    .pkVectors . : {
      _pkVectors_origin = .; _pkVectors_offset = . - _origin;
      KEEP(*(.vectors));
    } > rom
    _pkVectors_size = . - _pkVectors_origin;

    ////////////////////////////////
    // Read-only Data
    ////////////////////////////////

    . = ALIGN(8);
    _RODATA_SECTION_BASE = .;

   .text . : { *(.text) } > rom
   .data  . : { *(.data) } > rom

    // SDA2 constant sections .sdata2 and .sbss2 must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets.

    _SDA2_BASE_ = .;
   .sdata2 . : { *(.sdata2*) } > rom
   .sbss2  . : { *(.sbss2*) } > rom

    // Other read-only data.

    . = ALIGN(8);
    .rodata . : { ctor_start_address = .;
                  *(.ctors) *(.ctors.*)
                  ctor_end_address = .;
                  *(rodata*) *(.got2) } > rom

    _RODATA_SECTION_SIZE = . - _RODATA_SECTION_BASE;

    _sbe_image_size = _RODATA_SECTION_SIZE;

    ////////////////////////////////
    // Read-write Data
    ////////////////////////////////

    . = SRAM_ORIGIN;

    . = ALIGN(8);
    _DATA_SECTION_BASE = .;

    // SDA sections .sdata and .sbss must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets.

    _SDA_BASE_ = .;
    .sdata  . : { *(.sdata*)  } > sram
    _sbss_start = .;
    .sbss   . : { *(.sbss*)   } > sram
    _sbss_end = .;

    // Other read-write data
    // It's not clear why boot.S is generating empty .glink,.iplt

   .rela   . : { *(.rela*) } > sram
   .rwdata . : { *(.data*) *(.bss*) } > sram

   //_BASE_LOADER_STACK_LIMIT = .end;
   //_BASE_LOADER_STACK_LIMIT = . + BASE_LOADER_STACK_SIZE - 1;

    . = ALIGN(8);
   _PK_INITIAL_STACK_LIMIT = .;
   . = . + INITIAL_STACK_SIZE;
   _PK_INITIAL_STACK = . - 1;

    . = ALIGN(8);
    _loader_end = . - 0;
}
