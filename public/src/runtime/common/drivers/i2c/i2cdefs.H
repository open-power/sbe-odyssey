/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/drivers/i2c/i2cdefs.H $             */
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

#pragma once

#include <stdint.h>

/**
 * @brief 0xA is the PIB responder ID and
 *        0x10000 is the local reg space
 *        This holds good for all platforms
 */
#define I2C_MASTER_PIB_BASE_ADDR 0xA0000

#define ENGINE_SCOM_OFFSET 0x1000
#define SIM_INTERVAL_DELAY  320000

/**
 *  @brief Number of nanoseconds per second
 */
#define NS_PER_SEC (1000000000ull)

/**
 *  @brief Number of nanoseconds per milisecond
 */
#define NS_PER_MSEC (1000000ull)

/**
 * @brief Determine I2C Timeout Count based on I2C_MAX_WAIT_TIME_NS and
 *         I2C Polling Interval (in ns)
 */
#define I2C_MAX_WAIT_TIME_NS (20 * NS_PER_MSEC)

#define I2C_TIMEOUT_COUNT(i_interval_ns) (I2C_MAX_WAIT_TIME_NS / i_interval_ns)

/**
 * @brief FIFO capacity in bytes.  This dictates the maximum number
 *      of bytes that the FIFO can hold.
 */
#define I2C_MAX_FIFO_CAPACITY 8

/**
 * @brief I2C Engine offsets
 */
enum i2c_engine_offset_t
{
    I2C_ENGINE_B = 0x0,  //Engine 0
    I2C_ENGINE_C = 0x1,  //Engine 1
    I2C_ENGINE_D = 0x2,  //Engine 2
    I2C_ENGINE_E = 0x3,  //Engine 3
};

/**
 * @brief I2C Ports. There can be a max of 64 ports per engine.
 */
enum i2c_port_t
{
    I2C_PORT_0 = 0x0,
    I2C_PORT_1 = 0x1,
};

/**
 * @brief I2C Register Offsets
 */
enum i2c_reg_offset_t
{
    I2C_REG_FIFO        = 0x4,
    I2C_REG_COMMAND     = 0x5,
    I2C_REG_MODE        = 0x6,
    I2C_REG_INTMASK     = 0x8,
    I2C_REG_INTERRUPT   = 0xA,
    I2C_REG_STATUS      = 0xB,
    I2C_REG_RESET       = 0xB,
    I2C_REG_RESET_ERRORS= 0xC,
    I2C_REG_SET_SCL     = 0xD,
    I2C_REG_RESET_SCL   = 0xE,
    I2C_REG_SET_SDA     = 0x10,
    I2C_REG_RESET_SDA   = 0x11,
    I2C_REG_ATOMIC_LOCK = 0x3FF,
};

/**
 * @brief Status register definition
 */
union status_reg_t
{
    uint64_t value;
    struct
    {
        uint64_t invalid_cmd : 1;
        uint64_t lbus_parity_error : 1;
        uint64_t backend_overrun_error : 1;
        uint64_t backend_access_error : 1;
        uint64_t arbitration_lost_error : 1;
        uint64_t nack_received : 1;
        uint64_t data_request : 1;
        uint64_t command_complete : 1;
        uint64_t stop_error : 1;
        uint64_t max_num_of_ports : 7;
        uint64_t any_i2c_interrupt : 1;
        uint64_t waiting_for_i2c_busy : 1;
        uint64_t const_zero0 : 1;
        uint64_t i2c_port_history_busy : 1;
        uint64_t scl_input_level : 1;
        uint64_t sda_input_level : 1;
        uint64_t i2c_port_busy : 1;
        uint64_t i2c_interface_busy : 1;
        uint64_t const_zero1 : 4;
        uint64_t fifo_entry_count : 4;
        uint64_t peek_data1 : 8;
        uint64_t lbus_parity_error1 : 1;
        uint64_t const_zero2 : 3;
        uint64_t i2cm_steered_interrupts : 4;
        uint64_t external_status : 6;
        uint64_t const_zero3 : 10;
    };
};

/**
 * @brief Interrupt register definition
 */
union interrupt_reg_t
{
    uint64_t value;
    struct
    {
        uint64_t reserved0 : 16;
        uint64_t invalid_cmd : 1;
        uint64_t lbus_parity_error : 1;
        uint64_t backend_overrun_error : 1;
        uint64_t backend_access_error : 1;
        uint64_t arbitration_lost_error : 1;
        uint64_t nack_received_error : 1;
        uint64_t data_request : 1;
        uint64_t command_complete : 1;
        uint64_t stop_error : 1;
        uint64_t i2c_busy : 1;
        uint64_t not_i2c_busy : 1;
        uint64_t reserved1 : 1;
        uint64_t scl_eq_1 : 1;
        uint64_t scl_eq_0 : 1;
        uint64_t sda_eq_1 : 1;
        uint64_t sda_eq_0 : 1;
        uint64_t padding: 32;
    };
};

/**
 * @brief I2C Command register definition
 */
union command_reg_t
{
    uint64_t value;
    struct
    {
        uint64_t with_start : 1;
        uint64_t with_addr : 1;
        uint64_t read_continue : 1; // Not Supported at this time
        uint64_t with_stop : 1;
        uint64_t reserved : 4;
        uint64_t device_addr : 7;
        uint64_t read_not_write : 1;
        uint64_t length_b : 16;
        uint64_t padding : 32;
    };
};

/**
 * @brief I2C Mode register definition
 */
union mode_reg_t
{
    uint64_t value;
    struct
    {
        uint64_t bit_rate_div : 16;
        uint64_t port_num : 6;
        uint64_t reserved : 6;
        uint64_t enhanced_mode : 1;
        uint64_t diag_mode : 1;
        uint64_t pacing_allow_mode : 1;
        uint64_t wrap_mode : 1;
        uint64_t padding : 32;
    };
};

union fifo_reg_t
{
    uint64_t value;
    struct
    {
        uint64_t byte_0 : 8;
        uint64_t padding : 56;
    };
};

enum fifo_condition_t
{
    FIFO_FREE,
    FIFO_DATA_AVAILABLE
};
