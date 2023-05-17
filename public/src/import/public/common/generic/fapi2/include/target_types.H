/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/target_types.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2023                        */
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
/**
 * @file target_types.H
 * @brief definitions for fapi2 target types
 */

#ifndef __FAPI2_TARGET_TYPES__
#define __FAPI2_TARGET_TYPES__
#include <stdint.h>
#include <base_utils.H>

/// FAPI namespace
namespace fapi2
{
///
/// @enum fapi::TargetType
/// @brief Types, kinds, of targets
/// @note TYPE_NONE is used to represent empty/NULL targets in lists
/// or tables. TYPE_ALL is used to pass targets to methods which
/// can act generally on any type of target
///

// Since the basic target types are one-hot bit masks let's define them
// by their log2 and then derive the bit masks by shifting.
enum LogTargetType : uint8_t
{
    LOG_TARGET_TYPE_SYSTEM              =  0, ///< System type
    LOG_TARGET_TYPE_DIMM                =  1, ///< DIMM type
    LOG_TARGET_TYPE_PROC_CHIP           =  2, ///< Processor type
    LOG_TARGET_TYPE_MEMBUF_CHIP         =  3, ///< Membuf type
    LOG_TARGET_TYPE_EX                  =  4, ///< EX - 2x Core, L2, L3 - can be deconfigured
    LOG_TARGET_TYPE_MBA                 =  5, ///< MBA type
    LOG_TARGET_TYPE_MCS                 =  6, ///< MCS type
    LOG_TARGET_TYPE_XBUS                =  7, ///< XBUS type
    LOG_TARGET_TYPE_ABUS                =  8, ///< ABUS type
    LOG_TARGET_TYPE_L4                  =  9, ///< L4 type
    LOG_TARGET_TYPE_CORE                = 10, ///< Core
    LOG_TARGET_TYPE_EQ                  = 11, ///< EQ - 4x core, 2x L2, 2x L3 - can be deconfigured
    LOG_TARGET_TYPE_MCA                 = 12, ///< MCA type
    LOG_TARGET_TYPE_MCBIST              = 13, ///< MCBIST type
    LOG_TARGET_TYPE_MI                  = 14, ///< MI Memory Interface (Cumulus)
    LOG_TARGET_TYPE_CAPP                = 15, ///< CAPP target
    LOG_TARGET_TYPE_DMI                 = 16, ///< DMI type
    LOG_TARGET_TYPE_OBUS                = 17, ///< OBUS type
    LOG_TARGET_TYPE_OBUS_BRICK          = 18, ///< OBUS BRICK type
    LOG_TARGET_TYPE_SBE                 = 19, ///< SBE type
    LOG_TARGET_TYPE_PPE                 = 20, ///< PPE type
    LOG_TARGET_TYPE_PERV                = 21, ///< Pervasive type
    LOG_TARGET_TYPE_PEC                 = 22, ///< PEC type
    LOG_TARGET_TYPE_PHB                 = 23, ///< PHB type
    LOG_TARGET_TYPE_MC                  = 24, ///< MC type
    LOG_TARGET_TYPE_OMI                 = 25, ///< OMI type
    LOG_TARGET_TYPE_OMIC                = 26, ///< OMIC type
    LOG_TARGET_TYPE_MCC                 = 27, ///< MCC type
    LOG_TARGET_TYPE_OCMB_CHIP           = 28, ///< OCMB type
    LOG_TARGET_TYPE_MEM_PORT            = 29, ///< MEM_PORT type
    LOG_TARGET_TYPE_NMMU                = 30, ///< NEST MMU type
    LOG_TARGET_TYPE_RESERVED            = 31, ///< Reserved for Cronus (Z)
    LOG_TARGET_TYPE_PAU                 = 32, ///< PAU type
    LOG_TARGET_TYPE_IOHS                = 33, ///< IOHS type
    LOG_TARGET_TYPE_FC                  = 34, ///< Fused Core type
    LOG_TARGET_TYPE_PMIC                = 35, ///< PMIC type
    LOG_TARGET_TYPE_PAUC                = 36, ///< PAUC type
    LOG_TARGET_TYPE_GENERICI2CRESPONDER = 37, ///< GENERICI2CRESPONDER type
    LOG_TARGET_TYPE_IOLINK              = 38, ///< IOLINK type
    LOG_TARGET_TYPE_MDS_CTLR            = 39, ///< MDS Media Controller type
    LOG_TARGET_TYPE_COMPUTE_CHIP        = 40, ///< Compute chip type
    LOG_TARGET_TYPE_HUB_CHIP            = 41, ///< Hub chip type
    LOG_TARGET_TYPE_TBUSC               = 42, ///< TBUS chiplet type
    LOG_TARGET_TYPE_TBUSL               = 43, ///< TBUS link type
    LOG_TARGET_TYPE_L3CACHE             = 44, ///< L3CACHE type
    LOG_TARGET_TYPE_INT                 = 45, ///< INT type
    LOG_TARGET_TYPE_NX                  = 46, ///< NX type
    LOG_TARGET_TYPE_PAX                 = 47, ///< PAX type
    LOG_TARGET_TYPE_PAXO                = 48, ///< PAXO type
    LOG_TARGET_TYPE_SMPLINK             = 59, ///< SMPLINK type
    LOG_TARGET_TYPE_PEC6P               = 50, ///< PEC6P type
    LOG_TARGET_TYPE_PEC2P               = 51, ///< PEC2P type
    LOG_TARGET_TYPE_PHB248X             = 52, ///< PHB248X type
    LOG_TARGET_TYPE_PHB16X              = 53, ///< PHB16X type
    LOG_TARGET_TYPE_POWER_IC            = 54, ///< POWER_IC type
    LOG_TARGET_TYPE_TEMP_SENSOR         = 55, ///< TEMP_SENSOR type
    LOG_TARGET_TYPE_XBUSL               = 56, ///< XBUS Link
    LOG_TARGET_TYPE_SHADOW              = 57, ///< Shadow core
    LOG_TARGET_TYPE_MULTICAST           = 63, ///< MULTICAST type
};

/// Target Kind
enum TargetType : uint64_t
{
    TARGET_TYPE_NONE                = 0x0000000000000000, ///< No type

    TARGET_TYPE_SYSTEM              = 1ULL << LOG_TARGET_TYPE_SYSTEM,              ///< System type
    TARGET_TYPE_DIMM                = 1ULL << LOG_TARGET_TYPE_DIMM,                ///< DIMM type
    TARGET_TYPE_PROC_CHIP           = 1ULL << LOG_TARGET_TYPE_PROC_CHIP,           ///< Processor type
    TARGET_TYPE_MEMBUF_CHIP         = 1ULL << LOG_TARGET_TYPE_MEMBUF_CHIP,         ///< Membuf type
    TARGET_TYPE_EX                  = 1ULL << LOG_TARGET_TYPE_EX,                  ///< EX - 2x Core, L2, L3 - can be deconfigured
    TARGET_TYPE_MBA                 = 1ULL << LOG_TARGET_TYPE_MBA,                 ///< MBA type
    TARGET_TYPE_MCS                 = 1ULL << LOG_TARGET_TYPE_MCS,                 ///< MCS type
    TARGET_TYPE_XBUS                = 1ULL << LOG_TARGET_TYPE_XBUS,                ///< XBUS type
    TARGET_TYPE_ABUS                = 1ULL << LOG_TARGET_TYPE_ABUS,                ///< ABUS type
    TARGET_TYPE_L4                  = 1ULL << LOG_TARGET_TYPE_L4,                  ///< L4 type
    TARGET_TYPE_CORE                = 1ULL << LOG_TARGET_TYPE_CORE,                ///< Core
    TARGET_TYPE_EQ                  = 1ULL << LOG_TARGET_TYPE_EQ,                  ///< EQ - 4x core, 2x L2, 2x L3 - can be deconfigured
    TARGET_TYPE_MCA                 = 1ULL << LOG_TARGET_TYPE_MCA,                 ///< MCA type
    TARGET_TYPE_MCBIST              = 1ULL << LOG_TARGET_TYPE_MCBIST,              ///< MCBIST type
    TARGET_TYPE_MI                  = 1ULL << LOG_TARGET_TYPE_MI,                  ///< MI Memory Interface (Cumulus)
    TARGET_TYPE_CAPP                = 1ULL << LOG_TARGET_TYPE_CAPP,                ///< CAPP target
    TARGET_TYPE_DMI                 = 1ULL << LOG_TARGET_TYPE_DMI,                 ///< DMI type
    TARGET_TYPE_OBUS                = 1ULL << LOG_TARGET_TYPE_OBUS,                ///< OBUS type
    TARGET_TYPE_OBUS_BRICK          = 1ULL << LOG_TARGET_TYPE_OBUS_BRICK,          ///< OBUS BRICK type
    TARGET_TYPE_SBE                 = 1ULL << LOG_TARGET_TYPE_SBE,                 ///< SBE type
    TARGET_TYPE_PPE                 = 1ULL << LOG_TARGET_TYPE_PPE,                 ///< PPE type
    TARGET_TYPE_PERV                = 1ULL << LOG_TARGET_TYPE_PERV,                ///< Pervasive type
    TARGET_TYPE_PEC                 = 1ULL << LOG_TARGET_TYPE_PEC,                 ///< PEC type
    TARGET_TYPE_PHB                 = 1ULL << LOG_TARGET_TYPE_PHB,                 ///< PHB type
    TARGET_TYPE_MC                  = 1ULL << LOG_TARGET_TYPE_MC,                  ///< MC type
    TARGET_TYPE_OMI                 = 1ULL << LOG_TARGET_TYPE_OMI,                 ///< OMI type
    TARGET_TYPE_OMIC                = 1ULL << LOG_TARGET_TYPE_OMIC,                ///< OMIC type
    TARGET_TYPE_MCC                 = 1ULL << LOG_TARGET_TYPE_MCC,                 ///< MCC type
    TARGET_TYPE_OCMB_CHIP           = 1ULL << LOG_TARGET_TYPE_OCMB_CHIP,           ///< OCMB type
    TARGET_TYPE_MEM_PORT            = 1ULL << LOG_TARGET_TYPE_MEM_PORT,            ///< MEM_PORT type
    TARGET_TYPE_NMMU                = 1ULL << LOG_TARGET_TYPE_NMMU,                ///< NEST MMU type
    TARGET_TYPE_RESERVED            = 1ULL << LOG_TARGET_TYPE_RESERVED,            ///< Reserved for Cronus (Z)
    TARGET_TYPE_PAU                 = 1ULL << LOG_TARGET_TYPE_PAU,                 ///< PAU type
    TARGET_TYPE_IOHS                = 1ULL << LOG_TARGET_TYPE_IOHS,                ///< IOHS type
    TARGET_TYPE_FC                  = 1ULL << LOG_TARGET_TYPE_FC,                  ///< Fused Core type
    TARGET_TYPE_PMIC                = 1ULL << LOG_TARGET_TYPE_PMIC,                ///< PMIC type
    TARGET_TYPE_PAUC                = 1ULL << LOG_TARGET_TYPE_PAUC,                ///< PAUC type
    TARGET_TYPE_GENERICI2CRESPONDER = 1ULL << LOG_TARGET_TYPE_GENERICI2CRESPONDER, ///< GENERICI2CRESPONDER type
    TARGET_TYPE_IOLINK              = 1ULL << LOG_TARGET_TYPE_IOLINK,              ///< IOLINK type
    TARGET_TYPE_MDS_CTLR            = 1ULL << LOG_TARGET_TYPE_MDS_CTLR,            ///< MDS Media Controller type
    TARGET_TYPE_COMPUTE_CHIP        = 1ULL << LOG_TARGET_TYPE_COMPUTE_CHIP,        ///< Compute chip type
    TARGET_TYPE_HUB_CHIP            = 1ULL << LOG_TARGET_TYPE_HUB_CHIP,            ///< Hub chip type
    TARGET_TYPE_TBUSC               = 1ULL << LOG_TARGET_TYPE_TBUSC,               ///< TBUS chiplet type
    TARGET_TYPE_TBUSL               = 1ULL << LOG_TARGET_TYPE_TBUSL,               ///< TBUS link type
    TARGET_TYPE_L3CACHE             = 1ULL << LOG_TARGET_TYPE_L3CACHE,             ///< L3CACHE type
    TARGET_TYPE_INT                 = 1ULL << LOG_TARGET_TYPE_INT,                 ///< INT type
    TARGET_TYPE_NX                  = 1ULL << LOG_TARGET_TYPE_NX,                  ///< NX type
    TARGET_TYPE_PAX                 = 1ULL << LOG_TARGET_TYPE_PAX,                 ///< PAX type
    TARGET_TYPE_PAXO                = 1ULL << LOG_TARGET_TYPE_PAXO,                ///< PAXO type
    TARGET_TYPE_SMPLINK             = 1ULL << LOG_TARGET_TYPE_SMPLINK,             ///< SMPLINK type
    TARGET_TYPE_PEC6P               = 1ULL << LOG_TARGET_TYPE_PEC6P,               ///< PEC6P type
    TARGET_TYPE_PEC2P               = 1ULL << LOG_TARGET_TYPE_PEC2P,               ///< PEC2P type
    TARGET_TYPE_PHB248X             = 1ULL << LOG_TARGET_TYPE_PHB248X,             ///< PHB248X type
    TARGET_TYPE_PHB16X              = 1ULL << LOG_TARGET_TYPE_PHB16X,              ///< PHB16X type
    TARGET_TYPE_POWER_IC            = 1ULL << LOG_TARGET_TYPE_POWER_IC,            ///< POWER_IC type
    TARGET_TYPE_TEMP_SENSOR         = 1ULL << LOG_TARGET_TYPE_TEMP_SENSOR,         ///< TEMP_SENSOR type
    TARGET_TYPE_XBUSL               = 1ULL << LOG_TARGET_TYPE_XBUSL,               ///< XBUS Link
    TARGET_TYPE_SHADOW              = 1ULL << LOG_TARGET_TYPE_SHADOW,              ///< Shadow core
    TARGET_TYPE_MULTICAST           = 1ULL << LOG_TARGET_TYPE_MULTICAST,           ///< MULTICAST type

    TARGET_TYPE_ALL                 = 0x7FFFFFFFFFFFFFFF, ///< Any/All types
    TARGET_TYPE_ALL_MC              = 0xFFFFFFFFFFFFFFFF, ///< Any/All types + Multicast

    // Compound target types
    TARGET_TYPE_CHIPS         = TARGET_TYPE_PROC_CHIP |
                                TARGET_TYPE_MEMBUF_CHIP |
                                TARGET_TYPE_OCMB_CHIP |
                                TARGET_TYPE_COMPUTE_CHIP |
                                TARGET_TYPE_HUB_CHIP,

    // Types that correspond to a chiplet or a chiplet subunit
    TARGET_TYPE_CHIPLETS      = TARGET_TYPE_EX            |
                                TARGET_TYPE_SHADOW        |
                                TARGET_TYPE_MBA           |
                                TARGET_TYPE_MCS           |
                                TARGET_TYPE_XBUS          |
                                TARGET_TYPE_XBUSL         |
                                TARGET_TYPE_ABUS          |
                                TARGET_TYPE_L4            |
                                TARGET_TYPE_CORE          |
                                TARGET_TYPE_EQ            |
                                TARGET_TYPE_MCA           |
                                TARGET_TYPE_MCBIST        |
                                TARGET_TYPE_MI            |
                                TARGET_TYPE_DMI           |
                                TARGET_TYPE_OBUS          |
                                TARGET_TYPE_OBUS_BRICK    |
                                TARGET_TYPE_SBE           |
                                TARGET_TYPE_PPE           |
                                TARGET_TYPE_PERV          |
                                TARGET_TYPE_PEC           |
                                TARGET_TYPE_PHB           |
                                TARGET_TYPE_MC            |
                                TARGET_TYPE_OMI           |
                                TARGET_TYPE_OMIC          |
                                TARGET_TYPE_MCC           |
                                TARGET_TYPE_MEM_PORT      |
                                TARGET_TYPE_NMMU          |
                                TARGET_TYPE_PAU           |
                                TARGET_TYPE_IOHS          |
                                TARGET_TYPE_FC            |
                                TARGET_TYPE_PAUC          |
                                TARGET_TYPE_IOLINK        |
                                TARGET_TYPE_TBUSC         |
                                TARGET_TYPE_TBUSL         |
                                TARGET_TYPE_L3CACHE       |
                                TARGET_TYPE_INT           |
                                TARGET_TYPE_NX            |
                                TARGET_TYPE_PAX           |
                                TARGET_TYPE_PAXO          |
                                TARGET_TYPE_SMPLINK       |
                                TARGET_TYPE_PEC6P         |
                                TARGET_TYPE_PEC2P         |
                                TARGET_TYPE_PHB248X       |
                                TARGET_TYPE_PHB16X,

    TARGET_TYPE_MULTICASTABLE = TARGET_TYPE_CORE        |
                                TARGET_TYPE_EQ          |
                                TARGET_TYPE_EX          |
                                TARGET_TYPE_SHADOW      |
                                TARGET_TYPE_IOHS        |
                                TARGET_TYPE_MC          |
                                TARGET_TYPE_PAUC        |
                                TARGET_TYPE_PEC         |
                                TARGET_TYPE_PERV        |
                                TARGET_TYPE_ABUS        |
                                TARGET_TYPE_XBUS        |
                                TARGET_TYPE_XBUSL       |
                                TARGET_TYPE_TBUSC       |
                                TARGET_TYPE_TBUSL       |
                                TARGET_TYPE_NX          |
                                TARGET_TYPE_PAU         |
                                TARGET_TYPE_L3CACHE     |
                                TARGET_TYPE_PAX         |
                                TARGET_TYPE_PAXO        |
                                TARGET_TYPE_PEC6P       |
                                TARGET_TYPE_PEC2P       |
                                TARGET_TYPE_PHB16X,

    TARGET_TYPE_MULTICAST_CHIP = TARGET_TYPE_PROC_CHIP  |
                                 TARGET_TYPE_OCMB_CHIP   |
                                 TARGET_TYPE_HUB_CHIP   |
                                 TARGET_TYPE_COMPUTE_CHIP,

    TARGET_TYPE_ANY_POZ_CHIP   = TARGET_TYPE_PROC_CHIP  |
                                 TARGET_TYPE_OCMB_CHIP  |
                                 TARGET_TYPE_HUB_CHIP   |
                                 TARGET_TYPE_COMPUTE_CHIP,

    // Mappings to target types found in the error xml files
    TARGET_TYPE_EX_CHIPLET    = TARGET_TYPE_EX,
    TARGET_TYPE_MBA_CHIPLET   = TARGET_TYPE_MBA,
    TARGET_TYPE_MCS_CHIPLET   = TARGET_TYPE_MCS,
    TARGET_TYPE_XBUS_ENDPOINT = TARGET_TYPE_XBUS,
    TARGET_TYPE_ABUS_ENDPOINT = TARGET_TYPE_ABUS,
};

///
/// @brief Forward declaration of the TargetFilter enum type
///
/// The actual enum values will be declared in target_filters.H
///
enum TargetFilter : uint64_t;

// attributeOverride tool requires x86.nfp compile
#ifndef CONTEXT_x86_nfp
/// @cond
    constexpr TargetType operator|(TargetType x, TargetType y)
{
    return static_cast<TargetType>(static_cast<uint64_t>(x) |
                                   static_cast<uint64_t>(y));
}

constexpr TargetType operator&(TargetType x, TargetType y)
{
    return static_cast<TargetType>(static_cast<uint64_t>(x) &
                                   static_cast<uint64_t>(y));
}

constexpr TargetType operator~(TargetType x)
{
    return static_cast<TargetType>(~static_cast<uint64_t>(x));
}
/// @endcond
#endif

}

#endif
