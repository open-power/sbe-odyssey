/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_efuse_common.H $ */
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

#include <fapi2.H>

static const uint32_t OTPROM_COLUMNS               =      64;

static const uint32_t OTPROM_CONTROL_OFFSET        = 0x00010000;
static const uint32_t OTPROM_MEMORY_MAPPED_OFFSET  = 0x00018000;
static const uint32_t OTPROM_STATUS_REG_ADDR       = OTPROM_CONTROL_OFFSET +  2;
static const uint32_t OTPROM_MODE_REG_ADDR         = OTPROM_CONTROL_OFFSET +  8;
static const uint32_t OTPROM_PRGM_REG_ADDR         = OTPROM_CONTROL_OFFSET + 10;

static inline fapi2::ReturnCode check_alignment(uint32_t startBit, uint32_t endBit)
{

    FAPI_ASSERT( startBit  % 64 == 0
                 && endBit % 64 == 0,  fapi2::EFUSE_PROGRAMMING_ALIGNMENT_ERROR(),
                 "Need a proper alignment (mod 64 = 0): startBit: 0x%x, endBit: 0x%x", startBit, endBit);
fapi_try_exit:
    return fapi2::current_err;
}

static inline fapi2::ReturnCode check_prog_range(uint32_t startBit, uint32_t endBit, bool ecc)
{

    FAPI_ASSERT((startBit < 0x1C00 && endBit <= 0x1C00)
                || !ecc, fapi2::EFUSE_PROGRAMMING_RANGE_ERROR(),     "Need a startBit & endBit < 0x1800: startBit: 0x%x, endBit: 0x%x",
                startBit, endBit);

    FAPI_ASSERT((startBit < 0x2000 && endBit <= 0x2000)
                ||  ecc, fapi2::EFUSE_PROGRAMMING_RANGE_ERROR(),     "Need a startBit & endBit < 0x2000: startBit: 0x%x, endBit: 0x%x",
                startBit, endBit);

fapi_try_exit:
    return fapi2::current_err;
}
