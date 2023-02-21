/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_ext_cmd.h $ */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_ext_cmd.h
// *! TITLE       :
// *! DESCRIPTION : Definitions for IO External Command (ext_cmd) Interface
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr22061500 |vbr     | Add external command fail reporting
// vbr21030400 |vbr     | Split ext_cmd_lanes into rx and tx
// vbr21020400 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _IO_EXT_CMD_H_
#define _IO_EXT_CMD_H_

#include "io_lib.h"

// Maximum number of external commands supported (defines size of table and number of command bits checked)
#define MAX_NUM_EXT_CMD 18

// Macros for defining external command table and handlers
#define IO_EXT_CMD_HANDLER(func)  &func,
#define IO_EXT_CMD_TABLE_START    int (*io_ext_cmd_table[MAX_NUM_EXT_CMD])(t_gcr_addr *io_gcr_addr, uint32_t i_lane_mask_rx, uint32_t i_lane_mask_tx) __attribute__((section (".sdata"))) = {
#define IO_EXT_CMD_TABLE_END      };


// Table of external command handlers that gets defined on a per-image basis
extern int (*io_ext_cmd_table[MAX_NUM_EXT_CMD])(t_gcr_addr* io_gcr_addr, uint32_t i_lane_mask_rx,
        uint32_t i_lane_mask_tx) __attribute__((section (".sdata")));

// Default NOP command (and errors)
int cmd_nop(t_gcr_addr* io_gcr_addr, uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);

// Shared commands
int cmd_hw_reg_init_pg(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_ioreset_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_power_on_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_power_off_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_tx_fifo_init_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_dccal_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_train_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_recal_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_lab_code_pg(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);
int cmd_lab_code_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx);

// Function to check for commands and execute
void run_external_commands(t_gcr_addr* io_gcr_addr);

// Function for tracking which lanes are powered and if the group needs to be powered
void track_and_adjust_group_power(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx,
                                  const int new_power_state);

#endif //_IO_EXT_CMD_H_
