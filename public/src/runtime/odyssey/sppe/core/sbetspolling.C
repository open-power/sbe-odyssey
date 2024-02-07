/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspolling.C $         */
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
#include "sbetspolling.H"
#include "ody_thermal_read.H"
#include "ody_dqs_track.H"

using namespace fapi2;

void getTsPeriodInMicroSec(uint32_t &io_currentTsPeriodInMicroSec,
                           ATTR_ODY_DQS_TRACKING_PERIOD_Type &io_dqscurrentcount)
{
    #define SBE_FUNC " getTsPeriodInMicroSec "
    SBE_ENTER(SBE_FUNC);

    ATTR_ODY_SENSOR_POLLING_PERIOD_MS_Type l_tspPeriodInMilliSec =
            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD_MS;

    if(l_tspPeriodInMilliSec == 0)
    {
        // The polling interval will be set to the pre defined default value.
        // This is done so that the periodic timer is always active and never dies.
        l_tspPeriodInMilliSec = fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD_MS_INIT;
        SBE_DEBUG(SBE_FUNC "Polling is not enabled, Defaulting to ATTR_ODY_SENSOR_POLLING_PERIOD_MS_INIT 0x%08X",
                        l_tspPeriodInMilliSec);
    }

    //Convert TS Polling period from milli seconds to micro seconds
    uint32_t l_tspPeriodInMicroSec = l_tspPeriodInMilliSec * 1000;

    // If there is any change in TS Polling period update the poling period and reset DQS current
    // counter value.
    if(l_tspPeriodInMicroSec != io_currentTsPeriodInMicroSec)
    {
        io_currentTsPeriodInMicroSec = l_tspPeriodInMicroSec;
        io_dqscurrentcount = 0x0;
    }

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

uint32_t sbepollTSnDQS(uint8_t &io_dqscount)
{
    #define SBE_FUNC " sbepollTSnDQS "
    SBE_ENTER(SBE_FUNC);

    ReturnCode l_rc = FAPI2_RC_SUCCESS;

    do
    {
        //If TS Polling is not enabled then we will skip executing the HWP's
        if(fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD_MS == 0)
            break;

        Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();

        ATTR_ODY_DQS_TRACKING_PERIOD_Type l_dqscount =
            fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_DQS_TRACKING_PERIOD;

        // Execute the Thermal Sensor polling HWP.
        SBE_DEBUG(SBE_FUNC "Execute the Thermal Sensor polling HWP");
        SBE_EXEC_HWP(l_rc, ody_thermal_read, l_ocmb_chip);
        if(l_rc)
        {
            SBE_ERROR(SBE_FUNC "ody_thermal_read has failed with rc 0x%08x", l_rc);
        }

        // Execute the DQS HWP.
        if((l_dqscount != 0) && (io_dqscount == l_dqscount))

        {
            SBE_DEBUG(SBE_FUNC "Execute the DQS HWP");
            SBE_EXEC_HWP(l_rc, ody_dqs_track, l_ocmb_chip);
            if(l_rc != FAPI2_RC_SUCCESS)
            {
                // Set the attribute ATTR_ODY_DQS_TRACKING_PERIOD to 0.
                // So that next time HWP will not be called. This was done
                // to make sure there will be no repetitive FFDC packets from this
                // failure.
                fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_DQS_TRACKING_PERIOD = 0;
            }
            io_dqscount = 0x0;
        }
        ++io_dqscount;
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
