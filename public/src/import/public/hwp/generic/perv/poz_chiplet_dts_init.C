/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_dts_init.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
//------------------------------------------------------------------------------
/// @file poz_chiplet_dts_init.C
///
/// @brief Start KVREF calibration and check for calibration done
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SSBE, TSBE
//------------------------------------------------------------------------------

#include "poz_chiplet_dts_init.H"
#include "poz_perv_common_params.H"
#include <p11_scom_pc_an.H>

SCOMT_PC_USE_ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL;
SCOMT_PC_USE_ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_CAL_DONE;

using namespace fapi2;
using namespace scomt::pc;

enum POZ_CHIPLET_DTS_INIT_Private_Constants
{
    DELAY_10us = 10000, // unit is nano seconds
    DELAY_100us = 100000, // unit is nano seconds
    SIM_CYCLE_DELAY = 1000, // unit is sim cycles
    POLL_COUNT = 100
};

ReturnCode poz_chiplet_dts_init(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL_t KVREF_START_CAL;
    ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_CAL_DONE_t KVREF_CAL_DONE;
    int l_timeout = 0;

    FAPI_INF("Entering ...");

    // DTS calibration is scanned in via the repr ring, but we can use this to calibrate KVREF
    FAPI_INF("Calibrating voltage reference");

    KVREF_START_CAL = 0;
    KVREF_START_CAL.set_KVREF_START_CAL00(1);
    FAPI_TRY(KVREF_START_CAL.putScom(i_target));

    FAPI_TRY(delay(DELAY_10us, SIM_CYCLE_DELAY));

    FAPI_INF("Check for calibration done");

    l_timeout = POLL_COUNT;

    while (l_timeout != 0)
    {

        FAPI_TRY(KVREF_CAL_DONE.getScom(i_target));

        if (KVREF_CAL_DONE.get_KVREF_CAL_DONE00() == 1)
        {
            break;
        }

        FAPI_TRY(delay(DELAY_100us, SIM_CYCLE_DELAY));

        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    FAPI_ASSERT(l_timeout > 0,
                fapi2::KVREF_CAL_NOT_DONE_ERR()
                .set_KVREF_CAL_DONE(KVREF_CAL_DONE)
                .set_LOOP_COUNT(POLL_COUNT)
                .set_HW_DELAY(DELAY_100us),
                "ERROR: Calibration not done");

    KVREF_START_CAL = 0;
    FAPI_TRY(KVREF_START_CAL.putScom(i_target));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
