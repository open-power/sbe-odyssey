/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_stopclocks.H $  */
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
/// @file  ody_stopclocks.H
/// @brief To stop chiplet clocks for Odyssey chip
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : FSP:SBE:HB
//------------------------------------------------------------------------------

#ifndef _ODY_STOPCLOCKS_H_
#define _ODY_STOPCLOCKS_H_

#include <fapi2.H>
#include <poz_common_stopclocks.H>

typedef fapi2::ReturnCode (*ody_stopclocks_FP_t)(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>&,
        const StopClocksFlags);

/// @brief To stop chiplet clocks for Odyssey chip
///
/// @param[in]   i_target            Reference to TARGET_TYPE_OCMB_CHIP
/// @param[in]   i_stopclocks_flags  Stop clocks flags
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.

extern "C"
{
    fapi2::ReturnCode ody_stopclocks(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                     const StopClocksFlags i_stopclocks_flags);
}

#endif
