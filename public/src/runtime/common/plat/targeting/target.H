/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/target.H $           */
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

#include "plat_utils.H"
#include "target_service.H"
#include <fapi2_target.H>

namespace fapi2
{
    constexpr TargetType TARGET_TYPE_PROC_CHILDREN = TARGET_TYPE_CHIPLETS;
    constexpr TargetType TARGET_TYPE_PERV_CHILDREN = TARGET_TYPE_CHIPLETS;
    constexpr TargetType TARGET_TYPE_MEM_PORT_CHILDREN = TARGET_TYPE_DIMM;
    constexpr TargetType TARGET_TYPE_MC_CHILDREN = TARGET_TYPE_OMI;
    constexpr TargetType TARGET_TYPE_OMI_CHILDREN = TARGET_TYPE_OCMB_CHIP;
    constexpr TargetType TARGET_TYPE_XBUS_CHILDREN = TARGET_TYPE_XBUSL;
    constexpr TargetType TARGET_TYPE_EX_CHILDREN = TARGET_TYPE_L3CACHE
        | TARGET_TYPE_SHADOW | TARGET_TYPE_CORE;
    constexpr TargetType TARGET_TYPE_OCMB_CHIP_CHILDREN = TARGET_TYPE_PMIC | TARGET_TYPE_GENERICI2CRESPONDER | TARGET_TYPE_MEM_PORT | TARGET_TYPE_DIMM;

    template<TargetType T>
    constexpr LogTargetType fapiTargetTypeToPlatTargetType()
    {
        static_assert(T == SBE_ROOT_CHIP_TYPE, "Invalid or composite target type");
        return LOG_SBE_ROOT_CHIP_TYPE;
    }

#define TARGET_TYPE_SUPPORT(type)                                                \
    template<>                                                                   \
    constexpr LogTargetType fapiTargetTypeToPlatTargetType<TARGET_TYPE_##type>() \
    {                                                                            \
        return LOG_TARGET_TYPE_##type;                                           \
    }

    TARGET_TYPE_SUPPORT(PERV);
    TARGET_TYPE_SUPPORT(MEM_PORT);
    TARGET_TYPE_SUPPORT(DIMM);
    TARGET_TYPE_SUPPORT(TEMP_SENSOR);
    TARGET_TYPE_SUPPORT(EX);
    TARGET_TYPE_SUPPORT(L3CACHE);
    TARGET_TYPE_SUPPORT(SHADOW);
    TARGET_TYPE_SUPPORT(CORE);
    TARGET_TYPE_SUPPORT(PEC);
    TARGET_TYPE_SUPPORT(ABUS);
    TARGET_TYPE_SUPPORT(MC);
    TARGET_TYPE_SUPPORT(OMI);
    TARGET_TYPE_SUPPORT(XBUS);
    TARGET_TYPE_SUPPORT(XBUSL);
    TARGET_TYPE_SUPPORT(OCMB_CHIP);
    TARGET_TYPE_SUPPORT(PMIC);
    TARGET_TYPE_SUPPORT(GENERICI2CRESPONDER);

#undef TARGET_TYPE_SUPPORT

    template<TargetType K, MulticastType M, typename V>
    static constexpr void plat_apply_target_limits(void)
    {
    }

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
    /// @brief Inquality Comparison Operator
    /// @param[in] i_right Reference to Target to compare.
    /// @return bool. True if not equal.
    /// @note Platforms need to define this so that the physical
    /// targets are determined to be equivilent rather than just the handles
    ///
    template<TargetType K, MulticastType M, typename V>
    bool Target<K, M, V>::operator!=(const Target& i_right) const
    {
        return i_right.iv_handle != iv_handle;
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
        constexpr TargetType T_NO_MCAST = T & ~TARGET_TYPE_MULTICAST;
        constexpr TargetType K_NO_MCAST = K & ~TARGET_TYPE_MULTICAST;

        check_getParent<T>();

        // Anything to PROC_CHIP
        if ((T == SBE_ROOT_CHIP_TYPE) || (T == TARGET_TYPE_ANY_POZ_CHIP))
        {
            return g_platTarget->plat_getChipTargetPlatType();
        }
        else if (T_NO_MCAST == TARGET_TYPE_PERV)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_PERV_CHILDREN) == 0,
                         "Invalid child target for PERV parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_PERV>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_MEM_PORT)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_MEM_PORT_CHILDREN) == 0,
                         "Invalid child target for MEM PORT parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_MEM_PORT>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_MC)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_MC_CHILDREN) == 0,
                         "Invalid child target for MC parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_MC>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_XBUS)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_XBUS_CHILDREN) == 0,
                         "Invalid child target for XBUS parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_XBUS>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_EX)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_EX_CHILDREN) == 0,
                         "Invalid child target for EX parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_EX>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_OMI)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_OMI_CHILDREN) == 0,
                         "Invalid child target for OMI parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_OCMB_CHIP>(*this);
        }
        else if (T_NO_MCAST == TARGET_TYPE_OCMB_CHIP)
        {
            STATIC_CHECK((K_NO_MCAST & ~TARGET_TYPE_OCMB_CHIP_CHILDREN) == 0,
                         "Invalid child target for OMI parent");
            return g_platTarget->convertToChiplet<LOG_TARGET_TYPE_OCMB_CHIP>(*this);
        }
        // If we fall through to here, there is no defined code path to handle the
        // combination of T and K -- let's fail loudly.
        STATIC_COMPILE_ERROR("Unsupported combination of parent and child for getParent()");
        // Fake return value to suppress compiler warning; we rely on STATIC_COMPILE_ERROR
        return Target<T, M, V>();
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
    template<TargetType K, MulticastType M, typename V> // Parent target type
    template<TargetType T>          // Child target type
    std::vector<Target<T, M, V>>
    Target<K, M, V>::getChildren(const TargetState i_state) const
    {
        constexpr TargetType K_NO_T = K & ~T;
        constexpr TargetType K_NO_MCAST = K & ~TARGET_TYPE_MULTICAST;
        constexpr LogTargetType P = fapiTargetTypeToPlatTargetType<T>();

        const bool l_include_nonfunctional = i_state == TARGET_STATE_PRESENT;

        check_getChildren<T>();

        static_assert(sizeof(Target<T, M, V>) == sizeof(plat_target_sbe_handle),
                  "Sizes of plat target and FAPI target must match");

        // Using the same kind of "dispatcher" approach as getParent()
        std::vector<Target<T, M, V> > l_children;
        auto &l_cast_children = reinterpret_cast<std::vector<plat_target_sbe_handle>&>(l_children);

        if ((K & TARGET_TYPE_MULTICAST) && (T == K_NO_MCAST))
        {
            g_platTarget->getMulticastChildren(*this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if ((K & T) && (this->get().getTargetType() == P))
        {
            // Special case: Target<K | whatever>::getChildren<K> will return
            // itself if the _actual_ target type is K
            l_children.push_back(this->get());
            return l_children;
        }
        else if (K_NO_T == SBE_ROOT_CHIP_TYPE || K_NO_T == TARGET_TYPE_ANY_POZ_CHIP)
        {
            STATIC_CHECK((T & ~SBE_ROOT_CHIP_CHILDREN) == 0,
                         "Invalid child type for CHIP target");
            g_platTarget->getProcChildren(P, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_MEM_PORT)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_MEM_PORT_CHILDREN) == 0,
                         "Invalid child type for MEM PORT target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_MC)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_MC_CHILDREN) == 0,
                         "Invalid child type for MC target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_XBUS)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_XBUS_CHILDREN) == 0,
                         "Invalid child type for XBUS target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_EX)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_EX_CHILDREN) == 0,
                         "Invalid child type for EX target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_OCMB_CHIP)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_OCMB_CHIP_CHILDREN) == 0,
                         "Invalid child type for OCMB_CHIP target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else if (K_NO_T == TARGET_TYPE_OMI)
        {
            STATIC_CHECK((T & ~TARGET_TYPE_OMI_CHILDREN) == 0,
                         "Invalid child type for OMI target");
            g_platTarget->getChipletChildren(P, *this, l_include_nonfunctional, l_cast_children);
            return l_children;
        }
        else
        {
            // If we fall through to here, there is no defined code path to handle the
            // combination of T and K -- let's fail loudly.
            STATIC_COMPILE_ERROR("Unsupported combination of parent and child for getChildren()");
            return l_children;
        }
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
        const bool l_include_nonfunctional = i_state == TARGET_STATE_PRESENT;
        constexpr TargetType K_NO_MCAST = K & ~TARGET_TYPE_MULTICAST;
        check_getChildren<T>();

        static_assert(sizeof(Target<T, M, V>) == sizeof(plat_target_sbe_handle),
                  "Sizes of plat target and FAPI target must match");
        static_assert((T & ~(SBE_ROOT_CHIP_CHILDREN & TARGET_TYPE_CHIPLETS)) == 0,
                  "Child target type must be a pervasive chiplet");

        std::vector<Target<T, M, V> > l_children;
        auto &l_cast_children = reinterpret_cast<std::vector<plat_target_sbe_handle>&>(l_children);
        if ((K & TARGET_TYPE_CHIPS) != 0)
        {
            g_platTarget->getChildren(i_filter, l_include_nonfunctional, l_cast_children);
        }
        else if ((K & TARGET_TYPE_MULTICAST) && (T == K_NO_MCAST))
        {
            g_platTarget->getMulticastChildren(i_filter, *this, l_include_nonfunctional, l_cast_children);
        }
        else
        {
            // If we fall through to here, there is no defined code path to handle the
            // combination of T and K -- let's fail loudly.
            STATIC_COMPILE_ERROR("Unsupported combination of parent and child for getChildren() with filter");
        }
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
        constexpr LogTargetType l_plat_type = fapiTargetTypeToPlatTargetType<T>();

        l_platTarget.setIsMulticast(1);
        l_platTarget.setMcastGroup(i_group);
        l_platTarget.setTargetType(l_plat_type);

        return l_platTarget;
    }

    ///
    /// @brief Is the target functional?
    /// @return true if target is functional, false if non-functional
    ///
    template<TargetType K, MulticastType M, typename V>
    inline bool
    Target<K, M, V>::isFunctional(void) const
    {
        return iv_handle.getFunctional();
    }

    template< TargetType T, MulticastType M, typename V >
    inline void toString(const Target<T, M, V>& i_target, char* i_buffer, size_t i_bsize)
    {}
    template< TargetType T, MulticastType M, typename V >
    inline void toString(const Target<T, M, V>* i_target, char* i_buffer, size_t i_bsize)
    {}

    ///
    /// @brief Platform can return the core select value stored
    ///        in it's Target handle
    /// @tparam K The plat target handle typed
    /// @tparam M The target MulticastType
    /// @tparam V The type of parent
    /// @return Target<T, M, V> a core select value
    ///
    template<TargetType K, MulticastType M, typename V>
    inline MulticastCoreSelect
    Target<K, M, V>::_getCoreSelect(void) const
    {
        return (MulticastCoreSelect)(MCCORE_ALL);
    }

    ///
    /// @brief Here check that the platform target's type is
    ///        compatible with T, also taking multicast into account
    ///        (if the plat target is a real multicast target, T must include
    ///        TARGET_TYPE_MULTIAST). in it's Target handle
    /// @tparam K The plat target handle typed
    /// @tparam M The target MulticastType
    /// @tparam V The type of parent
    /// @param[in] A pointer to the Target<T, M, V>
    /// @return Target<T, M, V> a reduceType of target
    ///
    template<TargetType K, MulticastType M, typename V>
    template<TargetType T>
    inline ReturnCodes Target<K, M, V>::reduceType(Target<T, M, V> &o_target) const
    {
        const TargetType l_type = iv_handle.getFapiTargetType() |
            (iv_handle.getIsMulticast() ? TARGET_TYPE_MULTICAST : TARGET_TYPE_NONE);
        if ((l_type & T) != l_type)
        {
            return FAPI2_RC_INVALID_PARAMETER;
        }
        o_target = get();
        return FAPI2_RC_SUCCESS;
    }
}
