/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/pk/kernel/pk_debug_ptrs.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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
#ifndef __PK_DEBUG_PTRS_H__
#define __PK_DEBUG_PTRS_H__

/// \file pk_debug_ptrs.h
/// \brief Structure for a table of pointers to kernel debug data
///

#define PK_DEBUG_PTRS_VERSION  1

typedef struct
{
    // The size and version of this structure
    unsigned short   debug_ptrs_size;
    unsigned short   debug_ptrs_version;

    // Trace buffer location and size
    void*           debug_trace_ptr;
    unsigned long   debug_trace_size;

    // Thread table location and size
    void*           debug_thread_table_ptr;
    unsigned long   debug_thread_table_size;

    // Thread run queue location and size
    void*           debug_thread_runq_ptr;
    unsigned long   debug_thread_runq_size;

    // Emulated timebase location and size
    void*           debug_timebase_ptr;
    unsigned long   debug_timebase_size;

} pk_debug_ptrs_t;

extern pk_debug_ptrs_t pk_debug_ptrs;

#define PK_TRACE_PTR_OFFSET   (sizeof(pk_debug_ptrs.debug_ptrs_size) + \
                               sizeof(pk_debug_ptrs.debug_ptrs_version))
#define PK_TRACE_SIZE_OFFSET  (PK_TRACE_PTR_OFFSET + \
                               sizeof(pk_debug_ptrs.debug_trace_ptr))

#endif /*__PK_DEBUG_PTRS_H__*/
