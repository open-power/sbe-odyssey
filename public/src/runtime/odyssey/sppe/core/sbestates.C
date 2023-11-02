/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbestates.C $            */
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
#include "sbestates.H"

const stateEventRangeStr_t eventRangePerState[SBE_MAX_STATE] =
{
    {SBE_STATE_CMN_UNKNOWN_ENTRY_TO_MAP, SBE_STATE_CMN_UNKNOWN_MAX_EVENT},
    {SBE_STATE_CMN_IPLING_ENTRY_TO_MAP, SBE_STATE_CMN_IPLING_MAX_EVENT},
    {SBE_STATE_CMN_ISTEP_ENTRY_TO_MAP, SBE_STATE_CMN_ISTEP_MAX_EVENT},
    {SBE_STATE_CMN_RUNTIME_ENTRY_TO_MAP, SBE_STATE_CMN_RUNTIME_MAX_EVENT},
    {SBE_STATE_CMN_DUMP_ENTRY_TO_MAP, SBE_STATE_CMN_DUMP_MAX_EVENT},
    {SBE_STATE_CMN_FAILURE_ENTRY_TO_MAP, SBE_STATE_CMN_FAILURE_MAX_EVENT},
};

const stateTransitionStr_t stateTransMap[SBE_MAX_TRANSITIONS] =
{
    {SBE_STATE_CMN_UNKNOWN, SBE_EVENT_CMN_FAILURE, SBE_STATE_CMN_FAILURE},
    {SBE_STATE_CMN_UNKNOWN, SBE_EVENT_CMN_RUNTIME, SBE_STATE_CMN_RUNTIME},
    {SBE_STATE_CMN_UNKNOWN, SBE_EVENT_CMN_ISTEP, SBE_STATE_CMN_ISTEP},
    {SBE_STATE_CMN_UNKNOWN, SBE_EVENT_CMN_PLCK, SBE_STATE_CMN_IPLING},
    {SBE_STATE_CMN_IPLING, SBE_EVENT_CMN_RUNTIME, SBE_STATE_CMN_RUNTIME},
    {SBE_STATE_CMN_IPLING, SBE_EVENT_CMN_DUMP_FAILURE, SBE_STATE_CMN_DUMP},
    {SBE_STATE_CMN_IPLING, SBE_EVENT_CMN_FAILURE, SBE_STATE_CMN_FAILURE},
    {SBE_STATE_CMN_ISTEP, SBE_EVENT_CMN_RUNTIME, SBE_STATE_CMN_RUNTIME},
    {SBE_STATE_CMN_ISTEP, SBE_EVENT_CMN_FAILURE, SBE_STATE_CMN_FAILURE},
    {SBE_STATE_CMN_RUNTIME, SBE_EVENT_CMN_DUMP_FAILURE, SBE_STATE_CMN_DUMP},
    {SBE_STATE_CMN_RUNTIME, SBE_EVENT_CMN_FAILURE, SBE_STATE_CMN_FAILURE},
};
