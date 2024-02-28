/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestates.H $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
/*
 * @file: ppe/sbe/sbefw/sbestates.H
 *
 * @brief This file contains interfaces pertaining to SBE state/role management
 *
 */

#ifndef __SBE_STATES_H
#define __SBE_STATES_H

/**
 * @brief An enumeration of all SBE states
 *
 */
enum sbeState
{
    /** Common states across all platforms **/
    SBE_STATE_CMN_UNKNOWN           = 0x0, // Unkown, initial state
    SBE_STATE_CMN_IPLING            = 0x1, // IPL'ing - autonomous mode (transient)
    SBE_STATE_CMN_ISTEP             = 0x2, // ISTEP - Running IPL by steps (transient)
    SBE_STATE_CMN_AUTO_BOOT_DONE    = 0x3, // SBE auto boot is done
    SBE_STATE_CMN_DUMP              = 0x4, // Dumping
    SBE_STATE_CMN_FAILURE           = 0x5, // Internal SBE failure
    SBE_STATE_CMN_RUNTIME           = 0x6, // SBE Runtime
    SBE_STATE_CMN_HRESET            = 0xA, // Hreset SBE state
                                           // This should not be changed due to SROM dependency

    /** Plat specific states across all platforms.
     *  -If any state defined below is not used by a platform implies that
     *   particular state even will not be triggerd and hence should not affect
     *   any state transition's for any platform
     *  -Max 16 states are possible. So we can have 9 more plat specific state's
     *   that can be defined below.
     *  -If we want more than 16 states we need to increase bit field to 5 bits for sbestates
     *   in sbe messaging reg(0x2809)**/

    // Max States, Always keep it at the last of the enum and sequential
    SBE_MAX_STATE     = 0xB,
    // Don't count this in the state, just to intialize the state variables
    SBE_INVALID_STATE = 0xF,
};

/**
 * @brief An enumeration of all SBE state transition events
 *
 */
enum sbeEvent
{
    /** Common Events across all platforms **/
    SBE_EVENT_CMN_AUTO_BOOT_DONE   = 0x0, // From Unknown/ISTEP/IPLING State
    SBE_EVENT_CMN_ISTEP            = 0x1, // From Unknown State
    SBE_EVENT_CMN_PLCK             = 0x2, // From Unknown state
    SBE_EVENT_CMN_DUMP_FAILURE     = 0x3, // From IPLING/RUNTIME/Unknown State
    SBE_EVENT_CMN_FAILURE          = 0x4, // From Any State

    /** Platforms specific events **/
};

#endif //__SBE_STATES_H

