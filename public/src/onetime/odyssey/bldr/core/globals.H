/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/odyssey/bldr/core/globals.H $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2022                        */
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
#ifndef __GLOBALS_H
#define __GLOBALS_H

//Define trace level for Boot Loader Image
#ifdef SBE_TRACE_LEVEL
#undef SBE_TRACE_LEVEL
#endif
#define SBE_TRACE_LEVEL 2

#define SBE_REF_BASE_FREQ_HZ ((133 * 1000 * 1000)/4)
#define SBE_PAU_DPLL_BASE_FREQ_HZ ((1968 * 1000 * 1000)/4)

//Lets set BLDR image bit as well while updating progress code
#define UPDATE_BLDR_SBE_PROGRESS_CODE(value) SBE::updateProgressCode(value);

////////////////////////////////////////////////////////////////
//// @brief Stacks for Non-critical Interrupts ( timebase, timers )
////////////////////////////////////////////////////////////////
#define INITIAL_PK_TIMEBASE   0
#define BLDR_NONCRITICAL_STACK_SIZE 512

//Keep stack size as 24kb. Required for ECDSA .
#define BLDR_THREAD_SECURE_BOOT_STACK_SIZE (24 * 1024)

#endif //__GLOBALS_H
