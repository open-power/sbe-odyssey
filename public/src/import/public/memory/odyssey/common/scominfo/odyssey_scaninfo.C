/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/common/scominfo/odyssey_scaninfo.C $ */
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
/// @file odyssey_scaninfo.C
/// @brief ODYSSEY scan address control functions for platform
///
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: Cronus
///

#ifndef ODYSSEY_SCANINFO_C
#define ODYSSEY_SCANINFO_C

#include <odyssey_scaninfo.H>
#include <odyssey_scan_addr.H>
#include <odyssey_cu.H>

extern "C"
{
    uint32_t odyssey_scaninfo_isChipUnitScanRing(
        uint32_t i_scanAddr,
        bool& o_chipUnitRelated,
        std::vector<odyssey_chipUnitPairing_t>& o_chipUnitPairing)
    {
        uint32_t l_rc = 0;
        // There is no chip unit associated scan rings for Odyssey
        o_chipUnitRelated = false;
        return l_rc;
    }

    uint32_t odyssey_scaninfo_createChipUnitScanAddr(
        odysseyChipUnits_t i_odysseyCU,
        uint8_t i_ChipUnitNum,
        uint32_t i_scanAddr)
    {
        // There is no chip unit associated scan rings for Odyssey
        // If input chip unit is a chip, return input scan address; otherwise, return bad address

        uint32_t l_returnAddr = i_scanAddr;

        if (i_odysseyCU != ODYSSEY_NO_CU)
        {
            l_returnAddr = RC_ODYSSEY_SCANINFO_BAD_ADDR;
        }

        return l_returnAddr;
    }

} // extern "C"

#endif //ODYSSEY_SCANINFO_C
