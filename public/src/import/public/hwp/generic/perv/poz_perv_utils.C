/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_utils.C $  */
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
//------------------------------------------------------------------------------
/// @file  poz_perv_utils.C
/// @brief Utility function support for pervasive HWP code
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy <skadapal@in.ibm.com>
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_perv_utils.H>
#include <poz_perv_mod_misc.H>
#include <poz_ring_ids.H>
#include <target_filters.H>
#include <assert.h>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
using namespace fapi2;

Target<TARGET_TYPE_PERV> get_tp_chiplet_target(const Target<TARGET_TYPE_CHIPS> i_chip)
{
    auto l_children = i_chip.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_TP, TARGET_STATE_PRESENT);
    // The TP chiplet should always be present so just use
    // assert() instead of FAPI_ASSERT() as a sanity check.
    assert(!l_children.empty());
    return l_children[0];
}

uint64_t get_mc_group_members(const Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > & i_target)
{
    buffer<uint64_t> l_result;

    for (auto cplt : i_target.getChildren<TARGET_TYPE_PERV>(TARGET_STATE_PRESENT))
    {
        l_result.setBit(cplt.getChipletNumber());
    }

    return l_result;
}

uint16_t get_ring_region(const uint32_t i_ring_address)
{
    return ((i_ring_address & 0x0000FFF0) | ((i_ring_address & 0x00F00000) >> 20)) << 1;
}

ReturnCode get_hotplug_mask(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint64_t& o_hotplug_mask)
{
    Target<TARGET_TYPE_SYSTEM> l_system_target;
    uint8_t l_hotplug;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_HOTPLUG, l_system_target, l_hotplug));

    if (l_hotplug)
    {
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_HOTPLUG_MASK, i_target, o_hotplug_mask));
    }
    else
    {
        o_hotplug_mask = -1ULL;
    }

fapi_try_exit:
    return current_err;
}

ReturnCode get_hotplug_targets(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    Target < TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST > &o_chiplets_mc,
    std::vector<Target<TARGET_TYPE_PERV>>* o_chiplets_uc,
    fapi2::MulticastGroup i_non_hp_group)
{
    Target<TARGET_TYPE_SYSTEM> l_system_target;
    uint8_t l_hotplug;

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_HOTPLUG, l_system_target, l_hotplug));

    if (!l_hotplug && (i_non_hp_group != MCGROUP_ALL))
    {
        o_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(i_non_hp_group);
    }
    else
    {
        TargetState l_target_state = TARGET_STATE_PRESENT;
        uint64_t l_chiplet_mask = 0xFFFFFFFFFFFFFFFFULL;

        if (l_hotplug)
        {
            FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_HOTPLUG_MASK, i_target, l_chiplet_mask));
            l_target_state = TARGET_STATE_FUNCTIONAL;
        }

        FAPI_INF("Set up hotplug groups - l_hotplug=%d l_chiplet_mask=0x%08x%08x",
                 l_hotplug, l_chiplet_mask >> 32, l_chiplet_mask & 0xFFFFFFFF);
        FAPI_TRY(mod_multicast_setup(i_target, MCGROUP_5, l_chiplet_mask & 0x3FFFFFFFFFFFFFFF, l_target_state));
        o_chiplets_mc = i_target.getMulticast<TARGET_TYPE_PERV>(MCGROUP_5);
    }

    if (o_chiplets_uc)
    {
        *o_chiplets_uc = o_chiplets_mc.getChildren<TARGET_TYPE_PERV>(TARGET_STATE_PRESENT);
    }

fapi_try_exit:
    return current_err;
}

static char hex[] = "0123456789abcdef";

void strhex(char* o_str, uint64_t i_value, size_t i_width)
{
    while (i_width)
    {
        i_width--;
        o_str[i_width] = hex[i_value & 0xF];
        i_value >>= 4;
    }
}

ReturnCode putRingBucket(const Target<TARGET_TYPE_ALL_MC>& i_target,
                         const char* i_ring_id,
                         int i_bucket,
                         const RingMode i_ringMode)
{
    char ring_id[RING_ID_MAXLEN];
    auto len = strlen(i_ring_id);
    memcpy(ring_id, i_ring_id, len + 1);
    strhex(ring_id + len - 2, i_bucket, 2);
    return putRing(i_target, ring_id, i_ringMode);
}
