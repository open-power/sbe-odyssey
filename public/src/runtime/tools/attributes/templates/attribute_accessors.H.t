/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/attribute_accessors.H.t $ */
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
/* This file is generated from a python script - {{toolname}} */
#pragma once
#include <attribute.H>
#include <attribute_properties.H>
#include <fapi2_target.H>
#include <target.H>
#include <sbetrace.H>
#include <ppe42_string.h>

namespace fapi2
{
namespace ATTR
{

/* This template function is only to simplify the jinja template*/
template<typename T>
inline void attr_copy(T& dest, T& src)
{
    memcpy(&dest, &src, sizeof(T));
}

template<>
inline void attr_copy<uint8_t>(uint8_t& dest, uint8_t& src)
{
    dest = src;
}

template<>
inline void attr_copy<uint16_t>(uint16_t& dest, uint16_t& src)
{
    dest = src;
}

template<>
inline void attr_copy<uint32_t>(uint32_t& dest, uint32_t& src)
{
    dest = src;
}

template<>
inline void attr_copy<uint64_t>(uint64_t& dest, uint64_t& src)
{
    dest = src;
}

{% for attr in attributes if attr.has_storage %}
{% if attr.first_attribute %}
template <TargetType T>
fapi2::ReturnCode get_{{attr.name}}(const fapi2::Target<T>& i_target, {{attr.name}}_Type& o_val);

template <TargetType T>
fapi2::ReturnCode set_{{attr.name}}(const fapi2::Target<T>& i_target, {{attr.name}}_Type& o_val);
{% endif %}

{% if attr.support_composite_target %}
template <TargetType T>
inline fapi2::ReturnCode get_{{attr.name}}(
    const fapi2::Target<T>& i_target,
    {{attr.name}}_Type& o_val)
{
    constexpr fapi2::TargetType EKB_FULL_TARGET = {{attr.ekb_full_target}};
    static_assert(((T & EKB_FULL_TARGET) == T), "Invalid composite target");

    fapi2::TargetType l_type = i_target.get().getFapiTargetType();
    fapi2::ReturnCode l_rc   = fapi2::FAPI2_RC_SUCCESS;

    switch (l_type)
    {
    {% for targ in attr.ekb_target_list if targ in target_types.keys() %}
    case fapi2::{{targ}}:
        attr_copy(o_val,
        fapi2::ATTR::{{targ}}::{{attr.name}}{{attr.inst_index(target_types[targ].ntargets, targ,'i_target', target_types[attr.shared_mem_targets[targ]].ntargets if targ in attr.shared_mem_targets.keys())}});
        break;
    {% endfor %}
    default:
        l_rc = fapi2::FAPI2_RC_FALSE;
        SBE_ERROR("The target passed (type=0x%08X%08X) is not valid for the "
            "attribute {{attr.name}}", (l_type >> 32), (l_type & 0xFFFFFFFF));
        break;
    }
    return l_rc;
}

template <TargetType T>
inline fapi2::ReturnCode set_{{attr.name}}(
    const fapi2::Target<T>& i_target,
    {{attr.name}}_Type& o_val)
{
    constexpr fapi2::TargetType EKB_FULL_TARGET = {{attr.ekb_full_target}};
    static_assert(((T & EKB_FULL_TARGET) == T), "Invalid composite target");

    fapi2::TargetType l_type = i_target.get().getFapiTargetType();
    fapi2::ReturnCode l_rc   = fapi2::FAPI2_RC_SUCCESS;

    switch (l_type)
    {
    {% for targ in attr.ekb_target_list if targ in target_types.keys() %}
    case fapi2::{{targ}}:
        attr_copy(fapi2::ATTR::{{targ}}::{{attr.name}}{{attr.inst_index(target_types[targ].ntargets,targ,'i_target',target_types[attr.shared_mem_targets[targ]].ntargets if targ in attr.shared_mem_targets.keys())}}, o_val);
        break;
    {% endfor %}
    default:
        l_rc = fapi2::FAPI2_RC_FALSE;
        SBE_ERROR("The target passed (type=0x%08X%08X) is not valid for the "
             "attribute {{attr.name}}",(l_type >> 32), (l_type & 0xFFFFFFFF));
        break;
    }
    return l_rc;
}
{% endif %}

template <>
inline fapi2::ReturnCode get_{{attr.name}}(
    const fapi2::Target<fapi2::{{attr.sbe_targ_type}}>& i_target,
    {{attr.name}}_Type& o_val)
{
    attr_copy(o_val, fapi2::ATTR::{{attr.sbe_targ_type}}::{{attr.name}}{{attr.targ_inst('i_target')}});
    return fapi2::FAPI2_RC_SUCCESS;
}

template <>
inline fapi2::ReturnCode set_{{attr.name}}(
    const fapi2::Target<fapi2::{{attr.sbe_targ_type}}>& i_target,
    {{attr.name}}_Type& o_val)
{
    attr_copy(fapi2::ATTR::{{attr.sbe_targ_type}}::{{attr.name}}{{attr.targ_inst('i_target')}}, o_val);
    return fapi2::FAPI2_RC_SUCCESS;
}

{% endfor %}
} //ATTR

} //fapi2
