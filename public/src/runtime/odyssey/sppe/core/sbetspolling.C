/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbetspolling.C $         */
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
#include "sbetspolling.H"
#include "ody_thermal_read.H"
#include "ody_dqs_track.H"

using namespace fapi2;

uint32_t sbepollTSnDQS(uint32_t &io_tspperiod, uint8_t &io_dqscount)
{
    #define SBE_FUNC " sbepollTSnDQS "
    SBE_ENTER(SBE_FUNC);
    ReturnCode l_rc = FAPI2_RC_SUCCESS;

    do
    {
        Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
        // Read the ATTR_ODY_SENSOR_POLLING_PERIOD and
        // ATTR_ODY_DQS_TRACKING_PERIOD.

        ATTR_ODY_SENSOR_POLLING_PERIOD_Type l_tspperiod =
                     fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_SENSOR_POLLING_PERIOD;;

        ATTR_ODY_DQS_TRACKING_PERIOD_Type l_dqscount =
                     fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_ODY_DQS_TRACKING_PERIOD;

        // If there is any change in the polling period, update it.
        // Also set the DQS count to 0.
        if(io_tspperiod != l_tspperiod)
        {
            io_tspperiod = l_tspperiod;
            io_dqscount = 0;
        }
        SBE_DEBUG(SBE_FUNC "Thermal Sensor polling period is 0x%08X ms",
                           io_tspperiod);

        if(l_tspperiod == 0)
        {
            // This measn SBE does not have to execute the thermal init HWP.
            // The polling interval will be set to the pre defined one.
            io_tspperiod = THERMAL_SENSOR_POLLING_INITIAL_PERIOD_MICROSEC;
            SBE_DEBUG(SBE_FUNC "Polling is not enabled, l_tspperiod is 0x%08X",
                               l_tspperiod);
            break;
        }

        SBE_DEBUG(SBE_FUNC "Execute the Thermal Sensor polling HWP");
        // Execute the Thermal Sensor polling HWP.
        SBE_EXEC_HWP(l_rc, ody_thermal_read, l_ocmb_chip);
        if(l_rc)
        {
            SBE_ERROR(SBE_FUNC "ody_thermal_read has failed with rc 0x%08x", l_rc);
        }

        // Execute the DQS HWP.
        if((l_dqscount != 0) && ((io_dqscount % l_dqscount) == 0))

        {
            SBE_DEBUG(SBE_FUNC "Execute the DQS HWP");
            SBE_EXEC_HWP(l_rc, ody_dqs_track, l_ocmb_chip);
        }

        ++io_dqscount;
    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC

}
