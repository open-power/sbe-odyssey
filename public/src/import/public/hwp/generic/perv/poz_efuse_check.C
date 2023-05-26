/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_efuse_check.C $ */
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

#include <fapi2.H>

#include "poz_perv_common_params.H"
#include "poz_efuse_common.H"

#include <poz_perv_utils.H>
#include "poz_efuse_check.H"

#include "endian.h"


using namespace fapi2;


ReturnCode poz_efuse_check(const Target<TARGET_TYPE_PROC_CHIP>& i_target_chip,
                           const uint8_t* i_buffer, uint32_t i_startBit, uint32_t i_endBit)
{
    fapi2::buffer<uint64_t> data64;
    fapi2::buffer<uint64_t> modeReg;
    fapi2::buffer<uint64_t> statusReg;

    const uint32_t firstWord = i_startBit / 64;
    const uint32_t nwords = (i_endBit - i_startBit) / 64;
    const uint64_t* buf64 = reinterpret_cast<const uint64_t*>(i_buffer);

    bool ecc = true;

    FAPI_TRY(check_alignment(i_startBit, i_endBit));
    FAPI_TRY(check_prog_range(i_startBit, i_endBit, ecc));

    modeReg.clearBit<0>();
    modeReg.setBit<1>();
    modeReg.clearBit<2>();
    modeReg.clearBit<3>();
    modeReg.insertFromRight<16, 16>(0);

    FAPI_TRY(putScom(i_target_chip, OTPROM_MODE_REG_ADDR, modeReg));

    statusReg.flush<0>();
    FAPI_TRY(putScom(i_target_chip, OTPROM_STATUS_REG_ADDR, statusReg));

    for (uint32_t i = 0; i < nwords; i++)
    {

        FAPI_TRY(getScom(i_target_chip, OTPROM_MEMORY_MAPPED_OFFSET + firstWord + i, data64));

        const uint64_t expect64 = be64toh(buf64[i]);

        FAPI_ASSERT(data64 == expect64, EFUSE_PROGRAMMING_MISMATCH_ERROR(),
                    "HWP: data read vs expected: 0x%08X%08X 0x%08X%08X", data64 >> 32,
                    data64 & 0xFFFFFFFF, expect64 >> 32, expect64 & 0xFFFFFFFF);

        FAPI_TRY(getScom(i_target_chip, OTPROM_STATUS_REG_ADDR, statusReg));
        FAPI_ASSERT(!statusReg.getBit<4>() && !statusReg.getBit<5>(), EFUSE_PROGRAMMING_ECC_ERROR(),
                    "HWP: ECC error (correctable: %d, uncorrectable %d) from status register: 0x%08X%08X", statusReg.getBit<4>(),
                    statusReg.getBit<5>(), (uint64_t(statusReg) >> 32), static_cast<uint32_t>(uint64_t(statusReg) & 0xFFFFFFFF));
    }


fapi_try_exit:
    return current_err;

}
