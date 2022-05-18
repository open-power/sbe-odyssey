/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_cmdtable_interpreter.C $ */
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
/// @file  ody_cmdtable_interpreter.C
/// @brief Interpet and execute command table binary image
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_cmdtable_interpreter.H>
#include <poz_perv_common_params.H>
#include <poz_cmdtable_interpreter.H>

using namespace fapi2;

ReturnCode ody_cmdtable_interpreter(
    const Target<TARGET_TYPE_OCMB_CHIP>& i_target,
    const void* i_main_table, const size_t i_main_table_size,
    const void* i_cust_table, const size_t i_cust_table_size)
{
    FAPI_INF("Entering ...");
    FAPI_TRY(poz_cmdtable_interpreter(
                 i_target,
                 i_main_table, i_main_table_size,
                 i_cust_table, i_cust_table_size));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
