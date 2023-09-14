/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/hw_access.H $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2023                        */
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
/**
 *  @file hw_access.H
 *
*  @brief Defines the hardware access functions that platform code must
 *         implement.
 */

#ifndef FAPI2_HWACCESS_H_
#define FAPI2_HWACCESS_H_

#include <plat_hw_access.H>
#include <plat_ring_utils.H>
#include <hw_access_def.H>
#include <stdint.h>
#include <return_code.H>
#include <plat_trace.H>
#include <buffer.H>
#include "poz_putRingBackend.H"

namespace fapi2
{
    //--------------------------------------------------------------------------
    // HW Communication Functions
    //--------------------------------------------------------------------------

    /// @brief Reads a SCOM register from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target     HW target to operate on.
    /// @param[in]  i_address    SCOM register address to read from.
    /// @param[out] o_data       Buffer that holds data read from HW target.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K, MulticastType M, typename V >
        ReturnCode  getScom(const Target<K, M, V>& i_target, const uint64_t i_address,
                buffer<uint64_t>& o_data)
        {
            uint64_t & io_data = o_data;
            ReturnCode l_rc;
            PLAT_GETSCOM(l_rc,
                    i_target,
                    i_address,
                    io_data);

            return l_rc;
        }

    /// @brief Writes a SCOM register on a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target     HW target to operate on.
    /// @param[in] i_address    SCOM register address to write to.
    /// @param[in] i_data       Buffer that holds data to write into address.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K, MulticastType M, typename V >
    ReturnCode  putScom(const Target<K, M, V>& i_target, const uint64_t i_address,
                       const buffer<uint64_t> i_data)
    {
        uint64_t io_data = i_data;
        ReturnCode l_rc;
        PLAT_PUTSCOM(l_rc,
                    i_target,
                    i_address,
                    io_data);

        return l_rc;
    }

    ///
    /// @brief Platform-level implementation called by getCfamRegister()
    /// Hardware procedures writers will not call this function.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   HW target to operate on.
    /// @param[in]  i_address  CFAM address to read from.
    /// @param[out] o_data     32-bit buffer that holds data read from HW target.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    template< TargetType K, MulticastType M, typename V >
    inline ReturnCode getCfamRegister(const Target<K, M, V>& i_target,
                                    const uint32_t i_address,
                                    buffer<uint32_t>& o_data)
    {
        return fapi2::get_cfam_abs(&i_target, i_address, o_data);
    }

    ///
    /// @brief Platform-level implementation of putCfamRegister()
    /// Hardware procedures writers will not call this function.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   HW target to operate on.
    /// @param[in]  i_address  CFAM address to write to.
    /// @param[out] i_data     32-bit buffer that holds data to write into address.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    template< TargetType K, MulticastType M, typename V >
    inline ReturnCode putCfamRegister(const Target<K, M, V>& i_target,
                                    const uint32_t i_address,
                                    const buffer<uint32_t> i_data)
    {
        return fapi2::put_cfam_abs(&i_target, i_address, i_data);
    }

/// @brief Platform-level implementation  of putRing()
/// Hardware procedures writers will not call this function.
/// @tparam K template parameter, passed in target.
/// @param[in] i_target   Target to operate on.
/// @param[in] i_ringID   Ring ID name that will identify the Ring in the image.
/// @param[in] i_ringMode Ring operation mode.
/// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
template< TargetType K, MulticastType M, typename V >
ReturnCode putRing(const Target<K, M, V>& i_target,
                   const char* i_ringID,
                   const RingMode i_ringMode = RING_MODE_HEADER_CHECK)
{
    fapi2::ReturnCode l_rc = FAPI2_RC_SUCCESS;

    l_rc = poz_putRingBackend(i_target,
                              i_ringID, i_ringMode);

    if( l_rc != FAPI2_RC_SUCCESS )
    {
        FAPI_ERR("poz_putRingBackend failed with RC[%08X]", l_rc);
    }

    return l_rc;
}

    /// @brief Reads a ring from a chip.
    /// @tparam K template parameter, passed in target.
    /// @param[in]  i_target   Target to operate on.
    /// @param[in]  i_address  Ring address to read from.
    /// @param[out] o_data     Buffer that holds data read from HW target.
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K, MulticastType M, typename V >
    inline ReturnCode getRing(const Target<K, M, V>& i_target,
                       const scanRingId_t i_address,
                       const char * o_data)
    {
#ifndef __PPE__
        std::cout << std::hex << "   getRing "
                  << "target: {" << i_target.getType() << ","
                  << uint64_t(i_target) << "}; "
                  << "ring address: " << i_address << "; "
                  << "output data:";
                  o_data.print();
#endif
        return FAPI2_RC_SUCCESS;
    }

    /// @brief Backend platform implementation of putRing
    /// This applies a single scan image to a given target.
    /// Hardware procedures will not usually call this function; it is
    /// supposed to be called by putRingBackend() only.
    //  @tparam K template parameter, passed in target.
    /// @param[in] i_target    Target to scan into. May be multicast!
    /// @param[in] i_scanImage Scan image to apply
    /// @param[in] i_modifiedRingAddress
    ///                        Ring address modified to indicate the actual
    ///                        scan regions targeted by this call
    /// @param[in] i_ringMode  Ring operation mode.
    /// @return FAPI2_RC_SUCCESS on success, else error code.
    template< TargetType K, MulticastType M, typename V >
    ReturnCode plat_putRingImpl(const Target<K, M, V>& i_target,
                                const void* i_scanImage,
                                const uint32_t i_modifiedRingAddress,
                                const RingMode i_ringMode)
    {
        FAPI_INF("PUT-RING: plat_putRingImpl inside SBE plat_putringutil ");
        ReturnCode l_rc = plat_putringutil( (void*)&i_target, i_scanImage,
                                            i_modifiedRingAddress, i_ringMode );
        return l_rc;
    }
};

#endif // _FAPI2_HWACCESS_H_
