/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/ppetrace/pk_trace_core.c $ */
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

/// \file pk_trace_core.c
/// \brief PK Trace core data and code.
///
/// This file includes the minimal code/data required to do minimal tracing.
/// This includes the periodic timer initialization and the pk_trace_tiny
/// function.  The pk_trace_tiny function is called by the PK_TRACE() macro
/// when there is one or less parameters (not including the format string)
/// and the parameter size is 16 bits or smaller.
///

#include "pk_trace.h"


#if (PK_TRACE_SUPPORT)
#if  (PK_TIMER_SUPPORT)
void pk_trace_timer_callback(void* arg);
extern PkTimer       g_pk_trace_timer __attribute__((section (".sdata")));

//Static initialization of the trace timer
PkTimer g_pk_trace_timer __attribute__ ((section (".sdata"))) =
{
    .deque = PK_DEQUE_ELEMENT_INIT(),
    .timeout = 0,
    .callback = pk_trace_timer_callback,
    .arg = 0,
};
#endif

//Static initialization of the pk trace buffer
#ifndef APP_DEFINED_TRACE_BUFFER
PkTraceBuffer g_pk_trace_buf __attribute__ ((section (".sdata"))) =
{
    .version            = PK_TRACE_VERSION,
    .image_str          = PPE_IMG_STRING,
    .hash_prefix        = PK_TRACE_HASH_PREFIX,
    .partial_trace_hash =
    trace_ppe_hash("PARTIAL TRACE ENTRY. HASH_ID = %d", PK_TRACE_HASH_PREFIX),
    .size               = PK_TRACE_SZ,
    .max_time_change    = PK_TRACE_MTBT,
    .hz                 = 25000000, //Actula value set by pk_trace_set_freq()
    .time_adj64         = 0,
    .state.word64       = 0,
    .cb                 = {0}
};

PkTraceBuffer* g_pk_trace_buf_ptr = &g_pk_trace_buf;
#else
PkTraceBuffer* G_PK_TRACE_BUF __attribute__((section (".g_pk_trace_buf")));
#endif

#if (PK_OP_TRACE_SUPPORT)
//Static initialization of the pk op trace buffer
PkOpTraceBuffer g_pk_op_trace_buf __attribute__ ((section (".sdata"))) =
{
    .version            = PK_TRACE_VERSION,
    .image_str          = PPE_IMG_STRING,
    .hash_prefix        = PK_TRACE_HASH_PREFIX,
    .partial_trace_hash =
    trace_ppe_hash("PARTIAL TRACE ENTRY. HASH_ID = %d", PK_TRACE_HASH_PREFIX),
    .size               = PK_OP_TRACE_SZ,
    .max_time_change    = PK_TRACE_MTBT,
    .hz                 = 25000000, //Actula value set by pk_trace_set_freq()
    .time_adj64         = 0,
    .state.word64       = 0,
    .cb                 = {0}
};

//Needed for buffer extraction in simics for now
PkOpTraceBuffer* g_pk_op_trace_buf_ptr = &g_pk_op_trace_buf;

#endif


#ifdef PK_TRACE_BUFFER_WRAP_MARKER
    uint32_t G_wrap_mask = 0;
#endif

#if (PK_TRACE_VERSION == 4)

// Helper function to add a Special trace entry into the trace buffer.
// This function is expected to be called inside critical section.
//
// i_tbl32 lower 32-bit value of 64-bit time base
// i_type  type of special entry
// i_param parameter for the special entry
void _pk_trace_add_special_entry(
    uint32_t i_tbl32, PkTraceSpecialTypes i_type, uint32_t i_param)
{
    PkTraceSpecial      footer;
    uint64_t*           ptr64;

    // We might be already inside critical section,
    //  so not entering critical section again here.

    footer.time_format.word32 = i_tbl32;

    footer.time_format.format = PK_TRACE_FORMAT_SPECIAL;

    footer.type = i_type;
    footer.value = i_param;
#ifdef APP_DEFINED_TRACE_BUFFER
    //load the current byte count and calculate the address for this
    // entry in the cb
    ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[G_PK_TRACE_BUF->state.offset & PK_TRACE_CB_MASK];

    //calculate the offset for the next entry in the cb
    G_PK_TRACE_BUF->state.offset = G_PK_TRACE_BUF->state.offset + sizeof(PkTraceSpecial);
#else
    //load the current byte count and calculate the address for this
    // entry in the cb
    ptr64 = (uint64_t*)&g_pk_trace_buf.cb[g_pk_trace_buf.state.offset & PK_TRACE_CB_MASK];

    //calculate the offset for the next entry in the cb
    g_pk_trace_buf.state.offset = g_pk_trace_buf.state.offset + sizeof(PkTraceSpecial);
#endif

    //write the special entry to the circular buffer
    *ptr64 = footer.word64;
}

// Creates an 8 byte time bump trace entry
// This function is expected to be called inside critical section.
//
// i_tbu32 upper 32-bit value of 64-bit time base
// i_tbl32 lower 32-bit value of 64-bit time base
void _pk_trace_time_bump(uint32_t i_tbu32, uint32_t i_tbl32)
{
#ifdef APP_DEFINED_TRACE_BUFFER
    uint32_t current_tbu32 = G_PK_TRACE_BUF->state.tbu32;
    G_PK_TRACE_BUF->state.tbu32 = i_tbu32;
#else
    uint32_t current_tbu32 = g_pk_trace_buf.state.tbu32;
    g_pk_trace_buf.state.tbu32 = i_tbu32;
#endif

    _pk_trace_add_special_entry(
        i_tbl32,
        TIME_BUMP,
        (i_tbu32 - current_tbu32) & 0x00FFFFFF);
}

// Creates an 8 byte thread switch trace entry
// This function is expected to be called inside critical section.
//
// i_tbl32     lower 32-bit value of 64-bit time base
// i_thread_id id of currently running thread
void _pk_trace_thread_switch(uint32_t i_tbl32, uint8_t i_thread_id)
{
#ifdef APP_DEFINED_TRACE_BUFFER
    uint8_t cur_thread_id = G_PK_TRACE_BUF->thread_id;
    G_PK_TRACE_BUF->thread_id = i_thread_id;
#else
    uint8_t cur_thread_id = g_pk_trace_buf.thread_id;
    g_pk_trace_buf.thread_id = i_thread_id;
#endif

    _pk_trace_add_special_entry(
        i_tbl32,
        THREAD_CHANGE,
        cur_thread_id);
}

// Check whether a special trace entry needed or not. If needed, add the entry.
void _pk_check_and_add_special_trace_entries(uint64_t i_time_base)
{
    uint32_t tbu32 = i_time_base >> 32;
    uint32_t tbl32 = i_time_base & 0x00000000ffffffffull;

#ifdef APP_DEFINED_TRACE_BUFFER

    if(tbu32 < G_PK_TRACE_BUF->state.tbu32)
#else
    if(tbu32 < g_pk_trace_buf.state.tbu32)
#endif
    {
        // There is small window after pk_timebase_get() called from this thread
        //  where another thread (or a callback) will add some traces.
        //  And it may cause this condition.
        // TODO: PFSBE-802
        //  can remove this check as part of above story.
        PK_PANIC(PK_TRACE_TIME_INCONSISTENT);
    }

#ifdef APP_DEFINED_TRACE_BUFFER
    else if(tbu32 > G_PK_TRACE_BUF->state.tbu32)
#else
    else if(tbu32 > g_pk_trace_buf.state.tbu32)
#endif
    {
        // Add a time bump entry
        _pk_trace_time_bump(tbu32, tbl32);
    }

    // TODO: once we enable mutex for tracing then we should not allow tracing in
    //       non-thread (interrupt) context.

    // PK_THREADS can be used to indicate this trace is outside any thread context.
    uint8_t thread_id = __pk_current_thread ? __pk_current_thread->priority : PK_THREADS;
#ifdef APP_DEFINED_TRACE_BUFFER

    if(thread_id != G_PK_TRACE_BUF->thread_id)
#else
    if(thread_id != g_pk_trace_buf.thread_id)
#endif
    {
        _pk_trace_thread_switch(tbl32, thread_id);
    }
}

#endif

// Creates an 8 byte entry in the trace buffer that includes a timestamp,
// a format string hash value and a 16 bit parameter.
//
// i_parm has the hash value combined with the 16 bit parameter
void pk_trace_tiny(uint32_t i_parm)
{
    PkTraceTiny         footer;
    PkTraceState        state;
    uint64_t*           ptr64;
    uint64_t            tb64;
    PkMachineContext    ctx;

    //fill in the footer data
    footer.parms.word32 = i_parm;
    tb64 = pk_timebase_get();

    //The following operations must be done atomically
    pk_critical_section_enter(&ctx);

#if (PK_TRACE_VERSION == 4)
    _pk_check_and_add_special_trace_entries(tb64);
#endif

    // This is not required in new version (v4) since it will be done in above function,
    //   but keeping it common since it will not break anything for v4.
    state.tbu32 = tb64 >> 32;

    footer.time_format.word32 = tb64 & 0x00000000ffffffffull;

    footer.time_format.format = PK_TRACE_FORMAT_TINY;

    //load the current byte count and calculate the address for this
    //entry in the cb
#ifdef APP_DEFINED_TRACE_BUFFER
    ptr64 = (uint64_t*)&G_PK_TRACE_BUF->cb[G_PK_TRACE_BUF->state.offset & PK_TRACE_CB_MASK];
#else
    ptr64 = (uint64_t*)&g_pk_trace_buf.cb[g_pk_trace_buf.state.offset & PK_TRACE_CB_MASK];
#endif

    //calculate the offset for the next entry in the cb
#ifdef APP_DEFINED_TRACE_BUFFER
    state.offset = G_PK_TRACE_BUF->state.offset + sizeof(PkTraceTiny);
#else
    state.offset = g_pk_trace_buf.state.offset + sizeof(PkTraceTiny);
#endif

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

    //exit the critical section
    pk_critical_section_exit(&ctx);
}

#if  (PK_TIMER_SUPPORT)
// This function is called periodically in order to ensure that the max ticks
// between trace entries is no more than what will fit inside a 32bit value.
#ifndef PK_TRACE_TIMER_OUTPUT
    #define PK_TRACE_TIMER_OUTPUT 1
#endif
void pk_trace_timer_callback(void* arg)
{
#if PK_TRACE_TIMER_OUTPUT
    // guarantee at least one trace before the lower 32bit timebase flips
    PK_TRACE("PERIODIC TIMESTAMPING TRACE");
#endif
    // restart the timer
    pk_timer_schedule(&g_pk_trace_timer,
                      PK_TRACE_TIMER_PERIOD);
}
#endif  // PK_TIMER_SUPPORT (timed callback support)

// Use this function to synchronize the timebase between multiple PPEs.
// PPE A can send PPE B it's current timebase and then PPE B can set that
// as the current timebase for tracing purposes.  It can also be used
// to set the current time to 0.  This function changes the timebase for
// all entries that are currently in the trace buffer.  Setting the current
// timebase to 0 will cause previous traces to have very large timestamps.
void pk_trace_set_timebase(PkTimebase timebase)
{
#ifdef APP_DEFINED_TRACE_BUFFER
    G_PK_TRACE_BUF->time_adj64 = timebase - pk_timebase_get();
#else
    g_pk_trace_buf.time_adj64 = timebase - pk_timebase_get();
#endif
}

void pk_trace_set_freq(uint32_t i_frequency)
{
#ifdef APP_DEFINED_TRACE_BUFFER
    G_PK_TRACE_BUF->hz = i_frequency;
#else
    g_pk_trace_buf.hz = i_frequency;
#endif
}
#endif  // PK_TRACE_SUPPORT
