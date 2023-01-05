/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/plat_target_sbe.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#ifndef __TARGET_SBE__
#define __TARGET_SBE__

#include "plat_target_base.H"


namespace fapi2
{
    /*
     * Derived class for target handle.
     * It will be derived in each image.
     */

    class plat_target_sbe_handle : public plat_target_base_handle{

        public:
            ///
            /// @brief     Plat target handle constructor
            /// @param[in] i_value value to instantiate handle with
            ///
            plat_target_sbe_handle(uint32_t i_value = 0):
                plat_target_base_handle(i_value)
            {}

            /*
            * plat_target_base_handle bit layout :
            * BIT VALUES FOR BIT-0 TO BIT-31:
            *   Chiplet Number                 : 08 Bits => Chiplet Number          [ bit00 – bit7 ]
            *   Type                           : 06 Bits => PPE target type         [ bit08 – bit13 ]
            *   Present                        : 01 Bit  => Target present or not   [ bit14 ]
            *   Functional                     : 01 Bit  => Target functional state [ bit15 ]
            *   Instance Number                : 08 Bits => Target Instance Number  [ bit16 – bit23 ]
            *   Reserve                        : 08 Bits => Free Bits               [ bit24 – bit31 ]
            */

            ///
            /// @brief     Plat target handle constructor
            /// @param[in] i_chipletNum to update FAPI target type
            /// @param[in] i_type to update PPE target type
            /// @param[in] i_instanceNum to update target instance num
            /// @param[in] i_present to set target present or not
            /// @param[in] i_functional to set target functional state
            ///
            plat_target_sbe_handle(uint8_t i_chipletNum,
                                uint8_t i_type,
                                uint8_t i_instanceNum,
                                bool i_present = true,
                                bool i_functional = true):
                plat_target_base_handle(i_chipletNum,
                                        i_type,
                                        i_present,
                                        i_functional)
            {
                if(i_type == PPE_TARGET_TYPE_DIMM)
                {
                    iv_value.insertFromRight(i_instanceNum, 16, 8);
                }
                else
                {
                    iv_value.insertFromRight(i_chipletNum, 16, 8);
                }
            }
    };
}

#endif
