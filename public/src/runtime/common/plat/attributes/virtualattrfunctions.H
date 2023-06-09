/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/virtualattrfunctions.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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

#include <attribute_properties.H>
#include "fapi2_target.H"
#include "target.H"
#include "return_code.H"

namespace fapi2
{

// Class to pass return code for attribute related functions
class AttributeRC
{
    public:
    AttributeRC() : value(FAPI2_RC_SUCCESS){}
    AttributeRC(uint32_t i_val) : value(i_val){}
    operator uint32_t (){return value;}
    operator ReturnCode (){return value;}
    private:
    uint32_t value;
};

namespace ATTR
{
extern uint8_t ATTR_NAME;
}

/**
  * @brief read the targets positions
  *
  * Return the position of chp unit target passed. This function will internally
  *     use plat_target_handle_t.getTargetInstance()
  *
  * @param[in]  i_target target where position need to be found
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<TargetType K>
inline
AttributeRC get_ATTR_CHIP_UNIT_POS(const Target<K>& i_target,
        ATTR_CHIP_UNIT_POS_Type& o_val)
{
    o_val = i_target.get().getTargetInstance();
    return FAPI2_RC_SUCCESS;
}

template<>
inline
AttributeRC get_ATTR_CHIP_UNIT_POS(const Target<TARGET_TYPE_PERV>& i_target,
                                ATTR_CHIP_UNIT_POS_Type& o_val)
{
    o_val = i_target.getChipletNumber();
    return FAPI2_RC_SUCCESS;
}

/**
  * @brief read the targets relative positions
  *
  * Return the relative position of chip unit target passed.
  *     This function will internally
  *     use plat_target_handle_t.getTargetInstance()
  *
  * @tparam K the target type on which to operate
  * @param[in]  i_target target where position need to be found
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<TargetType K>
inline
AttributeRC get_ATTR_REL_POS(const Target<K>& i_target,
        ATTR_REL_POS_Type& o_val)
{
    o_val = i_target.get().getTargetInstance();
    return FAPI2_RC_SUCCESS;
}

/**
  * @brief read the targets relative positions - specialization for DIMM target type
  *
  * Return the relative position of chip unit target passed.
  *     This function will internally
  *     use plat_target_handle_t.getTargetInstance()
  *
  * @param[in]  i_target target where position need to be found
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<>
inline
AttributeRC get_ATTR_REL_POS(const Target<fapi2::TARGET_TYPE_DIMM>& i_target,
        ATTR_REL_POS_Type& o_val)
{
    o_val = i_target.get().getTargetInstance() % 2;
    return FAPI2_RC_SUCCESS;
}

/**
  * @brief read the chip name of the target
  *
  * Return the name of the chip.
  *
  * @param[in]  i_target input target
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<TargetType K>
inline
AttributeRC get_ATTR_NAME(
    const Target<K>& i_target,
    ATTR_NAME_Type& o_val)
{
    o_val = fapi2::ATTR::ATTR_NAME;

    return FAPI2_RC_SUCCESS;
}
}
