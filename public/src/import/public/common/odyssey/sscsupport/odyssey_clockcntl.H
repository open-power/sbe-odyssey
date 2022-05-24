/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_clockcntl.H $ */
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
/// @file  odyssey_clockcntl.H
/// @brief Clock control procedure used by Cronus, used in the product
///        code and Flex, also user defined dll
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Thi Tran (thi@us.ibm.com)
// *HWP FW Maintainer   : n/a
//------------------------------------------------------------------------------
#ifndef ODYSSEY_CLOCKCNTL_H
#define ODYSSEY_CLOCKCNTL_H

#include <iostream>
#include <vector>
#include <stdint.h>
#include <list>
#include <ClockDomains.H>
#include <OdysseyClockDomains.H>
#include <odyssey_cu.H>
#include <odyssey_scom_addr.H>

extern "C" {

    /// Constants
    const uint32_t NUM_CHIPLET_IDS      =    9;  // Num of chiplet IDs
    const uint8_t  ALL_GOOD_ID          = 0xFF;  // RingID that marks default clock domains

    // #################################################################
    // Chiplet vital domain array
    // Each entry defines the Vital clock domain name for the Chiplet ID index
    // For non-existing Chiplet ID, entry is ODYSSEY_FAKE_DOMAIN
    // #################################################################
    const CLOCK_DOMAIN ChipletVitalDomainTable[NUM_CHIPLET_IDS] =
    {
        ODYSSEY_STANDBY_DOMAIN,           // Chiplet 0, Standby/CFAM
        ODYSSEY_TPVITL_DOMAIN,            // Chiplet 1, TP
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 2, invalid
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 3, invalid
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 4, invalid
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 5, invalid
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 6, invalid
        ODYSSEY_FAKE_DOMAIN,              // Chiplet 7, invalid
        ODYSSEY_MEMVITL_DOMAIN,           // Chiplet 8, MC
    };

    // #################################################################
    // Chiplet SCOM ring clock domain table
    // Clock domain look up table for all chiplets
    // ALL_GOOD_ID is the 'common' region for the chiplet if specific
    // region is not found.
    // #################################################################
    struct ChipletRingClockDomain_t
    {
        const uint8_t       chipletId; // ChipletId
        const uint8_t       ringId;    // RingId
        const CLOCK_DOMAIN  domain;    // Domain
    };

    const ChipletRingClockDomain_t ChipletRingClockDomainTable[] =
    {
        //      Chiplet            RingID                 Clock domain
        //      -------        --------------       -----------------------------
        {  PIB_CHIPLET_ID,     ALL_GOOD_ID,         ODYSSEY_STANDBY_DOMAIN            }, // Standby/CFAM

        {  PERV_CHIPLET_ID,    PSCOM_RING_ID,       ODYSSEY_TPVITL_DOMAIN             }, // TP
        {  PERV_CHIPLET_ID,    ALL_GOOD_ID,         ODYSSEY_TPSBEPIBNET_DOMAIN        }, // TP

        {  MEM_CHIPLET_ID,     ALL_GOOD_ID,         ODYSSEY_MEMNONVITL_DOMAIN         }, // MEM
    };

    // #################################################################
    // Clock domain string/enum/address/mask table
    // Look up table for Clock domain enum to determine:
    //     - Clock enum string
    //     - Clock enum access address
    //     - Clock enum mask value.
    // #################################################################
    struct ClockDomainStrAddrMask_t
    {
        const char*          domainStr;        // Clock domain string
        const CLOCK_DOMAIN   domainEnum;       // Clock domain enum
        const uint64_t       clockAddr;        // Clock address
        const uint64_t       clockMask;        // Clock mask
        const uint8_t        inMulticastGroup; // Affected by multicast group [0-7]
    };

    const ClockDomainStrAddrMask_t ClockDomainStrAddrMaskTable[] =
    {
        { "FAKE",                 ODYSSEY_FAKE_DOMAIN,               0x0000281A, 0x0000800000000000ULL, 0x00 }, // vitl thold

        { "STANDBY",              ODYSSEY_STANDBY_DOMAIN,            0x0000281A, 0x0000800000000000ULL, 0x00 },

        { "TPVITL",               ODYSSEY_TPVITL_DOMAIN,             0x0000281A, 0x0000800000000000ULL, 0x00 }, // TP vitl thold
        { "TPSBEPIBNET",          ODYSSEY_TPSBEPIBNET_DOMAIN,        0x01030008, 0x0F00000000000000ULL, 0x81 }, // TP perv/sbe/pib/net regions
        { "TPPLLMC",              ODYSSEY_TPPLLMC_DOMAIN,            0x01030008, 0x0002000000000000ULL, 0x81 }, // TP pllmc region

        { "MEMVITL",              ODYSSEY_MEMVITL_DOMAIN,            0x080F0040, 0x0000800000000000ULL, 0x00 }, // MC vitl thold
        { "MEMNONVITL",           ODYSSEY_MEMNONVITL_DOMAIN,         0x08030008, 0x0FE0600000000000ULL, 0xC5 }, // MC perv/io/core/cfg/dfi/pub/prim regions
    };

    /// @brief Determines the CLOCK_DOMAIN for a given scom address
    /// @param[in]  i_odysseyCU Enumeration of the chip unit type
    /// @param[in]  i_chipUnitNum Instance number of the chip unit
    /// @param[in]  i_scomAddr addr to determine the CLOCK_DOMAIN
    /// @param[out] o_domain clock domain for the i_scomAddr
    /// @param[in]  i_mode future expansion, default to 0
    /// @retval uint32_t non-zero value for any error
    uint32_t odyssey_clockcntl_getScomClockDomain(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_scomAddr,
            CLOCK_DOMAIN& o_domain,
            const uint32_t i_mode = 0);

    /// @brief Using the rings address adjust the clock domain relative to the chipUnit,
    ///        non-chipUnit domains would not modify io_domain
    /// @param[in]      i_odysseyCU Enumeration of the chip unit type
    /// @param[in]      i_chipUnitNum Instance number of the chip unit
    /// @param[in/out]  io_domain clock domain for the i_scanAddr,
    ///                 normally the zeroth addr
    /// @param[in]      i_mode future expansion, default to 0
    /// @retval uint32_t non-zero value for any error
    uint32_t odyssey_clockcntl_getScanClockDomain(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            CLOCK_DOMAIN& io_domain,
            const uint32_t i_mode = 0);

    /// @brief Gets the data that the platform will need to check the clock domain.
    ///        The platforms will take this data and do a getcfam or getscom compare
    ///        it to the mask, then adjust the platforms clockstatus
    /// @param[in]  i_domain Enumeration of the clock domain to check
    /// @param[out] o_clockAddr address to check, could be cfam or scom
    /// @param[out] o_clockMask mask to compagre the result of checking the above addr
    /// @param[out] o_mode 0 = cfam, 1 = scom, plus future expansion
    /// @retval uint32_t non-zero value for any error
    uint32_t odyssey_clockcntl_getClockCheckData(const CLOCK_DOMAIN i_domain,
            uint64_t& o_clockAddr,
            uint64_t& o_clockMask,
            uint32_t& o_mode );

    /// @brief Used to determine if a command would alter a clock domain,
    ///        if so then return a list of domains it would touch.
    ///        The platform will take this list and reset the cached clock state
    ///        for these domains.
    /// @param[in]  i_odysseyCU Enumeration of the chip unit type
    /// @param[in]  i_chipUnitNum Instance number of the chip unit
    /// @param[in]  i_address address that can alter a clock comain
    /// @param[out] o_domain List of clock domains that need to be reset (status changed)
    /// @param[in]  i_mode future expansion, default to 0
    /// @retval uint32_t non-zero value for any error
    uint32_t odyssey_clockcntl_clockStateRegisterScreen(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_address,
            std::list<CLOCK_DOMAIN>& o_domainList,
            const uint32_t i_mode = 0);

    /// @brief Get the string equivalent of the enumerated clock domain.
    /// @param[in] i_domain Enumeration of the clock domain
    /// @retval string value of the clock domain
    std::string odyssey_clockcntl_convertClockDomainEnum(const CLOCK_DOMAIN i_domain);

    /// @brief Get the enumerated  equivalent of the string clock domain.
    /// @param[in] i_domain string value of the clock domain
    /// @retval enumerated value of the clock domain
    CLOCK_DOMAIN odyssey_clockcntl_convertClockDomainString(const std::string i_domain);

}

#endif /* ODYSSEY_CLOCKCNTL_H */
