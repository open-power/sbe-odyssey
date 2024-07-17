/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/build/links/hsbelink.H $                           */
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

/// \file hsbelink.H
/// \brief Constants required for linking SPINAL(HUB) SBE code images

/****************************** Memory ***************************************/
// HUB SBE RAM Memory
#define HUB_SBE_RAM_ORIGIN             (0xFFF80000)
#define HUB_SBE_RAM_SIZE               (0x80000)     // 512KB

/*************************** Common Constants ********************************/
#define HUB_SBE_RAM_END                (HUB_SBE_RAM_ORIGIN + HUB_SBE_RAM_SIZE)
// PK structure - PkTraceBuffer defined in pk_trace.h
#define PK_TRACE_STRUCT_SIZE           (56)

/*****************************************************************************/
/********************** HUB Runtime SBE RAM Image Layout *****************************/
               /***********************************************/
               /*    HUB image (.txt + .bss) (464 KB)     */// |
               /*                   +                     */// |
               /*           hub.ram.pak (5 KB)            */// |
               /*                   +                     */// > Total 495 KB
               /*       Decompression buffer (16KB)       */// |
               /*                   +                     */// |
               /*         Recoverable-Log (8 KB)          */// |
               /*                   +                     */// |
               /*         FFDC Init buffer (2 KB)         */// _
               /***********************************************/
               /* PK Trace buffer (8248B:-8KB circular buffer)*/
               /***********************************************/
               /* Sppe dump buffer (8 KB)                     */
               /***********************************************/
               /* Pointer to Sppe dump buffer (4B)            */
               /***********************************************/
               /* Pointer to PK trace buffer (4B)             */
               /***********************************************/
               /* Pointer to current image's metadata (4B)    */
               /***********************************************/

#define SPPE_ALGNMT_BOUNDARY                  (8)   //8 Byte Alignment
#define ROUND_DOWN(value, grain)              ((value) & ~((grain) - 1))

#define HUB_VECTOR_SIZE                       (0x200)

#define HUB_METADATA_PTR_SIZE                 (4)
#define HUB_METADATA_PTR_OFFSET               (HUB_SBE_RAM_END - HUB_METADATA_PTR_SIZE)

#define TRACE_BUF_PTR_SIZE                    (4)
#define HUB_PK_TRACE_BUF_SIZE                 (0x2000) //8KB
#define HUB_PK_TRACE_SIZE_WITH_HEADER         (PK_TRACE_STRUCT_SIZE + HUB_PK_TRACE_BUF_SIZE)
#define TRACE_BUF_PTR_OFFSET                  (HUB_METADATA_PTR_OFFSET - TRACE_BUF_PTR_SIZE)

#define SPPE_DUMP_BUF_PTR_SIZE                (4)
#define SPPE_DUMP_BUF_SIZE                    (0x2000) //8KB
#define SPPE_DUMP_BUF_PTR_OFFSET              (TRACE_BUF_PTR_OFFSET - SPPE_DUMP_BUF_PTR_SIZE)

#define HUB_TRACE_START_OFFSET                ROUND_DOWN((SPPE_DUMP_BUF_PTR_OFFSET - SPPE_DUMP_BUF_SIZE - HUB_PK_TRACE_SIZE_WITH_HEADER),SPPE_ALGNMT_BOUNDARY)

#define HUB_IMAGE_FIXED_AREA_SIZE             (HUB_METADATA_PTR_SIZE + TRACE_BUF_PTR_SIZE + SPPE_DUMP_BUF_PTR_SIZE)

// Based on Odyssey, assuming 5KB
#define HUB_RAM_PAK_MIN_SIZE                  (0x1400) // 5 KB
#define HUB_DECOMPRESSION_AREA_MIN_SIZE       (0x4000) // 16 KB
#define HUB_RECOVERABLE_LOG_MIN_SIZE          (0x2000) // 8 KB

// Minimum Space required for FFDC initilization
// Allocating the persistent space for LAST UE (840B) and Scratch space RC (112B)
// while FFDC initialization, which is used for both RE/UE
#define HUB_SBE_FFDC_INIT_REQUIRED_SIZE       (0x800)  // 2 KB

//SIZE: 31KB (5KB + 16KB + 8KB + 2KB)
#define HUB_SBE_HEAP_MIN_SIZE                 (HUB_RAM_PAK_MIN_SIZE                    \
                                                 + HUB_DECOMPRESSION_AREA_MIN_SIZE     \
                                                 + HUB_RECOVERABLE_LOG_MIN_SIZE        \
                                                 + HUB_SBE_FFDC_INIT_REQUIRED_SIZE)

//SIZE: ~464KB (512KB - (4B + 4B + 4B)
//               - (5KB + 16KB + 8KB +2KB) - (56B + 8KB) - 8KB)
#define HUB_SBE_SIZE                          (HUB_SBE_RAM_SIZE - HUB_IMAGE_FIXED_AREA_SIZE   \
                                                 - HUB_SBE_HEAP_MIN_SIZE                      \
                                                 - HUB_PK_TRACE_SIZE_WITH_HEADER              \
                                                 - SPPE_DUMP_BUF_SIZE)
