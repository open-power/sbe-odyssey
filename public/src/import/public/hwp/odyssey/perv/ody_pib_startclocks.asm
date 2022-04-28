/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_pib_startclocks.asm $ */
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
/// @brief
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Daniela Yacovone (falconed@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SPPE
//------------------------------------------------------------------------------

#include "defines.inc"
#include "registers.inc"
#include "settings.inc"
#include "cmdtable_hwp_errors.H"

ody_pib_startclocks:

         //SCAN_REGION_TYPE = 0
         putscom       SCAN_REGION_TYPE, ALL, 0xl0

         // Drop fences before starting clocks
         //if i_start_not_stop:
            //CPLT_CTRL1_WO_CLEAR = i_clock_regions << 44
         putscom       CPLT_CTRL1_CLEAR, ALL, CPLT_CTRL1__REGIONS(I_CLOCK_REGIONS)

        // Issue clock start/stop command
        //CLK_REGION = 0
        //CLK_REGION.set_CLOCK_CMD(i_start_not_stop ? 1 : 2);
        //CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions
        //CLK_REGION[4 bits starting at SEL_THOLD_SL] = i_clock_types
        putscom         CLK_REGION, ALL, CLK_REGION__CLK_CMD(I_START_NOT_STOP) | CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__SEL_THOLD_SL | CLK_REGION__SEL_THOLD_NSL | CLK_REGION__SEL_THOLD_ARY

        // Wait for command to be done
        // FAPI_TRY(poll_opcg_done(i_target, 10us, 1kcyc, 10));
        poll          CPLT_STAT, OPCG_DONE, OPCG_DONE, ERR_ROTATE_TIMEOUT

        // Check that the clock status is as expected
        //for check_type in (CLOCK_TYPE_SL, CLOCK_TYPE_NSL, CLOCK_TYPE_ARY):
        //FAPI_TRY(check_clock_status(i_target, i_clock_regions, i_clock_types & check_type, i_start_not_stop));
        test          CLOCK_STAT_SL, REGIONS_MASK, CLOCK_STAT_SL__REGIONS(I_CLOCK_REGIONS_TEST), ERR_CLOCK_STAT
        test          CLOCK_STAT_NSL, REGIONS_MASK, CLOCK_STAT_NSL__REGIONS(I_CLOCK_REGIONS_TEST), ERR_CLOCK_STAT
        test          CLOCK_STAT_ARY, REGIONS_MASK, CLOCK_STAT_ARY__REGIONS(I_CLOCK_REGIONS_TEST), ERR_CLOCK_STAT


        // Raise fences after clocks are stopped
        //if not i_start_not_stop:
        //CPLT_CTRL1_WO_OR = i_clock_regions << 44
