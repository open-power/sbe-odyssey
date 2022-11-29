/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/initthreads.H $          */
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

#include <stdint.h>
////////////////////////////////////////////////////////////////
//// @brief   sbeInitThreads
////          Create and resume for all the firmware threads
////
//// @return  See createAndResumeThreadHelper for more details
////////////////////////////////////////////////////////////////
int sbeInitThreads(void);


////////////////////////////////////////////////////////////////
//// @brief  sbeInitSems - Create the necessary semaphores
////
//// @return  PK_OK                          - Success
////         PK_INVALID_SEMAPHORE_AT_CREATE - Invalid PkSemaphore
////         PK_INVALID_ARGUMENT_SEMAPHORE  - max_count is non-zero
////                               and less than the initial_count
//////////////////////////////////////////////////////////////////
uint32_t sbeInitSems(void);
