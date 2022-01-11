/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_scaninfo.C $ */
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
/// *HWP HW Maintainer: Joe McGill <jmcgill@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: Cronus
///

#ifndef ODYSSEY_SCANINFO_C
#define ODYSSEY_SCANINFO_C

#include "odyssey_scaninfo.H"
#include "odyssey_scan_addr.H"

extern "C"
{

    uint32_t odyssey_scaninfo_isChipUnitScanRing(
        uint32_t i_scanAddr,
        bool& o_chipUnitRelated,
        std::vector<odyssey_chipUnitPairing_t>& o_chipUnitPairing)
    {
        uint32_t l_rc = 0;
        return l_rc;
    }

    uint32_t odyssey_scaninfo_createChipUnitScanAddr(
        odysseyChipUnits_t i_odysseyCU,
        uint8_t i_ChipUnitNum,
        uint32_t i_scanAddr)
    {
        uint32_t o_scanAddr = RC_ODYSSEY_SCANINFO_BAD_ADDR;
        return o_scanAddr;

    }

} // extern "C"

#endif //ODYSSEY_SCANINFO_C
