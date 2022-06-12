/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_pib_startclocks.asm $ */
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
/// @brief Shared command table code to handle start clocks
/// @param I_CLOCK_REGIONS  Clock regions to run startclocks on
/// @param I_CLOCK_TYPES    Clock types to run startclocks on; default ALL=0xE
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
// *HWP Consumed by     : SPPE
//------------------------------------------------------------------------------

#include "defines.inc"
#include "registers.inc"
#include "cmdtable_hwp_errors.H"

// Check parameters
#ifndef I_CLOCK_REGIONS
#error I_CLOCK_REGIONS is not defined
#endif

#ifndef I_CLOCK_TYPES
#define I_CLOCK_TYPES 0xE
#endif

poz_pib_startclocks:
         // Clear SCAN_REGION_TYPE register
         putscom  SCAN_REGION_TYPE, ALL, 0xl0

         // Drop fences before starting clocks
         putscom  CPLT_CTRL1_CLEAR, ALL, CPLT_CTRL1__REGIONS(I_CLOCK_REGIONS)

         // Issue clock start command selecting required clock_regions and clock_types
         putscom  CLK_REGION, ALL, CLK_REGION__CLK_CMD(CLOCK_START_CMD) | CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__TYPES(I_CLOCK_TYPES)

         // Wait for command to be done
         poll     CPLT_STAT, CPLT_STAT__OPCG_DONE,  CPLT_STAT__OPCG_DONE,  ERR_OPCG_TIMEOUT_ABIST

         // Check that the clock status is as expected
         //for check_type in (CLOCK_TYPE_SL, CLOCK_TYPE_NSL, CLOCK_TYPE_ARY):
         //FAPI_TRY(check_clock_status(i_target, i_clock_regions, i_clock_types & check_type, i_start_not_stop));
         test     CLOCK_STAT_SL,  CLOCK_STAT_SL__REGIONS(I_CLOCK_REGIONS), 0, ERR_CLOCK_STAT_SL
         test     CLOCK_STAT_NSL, CLOCK_STAT_SL__REGIONS(I_CLOCK_REGIONS), 0, ERR_CLOCK_STAT_NSL
         test     CLOCK_STAT_ARY, CLOCK_STAT_SL__REGIONS(I_CLOCK_REGIONS), 0, ERR_CLOCK_STAT_ARY

// Clear parameters to free up the #define namespace for the next module
#undef I_CLOCK_REGIONS
#undef I_CLOCK_TYPES
