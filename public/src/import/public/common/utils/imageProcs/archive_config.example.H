/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/utils/imageProcs/archive_config.example.H $ */
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

// -------------------------------------------------------------------------
// archive_config.H must be provided by each platform using the archive code
// to customize it for the platform. Don't you worry, it's easy!
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
// Step 1: Include whatever provides memcpy() on your platform.
// The default below is probably good enough for you.
// -------------------------------------------------------------------------

// __PPE42__ is emitted by the PPE compiler
// __PPE__ is defined in SBE code
#ifdef __PPE42__
    #include "ppe42_string.h"
#else
    #include <string.h>
#endif


// -------------------------------------------------------------------------
// Step 2: Choose your features, trading features for small code.
// -------------------------------------------------------------------------

// Define this if your compiler of choice supports pure virtual methods
#define __SBE_PPE__

// Support for compressed files
#define __USE_COMPRESSION__

// On-the-fly hashing support
#define __USE_SHA3__

// Support for streaming data through a hwp_data_stream
#define __USE_HWP_STREAM__


// -------------------------------------------------------------------------
// Step 3: Provide sha3_t. The default should be good enough again.
// -------------------------------------------------------------------------

#ifdef __USE_SHA3__
    #include "sha3.H"
#else
    typedef void sha3_t;
#endif


// -------------------------------------------------------------------------
// Step 4: Provide ARC_ERROR and ARC_ERROR_BIN.
// -------------------------------------------------------------------------

#include "sbetrace.H"
#include "sbe_sp_intf.H"
#define ARC_ERROR(...) SBE_ERROR(__VA_ARGS__)
#define ARC_ERROR_BIN(...) SBE_ERROR_BIN(__VA_ARGS__)


// -------------------------------------------------------------------------
// Step 5: Provide error codes.
// By providing your own enums you can map the ARC_xxx error codes to
// your platform's native error codes and save an annoying conversion
// step.
// -------------------------------------------------------------------------

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
