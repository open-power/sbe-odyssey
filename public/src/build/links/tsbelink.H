/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/build/links/tsbelink.H $                           */
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

/// \file tsbelink.H
/// \brief Constants required for linking TAP SBE code images

/****************************** Memory ***************************************/
// TAP SRAM Memory
#define TAP_SRAM_ORIGIN                (0xFFFA0000)
#define TAP_SRAM_SIZE                  (0x60000)     // 384KB

/*************************** Common Constants ********************************/
#define TAP_SRAM_END                   (TAP_SRAM_ORIGIN + TAP_SRAM_SIZE)
// PK structure - PkTraceBuffer defined in pk_trace.h
#define PK_TRACE_STRUCT_SIZE           (56)

/*****************************************************************************/
/********************** TAP Runtime SRAM Image Layout *****************************/
               /***********************************************/
               /*        TAP image (.txt + .bss)          */// |
               /*                   +                     */// |
               /*              tap.ram.pak                */// |
               /*                   +                     */// > Total ~376 KB
               /*      Pak Stack (+Rec-Log)(40 KB)        */// |
               /*                   +                     */// |
               /*              Pak Scratch(40 KB)         *///_
               /***********************************************/
               /* PK Trace buffer (8248B:-8KB circular buffer)*/
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/
#define TAP_VECTOR_SIZE                      (0x200)

#define TAP_METADATA_PTR_SIZE                 (4)
#define TAP_METADATA_PTR_OFFSET               (TAP_SRAM_END - TAP_METADATA_PTR_SIZE)

#define TRACE_BUF_PTR_SIZE                    (4)
#define TAP_PK_TRACE_SIZE                     (0x2000) //8KB
#define TAP_PK_TRACE_SIZE_WITH_HEADER         (PK_TRACE_STRUCT_SIZE + TAP_PK_TRACE_SIZE)
#define TRACE_BUF_PTR_OFFSET                  (TAP_METADATA_PTR_OFFSET - TRACE_BUF_PTR_SIZE)

#define TAP_CROSS_IMAGE_FIXED_AREA_SIZE       (TAP_METADATA_PTR_SIZE + TRACE_BUF_PTR_SIZE)

// As per now sppe.ram.pak size is 3.5KB. So given some buffer and declared to 5 KB
#define TAP_RAM_PAK_MIN_SIZE                   (0x1400) // 5 KB
//TODO: Need to review the size required for decompression.
#define TAP_DECOMPRESSION_AREA_MIN_SIZE        (0x6000) // 24 KB
#define TAP_RECOVERABLE_LOG_MIN_SIZE           (0x0)    // 0 KB
// Minimum Space required for FFDC initilization
// Allocating the persistent space for LAST UE (840B) and Scratch space RC (112B)
// while FFDC initialization, which is used for both RE/UE
#define TAP_SBE_FFDC_INIT_REQUIRED_SIZE  (0x800)  // 2 KB
#define TAP_SBE_HEAP_MIN_SIZE            (TAP_RAM_PAK_MIN_SIZE                    \
                                            + TAP_DECOMPRESSION_AREA_MIN_SIZE     \
                                            + TAP_RECOVERABLE_LOG_MIN_SIZE        \
                                            + TAP_SBE_FFDC_INIT_REQUIRED_SIZE)

#define TAP_SCRATCH_SIZE                 (0xA000) //40KB
#define TAP_RUNTIME_EMBED_PAK_SIZE       (0xA000) //40KB

//SIZE: ~256.5KB (384KB - (4B + 4B)
//               - (5KB + 24KB + 0KB + (.5KB + 8KB) +2KB) - 40KB - 40KB - (56B + 8KB))
#define TAP_SBE_SIZE                     (TAP_SRAM_SIZE - TAP_CROSS_IMAGE_FIXED_AREA_SIZE \
                                           - TAP_SBE_HEAP_MIN_SIZE - TAP_SCRATCH_SIZE     \
                                           - TAP_RUNTIME_EMBED_PAK_SIZE                   \
                                           - TAP_PK_TRACE_SIZE_WITH_HEADER)

//#define SCRATCH_START                      (TAP_SRAM_ORIGIN + TAP_SBE_SIZE + TAP_RUNTIME_EMBED_PAK_SIZE)
#define TAP_TRACE_START_OFFSET           (TRACE_BUF_PTR_OFFSET - TAP_PK_TRACE_SIZE_WITH_HEADER)

#ifndef __ASSEMBLER__
#ifndef __SBE__
// Linker script command. Assert it before the final stages of linking take place
ASSERT(((TAP_SBE_SIZE + TAP_CROSS_IMAGE_FIXED_AREA_SIZE + TAP_SBE_HEAP_MIN_SIZE + TAP_SCRATCH_SIZE  \
        + TAP_RUNTIME_EMBED_PAK_SIZE + TAP_PK_TRACE_SIZE_WITH_HEADER) <= TAP_SRAM_SIZE),            \
        "Total size exceeds TAP SRAM size");
#endif
#endif
