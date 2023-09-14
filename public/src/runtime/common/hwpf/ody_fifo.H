/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/ody_fifo.H $                   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

fapi2::ReturnCode ody_isetp(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, uint8_t i_class, uint8_t hwp_num);
uint32_t streamFifo(uint8_t *i_pArg);
uint32_t streamFifoReply(uint8_t *i_pArg);

fapi2::ReturnCode push_data_to_ody_eot(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target, const uint32_t *i_data, size_t word_count);
fapi2::ReturnCode ody_isetp_reply(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);
