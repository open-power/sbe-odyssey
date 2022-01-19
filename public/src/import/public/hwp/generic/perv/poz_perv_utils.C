/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_utils.C $  */
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
/// @file poz_perv_utils.C
/// @brief utility functions support for perv HWP's
//------------------------------------------------------------------------------
/// *HWP HW Maintainer: Sreekanth Reddy <skadapal@in.ibm.com>
//------------------------------------------------------------------------------

#include <poz_perv_utils.H>
#include <target_filters.H>
#include <assert.h>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
using namespace fapi2;

Target<TARGET_TYPE_PERV> get_tp_chiplet_target(const Target<TARGET_TYPE_CHIPS> i_chip)
{
    auto l_children = i_chip.getChildren<TARGET_TYPE_PERV>(TARGET_FILTER_TP, TARGET_STATE_PRESENT);
    // The TP chiplet should always be present so just use
    // assert() instead of FAPI_ASSERT() as a sanity check.
    assert(!l_children.empty());
    return l_children[0];
}
