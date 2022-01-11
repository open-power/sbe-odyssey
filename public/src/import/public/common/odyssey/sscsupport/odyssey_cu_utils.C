/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_cu_utils.C $ */
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
/// @file odyssey_cu_utils.C
/// @brief ODYSSEY chip unit utility definitions
///
/// HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// HWP FW Maintainer:
/// HWP Consumed by: CRONUS
///

// Includes
#include <odyssey_cu_utils.H>

#define ODYSSEY_CU_UTILS_C

extern "C"
{

    //################################################################################
    uint32_t odyssey_convertCUEnum_to_String(const odysseyChipUnits_t i_ODYSSEY_CU,
            std::string& o_chipUnitType)
    {
        uint8_t l_index;

        // Look for input chip unit value
        for (l_index = 0;
             l_index < (sizeof(odysseyChipUnitDescriptionTable) / sizeof(odyssey_chipUnitDescription_t));
             l_index++)
        {
            if (i_ODYSSEY_CU == odysseyChipUnitDescriptionTable[l_index].enumVal)
            {
                // Found a match
                o_chipUnitType = odysseyChipUnitDescriptionTable[l_index].strVal;
                return 0;
            }
        }

        return 1;
    }

    //################################################################################
    uint32_t odyssey_convertCUString_to_enum(const std::string& i_chipUnitType,
            odysseyChipUnits_t& o_ODYSSEY_CU)
    {
        uint8_t l_index;

        // Look for input chip unit string
        for (l_index = 0;
             l_index < (sizeof(odysseyChipUnitDescriptionTable) / sizeof(odyssey_chipUnitDescription_t));
             l_index++)
        {
            if (i_chipUnitType.compare(odysseyChipUnitDescriptionTable[l_index].strVal) == 0)
            {
                // Found a match
                o_ODYSSEY_CU = odysseyChipUnitDescriptionTable[l_index].enumVal;
                return 0;
            }
        }

        return 1;
    }


    //################################################################################
    void odyssey_displayAddrFields(uint64_t i_addr,
                                   odysseyChipUnits_t i_chipUnitType,
                                   uint32_t i_chipEcLevel)
    {
        odyssey_scom_addr l_scom(i_addr);
        printf("\n     Chip EC: 0x%.2X\n", i_chipEcLevel);
        printf("        Addr: 0x%08lx_%08llx\n", (i_addr >> 32), (i_addr & 0xFFFFFFFFULL));
        printf("   ChipletId: 0x%02X\n", l_scom.getChipletId());
        printf("    Endpoint: 0x%02X\n", l_scom.getEndpoint());
        printf("      RingId: 0x%.1X\n", l_scom.getRingId());
        printf("       SatId: 0x%.1X\n", l_scom.getSatId());
        printf("      SatReg: 0x%.2X\n", l_scom.getSatOffset());
        return;
    }

} // extern "C"

#undef ODYSSEY_CU_UTILS_C
