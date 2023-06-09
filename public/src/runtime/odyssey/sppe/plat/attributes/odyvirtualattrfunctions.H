/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/odyvirtualattrfunctions.H $ */
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

#ifndef __ODY_VIRTUAL_FUNCTIONS_H
#define __ODY_VIRTUAL_FUNCTIONS_H

#include <virtualattrfunctions.H>
// #include <hw_access.H>

namespace fapi2
{

/**
  * @brief read the chip name of the target
  *
  * Return the name of the chip. In Odyssey SPPE image this will always return
  *     fapi2::ENUM_ATTR_NAME_ODYSSEY
  *
  * @param[in]  i_target input target
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<TargetType K>
inline
AttributeRC get_ATTR_NAME(const Target<K>& i_target,
        ATTR_NAME_Type& o_val)
{
    o_val = fapi2::ENUM_ATTR_NAME_ODYSSEY;
fapi_try_exit:
    return FAPI2_RC_SUCCESS;
}

/**
  * @brief read the EC level of the chip
  *
  * Return the EC level of the chip.
  *
  * @param[in]  i_target input target
  * @param[out] o_val    output parameter
  *
  * @return   returncode
  */
template<TargetType K>
inline
AttributeRC get_ATTR_EC(const Target<K>& i_target,
        ATTR_EC_Type& o_val)
{
    // TODO: Uncomment this after plat code is ready for getscom
    /*
    fapi2::buffer<uint64_t> cbs_envstat_reg = 0;
    fapi2::getScom(i_target, scomt::poz::FSXCOMP_FSXLOG_CBS_ENVSTAT_RO,
                             cbs_envstat_reg);

    o_val = ((cbs_envstat_reg >> 32) & 0xFF);
    */
    o_val = 0x10;
    return FAPI2_RC_SUCCESS;
}
}

#endif //__ODY_VIRTUAL_FUNCTIONS_H
