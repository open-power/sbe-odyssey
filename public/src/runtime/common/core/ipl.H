/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/ipl.H $                        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2023                        */
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

#include "ipltable.H"

/**
 * @brief ipl class is a abstract class which
 *        can be used for ipling SBE
 *
 */
class ipl {

public:

    /**
     * @brief Executes IPL steps in auto mode.
     * @par run istep table continuously
     */
    void sbeAutoBoot();

protected:

private:

    /**
     * @brief isSpiParityError() Check for parity error while
     *        accessing data via SPI from PNOR/SEEPROM
     *
     * @return true if there is a parity error
     * @return false if there is no parity error
     */
    virtual bool isSpiParityError() = 0;

}; //class ipl
