/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/plat_attribute_service.H $ */
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

// This file is to just not include target.H file from import
// Nothing required here.

#pragma once

#include <attribute_properties.H>
#include <attribute_macros.H>
#include <attribute_accessors.H>

// Work around for EC attributes
#ifdef ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4_GETMACRO
    #undef ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4_GETMACRO
    namespace fapi2
    {
        namespace ATTR
        {
            inline fapi2::ReturnCode get_ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4(ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4_Type& o_val)
            {
                o_val = 1;
                return fapi2::FAPI2_RC_SUCCESS;
            }
        }
    }
    #define ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4_GETMACRO(ID, TARGET, VAL) ATTR::get_ATTR_CHIP_EC_FEATURE_PERV_NET_REGION_3_NOT_4(VAL)
#endif

/*
 * @brief - plat_AttrInit : Initiallize attributes based on the platform logic
 */
void plat_AttrInit(fapi2::ATTR_NAME_Type i_chip_name);
