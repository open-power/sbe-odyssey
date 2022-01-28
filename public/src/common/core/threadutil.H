/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/threadutil.H $                         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

#ifndef THREAD_UTIL_H
#define THREAD_UTIL_H

#include "threadutil.H"
#include "sbeexeintf.H"

////////////////////////////////////////////////////////////////
// @brief  createAndResumeThreadHelper
//            - Create and resume the given thread
//
// @param[in/out] io_thread  A pointer to an PkThread structure to initialize
// @param[in]     i_thread_routine The subroutine that implements the thread
// @param[in/out] io_arg     Private data to be passed as the argument to the
//                              thread routine when it begins execution
// @param[in]     i_stack    The stack space of the thread
// @param[in]     i_stack_size The size of the stack in bytes
// @param[in]     i_priority The initial priority of the thread
//
// @return        PK_OK Successfully created and resumed the thread
//
// @return        PK_INVALID_THREAD_AT_CREATE io_thread is null
// @return        PK_INVALID_ARGUMENT_THREAD1 i_thread_routine is null
// @return        PK_INVALID_ARGUMENT_THREAD2 i_priority is invalid
// @return        PK_INVALID_ARGUMENT_THREAD3 the stack area wraps around
//                                      the end of memory.
// @return        PK_STACK_OVERFLOW           The stack area at thread creation
//                                      is smaller than the min safe size
// @return        PK_INVALID_THREAD_AT_RESUME1 io_thread is null (unlikely)
// @return        PK_INVALID_THREAD_AT_RESUME2 The thread is not active,
//                                      i.e. has completed or been deleted,
// @return        PK_PRIORITY_IN_USE_AT_RESUME Another thread is already
//                                      mapped at the priority of the thread
////////////////////////////////////////////////////////////////
uint32_t createAndResumeThreadHelper(PkThread    *io_pThread,
                                PkThreadRoutine   i_thread_routine,
                                void             *io_pArg,
                                PkAddress         i_stack,
                                size_t            i_stack_size,
                                sbeThreadPriorities  i_priority);

#endif //THREAD_UTIL_H
