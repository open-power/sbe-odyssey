/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/security/sbesecurity.C $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#include "sbesecurity.H"
#include "sbetrace.H"
#include "sbeglobals.H"
#include "sbesecuritygen.H"
#include "attribute.H"
#include "attrutils.H"

namespace SBE_SECURITY
{
//----------------------------------------------------------------------------
bool isAllowed(const uint32_t i_addr, uint64_t i_mask,  accessType i_type)
{
    bool ret = true;

    if(i_type == WRITE)
    {
        ret =  ALLOWLIST::isPresent(i_addr);
        if( (ret == false ) && (i_mask != 0xffffffffffffffffull ))
        {
            ret = PARTIALALLOWLIST::isPresent(i_addr, i_mask);
        }
    }
    else if(i_type == READ)
        ret =  !DENYLIST::isPresent(i_addr);
    SBE_INFO("SBE_SECURITY access[%d] allowed[%d] addr[0x%08x]",
                                    i_type, ret, i_addr);

    return ret;
}
} // namespace SBE_SECURITY
