/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_efuse_program.C $ */
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
#include "poz_efuse_program.H"
#include "ibmecc.H"

using namespace fapi2;

ReturnCode write_efuse_bit(const Target<TARGET_TYPE_PROC_CHIP>& i_target_chip, uint32_t i_bit)
{

    fapi2::buffer<uint64_t> prgmReg;
    fapi2::buffer<uint64_t> statusReg;
    uint32_t l_timeout = 10;
    prgmReg.flush<0>();

    prgmReg.insertFromRight<6, 32>(i_bit);
    FAPI_TRY(putScom(i_target_chip, OTPROM_PRGM_REG_ADDR, prgmReg));

    while (l_timeout)
    {
        FAPI_TRY(getScom(i_target_chip, OTPROM_STATUS_REG_ADDR, statusReg));

        if (statusReg.getBit<44>())
        {
            break;
        }

        fapi2::delay(10000, 20000);
        l_timeout--;
    }

    FAPI_ASSERT(l_timeout > 0, EFUSE_PROGRAMMING_TIMEOUT_ERROR(),
                "Timeout reached when writing efuse bit, Status bit 44 not set");

fapi_try_exit:
    return current_err;
}


ReturnCode poz_efuse_program(const Target<TARGET_TYPE_PROC_CHIP>& i_target_chip,
                             const uint8_t* i_buffer, uint32_t i_startBit, uint32_t i_endBit)
{
    fapi2::buffer<uint64_t> data64;
    fapi2::buffer<uint64_t> modeReg;
    fapi2::buffer<uint64_t> statusReg;
    bool ecc = true;

    uint32_t l_otpromFreq = 50; //Set FSI clk to 50MHz to match with TAE uflex setup;
    //FAPI_TRY(FAPI_ATTR_GET(ATTR_OTPROM_CONTROL_FREQUENCY_IM_MHZ, i_target_chip, l_otpromFreq));
    //FAPI_INF("HWP: OTPROM controller's frequency in MHz: 0x%08lX", l_otpromFreq);

    FAPI_TRY(check_alignment(i_startBit, i_endBit));
    FAPI_TRY(check_prog_range(i_startBit, i_endBit, ecc));

    statusReg.flush<0>();
    FAPI_TRY(putScom(i_target_chip, OTPROM_STATUS_REG_ADDR, statusReg));
    // ToDo: set mailbox write protection
    FAPI_INF("HWP: First row & column to program: 0x%02X, %04d (overall bit position %06d)",
             i_startBit / OTPROM_COLUMNS,
             i_startBit % OTPROM_COLUMNS, i_startBit);
    FAPI_INF("HWP: Last  row & column to program: 0x%02X, %04d (overall bit position %06d)",
             (i_endBit - 1) / OTPROM_COLUMNS,
             (i_endBit - 1) % OTPROM_COLUMNS, (i_endBit - 1));

    FAPI_INF("HWP: Started programming");

    modeReg.clearBit<0>();
    modeReg.clearBit<1>();
    modeReg.setBit<2>();
    modeReg.setBit<3>();
    modeReg.insertFromRight<16, 16>(l_otpromFreq * 11);
    FAPI_TRY(putScom(i_target_chip, OTPROM_MODE_REG_ADDR, modeReg));
    FAPI_TRY(getScom(i_target_chip, OTPROM_STATUS_REG_ADDR, statusReg));

    for (uint32_t i = i_startBit; i < i_endBit; i++)
    {

        bool bitWrite = (bool)(i_buffer[(i - i_startBit) / 8] >> (7 - ((i - i_startBit) % 8))) & 1;

        if(bitWrite)
        {
            write_efuse_bit(i_target_chip, i);
        }

    }

    if (ecc)
    {
        for (uint32_t i = i_startBit; i < i_endBit; i += 64)
        {
            uint32_t eccByte = ecc64_gen(&i_buffer[(i - i_startBit) / 8]);
            FAPI_INF("HWP: ECC byte for programming: %x", eccByte);

            for (uint32_t j = 0; j < 8; j++)
            {

                bool bitWrite = (bool)((eccByte >> (7 - j)) & 1);

                if(bitWrite)
                {
                    write_efuse_bit(i_target_chip, ((0x1C00 + i / 8 + j) ^ 0x20));
                }

            }
        }
    }

    FAPI_INF("HWP: proc_sbe_program_efuse_bits: Exiting ...");

    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    FAPI_INF("HWP: Programming efuse has failed!");
    return current_err;

}
