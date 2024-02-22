/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/oce_iddq_data_common.h $ */
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
#ifndef __OCE_IDDQ_DATA_COMMON_H__
#define __OCE_IDDQ_DATA_COMMON_H__

#include <stdint.h>
#include <pst_hcode_image_defines.H>


//
// OCEIDDQData_t : High-level OCE IDDQ data struct for OCC(405)
//
// Notes:
// - oce_status_flag is a **vector** and may have multiple bits set. This is to
//   give OCC as much info as possible.
// - The data block is managed in link.ld to guarantee 8-byte alignment.
//
typedef struct
{
    uint32_t     iddq_100ua;              // 4B
    uint32_t     tbr_new_dce_data_detect; // 4B - Time OCE detected NEW data from DCE
    uint32_t     tbr_iddq_calc_duration;  // 4B - IDDQ calculation duration (ref: data_ready detect)
    uint32_t     tod_iddq_rcvd;           // 4B - For optional use by 405 using local 2MHz TOD
    uint16_t     oce_status_flag;         // 2B - Status vector. See OCE_STATUS_FLAGS enum below.
    uint16_t     undefined[3];            // 6B - Pad to 8B (VC push demand)
} OCEIDDQData_t;


//
// Enum of OCE status flags
//
// Notes:
// - Used for indicating protocol violations to the OCC.
// - Used by OCEIddqData_t.oce_status_flags:
//   - Note that oce_status_flag is a **vector** and may have multiple bits set.
//     This is to give OCC as much info as possible.
// - These violations are detected at the time of DB0 IRQ reception and at commencement
//   of data collection.
//
enum OCE_STATUS_FLAGS
{
    OCE_STATUS_RESET               = 0b0000000000000000, //Reset init value
    OCE_STATUS_NORMAL              = 0b0000000000000001, //All is good
    OCE_STATUS_DCE_NOT_RESPONDING  = 0b0000000000000010, //DCE has not updated w/NEW data in >1ms
    OCE_STATUS_IDDQ_RESULT_STALE   = 0b0000000000000100, //Previous time slot's Iddq result used
    OCE_STATUS_IDDQ_GOING_NEGATIVE = 0b0000000000001000, //Iddq calc going negative
};


#endif /* __OCE_IDDQ_DATA_COMMON_H__ */
