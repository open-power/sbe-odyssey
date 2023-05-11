/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspollctrl.H $        */
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

/**
 * @brief Control Thermal Sensor And DQS polling message header
 */
typedef struct
{
    uint32_t tempSensorPollIntervalMs; // Temp Sensor Polling Interval in Milliseconds
    uint32_t reserved:24;
    uint32_t dqsInterval:8;            // DQS Polling in multiple of tempSensorPollIntervalMs

    /**
     * @brief Validate the input parameters.
     *        There can be no case where temperature sensor polling is enabled but DQS is disabled
     *        and vise versa.
     *
     * @return true only if both TS Polling and DQS is enabled or disabled
     * @return false if either is enabled and later is disabled and vise versa
     */
    bool validateInputParams()
    {
        bool status = true;

        if((tempSensorPollIntervalMs == 0x0 && dqsInterval != 0x0) ||
            (tempSensorPollIntervalMs != 0x0 && dqsInterval == 0x0))
                status = false;

        return status;
    }
}sbeCtrlTSandDQSpollReqMsg_t;

/**
 * @brief Control Thermal Sensor And DQS polling [0xAC 0x2]
 *        This chip-op will update ATTR_ODY_SENSOR_POLLING_PERIOD & ATTR_ODY_DQS_TRACKING_PERIOD attr's.
 *        If ATTR_ODY_SENSOR_POLLING_PERIOD = 0x0 , we will disable ATTR_ODY_DQS_TRACKING_PERIOD as well.
 *        If ATTR_ODY_DQS_TRACKING_PERIOD = 0x0 and ATTR_ODY_SENSOR_POLLING_PERIOD =! 0 , we will send
 *        invalid request and response.
 *
 * @param[in] i_pArg Buffer to be passed to the function (not used as of now)
 *
 * @return    RC from the FIFO access utility
 */
uint32_t sbectrltspoll(uint8_t *i_pArg);