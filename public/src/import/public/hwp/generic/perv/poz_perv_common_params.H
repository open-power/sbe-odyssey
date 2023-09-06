/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_common_params.H $ */
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
/// @file  poz_perv_common_params.H
/// @brief Common parameters used by pervasive module code
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once


inline
fapi2::ReturnCode mod_multicast_setup_plat_remap(const uint8_t i_group_id,
        uint8_t& o_group_id)
{
    o_group_id = i_group_id;
    return fapi2::current_err;
}

namespace pll
{
enum pll_lock_bits
{
    ALL_PLLS            = 0xFF,

    P11T_PERV_DPLLNEST  = 0x20,

    P11S_PERV_PLLCLKSW1 = 0x80,
    P11S_PERV_PLLCLKSW2 = 0x40,
    P11S_PERV_FPLL1     = 0x20,
    P11S_PERV_FPLL2     = 0x10,
    P11S_PERV_FPLL3     = 0x08,
    P11S_PERV_FPLL4     = 0x04,
    P11S_PERV_PLLNEST   = 0x02,

    ODY_PERV_PLLMC      = 0x02,

    ZME_PERV_PLLCLKSW0  = 0x80,
    ZME_PERV_PLLCLKSW1  = 0x40,
    ZME_PERV_PLLXBFLT   = 0x20,
    ZME_PERV_PLLABFLT   = 0x10,
    ZME_PERV_PLLMCFLT   = 0x08,
    ZME_PERV_PLLNEST    = 0x04,

    ZME_PERV_PLLPCIFLT  = 0x80,

    PZ_PERV_PLLCLKSWA   = 0x80,
    PZ_PERV_PLLCLKSWB   = 0x40,
};
};

namespace cc
{
constexpr uint16_t REGION(const int number)
{
    return static_cast<uint16_t>(0x8000u >> number);
};

enum clock_region
{

    // Regions shared across all chiplets
    REGION_PERV = REGION(0),
    REGION_PLL  = REGION(13),   // Filter/Tank PLLs; DPLLs have their own separate regions

    // Regions shared across chips
    REGION_PERV_SBE = REGION(1),
    REGION_PERV_PIB = REGION(2),

    // Common combined regions
    REGION_ALL  = 0xFFFE,
    REGION_ALL_BUT_PLL = REGION_ALL & ~REGION_PLL,
    REGION_ALL_BUT_PERV_PLL = REGION_ALL & ~REGION_PERV & ~REGION_PLL,
    REGION_ALL_BUT_PERV_SBE = REGION_ALL & ~REGION_PERV_SBE,

    // P11-Tap regions
    P11T_PERV_PERV = REGION(0),
    P11T_PERV_SBE = REGION(1),
    P11T_PERV_PIB = REGION(2),
    P11T_PERV_NET = REGION(4),
    P11T_PERV_DPLLNEST = REGION(8),

    P11T_N0_PERV = REGION(0),
    P11T_N0_FBC = REGION(1),

    P11T_TBUS_PERV = REGION(0),
    P11T_TBUS_TB = REGION(1),
    P11T_TBUS_TCC = REGION(2),
    P11T_TBUS_IOPPE = REGION(12),
    P11T_TBUS_PLLTB = REGION(13),

    P11T_EQ_PERV = REGION(0),
    P11T_EQ_ECL20 = REGION(1),
    P11T_EQ_ECL21 = REGION(2),
    P11T_EQ_ECL22 = REGION(3),
    P11T_EQ_ECL23 = REGION(4),
    P11T_EQ_L30 = REGION(5),
    P11T_EQ_L31 = REGION(6),
    P11T_EQ_L32 = REGION(7),
    P11T_EQ_L33 = REGION(8),
    P11T_EQ_QME = REGION(9),
    P11T_EQ_CLKADJ = REGION(10),
    P11T_EQ_MMA0 = REGION(11),
    P11T_EQ_MMA1 = REGION(12),
    P11T_EQ_MMA2 = REGION(13),
    P11T_EQ_MMA3 = REGION(14),

    // P11-Spinal regions
    P11S_PERV_PERV = REGION(0),
    P11S_PERV_SBE = REGION(1),
    P11S_PERV_PIB = REGION(2),
    P11S_PERV_OCC = REGION(3),
    P11S_PERV_NET = REGION(4),
    P11S_PERV_CLKADJ = REGION(9),
    P11S_PERV_PLLPERV = REGION(13),

    P11S_N0_PERV = REGION(0),
    P11S_N0_ESQ0 = REGION(1),
    P11S_N0_ESC = REGION(2),
    P11S_N0_ESQ1 = REGION(3),
    P11S_N0_PAU00 = REGION(4),
    P11S_N0_PAU01 = REGION(5),
    P11S_N0_PAU02 = REGION(6),
    P11S_N0_PAU03 = REGION(7),
    P11S_N0_NX00 = REGION(8),
    P11S_N0_PCS0 = REGION(9),
    P11S_N0_PCS1 = REGION(10),

    P11S_N1_PERV = REGION(0),
    P11S_N1_ESQ0 = REGION(1),
    P11S_N1_ESC = REGION(2),
    P11S_N1_ESQ1 = REGION(3),
    P11S_N1_PAU00 = REGION(4),
    P11S_N1_PAU01 = REGION(5),
    P11S_N1_PAU02 = REGION(6),
    P11S_N1_PAU03 = REGION(7),
    P11S_N1_NX00 = REGION(8),
    P11S_N1_PCS0 = REGION(9),
    P11S_N1_PCS1 = REGION(10),

    P11S_N2_PERV = REGION(0),
    P11S_N2_PB = REGION(1),
    P11S_N2_SFBC = REGION(3),
    P11S_N2_INT0 = REGION(4),
    P11S_N2_INT1 = REGION(5),

    P11S_TBUS_PERV = REGION(0),
    P11S_TBUS_FBC = REGION(3),
    P11S_TBUS_IOT0 = REGION(4),
    P11S_TBUS_SF0 = REGION(5),
    P11S_TBUS_IOT1 = REGION(6),
    P11S_TBUS_SF1 = REGION(7),
    P11S_TBUS_IOPPE = REGION(12),
    P11S_TBUS_PLLIOT = REGION(13),

    P11S_PCI_PERV = REGION(0),
    P11S_PCI_PCI0 = REGION(4),
    P11S_PCI_PCI1 = REGION(5),
    P11S_PCI_PCI2 = REGION(6),
    P11S_PCI_PCISTG = REGION(7),
    P11S_PCI_PLLPCI = REGION(13),

    P11S_MC_PERV = REGION(0),
    P11S_MC_OMI0 = REGION(1),
    P11S_MC_DL = REGION(3),
    P11S_MC_MC0 = REGION(4),
    P11S_MC_MC1 = REGION(5),
    P11S_MC_IOPPE = REGION(12),
    P11S_MC_PLLMC = REGION(13),

    P11S_PAXO_PERV = REGION(0),
    P11S_PAXO_IOO = REGION(1),
    P11S_PAXO_PCS = REGION(2),
    P11S_PAXO_DL = REGION(3),
    P11S_PAXO_IOPPE = REGION(12),
    P11S_PAXO_PLLIOO = REGION(13),

    // Odyssey regions
    ODY_PERV_PERV = REGION(0),
    ODY_PERV_SBE = REGION(1),
    ODY_PERV_PIB = REGION(2),
    ODY_PERV_NET = REGION(3),
    ODY_PERV_PLLMC = REGION(13),

    ODY_MC_PERV = REGION(0),
    ODY_MC_IOO = REGION(1),
    ODY_MC_CORE = REGION(2),
    ODY_MC_CFG = REGION(3),
    ODY_MC_DFI = REGION(4),
    ODY_MC_PUB0 = REGION(5),
    ODY_MC_PUB1 = REGION(6),
    ODY_MC_PRIM0 = REGION(13),
    ODY_MC_PRIM1 = REGION(14),

    // zMetis regions
    ZME_PERV_PERV = REGION(0),
    ZME_PERV_SBE = REGION(1),
    ZME_PERV_PIB = REGION(2),
    ZME_PERV_NET = REGION(3),
    ZME_PERV_PLLPERV = REGION(13),

    ZME_NEST_PERV = REGION(0),
    ZME_NEST_NEST = REGION(1),
    ZME_NEST_PBUN0 = REGION(2),
    ZME_NEST_PBUN1 = REGION(3),
    ZME_NEST_MCN = REGION(4),
    ZME_NEST_FBC = REGION(5),
    ZME_NEST_MBUS = REGION(6),

    ZME_XBUS_PERV = REGION(0),
    ZME_XBUS_XBUS = REGION(1),
    ZME_XBUS_IOXB = REGION(2),
    ZME_XBUS_IOPPE = REGION(3),
    ZME_XBUS_PLLXBUS = REGION(13),

    ZME_PCIE_PERV = REGION(0),
    ZME_PCIE_PBCP = REGION(1),
    ZME_PCIE_IOPCI = REGION(2),
    ZME_PCIE_IOPPE = REGION(3),
    ZME_PCIE_ETU0 = REGION(4),
    ZME_PCIE_ETU1 = REGION(5),
    ZME_PCIE_ICB0 = REGION(6),
    ZME_PCIE_ICB1 = REGION(7),
    ZME_PCIE_PCIEX0 = REGION(8),
    ZME_PCIE_PCIEX1 = REGION(9),
    ZME_PCIE_PLLPCIE = REGION(13),

    ZME_MCS_PERV = REGION(0),
    ZME_MCS_XBUS = REGION(1),
    ZME_MCS_OMI = REGION(2),
    ZME_MCS_IOPPE = REGION(3),
    ZME_MCS_PLLMCS = REGION(13),

    ZME_ABUS_PERV = REGION(0),
    ZME_ABUS_ABUS = REGION(1),
    ZME_ABUS_IOAB = REGION(2),
    ZME_ABUS_IOPPE = REGION(3),
    ZME_ABUS_PLLABUS = REGION(13),

    ZME_EX_PERV = REGION(0),
    ZME_EX_L3 = REGION(1),
    ZME_EX_CORE = REGION(2),
    ZME_EX_SHADOW = REGION(3),

};

enum clock_type
{
    CLOCK_TYPE_SL  = 0x8,
    CLOCK_TYPE_NSL = 0x4,
    CLOCK_TYPE_ARY = 0x2,
    CLOCK_TYPE_ALL = 0xE,
};

enum scan_type
{
    SCAN_TYPE_FUNC = 0x800 >> 0,
    SCAN_TYPE_MODE = 0x800 >> 1,
    SCAN_TYPE_GPTR = 0x800 >> 2,
    SCAN_TYPE_REGF = 0x800 >> 3,
    SCAN_TYPE_LBST = 0x800 >> 4,
    SCAN_TYPE_ABST = 0x800 >> 5,
    SCAN_TYPE_REPR = 0x800 >> 6,
    SCAN_TYPE_TIME = 0x800 >> 7,
    SCAN_TYPE_BNDY = 0x800 >> 8,
    SCAN_TYPE_FARR = 0x800 >> 9,
    SCAN_TYPE_CMSK = 0x800 >> 10,
    SCAN_TYPE_INEX = 0x800 >> 11,
    SCAN_TYPE_ALL  = 0xFFF,
    SCAN_TYPE_RTG  = 0x230,  // repr+time+gptr
    SCAN_TYPE_NOT_RTG = 0xDCF,
    SCAN_TYPE_NOT_REPR = 0xFDF,
};
};

namespace mux
{
enum mux_type : int
{
    FSI2PCB = 16, ///< Direct connection between FSI2PIB engine and TP chiplet, PIB+PCB bypassed
    PIB2PCB = 18, ///< Direct connection between PIB and TP chiplet, PCB bypassed
    PCB2PCB = 19, ///< Normal operation, all chiplet requests go through PCB
    I2C2PCB = 24, ///< Odyssey only, direct connection between I2C responder and TP chiplet, LBUS+PIB+PCB bypassed
};
};

namespace pib
{
enum pib_controller : uint8_t
{
    PC_NONE    = 0,
    PC_GSD2PIB = 1,
    PC_FSI2PIB = 2,
    PC_I2CR    = 3,
    PC_TOD     = 4,
    PC_XSCOM   = 5,
    PC_GPE0    = 5,
    PC_GPE1    = 6,
    PC_GPE2    = 7,
    PC_GPE3    = 8,
    PC_ADU     = 9,
    PC_MMIO    = 9,
    PC_PME     = 12,
    PC_SPPE    = 13,
    PC_SBE     = 14,
};

enum pib_responder : uint8_t
{
    PR_NONE    = 0,
    PR_OTPROM  = 1,
    PR_FSIC0   = 2,
    PR_FSICTAP = 3,
    PR_I2CR    = 3,
    PR_TOD     = 4,
    PR_MBOX    = 5,
    PR_OCB     = 6,
    PR_SPIC    = 7,
    PR_PSU     = 8,
    PR_ADU     = 9,
    PR_MMIO    = 9,
    PR_I2CC    = 10,
    PR_SBEFIFO = 11,
    PR_SPIC0   = 12,
    PR_PME     = 12,
    PR_SPPE    = 13,
    PR_SBE     = 14,
    PR_PCBC    = 15,
};
};

/* Chiplet delays measured in PCB network levels relative to the lowest level chiplet */
const uint8_t p11s_chiplet_delay_table[64] =
{
    /* None, TP, None, None, N0, N1, N2, None */
    0, 0, 0, 0, 0, 0, 0, 0,
    /* 4x MC, 4x None */
    1, 1, 1, 1, 0, 0, 0, 0,
    /* 12x PAXO, 4x None */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0,
    /* 6x PCI, 2x None */
    2, 2, 2, 2, 3, 3, 0, 0,
    /* 4x TBUS, 4x None */
    3, 3, 3, 3, 0, 0, 0, 0,
};

const uint8_t p11t_chiplet_delay_table[64] =
{
    0, 0, 0, 1, /* None, TP, N0, TBUS */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 2,       /* EQs */
};

/* Odyssey is super boring since it only has two chiplets and both are at the same level */
const uint8_t ody_chiplet_delay_table[64] = { 0 };


/* zmetis placeholder */
const uint8_t zme_chiplet_delay_table[64] = { 0 };

/// @brief Describes a hang pulse configuration
struct hang_pulse_t
{
    uint16_t id            : 4; ///< Hang pulse number to program
    uint16_t value         : 8; ///< Frequency value for this hang pulse
    uint16_t stop_on_xstop : 1; ///< Hang pulse should stop when a checkstop comes in
    uint16_t               : 2;
    uint16_t last          : 1; ///< Set to 1 for the last entry in an array of this struct
};

/// @brief Describes a constant hang pulse configuration
struct constant_hang_pulse_t
{
    uint16_t pre_divider   : 8; ///< Pre-divider for this hang pulse
    uint16_t value         : 6; ///< Frequency value for this hang pulse
    uint16_t stop_on_xstop : 1; ///< Hang pulse should stop when a checkstop comes in
};
