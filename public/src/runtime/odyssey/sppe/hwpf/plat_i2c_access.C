/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_i2c_access.C $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

namespace fapi2
{

ReturnCode plati2c::populatei2cdetails(const Target<TARGET_TYPE_ALL>& target)
{
    Target<TARGET_TYPE_TEMP_SENSOR>  l_temp_target;
    FAPI_TRY(target.reduceType(l_temp_target));

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_I2C_DEV_ADDR,
                  l_temp_target,
                  iv_devAddr));

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_I2C_ENGINE,
                l_temp_target,
                iv_engine));

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_I2C_PORT,
                l_temp_target,
                iv_port));

    iv_bus_speed = I2C_BUS_SPEED;

    SBE_DEBUG("OMI Freq 0x%08x MHz", fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_FREQ_OMI_MHZ);

    iv_bit_rate_divisor = i2cGetBitRateDivisor(iv_bus_speed,
                            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_FREQ_OMI_MHZ/128);

    iv_polling_interval_ns = i2cGetPollingInterval(I2C_BUS_SPEED);

    iv_timeout_count = I2C_TIMEOUT_COUNT(iv_polling_interval_ns);

    FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_SPPE_I2C_MAX_RETRY_COUNT,
                        l_temp_target,
                        iv_max_retry_count));

    printi2cdetails();

    fapi_try_exit:
        return current_err;
}

ReturnCode plati2c::getI2c( const Target<TARGET_TYPE_ALL>& target,
                            const size_t get_size,
                            const std::vector<uint8_t>& cfgData,
                            std::vector<uint8_t>& o_data )
{
    #define SBE_FUNC " getI2c "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;
    do{
        //We just need to populate i2c details once
        rc = populatei2cdetails(target);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for populatei2cdetails with rc 0x%08X", rc);
            break;
        }

        do{
            rc = i2cLockEngine();
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cLockEngine with rc 0x%08X", rc);
                break;
            }

            SBE_DEBUG(SBE_FUNC "Current retry count 0x%02x", iv_curr_retry_count);
            rc = getI2cHelper(get_size, cfgData, o_data);
            //Lets exit out if there were no i2c fails
            if (rc == FAPI2_RC_SUCCESS)
            {
                break;
            }
            else
            {
                // if getI2cHelper failed try to reset
                ReturnCode l_rc = i2cReset();
                if(l_rc != FAPI2_RC_SUCCESS)
                {
                    break;
                }
            }
            iv_curr_retry_count++;

        }while(iv_curr_retry_count <= iv_max_retry_count);
    }while(false);
    ReturnCode l_rc = i2cUnlockEngine();
    if(l_rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC " failed for i2cUnlockEngine with l_rc 0x%08X", l_rc);
    }
    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

ReturnCode plati2c::putI2c( const Target<TARGET_TYPE_ALL>& target,
                            const std::vector<uint8_t>& data )
{
    #define SBE_FUNC " putI2c "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;

    do{
        //We just need to populate i2c details once
        rc = populatei2cdetails(target);
        if(rc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC " failed for populatei2cdetails with rc 0x%08X", rc);
            break;
        }

        do{
            rc = i2cLockEngine();
            if(rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " failed for i2cLockEngine with rc 0x%08X", rc);
                break;
            }

            SBE_DEBUG(SBE_FUNC "Current retry count 0x%02x", iv_curr_retry_count);
            rc = putI2cHelper(data);
            //Lets exit out if there were no i2c fails
            if (rc == FAPI2_RC_SUCCESS)
            {
                break;
            }
            else
            {
                // if putI2cHelper failed
                // try to reset
                ReturnCode l_rc = i2cReset();
                if(l_rc != FAPI2_RC_SUCCESS)
                {
                    break;
                }
            }
            iv_curr_retry_count++;

        }while(iv_curr_retry_count <= iv_max_retry_count);
    }while(false);
    ReturnCode l_rc = i2cUnlockEngine();
    if(l_rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC " failed for i2cUnlockEngine with l_rc 0x%08X", l_rc);
    }
    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

} //namespace fapi2
