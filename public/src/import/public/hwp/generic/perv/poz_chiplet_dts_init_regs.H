/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_chiplet_dts_init_regs.H $ */
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
/// @file  poz_chiplet_dts_init_regs.H
/// @brief Registers referenced by poz_chiplet_dts_init module
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <p11_scom_pc_an.H>
#include <p11_scom_pc_an_fields.H>

using namespace scomt::pc;

SCOMT_PC_USE_ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL;
SCOMT_PC_USE_ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_CAL_DONE;

typedef ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_START_CAL_t KVREF_START_CAL_t;
typedef ANALOG_SHIM_VOLTAGE_WRAP_SCOMSAT_KVREF_CAL_DONE_t KVREF_CAL_DONE_t;
