/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_ppe_common.H $ */
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
/// @file  poz_ppe_common.H
/// @brief Basic common types and enums for PPE dump utils
///
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 2
/// *HWP Consumed by     : SBE, Cronus

#pragma once

enum PPE_TYPES
{
    PPE_TYPE_SBE     = 0,
    PPE_TYPE_SPPE    = 1,
    PPE_TYPE_PME     = 2,
    PPE_TYPE_GPE     = 3,
    PPE_TYPE_QME     = 4,
    PPE_TYPE_IO      = 5,
    PPE_TYPE_MAX
};

typedef struct
{
    uint16_t    number;
    uint32_t    value;
    const char*  name;
} Reg32Value_t;

typedef struct
{
    uint16_t    number;
    uint32_t    value;
} Reg32N_t;


typedef struct
{
    uint16_t    number;
    uint64_t    value;
    const char*  name;
} Reg64Value_t;

//Register Tables
typedef struct
{
    bool check_one;
    uint16_t startBit;
    uint16_t endBit;
    const char* description;
    std::vector<const char*> enumDescription;
} RegField_t;
