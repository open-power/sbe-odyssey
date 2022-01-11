/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_clockcntl.C $ */
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
/// @file odyssey_clockcntl.C
/// @brief Clock control procedure used by Cronus, used in the product
///        code and Flex, also user defined dll.
///
/// HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// HWP FW Maintainer:
/// HWP Consumed by: CRONUS
///
#include <odyssey_clockcntl.H>
#include <odyssey_cu_utils.H>
#include <CronusData.H>   // Cronus won't have clock domains defined until it has ENGD.
#include <odyssey_scominfo.H> // For odyssey_scominfo_createChipUnitScomAddr call

#define ODYSSEY_CLOCKCNTL_C

extern "C"
{

// ####################################################################################
#ifndef ODYSSEY

    uint32_t odyssey_clockcntl_getScomClockDomain_INTERNAL(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_scomAddr,
            CLOCK_DOMAIN& o_domain,
            const uint32_t i_mode)
#else
    uint32_t odyssey_clockcntl_getScomClockDomain(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_scomAddr,
            CLOCK_DOMAIN& o_domain,
            const uint32_t i_mode)
#endif
    {
        uint32_t l_rc = 0;
        return l_rc;
    }

// ####################################################################################
// This is done for supporting DLL_OVERRIDE.
#ifndef ODYSSEY
    uint32_t odyssey_clockcntl_getScomClockDomain(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_scomAddr,
            CLOCK_DOMAIN& o_domain,
            const uint32_t i_mode)
    {
        return odyssey_clockcntl_getScomClockDomain_INTERNAL(i_odysseyCU,
                i_chipUnitNum,
                i_scomAddr,
                o_domain,
                i_mode);
    }
#endif

// ####################################################################################
#ifndef ODYSSEY
    std::string odyssey_clockcntl_convertClockDomainEnum_INTERNAL(CLOCK_DOMAIN i_domain)
#else
    std::string odyssey_clockcntl_convertClockDomainEnum(CLOCK_DOMAIN i_domain)
#endif
    {
        std::string l_ret = "ALL";
        return l_ret;
    }

// ####################################################################################
#ifndef ODYSSEY
    std::string odyssey_clockcntl_convertClockDomainEnum(CLOCK_DOMAIN i_domain)
    {
        return odyssey_clockcntl_convertClockDomainEnum_INTERNAL(i_domain);
    }
#endif

// ####################################################################################
#ifndef ODYSSEY
    CLOCK_DOMAIN odyssey_clockcntl_convertClockDomainString_INTERNAL(std::string i_domain)
#else
    CLOCK_DOMAIN odyssey_clockcntl_convertClockDomainString(std::string i_domain)
#endif
    {
        CLOCK_DOMAIN l_ret = CLOCK_DOMAIN_INVALID;
        return l_ret;
    }

// ####################################################################################
#ifndef ODYSSEY
    CLOCK_DOMAIN odyssey_clockcntl_convertClockDomainString(std::string i_domain)
    {
        return odyssey_clockcntl_convertClockDomainString_INTERNAL(i_domain);
    }
#endif

// ####################################################################################
    uint32_t odyssey_clockcntl_getScanClockDomain(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            CLOCK_DOMAIN& io_domain,
            const uint32_t i_mode)
    {
        uint32_t l_rc = 0;
        return l_rc;
    }


// ####################################################################################
    uint32_t odyssey_clockcntl_getClockCheckData(const CLOCK_DOMAIN i_domain,
            uint64_t& o_clockAddr,
            uint64_t& o_clockMask,
            uint32_t& o_mode )
    {
        uint32_t l_rc = 0;
        return l_rc;
    }


// ####################################################################################
    uint32_t odyssey_clockcntl_clockStateRegisterScreen(const odysseyChipUnits_t i_odysseyCU,
            const uint8_t i_chipUnitNum,
            const uint64_t i_address,
            std::list<CLOCK_DOMAIN>& o_domainList,
            const uint32_t i_mode)
    {
        uint32_t l_rc = 0;
        return l_rc;
    }

} // extern "C"

#undef ODYSSEY_CLOCKCNTL_C
