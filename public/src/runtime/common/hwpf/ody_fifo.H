/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/ody_fifo.H $                   */
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
//------------------------------------------------------------------------------
/// @file  ody_fifo.H

#pragma once

#include <fapi2.H>

///
/// @brief Read Host attribute/platform state to determine desired OCMB PLL bucket
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
/// @param[out]  o_pll_bucket              Target PLL bucket setting
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
//fapi2::ReturnCode push_data_to_ody(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, const uint32_t *data, size_t dataSize);
//fapi2::ReturnCode submit_eot_to_ody(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);

///
/// @brief start an IStep on an Odyssey, the function does not wait for completion, so user can kick off all 8 channels in parallel
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
/// @param[in]   i_class                   IStep class
/// @param[in]   i_hwp_num                 hwp number
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode ody_istep_start(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, uint8_t i_class, uint8_t hwp_num);

///
/// @brief wait for an IStep complete on an Odyssey,
///
/// @param[in]   i_target                  Reference to TARGET_TYPE_OCMB_CHIP
///
/// @return fapi::ReturnCode  FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode ody_istep_complete(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);

