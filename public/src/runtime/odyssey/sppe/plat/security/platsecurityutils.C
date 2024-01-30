/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/security/platsecurityutils.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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

#include "platsecurityutils.H"
#include "sbetrace.H"
#include "fapi2.H"
#include "sbeglobals.H"

using namespace fapi2;

void platSecurityUtils::setAttrSecurityLvl()
{
    #define SBE_FUNC " setAttrSecurityLvl "
    SBE_ENTER(SBE_FUNC)

    // Set the ATTR_SECURITY_LEVEL
    SBE_INFO(SBE_FUNC "Updating ATTR_SECURITY_LEVEL to 0x%02x", getSecurityLevel());
    ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_SECURITY_LEVEL = getSecurityLevel();

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}
