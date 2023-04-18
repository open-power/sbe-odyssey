/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_service.H $ */
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
#ifndef __SBE_TARGET_SERVICE__
#define __SBE_TARGET_SERVICE__

#include <fapi2_target.H>
#include <return_code.H>
#include <plat_target_sbe.H>
#include <target_filters.H>
#include <hw_access.H>
// TODO: Need to remove dependency on odyssey files in common space.
//       Same fix need in cross_file.txt also.
#include <ody_target_filters.H>
#include "sbe_sp_intf.H"

namespace fapi2
{
    /*
     * Base class for target services.
     * It will be derived in each object.
     */
    class sbe_target_service
    {
        public:
        /*
         * Constructor
         */
        sbe_target_service()
        {
            iv_targetTypesCnt = getTargetTypesCount();
        }

        std::vector<fapi2::plat_target_sbe_handle> iv_targets;
        ///
        /// @brief Function to print the G_targets vector
        ///
        void plat_PrintTargets()
        {
            uint32_t targetCnt = plat_getTargeCount();
            for(uint32_t i = 0; i < targetCnt; i++)
            {
                SBE_INFO("sbe_target_service 0x%08X", iv_targets[i].iv_value());
            }
        }

        ///
        /// @brief Function to initialize the G_targets vector based on partial good
        ///        targets
        /// @return FAPI2_RC_SUCCESS if okay
        ///
        ReturnCode plat_TargetsInit()
        {
            SBE_INFO("sbe_target_service plat_TargetsInit()");

            targetInfo_t * targetMap = getTargetMap();
            for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
            {
                SBE_DEBUG("target count is 0x%08X", (uint32_t)targetMap[i].targetCnt);
                for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                {
                    SBE_DEBUG("sbe_target_service target to be inserted is 0x%08X",
                               plat_target_sbe_handle(targetMap[i].chipletNum + j,
                                                      targetMap[i].targetType,
                                                       j));
                    uint32_t chipletNum = 0;
                    if(targetMap[i].isChipletType)
                    {
                        chipletNum = targetMap[i].chipletNum + j;
                    }
                    else
                    {
                        chipletNum = targetMap[i].chipletNum;
                    }
                    iv_targets.push_back(plat_target_sbe_handle(
                            chipletNum, targetMap[i].targetType, j));
                }
            }

            //Print Odyssey targets.
            plat_PrintTargets();
            return FAPI2_RC_SUCCESS;
        }

        ///
        /// @brief Fuction to get the target count.
        ///
        /// @return number of target.
        uint32_t plat_getTargeCount()
        {
            uint32_t targetCnt = 0;
            targetInfo_t * targetMap = getTargetMap();
            for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
            {
                for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                {
                    ++targetCnt;
                }
            }
            return targetCnt;
        }

        ///
        /// @brief Enumerate all targets with a given type regardless of chiplet ID
        ///
        /// @param [in] i_child_type            Log target type of requested children
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        void getProcChildren( const LogTargetType i_child_type,
                              const bool i_include_nonfunctional,
                              std::vector<plat_target_sbe_handle> &o_children) const
        {
            SBE_INFO("sbe_target_service getProcChildren");
            if(i_child_type == LOG_TARGET_TYPE_PERV)
            {
                const TargetFilter l_filter = TARGET_FILTER_NONE;
                getPervChildren(l_filter, i_include_nonfunctional, o_children, true);
            }
            else
            {
                targetInfo_t * targetMap = getTargetMap();
                uint32_t targetIndex = 0;
                for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
                {
                    if(targetMap[i].targetType == i_child_type)
                    {
                        for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                        {
                            plat_target_sbe_handle l_target = iv_targets[targetIndex + j];
                            if((i_include_nonfunctional || l_target.getFunctional()) &&
                               (l_target.getPresent()))
                            {
                                o_children.push_back(l_target);
                            }
                        }
                    }
                    targetIndex = targetIndex + targetMap[i].targetCnt;
                }
            }
         }

        /// @brief Get proc chip target's children - filtered
        ///
        /// @param [in] i_filter     Target filter
        /// @param [in] i_state      Required state of the children
        /// @param [out] o_children  A vector of child target handles
        ///
        void getChildren(const TargetFilter i_filter,
                         const bool i_include_nonfunctional,
                         std::vector<plat_target_sbe_handle>& o_children) const
        {
            SBE_INFO("sbe_target_service getChildren with filter");
            const fapi2::buffer<__underlying_type(TargetFilter)> l_filter = i_filter;

            // Walk the bits in the input target filter. For every bit, at
            // position x, that is set, x can be used as an index into our global
            // target vector (indexed by chiplet number)
            getPervChildren(i_filter, i_include_nonfunctional, o_children);
        }

        ///
        ///
        /// @brief Get all the children targets for the chiplet targets
        ///
        /// @param [in] i_child_type Children target type
        /// @param [in] i_parent     Parent target handle
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getChipletChildren(const LogTargetType i_child_type,
                                const plat_target_sbe_handle i_parent,
                                const bool i_include_nonfunctional,
                                std::vector<plat_target_sbe_handle> &o_children) const
        {
            targetInfo_t * targetMap = getTargetMap();
            uint32_t targetIndex = 0;
            for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
            {
                if( (targetMap[i].targetType == i_child_type) &&
                    (targetMap[i].chipletNum == i_parent.getChipletNumber()) )
                {
                    for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                    {
                        plat_target_sbe_handle l_target = iv_targets[targetIndex + j];
                        {
                            o_children.push_back(l_target);
                        }
                    }
                }
                targetIndex = targetIndex + targetMap[i].targetCnt;
            }
        }

        ///
        ///
        /// @brief Break a multicast target up into unicast targets
        ///
        /// @param [in] i_parent     Parent target handle
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getMulticastChildren(const plat_target_sbe_handle i_parent,
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
                loopTargetsByChiplet(l_enabledTargets, i_include_nonfunctional, o_children);
            }
            return;
        }

        ///
        ///
        /// @brief Return the multicast enabled targets
        ///
        /// @param [in] i_enabled               Multicast enabled buffer.
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void loopTargetsByChiplet(const buffer<uint64_t> &i_enabled,
                                         const bool i_include_nonfunctional,
                                         std::vector<plat_target_sbe_handle> &o_children)
        {
            targetInfo_t * targetMap = getTargetMap();
            for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
            {
                if(targetMap[i].isPervType)
                {
                    for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                    {
                        plat_target_sbe_handle l_target = iv_targets[i + j];
                        uint8_t chipletId = l_target.getChipletNumber();
                        if((i_enabled.getBit(chipletId)) &&
                           (i_include_nonfunctional || l_target.getFunctional()))
                        {
                            o_children.push_back(l_target);
                        }
                    }
                }
            }
        }


        ///
        /// @brief Function to get the proc chip target initialize the G_targets vector
        /// @return proc target
        ///
        Target<SBE_ROOT_CHIP_TYPE> plat_getChipTarget()
        {
            return ((fapi2::Target<fapi2::SBE_ROOT_CHIP_TYPE>)
                                    iv_targets[0]);
        }

        ///
        /// @brief Function to get the proc chip target initialize the G_targets vector
        /// @return proc target
        ///
        plat_target_sbe_handle plat_getChipTargetPlatType()
        {
            return iv_targets[0];
        }

        ///
        /// @brief Function to get target is chiplet
        /// @tparam K log target type (template)
        /// @return True: Target is chiplet, other wise assert
        ///
        template <LogTargetType K>
        constexpr bool plat_checkTargetIsChiplet( )
        {
            static_assert( (TARGET_TYPE_CHIPLETS && GET_FAPI_TARGET_TYPE(K)) , "Error: Destination type is not a chiplet");
            return true;
        }

        ///
        /// @brief Convert any target to a chiplet target of specified
        ///        type, stripping any unit specific address bits.
        ///
        /// This is used by getParent() to map a chiplet child target to a
        /// chiplet target, or any target to PERV. It returns a copy of this
        /// target that has all unit-level address bits cleared and
        /// type_target_num updated to match the new target type.
        ///
        template <LogTargetType K>
        plat_target_sbe_handle convertToChiplet(const plat_target_sbe_handle i_child) const
        {
            plat_checkTargetIsChiplet<K>();

            plat_target_sbe_handle copy = i_child;

            // type becomes the new type
            copy.setTargetType(K);

            // set the instance number.
            copy.setTargetInstance(i_child.getParentTargetInstance(i_child));

            // TODO: P11 SBE PORTING
            // This will work for Odyssey.
            // Set the specific bit related to core/phb/pauc.

            return copy;
        }

        ///
        /// @brief Function to get the plat target handle from log target type
        /// @param [in] i_logTargetType log target type
        /// @param [in] i_instanceId instance id
        /// @param [out] o_tgtHndl fapi plat target handle
        /// @return sbeSecondaryResponse Secondary RC
        ///                  SBE_SEC_OPERATION_SUCCESSFUL: Valid i_logTargetType and i_instanceId passed
        ///                  SBE_SEC_INVALID_TARGET_TYPE_PASSED: Invalid i_logTargetType passed
        ///                  SBE_SEC_INVALID_INSTANCE_ID_PASSED: Invalid i_instanceId passed
        ///
        sbeSecondaryResponse getSbePlatTargetHandle( const uint8_t i_logTargetType,
                                                  const uint8_t i_instanceId,
                                                  fapi2::plat_target_handle_t &o_tgtHndl)
        {
            #define SBE_FUNC " getSbePlatTargetHandle: "
            SBE_ENTER(SBE_FUNC);

            // Secondary RC
            sbeSecondaryResponse rc = SBE_SEC_INVALID_TARGET_TYPE_PASSED;

            SBE_DEBUG(SBE_FUNC "Log target type: 0x%02X, instance ID: 0x%02X",
                                                i_logTargetType, i_instanceId);

            /* Platform target map pointer */
            targetInfo_t * targetMap = getTargetMap();
            /* Target index form global plat target */
            uint32_t targetIndex = 0;

            /* checking the target types in TARGET MAP */
            for(uint32_t targetNo = 0; targetNo < iv_targetTypesCnt; targetNo++)
            {
                SBE_DEBUG("target count is 0x%08X", (uint32_t)targetMap[targetNo].targetCnt);

                /* target not matched goto next target */
                if(targetMap[targetNo].targetType != i_logTargetType)
                {
                    /* targetIndex depends on target count of target map */
                    targetIndex = targetIndex + targetMap[targetNo].targetCnt;
                    continue;
                }

                if (i_instanceId < targetMap[targetNo].targetCnt)
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

            SBE_EXIT(SBE_FUNC);
            return rc;
            #undef SBE_FUNC
        }


        /*
         * Destructor
         */
         ~sbe_target_service()
         {
         }

        private:

        ///
        /// @brief Get all pervasive children for a proc.
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getPervChildren(const TargetFilter i_filter,
                             const bool i_include_nonfunctional,
                             std::vector<plat_target_sbe_handle>& o_children,
                             bool i_ignore_filter = false) const
        {
            SBE_INFO("sbe_target_service getPervChildren with filter");
            const fapi2::buffer<__underlying_type(TargetFilter)> l_filter = i_filter;

            uint64_t l_chiplet_mask = 0;
            for (auto def : ody::TARGET_FILTERS)
            {
                if (l_filter & def.filter)
                {
                    l_chiplet_mask |= def.chiplets;
                }
            }

            const fapi2::buffer<__underlying_type(TargetFilter)> filter = l_chiplet_mask;
            targetInfo_t * targetMap = getTargetMap();
            for(uint32_t i = 0; i < iv_targetTypesCnt; i++)
            {
                if(targetMap[i].isPervType)
                {
                    for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                    {
                        plat_target_sbe_handle l_target = iv_targets[i + j];
                        uint8_t chipletId = l_target.getChipletNumber();
                        if((i_ignore_filter || filter.getBit(chipletId)) &&
                           (i_include_nonfunctional || l_target.getFunctional()))
                        {
                            o_children.push_back(l_target);
                        }
                    }
                }
            }

        }
        uint32_t iv_targetTypesCnt;
    };
}
#endif
