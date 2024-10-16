/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/odyssey_ioo_link.cmd $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2019
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : link_ioo.cmd
// *! TITLE       :
// *! DESCRIPTION : Linker command script for IO PPE image
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr22040700 |vbr     | Updated for 5->9 threads
// vbr22021600 |vbr     | Increase SRAM from 80K to 96K
// vbr21011300 |vbr     | Increased fw_regs to 32B
// vbr20111700 |vbr     | Added wildcards for the sections as recommended by Doug Gilbert.
// vbr19072400 |vbr     | Increase SRAM from 64K to 80K and add 1K debug log
//------------------------------------------------------------------------------

// Need to do this so that elf32-powerpc is not modified!
#undef powerpc

// This is the size of the stack for main() and immediately follows the code & global data
#define INITIAL_STACK_SIZE 512

OUTPUT_FORMAT(elf32-powerpc);

MEMORY
{
 // 96KB RAM on IOO/IOT
 sram : ORIGIN = 0xFFFE0000, LENGTH = 0x18000
}

SECTIONS
{
    // '.' is the current output location counter
    . = 0xfffe0000;

    // Code
    .text          : {. = ALIGN(512); *(.vectors*) *(.text*)} > sram

    ////////////////////////////////
    // Read-only Data
    ////////////////////////////////

    . = ALIGN(8);
    _RODATA_SECTION_BASE = .;

    // SDA2 constant sections .sdata2 and .sbss2 must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets. 

    _SDA2_BASE_ = .;
   .sdata2 . : { *(.sdata2*) } > sram
   .sbss2  . : { *(.sbss2*) } > sram

   // Other read-only data.  

   .rodata . : { *(.rodata*) *(.got2*) } > sram

    _RODATA_SECTION_SIZE = . - _RODATA_SECTION_BASE;

    ////////////////////////////////
    // Read-write Data
    ////////////////////////////////

    . = ALIGN(8);
    _DATA_SECTION_BASE = .;

    // SDA sections .sdata and .sbss must be adjacent to each
    // other.  Our SDATA sections are small so we'll use strictly positive
    // offsets. 

    _SDA_BASE_ = .;
    .sdata  . : { *(.sdata*)  } > sram
    .sbss   . : { *(.sbss*)   } > sram

    // Other read-write data
    // It's not clear why boot.S is generating empty .glink,.iplt

   .rela   . : { *(.rela*) } > sram
   .rwdata . : { *(.data*) *(.bss*) } > sram
//   .iplt . : { *(.iplt*) } > sram

   _PK_INITIAL_STACK_LIMIT = .;
   . = . + INITIAL_STACK_SIZE;
   _PK_INITIAL_STACK = . - 1;

   // The sizes of the following statically allocated sections must match the sizes defined in the c code.

   // Reserve 32B for general configuration and status of the image
   .imgdata     0xffff30c0: {  _img_regs_start = .; . = . + 0x20; _img_regs_end = .; } > sram = 0

   // Reserve 16B per thread of data for interaction with firmware
   .iodatafw0   0xffff30e0: {  _fw_regs0_start = .; . = . + 0x20; _fw_regs0_end = .; } > sram = 0
   .iodatafw1   0xffff3100: {  _fw_regs1_start = .; . = . + 0x20; _fw_regs1_end = .; } > sram = 0
   .iodatafw2   0xffff3120: {  _fw_regs2_start = .; . = . + 0x20; _fw_regs2_end = .; } > sram = 0
   .iodatafw3   0xffff3140: {  _fw_regs3_start = .; . = . + 0x20; _fw_regs3_end = .; } > sram = 0
   .iodatafw4   0xffff3160: {  _fw_regs4_start = .; . = . + 0x20; _fw_regs4_end = .; } > sram = 0
   .iodatafw5   0xffff3180: {  _fw_regs5_start = .; . = . + 0x20; _fw_regs5_end = .; } > sram = 0
   .iodatafw6   0xffff31a0: {  _fw_regs6_start = .; . = . + 0x20; _fw_regs6_end = .; } > sram = 0
   .iodatafw7   0xffff31c0: {  _fw_regs7_start = .; . = . + 0x20; _fw_regs7_end = .; } > sram = 0
   .iodatafw8   0xffff31e0: {  _fw_regs8_start = .; . = . + 0x20; _fw_regs8_end = .; } > sram = 0

   // Reserve 512B for the kernel and 1KB sections for the IO thread stacks at the end of the SRAM (before the mem_regs)
   .pkstack     0xffff3200: {  _kernel_stack_start = .;      . = . + 0x200; _kernel_stack_end = .; } > sram = 0
   .iostack0    0xffff3400: {  _io_thread_stack0_start = .;  . = . + 0x400; _io_thread_stack0_end = .; } > sram = 0
   .iostack1    0xffff3800: {  _io_thread_stack1_start = .;  . = . + 0x400; _io_thread_stack1_end = .; } > sram = 0
   .iostack2    0xffff3c00: {  _io_thread_stack2_start = .;  . = . + 0x400; _io_thread_stack2_end = .; } > sram = 0
   .iostack3    0xffff4000: {  _io_thread_stack3_start = .;  . = . + 0x400; _io_thread_stack3_end = .; } > sram = 0
   .iostack4    0xffff4400: {  _io_thread_stack4_start = .;  . = . + 0x400; _io_thread_stack4_end = .; } > sram = 0
   .iostack5    0xffff4800: {  _io_thread_stack5_start = .;  . = . + 0x400; _io_thread_stack5_end = .; } > sram = 0
   .iostack6    0xffff4c00: {  _io_thread_stack6_start = .;  . = . + 0x400; _io_thread_stack6_end = .; } > sram = 0
   .iostack7    0xffff5000: {  _io_thread_stack7_start = .;  . = . + 0x400; _io_thread_stack7_end = .; } > sram = 0
   .iostack8    0xffff5400: {  _io_thread_stack8_start = .;  . = . + 0x400; _io_thread_stack8_end = .; } > sram = 0

   // Reserve the 1KB mem_regs at the end, fill with 0. Duplicate for up to 5 threads where the last thread is at the end.
   .iodata0     0xffff5800: {  _mem_regs0_start = .; . = . + 0x400; _mem_regs0_end = .; } > sram = 0
   .iodata1     0xffff5c00: {  _mem_regs1_start = .; . = . + 0x400; _mem_regs1_end = .; } > sram = 0
   .iodata2     0xffff6000: {  _mem_regs2_start = .; . = . + 0x400; _mem_regs2_end = .; } > sram = 0
   .iodata3     0xffff6400: {  _mem_regs3_start = .; . = . + 0x400; _mem_regs3_end = .; } > sram = 0
   .iodata4     0xffff6800: {  _mem_regs4_start = .; . = . + 0x400; _mem_regs4_end = .; } > sram = 0
   .iodata5     0xffff6c00: {  _mem_regs5_start = .; . = . + 0x400; _mem_regs5_end = .; } > sram = 0
   .iodata6     0xffff7000: {  _mem_regs6_start = .; . = . + 0x400; _mem_regs6_end = .; } > sram = 0
   .iodata7     0xffff7400: {  _mem_regs7_start = .; . = . + 0x400; _mem_regs7_end = .; } > sram = 0
   .iodata8     0xffff7800: {  _mem_regs8_start = .; . = . + 0x400; _mem_regs8_end = .; } > sram = 0

   // Reserve the last 1KB for a debug log file.
   .debuglog    0xffff7c00: {  _debug_log_start = .; . = . + 0x400; _debug_log_end = .; } > sram = 0
}
