/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbethermalsensorpolltimer.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "sbethermalsensorpolltimer.H"
#include "sbetrace.H"
#include "sbeglobals.H"

//Initialization of the async thread Pk timer
timerService g_sbe_thermal_sensor_timer;

void sbeasyncthreadPkExpiryCallback(void *)
{
    #define SBE_FUNC " sbeasyncthreadPkExpiryCallback "
    SBE_INFO( SBE_FUNC "Async thread timer has expired. Async thread will be posted");

    // Post the Async thread.
    uint32_t l_rc = pk_semaphore_post(&SBE_GLOBAL->sbeSemAsyncProcess);
    if(l_rc)
    {
        SBE_ERROR(SBE_FUNC "Failed to post the thread with rc 0x%08X", l_rc);
        pk_halt();
    }
    #undef SBE_FUNC
}
