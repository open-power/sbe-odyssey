/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/ecfeatureutils.H $  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2022                        */
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

#ifndef __EC_FEATURE_UTILS_H
#define __EC_FEATURE_UTILS_H

#include <attribute_ids.H>
#include <virtualattrfunctions.H>
#include <ec_features.H>

namespace fapi2
{

ReturnCode queryChipEcAndName(
                    const Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
                    fapi2::ATTR_NAME_Type& , fapi2::ATTR_EC_Type & );

template<int T>
AttributeRC queryChipEcFeature(int2Type<T> id,
                    const Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
                    uint8_t & o_hasFeature)
{
    fapi2::ATTR_NAME_Type l_chipName;
    fapi2::ATTR_EC_Type l_chipEc;

    o_hasFeature = 0;

    queryChipEcAndName(i_target, l_chipName, l_chipEc);
    o_hasFeature = hasFeature(int2Type<T>(), l_chipName, l_chipEc);
    return FAPI2_RC_SUCCESS;
}

}
#endif //__EC_FEATURE_UTILS_H
