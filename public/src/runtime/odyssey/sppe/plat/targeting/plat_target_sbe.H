/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/plat_target_sbe.H $ */
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

            ///
            /// @brief     Plat target handle constructor
            /// @param[in] i_chipletNum to update FAPI target type
            /// @param[in] i_type to update PPE target type
            /// @param[in] i_present to set target present or not
            /// @param[in] i_functional to set target functional state
            ///
            plat_target_sbe_handle(uint8_t i_chipletNum,
                                uint8_t i_type,
                                bool i_present = true,
                                bool i_functional = true):
                plat_target_base_handle(i_chipletNum,
                                        i_type,
                                        i_present,
                                        i_functional)
            {
            }
    };
}

#endif
