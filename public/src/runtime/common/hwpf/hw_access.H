/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/hw_access.H $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2023                        */
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
#include <hw_access_def.H>
#include <stdint.h>
#include <return_code.H>
#include <plat_trace.H>
#include <buffer.H>

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
        __attribute__((always_inline))
        inline ReturnCode  getScom(const Target<K, M, V>& i_target, const uint64_t i_address,
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
    __attribute__((always_inline))
    inline ReturnCode  putScom(const Target<K, M, V>& i_target, const uint64_t i_address,
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
        FAPI_ERR("GET-CFAM ACCESS via SBE is NOOP!");
        return FAPI2_RC_FALSE;
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
        FAPI_ERR("PUT-CFAM ACCESS via SBE is NOOP!");
        return FAPI2_RC_FALSE;
    }

    /// @brief Platform-level implementation  of putRing()
    /// Hardware procedures writers will not call this function.
    /// @tparam K template parameter, passed in target.
    /// @param[in] i_target   Target to operate on.
    /// @param[in] i_filePath File Path
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    template< TargetType K, MulticastType M, typename V >
    inline ReturnCode putRing(  const Target<K, M, V>& i_target,
                                const char * i_filePath)
    {
        ReturnCode l_rc = FAPI2_RC_SUCCESS;

        if( SBE::isSimicsRunning() )
        {
            return l_rc;
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
};

#endif // _FAPI2_HWACCESS_H_
