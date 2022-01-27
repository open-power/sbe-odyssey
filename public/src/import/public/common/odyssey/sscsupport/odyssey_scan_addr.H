/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/sscsupport/odyssey_scan_addr.H $ */
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
/// @file odyssey_scan_addr.H
/// @brief ODYSSEY scan address utility functions for platforms
///
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: Cronus
///

#ifndef ODYSSEY_SCAN_ADDR_H
#define ODYSSEY_SCAN_ADDR_H

#include <odyssey_cu.H>

extern "C"
{

    class odyssey_scan_addr
    {
        public:
            static bool isVitlRing(uint32_t i_addr);
            static uint32_t getRegion(uint32_t i_addr);
            static uint32_t getExtendedRegion(uint32_t i_addr);
            static uint32_t getChiplet(uint32_t i_addr);
            static uint32_t getScanType(uint32_t i_addr);
    };

} // extern "C"

#endif // ODYSSEY_SCAN_ADDR_H
