/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/attribute_properties.H.t $ */
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
#include <attribute_ids.H>

namespace fapi2
{
{% for attr in attributes %}

/* {{attr.name}} */
typedef {{attr.value_type}} {{attr.name}}_Type{{attr.type_dims}};
const fapi2::TargetType {{attr.name}}_TargetType = {{attr.ekb_target_type}};
const bool {{attr.name}}_PlatInit = true;
const bool {{attr.name}}_InitToZero = false;
const bool {{attr.name}}_Privileged = false;
{% if attr.enum_values %}
enum {{attr.name}}_Enum
{
    {% for enum_name, enum_value in attr.enum_values.items() %}
    ENUM_{{attr.name}}_{{enum_name}} = {{enum_value|hex}},
    {% endfor %}
};
{% endif %}
{% endfor %}

} //fapi2
