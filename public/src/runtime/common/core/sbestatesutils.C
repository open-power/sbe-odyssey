/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestatesutils.C $             */
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

#include "sbestatesutils.H"
#include "sbetrace.H"
#include "sberegaccess.H"

//NOTE: User could use plat specific implememntation of eventRangePerState and stateTransMap
//      or from common.
#include "sbestates.H"

void stateTransition(const uint8_t &i_event)
{
    #define SBE_FUNC " stateTransition "

    //Fetch Current State
    uint32_t l_state = (uint32_t)SbeRegAccess::theSbeRegAccess().getSbeState();
    uint8_t l_startCnt = eventRangePerState[l_state].start;
    SBE_INFO(SBE_FUNC "Event Received %d CurrState 0x%08X StartCnt%d EndCnt%d",
        i_event, l_state, l_startCnt, eventRangePerState[l_state].end);
    // Fetch the final State from the Map
    while(l_startCnt <
          (eventRangePerState[l_state].end + eventRangePerState[l_state].start))
    {
        if(stateTransMap[l_startCnt].event == i_event)
        {
            SBE_INFO(SBE_FUNC "Updating State as %d",
                        (sbeState)stateTransMap[l_startCnt].finalState);
            SbeRegAccess::theSbeRegAccess().updateSbeState((sbeState)stateTransMap[l_startCnt].finalState);
            break;
        }
        else
            ++l_startCnt;
    }

    #undef SBE_FUNC
}
