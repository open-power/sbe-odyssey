/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/attribute.H.t $ */
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
#include <attribute_properties.H>
#include <attribute_macros.H>

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
fapi2::ReturnCode get_{{attr.name}}(const fapi2::Target<T>& i_target, {{attr.name}}_Type& o_val)
{
    return fapi2::FAPI2_RC_SUCCESS;
}
template <TargetType T>
fapi2::ReturnCode set_{{attr.name}}(const fapi2::Target<T>& i_target, {{attr.name}}_Type& o_val)
{
    return fapi2::FAPI2_RC_SUCCESS;
}
{% endif %}

namespace {{attr.sbe_targ_type}}
{
    extern {{attr.value_type}} {{attr.name}}{{attr.internal_dims}} __attribute__((section(".attrs")));
}

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
