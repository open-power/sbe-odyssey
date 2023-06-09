/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/plat_attribute_service.C $ */
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

#include <plat_attribute_service.H>
#include "sbeutil.H"

void plat_AttrInit(fapi2::ATTR_NAME_Type i_chip_name)
{
#define SBE_FUNC "plat_AttrInit "

    if(SBE::isSimicsRunning())
    {
        SBE_INFO(SBE_FUNC "Setting ATTR_IS_SIMICS");

        fapi2::ATTR::TARGET_TYPE_SYSTEM::ATTR_IS_SIMICS = 1;
    }

    fapi2::ATTR::ATTR_NAME = i_chip_name;

#undef SBE_FUNC
}
