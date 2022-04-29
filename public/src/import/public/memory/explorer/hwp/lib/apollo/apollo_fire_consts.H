/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/lib/apollo/apollo_fire_consts.H $ */
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

///
/// @file  apollo_fire_consts.H
/// @brief Contstants used in Apollo code for Fire.
///
// *HWP HWP Owner: Edgar Cordero <ecordero@us.ibm.com>
// *HWP HWP Backup: Ryan King <rpking@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: Memory

#ifndef _APOLLO_FIRE_CONSTS_H_
#define _APOLLO_FIRE_CONSTS_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------

//ADDRESSES
static const uint64_t FIRE_DDIMM_HOST_CONFIG_BASE = 0x0104000000000000;
static const uint64_t FIRE_OC_DLCTL_BASE          = FIRE_DDIMM_HOST_CONFIG_BASE | 0x0000000000000010;
static const uint64_t FIRE_DDIMM_PORT_OFFSET[5]   = {  0x0000000000000000, 0x0000040000000000, 0x0000080000000000, 0x00000C0000000000, 0x0000100000000000 };
static const uint64_t FIRE_OPENCAPI_BAR_BASE      = FIRE_DDIMM_HOST_CONFIG_BASE | 0x000000000000000C;
static const uint64_t RESET_CONTROL = 0x0100000000000004;


//DATA
static const uint64_t FIRE_OC_DLCTL_TRAIN_PAT_A   =  0x1;
static const uint64_t FIRE_OC_DLCTL_TRAIN_EN_AUTO =  0x8;
static const uint64_t FIRE_OC_DLCTL_TRAIN_START   =  44;
static const uint64_t FIRE_OC_DLCTL_TRAIN_WIDTH   =  4;

static const uint64_t FIRE_OPENCAPI_BAR_START     =  52;
static const uint64_t FIRE_EDPL_DISABLE_START     =  37;
static const uint64_t FIRE_BOOT_LANE_MODE_START   =  34;

static const uint64_t RESET_CONTROL_DEASSERT_BIT[5]   = {  60, 61, 62, 63, 58 };
static const uint64_t RESET_CONTROL_DEASSERT_WIDTH[5]   = {  1, 1, 1, 1, 2 };

// APOLLO FIRE
static const uint64_t FIRE_MAX_OMI = 5; // MAX OMI FIRE supports in Apollo

#endif // _APOLLO_FIRE_CONSTS_H_
