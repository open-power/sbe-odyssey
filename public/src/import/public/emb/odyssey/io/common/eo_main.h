/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/eo_main.h $    */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_main.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// jjb21101900 |jjb     | removed eo_main_recal_rx since no longer referenced in pipe code
// jjb21101500 |jjb     | Added eo_main_recal_rx for access in pcie rx margining
// vbr21020500 |vbr     | Updates so same function prototypes for tbus and ioo
// vbr20030200 |vbr     | Added hooks for handling PCIe training / PIPE signals
// ------------|--------|-------------------------------------------------------
// vbr17080800 |vbr     | Changed to individual functions for each cal type. Removed calibration_type typedef.
// vbr16081000 |vbr     | No bank input for IOF
// mbs16062400 |mbs     | Added gcr_addr struct
// vbr16021600 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_MAIN_H_
#define _EO_MAIN_H_

#include "io_lib.h"
#include "pk.h"

// Run DC Cal on the lane specified in gcr_addr (IOO: Both Banks)
int eo_main_dccal(t_gcr_addr* gcr_addr);
int eo_main_dccal_rx(t_gcr_addr* gcr_addr);
int eo_main_dccal_tx(t_gcr_addr* gcr_addr);

// Run Initial Cal on the lane specified in gcr_addr (IOO: Bank A)
int eo_main_init(t_gcr_addr* gcr_addr);

// Run Recal on the the lane specified in gcr_addr
int eo_main_recal(t_gcr_addr* gcr_addr);

// Macros for the timestamping code used in all 3 cal functions for measuring cal time.
// To avoid doing a divide, we assume a power-of-2 timer base which is close enough to the actual frequency.
#if IO_DEBUG_LEVEL < 2
#define CAL_TIMER_START {}
#define CAL_TIMER_STOP {}
#else
#define CAL_TIMER_START  PkTimebase cal_start_time = pk_timebase_get(); uint16_t cal_time_us;
#define CAL_TIMER_STOP { \
        uint32_t cal_time = (uint32_t)(pk_timebase_get() - cal_start_time); \
        cal_time_us = cal_time / TIMER_US_DIVIDER; \
        mem_pg_field_put(ppe_last_cal_time_us, cal_time_us); \
    }
#endif


#ifdef IOO
    #include "eo_main_ioo.h"
#endif
#ifdef IOT
    #include "eo_main_iot.h"
#endif

#endif //_EO_MAIN_H_
