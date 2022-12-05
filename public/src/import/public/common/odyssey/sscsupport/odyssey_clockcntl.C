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
//------------------------------------------------------------------------------
/// @file  odyssey_clockcntl.C
/// @brief Clock control procedure used by Cronus, used in the product
///        code and Flex, also user defined dll.
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Thi Tran (thi@us.ibm.com)
// *HWP FW Maintainer   : n/a
//------------------------------------------------------------------------------

#include <odyssey_clockcntl.H>
#include <odyssey_cu_utils.H>
#include <CronusData.H>
#include <odyssey_scom_addr.H>
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
        uint64_t l_xlatedAddr = 0;
        uint32_t l_EcLevel = 0;
        uint8_t l_index = 0;

        // Print for debug
        std::string l_chipUnitStr;
        l_rc = odyssey_convertCUEnum_to_String(i_odysseyCU, l_chipUnitStr);

        if (l_rc)
        {
            return l_rc;
        }

        dbg_print("\nEntering odyssey_clockcntl_getScomClockDomain\n");
        dbg_print(" Chip unit type : %s\n", l_chipUnitStr.c_str());
        dbg_print(" Chip unit num  : %d\n", i_chipUnitNum);
        dbg_print(" SCOM addr      : 0x%08lx_%08llx\n", (i_scomAddr >> 32), (i_scomAddr & 0xFFFFFFFFULL));
        dbg_print(" Mode           : %d\n", i_mode);

        do
        {
            // Default to bad domain
            o_domain = ODYSSEY_FAKE_DOMAIN;
            // Validate i_chipUnitNum value
            l_rc = odyssey_validateChipUnitNum(i_chipUnitNum, i_odysseyCU);

            if (l_rc)
            {
                break;
            }

            // Convert input address to that of the i_chipUnitNum instance
            // in order to get the correct chiplet Id, ringId, etc...
            l_xlatedAddr = odyssey_scominfo_createChipUnitScomAddr(i_odysseyCU,
                           l_EcLevel,
                           i_chipUnitNum,
                           i_scomAddr,
                           (odysseyTranslationMode_t) i_mode);

            if (l_xlatedAddr == FAILED_TRANSLATION)
            {
                l_rc = 1;
                break;
            }

            // Get address fields
            odyssey_scom_addr l_addr(l_xlatedAddr);
            uint8_t l_chipletId = l_addr.getChipletId();
            uint8_t l_ringId = l_addr.getRingId();
            uint8_t l_endPoint = l_addr.getEndpoint();

            // Debug info
            dbg_print("\nDetermine Clock domain for addr 0x%08lx_%08llx\n", (l_xlatedAddr >> 32), (l_xlatedAddr & 0xFFFFFFFFULL));
            dbg_print("ChipletId 0x%.2X\n", l_chipletId);
            dbg_print("RingId   0x%.8X\n", l_ringId);
            dbg_print("EndPoint 0x%.8X\n", l_endPoint);

            // Determine if address targets TP_VITL_DOMAIN
            if ( l_addr.isMulticast() ) // Multicast addr
            {
                o_domain = ODYSSEY_TPVITL_DOMAIN;
                break;
            }
            else if (l_endPoint == PCBSLV_ENDPOINT) // PCB slave endpoint
            {
                if ( (i_chipUnitNum != 0) || (i_odysseyCU != ODYSSEY_PERV_CHIPUNIT) )
                {
                    // Not a chip or PIB chiplet
                    if ( !( (i_odysseyCU == ODYSSEY_NO_CU && l_chipletId == 0) ) )
                    {
                        o_domain = ODYSSEY_TPVITL_DOMAIN;
                        break;
                    }
                }
            }

            // ----------------------------------------------------------------------------------
            // For all chiplets, if endpoint is Chiplet control or Clock control, domain is Vital
            // ----------------------------------------------------------------------------------
            if ( (l_endPoint == CHIPLET_CTRL_ENDPOINT) || // 0x0
                 (l_endPoint == CLOCK_CTRL_ENDPOINT) )    // 0x3
            {
                dbg_print("\nChipletVitlDomainTable look-up: ChipletId 0x%.2X\n", l_chipletId);

                o_domain = ODYSSEY_FAKE_DOMAIN;

                // Find matching chiplet ID in vitl domain table
                for (l_index = 0;
                     l_index < ( sizeof(ChipletVitlDomainTable) / sizeof(ChipletVitlDomain_t) );
                     ++l_index)
                {
                    if ( (ChipletVitlDomainTable[l_index].chipletId == l_chipletId) )
                    {
                        o_domain = ChipletVitlDomainTable[l_index].domain;
                        break;
                    }
                }

                // Check for invalid chiplet ID that results in bad domain
                if (o_domain == ODYSSEY_FAKE_DOMAIN)
                {
                    l_rc = 1;
                }

                break;  // Done if address is of Vital clock domain
            }

            // ----------------------------------------------------------------------------------
            // Get clock domain for chiplet Id
            // Use look-up ChipletRingClockDomainTable
            // ----------------------------------------------------------------------------------
            dbg_print("\nChipletRingClockDomainTable look-up: ChipletId 0x%.2X, RingId 0x%.2X\n", l_chipletId, l_ringId);

            // Find matching Chiplet and Ring Ids in table
            for (l_index = 0;
                 l_index < ( sizeof(ChipletRingClockDomainTable) / sizeof(ChipletRingClockDomain_t) );
                 ++l_index)
            {
                if ( (ChipletRingClockDomainTable[l_index].chipletId == l_chipletId) &&
                     (ChipletRingClockDomainTable[l_index].ringId == l_ringId) )
                {
                    o_domain = ChipletRingClockDomainTable[l_index].domain;
                    break;
                }
            }

            // If no match, re-run and look for ALL_GOOD_ID to pick-up 'allgood' clock domain
            if (o_domain == ODYSSEY_FAKE_DOMAIN)
            {
                dbg_print("\nRe-run ChipletRingClockDomainTable look-up: ChipletId 0x%.2X, RingId 0x%.2X\n", l_chipletId, ALL_GOOD_ID);

                for (l_index = 0;
                     l_index < ( sizeof(ChipletRingClockDomainTable) / sizeof(ChipletRingClockDomain_t) );
                     ++l_index)
                {
                    if ( (ChipletRingClockDomainTable[l_index].chipletId == l_chipletId) &&
                         (ChipletRingClockDomainTable[l_index].ringId == ALL_GOOD_ID) )
                    {
                        o_domain = ChipletRingClockDomainTable[l_index].domain;
                        break;
                    }
                }
            }

            if (o_domain == ODYSSEY_FAKE_DOMAIN)
            {
                l_rc = 1;
                break;
            }

            // Done
            break;
        }
        while(0);

        if (l_rc == 0)
        {
            std::string l_clockDomainStr = odyssey_clockcntl_convertClockDomainEnum(o_domain);
            dbg_print("  Output domain: %d (%s)\n", o_domain, l_clockDomainStr.c_str());
        }

        dbg_print("Exiting odyssey_clockcntl_getScomClockDomain. l_rc = %d\n", l_rc);
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
        uint8_t l_index;

        // Look for input domain enum
        for (l_index = 0;
             l_index < (sizeof(ClockDomainStrAddrMaskTable) / sizeof(ClockDomainStrAddrMask_t));
             l_index++)
        {
            if (i_domain == ClockDomainStrAddrMaskTable[l_index].domainEnum)
            {
                // Found a match
                l_ret = std::string(ClockDomainStrAddrMaskTable[l_index].domainStr);
                break;
            }
        }

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
        CLOCK_DOMAIN l_ret = ODYSSEY_FAKE_DOMAIN;
        uint8_t l_index;

        // Look for input string
        for (l_index = 0;
             l_index < (sizeof(ClockDomainStrAddrMaskTable) / sizeof(ClockDomainStrAddrMask_t));
             l_index++)
        {
            if (i_domain.compare(std::string(ClockDomainStrAddrMaskTable[l_index].domainStr)) == 0)
            {
                // Found a match
                l_ret = ClockDomainStrAddrMaskTable[l_index].domainEnum;
                break;
            }
        }

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
        std::string l_chipUnitStr;
        l_rc = odyssey_convertCUEnum_to_String(i_odysseyCU, l_chipUnitStr);

        if (l_rc)
        {
            return l_rc;
        }

        dbg_print("\nEntering odyssey_clockcntl_getScanClockDomain\n");
        dbg_print(" Chip unit type  : %s\n", l_chipUnitStr.c_str());
        dbg_print(" Chip unit num   : %d\n", i_chipUnitNum);
        dbg_print(" Clock domain    : %d\n", io_domain);
        dbg_print(" Mode            : %d\n", i_mode);

        // Use macro to convert to the input domain to the one for i_chipUnitNum
        // ## is the macro concatenation operator.
        // Notes: there is only 1 chiplet per chiplet type in Odyssey, but
        //        keep this macro code for consistency.

        // -------------------------------
        // STANDBY
        // -------------------------------
#define CHECKFSI(P)                         \
    if (io_domain == ODYSSEY_##P##_DOMAIN )  \
    { \
        io_domain = ODYSSEY_##P##_DOMAIN; \
    }
        CHECKFSI(STANDBY)

        // -------------------------------
        // TP chiplet
        // -------------------------------
#define CHECKTP(P)                         \
    if (io_domain == ODYSSEY_TP##P##_DOMAIN )  \
    {  \
        io_domain = ODYSSEY_TP##P##_DOMAIN; \
    }
        CHECKTP(VITL)
        CHECKTP(SBEPIBNET)
        CHECKTP(PLLMC)

        // -------------------------------
        // Mem chiplet
        // -------------------------------
#define CHECKMP(P)                         \
    if (io_domain == ODYSSEY_MEM##P##_DOMAIN )  \
    {  \
        io_domain = ODYSSEY_MEM##P##_DOMAIN; \
    }
        CHECKMP(VITL)
        CHECKMP(NONVITL)

        dbg_print("\nOutput io_domain = %u\n", io_domain);
        return l_rc;
    }


// ####################################################################################
    uint32_t odyssey_clockcntl_getClockCheckData(const CLOCK_DOMAIN i_domain,
            uint64_t& o_clockAddr,
            uint64_t& o_clockMask,
            uint32_t& o_mode )
    {
        uint32_t l_rc = 0;
        uint8_t l_index = 0;

        // Init output values
        o_clockAddr = 0xFFFFFFFFFFFFFFFFULL;
        o_clockMask = 0xFFFFFFFFFFFFFFFFULL;
        o_mode = 1;

        std::string l_clockDomainStr = odyssey_clockcntl_convertClockDomainEnum(i_domain);
        dbg_print(" \nClockDomainStrAddrMaskTable look-up: domain %s\n", l_clockDomainStr.c_str());

        // Look for input domain enum
        for (l_index = 0;
             l_index < (sizeof(ClockDomainStrAddrMaskTable) / sizeof(ClockDomainStrAddrMask_t));
             l_index++)
        {
            if (i_domain == ClockDomainStrAddrMaskTable[l_index].domainEnum)
            {
                // Found a match
                o_clockAddr = ClockDomainStrAddrMaskTable[l_index].clockAddr;
                o_clockMask = ClockDomainStrAddrMaskTable[l_index].clockMask;
                break;
            }
        }

        // Error, can't find input entry in table
        if (o_clockAddr == 0xFFFFFFFFFFFFFFFFULL)
        {
            l_rc = 1;
        }
        else if (o_clockAddr == 0x0000281A) // CFAM address
        {
            o_mode = 0;
        }

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
        CLOCK_DOMAIN l_domain;
        uint32_t l_mode = 0;

        // Determine if the input address has anything to do with the Clock control.
        // If it does, this function returns the list of clock domains that may be
        // affected.

        // There are 2 things to check to determine if Clock control could
        // be altered:
        //   1. See if the address is the start/stop of Clocks register (00000000yy030006)
        //   2. The domain is Vital

        do
        {
            // Mask off the chiplet ID bits, just check to see if the register is Clock
            // control for each chiplet (i.e. 0x30006)
            if ( (i_address & 0x0000000000FFFFFFULL) == 0x00030006ULL)
            {
                // check for multicast group
                if (i_address & 0x0000000040000000ULL)
                {
                    uint8_t l_multicast_group = ((i_address >> 24) & 0x07);

                    for (uint8_t l_index = 0;
                         l_index < (sizeof(ClockDomainStrAddrMaskTable) / sizeof(ClockDomainStrAddrMask_t));
                         l_index++)
                    {
                        if ((ClockDomainStrAddrMaskTable[l_index].inMulticastGroup >> (7 - l_multicast_group)) & 0x01)
                        {
                            o_domainList.push_back(ClockDomainStrAddrMaskTable[l_index].domainEnum);
                        }
                    }

                    break;
                }


                // If it is, get the clock domain of the given address/instance
#ifndef ODYSSEY
                l_rc = odyssey_clockcntl_getScomClockDomain_INTERNAL(i_odysseyCU,
                        i_chipUnitNum,
                        i_address,
                        l_domain,
                        l_mode);
#else
                l_rc = odyssey_clockcntl_getScomClockDomain(i_odysseyCU,
                        i_chipUnitNum,
                        i_address,
                        l_domain,
                        l_mode);
#endif

                if (l_rc)
                {
                    break;
                }

                switch (l_domain)
                {
                    case ODYSSEY_FAKE_DOMAIN:
                        break;

                    case ODYSSEY_STANDBY_DOMAIN:
                        break;

                    case ODYSSEY_TPVITL_DOMAIN:
                        o_domainList.push_back(ODYSSEY_TPSBEPIBNET_DOMAIN);
                        o_domainList.push_back(ODYSSEY_TPPLLMC_DOMAIN);
                        break;

                    case ODYSSEY_MEMVITL_DOMAIN:
                        o_domainList.push_back(ODYSSEY_MEMNONVITL_DOMAIN);
                        break;

                    default:
                        break;
                }

            } // End if (...0x00030006ULL) block

        }
        while (0);

        return l_rc;
    }

} // extern "C"

#undef ODYSSEY_CLOCKCNTL_C
