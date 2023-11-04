/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/attribute_override.C.t $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include <attribute.H>
#include <attribute_table.H>
#include <target_types.H>

namespace fapi2
{

namespace ATTR
{

AttributesTable g_attrsTab[] =
{
{% for key in target_types.keys() %}
{% for attr in attributes if attr.has_storage and attr.sbe_targ_type == key %}
{{attr.gen_attr_table()}}
{% endfor %}
{% endfor %}
}; //End g_attrsTab


TargetsTable  g_targetsTab[] =
{
{% set start_row=[0] %}
{% for key in target_types.keys() %}
{% set row_count=[0] %}
{% for attr in attributes if attr.has_storage and attr.sbe_targ_type == key %}
    {# <--- The below if-else statement is the workaround for performing #}
    {#      row_count = row_count + 1                                    #}
    {% if row_count.append(row_count.pop()+1)%} {% endif %}
{% endfor %}
{% if target_types[key].updateBlobGen %}
{ LOG_{{key}},{{target_types[key].ntargets}},{{start_row[0]}},{{row_count[0]}},1 },
{% else %}
{ LOG_{{key}},{{target_types[key].ntargets}},{{start_row[0]}},{{row_count[0]}},0 },
{% endif %}
    {# <--- The below if-else statement is the workaround for performing #}
    {#      start_row = start_row + row_count                    --->    #}
{% if start_row.append(start_row.pop()+row_count[0])%} {% endif %}
{% endfor %}
}; //End g_attrsTab

uint8_t g_tgts_tab_size = {{target_types.keys()|length}};
} //ATTR

} //fapi2
