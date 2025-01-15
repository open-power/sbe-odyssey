/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/sbe_utils/include/sbe_chip_type.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2025                             */
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

// @file sbe_chip_types.H
// @brief SBE chip types
//
// *HWP HWP Owner: P Rajees <rajerpp1@in.ibm.com>
// *HWP HWP Backup: T Rajeswaran <rajeswgo@in.ibm.com>
// *HWP Team: SBE
// *HWP Consumed by: Cronus:HB
//
#ifndef __SBE_CHIP_TYPE__
#define __SBE_CHIP_TYPE__

#include <cstdint>

namespace sbeutil
{
/**
 *  @brief  Enum to be used while sending a target info to SBE along with
 *          fapi2::target_type and target instance. Will be used in SBE interface
 *          like sbe-attribute library or sbei library.
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
    CHIP_TYPE_COMPUTE_00,   ///< Compute chip 0
    CHIP_TYPE_COMPUTE_01,   ///< Compute chip 1
    CHIP_TYPE_COMPUTE_02,   ///< Compute chip 2
    CHIP_TYPE_COMPUTE_03,   ///< Compute chip 3
    CHIP_TYPE_COMPUTE_04,   ///< Compute chip 4
    CHIP_TYPE_COMPUTE_05,   ///< Compute chip 5
    CHIP_TYPE_COMPUTE_06,   ///< Compute chip 6
    CHIP_TYPE_COMPUTE_07,   ///< Compute chip 7
    CHIP_TYPE_HUB,          ///< Hub chip
};

} // namespace sbe_utils

#endif
