/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_init_and_reset.h $ */
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
// *! FILENAME    : ioo_init_and_reset.h
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
// gap22062200 |gap     | Repartitioned pipe_clock_change moving clock_change_tx here
// jjb22042000 |jjb     | Added Setup Rate Dependent Analog Controls
// vbr21030400 |vbr     | Split group power on/off into rx and tx
// mbs19111100 |mbs     | Added enable_dl_clk flag to io_lane_power_on (HW508366)
// vbr17120100 |vbr     | Renamed file and added new function.
// vbr16101200 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _IOO_INIT_AND_RESET_H_
#define _IOO_INIT_AND_RESET_H_

#include "io_lib.h"

// Initialize HW Regs
void io_hw_reg_init(t_gcr_addr* gcr_addr);

// Setup Rate Dependent Analog Controls (both RX and TX)
void update_tx_rate_dependent_analog_ctrl_pl_regs(t_gcr_addr* gcr_addr, uint32_t ovr_rate_one_hot);
void update_rx_rate_dependent_analog_ctrl_pl_regs(t_gcr_addr* gcr_addr);
void clock_change_tx(t_gcr_addr* gcr_addr, uint32_t rate_one_hot);

// Power up a group (both RX and TX)
void io_group_power_on(t_gcr_addr* gcr_addr);
void io_group_power_on_rx(t_gcr_addr* gcr_addr);
void io_group_power_on_tx(t_gcr_addr* gcr_addr);

// Power down a group (both RX and TX)
void io_group_power_off(t_gcr_addr* gcr_addr);
void io_group_power_off_rx(t_gcr_addr* gcr_addr);
void io_group_power_off_tx(t_gcr_addr* gcr_addr);

// Lane power and reset
int io_reset_lane_rx(t_gcr_addr* gcr_addr);
int io_reset_lane_tx(t_gcr_addr* gcr_addr);

int io_lane_power_on_rx(t_gcr_addr* gcr_addr, t_power_banks_sel banks_sel, bool enable_dl_clk);
int io_lane_power_on_tx(t_gcr_addr* gcr_addr);
int io_lane_power_off_rx(t_gcr_addr* gcr_addr, t_power_banks_sel banks_sel);
int io_lane_power_off_tx(t_gcr_addr* gcr_addr);

#endif //_IOO_INIT_AND_RESET_H_
