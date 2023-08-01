/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attrutils.C $ */
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

#include "attrutils.H"
#include "attribute.H"

using namespace fapi2;

void platSetAttrSecurityLvl(fapi2::ATTR_SECURITY_LEVEL_Type i_securityLvl)
{
    ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_SECURITY_LEVEL = i_securityLvl;
}

uint8_t platGetChipType()
{
    return (CHIP_TYPE_ODYSSEY_00 +
            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_BUS_POS);
}

