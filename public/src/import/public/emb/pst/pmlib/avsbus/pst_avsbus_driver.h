/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/avsbus/pst_avsbus_driver.h $ */
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
#ifndef __PGPE_AVSBUS_DRIVER_H__
#define __PGPE_AVSBUS_DRIVER_H__

//#include "pgpe.h"
//#include "pgpe_pstate.h"

#include <stdint.h>
#include <pst_ppb_avsvrm.H>
#include "pst_ppb_global_base.H"
#define MAX_POLL_COUNT_AVS      10
#define CLOCK_SPIVID_MHZ        10
#define AVS_DRIVER_MAX_EXTERNAL_VOLTAGE  1350
#define AVS_DRIVER_MIN_EXTERNAL_VOLTAGE  500
#define O2S_BUSNUM_OFFSET_SHIFT  8
#define AVS_CURRENT_READ_ERROR_THRESHOLD 3

extern GPPBAVSBusVRM_t* G_gppb_avsbus;
extern GPPBBase_t*      G_gppb_base;
//AVS bus VRM data
#define gppb_get_avs_bus_vrm_data(idx,x)        G_gppb_avsbus->rails[idx].x
// Current Scaling Factor
#define gppb_get_avs_bus_current_factor(idx)    G_gppb_avsbus->current_scaling_factor[idx]
//Base data
#define gppb_get_base_data(x)                   G_gppb_base->x

enum AVSBUS_CMDS
{
    AVS_CMD_VOLTAGE_RW                  = 0x0,
    AVS_CMD_VOLTAGE_TRANSITION_RATE_RW  = 0x1,
    AVS_CMD_CURRENT_READ                = 0x2,
    AVS_CMD_TEMPURATURE_READ            = 0x3,
    AVS_CMD_VOLTAGE_RESET_W             = 0x4,
    AVS_CMD_POWER_MODE_RW               = 0x5,
    AVS_CMD_STATUS_RW                   = 0xE,
    AVS_CMD_VERSION_R                   = 0xF
};

// Status Frame: 0:1 Slave Ack encode (with 0b00 being a "good" value)
//                  2   Reserved
//                  3:7 Status Response
//                      3: Vdone
//                      4: Status Alert,
//                      5: AVSControl
//                      6-7: MfgSpecific
enum AVSBUS_SLAVE_ACK_MASK
{
    AVS_ACK_PREFIX                      = 0xC0000000,
    AVS_ACK_VDONE                       = 0x10000000,
    AVS_ACK_STAT_ALERT                  = 0x08000000,
    AVS_ACK_BUS_CONTROL                 = 0x04000000,
    AVS_ACK_MFG1                        = 0x02000000,
    AVS_ACK_MFG2                        = 0x01000000
};

enum AVSBUS_READ_STATUS_MASK
{
    AVS_STAT_VDONE                      = 0x8000,
    AVS_STAT_OCW                        = 0x4000,
    AVS_STAT_UCW                        = 0x2000,
    AVS_STAT_OTW                        = 0x1000,
    AVS_STAT_OPW                        = 0x0800
};


enum AVSBUS_DRIVER_RETURN_CODES
{
    AVS_RC_SUCCESS                      = 0,
    AVS_RC_ONGOING_TIMEOUT              = 1,
    AVS_RC_RESYNC_ERROR                 = 2,
    AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL   = 3,
    AVS_RC_NO_ACTION                    = 4,
    AVS_RC_VDONE_TIMEOUT                = 5,
    AVS_RC_VDONE_ASSERTED               = 6
};

typedef struct Avs_profile
{
    uint64_t total_time;
    uint32_t cnt;
    uint32_t max_time;
    uint32_t min_time;
    uint32_t avg_time;
} Avs_profile_t;

typedef struct Avsbus
{
    uint32_t voltage_zero_cnt;
    uint32_t current_zero_cnt;

    uint16_t idd_current_thrshd;
    uint16_t ics_current_thrshd;
    uint16_t occ_cyc_time_ps;
    uint16_t pad1;

    uint32_t timebase_tick_ns;
    uint16_t start_dly_ns[2];           // 0: VDD; 1: VCS

    uint16_t stabl_dly_ns[2];
    uint16_t incr_dly_uv_per_us[2];     // Put in Scoreboard for easy access

    uint16_t decr_dly_uv_per_us[2];     // Put in Scoreboard for easy access
    uint16_t start_dly_tb[2];

    uint16_t stabl_dly_tb[2];
    uint16_t incr_dly_tb[2];

    uint16_t decr_dly_tb[2];
    uint16_t dly_tb[2];

    uint16_t incr_to_dly_tb[2];
    uint16_t decr_to_dly_tb[2];

    uint16_t delta_tb[2];
    uint16_t to_dly_mult;
    uint16_t pad2;

    Avs_profile_t voltage_write[2];
} Avsbus_t;

/**
 * @brief  Initialises the AVS Bus interface for a given tap instance.
 * @param[in]   i_tap_id    instance of tap chip
 */
void avsbus_init( uint32_t i_tap_id );

/**
 * @brief   returns OCC SRAM address of
 */
void* avsbus_data_addr();
/**
 * @brief   initialises the AVS bus instance.
 * @param[in]   instance of AVS bus
 */
void avsbus_init_bus( uint32_t i_bus_num );

/**
 * @brief   interface to write a given voltage value to a rail using AVS bus instance
 * @param[in]   i_bus_num   instance of AVS bus
 * @param[in]   i_rail_num  voltage rail instance to be written
 * @param[in]   i_volt_mv   voltage value to be written
 * @param[in]   i_delta_volt_mv
 * @param[in]   i_rail      voltage rail type
 * @return
 */
uint32_t avsbus_voltage_write( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t i_volt_mv,  int32_t i_delta_volt_mv,
                               uint32_t i_rail );
/**
 * @brief   interface to read voltage value for a given rail using AVS bus instance
 * @param[in]   i_bus_num   instance of AVS bus
 * @param[in]   i_rail_num  voltage rail instance to be written
 * @param[in]   i_ret_volt  voltage value read back
 * @return      AVS_RC_SUCCESS if function aucceeds, error code otherwise
 */
uint32_t avsbus_voltage_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* i_ret_volt );
/**
 * @brief   interface to read current value for a given rail using AVS bus instance
 * @param[in]   i_bus_num       instance of AVS bus
 * @param[in]   i_rail_num      voltage rail instance to be written
 * @param[in]   i_ret_current   current value read back
 * @param[in]   i_current_scale_idx
 * @return
 */
void avsbus_current_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* i_ret_current,
                          uint32_t i_current_scale_idx );
/**
 * @brief   interface to read a given voltage value to a rail using AVS bus instance
 * @param[in]   i_bus_num   instance of AVS bus
 * @param[in]   i_rail_num  voltage rail instance to be written
 * @param[in]   i_ret_temp  temp value read back
 * @param[in]   i_current_scale_idx
 * @return      AVS_RC_SUCCESS if function aucceeds, error code otherwise
 */
void avsbus_temp_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* i_ret_temp,
                       uint32_t i_current_scale_idx );

/**
 * @brief   initialises data structs pertaining to AVS bus.
 */
void init_avs_bus_setup();
#endif
