/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/build/links/odysseylink.H $                        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
#pragma once

/// \file odysseylink.H
/// \brief Constants required for linking SBE code images

/****************************** Memory ****************************************/
// SROM
#define SROM_ORIGIN                     0xFFF70000
#define SROM_SIZE                       0x10000

// SRAM Memory
#define SRAM_ORIGIN                     0xFFF80000
#define SRAM_SIZE                       0x80000

/************************ Common Constants ************************************/
#define LAST_BYTE                       0x01
#define SRAM_END                        (SRAM_ORIGIN + SRAM_SIZE)
// PK structure - PkTraceBuffer defined in pk_trace.h
#define PK_TRACE_STRUCT_SIZE            56

/*****************************************************************************/
/********************** SRAM Layout (common for all images)*******************/
               /***********************************************/
               /*           Image specific layout             */
               /***********************************************/
               /*            Hashlist (2 KB)                  */
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/
#define METADATA_PTR_SIZE               4
#define TRACE_BUF_PTR_SIZE              4
#define HASH_LIST_SIZE                  0x800
#define METADATA_PTR_OFFSET             (SRAM_END - METADATA_PTR_SIZE)
#define TRACE_BUF_PTR_OFFSET            (METADATA_PTR_OFFSET - TRACE_BUF_PTR_SIZE)
#define HASH_LIST_START_OFFSET          (TRACE_BUF_PTR_OFFSET - HASH_LIST_SIZE)
#define CROSS_IMAGE_FIXED_AREA_SIZE     (METADATA_PTR_SIZE + TRACE_BUF_PTR_SIZE + HASH_LIST_SIZE)

/*********************************** SROM IMAGE *******************************/
#define VECTOR_SIZE                     0x200
#define SROM_VECTOR_SIZE                0x164
#define FUNCTION_TABLE_OFFSET           (SROM_ORIGIN + VECTOR_SIZE)
#define FUNCTION_TABLE_SIZE             0x40


/*****************************************************************************/
/********************** SROM Data Layout in SRAM *****************************/
               /***********************************************/
               /*           Data (30 KB)                      */
               /***********************************************/
               /*           Stack Scratch (180 KB)            */
               /***********************************************/
               /*           Pak Scratch (32 KB)               */
               /***********************************************/
               /*               Unused                        */
               /***********************************************/
               /*PK Trace buffer (2KB+56:-2KB circular buffer)*/
               /***********************************************/
               /*            Hashlist (2 KB)                  */
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/
#define SROM_SRAM_START                 SRAM_ORIGIN
#define SROM_DATA_SIZE                  0x7800 // 30 KB

// Common scratch used by SROM and Boot Loader code for dilithium
#define SROM_BLDR_CMN_DILITHIUM_SCRATCH_START   (SRAM_ORIGIN + SROM_DATA_SIZE)
#define SROM_BLDR_CMN_DILITHIUM_SCRATCH_SIZE    0x2D000 // 180 KB

// SROM BLDR Scratch used for storing Secure Header (SH)
#define SROM_BLDR_CMN_SH_FILE_SCRATCH_START     (SROM_BLDR_CMN_DILITHIUM_SCRATCH_START \
                                                 + SROM_BLDR_CMN_DILITHIUM_SCRATCH_SIZE)
#define SROM_BLDR_CMN_SH_FILE_SCRATCH_SIZE      0x8000 // 30 KB
#define SROM_BLDR_CMN_SH_FILE_SCRATCH_END       (SROM_BLDR_CMN_SH_FILE_SCRATCH_START \
                                                 + SROM_BLDR_CMN_SH_FILE_SCRATCH_SIZE - LAST_BYTE)

// Trace buffer
#define SROM_PK_TRACE_SIZE              0x800  // Size: 2KB
#define SROM_PK_TRACE_SIZE_WITH_HEADER  (PK_TRACE_STRUCT_SIZE + SROM_PK_TRACE_SIZE)

#define SROM_TRACE_START_OFFSET         (HASH_LIST_START_OFFSET - SROM_PK_TRACE_SIZE_WITH_HEADER)

/*****************************************************************************/
/********************** BootLoader Image Layout ******************************/
               /***********************************************/
               /*           Unused (32 KB)                    */
               /***********************************************/
               /*           Stack Scratch (180 KB)            */
               /***********************************************/
               /*           Pak Scratch (32 KB)               */
               /***********************************************/
               /*               Unused                        */
               /***********************************************/
               /*               BLDR Image (50 KB)            */
               /***********************************************/
               /*PK Trace buffer (4KB+56:-4KB circular buffer)*/
               /***********************************************/
               /*            Hashlist (2 KB)                  */
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/
#define ROUND_DOWN(value, grain)        ((value) & ~((grain) - 1))
#define BOOTLOADER_SIZE                 0x10000 // 64 KB (including both bldr.bin (32K fixed now) and .bss+.sbss (around 25K now))
#define BLDR_PK_TRACE_SIZE              0x1000  // Size: 4KB
#define BLDR_PK_TRACE_SIZE_WITH_HEADER  (PK_TRACE_STRUCT_SIZE + BLDR_PK_TRACE_SIZE)

#define BLDR_TRACE_START_OFFSET         (HASH_LIST_START_OFFSET - BLDR_PK_TRACE_SIZE_WITH_HEADER)
// vector table alignment boundary. As per PPE HW archetecture, vector table should
//      start in 0x200 byte boundary
#define PPE_VEC_TBL_ALGNMT_BOUNDARY     0x200
#define BOOTLOADER_ORIGIN               (ROUND_DOWN((BLDR_TRACE_START_OFFSET \
                                        - BOOTLOADER_SIZE), PPE_VEC_TBL_ALGNMT_BOUNDARY))

#ifndef __ASSEMBLER__
#ifndef __SBE__
ASSERT((SROM_BLDR_CMN_SH_FILE_SCRATCH_END < BOOTLOADER_ORIGIN), \
        "Scratch space overlap with bootloader origin");
#endif
#endif

/*****************************************************************************/
/********************** Runtime Image Layout *********************************/
               /***********************************************///-
               /*              SPPE Image (+.bss)             */// |
               /*                       +                     */// |
               /*                   sppe.ram.pak              */// |
               /*                       +                     */// > Total 504 KB
               /*              Pak Stack (+Rec-Log)           */// |
               /*                       +                     */// |
               /*                  Pak Scratch                *///_
               /***********************************************/
               /* PK Trace buffer (8248B:-8KB circular buffer)*/
               /***********************************************/
               /*            Hashlist (2 KB)                  */
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/
#define SPPE_ORIGIN                     SRAM_ORIGIN

#define SBE_NONCRITICAL_STACK_SIZE              512

// 6K of stack to thermal sensor polling
#define SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE    6144

#define SBE_THREAD_CMD_RECV_STACK_SIZE          512

//12K of Stack to handle large rings
#define SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE     10240

#define SBE_TOTAL_STACK_SIZE_REQUIRED_IN_SCRATCH  (     \
            SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE +      \
            SBE_THREAD_CMD_RECV_STACK_SIZE +            \
            SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE )

// As per now sppe.ram.pak size is 3.5KB. So given some buffer and declared to 5 KB
#define SPPE_RAM_PAK_MIN_SIZE            0x1400   // 5 KB
//- Most of the heavy decompression (where a space of 32 KB required) is done before
//  ody_sppe_draminit and SPPE_MEM_TRAINING_DATA_SIZE will not be used till this point.
//  Hence all these decompression can use SPPE_MEM_TRAINING_DATA_SIZE.
//- Large decompression space required after ody_sppe_draminit, is ody_load_pie, where we
//  need around 21 KB.
//- So setting this 24 KB
#define SPPE_DECOMPRESSION_AREA_MIN_SIZE 0x6000 // 24 KB
#define SPPE_MEM_TRAINING_DATA_SIZE      0x8200 // 32 KB + 512 B
#define SPPE_RECOVERABLE_LOG_MIN_SIZE    0x0    // 0 KB
// Minimum Space required for FFDC initilization
// Allocating the persistent space for LAST UE (840B) and Scratch space RC (112B)
// while FFDC initialization, which is used for both RE/UE
#define SPPE_FFDC_INIT_REQUIRED_SIZE     0x800  // 2 KB
#define SPPE_HEAP_MIN_SIZE              (SPPE_RAM_PAK_MIN_SIZE + SPPE_DECOMPRESSION_AREA_MIN_SIZE       \
                                         + SPPE_MEM_TRAINING_DATA_SIZE + SPPE_RECOVERABLE_LOG_MIN_SIZE  \
                                         + SBE_TOTAL_STACK_SIZE_REQUIRED_IN_SCRATCH + SPPE_FFDC_INIT_REQUIRED_SIZE)

#define SPPE_MAX_SIZE                   (SRAM_SIZE - CROSS_IMAGE_FIXED_AREA_SIZE \
                                         - SPPE_HEAP_MIN_SIZE - SPPE_PK_TRACE_SIZE_WITH_HEADER)

#define SPPE_PK_TRACE_SIZE              0x800  // Size: 2KB
#define SPPE_PK_TRACE_SIZE_WITH_HEADER  (PK_TRACE_STRUCT_SIZE + SPPE_PK_TRACE_SIZE)

#define SPPE_TRACE_START_OFFSET         (HASH_LIST_START_OFFSET - SPPE_PK_TRACE_SIZE_WITH_HEADER)

/********************** NOR partition Layout *********************************/
// Lets keep each side size as 4mb
#define NOR_SIDE_SIZE              0x00400000
#define NOR_SIDE_0_START_ADDR      0xFE000000
#define NOR_SIDE_1_START_ADDR      (NOR_SIDE_0_START_ADDR + NOR_SIDE_SIZE)
#define NOR_GOLDEN_SIDE_START_ADDR (NOR_SIDE_1_START_ADDR + NOR_SIDE_SIZE)

// Side index based on bit 17,18 in FSXCOMP_FSXLOG_SB_CS
#define SIDE_0_INDEX 0
#define SIDE_1_INDEX 1
#define GOLDEN_SIDE_INDEX 2

#ifndef __ASSEMBLER__
// This is done so that during linker compilation does not fail.
// The below macro is not defined for linker compilation
#ifdef __SBE__

    static inline uint32_t getAbsPartitionAddr(uint8_t partitionSelect)
    {
        switch(partitionSelect)
        {
            case SIDE_0_INDEX:
                return NOR_SIDE_0_START_ADDR;
            case SIDE_1_INDEX:
                return NOR_SIDE_1_START_ADDR;
            case GOLDEN_SIDE_INDEX:
                return NOR_GOLDEN_SIDE_START_ADDR;
            default:
                return 0;
        }
    }

#endif
#endif
