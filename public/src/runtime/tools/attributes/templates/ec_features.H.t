/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/tools/attributes/templates/ec_features.H.t $ */
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
#include <ecfeatureutils.H>

namespace fapi2
{
{% for attr in attributes if attr.has_ec %}

inline bool hasFeature(int2Type<{{attr.name}}>, fapi2::ATTR_NAME_Type i_name, fapi2::ATTR_EC_Type i_ec)
{
   uint8_t hasFeature = false;

    if(((i_name == {{attr.chip_name}}) &&  (i_ec == {{attr.ec_value}})))
    {
        hasFeature = true;
    }
    return hasFeature;
};
{% endfor %}

} //fapi2
