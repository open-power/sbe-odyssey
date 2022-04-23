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

#include "target.H"
#include "hw_access.H"
#include "plat_hw_access.H"
#include "ppe42_scom.h"
#include "hwp_return_codes.H"
#include "plat_error_scope.H"
#include "hwp_ffdc_classes.H"
#include "error_info_defs.H"
#include "ffdc.H"
#include <error_info.H>

namespace fapi2
{

static uint32_t getEffectiveAddress(const uint32_t *i_target, const uint32_t i_addr)
{

    uint32_t translatedAddr = i_addr;
    // TODO: P11SBE Porting
    // Need target support

    return translatedAddr;
}

fapi2::ReturnCode pibRcToFapiRc(const uint32_t i_pibRc)
{
    fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    switch(i_pibRc)
    {
        case PIB_XSCOM_ERROR:
            l_fapiRc = RC_SBE_PIB_XSCOM_ERROR;
            break;
        case PIB_OFFLINE_ERROR:
            l_fapiRc = RC_SBE_PIB_OFFLINE_ERROR;
            break;
        case PIB_PARTIAL_ERROR:
            l_fapiRc = RC_SBE_PIB_PARTIAL_ERROR;
            break;
        case PIB_ADDRESS_ERROR:
            l_fapiRc = RC_SBE_PIB_ADDRESS_ERROR;
            break;
        case PIB_CLOCK_ERROR:
            l_fapiRc = RC_SBE_PIB_CLOCK_ERROR;
            break;
        case PIB_PARITY_ERROR:
            l_fapiRc = RC_SBE_PIB_PARITY_ERROR;
            break;
        case PIB_TIMEOUT_ERROR:
            l_fapiRc = RC_SBE_PIB_TIMEOUT_ERROR;
            break;
        case PIB_NO_ERROR:
        default:
            break;
    }
    return l_fapiRc;
}

fapi2::ReturnCode handle_scom_error(const uint32_t i_addr, uint8_t i_pibRc)
{
    PLAT_FAPI_ASSERT( false,
                      SBE_SCOM_FAILURE().
                      set_address(i_addr).
                      set_pcb_pib_rc(i_pibRc),
                      "SCOM : pcb pib error, pibRc[0x%08X] Translated_ScomAddr[0x%08X]",
                      i_pibRc, i_addr);
    fapi_try_exit:
        // Override FAPI RC based on PIB RC
        fapi2::current_err = pibRcToFapiRc(i_pibRc);
        fapi2::g_FfdcData.fapiRc = fapi2::current_err;
    return fapi2::current_err;
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
