/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/target.H $           */
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
#ifndef __TARGET__
#define __TARGET__

#include "plat_utils.H"
#include "target_service.H"

namespace fapi2
{
    constexpr TargetType TARGET_TYPE_PROC_CHILDREN = TARGET_TYPE_CHIPLETS;
    constexpr TargetType TARGET_TYPE_PERV_CHILDREN = TARGET_TYPE_CHIPLETS;
    constexpr TargetType TARGET_TYPE_MC_CHILDREN = TARGET_TYPE_MI;

    template<TargetType T>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType()
    {
        static_assert(T == SBE_ROOT_CHIP_TYPE, "Invalid or composite target type");
        return PPE_TARGET_TYPE_NONE;
    }

    template<>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType<SBE_ROOT_CHIP_TYPE>()
    {
        return PPE_TARGET_TYPE_CHIP;
    }

    template<>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType<TARGET_TYPE_MC>()
    {
        return PPE_TARGET_TYPE_MC;
    }

    template<>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType<TARGET_TYPE_PERV>()
    {
        return PPE_TARGET_TYPE_PERV;
    }

    template<>
    constexpr plat_target_type_t fapiTargetTypeToPlatTargetType<TARGET_TYPE_MEM_PORT>()
    {
        return PPE_TARGET_TYPE_MEMPORT;
    }

    template<TargetType K, MulticastType M, typename V>
    static constexpr void plat_apply_target_limits(void)
    {}

    template<TargetType T, MulticastType M, typename V>
    inline void Target<T, M, V>::mcUpdateHandle()
    {
        plat_target_base_handle* l_platTarget =
                reinterpret_cast<plat_target_base_handle*>(this);
        if(l_platTarget->getIsMulticast())
        {
            l_platTarget->setMcastType(static_cast<int>(M));
        }
    }

    ///
    /// @brief Assignment Operator.
    /// @param[in] i_right Reference to Target to assign from.
    /// @return Reference to 'this' Target
    ///
    template<TargetType K, MulticastType M, typename V>
    Target<K, M, V>& Target<K, M, V>::operator=(const Target& i_right)
    {
        this->iv_handle = i_right.iv_handle;
        return *this;
    }

    ///
    /// @brief Get this target's immediate parent
    /// @tparam T The type of the parent
    /// @return Target<T, M, V> a target representing the parent
    ///
    template<TargetType K, MulticastType M, typename V>
    template<TargetType T>
    inline Target<T, M, V> Target<K, M, V>::getParent(void) const
    {
        SBE_INFO("getParent PROC");
        constexpr TargetType T_NO_MCAST = T & ~TARGET_TYPE_MULTICAST;
        constexpr TargetType K_NO_MCAST = K & ~TARGET_TYPE_MULTICAST;

        check_getParent<T>();

        // Anything to PROC_CHIP
        if (T == SBE_ROOT_CHIP_TYPE)
        {
            return g_platTarget->plat_getChipTarget();
        }
        else if (T_NO_MCAST == TARGET_TYPE_PERV)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_PERV_CHILDREN) == 0,
                         "Invalid child target for PERV parent");
            plat_target_sbe_handle child = *this;
            return g_platTarget->convertToChiplet<PPE_TARGET_TYPE_PERV>(child);
        }
        else if (T_NO_MCAST == TARGET_TYPE_MC)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_MC_CHILDREN) == 0,
                         "Invalid child target for MC parent");
            plat_target_sbe_handle child = *this;
            return g_platTarget->convertToChiplet<PPE_TARGET_TYPE_MC>(child);
        }
        STATIC_COMPILE_ERROR("Unsupported combination of parent and child for getParent()");
    }

    /// @brief Get the PROC target's children
    /// @tparam K The type of parent
    /// @tparam V The plat target handle type
    /// @tparam T The type of child
    /// @param[in] i_state The desired TargetState of the children
    /// @return std::vector<Target<T> > a vector of present/functional
    /// children
    /// @warning The children are returned in order, ex child[0] is
    /// std::vector[0]
    template<TargetType TARGET_TYPE_PROC_CHIP, MulticastType M, typename V> // Parent target type
    template<TargetType T>          // Child target type
    std::vector<Target<T, M, V>>
    Target<TARGET_TYPE_PROC_CHIP, M, V>::getChildren(const TargetState i_state) const
    {
        SBE_INFO("getChildren without filter");
        constexpr plat_target_type_t P = fapiTargetTypeToPlatTargetType<T>();
        const bool l_include_nonfunctional = i_state == TARGET_STATE_PRESENT;

        check_getChildren<T>();

        static_assert(sizeof(Target<T, M, V>) == sizeof(plat_target_sbe_handle),
                  "Sizes of plat target and FAPI target must match");

        // Using the same kind of "dispatcher" approach as getParent()
        std::vector<Target<T, M, V> > l_children;
        auto &l_cast_children = reinterpret_cast<std::vector<plat_target_sbe_handle>&>(l_children);
        g_platTarget->getProcChildren(P, l_include_nonfunctional, l_cast_children);
        return l_children;
    }
    /// @brief Specialization of getChildren, filtered for the chip target
    /// @tparam K The type of parent
    /// @tparam V The plat target handle type
    /// @tparam T The type of child
    /// @param[in] i_filter Target Filter.
    /// @param[in] i_state State of target.
    /// @return std::vector<Target<T> > a vector of present/functional
    /// children
    /// @warning The children are returned in order, ex child[0] is
    /// std::vector[0]
    template<TargetType K, MulticastType M, typename V>
    template<TargetType T>
    std::vector<Target<T, M, V> >
    Target<K, M, V>::getChildren(const TargetFilter i_filter,
                                      const TargetState i_state) const
    {
        SBE_INFO("getChildren with filter");
        const bool l_include_nonfunctional = i_state == TARGET_STATE_PRESENT;
        check_getChildren<T>();

        static_assert(sizeof(Target<T, M, V>) == sizeof(plat_target_sbe_handle),
                  "Sizes of plat target and FAPI target must match");
#if 0
        static_assert(K == TARGET_TYPE_PROC_CHIP,
                  "Parent target must be the proc chip");
#endif
        static_assert((T & ~TARGET_TYPE_PROC_CHILDREN) == 0,
                  "Child target type must be a pervasive chiplet");

        std::vector<Target<T, M, V> > l_children;
        g_platTarget->getChildren(i_filter, l_include_nonfunctional,
                      reinterpret_cast<std::vector<plat_target_sbe_handle>&>(l_children));
        return l_children;
    }

    // TODO: Need to support getchildren for multicast targets

    ///
    /// @brief Returns the chiplet number associated with the Target
    /// @return The chiplet number for the Target. 0 is returned if the
    /// Target does not have a chiplet number (for ex, the PROC_CHIP Target)
    /// @note For logical targets such as the EX, the chiplet number of
    /// their immediate parent chiplet is returned
    ///
    template<TargetType K, MulticastType M, typename V>
    inline uint8_t
    Target<K, M, V>::getChipletNumber(void) const
    {
        check_getChipletNumber();

        return reinterpret_cast<const plat_target_base_handle*>(this)->getChipletNumber();
    }

    ///
    /// @brief Get a multicast target for a given chip
    /// @tparam T The type of target to return; TARGET_TYPE_MULTICAST is added automatically
    /// @tparam O The type of multicast read operation for the target; defaults to OR
    /// @param[in] i_group The abstract multicast group the target should point to
    /// @return The multicast target
    ///
    /// This method is only applicable to chip-level targets.
    /// If the requested multicast group cannot be mapped to an available HW multicast
    /// group, a platform specific error will be thrown.
    ///
    template<TargetType K, MulticastType M, typename V>
    template< TargetType T, MulticastType O >
    inline Target < T | TARGET_TYPE_MULTICAST, O, V >
    Target<K, M, V>::getMulticast(const MulticastGroup i_group) const
    {
        check_getMulticast<T, O>();
        plat_target_handle_t l_platTarget;
        constexpr plat_target_type_t l_plat_type = fapiTargetTypeToPlatTargetType<T>();

        l_platTarget.setIsMulticast(1);
        l_platTarget.setMcastGroup(i_group);
        l_platTarget.setTargetType(l_plat_type);

        return l_platTarget;
    }
}
#endif
