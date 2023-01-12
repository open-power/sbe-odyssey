/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_ring_utils.H $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#ifndef PLATRINGACCESS_H_
#define PLATRINGACCESS_H_

#include <return_code.H>
#include <hw_access_def.H>

namespace fapi2
{
    // This function loads the scan region data for the given ring address and
    // updates the check word data
    // @param[in] : ring addtress
    // @param[in]:  ring mode
    // @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode getRing_setup(const uint32_t i_ringAddress,
                             const RingMode i_ringMode);

    // This function verify the check word data is matching or not and will
    // clean up the scan region data
    // @param[in] i_ringAddress - absolute ring address
    // @param[in] i_ringMode - Ring mode value
    // @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode getRing_verifyAndcleanup(const uint32_t i_ringAddress,
                                        const RingMode i_ringMode);

    /// @brief Backend platform implementation of iplat putRing
    /// This applies a single scan image to a given target.
    /// Hardware procedures will not usually call this function; it is
    /// supposed to be called by putRingBackend() only.
    /// @param[in] i_target    Target to scan into. May be multicast!
    /// @param[in] i_scanImage Scan image to apply
    /// @param[in] i_modifiedRingAddress
    ///                        Ring address modified to indicate the actual
    ///                        scan regions targeted by this call
    /// @param[in] i_ringMode  Ring operation mode.
    /// @return FAPI2_RC_SUCCESS on success, else error code.
    fapi2::ReturnCode plat_putringutil( const void *i_target,
                            const void *i_scanImage,
                            const uint32_t i_modifiedRingAddress,
                            const fapi2::RingMode i_ringMode);

}

#endif // PLATRINGACCESS_H_

