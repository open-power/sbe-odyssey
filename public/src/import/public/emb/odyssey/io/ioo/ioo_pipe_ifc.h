/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_pipe_ifc.h $  */
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
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_pipe_ifc.h
// *! TITLE       :
// *! DESCRIPTION : Functions for handling the PIPE Interface (PCIe DL)
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// jjb22121200 |jjb     | added bool restore to pipe_preset_rx
// jjb22101400 |jjb     | added pipe_clock_change and pipe_preset_rx
// jjb21071200 |jjb     | updated MAX_NUM_PIPE_CMD to 16
// jjb21070600 |jjb     | added pipe_cmd_txdeemph_update
// jjb21052400 |jjb     | updated pipe write committed to use pipe_put_blk
// jjb21051700 |jjb     | updated for pipe v5.1.1. Removed rxeqtraining (USB only in v5.1.1).
// vbr21040500 |vbr     | Updated base access function name/prototype
// vbr21040200 |vbr     | Renamed cmd_rsvd to cmd_nop per John B
// vbr21033100 |vbr     | Added constant defs, function prototypes, etc
// vbr21020800 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#ifndef _IOO_PIPE_IFC_H_
#define _IOO_PIPE_IFC_H_

#include "io_lib.h"
#include "ioo_common.h"


/////////////////////////////////////////////////
// PIPE Register Constants
/////////////////////////////////////////////////
#define pipe_reg_sel_phy 0
#define pipe_reg_sel_mac 1

#define pipe_reg_cmd_nop                0b00
#define pipe_reg_cmd_wr_u               0b01
#define pipe_reg_cmd_wr_c               0b10
#define pipe_reg_cmd_read               0b11
#define pipe_reg_cmd_write_uncommitted  pipe_reg_cmd_wr_u
#define pipe_reg_cmd_write_committed    pipe_reg_cmd_wr_c


// PIPE PHY Regsiters
#define pipe_phy_reg_rx_margin_control_0_addr  0x000
#define pipe_phy_reg_rx_margin_control_1_addr  0x001
#define pipe_phy_reg_rx_control_0_addr         0x003
#define pipe_phy_reg_rx_control_3_addr         0x006
#define pipe_phy_reg_rx_control_4_addr         0x008
#define pipe_phy_reg_tx_control_2_addr         0x402
#define pipe_phy_reg_tx_control_3_addr         0x403
#define pipe_phy_reg_tx_control_4_addr         0x404
#define pipe_phy_reg_tx_control_5_addr         0x405
#define pipe_phy_reg_tx_control_6_addr         0x406
#define pipe_phy_reg_tx_control_7_addr         0x407
#define pipe_phy_reg_common_control_0_addr     0x800

#define pipe_phy_reg_rx_margin_control_0_reg_sel  pipe_reg_sel_phy
#define pipe_phy_reg_rx_margin_control_1_reg_sel  pipe_reg_sel_phy
#define pipe_phy_reg_rx_control_0_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_rx_control_3_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_rx_control_4_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_2_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_3_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_4_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_5_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_6_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_tx_control_7_reg_sel         pipe_reg_sel_phy
#define pipe_phy_reg_common_control_0_reg_sel     pipe_reg_sel_phy


// PIPE MAC Regsiters
#define pipe_mac_reg_rx_margin_status_0_addr     0x000
#define pipe_mac_reg_rx_margin_status_1_addr     0x001
#define pipe_mac_reg_rx_margin_status_2_addr     0x002
#define pipe_mac_reg_rx_link_eval_status_0_addr  0x00A
#define pipe_mac_reg_rx_status_4_addr            0x00C
#define pipe_mac_reg_rx_status_5_addr            0x00D
#define pipe_mac_reg_tx_status_0_addr            0x400
#define pipe_mac_reg_tx_status_1_addr            0x401
#define pipe_mac_reg_tx_status_2_addr            0x402
#define pipe_mac_reg_tx_status_3_addr            0x403
#define pipe_mac_reg_tx_status_4_addr            0x404
#define pipe_mac_reg_tx_status_5_addr            0x405
#define pipe_mac_reg_tx_status_6_addr            0x406

#define pipe_mac_reg_rx_margin_status_0_reg_sel     pipe_reg_sel_mac
#define pipe_mac_reg_rx_margin_status_1_reg_sel     pipe_reg_sel_mac
#define pipe_mac_reg_rx_margin_status_2_reg_sel     pipe_reg_sel_mac
#define pipe_mac_reg_rx_link_eval_status_0_reg_sel  pipe_reg_sel_mac
#define pipe_mac_reg_rx_status_4_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_rx_status_5_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_0_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_1_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_2_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_3_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_4_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_5_reg_sel            pipe_reg_sel_mac
#define pipe_mac_reg_tx_status_6_reg_sel            pipe_reg_sel_mac


/////////////////////////////////////////////////
// external referenced functions
/////////////////////////////////////////////////
void pipe_clock_change(t_gcr_addr* gcr_addr, uint32_t rate_one_hot);
void pipe_preset_rx(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode, bool restore);
void pipe_put_blk(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr, uint32_t cmd, uint32_t data);

/////////////////////////////////////////////////
// Functions for accessing PIPE registers
/////////////////////////////////////////////////
void     pipe_put(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr, uint32_t cmd, uint32_t data);
uint32_t pipe_get(t_gcr_addr* gcr_addr, uint32_t reg_sel, uint32_t reg_addr);

#define pipe_reg_write_committed(gcr_addr, reg, data) pipe_put_blk(gcr_addr, reg##_reg_sel, reg##_addr, pipe_reg_cmd_write_committed, data)
#define pipe_reg_write_uncommitted(gcr_addr, reg, data) pipe_put(gcr_addr, reg##_reg_sel, reg##_addr, pipe_reg_cmd_write_uncommitted, data)
#define pipe_reg_read(gcr_addr, reg) pipe_get(gcr_addr, reg##_reg_sel, reg##_addr)


////////////////////////////////////////////////////
// Constants/Functions for handling PIPE commands
////////////////////////////////////////////////////
void run_pipe_commands(t_gcr_addr* gcr_addr, uint32_t num_lanes);
void pipe_cmd_nop(t_gcr_addr* gcr_addr);
void pipe_cmd_resetn_active(t_gcr_addr* gcr_addr);
void pipe_cmd_resetn_inactive(t_gcr_addr* gcr_addr);
void pipe_cmd_rxstandby_active(t_gcr_addr* gcr_addr);
void pipe_cmd_pclkchangeack(t_gcr_addr* gcr_addr);
void pipe_cmd_rate_updated(t_gcr_addr* gcr_addr);
void pipe_cmd_powerdown_updated(t_gcr_addr* gcr_addr);
void pipe_cmd_rxeqeval(t_gcr_addr* gcr_addr);
void pipe_cmd_txdetectrx(t_gcr_addr* gcr_addr);
//void pipe_cmd_rxelecidle_active(t_gcr_addr *gcr_addr);
void pipe_cmd_rxelecidle_inactive(t_gcr_addr* gcr_addr);
//void pipe_cmd_txelecidle_active(t_gcr_addr *gcr_addr);
//void pipe_cmd_txelecidle_inactive(t_gcr_addr *gcr_addr);
void pipe_cmd_rxmargincontrol(t_gcr_addr* gcr_addr);
//void pipe_cmd_txdeemph_updated(t_gcr_addr *gcr_addr);

// Maximum number of pipe commands supported (defines size of table and number of command bits checked)
#define MAX_NUM_PIPE_CMD 16

// Macros for defining external command table and handlers
#define IOO_PIPE_CMD_HANDLER(func)  &func,
#define IOO_PIPE_CMD_TABLE_START    void (*ioo_pipe_cmd_table[MAX_NUM_PIPE_CMD])(t_gcr_addr *gcr_addr) __attribute__((section (".sdata"))) = {
#define IOO_PIPE_CMD_TABLE_END      };

// Table of PIPE command handlers
extern void (*ioo_pipe_cmd_table[MAX_NUM_PIPE_CMD])(t_gcr_addr* gcr_addr) __attribute__((section (".sdata")));

#endif //_IOO_PIPE_IFC_H_
