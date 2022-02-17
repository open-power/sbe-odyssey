/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/initthreads.C $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include <sbeglobals.H>
#include "sbetrace.H"

// Max defines for Semaphores
static uint32_t MAX_SEMAPHORE_COUNT = 3;

// As periodic timer expire in 24 hours count should never
// be more than 1.
static uint32_t MAX_PERIODIC_TIMER_SEMAPHORE_COUNT = 1;

uint8_t sbe_Kernel_NCInt_stack[SBE_NONCRITICAL_STACK_SIZE];
uint8_t sbeCommandReceiver_stack[SBE_THREAD_CMD_RECV_STACK_SIZE];
uint8_t sbeSyncCommandProcessor_stack[SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE];
uint8_t sbeAsyncCommandProcessor_stack[SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE];

uint32_t sbeInitSems(void)
{
    SBE_ENTER("sbeInitSems");
    int l_rc = PK_OK;

    do
    {
        l_rc = pk_semaphore_create(&SBE_GLOBAL->sbeSemCmdRecv, 0, MAX_SEMAPHORE_COUNT);
        if (l_rc)
        {
            break;
        }
        l_rc = pk_semaphore_create(&SBE_GLOBAL->sbeSemCmdProcess, 0, MAX_SEMAPHORE_COUNT);
        if (l_rc)
        {
            break;
        }
        l_rc = pk_semaphore_create(&SBE_GLOBAL->sbeSemAsyncProcess, 0,
                                         MAX_PERIODIC_TIMER_SEMAPHORE_COUNT);
        if (l_rc)
        {
            break;
        }
        #ifdef SBE_CONSOLE_SUPPORT
        l_rc = pk_semaphore_create(&SBE_GLOBAL->sbeUartBinSem, 1, 1);
        if (l_rc)
        {
            break;
        }
        #endif
    } while (false);

    if (l_rc)
    {
        SBE_ERROR ("pk_semaphore_create, rc=[%d]", l_rc);
    }
    return l_rc;
}

////////////////////////////////////////////////////////////////
int sbeInitThreads(void)
{
    // Locals
    uint32_t l_rc         = PK_OK;

    do
    {
        // Initialize Command receiver thread
        l_rc = createAndResumeThreadHelper(&SBE_GLOBAL->sbeCommandReceiver_thread,
                            sbeCommandReceiver_routine,
                            (void *)0,
                            (PkAddress)sbeCommandReceiver_stack,
                            SBE_THREAD_CMD_RECV_STACK_SIZE,
                            THREAD_PRIORITY_5);
        if (l_rc)
        {
            break;
        }
        // Initialize Synchronous Command Processor thread
        l_rc = createAndResumeThreadHelper(&SBE_GLOBAL->sbeSyncCommandProcessor_thread,
                            sbeSyncCommandProcessor_routine,
                            (void *)0,
                            (PkAddress)sbeSyncCommandProcessor_stack,
                            SBE_THREAD_SYNC_CMD_PROC_STACK_SIZE,
                            THREAD_PRIORITY_6);
        if (l_rc)
        {
            break;
        }

        // Initialize Asynchronous Command Processor thread
        l_rc = createAndResumeThreadHelper(&SBE_GLOBAL->sbeAsyncCommandProcessor_thread,
                            sbeAsyncCommandProcessor_routine,
                            (void *)0,
                            (PkAddress)sbeAsyncCommandProcessor_stack,
                            SBE_THREAD_ASYNC_CMD_PROC_STACK_SIZE,
                            THREAD_PRIORITY_7);
        if (l_rc)
        {
            break;
        }
    } while (false);

    // If there are any errors initializing the threads
    if( l_rc )
    {
        SBE_ERROR ("Error Initializing a thread, rc=[%d]", l_rc);
    }

    return l_rc;
}
