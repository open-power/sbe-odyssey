/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/ring/generic/poz_putRingUtils.H $    */
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
//------------------------------------------------------------------------------
/// @file   poz_putRingUtils.H
/// @brief  Given a chiplet target, scan address and RS4 image, scan the image
///         contents into the ring(s) targeted by the target & scan address
//------------------------------------------------------------------------------
// *HWP HW Maintainer:    Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer:    Prem S Jha <premjha2@in.ibm.com>
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <ffdc.H>

// Constants and Structures
//
struct restoreOpcgRegisters
{
    fapi2::buffer<uint64_t> l_opcgAlign;
    fapi2::buffer<uint64_t> l_opcgReg0;
    fapi2::buffer<uint64_t> l_opcgReg1;
    fapi2::buffer<uint64_t> l_opcgReg2;
    fapi2::buffer<uint64_t> l_opcgCapt1;
    fapi2::buffer<uint64_t> l_opcgCapt2;
    fapi2::buffer<uint64_t> l_opcgCapt3;
};

typedef fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST, fapi2::MULTICAST_AND > ScanTarget;

typedef union  __attribute__((__packed__)) rs4_data
{
    uint32_t iv_data;
    struct
    {
        uint8_t val[4];
    } fields;
} rs4_data_t;

extern "C"
{

    typedef fapi2::ReturnCode( *poz_putRingUtils_FP_t )(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
        const uint8_t* i_rs4,
        const uint32_t i_modifiedRingAddress,
        const fapi2::RingMode i_ringMode);

/// @brief This applies a single scan image to a given target.
/// @param[in] i_target    Target to scan into. May be multicast!
/// @param[in] i_scanImage Scan image to apply
/// @param[in] i_modifiedRingAddress
///                        Ring address modified to indicate the actual
///                        scan regions targeted by this call
/// @param[in] i_ringMode  Ring operation mode.
/// @return FAPI2_RC_SUCCESS on success, else error code.
    fapi2::ReturnCode poz_putRingUtils(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
        const uint8_t* i_rs4,
        const uint32_t i_modifiedRingAddress,
        const fapi2::RingMode i_ringMode);

/// @brief Function to reader the header data from the ring and verify it.
/// @param[in] i_target         Chiplet Target of Scan
/// @param[in] i_scanRegion     64 bit scan region type value
/// @param[in] i_bitsDecoded    number of bits rotated and scanned
/// @return FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode verifyHeader(
        const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST,
        fapi2::MULTICAST_COMPARE > & i_target_comp,
        uint64_t       i_scanRegion,
        const uint32_t i_bitsDecoded);

/// @brief Function to setup scan region and type
/// @param[in] i_target         Chiplet Target of Scan
/// @param[in] i_ringMode       different ring access operation mode
/// @param[in] i_scanRegion     64 bit scan region type value
/// @param[in] o_OPCGData       Structures data as per OpcgRegisters
/// @return FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode setupClockController(
        const ScanTarget& i_target,
        const fapi2::RingMode i_ringMode,
        const uint64_t i_scanRegion,
        struct restoreOpcgRegisters& o_OPCGData);

/// @brief Function to clean up the clock controller scan region
/// @param[in] i_target         Chiplet Target of Scan
/// @param[in] i_ringMode       different ring access operation mode
/// @param[in] i_scanRegion     64 bit scan region type value
/// @param[in] o_OPCGData       Structures data as per OpcgRegisters
/// @return FAPI2_RC_SUCCESS if success, else error code.
    fapi2::ReturnCode cleanupClockController(
        const ScanTarget& i_target,
        const fapi2::RingMode i_ringMode,
        const uint64_t i_scanRegion,
        const struct restoreOpcgRegisters& i_OPCGData);

/// @brief Function to decode scan region data
/// @param[in] i_target         Chiplet Target of Scan
/// @param[in] i_ringAddress    Ring address
/// @param[in] i_ringMode       different ring access operation mode
/// @return uint64 ScanRegionData data for caller
    uint64_t decodeScanRegionData(
        const ScanTarget& i_target,
        const uint32_t i_ringAddress,
        const fapi2::RingMode i_ringMode);

} //extern "C"
