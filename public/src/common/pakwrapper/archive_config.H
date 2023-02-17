/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/pakwrapper/archive_config.H $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

// __PPE42__ is emitted by the PPE compiler
// __PPE__ is defined in SBE code


#include "ppe42_string.h"

#define __SBE_PPE__
#define __USE_SHA3__

#ifndef __SROM_IMAGE__
    #define __USE_COMPRESSION__
    #define __USE_HWP_STREAM__
#endif

#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sha3.H"

#define ARC_ERROR(...) SBE_ERROR(__VA_ARGS__)
#define ARC_ERROR_BIN(...) SBE_ERROR_BIN(__VA_ARGS__)

typedef int ARC_RET_t;

enum ARCHIVE_RETURN_CODES
{
    ARC_OPERATION_SUCCESSFUL = SBE_SEC_OPERATION_SUCCESSFUL,
    ARC_INVALID_PARAMS = SBE_SEC_INVALID_PARAMS,
    ARC_FUNCTIONALITY_NOT_SUPPORTED = SBE_SEC_FUNCTIONALITY_NOT_SUPPORTED,
    ARC_INPUT_BUFFER_OVERFLOW = SBE_SEC_INPUT_BUFFER_OVERFLOW,
    ARC_FILE_CORRUPTED = SBE_SEC_FILE_CORRUPTED,
    ARC_FILE_NOT_FOUND = SBE_SEC_FILE_NOT_FOUND,
};

/// @brief Convert an arc_ret_t into a status code that fits into 4 bits
constexpr uint8_t compressed_arc_return_code(int i_arc_rc)
{
    return
        (i_arc_rc == ARC_OPERATION_SUCCESSFUL) ? 0 :
        (i_arc_rc == ARC_INVALID_PARAMS) ? 1 :
        (i_arc_rc == ARC_FUNCTIONALITY_NOT_SUPPORTED) ? 2 :
        (i_arc_rc == ARC_INPUT_BUFFER_OVERFLOW) ? 3 :
        (i_arc_rc == ARC_FILE_CORRUPTED) ? 4 :
        (i_arc_rc == ARC_FILE_NOT_FOUND) ? 5 :
        0xF;
}
