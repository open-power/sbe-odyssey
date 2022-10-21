/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/common/scominfo/odyssey_scan_addr.C $ */
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
/// @file odyssey_scan_addr.C
/// @brief ODYSSEY scan address utility functions for platforms
///
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: Cronus
///

#ifndef ODYSSEY_SCAN_ADDR_C
#define ODYSSEY_SCAN_ADDR_C

#include <odyssey_scan_addr.H>

extern "C"
{
    // Note: Code is written based on Odyssey Ring Table doc.

    bool odyssey_scan_addr::isVitlRing(uint32_t i_addr)
    {
        return (((i_addr >> 16) & 0xF) == 0);
    }

    uint32_t odyssey_scan_addr::getScanType(uint32_t i_addr)
    {
        return (i_addr & 0xF);
    }

    uint32_t odyssey_scan_addr::getRegion(uint32_t i_addr)
    {
        return ((0x0000FFF0UL & i_addr) >> 4);
    }

    uint32_t odyssey_scan_addr::getExtendedRegion(uint32_t i_addr)
    {
        return ((0x00F00000UL & i_addr) >> 20);
    }

    uint32_t odyssey_scan_addr::getChiplet(uint32_t i_addr)
    {
        return ((0xFF000000UL & i_addr) >> 24);
    }

} // extern "C"

#endif // ODYSSEY_SCAN_ADDR_C
