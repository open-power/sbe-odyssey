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
///
/// @file odyssey_clockcntl.H
/// @brief Clock control procedure used by Cronus, used in the product
///        code and Flex, also user defined dll
///
/// HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// HWP FW Maintainer:
/// HWP Consumed by: CRONUS
///

#ifndef ODYSSEY_CLOCKCNTL_H
#define ODYSSEY_CLOCKCNTL_H

#include <iostream>
#include <vector>
#include <stdint.h>
#include <list>
#include <ClockDomains.H>
#include <OdysseyClockDomains.H>
#include <odyssey_cu.H>

extern "C" {

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
