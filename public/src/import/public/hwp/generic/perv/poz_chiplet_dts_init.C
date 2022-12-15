/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_dts_init.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
/// @file  poz_chiplet_dts_init.C
/// @brief Start KVREF calibration and check for calibration done
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <poz_chiplet_dts_init.H>
#include <poz_perv_common_params.H>

using namespace fapi2;

enum POZ_CHIPLET_DTS_INIT_Private_Constants
{
    DELAY_10us = 10000, // unit is nano seconds
    DELAY_100us = 100000, // unit is nano seconds
    SIM_CYCLE_DELAY = 1000, // unit is sim cycles
    POLL_COUNT = 10000,
    KVREF_START_CAL00 = 0,
    KVREF_CAL_DONE00 = 0
};

ReturnCode poz_chiplet_dts_init(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, uint64_t i_start_cal_address)
{
    buffer<uint64_t> l_data64;
    int l_timeout = 0;

    FAPI_INF("Entering ...");

    // DTS calibration is scanned in via the repr ring, but we can use this to calibrate KVREF
    FAPI_INF("Calibrating voltage reference");

    l_data64.flush<0>();
    l_data64.setBit<KVREF_START_CAL00>();
    FAPI_TRY(fapi2::putScom(i_target, i_start_cal_address, l_data64));

    FAPI_TRY(delay(DELAY_10us, SIM_CYCLE_DELAY));

    FAPI_INF("Check for calibration done");

    l_timeout = POLL_COUNT;

    while (l_timeout != 0)
    {

        l_data64.flush<0>();
        FAPI_TRY(fapi2::getScom(i_target, i_start_cal_address + 4,
                                l_data64)); // start_cal_address+4 is equal to cal_done address

        if (l_data64.getBit<KVREF_CAL_DONE00>() == 1)
        {
            break;
        }

        FAPI_TRY(delay(DELAY_100us, SIM_CYCLE_DELAY));

        --l_timeout;
    }

    FAPI_DBG("Loop Count :%d", l_timeout);

    FAPI_ASSERT(l_timeout > 0,
                fapi2::POZ_KVREF_CAL_NOT_DONE_ERR()
                .set_KVREF_CAL_DONE(l_data64)
                .set_LOOP_COUNT(POLL_COUNT)
                .set_HW_DELAY(DELAY_100us),
                "ERROR: Calibration not done");

    l_data64.flush<0>();
    FAPI_TRY(fapi2::putScom(i_target, i_start_cal_address, l_data64));

fapi_try_exit:
    FAPI_INF("Exiting ...");
    return current_err;
}
