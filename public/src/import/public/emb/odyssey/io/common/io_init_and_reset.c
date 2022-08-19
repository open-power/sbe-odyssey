/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_init_and_reset.c $ */
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
// *! FILENAME    : io_init_and_reset.c
// *! TITLE       :
// *! DESCRIPTION : Functions for initialize HW Regs and Resetting lanes.
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr22061500 |vbr     | Add external command fail reporting
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21020500 |vbr     | New common version
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_init_and_reset.h"
#include "ppe_fw_reg_const_pkg.h"


////////////////////////////////////////////////////////////////////
// Reset a lane
// HW508366: This function does NOT power the lane back on
////////////////////////////////////////////////////////////////////
int io_reset_lane(t_gcr_addr* gcr_addr)
{
    int lane = get_gcr_addr_lane(gcr_addr);
    int status = rc_no_error;

    // Run TX if lane exists
    int num_lanes_tx = get_num_tx_lane_slices();

    if (lane < num_lanes_tx)
    {
        status |= io_reset_lane_tx(gcr_addr);
    }

    // Run RX if lane exists
    // Also clears per-lane mem_regs (addresses 0x00-0x0F)
    int num_lanes_rx = get_num_rx_lane_slices();

    if (lane < num_lanes_rx)
    {
        status |= io_reset_lane_rx(gcr_addr);
    }

    return status;
} //io_reset_lane



//////////////////////////////////
// Power Up/Down
//////////////////////////////////


// Power up a lane (both RX and TX)
int io_lane_power_on(t_gcr_addr* gcr_addr, bool enable_dl_clk)
{
    int lane = get_gcr_addr_lane(gcr_addr);
    int status = rc_no_error;

    // Run TX if lane exists
    int num_lanes_tx = get_num_tx_lane_slices();

    if (lane < num_lanes_tx)
    {
        status |= io_lane_power_on_tx(gcr_addr);
    }

    // Run RX if lane exists
    int num_lanes_rx = get_num_rx_lane_slices();

    if (lane < num_lanes_rx)
    {
        status |= io_lane_power_on_rx(gcr_addr, both_banks, enable_dl_clk);
    }

    return status;
} //io_lane_power_on


// Power down a lane (both RX and TX)
int io_lane_power_off(t_gcr_addr* gcr_addr)
{
    int lane = get_gcr_addr_lane(gcr_addr);
    int status = rc_no_error;

    // Run TX if lane exists
    int num_lanes_tx = get_num_tx_lane_slices();

    if (lane < num_lanes_tx)
    {
        status |= io_lane_power_off_tx(gcr_addr);
    }

    // Run RX if lane exists
    int num_lanes_rx = get_num_rx_lane_slices();

    if (lane < num_lanes_rx)
    {
        status |= io_lane_power_off_rx(gcr_addr, both_banks);
    }

    return status;
} //io_lane_power_off
