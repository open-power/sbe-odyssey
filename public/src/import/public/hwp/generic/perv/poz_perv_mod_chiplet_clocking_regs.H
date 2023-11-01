/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_chiplet_clocking_regs.H $ */
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
/// @file  poz_perv_mod_chiplet_clocking_regs.H
/// @brief Registers referenced by poz_perv_mod_chiplet_clocking module
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <poz_scom_perv.H>

using namespace scomt::poz;

SCOMT_PERV_USE_BIST;
SCOMT_PERV_USE_OPCG_ALIGN;
SCOMT_PERV_USE_OPCG_REG0;
SCOMT_PERV_USE_OPCG_REG1;
SCOMT_PERV_USE_OPCG_REG2;
SCOMT_PERV_USE_OPCG_CAPT1;
SCOMT_PERV_USE_OPCG_CAPT2;
SCOMT_PERV_USE_OPCG_CAPT3;
SCOMT_PERV_USE_PCB_OPCG_STOP;
SCOMT_PERV_USE_PCB_OPCG_GO;
SCOMT_PERV_USE_CPLT_CTRL0;
SCOMT_PERV_USE_CPLT_CTRL1;
SCOMT_PERV_USE_CPLT_CTRL2;
SCOMT_PERV_USE_CPLT_CTRL4;
SCOMT_PERV_USE_CPLT_CONF0;
SCOMT_PERV_USE_CPLT_CONF1;
SCOMT_PERV_USE_CPLT_STAT0;
SCOMT_PERV_USE_SCAN_REGION_TYPE;
SCOMT_PERV_USE_CLK_REGION;
SCOMT_PERV_USE_CLOCK_STAT_SL;
SCOMT_PERV_USE_CLOCK_STAT_NSL;
SCOMT_PERV_USE_CLOCK_STAT_ARY;
SCOMT_PERV_USE_SYNC_CONFIG;
SCOMT_PERV_USE_NET_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_SCRATCH_REGISTER_11;
