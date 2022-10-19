/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_ext_cmd_handlers.c $ */
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
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_ext_cmd_handlers.c
// *! TITLE       :
// *! DESCRIPTION : IOO External Command (ext_cmd) Interface Table & Handlers
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// mbs22082601 |mbs     | Updated with PSL comments
// mwh22082900 |mwh     | Issue 286972 gate llbist so does not run during pcie
// mwh22083000 |mwh     | Issue 286972 gating for llbist when in PCIE mode
// vbr22061500 |vbr     | Add external command fail reporting
// vbr22060200 |vbr     | Add lab commands to table
// vbr21120300 |vbr     | Use functions for number of lanes
// vbr21101100 |vbr     | Added sleeps at end of tx zcal, tx ffe.
// mwh21008110 |mwh     | To support removing of gcr  rx_a/b_lane_fail_0_15_16_23 and moving to using rx_lane_fail_0_15,16_23
// mwh21060700 |mwh     | changed ppe_error_valid get to pg, was img. Fixed issue 241007
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// vbr21030400 |vbr     | Split ext_cmd_lanes into rx and tx, changed max lanes to 18
// vbr21020400 |vbr     | Initial Rev
//------------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"

#include "io_ext_cmd.h"
#include "ioo_ext_cmd_handlers.h"
#include "io_logger.h"

#include "tx_zcal_tdr.h"
#include "tx_ffe.h"
#include "tx_seg_test.h"
#include "txbist_main.h"
#include "eo_rxbist_ber.h"
#include "eo_llbist.h"
#include "eo_dac_test.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"


// Use this to set debug_state levels for testing (on select debug_states which are not necessary outside initial dev)
#define EXT_CMD_DBG_LVL 3


////////////////////////////////////////////////
// External Command Handlers
////////////////////////////////////////////////

/**
 * @brief Calls Tx FFE Per-Lane
 * @param[inout] io_gcr_addr   Target Information
 * @param[in   ] i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_tx_ffe_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD09, EXT_CMD_DBG_LVL);
    set_gcr_addr_reg_id(io_gcr_addr, tx_group); // set to tx gcr address

    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_tx;
    uint32_t l_num_lanes = get_num_tx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        // PSL tx_ffe
        tx_ffe(io_gcr_addr);
        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }

    set_gcr_addr_reg_id(io_gcr_addr, rx_group); // set back to rx gcr address
    //set_gcr_addr_lane(io_gcr_addr, 0);
    return rc_no_error;
} //cmd_tx_ffe_pl


/**
 * @brief Calls Tx ZCAL Per-Lane
 * @param[inout] io_gcr_addr   Target Information
 * @param[in   ] i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_tx_zcal_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0A, EXT_CMD_DBG_LVL);
    set_gcr_addr_reg_id(io_gcr_addr, tx_group); // set to tx gcr address

    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_tx;
    uint32_t l_num_lanes = get_num_tx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        // PSL tx_zcal_tdr
        tx_zcal_tdr(io_gcr_addr);
        io_sleep(get_gcr_addr_thread(io_gcr_addr));
    }

    set_gcr_addr_reg_id(io_gcr_addr, rx_group); // set back to rx gcr address
    //set_gcr_addr_lane(io_gcr_addr, 0);
    return rc_no_error;
} //cmd_tx_zcal_pl


/**
 * @brief Calls Rx Bist Tests Per-Lane
 * @param[inout] io_gcr_addr   Target Information
 * @param[in   ] i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_rx_bist_tests_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0B, EXT_CMD_DBG_LVL);
    // TODO Write Code
    return rc_error;
} //cmd_rx_bist_tests_pl


/**
 * @brief Calls Tx Bist Tests Per-Lane
 * @param[inout] io_gcr_addr   Target Information
 * @param[in   ] i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_tx_bist_tests_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0C, EXT_CMD_DBG_LVL);
    set_gcr_addr_reg_id(io_gcr_addr, tx_group); // set to tx gcr address

    int status = rc_no_error;
    uint32_t l_tx_seg_test_en = mem_pg_field_get(tx_seg_test_en);
    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_tx;
    uint32_t l_num_lanes = get_num_tx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        // PSL txbist_main
        status |= txbist_main(io_gcr_addr);

        if (l_tx_seg_test_en == 1)
        {
            status |= tx_seg_test(io_gcr_addr);
        }
    }

    set_gcr_addr_reg_id(io_gcr_addr, rx_group); // set to rx gcr address
    return status;
} //cmd_tx_bist_tests_pl


/**
 * @brief Calls Tx RxDetect Per-Lane (Using TX TDR to detect a receiver on the other end)
 * @param[inout] io_gcr_addr   Target Information
 * @param[in]    i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_tx_rxdetect_pl(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0D, EXT_CMD_DBG_LVL);

    /*
    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_tx;
    uint32_t l_num_lanes = get_num_tx_lane_slices();
    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1) {
      if ((i_lane_shift & 0x80000000) == 0x0) continue;
      set_gcr_addr_lane(io_gcr_addr, l_lane);
      //TODO Run RxDetect - Where do we store results
    }
    */

    //set_gcr_addr_lane(io_gcr_addr, 0);
    return rc_error;
} //cmd_tx_rxdetect_pl


/**
 * @brief Calls Bist Final
 * @param[inout] io_gcr_addr   Target Information
 * @param[in   ] i_lane_mask   Lane Mask
 * @retval void
 */
int cmd_bist_final(t_gcr_addr* io_gcr_addr, const uint32_t i_lane_mask_rx, const uint32_t i_lane_mask_tx)
{
    set_debug_state(0xFD0E, EXT_CMD_DBG_LVL);
    int status = rc_no_error;

    // Run RX SIGDET BIST if enabled
    int pcie_mode = fw_field_get(fw_pcie_mode);

    if (!pcie_mode)  //only run for non pcie
    {
        // TEST LINK LAYER
        // TODO - CWS Should we run this on both banks?
        if (get_ptr_field(io_gcr_addr, rx_link_layer_check_en))
        {
            status |= eo_llbist(io_gcr_addr);
        }
    }

    // TEST PHASE ROTATOR (BER)
    if (get_ptr_field(io_gcr_addr, rx_pr_ber_check_en))
    {
        // Test Bank A
        // PSL eo_rxbist_ber_bank_a
        status |= eo_rxbist_ber(io_gcr_addr, i_lane_mask_rx, bank_a);

        // Test Bank B
        // PSL eo_rxbist_ber_bank_b
        status |= eo_rxbist_ber(io_gcr_addr, i_lane_mask_rx, bank_b);
    }

    int rx_dac_test_check_en_int = get_ptr_field(io_gcr_addr, rx_dac_test_check_en); //pg

    if(rx_dac_test_check_en_int)
    {
        status |= eo_dac_test(io_gcr_addr, i_lane_mask_rx);
    }

    // Bist Reporting Code
    set_gcr_addr_reg_id(io_gcr_addr, tx_group);
    uint32_t l_tx_fail = get_ptr_field(io_gcr_addr, tx_bist_fail_alias) == 0 ? 0x0 : 0x1;

    set_gcr_addr_reg_id(io_gcr_addr, rx_group);
    uint32_t l_rx_fail = mem_pg_field_get(rx_fail_flag) & 0x1;
    uint32_t l_rx_ll_fail = mem_pg_field_get(rx_linklayer_fail) & 0x1;
    uint32_t l_ppe_error = mem_pg_field_get(ppe_error_valid);


    mem_pg_field_put(bist_rx_fail, l_rx_fail);
    mem_pg_field_put(bist_tx_fail, l_tx_fail);
    mem_pg_field_put(bist_other_fail, (~l_rx_fail & ~l_tx_fail & l_ppe_error) & 0x1);
    mem_pg_field_put(bist_overall_pass, (~l_rx_fail & ~l_tx_fail & ~l_ppe_error) & 0x1);
    mem_pg_field_put(bist_in_progress, 0);

    uint32_t l_internal_error = 0;

    // Check Per-Lane Data
    uint32_t l_lane_fail_mask = (mem_pg_field_get            (rx_lane_fail_0_15 ) << 16) |
                                //(get_ptr_field(io_gcr_addr, rx_a_lane_fail_0_15 ) << 16) |
                                //(get_ptr_field(io_gcr_addr, rx_b_lane_fail_0_15 ) << 16) |
                                (mem_pg_field_get            (rx_lane_fail_16_23) <<  8);
    //(get_ptr_field(io_gcr_addr, rx_a_lane_fail_16_23) <<  8) |
    //(get_ptr_field(io_gcr_addr, rx_b_lane_fail_16_23) <<  8);

    uint32_t l_lane = 0;
    uint32_t i_lane_shift = i_lane_mask_rx;
    uint32_t l_num_lanes = get_num_rx_lane_slices();

    for (; l_lane < l_num_lanes; ++l_lane, i_lane_shift = i_lane_shift << 1)
    {
        if ((i_lane_shift & 0x80000000) == 0x0)
        {
            continue;
        }

        set_gcr_addr_lane(io_gcr_addr, l_lane);
        uint32_t l_rxc_fail = mem_pl_field_get(rx_step_fail_alias, l_lane) == 0 ? 0x0 : 0x1;

        // Verify that a rx circuit fail correlates to the rx lane mask fail
        l_internal_error |= (l_rxc_fail ^ ((l_lane_fail_mask >> (31 - l_lane)) & 0x1));
    }

    // Verify that a rx lane mask fail sets the fail flag (only if the link layer fail bit is low)
    l_internal_error |= (((l_lane_fail_mask == 0 ? 0x0 : 0x1) ^ l_rx_fail) & ~l_rx_ll_fail);


    // Verify the tx lane mask sets a tx fail flag
    l_lane_fail_mask = (mem_pg_field_get(tx_bist_fail_0_15 ) << 16) |
                       (mem_pg_field_get(tx_bist_fail_16_23) <<  8);
    l_internal_error |= ((l_lane_fail_mask == 0 ? 0x0 : 0x1) ^ l_tx_fail);

    // Verify that PPE ERROR is always valid when there is a rx/tx failure
    l_internal_error |= ((l_rx_fail | l_tx_fail) & ~l_ppe_error);

    mem_pg_field_put(bist_internal_error, l_internal_error);

    return status;
} //cmd_bist_final


////////////////////////////////////////////////
// External Command Table - 16 Entries
////////////////////////////////////////////////
IO_EXT_CMD_TABLE_START
IO_EXT_CMD_HANDLER(cmd_hw_reg_init_pg)    // [00]:
IO_EXT_CMD_HANDLER(cmd_ioreset_pl)        // [01]:
IO_EXT_CMD_HANDLER(cmd_nop)               // [02]: Reserved cmd_tx_rxdetect_pl
IO_EXT_CMD_HANDLER(cmd_dccal_pl)          // [03]:
IO_EXT_CMD_HANDLER(cmd_tx_zcal_pl)        // [04]:
IO_EXT_CMD_HANDLER(cmd_tx_ffe_pl)         // [05]:
IO_EXT_CMD_HANDLER(cmd_power_off_pl)      // [06]:
IO_EXT_CMD_HANDLER(cmd_power_on_pl)       // [07]:
IO_EXT_CMD_HANDLER(cmd_tx_fifo_init_pl)   // [08]:
IO_EXT_CMD_HANDLER(cmd_nop)               // [09]: Reserved cmd_rx_bist_tests_pl
IO_EXT_CMD_HANDLER(cmd_tx_bist_tests_pl)  // [10]:
IO_EXT_CMD_HANDLER(cmd_train_pl)          // [11]:
IO_EXT_CMD_HANDLER(cmd_recal_pl)          // [12]:
IO_EXT_CMD_HANDLER(cmd_bist_final)        // [13]:
IO_EXT_CMD_HANDLER(cmd_nop)               // [14]: Reserved Deskew (IOT)
IO_EXT_CMD_HANDLER(cmd_nop)               // [15]: Reserved Repair (IOT)
IO_EXT_CMD_HANDLER(cmd_lab_code_pl)       // [16]:
IO_EXT_CMD_HANDLER(cmd_lab_code_pg)       // [17]:
IO_EXT_CMD_TABLE_END

PK_STATIC_ASSERT(ext_cmd_req_hw_reg_init_pg_startbit == 0);
PK_STATIC_ASSERT(ext_cmd_req_ioreset_pl_startbit == 1);
PK_STATIC_ASSERT(ext_cmd_req_rx_detect_pl_startbit == 2);
PK_STATIC_ASSERT(ext_cmd_req_dccal_pl_startbit == 3);
PK_STATIC_ASSERT(ext_cmd_req_tx_zcal_pl_startbit == 4);
PK_STATIC_ASSERT(ext_cmd_req_tx_ffe_pl_startbit == 5);
PK_STATIC_ASSERT(ext_cmd_req_power_off_pl_startbit == 6);
PK_STATIC_ASSERT(ext_cmd_req_power_on_pl_startbit == 7);
PK_STATIC_ASSERT(ext_cmd_req_tx_fifo_init_pl_startbit == 8);
PK_STATIC_ASSERT(ext_cmd_req_rx_bist_tests_pl_startbit == 9);
PK_STATIC_ASSERT(ext_cmd_req_tx_bist_tests_pl_startbit == 10);
PK_STATIC_ASSERT(ext_cmd_req_train_pl_startbit == 11);
PK_STATIC_ASSERT(ext_cmd_req_recal_pl_startbit == 12);
PK_STATIC_ASSERT(ext_cmd_req_bist_final_pl_startbit == 13);
PK_STATIC_ASSERT(ext_cmd_req_lab_code_pl_startbit == (16 - 16));
PK_STATIC_ASSERT(ext_cmd_req_lab_code_pg_startbit == (17 - 16));
