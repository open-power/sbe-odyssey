/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/exchwp/hwptable.H $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include <stddef.h>
#include "return_code.H"

using voidfuncptr_t = void (*)(void);
using hwp_t = fapi2::ReturnCode (*)( voidfuncptr_t );

struct hwpMap_t
{
    hwp_t         hwpWrapper;
    voidfuncptr_t hwp;
};

struct hwpTableEntry_t
{
    uint8_t         hwpClassNum;
    hwpMap_t*       hwpNum;
    size_t          len;
};

struct hwpTable_t
{
    hwpTableEntry_t*    hwpClassArr;
    size_t              len;
};

#define HWP_MAP(wrapper, hwp) \
    {wrapper, reinterpret_cast<voidfuncptr_t>(hwp)}

#define HWP_ENTRY(hwpClass, hwpClassTable) \
    {hwpClass, hwpClassTable, sizeof(hwpClassTable)/sizeof(hwpClassTable[0])}

#define REGISTER_HWP_TABLE(entries) \
     hwpTable_t hwpTable = { \
       entries, \
       sizeof(entries)/sizeof(entries[0]) \
    };

extern hwpTable_t hwpTable;