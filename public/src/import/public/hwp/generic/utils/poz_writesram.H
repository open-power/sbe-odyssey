/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_writesram.H $  */
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
///
/// @file poz_writesram.H
/// @brief Write data to chip SRAM
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: HB, Cronus, SBE
///

#pragma once

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
///
/// @brief Write SRAM
///
/// @param[in]  i_target        Reference to ocmb, compute, tbusc, iohs, or mc target
/// @param[in]  i_controlReg    SCOM control reg address
/// @param[in]  i_dataRegAddr   SCOM data register address
/// @param[in]  i_bytes         Number of bytes to write
/// @param[out] i_data          Input data data
///
/// @return FAPI_RC_SUCCESS if success, else error code
///
fapi2::ReturnCode poz_writesram(const fapi2::Target <
                                fapi2::TARGET_TYPE_OCMB_CHIP |
                                fapi2::TARGET_TYPE_COMPUTE_CHIP |
                                fapi2::TARGET_TYPE_TBUSC |
                                fapi2::TARGET_TYPE_IOHS |
                                fapi2::TARGET_TYPE_MC |
                                fapi2::TARGET_TYPE_PERV |
                                fapi2::TARGET_TYPE_MULTICAST > & i_target,
                                const uint32_t i_controlReg,
                                const uint32_t i_dataReg,
                                const uint32_t i_bytes,
                                const uint8_t* i_data);
