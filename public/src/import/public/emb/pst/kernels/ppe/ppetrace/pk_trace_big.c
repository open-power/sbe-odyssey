/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/ppetrace/pk_trace_big.c $ */
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

/// \file pk_trace_big.c
/// \brief PK Trace function that supports up to four 32-bit parameters
///
/// The pk_trace_big function is only called (via some macro magic) if the
/// caller passes in a single parameter (not including the format string)
/// that is larger than 16 bits to the PK_TRACE(...) macro.
///

#include "pk_trace.h"

#if (PK_TRACE_SUPPORT)
void pk_trace_big(uint32_t i_hash_and_count,
                  uint64_t i_parm1, uint64_t i_parm2)
{
    PkTraceBig          footer;
    PkTraceBig*         footer_ptr;
    PkTraceState        state;
    uint64_t*           ptr64;
    uint64_t            tb64;
    PkMachineContext    ctx;
    uint32_t            parm_size;
    uint32_t            cur_offset;
    uint32_t            footer_offset;

    //fill in the footer data
    tb64 = pk_timebase_get();
    footer.parms.word32 = i_hash_and_count; //this has the parm count and hash

    //*****The following operations must be done atomically*****
    pk_critical_section_enter(&ctx);

#if (PK_TRACE_VERSION == 4)
    _pk_check_and_add_special_trace_entries(tb64);
#endif

    // This is not required in new version (v4) since it will be done in above function,
    //   but keeping it common since it will not break anything for v4.
    state.tbu32 = tb64 >> 32;

    footer.time_format.word32 = tb64 & 0x00000000ffffffffull;
    footer.time_format.format = PK_TRACE_FORMAT_BIG;

    //round up to 8 byte boundary
    if(footer.parms.num_parms <= 2)
    {
        parm_size = 8;
    }
    else
    {
        parm_size = 16;
    }

    //load in the offset in the cb for the entry we are adding
#ifdef APP_DEFINED_TRACE_BUFFER
    cur_offset = G_PK_TRACE_BUF->state.offset;
#else
    cur_offset = g_pk_trace_buf.state.offset;
#endif

    //Find the offset for the footer (at the end of the entry)
    footer_offset = cur_offset + parm_size;

    //calculate the address of the footer
#ifdef APP_DEFINED_TRACE_BUFFER
    ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[footer_offset & PK_TRACE_CB_MASK];
#else
    ptr64 = (uint64_t*)&g_pk_trace_buf.cb[footer_offset & PK_TRACE_CB_MASK];
#endif

    //calculate the offset for the next entry in the cb
    state.offset = footer_offset + sizeof(PkTraceBig);

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

    //write the data to the circular buffer including the
    //timesamp, string hash, and 16bit parameter
    *ptr64 = footer.word64;

    //*******************exit the critical section***************
    pk_critical_section_exit(&ctx);


    //write parm values to the circular buffer
    footer_ptr = (PkTraceBig*)ptr64;
#ifdef APP_DEFINED_TRACE_BUFFER
    ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[cur_offset & PK_TRACE_CB_MASK];
#else
    ptr64 = (uint64_t*)&g_pk_trace_buf.cb[cur_offset & PK_TRACE_CB_MASK];
#endif
    *ptr64 = i_parm1;

    if(parm_size > 8)
    {
#ifdef APP_DEFINED_TRACE_BUFFER
        ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[(cur_offset + 8) & PK_TRACE_CB_MASK];
#else
        ptr64 = (uint64_t*)&g_pk_trace_buf.cb[(cur_offset + 8) & PK_TRACE_CB_MASK];
#endif
        *ptr64 = i_parm2;
    }

    //Mark the trace entry update as being completed
    footer_ptr->parms.complete = 1;

}

#endif
