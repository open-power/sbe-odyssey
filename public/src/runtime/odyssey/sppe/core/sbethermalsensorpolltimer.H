/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbethermalsensorpolltimer.H $ */
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
#ifndef __SBE_ASYNCTHREAD_TIMER_H__
#define __SBE_ASYNCTHREAD_TIMER_H__

#include "sbetimersvc.H"

// Async thread timer
extern timerService g_sbe_thermal_sensor_timer;

/**
  * @brief Callback function for the thermal sensor polling timer.
  *        This function will post a semaphore where the async
  *        thread will be waiting for this semaphore to execute
  *        thermal sensor polling. So that this callback will be
  *        returned very fast, and will not block a high-priority
  *        thread for a long time.
  *
  * @param[in]  This is a dummy void pointer to match with "PkTimerCallback".
  *             So that caller can pass a NULL pointer always..
  *
  * @return    void
  */
void sbeasyncthreadPkExpiryCallback(void *);
#endif
