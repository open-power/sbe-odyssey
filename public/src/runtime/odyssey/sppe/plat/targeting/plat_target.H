/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/plat_target.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
/**
 * @file plat_target.H
 * @brief platform definitions for fapi2 targets
 */

#ifndef __FAPI2_PLAT_TARGET__
#define __FAPI2_PLAT_TARGET__

#include <plat_target_base.H>

namespace fapi2
{

    /*
     * The plat_target_handle bit layout is designed so that when the bits
     * used for internal bookkeeping are masked off
     */

    /*
     * plat_target_base_handle bit layout :
     * BIT VALUES FOR BIT-0 TO BIT-31:
     *   Chiplet_Num : 08 Bits => FAPI target type [ bit00 – bit07 ]
     *   Type        : 05 Bits => PPE target type  [ bit08 – bit12 ]
     *   Present     : 01 Bit  => Target present or not [ bit13 ]
     *   Functional  : 01 Bit  => Target functional state is on or off [ bit14 ]
     *   Reserve     : 17 Bits => Free Bits [ bit15 – bit31 ]
     */
    class plat_target_odyssey_handle : public plat_target_base_handle
    {
        public:

            ///
            /// @brief Plat target handle constructor
            /// @param[in] i_value Value to instantiate handle with
            ///
            explicit plat_target_odyssey_handle(uint32_t i_value = 0):
                     plat_target_base_handle(i_value)
            {
            }

            ///
            /// @brief Plat target handle constructor with parameters
            /// @param[in] i_chipletNum to update FAPI target type
            /// @param[in] i_type to update PPE target type
            /// @param[in] i_present to set target present or not
            /// @param[in] i_functional to set target functional state
            ///
            explicit plat_target_odyssey_handle( uint8_t i_chipletNum,
                                        uint8_t i_type,
                                        bool i_present = true,
                                        bool i_functional = true ):
            plat_target_base_handle(i_chipletNum, i_type, i_present, i_functional)
            {
            }
    };

    typedef class plat_target_odyssey_handle plat_target_handle_t;
}
#endif
