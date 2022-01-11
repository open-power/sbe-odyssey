/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_scaninfo.H $ */
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
/// @file odyssey_scaninfo.H
/// @brief ODYSSEY scan address control functions for platform
///
/// *HWP HW Maintainer: Joe McGill <jmcgill@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: Cronus
///

#ifndef ODYSSEY_SCANINFO_H
#define ODYSSEY_SCANINFO_H

#include <vector>
#include "odyssey_scan_addr.H"
#include "odyssey_cu.H"

extern "C" {

    const uint32_t RC_ODYSSEY_SCANINFO_BAD_ADDR = 0xFFFFFFFFUL;

    /**
     @brief Determines if the scan address correlates to a chipUnit, if so returns
            a vector of associated chipUnits and their instance numbers
     @param i_scanAddr         scan address to test
     @param o_chipUnitRelated  bool returns true if scan addr is a chipUnit Addr
     @param o_chipUnitPairing  vector of matching chipUnitPairings
     @retval uint32_t return non-zero for error
    */
    uint32_t odyssey_scaninfo_isChipUnitScanRing(
        uint32_t i_scanAddr,
        bool& o_chipUnitRelated,
        std::vector<odyssey_chipUnitPairing_t>& o_chipUnitPairing);

    /**
     @brief Creates the actual physical scan addr based on the chipunit,
            chipUnitNum, and chipUnit zero's scan addr
     @param i_odysseyCU    enumeration of the ChipUnit
     @param i_ChipUnitNum  instance number of the chipUnit
     @param i_scanAddr     scan addr of chipUnit zero
     @retval uint32_t actual scan addr for the chipUnit/chipUnitNum passed in
    */
    uint32_t odyssey_scaninfo_createChipUnitScanAddr(
        odysseyChipUnits_t i_odysseyCU,
        uint8_t i_ChipUnitNum,
        uint32_t i_scanAddr);


} // extern "C"

#endif /* ODYSSEY_SCANINFO_H */
