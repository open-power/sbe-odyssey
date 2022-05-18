/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_cmdtable_interpreter.H $ */
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
/// @file  poz_cmdtable_interpreter.H
/// @brief Interpreter for boot ROM command tables
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer   : Kahn Evans <kahneva@us.ibm.com>
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>

typedef fapi2::ReturnCode (*poz_cmdtable_interpreter_FP_t)(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>&,
    const void*, const size_t,
    const void*, const size_t);

/// @brief Interpreter for boot ROM command tables
///
/// This executes the command tables from SEEPROM like the boot ROM would.
///
/// @param[in]   i_target                  The chip to operate on
/// @param[in]   i_main_table              Pointer to memory where the main command table resides
/// @param[in]   i_main_table_size         Size of the main command table in bytes
/// @param[in]   i_main_table              Pointer to memory where the custom command table resides,
///                                        or NULL if there is no custom table
/// @param[in]   i_main_table_size         Size of the custom command table in bytes, or 0 if no table
///
/// @return  FAPI2_RC_SUCCESS if the command table returned
/// @return  FAPI2_RC_FALSE   if command table execution failed and the SBE would have halted
/// @return  FAPI2_RC_INVALID_PARAMETER if either command table is malformed
/// @return  FAPI error code  if another FAPI error occurred, such as a SCOM error
extern "C"
{
    fapi2::ReturnCode poz_cmdtable_interpreter(
        const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
        const void* i_main_table, const size_t i_main_table_size,
        const void* i_cust_table, const size_t i_cust_table_size);
}
