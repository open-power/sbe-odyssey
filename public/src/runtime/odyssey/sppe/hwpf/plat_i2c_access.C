/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_i2c_access.C $      */
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

#include "plat_i2c_access.H"

/**
 * @brief I2C bus speed in KHz
 */
#define I2C_BUS_SPEED 400

#define TEMP_SENSOR_ENGINE_NU I2C_ENGINE_B
#define TEMP_SENSOR_PORT_NU   I2C_PORT_0

namespace fapi2
{

ReturnCode plati2c::populatei2cdetails(const Target<TARGET_TYPE_ALL>& target)
{
    //TODO: PFSBE-253 Read the from attributes port and Engine for i2c init.
    iv_port = TEMP_SENSOR_PORT_NU;

    iv_engine = TEMP_SENSOR_ENGINE_NU;

    Target<TARGET_TYPE_TEMP_SENSOR>  l_temp_target;
    target.reduceType(l_temp_target);
    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_I2C_DEV_ADDR,
                  l_temp_target,
                  iv_devAddr));

    iv_bus_speed = I2C_BUS_SPEED;

    SBE_INFO("OMI Freq 0x%08x MHz", fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_FREQ_OMI_MHZ);

    iv_bit_rate_divisor = i2cGetBitRateDivisor(iv_bus_speed,
                            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_FREQ_OMI_MHZ/128);

    iv_polling_interval_ns = i2cGetPollingInterval(I2C_BUS_SPEED);

    iv_timeout_count = I2C_TIMEOUT_COUNT(iv_polling_interval_ns);

    printi2cdetails();

    fapi_try_exit:
        return current_err;
}

} //namespace fapi2