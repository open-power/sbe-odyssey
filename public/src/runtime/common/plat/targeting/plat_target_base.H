/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/targeting/plat_target_base.H $ */
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


#ifndef __FAPI2_PLAT_BASE_TARGET__
#define __FAPI2_PLAT_BASE_TARGET__

#include <stdint.h>
#include <target_types.H>
#include <target_states.H>
#include <target_filters.H>
#include <assert.h>
#include <vector>
#include <buffer.H>

namespace fapi2
{
   /*
    * Structure for target params
    */
    typedef struct targetInfo
    {
        uint32_t chipletNum:8;
        uint32_t targetType:6;
        uint32_t isPervType:1;
        uint32_t isChipletType:1;
        uint32_t targetCnt:16;
    }targetInfo_t;

    extern const ArrayWrapper<targetInfo_t> G_projTargetMap;
    extern const ArrayWrapper<target_filter_definition> G_projTargetFilters;

    // LOG target type to FAPI target type
    #define GET_FAPI_TARGET_TYPE(log_target)    static_cast<fapi2::TargetType>(1ull << log_target)

    /*
     * The plat_target_base_handle bit layout is designed so that when the bits
     * used for internal bookkeeping are masked off, we end up with a PIB
     * base address that can be ORed onto a register address constant to
     * yield a final PIB address.
     */

    /*
     * plat_target_base_handle bit layout :
     * BIT VALUES FOR BIT-0 TO BIT-31:
     *   Chiplet Number                 : 08 Bits => Chiplet Number          [ bit00 – bit7 ]
     *   Type                           : 06 Bits => PPE target type         [ bit08 – bit13 ]
     *   Present                        : 01 Bit  => Target present or not   [ bit14 ]
     *   Functional                     : 01 Bit  => Target functional state [ bit15 ]
     *   Instance number                : 08 Bits => Target instance number  [ bit16 - bit23 ]
     *   Reserve                        : 08 Bits => Free Bits               [ bit24 – bit31 ]
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
                setChipletNumber(i_chipletNum);
                setTargetType(i_type);
                setPresent(i_present);
                setFunctional(i_functional);
            }

            ///
            /// @brief Get the instance number for this target
            /// @return uint8 of FAPI chiplet number
            ///
            uint8_t getChipletNumber() const
            {
                return static_cast<uint8_t>(iv_value.getBits<0, 8>());
            }

            ///
            /// @brief Get the is_multicast for this target
            /// @return is_multicast value
            ///
            bool getIsMulticast() const
            {
                return static_cast<bool>(iv_value.getBit<1>());
            }

            ///
            /// @brief Get the mcast_type for this target
            /// @return mcast_type value
            ///
            bool getMcastType() const
            {
                return static_cast<uint8_t>(iv_value.getBits<2, 3>());
            }

            ///
            /// @brief Set the mcast_type for this target
            /// @param[in] i_mcast_type present state to set
            ///
            void setMcastType(uint8_t i_mcast_type)
            {
                iv_value.insertFromRight<2, 3>(i_mcast_type);
            }

            ///
            /// @brief Get the mcast_group for this target
            /// @return mcast_group value
            ///
            bool getMcastGroup() const
            {
                return static_cast<uint8_t>(iv_value.getBits<5, 3>());
            }

            ///
            /// @brief Set the mcast_group for this target
            /// @param[in] i_mcast_group present state to set
            ///
            void setMcastGroup(uint8_t i_mcast_group)
            {
                iv_value.insertFromRight<5, 3>(i_mcast_group);
            }

            ///
            /// @brief Get the target instance number.
            /// @return uint8 of target instance number
            ///
            uint8_t getTargetInstance() const
            {
                return static_cast<uint8_t>(iv_value.getBits<16, 8>());
            }

            ///
            /// @brief Get the plat target type
            /// @return The plat target type as a fapi2::TargetType
            ///
            LogTargetType getTargetType() const
            {
                return static_cast<LogTargetType>(iv_value.getBits<8, 6>());
            }

            ///
            /// @brief Returns whether this target is present or not
            /// @return true if target is present state
            ///
            bool getPresent() const
            {
                return iv_value.getBit<14>();
            }

            ///
            /// @brief Set present state of the target
            /// @param[in] i_state present state to set
            ///
            void setPresent(const bool i_state)
            {
               iv_value.writeBit<14>(i_state);
            }

            ///
            /// @brief Returns whether this target is functional
            /// @return true if target is functional
            ///
            bool getFunctional() const
            {
                return iv_value.getBit<15>();
            }

            ///
            /// @brief Set functional state of the Target
            /// @param[in] i_state functional state to set
            ///
            void setFunctional(const bool i_state)
            {
                iv_value.writeBit<15>(i_state);
            }

            ///
            /// @brief Get the fapi2::TargetType for this target
            /// @return The fapi2::TargetType for this target
            ///
            TargetType getFapiTargetType() const
            {
                return GET_FAPI_TARGET_TYPE(getTargetType());
            }

            ///
            /// @brief Return the target value with all internal bits masked away
            /// @return return the chiplet ID which can be used to find effective scom address
            ///
            uint32_t getPIBAddress() const
            {
                return iv_value & 0x7F000000;
            }

            ///
            /// @brief Gets the plat target handle as a uint32
            /// @return Plat target handle as a uint32_t
            ///
            operator uint32_t() const {return iv_value();}

            // TODO: Find ways to make these setters protected

            ///
            /// @brief Set the is_multicast for this target
            /// @param[in] i_is_multicast present state to set
            ///
            void setIsMulticast(const bool i_is_multicast)
            {
                iv_value.writeBit<1>(i_is_multicast);
            }

            ///
            /// @brief Set the plat target type
            /// @return @param[in] i_value for target number to set
            ///
            void setTargetType(uint8_t i_value)
            {
                iv_value.insertFromRight<8, 6>(i_value);
            }

            ///
            /// @brief Set the target instance number.
            /// @param[in] i_value target instance number
            ///
            void setTargetInstance(const uint8_t i_value)
            {
                iv_value.insertFromRight<16, 8>(i_value);
            }
    protected:
            ///
            /// @brief Set the chiplet number
            /// @param[in] i_value chiplet number to set
            ///
            void setChipletNumber(const uint8_t i_value)
            {
                iv_value.insertFromRight<0, 8>(i_value);
            }
    };
};

#endif
