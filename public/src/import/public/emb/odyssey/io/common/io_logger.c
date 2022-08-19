/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_logger.c $  */
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
// *! FILENAME    : io_logger.c
// *! TITLE       :
// *! DESCRIPTION : Debug Logger
// *!
// *! OWNER NAME  : Chris Steffen       Email: cwsteffen@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr22060200 |vbr     | Consolidated functions to reduce code size
// vbr21012100 |vbr     | Update to work for IOT
// vbr20111700 |vbr     | Added extern keyword on linker symbol for debuglog address
// cws18071000 |cws     | Initial Rev
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
//#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "io_logger.h"

#include "ppe_img_reg_const_pkg.h"


//------------------------------------------------------------------------------
// Constant Definitions
//------------------------------------------------------------------------------
#define DEBUG_LOG_SIZE 128

extern t_log _debug_log_start __attribute__ ((section ("debuglog")));
volatile t_log* g_io_logs = &_debug_log_start;


//------------------------------------------------------------------------------
// Function Definitions
//------------------------------------------------------------------------------
void _add_log(const uint8_t i_type, t_gcr_addr* io_gcr_addr, const uint16_t i_data)
{
    // Check if the log file is full
    uint32_t l_log_num = img_field_get(ppe_debug_log_num);

    if (l_log_num >= DEBUG_LOG_SIZE - 1)
    {
        return;
    }

    // Generate a new log entry
    t_log l_new_log;
    l_new_log.type       = i_type;
    l_new_log.unit       = 0x0;
    l_new_log.lane       = 0x0;
    l_new_log.bank       = 0x0;
    l_new_log.trained    = 0x0;
    l_new_log.data       = i_data;
    l_new_log.timestamp  = (uint32_t)(pk_timebase_get() >> 10);

    // If a GCR address is provided, insert additional info
    if (io_gcr_addr != NULL)
    {
        l_new_log.unit    = get_gcr_addr_bus_id(io_gcr_addr);
        l_new_log.lane    = get_gcr_addr_lane(io_gcr_addr);
        l_new_log.trained = mem_pl_field_get(rx_init_done, l_new_log.lane);
#ifdef IOO
        // Save current reg ID and Update reg ID to the rx group so we can access rx regs
        int l_reg_id = get_gcr_addr_reg_id(io_gcr_addr);
        set_gcr_addr_reg_id(io_gcr_addr, rx_group);

        l_new_log.bank = get_ptr_field(io_gcr_addr, rx_bank_sel_a);

        // Return reg ID to original value
        set_gcr_addr_reg_id(io_gcr_addr, l_reg_id);
#endif
    }

    // Write the new log to the log file and increment the log pointer
    g_io_logs[l_log_num] = l_new_log;
    img_field_put(ppe_debug_log_num, ++l_log_num);

    return;
} //_add_log
