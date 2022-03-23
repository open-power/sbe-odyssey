/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_nest_enable_io.H $ */
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
//------------------------------------------------------------------------------
/// @file poz_nest_enable_io.H
///
/// @brief
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

typedef fapi2::ReturnCode (*poz_nest_enable_io_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
        fapi2::TargetFilter);

/// @brief
/// @param[in]   i_target         Reference to TARGET_TYPE_ANY_POZ_CHIP
/// @param[in]   i_nest_chiplets  nest targets TARGET_FILTER_[N0/N1/N2]
/// @return  FAPI2_RC_SUCCESS if success, else error code.
extern "C"
{
    fapi2::ReturnCode poz_nest_enable_io(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                                         fapi2::TargetFilter i_nest_chiplets);
}
