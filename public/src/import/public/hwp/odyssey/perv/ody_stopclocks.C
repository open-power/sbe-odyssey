/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_stopclocks.C $  */
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
/// @file  ody_stopclocks.C
/// @brief To stop chiplet clocks for Odyssey chip
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:SBE:HB
//------------------------------------------------------------------------------

#include <ody_stopclocks.H>
#include <poz_tp_stopclocks.H>
#include <target_filters.H>

using namespace fapi2;

fapi2::ReturnCode ody_stopclocks(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                 const StopClocksFlags i_stopclocks_flags)
{
    fapi2::ReturnCode l_rc;
    bool l_stop_chiplet_clocks_possible = false;
    bool l_stop_tp_clocks_possible = true;

    FAPI_INF("Entering... i_stopclocks_flags = %#08lX", i_stopclocks_flags);

    l_rc = poz_stopclocks_pre_check(i_target, l_stop_chiplet_clocks_possible, l_stop_tp_clocks_possible);

    if(l_rc != fapi2::FAPI2_RC_SUCCESS)
    {
        return l_rc;
    }

    if(l_stop_chiplet_clocks_possible)
    {
        // stop MC chiplet clocks
        if (i_stopclocks_flags & STOP_REGION_MC)
        {
            for (auto l_chiplet : i_target.getChildren<fapi2::TARGET_TYPE_PERV>
                 (static_cast<fapi2::TargetFilter>(fapi2::TARGET_FILTER_MC),
                  fapi2::TARGET_STATE_FUNCTIONAL))
            {
                l_rc = poz_common_stopclocks_chiplet_accessible(l_chiplet);

                if(l_rc == fapi2::FAPI2_RC_SUCCESS)
                {
                    // poz_chiplet_stopclocks will log errors if any
                    FAPI_INF("Calling poz_chiplet_stopclocks...");
                    poz_chiplet_stopclocks(l_chiplet);
                }
            }
        }
    }

    // TP chiplet stopclocks
    if((i_stopclocks_flags & (STOP_REGION_TP | STOP_REGION_SBE)) && l_stop_tp_clocks_possible)
    {
        // poz_tp_stopclocks will log errors if any
        FAPI_INF("Calling poz_tp_stopclocks....");
        poz_tp_stopclocks(i_target, i_stopclocks_flags, l_stop_chiplet_clocks_possible, mux::I2C2PCB);
    }

    // Vital stopclocks
    if(i_stopclocks_flags & STOP_REGION_VITL)
    {
        l_rc = poz_vital_stopclocks(i_target);
        return l_rc;
    }

//we want to avoid FAPI_TRY in this HWP and thus don't have fapi_try_exit
    FAPI_INF("Exiting ...");
    return fapi2::FAPI2_RC_SUCCESS;
}
