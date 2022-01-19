/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_common_params.H $ */
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
#pragma once

namespace pll
{
enum pll_lock_bits
{
    ALL_PLLS            = 0xFF,

    P11T_PERV_DPLLNEST  = 0x80,

    P11S_PERV_PLLCLKSW1 = 0x80,
    P11S_PERV_PLLCLKSW2 = 0x40,
    P11S_PERV_FPLL1     = 0x20,
    P11S_PERV_FPLL2     = 0x10,
    P11S_PERV_FPLL3     = 0x08,
    P11S_PERV_FPLL4     = 0x04,
    P11S_PERV_PLLNEST   = 0x02,

    ODY_PERV_PLLMC      = 0x02,
};
};

namespace cc
{
constexpr uint16_t REGION(const int number)
{
    return 0x8000 >> number;
};

enum clock_region
{
    REGION_PERV = REGION(0),
    REGION_PLL  = REGION(12),

    REGION_ALL  = 0xFFFE,
    REGION_ALL_BUT_PLL = REGION_ALL & ~REGION_PLL,
    REGION_ALL_BUT_PERV_PLL = REGION_ALL & ~REGION_PERV & ~REGION_PLL,

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
    SCAN_TYPE_GTR  = 0x230,  // gptr+time+repr
    SCAN_TYPE_NOT_GTR = 0xDCF,
};
};

namespace mux
{
enum mux_type : int
{
    FSI2PCB = 16,
    PIB2PCB = 18,
    PCB2PCB = 19,
};
};


enum chiplet_type : uint8_t
{
    NO_CHIPLET = 0xFF,

    P11S_CT_PERV = 0,
    P11S_CT_N0,
    P11S_CT_N1,
    P11S_CT_N2,
    P11S_CT_TBUS,
    P11S_CT_PCI,
    P11S_CT_MC,
    P11S_CT_PAXO,
    P11S_CT_COUNT,

    P11T_CT_PERV = 0,
    P11T_CT_NEST,
    P11T_CT_TBUS,
    P11T_CT_EQ,
    P11T_CT_COUNT,
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
    2, 3, 4,    /* EQs */
};

/* Map from chiplet ID to chiplet type */
const chiplet_type p11s_chiplets[64] =
{
    /* 0x00..0x07 */
    NO_CHIPLET, P11S_CT_PERV, NO_CHIPLET, NO_CHIPLET, P11S_CT_N0, P11S_CT_N1, P11S_CT_N2, NO_CHIPLET,
    /* 0x08..0x0F */
    P11S_CT_MC, P11S_CT_MC, P11S_CT_MC, P11S_CT_MC, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x10..0x17 */
    P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO,
    /* 0x18..0x1F */
    P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, P11S_CT_PAXO, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x20..0x27 */
    P11S_CT_PCI, P11S_CT_PCI, P11S_CT_PCI, P11S_CT_PCI, P11S_CT_PCI, P11S_CT_PCI, NO_CHIPLET, NO_CHIPLET,
    /* 0x28..0x2F */
    P11S_CT_TBUS, P11S_CT_TBUS, P11S_CT_TBUS, P11S_CT_TBUS, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x30..0x37 */
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x38..0x2F */
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
};

const chiplet_type p11t_chiplets[64] =
{
    /* 0x00..0x0F */
    NO_CHIPLET, P11T_CT_PERV, P11T_CT_NEST, P11T_CT_TBUS, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x10..0x1F */
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x20..0x2F */
    P11T_CT_EQ, P11T_CT_EQ, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    /* 0x30..0x3F */
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
    NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET, NO_CHIPLET,
};

#define P11S_SUPPORT 1
#define P11T_SUPPORT 1
#define ODY_SUPPORT 0
#define ZME_SUPPORT 0

#if 0
ReturnCode _get_chiplet_type(
    Target<TARGET_TYPE_PERV>& i_target,
    ATTR_NAME_Type& o_chip_type,
    chiplet_type& o_chiplet_type)
{
    auto l_chip = i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>();
    FAPI_TRY(FAPI_ATTR_GET(ATTR_NAME, l_chip, o_chip_type));

    if (P11S_SUPPORT && (o_chip_type == ENUM_ATTR_NAME_P11S))
    {
        o_chiplet_type = p11s_chiplets[i_target.getChipletNumber()];
    }
    else if (P11T_SUPPORT && (o_chip_type == ENUM_ATTR_NAME_P11T))
    {
        o_chiplet_type = p11t_chiplets[i_target.getChipletNumber()];
    }
    else if (ODY_SUPPORT && (o_chip_type == ENUM_ATTR_NAME_ODYSSEY))
    {
        o_chiplet_type = ody_chiplets[i_target.getChipletNumber()];
    }
    else if (ZME_SUPPORT && (o_chip_type == ENUM_ATTR_NAME_ZMETIS))
    {
        o_chiplet_type = zme_chiplets[i_target.getChipletNumber()];
    }
    else
    {
        ASSERT(chip type not supported);
    }

    FAPI_ASSERT(o_chiplet_type != NO_CHIPLET, invalid chiplet target);

fapi_try_exit:
    return current_err;
}

static inline ReturnCode get_hang_pulse_config(Target<TARGET_TYPE_PERV> i_target, hang_pulse_t*& o_config)
{

}

template <typename T>
static inline ReturnCode p11s_get_chiplet_traits(Target<TARGET_TYPE_PERV> i_target, T* i_traits, T*& o_trait)
{
    STATIC_ASSERT(false);
}

template <typename T>
static inline const T& p11t_get_chiplet_traits(Target<TARGET_TYPE_PERV> i_target, T* i_traits)
{
    return get_chiplet_traits(i_target, p11t_chiplets, i_traits);
}
#endif

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

#if 0
const hang_pulse_t p11s_hangpulse_setup[P11S_CT_COUNT] =
{
    /* P11S_CT_PERV */ p11s_perv_hangpulses,
    /* P11S_CT_N0   */ p11s_n0_hangpulses,
    /* P11S_CT_N1   */ p11s_n1_hangpulses,
    /* P11S_CT_N2   */ p11s_n2_hangpulses,
    /* P11S_CT_TBUS */ p11s_tbus_hangpulses,
    /* P11S_CT_PCI  */ p11s_pci_hangpulses,
    /* P11S_CT_MC   */ p11s_mc_hangpulses,
    /* P11S_CT_PAXO */ p11s_paxo_hangpulses,
};

const hang_pulse_t p11t_hangpulse_setup[P11T_CT_COUNT] =
{
    /* P11S_CT_PERV */ p11t_perv_hangpulses,
    /* P11S_CT_NEST */ p11t_nest_hangpulses,
    /* P11S_CT_TBUS */ p11t_tbus_hangpulses,
    /* P11S_CT_EQ   */ p11t_eq_hangpulses,
};
#endif

struct scan_traits
{
    uint8_t chip_targeted  : 1; ///< Ring operations target the chip vs. the chiplet
    uint8_t eq_chiplet     : 1; ///< Use EQ chiplet targeting rules
};

const scan_traits p11s_scan_traits[P11S_CT_COUNT] =
{
    //                   chip_targeted
    //                   |      eq_chiplet
    //                   |      |
    /* P11S_CT_PERV */ { true,  false },
    /* P11S_CT_N0   */ { true,  false },
    /* P11S_CT_N1   */ { true,  false },
    /* P11S_CT_N2   */ { true,  false },
    /* P11S_CT_TBUS */ { false, false },
    /* P11S_CT_PCI  */ { false, false },
    /* P11S_CT_MC   */ { false, false },
    /* P11S_CT_PAXO */ { false, false },
};

const scan_traits p11t_scan_traits[P11T_CT_COUNT] =
{
    //                   chip_targeted
    //                   |      eq_chiplet
    //                   |      |
    /* P11S_CT_PERV */ { true,  false },
    /* P11S_CT_NEST */ { true,  false },
    /* P11S_CT_TBUS */ { false, false },
    /* P11S_CT_EQ   */ { false, true  },
};

struct bist_params
{
    char setup_image[16];                   ///< Name of setup ring image
    char compare_image[16];                 ///< Name of compare ring image
    uint64_t chiplets;                      ///< Mask of chiplets to test

    /// Option flags
    enum bist_flags : uint32_t
    {
        ABIST_NOT_LBIST   = 0x80000000,
        DO_PRE_SCAN0_GTR  = 0x40000000,
        DO_PRE_SCAN0_REST = 0x20000000,
        DO_SETUP          = 0x10000000,
        DO_RUN            = 0x08000000,
        DO_COMPARE        = 0x04000000,
        DO_CLEANUP        = 0x02000000,
        DO_SCAN0_GTR      = 0x01000000,
        DO_SCAN0_REST     = 0x00800000,
        DO_ARRAYINIT      = 0x00400000,
        SKIP_FIRST_CLOCK  = 0x00200000,
        SKIP_LAST_CLOCK   = 0x00100000,
        UNICAST_GO        = 0x00080000,  ///< Trigger OPCG_GO via unicast as opposed to multicast
        POLL_ABIST_DONE   = 0x00040000,  ///< Poll on ABIST_DONE in addition to OPCG_DONE and external abort scratch register
    } flags;

    uint32_t opcg_count;                    ///< Amount of total RUNN cycles to clock - 0 means infinite / until ABIST_DONE
    uint32_t idle_count;                    ///< Base idle RUNN cycles before BIST starts on any chiplet
    uint32_t timeout;                       ///< Polling timeout in microseconds; not guaranteed to be exact!
    uint16_t linear_stagger;                ///< Increase idle count by this much for each successive chiplet
    uint16_t zigzag_stagger;                ///< Increase idle count by this much on any odd chiplet
    uint16_t regions;                       ///< Mask of clock regions to test


    //    final_idle_count = idle_count + (linear_stagger * target_number) + (zigzag_stagger * (target_number & 1));

    //  no burnin type, that is handled by providing different setup and check image names
    //  no DTS mode, that shall be handled by outside code

    // TODO: Add LBIST parameters
};

constexpr bist_params::bist_flags operator|(const bist_params::bist_flags a, const bist_params::bist_flags b)
{
    return static_cast<bist_params::bist_flags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}
