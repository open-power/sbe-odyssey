/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/ipl_table.C $            */
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
#include "ipl.H"
#include "return_code.H"
#include "sbetrace.H"
#include "sbeutil.H"
#include "target.H"
#include "istep_includes.H"
#include "sbecmnhwpwrapper.H"
#include "hwpWrapper.H"

using namespace fapi2;

//----------------------------------------------------------------------------

static istepMap_t g_istep1PtrTbl[] =
         {
             ISTEP_MAP( NULL, NULL ),                               // 1.01
             ISTEP_MAP( NULL, NULL ),                               // 1.02
             ISTEP_MAP( NULL, NULL ),                               // 1.03
             ISTEP_MAP( NULL, NULL ),                               // 1.04
             ISTEP_MAP( NULL, NULL ),                               // 1.05
             ISTEP_MAP( NULL, NULL ),                               // 1.06
             ISTEP_MAP( NULL, NULL ),                               // 1.07
             ISTEP_MAP( NULL, NULL ),                               // 1.08
             ISTEP_MAP( NULL, NULL ),                               // 1.09
             ISTEP_MAP( NULL, NULL ),                               // 1.10
             ISTEP_MAP( NULL, NULL ),                               // 1.11
             ISTEP_MAP( NULL, NULL ),                               // 1.12
             ISTEP_MAP( istepWithOcmb, ody_sppe_attr_setup ),       // 1.13
             ISTEP_MAP( NULL, NULL ),                               // 1.14
             ISTEP_MAP( NULL, NULL ),                               // 1.15
             ISTEP_MAP( NULL, NULL ),                               // 1.16
             ISTEP_MAP( NULL, NULL ),                               // 1.17
             ISTEP_MAP( istepWithOcmb, ody_tp_arrayinit ),          // 1.18
             ISTEP_MAP( istepWithOcmb, ody_tp_arrayinit_cleanup ),  // 1.19
             ISTEP_MAP( istepWithOcmb, ody_tp_initf ),              // 1.20
             ISTEP_MAP( istepWithOcmb, ody_tp_startclocks ),        // 1.21
             ISTEP_MAP( istepWithOcmb, ody_tp_init ),               // 1.22
         };

static istepMap_t g_istep3PtrTbl[] =
         {
             ISTEP_MAP( NULL, NULL ),                               // 3.01
             ISTEP_MAP( istepWithOcmb, ody_chiplet_clk_config ),    // 3.02
             ISTEP_MAP( NULL, NULL ),                               // 3.03
             ISTEP_MAP( istepWithOcmb, ody_chiplet_reset ),         // 3.04
             ISTEP_MAP( istepWithOcmb, ody_chiplet_unused_psave ),  // 3.05
             ISTEP_MAP( NULL, NULL ),                               // 3.06
             ISTEP_MAP( NULL, NULL ),                               // 3.07
             ISTEP_MAP( istepBistWithOcmb,  ody_bist_repr_initf ),  // 3.08
             ISTEP_MAP( istepAbistWithOcmb, ody_abist ),            // 3.09
             ISTEP_MAP( istepLbistWithOcmb, ody_lbist ),            // 3.10
             ISTEP_MAP( istepWithOcmb, ody_chiplet_repr_initf ),    // 3.11
             ISTEP_MAP( istepWithOcmb, ody_chiplet_arrayinit ),     // 3.12
             ISTEP_MAP( NULL, NULL ),                               // 3.13
             ISTEP_MAP( istepWithOcmb, ody_chiplet_initf ),         // 3.14
             ISTEP_MAP( istepWithOcmb, ody_chiplet_init ),          // 3.15
             ISTEP_MAP( NULL, NULL ),                               // 3.16
             ISTEP_MAP( NULL, NULL ),                               // 3.17
             ISTEP_MAP( NULL, NULL ),                               // 3.18
             ISTEP_MAP( NULL, NULL ),                               // 3.19
             ISTEP_MAP( istepWithOcmb, ody_chiplet_startclocks ),   // 3.20
             ISTEP_MAP( istepWithOcmb, ody_chiplet_fir_init ),      // 3.21
             ISTEP_MAP( NULL, NULL ),                               // 3.22
             ISTEP_MAP( NULL, NULL ),                               // 3.23
             ISTEP_MAP( istepWithOcmb, ody_nest_enable_io ),        // 3.24
             ISTEP_MAP( NULL, NULL ),                               // 3.25
             ISTEP_MAP( NULL, NULL ),                               // 3.26
             ISTEP_MAP( NULL, NULL ),                               // 3.27
             ISTEP_MAP( NULL, NULL ),                               // 3.28
             ISTEP_MAP( NULL, NULL ),                               // 3.29
             ISTEP_MAP( NULL, NULL ),                               // 3.30
         };

// @TODO: End

istepTableEntry_t istepTableEntries[] = {
    ISTEP_ENTRY(  1, g_istep1PtrTbl),
    ISTEP_ENTRY(  3, g_istep3PtrTbl),
};

REGISTER_ISTEP_TABLE(istepTableEntries)
