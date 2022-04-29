/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_handle_scom.C $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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

#include "plat_hw_access.H"

namespace fapi2
{
    fapi2::ReturnCode pibRcToFapiRc(const uint32_t i_pibRc)
    {
        fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
        //TODO: P11SBE Porting
        // Need Error XML support.

        return l_fapiRc;
    }

    fapi2::ReturnCode handle_scom_error(const uint32_t i_addr, uint8_t i_pibRc)
    {
        fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
        //TODO: P11SBE Porting
        // Need Error XML support.

        return l_fapiRc;
    }

};
