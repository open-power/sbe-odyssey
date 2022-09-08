/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/virtualattrfunctions.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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

#ifndef __VIRTUAL_FUNCTIONS_H
#define __VIRTUAL_FUNCTIONS_H

#include <attribute_ids.H>
#include <return_code.H>
#include "fapi2_target.H"
#include "target.H"

namespace fapi2
{

// create a unique type from an int ( or attribute id)
template<int I>
struct int2Type {
enum { value = I };
};

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
AttributeRC _getAttrChipUnitPos(const Target<K>& i_target,
        ATTR_CHIP_UNIT_POS_Type& o_val)
{
    o_val = i_target.get().getTargetInstance();
    return FAPI2_RC_SUCCESS;
}

template<>
inline
AttributeRC _getAttrChipUnitPos(const Target<TARGET_TYPE_PERV>& i_target,
                                ATTR_CHIP_UNIT_POS_Type& o_val)
{
    o_val = i_target.getChipletNumber();
    return FAPI2_RC_SUCCESS;
}
}

#endif //__VIRTUAL_FUNCTIONS_H
