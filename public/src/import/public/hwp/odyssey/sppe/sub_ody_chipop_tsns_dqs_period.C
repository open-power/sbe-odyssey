/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/sppe/sub_ody_chipop_tsns_dqs_period.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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

///
/// @file sub_ody_chipop_tsns_dqs_period.C
/// @brief Odyssey Chipop Modify Temperature Sensor and/or DQS Drift Tracking Polling Period
///
// *HWP Owner: Cronus Team
// *HWP Backup:
// *HWP Level: 3
// *HWP Consumed by: Cronus

#include <ody_chipop_tsns_dqs_period.H>

fapi2::ReturnCode ody_chipop_tsns_dqs_period (
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint32_t i_tsns_period_ms,
    const uint8_t i_dqs_period )
{
    std::list<void*> l_args;

    const char* l_function = "ody_chipop_tsns_dqs_period";
    l_args.push_back((void*)l_function);

    const uint32_t l_version = 0x1;
    l_args.push_back((void*)&l_version);

    l_args.push_back(&fapi2::current_err);

    ecmdChipTarget l_target;
    fapiTargetToEcmdTarget(i_target, l_target);
    l_args.push_back(&l_target);

    l_args.push_back((void*)&i_tsns_period_ms);
    l_args.push_back((void*)&i_dqs_period);

    uint32_t l_rc = fapi2GeneralApi(l_args);

    if( l_rc == ECMD_FUNCTION_NOT_SUPPORTED )
    {
        FAPI_INF("No real hwp to call for ody_chipop_tsns_dqs_period - check that OCMB_SBE_FIFO is not off");
    }

    return fapi2::current_err;
}
