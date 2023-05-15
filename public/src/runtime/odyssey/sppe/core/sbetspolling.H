/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspolling.H $         */
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

#define THERMAL_SENSOR_POLLING_INITIAL_PERIOD_MICROSEC 2*1000*1000

/**
 * @brief sbepollTSnDQS: Poll thermal sensor and DQS.
 *
 * @param[in] io_tspperiod Thermal sensor polling period in ms. This will
 *                         be default to 0 and willbe set in
 *                         ody_thermal_init.
 * @prarm[in] io_dqscount   DQS polling count. This will be in multiple
 *                         of io_tspperiod
 *
 * @return   Success, otherwise failure in case of any error.
 */

uint32_t sbepollTSnDQS(uint32_t &io_tspperiod, uint8_t &io_dqscount);
