/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/generic/ppe/pk/kernel/pk_bh_core.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2025                        */
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

/// \file pk_bh_core.c
/// \brief PK bottom half APIs
///
///  The entry points in this file are considered 'core' routines that will
///  always be present at runtime in any PK application.

#include "pk.h"

/// Statically initialize the bottom half queue
PK_DEQUE_SENTINEL_STATIC_CREATE(_pk_bh_queue);

// This function is called from interrupt context with interrupts disabled.
// It is responsible for enabling interrupts while a bh handler is running.
void _pk_process_bh(void)
{
    PkBottomHalf* bh;
    PkMachineContext ctx;

    // Queue checks must run with interrupts disabled:
    // - If an interrupt jumpscares us in the middle of removing an
    //   entry from the queue and then tries to add a new entry,
    //   it will corrupt the queue structure.
    // - If the interrupt hits just as we decided that we're done,
    //   we'll resume that timeline after the interrupt, return and
    //   miss the queue entry that just got added.
    while((bh = (PkBottomHalf*)pk_deque_pop_front(&_pk_bh_queue)) != 0)
    {
        pk_interrupt_enable(&ctx);
        bh->bh_handler(bh->arg);
        pk_interrupt_disable(&ctx);
    }

    return;
}


#undef __PK_THREAD_CORE_C__
