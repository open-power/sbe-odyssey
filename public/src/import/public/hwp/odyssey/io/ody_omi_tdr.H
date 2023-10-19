/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/io/ody_omi_tdr.H $       */
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
///------------------------------------------------------------------------------
/// @file ody_omi_tdr.H
/// @brief Header file for Odyssey HWP that runs TDR on links
///
/// *HWP HW Maintainer : Josh Chica <josh.chica@ibm.com>
/// *HWP FW Maintainer :
/// *HWP Consumed by: SBE
///------------------------------------------------------------------------------

#pragma once
#include <fapi2.H>
#include <hwp_data_stream.H>

// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*ody_omi_tdr_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>&,
        const uint32_t,
        fapi2::hwp_bit_ostream&);

extern "C"
{
    /// @brief Initiates TDR run on Odyssey links to check for open/short
    /// @param[in]  i_target            Reference to TARGET_TYPE_OCMB_CHIP target
    /// @param[in]  i_link_mask         Which chip units will be running this command (For consistency with P/Z)
    /// @param[in]  o_ostream           Stream object that writes the data to /tmp
    /// @return  FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode ody_omi_tdr(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                  const uint32_t i_link_mask,
                                  fapi2::hwp_bit_ostream& o_ostream);
}
