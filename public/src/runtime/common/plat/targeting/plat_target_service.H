/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_service.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
            iv_targetCnt = getTargetCount(); 
        }
        std::vector<fapi2::plat_target_sbe_handle> iv_targets;
        ///
        /// @brief Function to print the G_targets vector
        ///
        void plat_PrintTargets()
        {
            for(uint32_t i = 0; i < iv_targetCnt; i++)
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
            for(uint32_t i = 0; i < iv_targetCnt; i++)
            {
                SBE_DEBUG("target count is 0x%08X", (uint32_t)targetMap[i].targetCnt);
                for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                {
                    SBE_DEBUG("sbe_target_service target to be inserted is 0x%08X",
                               plat_target_sbe_handle(targetMap[i].chipletNum + j,
                                                       targetMap[i].targetType));
                    iv_targets.push_back(plat_target_sbe_handle(
                            targetMap[i].chipletNum + j, targetMap[i].targetType));
                }
            }

            //Print Odyssey targets.
            plat_PrintTargets();
            return FAPI2_RC_SUCCESS;
        }

        ///
        /// @brief Fuction to set the target count.
        ///
        void plat_setTargeCount(uint32_t count)
        {
            iv_targetCnt = count;
        }

        ///
        /// @brief Fuction to get the target count.
        ///
        /// @return number of target.
        uint32_t plat_getTargeCount()
        {
            return iv_targetCnt;
        }

        ///
        /// @brief Enumerate all targets with a given type regardless of chiplet ID
        ///
        /// @param [in] i_child_type            Plat target type of requested children
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        void getProcChildren( const plat_target_type_t i_child_type,
                              const bool i_include_nonfunctional,
                              std::vector<plat_target_sbe_handle> &o_children) const
        {
            SBE_INFO("sbe_target_service getProcChildren");
            if(i_child_type == PPE_TARGET_TYPE_PERV)
            {
                const TargetFilter l_filter = TARGET_FILTER_NONE;
                getPervChildren(l_filter, i_include_nonfunctional, o_children, true);
            }
            else
            {
                 for (uint32_t i = 1; i < iv_targetCnt; i++)
                 {
                     plat_target_sbe_handle l_target = iv_targets[i];
                     if ((l_target.getTargetType() == i_child_type) &&
                         (i_include_nonfunctional || l_target.getFunctional()) &&
                         (l_target.getPresent()))
                     {
                         o_children.push_back(l_target);
                     }
                 }
            }
         }
        ///
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
        /// @brief Function to get the proc chip target initialize the G_targets vector
        /// @return proc target
        ///
        Target<TARGET_TYPE_PROC_CHIP> plat_getChipTarget()
        {
            return ((fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>)
                                    iv_targets[0]);
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
        template <plat_target_type_t K>
        plat_target_sbe_handle convertToChiplet(const plat_target_sbe_handle i_child) const
        {
            static_assert(plat_target_is_chiplet<K>(),
                          "Destination type is not a chiplet");

            plat_target_sbe_handle copy = i_child;

            // type becomes the new type
            copy.setTargetType(K);

            // TODO: P11 SBE PORTING
            // This will work for Odyssey.
            // Set the specific bit related to core/phb/pauc.

            return copy;
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
                             bool i_include_isFilter = false) const
        {
            const fapi2::buffer<__underlying_type(TargetFilter)> l_filter = i_filter;
            targetInfo_t * targetMap = getTargetMap();
            for(uint32_t i = 0; i < iv_targetCnt; i++)
            {
                if(targetMap[i].isPervType)
                {
                    for(uint32_t j = 0; j < targetMap[i].targetCnt; j++)
                    {
                        plat_target_sbe_handle l_target = iv_targets[i + j];
                        SBE_INFO("sbe_target_service Target is 0x%08X", uint32_t(l_target));
                        if(((i_include_isFilter || l_filter.getBit(i)) &&
                             i_include_nonfunctional) || l_target.getFunctional())
                        {
                            o_children.push_back(l_target);
                        }
                    }
                }
            }

        }
        uint32_t iv_targetCnt;
    };
}
#endif
