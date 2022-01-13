/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive_config.H $ */
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
#pragma once

// __PPE42__ is emitted by the PPE compiler
// __PPE__ is defined in SBE code

#ifdef __PPE42__
    #include "ppe42_string.h"
#else
    #include <string.h>
#endif


#if defined(__PPE__) && !defined(__PPE_QME)

#define __SBE_PPE__
#define __USE_COMPRESSION__
#define __USE_SHA3__
#define __USE_HWP_STREAM__

#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "crypto_rom.H"

#define ARC_ERROR(...) SBE_ERROR(__VA_ARGS__)
#define ARC_ERROR_BIN(...) SBE_ERROR_BIN(__VA_ARGS__)

typedef int ARC_RET_t;

enum ARCHIVE_RETURN_CODES
{
    ARC_OPERATION_SUCCESSFUL = SBE_SEC_OPERATION_SUCCESSFUL,
    ARC_FUNCTIONALITY_NOT_SUPPORTED = SBE_SEC_FUNCTIONALITY_NOT_SUPPORTED,
    ARC_INPUT_BUFFER_OVERFLOW = SBE_SEC_INPUT_BUFFER_OVERFLOW,
    ARC_FILE_CORRUPTED = SBE_SEC_FILE_CORRUPTED,
    ARC_FILE_NOT_FOUND = SBE_SEC_FILE_NOT_FOUND,
};

#else

#ifndef __PPE_QME

    #include "fapi2.H"

    #define ARC_ERROR(...) FAPI_ERR(__VA_ARGS__)
    #define ARC_ERROR_BIN(...)

    #define __USE_COMPRESSION__
    #include "sha3.H"

#else

    #include "iota_trace.h"
    typedef void sha3_t;
    typedef uint32_t trace_hash_val;
    #define ARC_ERROR(...) PK_TRACE(__VA_ARGS__)
    #define ARC_ERROR_BIN(...)

#endif

typedef int ARC_RET_t;

enum ARCHIVE_RETURN_CODES
{
    ARC_OPERATION_SUCCESSFUL,
    ARC_FUNCTIONALITY_NOT_SUPPORTED,
    ARC_INPUT_BUFFER_OVERFLOW,
    ARC_FILE_CORRUPTED,
    ARC_FILE_NOT_FOUND,
};

#endif
