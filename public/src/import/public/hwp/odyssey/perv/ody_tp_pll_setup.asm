/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_pll_setup.asm $ */
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

ody_tp_pll_setup:

         // Drop PLL region fence
         // CPLT_CTRL1.set_TC_REGION13_FENCE_DC(0);
         putscom       CPLT_CTRL1_CLEAR, ALL, CPLT_CTRL1__TC_REGION13_FENCE_DC

         // if bypass scratch register is not valid, default to locking the PLL
         cmpbne SCRATCH_REGISTER_16, ATTR_PLL_BYPASS_VALID, ATTR_PLL_BYPASS_VALID, lock_pll
         // if bypass scratch register is valid, check and potentially bypass
         cmpbne ATTR_PLL_BYPASS_REG, ATTR_PLL_BYPASS_BIT, ATTR_PLL_BYPASS_BIT, lock_pll

bypass_pll:
         putscom       ATTR_NEST_FREQ_MHZ_REG, ATTR_NEST_FREQ_MHZ(ALL), ATTR_NEST_FREQ_MHZ(REF_CLOCK_FREQ_MHZ)
         b             pll_setup_done

lock_pll:
     // lock PLL
         //FAPI_INF("Attempt to lock PLL");
         //ROOT_CTRL3.set_TP_MCPLL_TEST_EN_DC(0); // not available in headers yet - bit 24
         //ROOT_CTRL3.set_TP_MCPLL_RESET_DC(0);   // not available in headers yet - bit 25
         putscom       ROOT_CTRL3_CLEAR, ALL, ROOT_CTRL3__TP_MCPLL_TEST_EN_DC
         putscom       ROOT_CTRL3_CLEAR, ALL, ROOT_CTRL3__TP_MCPLL_RESET_DC

         //FAPI_TRY(mod_poll_pll_lock_fsi2pib(i_target, ODY_PERV_MCPLL));// ODY_PERV_MCPLL = 0x02
         poll     PLL_LOCK_REG_FSI2PIB, ODY_PERV_MCPLL, ODY_PERV_MCPLL, ERR_LOCK_PLL_TIMEOUT


         //FAPI_INF("Prepare chip for at-speed operation");
         //OPCG_ALIGN.set_SCAN_RATIO(3);          // Swich scan ratio to 4:1
         putscom       OPCG_ALIGN, OPCG_ALIGN__SCAN_RATIO(ALL), OPCG_ALIGN__SCAN_RATIO(0b00011)


        //FAPI_INF("Take PLL out of bypass");
        //ROOT_CTRL3.set_TP_MCPLL_BYPASS_DC(0);  // not available in headers yet - bit 26
        putscom        ROOT_CTRL3_CLEAR, ALL, ROOT_CTRL3__TP_MCPLL_BYPASS_DC

pll_setup_done:
