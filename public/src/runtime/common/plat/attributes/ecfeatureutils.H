/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/attributes/ecfeatureutils.H $  */
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

#include <fapi2_target.H>
#include <target.H>
#include <fapi2_chip_ec_feature.H>
#include <fapi2_error_scope.H>

namespace fapi2
{

ReturnCode queryChipEcAndName(
    const Target < fapi2::TARGET_TYPE_ALL>& i_target,
    fapi2::ATTR_NAME_Type& o_chipName,
    fapi2::ATTR_EC_Type& o_chipEc );

template<int T>
ReturnCode queryChipEcFeature(
    int2Type<T> id,
    const Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint8_t & o_hasFeature)
{
    fapi2::ATTR_NAME_Type l_chipName;
    fapi2::ATTR_EC_Type l_chipEc;

    o_hasFeature = 0;

    FAPI_TRY(queryChipEcAndName(i_target, l_chipName, l_chipEc), "queryChipEcAndName failed");
    o_hasFeature = hasFeature(int2Type<T>(), l_chipName, l_chipEc);


fapi_try_exit:
    FAPI_INF("Exiting ...");
    return fapi2::current_err;
}

inline ReturnCode queryChipConstTrue(
    const Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint8_t & o_hasFeature)
{
    o_hasFeature = true;
    return FAPI2_RC_SUCCESS;
}

inline ReturnCode queryChipConstFalse(
    const Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    uint8_t & o_hasFeature)
{
    o_hasFeature = false;
    return FAPI2_RC_SUCCESS;
}

}
