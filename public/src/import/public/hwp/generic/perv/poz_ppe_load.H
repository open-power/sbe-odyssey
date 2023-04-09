/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_ppe_load.H $    */
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
/// @file poz_perv_ppe_load.H
/// @brief Load PPE images onto SBE/PME SRAM
///
/// *HWP HW Maintainer: Daniel Kiss <kiss@de.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: SPPE
///

#pragma once

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>
#include <poz_common_image_load.H>

// Constants

//------------------------------------------------------------------------------
// Structure definitions
//------------------------------------------------------------------------------
// Function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode(*poz_ppe_load_FP_t) (
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
    const uint8_t*,
    const uint32_t,
    const uint32_t,
    poz_image_type);

//------------------------------------------------------------------------------
// Auxiliary functions
//------------------------------------------------------------------------------
//
fapi2::ReturnCode poz_ioppe_load(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    const uint8_t* i_img_data,
    const uint32_t i_img_size,
    const uint32_t i_offset,
    poz_image_type i_type,
    const uint32_t i_ppe_base_address,
    const uint32_t i_ioppe_ram_offset,
    const uint32_t i_ioppe_ram_repeat_size,
    const uint32_t i_ioppe_ram_repeat_num);

fapi2::ReturnCode poz_ppe_stop(
    const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
    const uint32_t i_ppe_base_address);

fapi2::ReturnCode poz_ppe_start(
    const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
    const uint32_t i_ppe_base_address);

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
//
extern "C" {

///
/// @brief Load PPE images to SBE/PME SRAM
///
/// @param[in]  i_target          Reference to chip target
/// @param[in]  i_img_data        Pointer to image data
/// @param[in]  i_img_size        The size (in bytes) of the data
/// @param[in]  i_offset          Load offset
/// @param[in]  i_type            Type of image to load (base or memregs)
///
/// @return FAPI_RC_SUCCESS if success, else error code
///
    fapi2::ReturnCode poz_ppe_load(
        const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
        const uint8_t* i_img_data,
        const uint32_t i_img_size,
        const uint32_t i_offset,
        poz_image_type i_type);

} //extern "C"
