/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_base.H $ */
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


#ifndef __FAPI2_PLAT_BASE_TARGET__
#define __FAPI2_PLAT_BASE_TARGET__

#include <stdint.h>
#include <target_types.H>
#include <target_states.H>
#include <assert.h>
#include <vector>
#include <buffer.H>

namespace fapi2
{

    typedef enum plat_target_type
    {
        PPE_TARGET_TYPE_NONE           = 0x00,
        PPE_TARGET_TYPE_PROC_CHIP      = 0x01,
    } plat_target_type_t;

    /*
     * The plat_target_base_handle bit layout is designed so that when the bits
     * used for internal bookkeeping are masked off, we end up with a PIB
     * base address that can be ORed onto a register address constant to
     * yield a final PIB address.
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
    class plat_target_base_handle {

        public:

            fapi2::buffer<uint32_t> iv_value;
            ///
            /// @brief     Plat target handle constructor
            /// @param[in] i_value value to instantiate handle with
            ///
            explicit plat_target_base_handle(uint32_t i_value = 0):iv_value(i_value)
            {
                static_assert(sizeof(*this) == sizeof(uint32_t),
                              "Incorrect size of plat_target, check your values!");
            }

            ///
            /// @brief Plat target handle constructor with parameters
            /// @param[in] i_chipletNum to update FAPI target type
            /// @param[in] i_type to update PPE target type
            /// @param[in] i_present to set target present or not
            /// @param[in] i_functional to set target functional state
            ///
            explicit plat_target_base_handle( uint8_t i_chipletNum,
                                     uint8_t i_type,
                                     bool i_present = true,
                                     bool i_functional = true )
            {
                iv_value.insertFromRight(i_chipletNum, 0, 8);
                iv_value.insertFromRight(i_type, 8, 5);
                iv_value.writeBit<13>(i_present);
                iv_value.writeBit<14>(i_functional);
            }

            ///
            /// @brief Get the instance number for this target
            /// @return uint8 of FAPI chiplet number
            ///
            uint8_t getChipletNumber() const
            {
                return static_cast<uint8_t>(iv_value.getBits<0,8>());
            }

            ///
            /// @brief Get the plat target type
            /// @param[in] i_value for target number to set
            ///
            void setChipletNumber(const uint8_t i_value)
            {
                iv_value.insertFromRight(i_value, 0, 8);
                return;
            }

            ///
            /// @brief Get the plat target type
            /// @return The plat target type as a fapi2::TargetType
            ///
            plat_target_type getTargetType() const
            {
                return static_cast<plat_target_type>(iv_value.getBits<8,5>());
            }

            ///
            /// @brief Set the plat target type
            /// @return @param[in] i_value for target number to set
            ///
            void setTargetType(uint8_t i_value)
            {
                iv_value.insertFromRight(i_value, 8, 5);
            }

            ///
            /// @brief Returns whether this target is present or not
            /// @return true if target is present state
            ///
            bool getPresent() const
            {
                return iv_value.getBit<13>();
            }

            ///
            /// @brief Set present state of the target
            /// @param[in] i_state present state to set
            ///
            void setPresent(const bool i_state)
            {
               iv_value.writeBit<13>(i_state);
            }

            ///
            /// @brief Returns whether this target is functional
            /// @return true if target is functional
            ///
            bool getFunctional() const
            {
                return iv_value.getBit<14>();
            }

            ///
            /// @brief Set functional state of the Target
            /// @param[in] i_state functional state to set
            ///
            void setFunctional(const bool i_state)
            {
                iv_value.writeBit<14>(i_state);
            }

            ///
            /// @brief Get the fapi2::TargetType for this target
            /// @return The fapi2::TargetType for this target
            ///
            TargetType getFapiTargetType() const
            {
                TargetType l_targetType = TARGET_TYPE_NONE;
                switch(getTargetType())
                {
                    case PPE_TARGET_TYPE_PROC_CHIP:
                        l_targetType = TARGET_TYPE_PROC_CHIP;
                        break;
                    case PPE_TARGET_TYPE_NONE:
                    default:
                        assert(false);
                        break;
                }
                return l_targetType;
            }

            ///
            /// @brief Gets the plat target handle as a uint32
            /// @return Plat target handle as a uint32_t
            ///
            operator uint32_t() const {return iv_value();}
    };
};

#endif

