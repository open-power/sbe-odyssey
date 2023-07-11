/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/istepIplUtils.C $              */
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

#include "istepIplUtils.H"
#include "sbetrace.H"
#include "ppe42_scom.h"

/**
 * @brief Macro to check if there are any parity errors during
 *        SPI operation via spi controller status register.
 *        Bits 32-42 in spi controller status register indicate
 *        if there were any parity errors.
 *
 */
#define SPI_PARITY_CHECK_MASK 0x00000000FFE00000

using namespace fapi2;

istepIplUtils& istepIplUtils::getInstance(uint32_t i_spiStatusRegAddr,
                      uint8_t i_istepStartMinorNumber)
{
    static istepIplUtils iv_instance(i_spiStatusRegAddr, i_istepStartMinorNumber);
    return iv_instance;
}

bool istepIplUtils::isSpiParityError()
{
    #define SBE_FUNC " isSpiParityError "
    SBE_ENTER(SBE_FUNC)

    bool status = false;
    uint64_t data = 0;

    // Load the SPI Status Register Here
    PPE_LVD(iv_spiStatusRegAdr, data);

    if(data & SPI_PARITY_CHECK_MASK)
    {
        SBE_ERROR(SBE_FUNC "SPI Parity Error observed");
        status = true;
    }

    SBE_EXIT(SBE_FUNC)
    return status;
    #undef SBE_FUNC
}