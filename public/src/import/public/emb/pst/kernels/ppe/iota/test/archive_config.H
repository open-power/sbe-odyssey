/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/iota/test/archive_config.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#include "ppe42_string.h"
#include "iota_trace.h"

// we do not define __SBE_PPE__
// we do not define __USE_COMPRESSION__
// we do not define __USE_SHA3__
// we do not define __USE_HWP_STREAM__

typedef void sha3_t;
typedef uint32_t trace_hash_val;
#define ARC_ERROR(...) PK_TRACE(__VA_ARGS__)
#define ARC_ERROR_BIN(...)

typedef int ARC_RET_t;

enum ARCHIVE_RETURN_CODES
{
    ARC_OPERATION_SUCCESSFUL,
    ARC_INVALID_PARAMS,
    ARC_FUNCTIONALITY_NOT_SUPPORTED,
    ARC_INPUT_BUFFER_OVERFLOW,
    ARC_FILE_CORRUPTED,
    ARC_FILE_NOT_FOUND,
};
