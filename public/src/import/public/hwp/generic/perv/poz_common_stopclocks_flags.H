/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_common_stopclocks_flags.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
/// @file  poz_common_stopclocks_flags.H
/// @brief Common module for stopclocks
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:HB
//------------------------------------------------------------------------------

#ifndef _POZ_COMMON_STOPCLOCKS_FLAGS_H_
#define _POZ_COMMON_STOPCLOCKS_FLAGS_H_

enum StopClocksFlags : uint32_t
{
    STOP_REGION_NONE    = 0x00000000,
    STOP_REGION_SBE     = 0x80000000,
    STOP_REGION_TP      = 0x40000000,
    STOP_REGION_NEST    = 0x20000000,
    STOP_REGION_TBUS    = 0x10000000,
    STOP_REGION_MC      = 0x08000000,
    STOP_REGION_PAXO    = 0x04000000,
    STOP_REGION_PCIE    = 0x02000000,
    STOP_REGION_CACHE   = 0x01000000,
    STOP_REGION_CORE    = 0x00800000,
    STOP_REGION_EQ      = 0x00400000,
    STOP_REGION_VITL    = 0x00200000,
};

inline StopClocksFlags operator|(const StopClocksFlags a, const StopClocksFlags b)
{
    return static_cast<StopClocksFlags>((uint32_t)a | (uint32_t)b);
}

#endif
