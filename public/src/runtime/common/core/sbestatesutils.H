/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestatesutils.H $             */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

/**
 * @brief Update the SBE State as per the transition event
 *
 * @param [in] i_event Transition Event
 */
void stateTransition(const uint8_t &i_event);

#endif //__SBE_STATES_UTILS_H