/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/fire_omi_setup_bars.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

///
/// @file fire_omi_setup_bars.H
/// @brief Contains the explorer OMI setup bar regs for apollo
///
// *HWP HWP Owner: Edgar Cordero <ecordero@us.ibm.com>
// *HWP HWP Backup: Ryan King <rpking@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: Memory

#ifndef FIRE_OMI_SETUP_BARS_H
#define FIRE_OMI_SETUP_BARS_H

#include <fapi2.H>

typedef fapi2::ReturnCode (*fire_omi_setup_bars_FP_t) (const fapi2::Target<fapi2::TARGET_TYPE_MCC>&);

extern "C"
{

    ///
    /// @brief Trains the OCMB link on Apollo
    /// @param[in] i_target the MCC in Fire target to operate on
    /// @return FAPI2_RC_SUCCESS iff ok
    ///

    fapi2::ReturnCode fire_omi_setup_bars(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target);
}

#endif
