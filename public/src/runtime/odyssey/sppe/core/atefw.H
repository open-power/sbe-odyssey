/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/atefw.H $                */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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

#include "fapi2.H"
#include "hwptable.H"

/**
 * @brief Wrapper function to start the ATE PHY reset sequence
 * @param[in] i_hwp Function pointer to HWP
 * @retrun the fapi2::ReturnCode SUCCESS only if the procedure passed
 *
 */
fapi2::ReturnCode istepATEPHYReset(voidfuncptr_t i_hwp);

/**
 * @brief Wrapper function to run the ATE FW
 * @param[in] i_hwp Function pointer to HWP
 * @retrun the fapi2::ReturnCode SUCCESS only if the procedure passed
 *
 */
fapi2::ReturnCode istepATEFW(voidfuncptr_t i_hwp);
