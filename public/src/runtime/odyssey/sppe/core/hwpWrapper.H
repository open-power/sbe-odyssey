/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwpWrapper.H $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
#ifndef __HWPWRAPPER_H_
#define __HWPWRAPPER_H_

#include "sbecmnhwpwrapper.H"
#include "plat_hwp_data_stream.H"
#include "sbestates.H"

using namespace fapi2;

//----------------------------------------------------------------------------
using  sbeIstepHwpOcmb_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target);

using  sbeHwpLoadPIE_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target,
                      fapi2::hwp_data_istream&, fapi2::hwp_data_istream&,
                      fapi2::hwp_data_istream&, fapi2::hwp_data_istream&);

// MEM HWPs
using  sbeHwpMEMLoadbin_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target,
                       uint8_t* const i_bin_data,
                       const uint32_t i_bin_size,
                       const uint32_t i_bin_offset);

// IO HWPs
using  sbeHwpIOLoadbin_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target,
                       uint8_t* const i_bin_data,
                       const uint32_t i_bin_size,
                       const uint32_t i_bin_offset,
                       uint8_t i_ioppeImage);

using  sbeHwpDraminit_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target,
                       fapi2::hwp_array_ostream  & ostream);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepWithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepLoadPIEwithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepBistWithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepAbistWithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepLbistWithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepLoadIMEMwithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepLoadDMEMwithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepLoadIOPPEwithOcmb( voidfuncptr_t i_hwp);

/**
 * @brief Wrapper to call the HWP
 *
 * @param[in] i_hwp Function pointer to HWP
 *
 * @return FAPI2_RC_SUCCESS on success
 */
ReturnCode istepDraminitWithOcmb( voidfuncptr_t i_hwp);

#endif
