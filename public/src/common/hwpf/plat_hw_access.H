/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/hwpf/plat_hw_access.H $                     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
 *  @file plat_hw_access.H
 *
 *  @brief Define platform specific calls for hardware accesses.
 */

#ifndef PLATHWACCESS_H_
#define PLATHWACCESS_H_

#include <return_code.H>

// PIB errors
enum PCB_ERRORS
{
    PIB_NO_ERROR      = 0x0,
    PIB_XSCOM_ERROR   = 0x1,
    PIB_OFFLINE_ERROR = 0x2,
    PIB_PARTIAL_ERROR = 0x3,
    PIB_ADDRESS_ERROR = 0x4,
    PIB_CLOCK_ERROR   = 0x5,
    PIB_PARITY_ERROR  = 0x6,
    PIB_TIMEOUT_ERROR = 0x7
};

/// GetScom
#define PLAT_GETSCOM(_m_rc, _m_base, _m_offset, _m_data) \
    _m_rc = fapi2::getscom_abs_wrap(&(_m_base), _m_offset, _m_data)

/// PutScom
#define PLAT_PUTSCOM(_m_rc, _m_base, _m_offset, _m_data) \
    _m_rc = fapi2::putscom_abs_wrap(&(_m_base), _m_offset, _m_data)

namespace fapi2
{
    ///
    /// @brief Platform wrapper over PK getscom_abs
    ///
    /// @param [in]  i_addr The SCOM address
    /// @param [out] o_data The data read
    ///
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    fapi2::ReturnCode getscom_abs_wrap(const void *i_target,
                                    const uint32_t i_addr, uint64_t *o_data);

    ///
    /// @brief Platform wrapper over PK putscom_abs
    ///
    /// @param [in] i_addr The SCOM address
    /// @param [in] i_data The data to write
    ///
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    fapi2::ReturnCode putscom_abs_wrap(const void *i_target,
                                    const uint32_t i_addr, uint64_t i_data);

    ///
    /// @brief Map platform specific pib rc to fapi rc.
    ///
    /// @param [in] i_pibRc
    ///
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    fapi2::ReturnCode pibRcToFapiRc(const uint32_t i_pibRc);

    ///
    /// @brief handle scom error 
    ///
    /// @param [in] i_addr   The SCOM address
    /// @param [in] i_pibRc  scom return code
    ///
    /// @return fapi::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
    ///
    fapi2::ReturnCode handle_scom_error(const uint32_t i_addr, uint8_t i_pibRc);
}


#endif // PLATHWACCESS_H_

