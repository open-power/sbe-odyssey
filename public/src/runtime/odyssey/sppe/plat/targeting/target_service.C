/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/target_service.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
/**
 * @file plat_target.H
 * @brief platform definitions for fapi2 targets
 */

#include "plat_target_base.H"
#include "target.H"
#include "target_service.H"
#include <fapi2_attribute_service.H>

fapi2::target_service * g_platTarget = &fapi2::target_service::getInstance();
fapi2::target_service fapi2::target_service::iv_instance;

void fapi2::target_service::plat_TargetStateUpdateFromAttribute()
{
#define SBE_FUNC "plat_TargetStateUpdateFromAttribute "
    SBE_ENTER(SBE_FUNC);

    // set functional state for TARGET_TYPE_MEM_PORT, TARGET_TYPE_DIMM, TARGET_TYPE_TEMP_SENSOR
    //    using attribute value ATTR_SPPE_TARGET_STATE
    for(uint16_t j = 0; j < iv_targets.size(); j++)
    {
        uint8_t l_state = 0;
        uint8_t l_targ_inst = iv_targets[j].getTargetInstance();
        if(iv_targets[j].getTargetType() == fapi2::LOG_TARGET_TYPE_MEM_PORT)
        {
            l_state = fapi2::ATTR::TARGET_TYPE_MEM_PORT::ATTR_SPPE_TARGET_STATE[l_targ_inst];
        }
        else if(iv_targets[j].getTargetType() == fapi2::LOG_TARGET_TYPE_DIMM)
        {
            l_state = fapi2::ATTR::TARGET_TYPE_DIMM::ATTR_SPPE_TARGET_STATE[l_targ_inst];
        }
        else if(iv_targets[j].getTargetType() == fapi2::LOG_TARGET_TYPE_TEMP_SENSOR)
        {
            l_state = fapi2::ATTR::TARGET_TYPE_TEMP_SENSOR::ATTR_SPPE_TARGET_STATE[l_targ_inst];
        }
        else
        {
            // ATTR_SPPE_TARGET_STATE is not available for other targets
            continue;
        }

        // set present if either TARGET_STATE_PRESENT or TARGET_STATE_FUNCTIONAL
        iv_targets[j].setPresent(
            (l_state == fapi2::TARGET_STATE_PRESENT) || (l_state == fapi2::TARGET_STATE_FUNCTIONAL));

        iv_targets[j].setFunctional(l_state == fapi2::TARGET_STATE_FUNCTIONAL);
    }
#undef SBE_FUNC
}
