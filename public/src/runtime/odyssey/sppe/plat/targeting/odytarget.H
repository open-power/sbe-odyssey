/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/odytarget.H $  */
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
/**
 * @file target.H
 * @brief platform specializations for fapi2 targets
 */

#ifndef __FAPI2_TARGET__
#define __FAPI2_TARGET__

#include <plat_target.H>
#include <fapi2_target.H>
#include <utils.H>
#include <stdint.h>
#include <vector>
#include <plat_target_utils.H>
#include <plat_target_base.H>

extern std::vector<fapi2::plat_target_handle_t> G_vec_targets;

namespace fapi2
{
    template<TargetType T>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType()
    {
        static_assert(T == TARGET_TYPE_PROC_CHIP, "Invalid or composite target type");
        return PPE_TARGET_TYPE_NONE;
    }

    template<>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType<TARGET_TYPE_PROC_CHIP>()
    {
        return PPE_TARGET_TYPE_PROC_CHIP;
    }
}

#endif
