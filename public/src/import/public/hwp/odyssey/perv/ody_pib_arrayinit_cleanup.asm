/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_pib_arrayinit_cleanup.asm $ */
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

ody_tp_arrayinit_cleanup:

mod_abist_cleanup:

         //OPCG_REG0 = 0
         putscom       OPCG_REG0, ALL, 0xl0

         //CLK_REGION = 0
         putscom       CLK_REGION, ALL, 0xl0

         //CPLT_CTRL0.set_CTRL_CC_ABSTCLK_MUXSEL_DC(0);
         putscom       CPLT_CTRL0_CLEAR, ALL, CPLT_CTRL0__CTRL_CC_ABSTCLK_MUXSEL_DC

         //BIST = 0
         //BIST.set_TC_SRAM_ABIST_MODE_DC(!i_clear_sram_abist_mode);
         putscom       BIST, ALL, 0xl0

mod_scan0:  //regions=[perv, pib, occ, net])

         // Set up clock regions for NSL fill
         //CLK_REGION = 0
         //CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions
         //CLK_REGION[4 bits starting at SEL_THOLD_SL] = i_clock_types
         putscom       CLK_REGION, ALL, CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__SEL_THOLD_NSL

         // Set up scan regions for scan0
         //SCAN_REGION_TYPE = 0
         //SCAN_REGION_TYPE[16 bits starting at SCAN_REGION_PERV] = i_clock_regions
         //SCAN_REGION_TYPE[12 bits starting at SCAN_TYPE_FUNC] = i_scan_types
         putscom       SCAN_REGION_TYPE, ALL, SCAN_REGION_TYPE__REGIONS(I_CLOCK_REGIONS) | SCAN_REGION_TYPE__TYPES(I_SCAN_TYPES)

         // Trigger scan0
         //OPCG_REG0 = 0
         //OPCG_REG0.set_RUN_SCAN0(1);
         putscom       OPCG_REG0, ALL, OPCG_REG0__RUN_SCAN0

         // Wait for scan0 to complete
         //FAPI_TRY(poll_opcg_done(i_target, 16us, 800kcyc, 200));
         poll          CPLT_STAT, OPCG_DONE, OPCG_DONE, ERR_OPCG_TIMEOUT_SCAN0

         // Clean up
         //CLK_REGION = 0
         putscom       CLK_REGION, ALL, 0xl0

         //SCAN_REGION_TYPE = 0
         putscom       SCAN_REGION_TYPE, ALL, 0xl0
