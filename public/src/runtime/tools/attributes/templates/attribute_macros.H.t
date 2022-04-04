/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/attribute_macros.H.t $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include <ecfeatureutils.H>
#include <ppe42_string.h>
{% if virt_header %}
#include <{{virt_header}}>
{% endif %}

namespace fapi2
{
// when user calling FAPI_ATTR_GET(fapi2::<ATTR_ID>,...)
// fapi will convert this to fapi2::<ATTR_ID>_GETMACRO
// if we add a dummy function call before SET/GET call, 'fapi2::' scope operator will
// be applied to dummy function
__attribute__ ((always_inline)) inline void attr_macro_dummy(){}
} //fapi2

{% for attr in attributes %}
/*-----------------------------------------*/
#define {{attr.name}}_GETMACRO(ID, TARGET, VAL) {{attr.name}}_GETMACRO_HELPER(ID, TARGET, VAL)
#define {{attr.name}}_SETMACRO(ID, TARGET, VAL){% if attr.writeable %} {{attr.name}}_SETMACRO_HELPER(ID, TARGET, VAL){% endif +%}
#define {{attr.name}}_PLAT_INIT(ID, TARGET, VAL){% if attr.platinit %} {{attr.name}}_SETMACRO_HELPER(ID, TARGET, VAL){% endif +%}

#define {{attr.name}}_GETMACRO_HELPER(ID, TARGET, VAL) \
    attr_macro_dummy(), {{attr.getter}}
{% if attr.writeable or attr.platinit %}
#define {{attr.name}}_SETMACRO_HELPER(ID, TARGET, VAL) \
    attr_macro_dummy(), {{attr.setter}}
{% endif %}

{% endfor %}
