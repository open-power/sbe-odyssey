/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_bist.H $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
/// @file  poz_bist.H
/// @brief Common module for ABIST/LBIST
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

static const uint16_t BIST_PARAMS_CURRENT_VERSION = 5;

struct bist_params
{
    uint16_t BIST_PARAMS_VERSION;
    char program[16];                    ///< Name of program for image selection
    char ring_patch[16];                 ///< Name of patch ring image
    uint64_t chiplets;                   ///< Mask of chiplets to test

    /// Mask of chiplets to configure for control mode and issue unicast OPCG_GO to.
    /// Chiplets not in the mask but in the chiplets mask will be configured for receiver mode.
    /// Since ABIST doesn't use control/recevier, this will just be the unicast OPCG_GO mechanic.
    /// To do a simple unicast OPCG_GO, this variable should be equal to chiplets mask.
    /// To instead use multicast OPCG_GO, this mask should be assigned to 0x0.
    uint64_t uc_go_chiplets;

    /// Option flags
    enum bist_flags : uint32_t
    {
        ABIST_NOT_LBIST   = 0x80000000,
        DO_SCAN0          = 0x40000000,
        DO_ARRAYINIT      = 0x20000000,
        DO_RING_SETUP     = 0x10000000,
        DO_RING_PATCH     = 0x08000000,
        DO_REG_SETUP      = 0x04000000,
        DO_GO             = 0x02000000,
        DO_POLL           = 0x01000000,
        DO_REG_CLEANUP    = 0x00800000,
        DO_COMPARE        = 0x00400000,
        SKIP_FIRST_CLOCK  = 0x00200000,
        SKIP_LAST_CLOCK   = 0x00100000,
        POLL_ABIST_DONE   = 0x00080000,  ///< Poll on ABIST_DONE in addition to OPCG_DONE and BIST_HALT
        ASSERT_ABIST_DONE = 0x00040000,  ///< Assert ABIST_DONE at end of polling
    } flags;

    uint64_t opcg_count;           ///< Amount of total RUNN cycles to clock (>0x7FFFFFFFFFF for infinite)
    uint64_t idle_count;           ///< Base idle RUNN cycles before BIST starts on any chiplet
    uint64_t linear_stagger;       ///< Increase idle count by this much for each successive chiplet
    uint64_t zigzag_stagger;       ///< Increase idle count by this much on any odd chiplet
    uint32_t max_polls;            ///< Max amount of DONE / HALT polls to issue (>0x7FFFFFFF for infinite)
    uint32_t poll_delay_hw;        ///< Amount of delay (in terms of nS) per polling loop for hardware environment
    uint32_t poll_delay_sim;       ///< Amount of delay (in terms of number of cycles) per polling loop simulation environment
    uint16_t scan0_types;          ///< Mask of ring scan types to dictate what we will scan0

    /// First, multicast write base_regions as clock / BIST regions to everything in chiplets.
    /// Next, for each chiplet in chiplets, inspect the entry in chiplets_regions indexed by chiplet ID.
    /// If the entry is zero, pass; else, overwrite base_regions with the entry for that respective chiplet.
    uint16_t base_regions;
    uint16_t chiplets_regions[64];

    uint16_t outer_loop_mask;      ///< Mask of outer loop BIST iterations to run
    uint16_t inner_loop_mask;      ///< Mask of inner loop BIST iterations to run

    // final_idle_count = idle_count + (linear_stagger * target_number) + (zigzag_stagger * (target_number & 1));

    // no burnin type, that is handled by providing different setup and check image names
    // no DTS mode, that shall be handled by outside code

    // TODO: Add LBIST parameters
};

constexpr bist_params::bist_flags operator|(const bist_params::bist_flags a, const bist_params::bist_flags b)
{
    return static_cast<bist_params::bist_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

typedef fapi2::ReturnCode (*poz_bist_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&, const bist_params&);

/// @brief Common module for ABIST/LBIST
///
/// @param[in]   i_target   Chip to operate on
/// @param[in]   i_params   Parameter block to select operation type, mode, parameters...
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_bist(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target, const bist_params& i_params);
}
