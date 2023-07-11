/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/istepIplUtils.H $              */
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
#include "istep.H"
#include "ipl.H"

/**
 * @brief istepIplUtils is a multilevel inherited
 *        singleton class.
 *        This class contains both istep and ipl member
 *        functions inherited from istep and ipl class
 *
 *        It also has common member functions between istep
 *        and ipl class.
 */

class istepIplUtils : public istep, public ipl {

public:

    // Disable copy construction and assignment operators
    istepIplUtils(const istepIplUtils&) = delete;
    istepIplUtils& operator=(const istepIplUtils&) = delete;

    /**
     * @brief Returns the instance of this class
     *
     * @param i_spiStatusRegAddr SPI status reg from which parity
     *                           errors need to be checked
     * @param i_istepStartMinorNumber istep start minor number
     *
     * @return A reference to istepIplUtils
     */
    static istepIplUtils& getInstance(uint32_t i_spiStatusRegAddr,
                    uint8_t i_istepStartMinorNumber);

protected:

private:

    uint32_t iv_spiStatusRegAdr;

    /**
     * @brief Construct a new istepIplUtils object
     *
     * @param i_spiStatusRegAddr SPI status reg from which parity
     *                           errors need to be checked
     * @param i_istepStartMinorNumber istep start minor number
     */
    istepIplUtils(uint32_t i_spiStatusRegAddr,
                    uint8_t i_istepStartMinorNumber) :
        istep(i_istepStartMinorNumber),
        iv_spiStatusRegAdr(i_spiStatusRegAddr)
        {};

    /**
     * @brief isSpiParityError() Check for parity error while
     *        accessing data via SPI from PNOR/SEEPROM
     *
     * @return true if there is a parity error
     * @return false if there is no parity error
     */
    bool isSpiParityError();

}; //class istepIplUtils

extern istepIplUtils* g_pSbeIstepIplUtils;