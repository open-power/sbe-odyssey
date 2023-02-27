/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/ody_getsram.H $   */
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
///
/// @file ody_getsram.H
/// @brief Read data from Odyssey SRAM
///
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
// Structure definitions
//------------------------------------------------------------------------------
// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode(*ody_getsram_FP_t) (
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>&,
    const uint64_t,
    const uint32_t,
    uint8_t*);

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
//

extern "C" {

///
/// @brief Read data from Odyssey SRAM
///
/// @param[in]  i_target        Reference to proc Odyssey target
/// @param[in]  i_offset        SRAM offset to read from
/// @param[in]  i_bytes         Size of data to read (in bytes)
/// @param[out] o_data          Pointer to data storage
///
/// @return FAPI_RC_SUCCESS if success, else error code
///
    fapi2::ReturnCode ody_getsram(
        const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint64_t i_offset,
        const uint32_t i_bytes,
        uint8_t* o_data);

} //extern "C"
