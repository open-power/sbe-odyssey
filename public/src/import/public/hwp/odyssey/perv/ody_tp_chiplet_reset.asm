/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_tp_chiplet_reset.asm $ */
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

hreset_check:
         // Check whether we are in hreset path or not
         cmpbne SB_CS_REG, HRESET_SET, HRESET_SET, ody_tp_chiplet_reset

         // return if it is hreset path
         return

ody_tp_chiplet_reset:

         // Drop PCB interface reset to enable access into TP chiplet
         putscom     ROOT_CTRL0_CLEAR, ALL, ROOT_CTRL0__PCB_RESET_DC    //ROOT_CTRL0.set_PCB_RESET_DC(0);

         // Increase OPCG wait cycles to enable scanning
         putscom     OPCG_ALIGN, OPCG_ALIGN__OPCG_WAIT_CYCLES(ALL), OPCG_ALIGN__OPCG_WAIT_CYCLES(0x020)
