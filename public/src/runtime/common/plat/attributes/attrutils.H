/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/attrutils.H $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "attribute_properties.H"

// TODO : This enum is defined in sbe_targets.H and has been temporarily
//        copied here. When the commit :
//           https://rchgit01.rchland.ibm.com/gerrit1/#/c/155120/ is
//        merged to p11-ekb and then mirrored to p11-sbe, remove the below
//        enum and include sbe_targets.H.
/**
 *  @brief  Enum to be used while sending a target info to SBE along with
 *          fapi2::target_type and target instance. Will be used in SBE
 *          interface like sbe-attribute library or sbei library.
 *
 */
enum SbeChipType_t : uint8_t
{
    CHIP_TYPE_ANY,          ///< Any chip (will be used for TARGET_TYPE_SYSTEM)
    CHIP_TYPE_ODYSSEY_00,   ///< Odyssey chip 0
    CHIP_TYPE_ODYSSEY_01,   ///< Odyssey chip 1
    CHIP_TYPE_ODYSSEY_02,   ///< Odyssey chip 2
    CHIP_TYPE_ODYSSEY_03,   ///< Odyssey chip 3
    CHIP_TYPE_ODYSSEY_04,   ///< Odyssey chip 4
    CHIP_TYPE_ODYSSEY_05,   ///< Odyssey chip 5
    CHIP_TYPE_ODYSSEY_06,   ///< Odyssey chip 6
    CHIP_TYPE_ODYSSEY_07,   ///< Odyssey chip 7
    CHIP_TYPE_ODYSSEY_08,   ///< Odyssey chip 8
    CHIP_TYPE_ODYSSEY_09,   ///< Odyssey chip 9
    CHIP_TYPE_ODYSSEY_10,   ///< Odyssey chip 10
    CHIP_TYPE_ODYSSEY_11,   ///< Odyssey chip 11
    CHIP_TYPE_ODYSSEY_12,   ///< Odyssey chip 12
    CHIP_TYPE_ODYSSEY_13,   ///< Odyssey chip 13
    CHIP_TYPE_ODYSSEY_14,   ///< Odyssey chip 14
    CHIP_TYPE_ODYSSEY_15,   ///< Odyssey chip 15
};

/**
 * @brief This file contains APIs that are specific to project
 *        Project can implement these APIs in plat/attributes/attrutils.C
 *
 */

/**
 * @brief Return the ChipType based on SbeChipType +
 *                   TARGET_TYPE_OCMB_CHIP::ATTR_BUS_POS
 *
 * return ChipType of the SBE instance
 */
uint8_t platGetChipType();

