/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/multicast_defs.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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
#ifndef __FAPI2_COMMON_MULTICAST_DEFS__
#define __FAPI2_COMMON_MULTICAST_DEFS__

namespace fapi2
{
enum MulticastType : uint32_t
{
    // Code points picked according to PCB spec
    MULTICAST_OR      = 0,
    MULTICAST_AND     = 1,
    MULTICAST_BITX    = 2,
    MULTICAST_COMPARE = 4,
};

/**
 * @brief Statically defined multicast groups
 *
 * These multicast groups are set up statically on every chip
 * and available everywhere across chip types.
 *
 * If a specific chip has other statically defined groups,
 * it may define them in a separate multicast_group_defs.H.
 *
 * Any groups that are not static are free for HWPs to set up
 * as required. The philosophy is to "make your own bed", i.e.
 * set up all the MC groups you need before you go.
 */
enum MulticastGroup : uint32_t
{
    MCGROUP_GOOD       = 0,    ///< All good chiplets
    MCGROUP_GOOD_NO_TP = 1,    ///< All good chiplets except PERV
    MCGROUP_2          = 2,
    MCGROUP_3          = 3,
    MCGROUP_4          = 4,
    MCGROUP_5          = 5,
    MCGROUP_6          = 6,
    MCGROUP_ALL        = 7,    ///< All chiplets, good or bad, a.k.a. broadcast
};

/// @brief Leftover from P10 days to keep platform compilation intact
struct MulticastGroupMapping
{
};

/**
 * @brief Bit mask for selecting cores out of an EQ.
 */
enum MulticastCoreSelect : uint32_t
{
    MCCORE_0   = 0x8,
    MCCORE_1   = 0x4,
    MCCORE_2   = 0x2,
    MCCORE_3   = 0x1,
    MCCORE_ALL = 0xF,
};

inline MulticastCoreSelect operator|(const MulticastCoreSelect a, const MulticastCoreSelect b)
{
    return static_cast<MulticastCoreSelect>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

}

#endif
