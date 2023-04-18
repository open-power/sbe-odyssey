/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/odyTarget.C $  */
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
#include "plat_target_sbe.H"

namespace fapi2
{
    /*
     * Target map for Odyssey.
     */
     //struct targetInfo odyTargetMap;

    targetInfo_t odyTargetMap[] =
    {
        {0x0, LOG_SBE_ROOT_CHIP_TYPE, 0, 0, 1},
        {0x1, LOG_TARGET_TYPE_PERV, 1, 1, 1},
        // Adding second PERV target since chiplet number is not continuous.
        {0x8, LOG_TARGET_TYPE_PERV, 1, 1, 1},
        {0xD, LOG_TARGET_TYPE_MEM_PORT, 0, 1, 2},
        {0xD, LOG_TARGET_TYPE_DIMM, 0, 0, 2},
        {0xE, LOG_TARGET_TYPE_DIMM, 0, 0, 2},
        {0x0, LOG_TARGET_TYPE_TEMP_SENSOR, 0, 0, 4}, // First byte is port number
                                                    // for the TEMP SENSOR target.
    };

    targetInfo_t * getTargetMap()
    {
        return odyTargetMap;
    }
    uint32_t targetTypesCount = sizeof(odyTargetMap) / sizeof(odyTargetMap[0]);

    uint32_t getTargetTypesCount()
    {
        return targetTypesCount;
    }

}
