/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/threadutil.C $                         */
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

#include "threadutil.H"
#include "sbetrace.H"

uint32_t createAndResumeThreadHelper(PkThread    *io_pThread,
                                PkThreadRoutine   i_thread_routine,
                                void             *io_pArg,
                                PkAddress         i_stack,
                                size_t            i_stack_size,
                                sbeThreadPriorities  i_priority)
{
    int rc = PK_OK;

    // Thread creation
    rc =  pk_thread_create(io_pThread,
                             i_thread_routine,
                             io_pArg,
                             i_stack,
                             i_stack_size,
                             (PkThreadPriority)i_priority);
    if(rc == PK_OK)
    {
        // resume the thread once created
        rc = pk_thread_resume(io_pThread);
    }

    // Check for errors creating or resuming the thread
    if(rc != PK_OK)
    {
        SBE_ERROR ("Failure creating/resuming thread, rc=[%d]", rc);
    }

    return rc;
}
