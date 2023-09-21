/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/initthreads.C $                */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#include "initthreads.H"
#include "threadutil.H"
#include <stdint.h>
#include "sbeglobals.H"
#include "sbetrace.H"

// Max defines for Semaphores
static uint32_t MAX_SEMAPHORE_COUNT = 3;

// As periodic timer expire in 24 hours count should never
// be more than 1.
static uint32_t MAX_PERIODIC_TIMER_SEMAPHORE_COUNT = 1;

uint8_t sbe_Kernel_NCInt_stack[SBE_NONCRITICAL_STACK_SIZE] __attribute__((aligned(8)));
uint8_t sbeCommandReceiver_stack[SBE_THREAD_CMD_RECV_STACK_SIZE] __attribute__((aligned(8)));
uint8_t sbeSyncCommandProcessor_stack[SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE] __attribute__((aligned(8)));
uint8_t sbeAsyncCommandProcessor_stack[SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE] __attribute__((aligned(8)));

uint32_t sbeInitSems(void)
{
    static const sSema_t semaList[ ] __attribute__((aligned(8))) =
    {
        {
            &SBE_GLOBAL->semphores.sbeSemCmdRecv,
            0,
            MAX_SEMAPHORE_COUNT
        },

        {
            &SBE_GLOBAL->semphores.sbeSemCmdProcess,
            0,
            MAX_SEMAPHORE_COUNT
        },

        {
            &SBE_GLOBAL->semphores.sbeSemAsyncProcess,
            0,
            MAX_PERIODIC_TIMER_SEMAPHORE_COUNT
        },
    };

    /* Initialize the Semaphore */
    return (SemaCreate (semaList, (sizeof(semaList)/sizeof(sSema_t)) ));
}


////////////////////////////////////////////////////////////////
int sbeInitThreads(void)
{

    static const sThread_t threadList[ ] __attribute__((aligned(8))) =
    {
        {
            &SBE_GLOBAL->threads.sbeCommandReceiver_thread,
            sbeCommandReceiver_routine,
            (void *)0,
            (PkAddress) &sbeCommandReceiver_stack,
            SBE_THREAD_CMD_RECV_STACK_SIZE,
            THREAD_PRIORITY_5
        },

        {
            &SBE_GLOBAL->threads.sbeSyncCommandProcessor_thread,
            sbeSyncCommandProcessor_routine,
            (void *)0,
            (PkAddress) sbeSyncCommandProcessor_stack,
            SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE,
            THREAD_PRIORITY_6
        },
#ifndef OMIT_ASYNC_COMMAND_PROCESSOR
        {
            &SBE_GLOBAL->threads.sbeAsyncCommandProcessor_thread,
            sbeAsyncCommandProcessor_routine,
            (void *)0,
            (PkAddress) sbeAsyncCommandProcessor_stack,
            SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE,
            THREAD_PRIORITY_7
        },
#endif
    };

    // Initialize SBE control loop threads
    return (ThreadsCreate (threadList, (sizeof(threadList)/sizeof(sThread_t))));
}
