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
    // Declare the SBE_ROOT_CHIP_TYPE at plat level
    static const TargetType SBE_ROOT_CHIP_TYPE = TARGET_TYPE_OCMB_CHIP;
    static const LogTargetType LOG_SBE_ROOT_CHIP_TYPE = LOG_TARGET_TYPE_OCMB_CHIP;
    static const TargetType SBE_ROOT_CHIP_CHILDREN = TARGET_TYPE_PERV
        | TARGET_TYPE_MEM_PORT
        | TARGET_TYPE_DIMM
        | TARGET_TYPE_TEMP_SENSOR;

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
            *   Chiplet Number                 : 08 Bits => Chiplet Number  or             [ bit00 – bit7 ]
            *                                               Port Number for I2C targets (TEMP SENSOR)
            *   Type                           : 06 Bits => PPE target type                [ bit08 – bit13 ]
            *   Present                        : 01 Bit  => Target present or not          [ bit14 ]
            *   Functional                     : 01 Bit  => Target functional state        [ bit15 ]
            *   Instance Number                : 08 Bits => Target Instance Number         [ bit16 – bit23 ]
            *   Device Addr                    : 08 Bits => Device Address for I2C targets [ bit24 – bit31 ]
            */

            ///
            /// @brief Create a platform target handle, defaults to present+functional
            /// @param[in] i_chipletNum The chiplet ID for this target
            /// @param[in] i_type       The log FAPI of this target
            /// @param[in] i_instance   The instance ID of this target
            ///
            plat_target_sbe_handle(uint8_t i_chipletNum,
                                uint8_t i_type,
                                uint8_t i_instance):
                plat_target_base_handle(i_chipletNum,
                                        i_type,
                                        i_instance)
            {
                if(i_type == LOG_TARGET_TYPE_TEMP_SENSOR)
                {
                    // TODO: Read the attributes to set the port and device addr.
                    uint8_t portNum = 0;
                    uint8_t devAddr = 0xFF;
                    iv_value.insertFromRight(portNum, 0, 8);
                    iv_value.insertFromRight(devAddr, 24, 8);
                }
            }

            /// @brief Get the parent target instance number, used incase of convertToChiplet
            /// @praram[in] i_child child target
            /// @return uint8 of parent target instance number
            ///
            uint8_t getParentTargetInstance(const plat_target_sbe_handle i_child) const
            {
                uint8_t parentInstanceNum = 0;
                // Child target type is memport.
                if(i_child.getTargetType() == LOG_TARGET_TYPE_DIMM)
                {
                    parentInstanceNum = (i_child.getTargetInstance() / 2);
                }
                return parentInstanceNum;
            }
    };
}

#endif
