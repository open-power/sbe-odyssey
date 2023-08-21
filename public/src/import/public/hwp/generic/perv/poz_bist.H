/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_bist.H $        */
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
/// @file  poz_bist.H
/// @brief Common module for ABIST/LBIST
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <hwp_data_stream.H>
#include <poz_tlv_utils.H>

static const uint16_t BIST_PARAMS_CURRENT_VERSION = 11;
static const uint16_t BIST_CHIPOP_ID = 0xF101;

struct bist_params
{
    ////////////////////////////////////////////////////////////////
    // Special constituents
    ////////////////////////////////////////////////////////////////
    /// BIST_PARAMS_VERSION must never change size nor location
    uint16_t BIST_PARAMS_VERSION;
    // Stages of BIST to perform
    enum bist_stages : uint16_t
    {
        SCAN0          = 0x0200,
        ARRAYINIT      = 0x0100,
        RING_SETUP     = 0x0080,
        RING_PATCH     = 0x0040,
        REG_SETUP      = 0x0020,
        GO             = 0x0010,
        POLL           = 0x0008,
        REG_CLEANUP    = 0x0004,
        COMPARE        = 0x0002,
        UNLOAD         = 0x0001,
    } stages;
    /// Option flags
    enum bist_flags : uint32_t
    {
        /// Single bit flags
        ABIST_NOT_LBIST      = 0x80000000,
        FAST_DIAGNOSTICS     = 0x40000000,
        SKIP_FIRST_CLOCK     = 0x20000000,
        SKIP_LAST_CLOCK      = 0x10000000,
        POLL_ABIST_DONE      = 0x08000000,
        ASSERT_ABIST_DONE    = 0x04000000,
        CHIPLET_FENCE_ACTIVE = 0x02000000,
        LBIST_COMBINED       = 0x01000000,
        INT_MODE             = 0x00800000,
        SCAN0_ARY_FILL       = 0x00400000,

        /// Multi bit flags
        /// PCB mux switching
        PCB_MUX_MASK        = 0x00000003,
        PCB_MUX_UNCHANGED   = 0x00000000,
        PCB_MUX_FSI2PCB     = 0x00000001,
        PCB_MUX_PIB2PCB     = 0x00000002,
        PCB_MUX_PCB2PCB     = 0x00000003,
    } flags;

    ////////////////////////////////////////////////////////////////
    // 64-bit constituents
    ////////////////////////////////////////////////////////////////
    uint64_t chiplets;             ///< Mask of chiplets to test
    /// Mask of chiplets to configure for control mode and issue unicast OPCG_GO to.
    /// Chiplets not in the mask but in the chiplets mask will be configured for receiver mode.
    /// Since ABIST doesn't use control/recevier, this will just be the unicast OPCG_GO mechanic.
    /// To do a simple unicast OPCG_GO, this variable should be equal to chiplets mask.
    /// To instead use multicast OPCG_GO, this mask should be assigned to 0x0.
    uint64_t uc_go_chiplets;
    uint64_t opcg_count;           ///< Amount of total RUNN cycles to clock (>0x7FFFFFFFFFF for infinite)
    uint64_t idle_count;           ///< Base idle RUNN cycles before BIST starts on any chiplet
    uint64_t linear_stagger;       ///< Increase idle count by this much for each successive chiplet
    uint64_t zigzag_stagger;       ///< Increase idle count by this much on any odd chiplet
    uint64_t lbist_opcg_align;     ///< OPCG_ALIGN register
    uint64_t lbist_opcg1;          ///< LBIST setting of OPCG_REG1
    /// Used to divide opcg_count by either 2, 3, 4. 2-bits are needed to define the following modes:
    /// 00 - no override, 01 - divide value by 2, 10 - divide value by 3, 11 - divide value by 4
    /// Each chiplet can be overrided by setting their respective two bits in the mask.
    /// Format:
    /// Chiplet #:    0  1 ... 31        32 33 ... 63
    ///             [00|00|...|00]      [00|00|...|00]
    /// Entry #:          0                   1
    uint64_t opcg_count_adjust[2]; ///< Override opcg count cycles

    ////////////////////////////////////////////////////////////////
    // 32-bit constituents
    ////////////////////////////////////////////////////////////////
    uint32_t max_polls;            ///< Max amount of DONE / HALT polls to issue (>0x7FFFFFFF for infinite)
    uint32_t poll_delay_hw;        ///< Amount of delay (in ns) per polling loop on hardware
    uint32_t poll_delay_sim;       ///< Amount of delay (in simcycles) per polling loop in sim

    ////////////////////////////////////////////////////////////////
    // 16-bit constituents
    ////////////////////////////////////////////////////////////////
    uint16_t scan0_types;          ///< Mask of ring scan types to dictate what we will scan0
    uint16_t lbist_scan_types;     ///< Mask of ring scan types to dictate what we will lbist
    /// First, multicast write base_regions as clock / BIST regions to everything in chiplets.
    /// Next, for each chiplet in chiplets, inspect the entry in chiplets_regions indexed by chiplet ID.
    /// If the entry is zero, pass; else, overwrite base_regions with the entry for that respective chiplet.
    uint16_t base_regions;
    uint16_t padding;              ///< Padding bits to enforce data alignment within struct
    uint16_t chiplets_regions[64];
    uint16_t outer_loop_mask;      ///< Mask of outer loop BIST iterations to run
    uint16_t inner_loop_mask;      ///< Mask of inner loop BIST iterations to run

    ////////////////////////////////////////////////////////////////
    // 8-bit constituents
    ////////////////////////////////////////////////////////////////
    /// Make sure to keep these fields 8-byte aligned for FAPI_DBG prints
    char program[32];              ///< Name of program for image selection
    char ring_patch[32];           ///< Name of patch ring image

    // final_idle_count = idle_count + (linear_stagger * target_number) + (zigzag_stagger * (target_number & 1));

    // no burnin type, that is handled by providing different setup and check image names
    // no DTS mode, that shall be handled by outside code
};

constexpr bist_params::bist_stages operator~(const bist_params::bist_stages a)
{
    return static_cast<bist_params::bist_stages>(~static_cast<uint16_t>(a));
}

constexpr bist_params::bist_stages operator&(const bist_params::bist_stages a, const bist_params::bist_stages b)
{
    return static_cast<bist_params::bist_stages>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

constexpr bist_params::bist_stages operator|(const bist_params::bist_stages a, const bist_params::bist_stages b)
{
    return static_cast<bist_params::bist_stages>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

static inline bist_params::bist_stages& operator&=(bist_params::bist_stages& a, const bist_params::bist_stages b)
{
    a = a & b;
    return a;
}

static inline bist_params::bist_stages& operator|=(bist_params::bist_stages& a, const bist_params::bist_stages b)
{
    a = a | b;
    return a;
}

constexpr bist_params::bist_flags operator|(const bist_params::bist_flags a, const bist_params::bist_flags b)
{
    return static_cast<bist_params::bist_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

struct bist_diags
{
    uint64_t opcg_counts[64];
    uint16_t pass_counter;
    bist_params::bist_stages completed_stages;
};

typedef fapi2::ReturnCode (*poz_bist_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
        const bist_params&, bist_diags&, fapi2::hwp_data_ostream&, std::vector<compare_fail>&, const uint16_t);

/// @brief Common module for ABIST/LBIST
///
/// @param[in]   i_target               Chip to operate on
/// @param[in]   i_params               Parameter block to define runtime options
/// @param[out]  o_diags                Diagnostic return data
/// @param[out]  o_failing_rings        Vector of ring addresses which failed compare
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_bist(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                               const bist_params& i_params, bist_diags& o_diags, fapi2::hwp_data_ostream& o_stream,
                               std::vector<compare_fail>& o_failing_rings, const uint16_t i_dict_def = BIST_CHIPOP_ID);
}
