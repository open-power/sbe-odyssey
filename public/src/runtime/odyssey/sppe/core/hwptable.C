/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwptable.C $             */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "hwptable.H"
#include "hwp_includes.H"
#include "hwpWrapper.H"
#include "atefw.H"

using namespace fapi2;

// IO HWP Num enum. DO not change the sequence. Refer the POZ specs.
enum iohwpnum
{
    ody_omi_hss_load_ppe_hwp,
    ody_omi_hss_config_hwp,
    ody_omi_hss_start_ppe_hwp,
    ody_omi_hss_bist_init_hwp,
    ody_omi_hss_bist_start_hwp,
    ody_omi_hss_bist_poll_hwp,
    ody_omi_hss_bist_cleanup_hwp,
    ody_omi_hss_init_hwp,
    ody_omi_hss_dccal_start_hwp,
    ody_omi_hss_dccal_poll_hwp,
    ody_omi_hss_tx_zcal_hwp,
    ody_omi_pretrain_adv_hwp,
    ody_omi_setup_hwp,
    ody_omi_train_hwp,
    ody_omi_train_check_hwp,
    ody_omi_posttrain_adv_hwp,
};

// MEM HWP Num enum
enum memhwpnum
{
    ody_scominit_hwp,
    ody_ddrphyinit_hwp,
    ody_ddrphyinit_ate_hwp,
    ody_load_imem_hwp,
    ody_load_dmem_hwp,
    ody_sppe_draminit_hwp,
    ody_ate_fw_hwp,
    ody_load_pie_hwp,
    ody_draminit_mc_hwp,
    ody_enable_ecc_hwp,
    ody_thermal_init_hwp,

};

// Refer POZ spec for this table.
// IO HWP Class
static hwpMap_t hwpClass1PtrTbl[] =
          {
               HWP_MAP(istepLoadIOPPEwithOcmb, ody_omi_hss_ppe_load),     // 1.01
               HWP_MAP( istepWithOcmb, ody_omi_hss_config ),              // 1.02
               HWP_MAP( istepWithOcmb, ody_omi_hss_ppe_start),            // 1.03
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_init ),           // 1.04
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_start ),          // 1.05
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_poll ),           // 1.06
               HWP_MAP( istepWithOcmb, ody_omi_hss_bist_cleanup),         // 1.07
               HWP_MAP( istepWithOcmb, ody_omi_hss_init ),                // 1.08
               HWP_MAP( istepWithOcmb, ody_omi_hss_dccal_start ),         // 1.09
               HWP_MAP( istepWithOcmb, ody_omi_hss_dccal_poll ),          // 1.10
               HWP_MAP( istepWithOcmb, ody_omi_hss_tx_zcal ),             // 1.11
               HWP_MAP( istepWithOcmb, ody_omi_pretrain_adv ),            // 1.12
               HWP_MAP( istepWithOcmb, ody_omi_setup ),                   // 1.13
               HWP_MAP( istepWithOcmb, ody_omi_train ),                   // 1.14
               HWP_MAP( istepWithOcmb, ody_omi_train_check ),             // 1.15
               HWP_MAP( istepWithOcmb, ody_omi_posttrain_adv ),           // 1.16
          };

// MEM HWP Class
static hwpMap_t hwpClass2PtrTbl[] =
          {
               HWP_MAP( istepWithOcmb, ody_scominit ),                   // 2.01
               HWP_MAP( istepWithOcmb, ody_ddrphyinit ),                 // 2.02
               HWP_MAP( istepATEPHYReset, NULL ),                        // 2.03 LAB HWPs
               HWP_MAP( istepLoadIMEMwithOcmb, ody_load_imem ),          // 2.04
               HWP_MAP( istepLoadDMEMwithOcmb, ody_load_dmem ),          // 2.05
               HWP_MAP( istepDraminitWithOcmb, ody_sppe_draminit ),      // 2.06
               HWP_MAP( istepATEFW, NULL),                               // 2.07 LAB HWPs
               HWP_MAP( istepLoadPIEwithOcmb, ody_load_pie ),            // 2.08
               HWP_MAP( istepWithOcmb, ody_draminit_mc ),                // 2.09
               HWP_MAP( istepWithOcmb, ody_enable_ecc ),                 // 2.10
               HWP_MAP( istepWithOcmb, ody_thermal_init ),               // 2.11
          };

hwpTableEntry_t hwpTableEntries[] = {
    HWP_ENTRY(  1, hwpClass1PtrTbl),
    HWP_ENTRY(  2, hwpClass2PtrTbl),
};

REGISTER_HWP_TABLE(hwpTableEntries)