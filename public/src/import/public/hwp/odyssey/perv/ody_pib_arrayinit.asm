/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_pib_arrayinit.asm $ */
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

ody_pib_arrayinit:

mod_abist_start:

         //Disable PIB region fencing - TODO remove
         // putscom       CPLT_CTRL1_CLEAR, ALL, CPLT_CTRL1__REGIONS(I_CLOCK_REGIONS)


         // Switch dual-clocked arrays to ABIST clock domain
         putscom     CPLT_CTRL0_WO_OR, ALL, CPLT_CTRL0__CTRL_CC_ABSTCLK_MUXSEL_DC         //CPLT_CTRL0.set_CTRL_CC_ABSTCLK_MUXSEL_DC(1);

         // Set up BISTed regions
         //BIST = 0
         //BIST.set_TC_SRAM_ABIST_MODE_DC(1);
         //BIST[16 bits starting at BIST_REGION_PERV] = i_clock_regions    //perv regions: 0-3, 10
         putscom     BIST, ALL, BIST__TC_SRAM_ABIST_MODE_DC | BIST__REGIONS(I_CLOCK_REGIONS)

         //Set up clocking
         //CLK_REGION = 0
         //CLK_REGION[16 bits starting at CLOCK_REGION_PERV] = i_clock_regions    //perv regions: 0-3, 10
         //CLK_REGION.set_SEL_THOLD_SL(1);
         //CLK_REGION.set_SEL_THOLD_NSL(1);
         //CLK_REGION.set_SEL_THOLD_ARY(1);
         putscom      CLK_REGION, ALL, CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__SEL_THOLD_SL | CLK_REGION__SEL_THOLD_NSL | CLK_REGION__SEL_THOLD_ARY

         // Configure idle count
         //OPCG_REG1[bits 0:35] = i_abist_start_at
         putscom       OPCG_REG1, 0xlFFFFFFFFF, PERV_ABIST_START_AT

         // Configure loop count and start OPCG
         //OPCG_REG0 = 0
         //OPCG_REG0.set_RUNN_MODE(1);
         //OPCG_REG0.set_OPCG_GO(1);
         //OPCG_REG0.set_OPCG_STARTS_BIST(1);
         //OPCG_REG0.set_LOOP_COUNT(i_runn_cycles)
         putscom       OPCG_REG0, ALL, OPCG_REG0__RUNN_MODE | OPCG_REG0__OPCG_GO | OPCG_REG0__OPCG_START_BIST | OPCG_REG0__LOOP_COUNT(I_RUNN_CYCLES)

mod_abist_poll:
         //FAPI_TRY(poll_opcg_done(i_target, 200us, 1120kcyc, 400));
         poll     CPLT_STAT, OPCG_DONE, OPCG_DONE, ERR_ROTATE_TIMEOUT

         // Check that ABIST_DONE is set
         //if not CPLT_STAT0.ABIST_DONE_DC:
           //FAPI_TRY(ASSERT(SRAM_ABIST_DONE_BIT_ERR));
         poll     CPLT_STAT, ABIST_DONE, ABIST_DONE, ERR_ROTATE_TIMEOUT   //TODO uncomment once fencing issue is resolved
