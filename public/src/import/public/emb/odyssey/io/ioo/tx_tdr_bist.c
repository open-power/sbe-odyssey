/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_tdr_bist.c $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : tx_tdr_bist.c
// *! TITLE       :
// *! DESCRIPTION : TDR (Time Domain Reflectometry) BIST test PPE code.
// *!             :
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh23091800 |mwh     | Initial Code
//------------------------------------------------------------------------------

//Mparms need to set to get tx_tdr_bist to pass
//mparm_tdr_bist_mode = 1
//mparm_tdr_enable_level = 1
//mparm_tdr_rload = 0
//mparm_tdr_rnseg = 850 (for tdr bist work rnseg and rpseg must be same value)

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ppe_fw_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"
#include "txbist_main.h"
#include "tx_tdr_bist.h"

//control signals for running bist 14 registers in / 1 out
// tc_dctest_dc              | Chip level control | 0            |in |pg
// tx_tdr_enable             | Regdef register    | 1            |in |pl
// tx_bist_txidle_pulse_mask | Regdef register    | 1            |in |pl
// tx_tdr_dac_cntl_pl(0:7)   | Regdef register    | varies       |in |pg
// tx_tdr_phase_sel          | Regdef register    | 0/1          |in |pg
// tx_tdr_pulse_offset(0:14) | Regdef register    | pulse        |in |pg
// tx_tdr_pulse_width(0:8)   | Regdef register    | pulse width  |in |pg
// tx_bist_hs_cust_sel       | Regdef register    | 0            |in |pl
// tx_bist_hs_en             | Regdef register    | 1            |in |pg
// tx_tdr_clamp_disable      | Regdef register    | 0            |in |pg
// tx_tdr_offset_ctrl        | Regdef register    | 0/1          |in |pg
// tx_pcie_rxdet_mode_dc     | Regdef register    | 0            |in |pl
// tx_nseg_main_en_full(0:15)| Regdef register    | varies       |in |pl
// tx_pseg_main_en_full(0:15)| Regdef register    | varies       |in |pl
// tx_nseg_pre1_en(0:8)      | Regdef register    | varies       |in |pl
// tx_pseg_pre1_en(0:8)      | Regdef register    | varies       |in |pl
// tx_pre1_sel(0:8)          | Regdef register    | varies       |in |pl
// tx_tdr_capt_val           | Regdef register    | 0/1          |out|pl


//24  = 0001 1000
//72  = 0100 1000
//32  = 0010 0000
//88  = 0101 1000
//56  = 0011 1000
//108 = 0110 1100
//94  = 0101 1110
//156 = 1001 1100
//134 = 1000 0110
//200 = 1100 1000
//164 = 1010 0100
//222 = 1101 1110
//176 = 1011 0000
//228 = 1110 0100

//----------------------------------------------------------------------------
// Struct and Constants for data rate dependent settigs.
// See this spreadsheet for the documented settings:
// The ordering, bit sizes, and fills of this struct are very important for proper execution and optimal code size.

PK_STATIC_ASSERT(tx_nseg_main_en_full_width == 16);
PK_STATIC_ASSERT(tx_pseg_main_en_full_width == 16);
PK_STATIC_ASSERT(tx_nseg_pre1_en_width == 9);
PK_STATIC_ASSERT(tx_pseg_pre1_en_width == 9);
PK_STATIC_ASSERT(tx_pre1_sel_width == 9);
PK_STATIC_ASSERT(tx_tdr_dac_cntl_pl_width == 8);


typedef struct struct_data_test_settings
{
    // byte 0 1 2 3
    uint16_t  tx_nseg_main_en_full : 16;
    uint16_t  tx_pseg_main_en_full : 16;

    // byte 4-5
    uint16_t  tx_nseg_pre1_en    : 9;
    uint8_t  fill_data0          : 7; // Fill space for alignment

    // byte 6-7
    uint16_t  tx_pseg_pre1_en   : 9;
    uint8_t   fill_data1        : 7; // Fill space for alignment

    // byte 8-9
    uint16_t tx_pre1_sel        : 9;
    uint8_t   fill_data2        : 7; // Fill space for alignment

    //byte 10-11
    uint8_t  tx_tdr_dac_cntl_pl : 8;
    uint8_t   fill_data3        : 8; // Fill space for alignment


} __attribute__((packed, aligned(2))) t_data_test_settings;

// 0 Setting for test part tx_tdr_dac_cntl_pl 24
const t_data_test_settings c_data_test_24 =
{
    .tx_nseg_main_en_full    = 0b0111100000000000,//x7800
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011110000,//xF0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011110000,//xF0
    .tx_tdr_dac_cntl_pl      = 0b00011000,//x18
};

// 1 Setting for test part tx_tdr_dac_cntl_pl 72
const t_data_test_settings c_data_test_72 =
{
    .tx_nseg_main_en_full    = 0b0111100000000000,//x7800
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011110000,//xF0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011110000,//xF0
    .tx_tdr_dac_cntl_pl      = 0b01001000,//x48
};

// 2 Setting for test tx_tdr_dac_cntl_pl 32
const t_data_test_settings c_data_test_32 =
{
    .tx_nseg_main_en_full    = 0b0111000000000000,//x7000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011100000,//xE0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011100000,//xE0
    .tx_tdr_dac_cntl_pl      = 0b00100000,//x20
};

// 3 Setting for test tx_tdr_dac_cntl_pl 88
const t_data_test_settings c_data_test_88 =
{
    .tx_nseg_main_en_full    = 0b0111000000000000,//x7000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011100000,//xE0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011100000,//xE0
    .tx_tdr_dac_cntl_pl      = 0b01011000,//x58
};

// 4 Setting for test tx_tdr_dac_cntl_pl 56
const t_data_test_settings c_data_test_56 =
{
    .tx_nseg_main_en_full    = 0b0110000000000000,//x6000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011000000,//xC0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011000000,//xC0
    .tx_tdr_dac_cntl_pl      = 0b00111000,//x38
};

// 5 Setting for test tx_tdr_dac_cntl_pl 108
const t_data_test_settings c_data_test_108 =
{
    .tx_nseg_main_en_full    = 0b0110000000000000,//x6000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b011000000,//xC0
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b011000000,//xC0
    .tx_tdr_dac_cntl_pl      = 0b01101100,//x6C
};

// 6 Setting for test tx_tdr_dac_cntl_pl 94
const t_data_test_settings c_data_test_94 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b010000000,//x80
    .tx_tdr_dac_cntl_pl      = 0b01011110,//5E
};

// 7 Setting for test tx_tdr_dac_cntl_pl 156
const t_data_test_settings c_data_test_156 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b010000000,//x80
    .tx_pre1_sel             = 0b010000000,//x80
    .tx_tdr_dac_cntl_pl      = 0b10011100,//x9C
};

// 8 Setting for test tx_tdr_dac_cntl_pl 134
const t_data_test_settings c_data_test_134 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0110000000000000,//x6000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011000000,//xC0
    .tx_pre1_sel             = 0b011000000,//xC0
    .tx_tdr_dac_cntl_pl      = 0b10000110,//x86
};

// 9 Setting for test tx_tdr_dac_cntl 200
const t_data_test_settings c_data_test_200 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0110000000000000,//x6000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011000000,//xC0
    .tx_pre1_sel             = 0b011000000,//xC0
    .tx_tdr_dac_cntl_pl      = 0b11001000,//xC8
};

// 10 Setting for test tx_tdr_dac_cntl_pl 164
const t_data_test_settings c_data_test_164 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0111000000000000,//x7000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011100000,//xE0
    .tx_pre1_sel             = 0b011100000,//xE0
    .tx_tdr_dac_cntl_pl      = 0b10100100,//xA4
};

// 11 Setting for test tx_tdr_dac_cntl_pl 222
const t_data_test_settings c_data_test_222 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0111000000000000,//x7000
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011100000,//xE0
    .tx_pre1_sel             = 0b011100000,//xE0
    .tx_tdr_dac_cntl_pl      = 0b11011110,//xDE
};

// 12 Setting for test tx_tdr_dac_cntl_pl 176
const t_data_test_settings c_data_test_176 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0111100000000000,//x7800
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011110000,//xF0
    .tx_pre1_sel             = 0b011110000,//xF0
    .tx_tdr_dac_cntl_pl      = 0b10110000,//xB0
};

// 13 Setting for test tx_tdr_dac_cntl_pl 228
const t_data_test_settings c_data_test_228 =
{
    .tx_nseg_main_en_full    = 0b0100000000000000,//x4000
    .tx_pseg_main_en_full    = 0b0111100000000000,//x7800
    .tx_nseg_pre1_en         = 0b010000000,//x80
    .tx_pseg_pre1_en         = 0b011110000,//xF0
    .tx_pre1_sel             = 0b011110000,//xF0
    .tx_tdr_dac_cntl_pl      = 0b11100100,//xE4
};



// Array of pointers to the data rate setting constants
const t_data_test_settings* c_data_test_settings[14] =
{
    & c_data_test_24,  // 0 =
    & c_data_test_72,  // 1 =
    & c_data_test_32,  // 2 =
    & c_data_test_88,  // 4 =
    & c_data_test_56,  // 3 =
    & c_data_test_108, // 5 =
    & c_data_test_94,  // 6 =
    & c_data_test_156, // 7 =
    & c_data_test_134, // 8 =
    & c_data_test_200, // 9 =
    & c_data_test_164, // 10 =
    & c_data_test_222, // 11 =
    & c_data_test_176, // 12 =
    & c_data_test_228, // 13 =
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

int tx_tdr_bist(t_gcr_addr* gcr_addr)
{
    //start tx_tdr_bist
    set_debug_state(0XC400); // Start of TX_TDR_BIST
    uint32_t status = rc_no_error;

//began init of signals common to all runs
    put_ptr_field(gcr_addr, tx_bist_hs_cust_sel, 0b000 , read_modify_write); //pl not need
    put_ptr_field(gcr_addr, tx_bist_hs_en,       0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_clamp_disable, 0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_offset_ctrl,  0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_pulse_offset, 0b000000000000000 ,
                  fast_write); //pg delays when pulse go high after tdr_raw_counter get 0
    put_ptr_field(gcr_addr, tx_tdr_pulse_width , 0b000001000,
                  read_modify_write); //pg width of pulse; period is 4*pulse_width*grid clk period

    put_ptr_field(gcr_addr, tx_pseg_pre2_en, 0b00000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_nseg_pre2_en, 0b00000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_pseg_post_en, 0b000000000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_nseg_post_en, 0b000000000 , read_modify_write); //pl

    //enable tdr circuit
    put_ptr_field(gcr_addr, tx_tdr_enable, 0b1 , read_modify_write); //pl

    uint32_t l_pcie_mode = fw_field_get(fw_pcie_mode);

    if (l_pcie_mode == 1)
    {
        put_ptr_field(gcr_addr, tx_pcie_rxdet_mode_dc, 0b0 , read_modify_write);   //pl
    }

//end init of signals common to all runs

    int pass_fail_value = 0;
    uint32_t l_test_rate;
    uint32_t l_tdr_offset_ctrl;
    uint32_t l_tdr_phase_sel;
    uint32_t l_test_rate_end = 14;
    uint32_t l_tdr_offset_ctrl_end = 2;
    uint32_t l_tdr_phase_sel_end = 2;
    uint32_t result = 0;

    for (l_tdr_offset_ctrl = 0; l_tdr_offset_ctrl  < l_tdr_offset_ctrl_end ;
         l_tdr_offset_ctrl++) //begin for tdr_offset_ctrl
    {
        for (l_tdr_phase_sel = 0; l_tdr_phase_sel < l_tdr_phase_sel_end ; l_tdr_phase_sel++) //begin for tdr_phase_sel
        {
            for (l_test_rate = 0; l_test_rate < l_test_rate_end; l_test_rate++)  //begin for 13 segment changes
            {


                put_ptr_field(gcr_addr, tx_bist_txidle_pulse_mask, 0b0 , read_modify_write); //pl

                if ((l_test_rate & 0b1) == 0b1)
                {
                    pass_fail_value = 0;
                }
                else
                {
                    pass_fail_value  = 1;
                }

                //Variable input values
                const t_data_test_settings* t_data_test_settings = c_data_test_settings[l_test_rate];

                put_ptr_field(gcr_addr, tx_nseg_main_en_full, t_data_test_settings->tx_nseg_main_en_full, read_modify_write); //pl
                put_ptr_field(gcr_addr, tx_pseg_main_en_full, t_data_test_settings->tx_pseg_main_en_full, read_modify_write); //pl
                put_ptr_field(gcr_addr, tx_nseg_pre1_en,     t_data_test_settings->tx_nseg_pre1_en     , read_modify_write); //pl
                put_ptr_field(gcr_addr, tx_pseg_pre1_en,     t_data_test_settings->tx_pseg_pre1_en     , read_modify_write); //pl
                put_ptr_field(gcr_addr, tx_pre1_sel,         t_data_test_settings->tx_pre1_sel         , read_modify_write); //pl
                put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl,  t_data_test_settings->tx_tdr_dac_cntl_pl  , read_modify_write); //pl

                io_spin(25);//Let circuit settle

                int l_tx_tdr_capt_val = get_ptr_field(gcr_addr, tx_tdr_capt_val ); //pass fail value

                // PSL not_test_stat
                if ((l_tx_tdr_capt_val == pass_fail_value) && (result == 0))
                {
                    result = 0; //pass
                }
                else
                {
                    result = 1;   //fail this is sticky
                }

            }//end for 13 segment changes
        }//end for tdr_phase_sel
    }//end for tdr_offset_ctrl

    put_ptr_field(gcr_addr, tx_tdr_enable, 0b0 , read_modify_write); //End test

    uint32_t lane = get_gcr_addr_lane(gcr_addr);

    if (result) //begin if
    {
        uint32_t new_lane_bad;

        if (lane < 16)  //begin if
        {
            new_lane_bad = mem_pg_field_get(tx_bist_fail_0_15) | (0b1 << (15 - lane));
            mem_pg_field_put(tx_bist_fail_0_15, new_lane_bad);
        }//end if
        else  //begin else
        {
            new_lane_bad = mem_pg_field_get(tx_bist_fail_16_23) | (0b1 << (23 - lane));
            mem_pg_field_put(tx_bist_fail_16_23, new_lane_bad);
        }//end else
    }//end if


    mem_pl_field_put(tx_tdr_bist_fail, lane, result);
    mem_pl_field_put(tx_tdr_bist_done, lane, 0b1); // update bank a done bit even though there is only a single tdr bist

    //----------Reseting all registers-----------------//

    put_ptr_field(gcr_addr, tx_nseg_main_en_full, 0b1111111111111111, fast_write); //pl
    put_ptr_field(gcr_addr, tx_pseg_main_en_full, 0b1111111111111111, fast_write); //pl


    //does both tx_nseg_pre1_en and tx_nseg_pre1_hs_en
    put_ptr_field(gcr_addr, tx_nseg_pre1_en, 0b111111111, fast_write); //pl
    put_ptr_field(gcr_addr, tx_nseg_pre2_hs_en, 0b11111, fast_write); //pl

    //does both tx_pseg_pre1_en and tx_pseg_pre1_hs_en
    put_ptr_field(gcr_addr, tx_pseg_pre1_en, 0b111111111, fast_write); //pl
    put_ptr_field(gcr_addr, tx_pseg_pre2_hs_en, 0b11111, fast_write); //pl

    put_ptr_field(gcr_addr, tx_tdr_dac_cntl_pl, 0b00000000, read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_bist_hs_cust_sel, 0b000 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_bist_hs_en, 0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_clamp_disable, 0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_offset_ctrl, 0b0 , read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_pulse_offset, 0b000000000000000 , fast_write); //pg
    put_ptr_field(gcr_addr, tx_tdr_pulse_width , 0b000000000, read_modify_write); //pg
    put_ptr_field(gcr_addr, tx_pseg_pre2_en, 0b11111 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_nseg_pre2_en, 0b11111 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_pseg_post_en, 0b111111111 , read_modify_write); //pl
    put_ptr_field(gcr_addr, tx_nseg_post_en, 0b111111111 , read_modify_write); //pl
    //put_ptr_field(gcr_addr, tx_tdr_enable, 0b1 ,read_modify_write);//pl up above
    put_ptr_field(gcr_addr, tx_bist_txidle_pulse_mask, 0b0 , read_modify_write); //pl

    if (l_pcie_mode == 1)
    {
        put_ptr_field(gcr_addr, tx_pcie_rxdet_mode_dc, 0b0 , read_modify_write);   //pl
    }

    //----------Reseting all registers done-----------------//


    if (result)  //begin if
    {
        // PSL set_fir_bad_lane_warning_and_dft_error
        set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        ADD_LOG(DEBUG_BIST_TDR_FAIL, gcr_addr, result);
    }//end if

    set_debug_state(0XC401); // End TX_TDR_BIST


    return status;
}//end rx_tdr_bist
