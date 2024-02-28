/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/fences.H $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#ifndef __FENCES_H__
#define __FENCES_H__

enum sbe_command_fence_attrs
{
    SBE_NO_FENCE                = 0x0000, ///< Allow cmd in all states
    SBE_FENCE_AT_DUMPING        = 0x0001, ///< Fence off at DUMPING State
    SBE_FENCE_AT_CONTINUOUS_IPL = 0x0004, ///< Fence off at cont IPL
    SBE_FENCE_AT_ISTEP          = 0x0008, ///< Fence off at istep state
    SBE_FENCE_AT_RUNTIME        = 0x0010, ///< Fence off at Runtime state
    SBE_FENCE_AT_SECURE_MODE    = 0x0020, ///< Fense off in secure mode
    SBE_FENCE_AT_AUTO_BOOT_DONE = 0x0040, ///< Fence off at autobot-done state
};

#endif
