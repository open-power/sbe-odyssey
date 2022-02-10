/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_hw_access.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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

#include "hw_access.H"
#include "plat_hw_access.H"
#include "ppe42_scom.h"

namespace fapi2
{

static uint32_t getEffectiveAddress(const uint32_t *i_target, const uint32_t i_addr)
{

    uint32_t translatedAddr = i_addr;
    // TODO: P11SBE Porting
    // Need target support

    return translatedAddr;
}


fapi2::ReturnCode getscom_abs_wrap(const void *i_target,
                                   const uint32_t i_addr, uint64_t *o_data)
{
    uint32_t l_pibRc = 0;
    uint32_t l_addr = i_addr;
    l_addr = getEffectiveAddress((uint32_t *)i_target, i_addr);
    l_pibRc = getscom_abs(l_addr, o_data);
    return (l_pibRc == PIB_NO_ERROR) ? fapi2::ReturnCode(FAPI2_RC_SUCCESS) :
                 handle_scom_error(l_addr, l_pibRc);
}

fapi2::ReturnCode putscom_abs_wrap(const void *i_target,
                                    const uint32_t i_addr, uint64_t i_data)
{
    uint32_t l_pibRc = 0;
    uint32_t l_addr = i_addr;
    l_addr = getEffectiveAddress((uint32_t *)i_target, i_addr);
    l_pibRc = putscom_abs(l_addr, i_data);
    return (l_pibRc == PIB_NO_ERROR) ? fapi2::ReturnCode(FAPI2_RC_SUCCESS) :
                 handle_scom_error(l_addr, l_pibRc);
}

};
