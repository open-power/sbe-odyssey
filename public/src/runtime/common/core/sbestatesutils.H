/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestatesutils.H $             */
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
#ifndef __SBE_STATES_UTILS_H
#define __SBE_STATES_UTILS_H

#include "sbestates.H"
#include <stdint.h>

// Struct to Map Current State - Event - Final State Transition
typedef struct stateTransitionStr
{
    uint16_t currState:4;
    uint16_t event:4;
    uint16_t finalState:4;
    uint16_t reserved:4;
} stateTransitionStr_t;

// Start and End point of Event Transition in stateTransMap Table
typedef struct stateEventRangeStr
{
    uint16_t start:8;
    uint16_t end:8;
}stateEventRangeStr_t;


// Maximum number of Events per State
enum maxEventPerState
{
    /** Common Max Events per states across all platforms **/
    SBE_STATE_CMN_UNKNOWN_MAX_EVENT = 4,
    SBE_STATE_CMN_IPLING_MAX_EVENT  = 3,
    SBE_STATE_CMN_ISTEP_MAX_EVENT   = 2,
    SBE_STATE_CMN_RUNTIME_MAX_EVENT = 2,
    SBE_STATE_CMN_DUMP_MAX_EVENT    = 0,
    SBE_STATE_CMN_FAILURE_MAX_EVENT = 0,

    /** Platforms specific Max Events per states across all platforms(including common) **/

    // Total number of State Transition Events, Addition of all the above(common+platform)
    SBE_MAX_TRANSITIONS = 11,
};


// Entry Point to stateTransitionStr_t Map Structure, This adds up all the state
// transition of the previous state, If any more transition is added, this will
// add up to all subsequent entries. This is closely mapped with the sbestates.H
// as well.
enum entryToStateMap
{
    SBE_STATE_CMN_UNKNOWN_ENTRY_TO_MAP = 0,
    SBE_STATE_CMN_IPLING_ENTRY_TO_MAP  = SBE_STATE_CMN_UNKNOWN_ENTRY_TO_MAP + SBE_STATE_CMN_UNKNOWN_MAX_EVENT, //4
    SBE_STATE_CMN_ISTEP_ENTRY_TO_MAP   = SBE_STATE_CMN_IPLING_ENTRY_TO_MAP + SBE_STATE_CMN_IPLING_MAX_EVENT, //7
    SBE_STATE_CMN_RUNTIME_ENTRY_TO_MAP = SBE_STATE_CMN_ISTEP_ENTRY_TO_MAP + SBE_STATE_CMN_ISTEP_MAX_EVENT, //9
    SBE_STATE_CMN_DUMP_ENTRY_TO_MAP    = SBE_STATE_CMN_RUNTIME_ENTRY_TO_MAP + SBE_STATE_CMN_RUNTIME_MAX_EVENT, //11
    SBE_STATE_CMN_FAILURE_ENTRY_TO_MAP = SBE_STATE_CMN_DUMP_ENTRY_TO_MAP + SBE_STATE_CMN_DUMP_MAX_EVENT, //11
};

extern const stateEventRangeStr_t eventRangePerState[SBE_MAX_STATE];
extern const stateTransitionStr_t stateTransMap[SBE_MAX_TRANSITIONS];


/**
 * @brief Update the SBE State as per the transition event
 *
 * @param [in] i_event Transition Event
 */
void stateTransition(const uint8_t &i_event);

#endif //__SBE_STATES_UTILS_H