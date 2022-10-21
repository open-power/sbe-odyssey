/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/common/scominfo/OdysseyClockDomains.H $ */
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
/// @file OdysseyClockDomains.H
/// @brief Clock domain definitions
///
/// HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// HWP FW Maintainer: n/a
///

#ifndef _ODYSSEYCLOCKDOMAINS_H
#define _ODYSSEYCLOCKDOMAINS_H

// Due to Cronus legacy, we need to start at CLOCK_DOMAIN_32
#define ODYSSEY_FAKE_DOMAIN                      CLOCK_DOMAIN_32  // Fake domain placeholder
#define ODYSSEY_FAKE_DOMAIN_MASK                 0x00000000

// Standby
#define ODYSSEY_STANDBY_DOMAIN                   CLOCK_DOMAIN_33  // Standby/CFAM

// TP chiplet
#define ODYSSEY_TPVITL_DOMAIN                    CLOCK_DOMAIN_34  // TP VITL
#define ODYSSEY_TPSBEPIBNET_DOMAIN               CLOCK_DOMAIN_35  // TP PERV + SBE + PIB + NET
#define ODYSSEY_TPPLLMC_DOMAIN                   CLOCK_DOMAIN_36  // TP PLL

// MEM chiplet
#define ODYSSEY_MEMVITL_DOMAIN                   CLOCK_DOMAIN_37  // MC VITL
#define ODYSSEY_MEMNONVITL_DOMAIN                CLOCK_DOMAIN_38  // MC PERV + IO + CORE + CFG + DFI + PUB[01] + PRIM[01]

#endif
