/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/ring/generic/poz_putRingBackend.H $  */
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
/// @file  poz_putRingBackend.H
/// @brief Backend code for fapi2::putRing
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer   : TBD
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

typedef fapi2::ReturnCode (*poz_putRingBackend_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>&,
        const char*, const fapi2::RingMode);

/// @brief Apply a composite scan image in memory to a target as putRingBackend would
///
/// This function is internally used by putRingBackend. It is exposed for use by chipOps.
///
/// It will locate and load composite scan images by decomposing each image into its
/// constituent RS4s and applying them to the target in order.
///
/// This function will accept an entire chip as a target or a chipUnit (multicast or unicast).
/// In the case of the chip, the HWP will apply any RS4 to all functional units in the chip which
/// match the RS4.
/// In the case of a chipUnit, the HWP will apply all RS4s that match the targeted chipUnit and
/// skip all others.
///
/// @param[in]  i_target      Target to apply the scan image to
/// @param[in]  i_image       Pointer to a composite scan image (or a single RS4)
/// @param[in]  i_size        Size if the scan image in memory
/// @param[in]  i_ring_mode   Special flags for applying the image
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_applyCompositeImage(const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
        const void* i_image,
        const size_t i_size,
        const fapi2::RingMode i_ringMode = fapi2::RING_MODE_HEADER_CHECK);

/// @brief Backend code for fapi2::putRing; apply a scan image to a target
///
/// This HWP will locate and load composite scan images based on their name (the i_ring_id parameter).
/// It will go through several image sources (base, cust, dynamic inits, HB/BMC/Lab overrides) and
/// apply any composite images it finds in order.
/// It will decompose each image into its constituent RS4s and apply them to the target in order.
///
/// The HWP will accept an entire chip as a target or a chipUnit (multicast or unicast).
/// In the case of the chip, the HWP will apply any RS4 to all functional units in the chip which
/// match the RS4.
/// In the case of a chipUnit, the HWP will apply all RS4s that match the targeted chipUnit and
/// skip all others.
///
/// @param[in]  i_target      Target to apply the scan image to
/// @param[in]  i_ring_id     Name of the scan image
/// @param[in]  i_ring_mode   Special flags for applying the image
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_putRingBackend(const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
                                         const char* i_ring_id,
                                         const fapi2::RingMode i_ringMode = fapi2::RING_MODE_HEADER_CHECK);
}
