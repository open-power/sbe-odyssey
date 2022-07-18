/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_readsram.C $   */
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
///
/// @file poz_readsram.C
/// @brief Read data from chip SRAM
///
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: HB, Cronus, SBE
///

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <poz_readsram.H>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

/////////////////////////////////////////////////
// poz_readsram
/////////////////////////////////////////////////
fapi2::ReturnCode poz_readsram(const fapi2::Target <
                               fapi2::TARGET_TYPE_OCMB_CHIP |
                               fapi2::TARGET_TYPE_COMPUTE_CHIP |
                               fapi2::TARGET_TYPE_TBUSC |
                               fapi2::TARGET_TYPE_IOHS |
                               fapi2::TARGET_TYPE_MC > & i_target,
                               const uint32_t i_controlReg,
                               const uint32_t i_dataReg,
                               const uint32_t i_bytes,
                               uint8_t* o_data)
{
    FAPI_DBG("Start");
    fapi2::buffer<uint64_t> l_data64 = 0;
    uint8_t* l_dataPtr = o_data;

    // Enable auto increment
    if (i_bytes > 8)
    {
        FAPI_DBG("Enable auto-increment.");
        l_data64.setBit<0>();
        FAPI_TRY(fapi2::putScom(i_target, i_controlReg, l_data64));
    }

    // Read data
    while (l_dataPtr < (o_data + i_bytes))
    {
        // Read 8 bytes at a time
        FAPI_TRY(fapi2::getScom(i_target, i_dataReg, l_data64),
                 "Error from getScom");

        // Store data into output buffer
        for (uint8_t ii = 0; ii < 8; ii++)
        {
            *l_dataPtr++ = (l_data64 >> (56 - (ii * 8))) & 0xFFull;

            // Exit if size has reached
            if (l_dataPtr >= o_data + i_bytes)
            {
                break;
            }
        }

        FAPI_DBG("Read data: l_dataPtr %p, l_data64 = 0x%.16llX", l_dataPtr - 1, l_data64);
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
