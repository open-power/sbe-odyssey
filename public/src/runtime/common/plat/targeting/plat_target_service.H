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
#pragma once

#include <vector>
#include <plat_target_sbe.H>
#include <target_types.H>
#include <target_filters.H>
#include "sbe_sp_intf.H"
#include <fapi2_target.H>

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
        }

        ///
        /// @brief Function to print the G_targets vector
        ///
        void plat_printTargets() const;

        ///
        /// @brief Function to initialize the G_targets vector based on partial good
        ///        targets
        /// @return FAPI2_RC_SUCCESS if okay
        ///
        uint32_t plat_targetsInit();

        ///
        /// @brief Function to get the target count.
        ///
        /// @return number of target.
        uint32_t plat_getTargetCount() const;

        ///
        /// @brief Update target functional state based on ATTR_PG
        ///
        void plat_updateFunctionalState();

        ///
        /// @brief Enumerate all targets with a given type regardless of chiplet ID
        ///
        /// @param [in] i_child_type            Log target type of requested children
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        void getProcChildren( const LogTargetType i_child_type,
                              const bool i_include_nonfunctional,
                              std::vector<plat_target_sbe_handle> &o_children) const;

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
                                std::vector<plat_target_sbe_handle> &o_children) const;

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
                                  std::vector<plat_target_sbe_handle> &o_children) const
        {
            getMulticastChildrenInternal(-1ULL, i_parent, i_include_nonfunctional, o_children);
        }

        ///
        ///
        /// @brief Break a multicast target up into unicast targets, and apply a target filter
        ///
        /// @param [in] i_filter     Target filter to apply
        /// @param [in] i_parent     Parent target handle
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getMulticastChildren(const TargetFilter i_filter,
                                  const plat_target_sbe_handle i_parent,
                                  const bool i_include_nonfunctional,
                                  std::vector<plat_target_sbe_handle> &o_children) const
        {
            const uint64_t l_chiplet_mask = convertTargetFilterToChipletMask(i_filter);
            getMulticastChildrenInternal(l_chiplet_mask, i_parent, i_include_nonfunctional, o_children);
        }

        ///
        /// @brief Function to get the chip target initialize the G_targets vector
        /// @return chip target
        ///
        Target<SBE_ROOT_CHIP_TYPE>& plat_getChipTarget() const;

        ///
        /// @brief Function to get target is chiplet
        /// @tparam K log target type (template)
        ///
        template <LogTargetType K>
        constexpr void plat_checkTargetIsChiplet( ) const
        {
            static_assert( (TARGET_TYPE_CHIPLETS && GET_FAPI_TARGET_TYPE(K)) , "Error: Destination type is not a chiplet");
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
        /// @param [in] i_instanceId
        ///                if i_logTargetType = LOG_TARGET_TYPE_PERV, then chiplet id of the target
        ///                else instance id of the target
        /// @param [out] o_tgtHndl fapi plat target handle
        /// @return sbeSecondaryResponse Secondary RC
        ///           SBE_SEC_OPERATION_SUCCESSFUL: Valid i_logTargetType and i_instanceId passed
        ///           SBE_SEC_INVALID_TARGET_TYPE_PASSED: Invalid i_logTargetType passed
        ///           SBE_SEC_INVALID_INSTANCE_ID_PASSED: Invalid i_instanceId passed
        ///
        sbeSecondaryResponse getSbePlatTargetHandle( const uint8_t i_logTargetType,
                                                     const uint8_t i_instanceId,
                                                     plat_target_sbe_handle &o_tgtHndl) const;

        /// @brief Function to apply the target state attributes
        ///        to corresponding target states in target vector
        ///
        virtual void plat_TargetStateUpdateFromAttribute()
        {
            // Platform are supposed to provide the implementation;
        }

    protected:
        std::vector<plat_target_sbe_handle> iv_targets;

        ///
        /// @brief Determine functional state of a chiplet based target based on functional regions
        ///
        /// This is a helper function called internally by plat_updateFunctionalState for all
        /// targets that are backed by a chiplet (i.e. chiplets or chip units).
        ///
        /// Platform specializations are supposed to override the default implementation; some
        /// chip unit specific targets (such as TBUSL) are functional based on the partial-goodness
        /// of specific clock regions inside a chiplet.
        ///
        /// @param[in] i_target              The target to assess
        /// @param[in] i_functional_regions  Set of functional regions, extracted from ATTR_PG
        ///                                  Bit 0 corresponds to region 0 and so forth.
        ///                                  A set bit means that the region is good.
        ///
        /// @return that target's desired functional state
        virtual bool plat_isChipletBasedTargetFunctional(
            plat_target_sbe_handle &i_target,
            uint16_t i_functional_regions) const;

        ///
        /// @brief Determine functional state of a specialtarget based on functional regions
        ///
        /// This is a helper function called internally by plat_updateFunctionalState for all
        /// targets that are not backed by a chiplet and not the chip or system.
        ///
        /// Platform specializations are supposed to override the default implementation.
        ///
        /// @param[in] i_target              The target to assess
        ///
        /// @return that target's desired functional state
        virtual bool plat_isOtherTargetFunctional(
            plat_target_sbe_handle &i_target) const;

    private:
        ///
        /// @brief Function to get the proc chip target initialize the G_targets vector
        ///        Making this function private since, this can be called and casted to any target type,
        ///        and will not ensure type safe.
        ///        But this function is required to be called from
        ///        "inline Target<T, M, V> Target<K, M, V>::getParent(void) const"
        ///        hence making it friend function of this class.
        /// @return proc target
        ///
        plat_target_sbe_handle plat_getChipTargetPlatType()
        {
            return iv_targets[0];
        }
        template<TargetType K, MulticastType M, typename V>
        template<TargetType T>
        friend
        inline Target<T, M, V> Target<K, M, V>::getParent(void) const;

        ///
        /// @brief Get all pervasive children for a proc.
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getPervChildren(const TargetFilter i_filter,
                             const bool i_include_nonfunctional,
                             std::vector<plat_target_sbe_handle>& o_children,
                             bool i_ignore_filter = false) const;

        ///
        /// @brief Break a multicast target up into unicast targets (internal implementation)
        ///
        /// @param [in] i_chiplet_mask Mask of chiplets to include in the result
        /// @param [in] i_parent     Parent target handle
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void getMulticastChildrenInternal(const uint64_t i_chiplet_mask,
                                          const plat_target_sbe_handle i_parent,
                                          const bool i_include_nonfunctional,
                                          std::vector<plat_target_sbe_handle> &o_children) const;

        ///
        /// @brief convert a fapi2::TargetFilter to a chiplet mask
        /// @param [in]  i_filter  The target filter to convert
        /// @return The converted chiplet mask
        ///
        static uint64_t convertTargetFilterToChipletMask(const TargetFilter i_filter);

        ///
        ///
        /// @brief Enumerate targets of a specific type based on a mask of chiplets to include
        ///
        /// @param [in] i_type                  Type of targets to return
        /// @param [in] i_enabled               Chiplet mask - 1 means include, 0 means exclude
        /// @param [in] i_include_nonfunctional Return also targets that are not functional
        /// @param [out] o_children             A vector of child target handles
        ///
        void loopTargetsByChiplet(const LogTargetType i_type,
                                  const buffer<uint64_t> &i_enabled,
                                  const bool i_include_nonfunctional,
                                  std::vector<plat_target_sbe_handle> &o_children) const;

        ///
        /// @brief Function to get the plat target handle of a perv target using chiplet id
        /// @param [in] i_chiplet_num chilet id of the target
        /// @param [out] o_tgtHndl fapi plat target handle
        /// @return sbeSecondaryResponse Secondary RC
        ///           SBE_SEC_OPERATION_SUCCESSFUL: Valid i_logTargetType and i_instanceId passed
        ///           SBE_SEC_CHIPLET_ID_NOT_PRESENT_IN_SBE: chiplet id passed is not valid for this SBE
        ///
        sbeSecondaryResponse getPervTargetByChipletId(
            uint8_t i_chiplet_num,
            plat_target_sbe_handle &o_tgtHndl) const;
    };
}
