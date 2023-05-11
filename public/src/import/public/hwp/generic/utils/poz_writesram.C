/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_writesram.C $  */
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
///
/// @file poz_writesram.C
/// @brief Write data to chip SRAM
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: HB, Cronus, SBE
///

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <poz_writesram.H>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

/////////////////////////////////////////////////
/// poz_writesram
/////////////////////////////////////////////////
fapi2::ReturnCode poz_writesram(const fapi2::Target <
                                fapi2::TARGET_TYPE_OCMB_CHIP |
                                fapi2::TARGET_TYPE_COMPUTE_CHIP |
                                fapi2::TARGET_TYPE_TBUSC |
                                fapi2::TARGET_TYPE_IOHS |
                                fapi2::TARGET_TYPE_MC |
                                fapi2::TARGET_TYPE_PERV |
                                fapi2::TARGET_TYPE_MULTICAST > & i_target,
                                const uint32_t i_controlReg,
                                const uint32_t i_dataReg,
                                const uint32_t i_bytes,
                                const uint8_t* i_data)
{
    FAPI_DBG("Start");
    fapi2::buffer<uint64_t> l_data64 = 0;
    const uint8_t* l_dataPtr = i_data;

    // Enable auto increment
    if (i_bytes > 8)
    {
        FAPI_DBG("Enable auto-increment.");
        l_data64.setBit<0>();
        FAPI_TRY(fapi2::putScom(i_target, i_controlReg, l_data64));
    }

    // Write data
    while (l_dataPtr < (i_data + i_bytes))
    {
        l_data64 = 0;

        // Load 8 bytes into 64-bit word
        for (uint8_t ii = 0; ii < 8; ii++)
        {
            l_data64 |= ( static_cast<uint64_t>(*l_dataPtr++) << (56 - (8 * ii)) );

            // Exit if size has reached. Remaining data in double words are zeroes
            if (l_dataPtr >= i_data + i_bytes)
            {
                break;
            }
        }

        // For debug
        // FAPI_DBG("Write data: l_dataPtr %p, l_data64 = 0x%.16llX", l_dataPtr - 1, l_data64);
        FAPI_TRY(fapi2::putScom(i_target, i_dataReg, l_data64),
                 "Error from putScom");
    }

    // Disable auto-increment
    if (i_bytes > 8)
    {
        FAPI_DBG("Disable auto-increment.");
        l_data64.clearBit<0>();
        FAPI_TRY(fapi2::putScom(i_target, i_controlReg, l_data64));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
