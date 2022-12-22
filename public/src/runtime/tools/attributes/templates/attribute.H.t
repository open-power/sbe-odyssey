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
{% for attr in attributes if attr.has_storage %}

{{attr.get_template_definition()}}

{{attr.set_template_definition()}}

    {% for target_type in attr.sbe_target_type %}
        {% set ntargets = TARGET_TYPES[target_type].ntargets %}
{{attr.get_var_declaration(target_type,ntargets)}}

{{attr.get_template_specialization(target_type,ntargets)}}
{{attr.set_template_specialization(target_type,ntargets)}}

    {% endfor %}
{% endfor %}
} //ATTR

} //fapi2
