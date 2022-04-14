/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/build/links/odysseylink.H $                        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
#ifndef __ODYSSEY_LINK_H
#define __ODYSSEY_LINK_H

/// \file odysseylink.H
/// \brief Constants required for linking SBE code images

// Common
#define LAST_BYTE                       0x01
#define HASH_LIST_SIZE                  0x800

// SRAM IMAGE
#define SRAM_ORIGIN                     0xFFF80000
#define SRAM_SIZE                       0x80000
#define SRAM_END                        (SRAM_ORIGIN + SRAM_SIZE - LAST_BYTE)

// SROM IMAGE
#define SROM_ORIGIN                     0xFFF70000
#define SROM_SIZE                       0x10000
#define SROM_DATA_SIZE                  0x7800

// Common scratch used by SROM and Boot Loader code for dilithium
#define SROM_BOOTLOADER_SCRATCH_START   (SRAM_ORIGIN + SROM_DATA_SIZE)
#define SROM_BOOTLOADER_SCRATCH_SIZE    0x2D000

// SROM Scratch used for storing Secure Header
#define SROM_SCRATCH_START              (SROM_BOOTLOADER_SCRATCH_START + SROM_BOOTLOADER_SCRATCH_SIZE)
#define VECTOR_SIZE                     0x200
#define FUNCTION_TABLE_OFFSET           SROM_ORIGIN + VECTOR_SIZE
#define FUNCTION_TABLE_SIZE             0x40

// BOOTLOADER IMAGE
#define BOOTLOADER_SIZE                 0xC800
#define BOOTLOADER_ORIGIN               (SRAM_END + LAST_BYTE - BOOTLOADER_SIZE - HASH_LIST_SIZE)

// Boot Loader Scratch used for storing Secure Header
#define BOOTLOADER_SCRATCH_START        (SROM_BOOTLOADER_SCRATCH_START + SROM_BOOTLOADER_SCRATCH_SIZE)

// RUNTIME SPPE IMAGE
#define RUNTIME_EMBED_PAK               0xA000
#define SPPE_SCRATCH_SIZE               0xA000
#define SPPE_ORIGIN                     SRAM_ORIGIN
#define SPPE_SIZE                       (SRAM_SIZE - HASH_LIST_SIZE - SPPE_SCRATCH_SIZE - RUNTIME_EMBED_PAK)
#define SPPE_SCRATCH_START              (SPPE_ORIGIN + SPPE_SIZE)
#define SBE_CODE_BOOT_PIBMEM_MAIN_MSG   0x0F

// HASH LIST START OFFSET
// NOTE: We will always keep hash list at end of sram
#define HASH_LIST_START_OFFSET          (SRAM_END - HASH_LIST_SIZE)

#define NOR_PARTITION_0_OFFSET          0xFE000000

#endif  // __ODYSSEY_LINK_H
