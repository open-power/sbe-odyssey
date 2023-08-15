/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/ody_dqs_track.C $ */
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

///
/// @file ody_dqs_track.C
/// @brief Procedure definition to run temp based DQS tracking recalibration
///
// *HWP HWP Owner: Geetha Pisapati <geetha.pisapati@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 1
// *HWP Consumed by: SBE

#include <fapi2.H>
#include <ody_dqs_track.H>

extern "C"
{

    ///
    /// @brief Runs a one-time DQS tracking recalibration over CCS
    /// @param[in] i_target the controller target
    /// @return FAPI2_RC_SUCCESS iff ok
    ///
    fapi2::ReturnCode ody_dqs_track( const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target )
    {
        FAPI_TRY(mss::ody::ody_dqs_track(i_target));
    fapi_try_exit:
        return fapi2::current_err;
    }
} //extern C
