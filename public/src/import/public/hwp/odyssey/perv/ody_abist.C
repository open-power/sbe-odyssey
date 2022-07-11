/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_abist.C $       */
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
/// @file  ody_abist.C
/// @brief Execute Array Built In Self Test
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include "ody_abist.H"
#include "poz_bist.H"

using namespace fapi2;

static const bist_params ody_abist_params =
{
    BIST_PARAMS_CURRENT_VERSION,     /// BIST_PARAMS_VERSION
    "-program-string",       /// Program
    "--ring-patch",           /// ring_patch
    0x3fffffffffffffff,              /// chiplets

    bist_params::ABIST_NOT_LBIST  |     /// 0x80000000
    bist_params::DO_SCAN0         |     /// 0x40000000
    bist_params::DO_ARRAYINIT     |     /// 0x20000000
    bist_params::DO_RING_SETUP    |     /// 0x10000000
    bist_params::DO_RING_PATCH    |     /// 0x08000000
    bist_params::DO_REG_SETUP     |     /// 0x04000000
    bist_params::DO_GO            |     /// 0x02000000
    bist_params::DO_POLL          |     /// 0x01000000
    bist_params::DO_REG_CLEANUP   |     /// 0x00800000
    bist_params::DO_COMPARE       |     /// 0x00400000
    bist_params::SCAN0_REPR       |     /// 0x00200000
    bist_params::SKIP_FIRST_CLOCK |     /// 0x00100000
    bist_params::SKIP_LAST_CLOCK  |     /// 0x00080000
    bist_params::UNICAST_GO       |     /// 0x00040000
    bist_params::POLL_ABIST_DONE,       /// 0x00020000

    0,                                ///  opcg_count
    1000,                             ///  idle_count
    10000,                            ///  timeout
    0,                                ///  linear_stagger
    0,                                ///  zigzag_stagger
    0x1234,                           ///  regions
};

enum ODY_ABIST_Private_Constants
{
};

ReturnCode ody_abist(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    FAPI_DBG("Entering ody_abist Function to Set Up the "
             "BIST_PARAMS Structure.");

    FAPI_DBG("Calling poz_bist with Chip Target and "
             "BIST_PARAMS Structure.");

    FAPI_TRY(poz_bist(i_target, ody_abist_params));

fapi_try_exit:
    return current_err;
}
