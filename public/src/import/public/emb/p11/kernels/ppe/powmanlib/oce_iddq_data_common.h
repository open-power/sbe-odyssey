/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/oce_iddq_data_common.h $ */
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
#ifndef __OCE_IDDQ_DATA_COMMON_H__
#define __OCE_IDDQ_DATA_COMMON_H__

#include <stdint.h>
#include <p11_hcode_image_defines.H>


//
// IddqResult_t : IDDQ result structure
//
typedef struct
{
    uint32_t      iddq_100ua; // 4
    uint32_t      icsq_100ua; // 4
} IDDQResult_t;

//
// OCEIDDQData_t : High-level OCE IDDQ data struct for OCC(405)
//
// Notes:
// - The data block is managed in link.ld to guarantee 8-byte alignment.
//
typedef struct
{
    IDDQResult_t iddq_result;             // 8B
    uint32_t     tbr_new_dce_data_detect; // 4B - Time OCE detected NEW data from DCE
    uint32_t     tbr_iddq_calc_duration;  // 4B - IDDQ calculation duration (ref: data_ready detect)
    uint32_t     tod_iddq_rcvd;           // 4B - For optional use by 405 using local 2MHz TOD
    uint16_t     oce_status_flag;         // 2B - Status vector. See OCE_STATUS_FLAGS enum below.
    uint16_t     undefined[1];            // 2B - Pad to 8B (VC push demand)
} OCEIDDQData_t;

extern OCEIDDQData_t*  G_oce_iddq_data; //Big "G_" because it's shared across CEs.

//
// Enum of OCE status flags (used by OCEIddqData_t.oce_status_flags)
//
// Notes:
// - Used for indicating protocol violations to the 405 (and possibly the OCE too).
// - These violations are detected at the time of DB0 IRQ reception and at commencement
//   of data collection.
//
enum OCE_STATUS_FLAGS
{
    OCE_STATUS_RESET              = 0b0000000000000000, //Reset init value
    OCE_STATUS_NORMAL             = 0b0000000000000001, //Everything is NORMAL
    OCE_STATUS_DCE_NOT_RESPONDING = 0b0000000000000100, //DCE has not updated w/NEW data in >1ms
};


#endif /* __OCE_IDDQ_DATA_COMMON_H__ */
