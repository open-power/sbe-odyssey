/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/ppetrace/pk_trace_binary.c $ */
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

/// \file pk_trace_binary.c
/// \brief PK Trace function for dumping memory contents
///
/// The pk_trace_binary function is called by the PK_TRACE_BINARY() macro.
///


#include "pk_trace.h"

#if (PK_TRACE_SUPPORT)
void pk_trace_binary(uint32_t i_hash_and_size, const void* bufp)
{
    PkTraceBinary           footer;
    PkTraceBinary*          footer_ptr;
    PkTraceState            state;
    uint64_t*               ptr64;
    uint64_t                tb64;
    PkMachineContext        ctx;
    uint32_t                data_size;
    uint32_t                cb_offset;
    uint32_t                footer_offset;
    uint8_t*                dest;
    const uint8_t*          src;
    uint32_t                index;

    //fill in the footer data
    tb64 = pk_timebase_get();

    //*****The following operations must be done atomically*****
    pk_critical_section_enter(&ctx);
    footer.parms.word32 = i_hash_and_size; //this has the size and hash

#if (PK_TRACE_VERSION == 4)
    _pk_check_and_add_special_trace_entries(tb64);
#endif

    // This is not required in new version (v4) since it will be done in above function,
    //   but keeping it common since it will not break anything for v4.
    state.tbu32 = tb64 >> 32;
    footer.time_format.word32 = tb64 & 0x00000000ffffffffull;
    footer.time_format.format = PK_TRACE_FORMAT_BINARY;

    //round up to 8 byte boundary
    data_size = (footer.parms.num_bytes + 7) & ~0x00000007ul;

    //limit data size
    if(data_size > PK_TRACE_CLIPPED_BINARY_SZ)
    {
        data_size = PK_TRACE_CLIPPED_BINARY_SZ;
    }

    //load in the offset in the cb for the entry we are adding
#ifdef APP_DEFINED_TRACE_BUFFER
    cb_offset = G_PK_TRACE_BUF->state.offset;
#else
    cb_offset = g_pk_trace_buf.state.offset;
#endif

    //Find the offset for the footer (at the end of the entry)
    footer_offset = cb_offset + data_size;

    //calculate the address of the footer
#ifdef APP_DEFINED_TRACE_BUFFER
    ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[footer_offset & PK_TRACE_CB_MASK];
#else
    ptr64 = (uint64_t*)&g_pk_trace_buf.cb[footer_offset & PK_TRACE_CB_MASK];
#endif

    //calculate the offset for the next entry in the cb
    state.offset = footer_offset + sizeof(PkTraceBinary);

#ifdef PK_TRACE_BUFFER_WRAP_MARKER

    //insert marker to indicate when circular buffer wraps
    if ((state.offset & PK_TRACE_SZ) ^ G_wrap_mask)
    {
        G_wrap_mask = state.offset & PK_TRACE_SZ;
        asm volatile ("tw 0, 31, 31");
    }

#endif

    //update the cb state (tbu and offset)
#ifdef APP_DEFINED_TRACE_BUFFER
    G_PK_TRACE_BUF->state.word64 = state.word64;
#else
    g_pk_trace_buf.state.word64 = state.word64;
#endif

    //write the footer data to the circular buffer including the
    //timestamp, string hash and data size
    *ptr64 = footer.word64;

    //*******************exit the critical section***************
    pk_critical_section_exit(&ctx);

    //write data to the circular buffer
    for(src = bufp, index = 0;
        index < data_size;
        index++)
    {
#ifdef APP_DEFINED_TRACE_BUFFER
        dest = &G_PK_TRACE_BUF->cb[(cb_offset + index) & PK_TRACE_CB_MASK];
#else
        dest = &g_pk_trace_buf.cb[(cb_offset + index) & PK_TRACE_CB_MASK];
#endif
        *dest = *(src++);
    }

    //Mark the trace entry update as being completed
    footer_ptr = (PkTraceBinary*)ptr64;
    footer_ptr->parms.complete = 1;

}

#endif
