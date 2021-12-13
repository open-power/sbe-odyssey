/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/ppetrace/ppetracepp/ppeOpCodes.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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
 * @file ppeOpCodes.C
 * Pick up op codes from Hcode source
 */
#include <stdint.h>
#include <string.h>
#include "ppeOpCodes.H"

typedef uint32_t u32;
typedef uint8_t u8;

#include "jhash.h"


#define __PPE2FSP__
#include "stdio.h"

// TODO need a way to put opecode files in config file
#if !defined(PPETRACE_OPCODES_DISABLED)
    #include "../../../../procedures/ppe/pgpe/pgpe_opCodes.C"
#endif

/**
 * Convert format strings to hash ids
 */
extern "C"
{
    unsigned int trace_ppe_hash(const char* i_fmt, int i_prefix)
    {
        unsigned int prefix = (unsigned int)i_prefix << 16;
        unsigned int salt = -1;
        uint32_t hash = jhash(i_fmt, strlen(i_fmt), salt);

        return (hash & 0xffff) | prefix;
    }
}

void getOpCodeTables(map<std::string, mark_hash_t*>& i_markHash)
{
    // get all tables
#if !defined(PPETRACE_OPCODES_DISABLED)
    pgpe_opCodeTable(i_markHash);
#endif
}
