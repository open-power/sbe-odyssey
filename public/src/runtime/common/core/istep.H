/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/istep.H $                      */
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

#include <stdint.h>
#include "fapi2.H"

class istep {

public:

    /**
     * @brief Function to validate if given istep is allowed or not
     *        If there are any skips in istep and security is enabled
     *        istep skipping is not allowed.
     *
     * @param i_major Major Istep Number to be validated
     * @param i_minor Minor Istep Number to be validated
     * @return true if major.minor istep to be executed is allowed
     * @return false if major.minor istep to be executed is not allowed
     */
    bool validateIstep(const uint8_t i_major, const uint8_t i_minor);

    /**
     * @brief Support function to execute specific istep
     *
     * @param[in] i_major Major Istep Number
     * @param[in] i_minor Minor Istep Number
     *
     * @note It is the responsibilty of caller to verify major/minor
     *       number before calling this function
     *
     * @return FAPI2_RC_SUCCESS if success, else error code.
     */
    fapi2::ReturnCode sbeExecuteIstep (uint8_t i_major, uint8_t i_minor);

protected:

    uint8_t iv_istepStartMinorNumber;

    /**
     * @brief Construct a new istep object
     *
     * @param i_istepStartMinorNumber istep start minor number
     */
    istep(uint8_t i_istepStartMinorNumber) :
      iv_istepStartMinorNumber(i_istepStartMinorNumber)
      {};

private:

    /**
     * @brief isSpiParityError() Check for parity error while
     *        accessing data via SPI from PNOR/SEEPROM
     *
     * @return true if there is a parity error
     * @return false if there is no parity error
     */
    virtual bool isSpiParityError()
    = 0;

    /**
     * @brief This API finds the next istep major.minor that
     *        needs to be executed
     *
     * @param o_nextMajor Next major number of istep to be executed
     * @param o_nextMinor Next minor number of istep to be executed
     */
    void findNextIstep(uint8_t& o_nextMajor, uint8_t& o_nextMinor);

    /**
     * @brief This api checks if i_major.i_minor are within the
     *        limits of the defined ipltable
     *
     * @param i_major Major number of istep to be executed
     * @param i_minor Minor number of istep to be executed
     * @return true   if i_major.i_minor within defined ipltable
     * @return false  if i_major.i_minor not within defined ipltable
     */
    bool validateIstepRange(const uint8_t i_major, const uint8_t i_minor);

}; //class istep

/**
  * @brief structure for execute istep chipop (0xA101) contents.
  *
*/
typedef struct
{
    uint32_t reserved1:8;
    uint32_t major:8;
    uint32_t reserved2:8;
    uint32_t minor:8;
}sbeIstepReqMsg_t;

/**
  * @brief sbeHandleIstep : Executes istep
  *
  * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
  *
  * @return    Rc from the FIFO access utility
  */
uint32_t sbeHandleIstep(uint8_t *i_pArg);
