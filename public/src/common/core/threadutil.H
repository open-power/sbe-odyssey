/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/threadutil.H $                         */
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

#pragma once

#include "sbeexeintf.H"

/**
 * @brief Struct containing all info to create and resume a thread
 *
 * io_thread  A pointer to an PkThread structure to initialize
 * i_thread_routine The subroutine that implements the thread
 * io_arg     Private data to be passed as the argument to the
 *               thread routine when it begins execution
 * i_stack    The stack space of the thread
 * i_stack_size The size of the stack in bytes
 * i_priority The initial priority of the thread
 *
 */
typedef struct ThreadsDefinition
{
    PkThread *           io_pThread;
    PkThreadRoutine      i_thread_routine;
    void *               io_pArg;
    PkAddress            i_stack;
    size_t               i_stack_size;
    sbeThreadPriorities  i_priority;
}sThread_t;


/**
 * @brief Structure containing all info to create semaphores
 */
typedef struct SemaphoresDefinition
{
    PkSemaphore *    pSemaHandler; /**< Pointer Semaphore handler */
    PkSemaphoreCount initialCount; /**< initial Count for semaphore */
    PkSemaphoreCount maxCount;     /**< maximum count for semaphore */
}sSema_t;


////////////////////////////////////////////////////////////////
/// @brief  createAndResumeThreadHelper
///            - Create and resume the given thread
///
/// @param[in]     i_Threads Array of struct containing all params for
///                              creating threads
///
/// @param[in]     i_NoOfThreads Number of threads to create and resume
///
/// @return        PK_OK Successfully created and resumed the thread
///
/// @return        PK_INVALID_THREAD_AT_CREATE io_thread is null
/// @return        PK_INVALID_ARGUMENT_THREAD1 i_thread_routine is null
/// @return        PK_INVALID_ARGUMENT_THREAD2 i_priority is invalid
/// @return        PK_INVALID_ARGUMENT_THREAD3 the stack area wraps around
///                                      the end of memory.
/// @return        PK_STACK_OVERFLOW           The stack area at thread creation
///                                      is smaller than the min safe size
/// @return        PK_INVALID_THREAD_AT_RESUME1 io_thread is null (unlikely)
/// @return        PK_INVALID_THREAD_AT_RESUME2 The thread is not active,
///                                      i.e. has completed or been deleted,
/// @return        PK_PRIORITY_IN_USE_AT_RESUME Another thread is already
///                                      mapped at the priority of the thread
////////////////////////////////////////////////////////////////
uint32_t ThreadsCreate( const sThread_t * i_Threads, uint8_t i_NoOfThreads);


/**
 * @brief Function for create the semaphore in one shot
 *
 * @param i_SemaData Array of @ref sSema_t struct pointer, which contain
 *                   max number of semaphore data
 * @param i_NoOfSema Maximum number of semaphore to be initialize
 * @return uint32_t 0 Successful completion
 * -PK_INVALID_ARGUMENT_SEMAPHORE The \a spSemaData is null pointer or
 *  u32NoOfSema is zero
 */
uint32_t SemaCreate ( const sSema_t * i_SemaData, uint32_t i_NoOfSema);
