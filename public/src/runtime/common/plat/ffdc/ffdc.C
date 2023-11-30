/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/ffdc.C $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
/**
 *  @file ffdc.C
 *  @brief Implements the ffdc for sbe
 */

#include <ffdc.H>
#include <error_info.H>
#include <return_code.H>
#include "sbeffdctype.H"

namespace fapi2
{
#ifndef __noRC__
    ReturnCode current_err __attribute__ ((aligned (8)));
#endif

#ifdef MINIMUM_FFDC
    // buffer used to hold ffdc data
    pozFfdcData_t g_FfdcData;

    // Hold the FFDC data which mainly hold the first commited ffdc
    pozFfdcCtrl_t g_ffdcCtrlSingleton;
#endif

};
