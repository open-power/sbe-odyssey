/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/return_code.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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
 * @file return_code.H
 * @brief definitions for fapi2 return codes
 */

#ifndef __FAPI2_RETURN_CODE__
#define __FAPI2_RETURN_CODE__

#include <stdint.h>
#include <return_code_defs.H>

#ifndef __SBE__
    #include <hwp_return_codes.H>
#endif

#ifndef FAPI2_NO_FFDC
    #include <ffdc.H>
#endif

// MINIMUM_FFDC_RE feature is on top of MINIMUM_FFDC feature.
//   So MINIMUM_FFDC needed for enabling MINIMUM_FFDC_RE.
//   Checking this at compile time.
#if !defined (MINIMUM_FFDC) && defined (MINIMUM_FFDC_RE)
    static_assert(false, "MINIMUM_FFDC is not defined");
#endif

namespace fapi2
{
///
/// @brief Class representing a FAPI2 ReturnCode
///
// Remove the inheritance relationship with FirstFailureData if
// the platform doesn't support FFDC.
#if defined (FAPI2_NO_FFDC) || defined (MINIMUM_FFDC)
class ReturnCode
#else
class ReturnCode : public FirstFailureData<ReturnCode>
#endif
{
    public:

#if defined (MINIMUM_FFDC_RE)
        ///
        /// @brief Constructor for ReturnCode
        /// @param[in] i_rc Initial value for the RC. Default is FAPI2_RC_SUCCESS
        /// @param[in] i_data_ptr Initial value for the data pointer. Default is 0
        ///
        ReturnCode(const uint32_t i_rc = FAPI2_RC_SUCCESS, const uint32_t i_data_ptr = 0):
            iv_rc(i_rc), iv_data_ptr(i_data_ptr) {};
#else
        ///
        /// @brief Constructor for ReturnCode.
        /// @param[in] i_rc Initial value for the RC. Default is FAPI2_RC_SUCCESS
        ///
        ReturnCode(const uint32_t i_rc = FAPI2_RC_SUCCESS):
            iv_rc(i_rc)
        {};
#endif

        ///
        /// @brief integral type conversion function. Returns the error code
        /// @return The error code
        ///
        inline operator uint32_t() const
        {
            return iv_rc;
        }

        ///
        /// @brief integral type conversion function. Returns the error code
        /// @return The error code
        ///
        inline operator uint64_t() const
        {
            return iv_rc;
        }

        ///
        /// @brief Returns true iff iv_rc != SUCCESS
        /// @return true or false
        ///
        inline operator bool() const
        {
            return iv_rc != FAPI2_RC_SUCCESS;
        }

        ///
        /// @brief Assignement operator
        ///
#ifdef DOXYGEN
        inline ReturnCode& operator=(const uint32_t& rhs);
        inline ReturnCode& operator=(const ReturnCodes& rhs);
#endif
#if defined (MINIMUM_FFDC_RE)
        inline ReturnCode& operator=(const uint32_t& rhs)
        {
            iv_rc = rhs;
            iv_data_ptr = 0;
            return *this;
        }
        inline ReturnCode& operator=(const ReturnCodes& rhs)
        {
            iv_rc = rhs;
            iv_data_ptr = 0;
            return *this;
        }
#endif
        inline bool operator==(const uint32_t& rhs) const
        {
            return rhs == iv_rc;
        }

        inline bool operator==(const ReturnCodes& rhs) const
        {
            return rhs == iv_rc;
        }

        inline bool operator!=(const uint32_t& rhs) const
        {
            return rhs != iv_rc;
        }

        inline bool operator!=(const ReturnCodes& rhs) const
        {
            return rhs != iv_rc;
        }

        ///
        /// @brief Enumeration of return code creators
        ///
        enum returnCodeCreator
        {
            CREATOR_FAPI = 1,
            CREATOR_PLAT = 2,
            CREATOR_HWP = 3,
        };

        ///
        /// @brief Gets the creator of the return code
        /// @return ReturnCodeCreator
        ///
        inline returnCodeCreator getCreator(void) const
        {
            returnCodeCreator l_creator = CREATOR_HWP;

            if (iv_rc & FAPI2_RC_FAPI2_MASK)
            {
                l_creator = CREATOR_FAPI;
            }
            else if (iv_rc & FAPI2_RC_PLAT_MASK)
            {
                l_creator = CREATOR_PLAT;
            }

            return l_creator;
        }


        ///
        /// @brief explicit check for RC value
        /// @return true if this instance has a matching RC
        ///
        inline bool isRC(const ReturnCodes i_rc) const
        {
            return static_cast<uint32_t>(i_rc) == iv_rc;
        }


        ///
        /// @brief explicit check for RC value
        /// @return true if this instance has a matching RC
        ///
        inline bool isRC(const uint32_t i_rc) const
        {
            return i_rc == iv_rc;
        }

        ///
        /// @brief set the underlying return code to the given value
        /// @param[in] i_rc the target return code
        ///
        inline void setRC(const uint32_t i_rc)
        {
            iv_rc = i_rc;
        }

        ///
        /// @brief Retrieves the return code (RC)
        ///        This inline function retrieves the value of the member
        ///        variable iv_rc.
        /// @return The current value of iv_rc.
        ///
        inline uint32_t getRC(void)
        {
            return iv_rc;
        }

#if defined (MINIMUM_FFDC_RE)
        ///
        /// @brief Sets the data pointer.
        ///        This inline function sets the value of the member variable
        ///        iv_data_ptr.
        /// @param[in] i_data_ptr The data pointer to be set.
        ///
        inline void setDataPtr(const uint32_t i_data_ptr)
        {
            iv_data_ptr = i_data_ptr;
        }

        ///
        /// @brief Retrieves the data pointer
        ///        This inline function retrieves the value of the member
        ///        variable iv_data_ptr.
        /// @return The current value of iv_data_ptr.
        ///
        inline uint32_t getDataPtr(void)
        {
            return iv_data_ptr;
        }
#endif

    private:
        uint32_t iv_rc __attribute__ ((aligned (8)));
#if defined (MINIMUM_FFDC_RE)
        ///
        /// @brief Stores the data pointer associated with the class instance.
        ///
        /// Data pointer which will point to the FFDC in the PIBMEM.
        ///
        uint32_t iv_data_ptr;
#endif
};

/// This implementation assumes no exception handling and leverages thread-local
/// storage. For platforms without thread support, a global variable will
/// suffice for the error state.


extern thread_local uint64_t pib_error_mask;     /// the pib mask
extern thread_local uint64_t operational_state;  /// the operational mode
#ifndef PLAT_NO_THREAD_LOCAL_STORAGE
extern thread_local ReturnCode current_err;      /// the current error state

#elif defined(NATIVE_THREAD_LOCAL_SUPPORT)

} // namspace fapi2

#include <nativethreadlocalreturncode.H>

namespace fapi2
{
extern NativeThreadLocalReturnCode current_err;

#else
extern ReturnCode current_err;
#endif

} // namspace fapi2

#endif
