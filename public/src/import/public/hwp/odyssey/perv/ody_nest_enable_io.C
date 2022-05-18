/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_nest_enable_io.C $ */
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
/// @file  ody_nest_enable_io.C
/// @brief Enable chip/nest drivers and receivers
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_nest_enable_io.H>
#include <poz_perv_common_params.H>
#include <ody_scom_perv.H>

using namespace fapi2;
using namespace scomt::perv;

SCOMT_PERV_USE_CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_ROOT_CTRL1;
typedef CFAM_FSI_W_MAILBOX_FSXCOMP_FSXLOG_ROOT_CTRL1_t ROOT_CTRL1_t;

enum ODY_NEST_ENABLE_IO_Private_Constants
{
};

ReturnCode ody_nest_enable_io(const Target<TARGET_TYPE_OCMB_CHIP>& i_target)
{
    ROOT_CTRL1_t ROOT_CTRL1;

    FAPI_INF("Entering ...");

    FAPI_TRY(ROOT_CTRL1.getScom(i_target));
    ROOT_CTRL1.set_TP_RI_DC_B(1);
    ROOT_CTRL1.set_TP_DI1_DC_B(1);
    ROOT_CTRL1.set_TP_DI2_DC_B(1);
    FAPI_TRY(ROOT_CTRL1.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
