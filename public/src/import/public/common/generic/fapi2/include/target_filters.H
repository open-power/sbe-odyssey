/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/target_filters.H $ */
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
#pragma once
#include <stdint.h>

namespace fapi2
{

///
/// @brief Enumeration of chiplet filters
///
/// These constants select filters to apply, it is the task of the platform implementation
/// to translate these into specific sets of chiplets based on the target chip.
///
enum TargetFilter : uint64_t
{
    TARGET_FILTER_NONE       = 0,
    TARGET_FILTER_TP         = (1ull << 63) >> 0,
    TARGET_FILTER_N0         = (1ull << 63) >> 1,
    TARGET_FILTER_N1         = (1ull << 63) >> 2,
    TARGET_FILTER_N2         = (1ull << 63) >> 3,
    TARGET_FILTER_N3         = (1ull << 63) >> 4,
    TARGET_FILTER_NEST       = TARGET_FILTER_N0 | TARGET_FILTER_N1 | TARGET_FILTER_N2 | TARGET_FILTER_N3,
    TARGET_FILTER_PCI        = (1ull << 63) >> 5,
    TARGET_FILTER_MC         = (1ull << 63) >> 6,
    TARGET_FILTER_PAXO       = (1ull << 63) >> 7,
    TARGET_FILTER_EQ         = (1ull << 63) >> 8,
    TARGET_FILTER_XBUS       = (1ull << 63) >> 9,
    TARGET_FILTER_ABUS       = (1ull << 63) >> 10,
    TARGET_FILTER_CORE       = (1ull << 63) >> 11,
    TARGET_FILTER_TBUS       = (1ull << 63) >> 12,
};

// Helper macros to make chiplet masks easier to write and read
#define _CHIPLET(id) ((id) ? (1ULL << (63 - (id))) : 0)
#define _CHIPLET_MASK(id1, id2, id3, id4, id5, id6, id7, id8, id9, id10, id11, id12, ...) \
    (_CHIPLET(id1) | _CHIPLET(id2) | _CHIPLET(id3) | _CHIPLET(id4) | \
     _CHIPLET(id5) | _CHIPLET(id6) | _CHIPLET(id7) | _CHIPLET(id8) | \
     _CHIPLET(id9) | _CHIPLET(id10) | _CHIPLET(id11) | _CHIPLET(id12))
#define CHIPLET_MASK(...) _CHIPLET_MASK(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

///
/// @brief Definition of a chip specific target filter
///
/// Maps one of the TargetFilter values to a mask of chiplets
///
struct target_filter_definition
{
    TargetFilter filter;
    uint64_t chiplets;
};

}
