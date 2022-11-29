/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/threadutil.C $                         */
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

#include "threadutil.H"
#include "sbetrace.H"

/**
 * @brief Create the PK thread in the loop
 */
uint32_t ThreadsCreate( sThread_t * i_Threads, uint8_t i_NoOfThreads)
{
    #define SBE_FUNC "ThreadsCreate "
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = PK_OK;
    volatile const sThread_t * pthread = i_Threads;


    do
    {
        /* Assert Check */
        if ( (pthread ==  NULL) || (i_NoOfThreads == 0))
        {
            rc = PK_INVALID_ARGUMENT_THREAD1;
            SBE_ERROR (SBE_FUNC "Invalid arg's");
            break;
        }

        for (uint32_t i = 0; i < i_NoOfThreads; i++)
        {
            SBE_INFO(SBE_FUNC "Thread Creating, No:%d, stack size: %d",
                                                i , pthread[i].i_stack_size);
            // Thread creation
            rc =  pk_thread_create( pthread[i].io_pThread,
                                    pthread[i].i_thread_routine,
                                    pthread[i].io_pArg,
                                    (PkAddress) pthread[i].i_stack,
                                    pthread[i].i_stack_size,
                                    (PkThreadPriority) pthread[i].i_priority);

            if (rc != PK_OK)
            {
                SBE_ERROR (SBE_FUNC "Failure creating thread, "
                          "Thread Number: [0x%X], rc:[0x%X]", i, rc);
                break;
            }

            // resume the thread once created
            rc = pk_thread_resume(pthread[i].io_pThread);

            // Check for errors creating or resuming the thread
            if (rc != PK_OK)
            {
                SBE_ERROR (SBE_FUNC "Failure resuming thread, rc=[%d]", rc);
                break;
            }
        }
    } while (0);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}


uint32_t SemaCreate ( sSema_t * i_SemaData, uint32_t i_NoOfSema)
{
    #define SBE_FUNC "SemaCreate "
    SBE_ENTER(SBE_FUNC);
    int rc = PK_OK;
    volatile const sSema_t * pSema = i_SemaData;

    do
    {
        /* Assert Check */
        if ( (pSema == NULL) || (i_NoOfSema ==0) )
        {
            rc = PK_INVALID_ARGUMENT_SEMAPHORE;
            SBE_ERROR (SBE_FUNC "Invalid arg's");
            break;
        }

        for (uint32_t i = 0; i < i_NoOfSema; i++)
        {
            SBE_INFO( "Semaphore Creating, No:%d, max Count: %d", i, pSema[i].maxCount);

            // Sema creation
            rc =  pk_semaphore_create ( pSema[i].pSemaHandler,
                                        pSema[i].initialCount,
                                        pSema[i].maxCount );

            // Check for errors creating semaphore
            if (rc != PK_OK)
            {
                SBE_ERROR ("Failure creating sema, rc=[%d]", rc);
                break;
            }
        }
    } while (0);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
