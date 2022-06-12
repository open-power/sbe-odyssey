/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_pib_arrayinit_cleanup.asm $ */
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
/// @brief Shared command table code to handle array init cleanup
/// @param I_CLOCK_REGIONS  Clock regions to run arrayinit_cleanup on
/// @param I_SCAN_TYPES     Scan types to run scan0; default SCAN_TYPES_NOT_RTG = 0xDCF
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

#ifndef I_SCAN_TYPES
#define I_SCAN_TYPES 0xDCF
#endif

mod_abist_cleanup:
         //OPCG_REG0 = 0
         putscom       OPCG_REG0, ALL, 0xl0

         //CLK_REGION = 0
         putscom       CLK_REGION, ALL, 0xl0

         //CPLT_CTRL0.set_CTRL_CC_ABSTCLK_MUXSEL_DC(0);
         putscom       CPLT_CTRL0_CLEAR, ALL, CPLT_CTRL0__CTRL_CC_ABSTCLK_MUXSEL_DC

         //BIST = 0
         putscom       BIST, ALL, 0xl0

mod_scan0:
         // Set up clock regions for NSL fill
         putscom       CLK_REGION, ALL, CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__SEL_THOLD_NSL | CLK_REGION__SEL_THOLD_ARY

         // Set up scan regions for scan0
         putscom       SCAN_REGION_TYPE, ALL, SCAN_REGION_TYPE__REGIONS(I_CLOCK_REGIONS) | SCAN_REGION_TYPE__TYPES(I_SCAN_TYPES)

         // Trigger scan0
         putscom       OPCG_REG0, ALL, OPCG_REG0__RUN_SCAN0

         // Wait for scan0 to complete
         poll          CPLT_STAT, CPLT_STAT__OPCG_DONE,  CPLT_STAT__OPCG_DONE,  ERR_OPCG_TIMEOUT_SCAN0

         // Clean up
         putscom       CLK_REGION, ALL, 0xl0

         //SCAN_REGION_TYPE = 0
         putscom       SCAN_REGION_TYPE, ALL, 0xl0

// Clear parameters to free up the #define namespace for the next module
#undef I_CLOCK_REGIONS
#undef I_SCAN_TYPES
