/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/build/links/odysseylink.H $                        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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
#ifndef __ODYSSEY_LINK_H
#define __ODYSSEY_LINK_H

/// \file odysseylink.H
/// \brief Constants required for linking SBE code images

//TODO:Update file with actual offset's and size

//SROM Start location
#define SROM_ORIGIN 0xFFF70000

//SROM Size
#define SROM_SIZE 0x10000

//SRAM Start location
#define SRAM_ORIGIN 0xFFE80000

//SRAM Size
#define SRAM_SIZE 0x80000

#define SBE_CODE_BOOT_PIBMEM_MAIN_MSG 0x0F

#endif  // __ODYSSEY_LINK_H

