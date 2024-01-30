/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/imgcustomize.H $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include <stdint.h>

/**
 * @brief This file contains wrapper functions that differin implementation for
 *        SPPE pibmem image and PNOR image.
 *        The PNOR implementation is public while SPPE pibmem image implementation
 *        is lab only and can be found in internal
 *
 */

/**
 * @brief Wrapper function to perform SBE security boot checks
 *
 */
void sbeSecurityCheckWrap(void);

/**
 * @brief Wrapper function to set start offset to search files
 *        in PNOR inacse of PNOR image or
 *        in HEAP incase of SPPE pibmem image
 *
 */
void sbePakSearchStartOffset(void);

/**
 * @brief Wrapper function to populate metadata in runtime image
 *
 */
void sbeRuntimePopulateMetadataWrap(uint32_t i_metadata_ptr);

