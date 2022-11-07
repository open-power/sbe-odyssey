/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_ring_utils.H $      */
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

#ifndef PLAT_RING_UTILS_H_
#define PLAT_RING_UTILS_H_

#include <stdint.h>

/// @cond
typedef uint64_t scanRingId_t;
typedef uint16_t RingId_t;
/// @endcond

namespace fapi2
{
///
/// @enum fapi2::RingMode
/// @brief Enumeration of Ring access operation modes
///        This is a bitmap to allow the user to specify multiple modes.
///
enum RingMode
{
    RING_MODE_HEADER_CHECK              = 0x00000000, ///< Check header
    RING_MODE_SET_PULSE_NO_OPCG_COND    = 0x00000001, ///< Set pulse with
    ///  no OPCG conditioning
    RING_MODE_NO_HEADER_CHECK           = 0x00000002, ///< Don't check header
    RING_MODE_SET_PULSE_NSL             = 0x00000004, ///< Set pulse with NSL
    ///  pulse
    RING_MODE_SET_PULSE_SL              = 0x00000008, ///< Set pulse with SL
    ///  pulse
    RING_MODE_SET_PULSE_ALL             = 0x00000010, ///< Set pulse with pulse
    ///  to all hold types
    RING_MODE_FASTARRAY                 = 0x00000020, ///< Use fastarray TOR image
};

}

#endif // PLAT_RING_UTILS_H_