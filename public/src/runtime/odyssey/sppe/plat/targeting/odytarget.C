/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/odytarget.C $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2022                        */
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


#include <target_types.H>
#include <vector>
#include <plat_target.H>
#include <plat_utils.H>
#include <plat_target_base.H>

// Global Vector containing ALL targets.  This structure is referenced by
// fapi2::getChildren to produce the resultant returned vector from that
// call.
std::vector<fapi2::plat_target_handle_t> G_vec_targets;

namespace fapi2
{
    plat_target_odyssey_handle l_handle;
    template<TargetType K>
    plat_target_handle_t createPlatTargetHandle(const uint32_t i_plat_argument)
    {
        static_assert(K != TARGET_TYPE_ALL, "Target instances cannot be of type ALL");

        if(K & TARGET_TYPE_PROC_CHIP)
        {
            plat_target_odyssey_handle l_odyTarget(0, PPE_TARGET_TYPE_PROC_CHIP);
            l_handle = l_odyTarget;
        }
        else
        {
            STATIC_COMPILE_ERROR("Unhandled target type");
        }
        return l_handle;
    }

    /// @brief Function to initialize the G_targets vector based on target
    ///        values as per odyssey attributes
    uint32_t plat_TargetsInit()
    {
        plat_target_handle_t l_platHandle;

        uint32_t l_beginning_offset;
        /*
         * Chip Target is the first one
         */
        l_beginning_offset = 0; // CHIP_TARGET_OFFSET;
        G_vec_targets[l_beginning_offset] = createPlatTargetHandle<fapi2::TARGET_TYPE_PROC_CHIP>(0);

        return 0;
    }
} // fapi2
