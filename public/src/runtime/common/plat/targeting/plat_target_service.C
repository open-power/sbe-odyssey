/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_service.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "plat_target_base.H"
#include "plat_target_service.H"
#include <fapi2.H>

using namespace fapi2;

// The only shared clock region definition is the PERV region,
// which is on region 0 for all chiplets on all chips. Any other
// region is chip specific and should be defined in the chip
// specific targeting implementation.
static const uint16_t CLOCK_REGION_PERV = 0x8000;

void sbe_target_service::plat_PrintTargets()
{
    uint32_t targetCnt = plat_getTargetCount();
    for(uint32_t i = 0; i < targetCnt; i++)
    {
        SBE_INFO("sbe_target_service 0x%08X p%d f%d", iv_targets[i].iv_value(),
                 iv_targets[i].getPresent(), iv_targets[i].getFunctional());
    }
}

ReturnCode sbe_target_service::plat_TargetsInit()
{
    SBE_INFO("sbe_target_service plat_TargetsInit()");

    for (auto &targetInfo : G_projTargetMap)
    {
        SBE_DEBUG("target count is 0x%08X", (uint32_t)targetInfo.targetCnt);
        for(uint32_t j = 0; j < targetInfo.targetCnt; j++)
        {
            const uint8_t chipletNum = targetInfo.isChiplet ?
                targetInfo.chipletNum + j :
                targetInfo.chipletNum;

            const uint8_t instanceNum = j + targetInfo.instanceBase;

            iv_targets.push_back(
                plat_target_sbe_handle(
                    chipletNum, targetInfo.targetType, instanceNum));
        }
    }

    plat_PrintTargets();
    return FAPI2_RC_SUCCESS;
}

uint32_t sbe_target_service::plat_getTargetCount()
{
    uint32_t targetCnt = 0;
    for (auto &targetInfo : G_projTargetMap)
    {
        targetCnt += targetInfo.targetCnt;
    }
    return targetCnt;
}

bool sbe_target_service::plat_isChipletBasedTargetFunctional(
    plat_target_sbe_handle&, uint16_t i_functional_regions)
{
    // Default implementation: The target is good if the entire
    // chiplet is good. The PERV clock region will always be
    // enabled on good chiplets so we can use it as reference.
    return i_functional_regions & CLOCK_REGION_PERV;
}

bool sbe_target_service::plat_isOtherTargetFunctional(
    plat_target_sbe_handle&)
{
    // Just a placeholder implementation
    return true;
}

void sbe_target_service::plat_updateFunctionalState()
{
    uint32_t targetCnt = 0;
    for (auto &targetInfo : G_projTargetMap)
    {
        for (uint32_t i = 0; i < targetInfo.targetCnt; i++)
        {
            auto &target = iv_targets[targetCnt + i];
            const LogTargetType type = target.getTargetType();
            const uint32_t attr_pg = fapi2::ATTR::TARGET_TYPE_PERV::ATTR_PG[target.getChipletNumber()];
            const uint16_t good_regions = ~(attr_pg >> 4);
            bool functional = true;

            if (type == LOG_SBE_ROOT_CHIP_TYPE || type == LOG_TARGET_TYPE_SYSTEM)
            {
                functional = true;
            }
            else if (targetInfo.isChipletBased)
            {
                functional = plat_isChipletBasedTargetFunctional(target, good_regions);
            }
            else
            {
                functional = plat_isOtherTargetFunctional(target);
            }

            target.setFunctional(functional);
        }
        targetCnt += targetInfo.targetCnt;
    }
    plat_PrintTargets();
}

void sbe_target_service::getProcChildren( const LogTargetType i_child_type,
                                          const bool i_include_nonfunctional,
                                          std::vector<plat_target_sbe_handle> &o_children) const
{
    if(i_child_type == LOG_TARGET_TYPE_PERV)
    {
        const TargetFilter l_filter = TARGET_FILTER_NONE;
        getPervChildren(l_filter, i_include_nonfunctional, o_children, true);
    }
    else
    {
        uint32_t targetIndex = 0;
        for (auto &targetInfo : G_projTargetMap)
        {
            if(targetInfo.targetType == i_child_type)
            {
                for(uint32_t j = 0; j < targetInfo.targetCnt; j++)
                {
                    plat_target_sbe_handle l_target = iv_targets[targetIndex + j];
                    if((i_include_nonfunctional || l_target.getFunctional()) &&
                       (l_target.getPresent()))
                    {
                        o_children.push_back(l_target);
                    }
                }
            }
            targetIndex = targetIndex + targetInfo.targetCnt;
        }
    }
}

void sbe_target_service::getChipletChildren(const LogTargetType i_child_type,
                                            const plat_target_sbe_handle i_parent,
                                            const bool i_include_nonfunctional,
                                            std::vector<plat_target_sbe_handle> &o_children) const
{
    const uint8_t cplt_id = i_parent.getChipletNumber();
    for (auto &target : iv_targets)
    {
        if (target.getTargetType() == i_child_type &&
            target.getChipletNumber() == cplt_id)
        {
            o_children.push_back(target);
        }
    }
}

void sbe_target_service::getMulticastChildren(const plat_target_sbe_handle i_parent,
                                              const bool i_include_nonfunctional,
                                              std::vector<plat_target_sbe_handle> &o_children)
{
    if (!i_parent.getIsMulticast())
    {
        // Trivial case where we're already a unicast target of the requested type - return just this target
        o_children.push_back(i_parent);
        return;
    }

    // If a real multicast target, loop over all targets in the chip
    // but filter for multicast group members.
    fapi2::buffer<uint64_t> l_enabledTargets;
    Target<TARGET_TYPE_PERV | TARGET_TYPE_MULTICAST, MULTICAST_BITX> l_tmpTarget(i_parent);
    getScom(l_tmpTarget, 0xF0001, l_enabledTargets);

    if ((i_parent.getTargetType() == LOG_TARGET_TYPE_PERV) )
    {
        // Non-core (i.e. chiplet) target -> loop over all PERV targets, match chiplet ID
        loopTargetsByChiplet(i_parent.getTargetType(),
                             l_enabledTargets,
                             i_include_nonfunctional,
                             o_children);
    }
    return;
}

void sbe_target_service::loopTargetsByChiplet(const LogTargetType i_type,
                                              const buffer<uint64_t> &i_enabled,
                                              const bool i_include_nonfunctional,
                                              std::vector<plat_target_sbe_handle> &o_children)
{
    int targetIndex = 0;
    for (auto &targetInfo : G_projTargetMap)
    {
        if(targetInfo.targetType == i_type)
        {
            for(uint32_t j = 0; j < targetInfo.targetCnt; j++)
            {
                plat_target_sbe_handle l_target = iv_targets[targetIndex + j];
                uint8_t chipletId = l_target.getChipletNumber();
                if((i_enabled.getBit(chipletId)) &&
                   (i_include_nonfunctional || l_target.getFunctional()))
                {
                    o_children.push_back(l_target);
                }
            }
        }
        targetIndex += targetInfo.targetCnt;
    }
}

sbeSecondaryResponse sbe_target_service::getSbePlatTargetHandle( const uint8_t i_logTargetType,
                                                                 const uint8_t i_instanceId,
                                                                 fapi2::plat_target_handle_t &o_tgtHndl)
{
#define SBE_FUNC " getSbePlatTargetHandle: "
    SBE_ENTER(SBE_FUNC);

    // Secondary RC
    sbeSecondaryResponse rc = SBE_SEC_INVALID_TARGET_TYPE_PASSED;

    SBE_DEBUG(SBE_FUNC "Log target type: 0x%02X, instance ID: 0x%02X",
              i_logTargetType, i_instanceId);

    if(i_logTargetType == LOG_TARGET_TYPE_PERV)
    {
        // here instance id has to be treated as chiplet id, and may spread across
        // multiple specific target types. So we need perv specific logic.
        rc = getPervTargetByChipletId(i_instanceId, o_tgtHndl);
        if(rc == SBE_SEC_CHIPLET_ID_NOT_PRESENT_IN_SBE)
        {
            rc = SBE_SEC_INVALID_INSTANCE_ID_PASSED;
        }
    }
    else
    {
        /* Target index form global plat target */
        uint32_t targetIndex = 0;

        /* checking the target types in TARGET MAP */
        for (auto &targetInfo : G_projTargetMap)
        {
            SBE_DEBUG("target count is 0x%08X", (uint32_t)targetInfo.targetCnt);

            /* target not matched goto next target */
            if(targetInfo.targetType != i_logTargetType)
            {
                /* targetIndex depends on target count of target map */
                targetIndex = targetIndex + targetInfo.targetCnt;
                continue;
            }

            if (i_instanceId < targetInfo.targetCnt)
            {
                /* get target handle from TARGET MAP */
                o_tgtHndl = iv_targets[targetIndex + i_instanceId];
                rc = SBE_SEC_OPERATION_SUCCESSFUL; // Target and Instance ID found
            }
            else
            {
                rc = SBE_SEC_INVALID_INSTANCE_ID_PASSED; // Instance ID not found
            }

            break;
        }
    }

    SBE_EXIT(SBE_FUNC);
    return rc;
#undef SBE_FUNC
}

sbeSecondaryResponse sbe_target_service::getPervTargetByChipletId(
    uint8_t i_chiplet_num,
    fapi2::plat_target_handle_t &o_tgtHndl)
{
#define SBE_FUNC " getPervTargetByChipletId: "
    SBE_ENTER(SBE_FUNC);
    // Secondary RC
    sbeSecondaryResponse l_rc = SBE_SEC_CHIPLET_ID_NOT_PRESENT_IN_SBE;

    /* Target index form global plat target */
    uint32_t targetIndex = 0;

    /* checking the target types in TARGET MAP */
    for (auto &targetInfo : G_projTargetMap)
    {
        SBE_DEBUG("target count is 0x%08X", (uint32_t)targetInfo.targetCnt);

        /* target is not pervasive type goto next target */
        if (targetInfo.targetType == LOG_TARGET_TYPE_PERV)
        {
            for(uint8_t inst = 0; inst < targetInfo.targetCnt; inst++)
            {
                if(iv_targets[targetIndex + inst].getChipletNumber() == i_chiplet_num)
                {
                    /* get target handle from TARGET MAP */
                    o_tgtHndl = iv_targets[targetIndex + inst];
                    l_rc = SBE_SEC_OPERATION_SUCCESSFUL; // Target and Instance ID found
                    break;
                }
            }
        }
        if(l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }
        /* targetIndex depends on target count of target map */
        targetIndex = targetIndex + targetInfo.targetCnt;
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

void sbe_target_service::getPervChildren(const TargetFilter i_filter,
                                         const bool i_include_nonfunctional,
                                         std::vector<plat_target_sbe_handle>& o_children,
                                         bool i_ignore_filter) const
{
    const fapi2::buffer<__underlying_type(TargetFilter)> l_filter = i_filter;

    uint64_t l_chiplet_mask = 0;
    for (auto &def :  G_projTargetFilters)
    {
        if (l_filter & def.filter)
        {
            l_chiplet_mask |= def.chiplets;
        }
    }

    const fapi2::buffer<__underlying_type(TargetFilter)> filter = l_chiplet_mask;
    int targetIndex = 0;
    for (auto &targetInfo : G_projTargetMap)
    {
        if (targetInfo.targetType == LOG_TARGET_TYPE_PERV)
        {
            for(uint32_t j = 0; j < targetInfo.targetCnt; j++)
            {
                plat_target_sbe_handle l_target = iv_targets[targetIndex + j];
                uint8_t chipletId = l_target.getChipletNumber();
                if((i_ignore_filter || filter.getBit(chipletId)) &&
                   (i_include_nonfunctional || l_target.getFunctional()))
                {
                    o_children.push_back(l_target);
                }
            }
        }
        targetIndex += targetInfo.targetCnt;
    }
}
