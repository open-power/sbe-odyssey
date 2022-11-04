/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/ring/generic/poz_putRingUtils.H $    */
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
//------------------------------------------------------------------------------
/// @file   poz_putRingUtils.H
/// @brief  Given a chiplet target, scan address and RS4 image, scan the image
///         contents into the ring(s) targeted by the target & scan address
//------------------------------------------------------------------------------
// *HWP HW Maintainer:    Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer:    Prem S Jha <premjha2@in.ibm.com>
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

extern "C"
{

    typedef fapi2::ReturnCode( *poz_putRingUtils_FP_t )(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
        const uint8_t* i_rs4,
        const uint32_t i_modifiedRingAddress,
        const fapi2::RingMode i_ringMode);

/// @brief This applies a single scan image to a given target.
/// @param[in] i_target    Target to scan into. May be multicast!
/// @param[in] i_scanImage Scan image to apply
/// @param[in] i_modifiedRingAddress
///                        Ring address modified to indicate the actual
///                        scan regions targeted by this call
/// @param[in] i_ringMode  Ring operation mode.
/// @return FAPI2_RC_SUCCESS on success, else error code.
    fapi2::ReturnCode poz_putRingUtils(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
        const uint8_t* i_rs4,
        const uint32_t i_modifiedRingAddress,
        const fapi2::RingMode i_ringMode);

} //extern "C"
