/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspolling.H $         */
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

#include <stdint.h>
#include "fapi2.H"

/**
 * @brief sbepollTSnDQS: Poll thermal sensor and DQS.
 *
 * @param[in,out] io_dqscurrentcount   DQS polling current count.
 *
 * @return   Success, otherwise failure in case of any error.
 */

uint32_t sbepollTSnDQS(uint8_t &io_dqscurrentcount);

/**
 * @brief This API updates the TS Polling period to latest value.
 *        It also converts it to micro seconds.
 *        If there is any change in the polling period it also resets the current DQS counter value
 *
 * @param[in,out] io_currentTsPeriodInMicroSec Current value of TS Period in Micro sec
 *
 * @param[in,out] io_dqscurrentcount Current count of DQS
 *
 */
void getTsPeriodInMicroSec(uint32_t &io_currentTsPeriodInMicroSec,
                           fapi2::ATTR_ODY_DQS_TRACKING_PERIOD_Type &io_dqscurrentcount);
