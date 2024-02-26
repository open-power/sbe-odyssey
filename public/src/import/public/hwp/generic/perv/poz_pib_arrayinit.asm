/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_pib_arrayinit.asm $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
/// @brief Shared command table code to handle array init
/// @param I_CLOCK_REGIONS  Clock regions to run arrayinit on
/// @param I_RUNN_CYCLES    Number of ABIST cycles to run; default 0x42FFF
/// @param I_ABIST_START_AT Number of idle cycles before ABIST is started; default 0xF0
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

#ifndef I_RUNN_CYCLES
#define I_RUNN_CYCLES 0x10FFFF
#endif

#ifndef I_ABIST_START_AT
#define I_ABIST_START_AT 0xF0
#endif

mod_abist_start:
        // Switch dual-clocked arrays to ABIST clock domain
        putscom  CPLT_CTRL0_WO_OR, ALL, CPLT_CTRL0__CTRL_CC_ABSTCLK_MUXSEL_DC

        // Set up BISTed regions
        putscom  BIST, ALL, BIST__TC_SRAM_ABIST_MODE_DC | BIST__REGIONS(I_CLOCK_REGIONS)

        // Set up clocking
        putscom  CLK_REGION, ALL, CLK_REGION__REGIONS(I_CLOCK_REGIONS) | CLK_REGION__SEL_THOLD_SL | CLK_REGION__SEL_THOLD_NSL | CLK_REGION__SEL_THOLD_ARY

        // Configure idle count
        putscom  OPCG_REG1, ALL, OPCG_REG1__START_BIST(I_ABIST_START_AT)

        // Configure loop count and start OPCG
        putscom  OPCG_REG0, ALL, OPCG_REG0__RUNN_MODE | OPCG_REG0__OPCG_GO | OPCG_REG0__OPCG_START_BIST | OPCG_REG0__LOOP_COUNT(I_RUNN_CYCLES)

mod_abist_poll:
        // Poll for the OPCG to be done
        poll     CPLT_STAT, CPLT_STAT__OPCG_DONE,  CPLT_STAT__OPCG_DONE,  ERR_OPCG_TIMEOUT_ABIST

        // Check that the ABIST engines are also done
        test     CPLT_STAT, CPLT_STAT__ABIST_DONE, CPLT_STAT__ABIST_DONE, ERR_ABIST_TIMEOUT

// Clear parameters to free up the #define namespace for the next module
#undef I_CLOCK_REGIONS
#undef I_RUNN_CYCLES
#undef I_ABIST_START_AT
