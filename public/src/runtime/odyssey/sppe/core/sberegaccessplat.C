/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sberegaccessplat.C $     */
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
#include "sberegaccess.H"
#include "sbetrace.H"
#include "fapi2.H"
#include "sbeutil.H"

void SbeRegAccess::platInitAttrBootFlags()
{
    #define SBE_FUNC "SbeRegAccess::platInitAttrBootFlags "
    uint32_t l_attr = 0;
    FAPI_ATTR_GET(fapi2::ATTR_OCMB_BOOT_FLAGS,
                fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>(),l_attr);
    SBE_INFO(SBE_FUNC" mbx11 from Attribute, 0x%08x", l_attr);
    SbeRegAccess::theSbeRegAccess().updateMbx11( (uint64_t)l_attr<<32 );
    #undef SBE_FUNC
}