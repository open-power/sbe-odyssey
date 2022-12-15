/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbe_sp_intf.H $                */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
/*
 *  @file sbe_sp_intf.H
 *
 *  @brief This file contains the SP - SBE interface protocol common details
 */
#ifndef __SBEFW_SBE_SP_INTF_H
#define __SBEFW_SBE_SP_INTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/**
 * $Version: Conforms to SP-SBE Interface Spec v0.8e
 */

/*
 * Constants for SBE FW major and minor number
 */
static const uint16_t SBE_FW_MAJOR_VERSION = 1;
static const uint16_t SBE_FW_MINOR_VERSION = 4;

/**
 * @brief enums for SBE command classes
 *
*/
enum sbeCommandClass
{
    SBE_CMD_CLASS_UNKNOWN                = 0,
    SBE_CMD_CLASS_IPL_CONTROL            = 0xA1,
    SBE_CMD_CLASS_SCOM_ACCESS            = 0xA2,
    SBE_CMD_CLASS_RING_ACCESS            = 0xA3,
    SBE_CMD_CLASS_MEMORY_ACCESS          = 0xA4,
    SBE_CMD_CLASS_REGISTER_ACCESS        = 0xA5,
    SBE_CMD_CLASS_ARRAY_ACCESS           = 0xA6,
    SBE_CMD_CLASS_INSTRUCTION_CONTROL    = 0xA7,
    SBE_CMD_CLASS_GENERIC_MESSAGE        = 0xA8,
    SBE_CMD_CLASS_MPIPL_COMMANDS         = 0xA9,
    SBE_CMD_CLASS_DUMP_COMMANDS          = 0xAA,
    SBE_CMD_CLASS_HOST_FIFO_COMMANDS     = 0xAB,
    SBE_CMD_CLASS_PMIC_TELEMETRY         = 0xAC,
};

static const uint8_t SBE_CMD_UNKNOWN = 0x00;

/**
 * @brief enums for primary SBE response
 *
*/

enum sbePrimResponse
{
    SBE_PRI_OPERATION_SUCCESSFUL        = 0x00,
    SBE_PRI_INVALID_COMMAND             = 0x01,
    SBE_PRI_INVALID_DATA                = 0x02,
    SBE_PRI_USER_ERROR                  = 0x03,
    SBE_PRI_INTERNAL_ERROR              = 0x04,
    SBE_PRI_UNSECURE_ACCESS_DENIED      = 0x05,
    SBE_PRI_GENERIC_EXECUTION_FAILURE   = 0xFE,
};

/**
 * @brief enums for secondary SBE response
 *   @TODO via RTC: 129763
 *         Discuss on SBE_SEC_INVALID_TARGET_ID_PASSED
 *
 */
enum sbeSecondaryResponse
{
    SBE_SEC_OPERATION_SUCCESSFUL              = 0x00,
    SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED       = 0x01,
    SBE_SEC_COMMAND_NOT_SUPPORTED             = 0x02,
    SBE_SEC_INVALID_ADDRESS_PASSED            = 0x03,
    SBE_SEC_INVALID_TARGET_TYPE_PASSED        = 0x04,
    SBE_SEC_INVALID_CHIPLET_ID_PASSED         = 0x05,
    SBE_SEC_SPECIFIED_TARGET_NOT_PRESENT      = 0x06,
    SBE_SEC_SPECIFIED_TARGET_NOT_FUNCTIONAL   = 0x07,
    SBE_SEC_COMMAND_NOT_ALLOWED_IN_THIS_STATE = 0x08,
    SBE_SEC_FUNCTIONALITY_NOT_SUPPORTED       = 0x09,
    SBE_SEC_GENERIC_FAILURE_IN_EXECUTION      = 0x0A,
    SBE_SEC_BLACKLISTED_REG_ACCESS            = 0x0B,
    SBE_SEC_OS_FAILURE                        = 0x0C,
    SBE_SEC_FIFO_ACCESS_FAILURE               = 0x0D,
    SBE_SEC_UNEXPECTED_EOT_INSUFFICIENT_DATA  = 0x0E,
    SBE_SEC_UNEXPECTED_EOT_EXCESS_DATA        = 0x0F,
    SBE_SEC_HW_OP_TIMEOUT                     = 0x10,
    SBE_SEC_PCB_PIB_ERR                       = 0x11,
    SBE_SEC_FIFO_PARITY_ERROR                 = 0x12,
    SBE_SEC_TIMER_ALREADY_STARTED             = 0x13,
    SBE_SEC_BLACKLISTED_MEM_ACCESS            = 0x14,
    SBE_SEC_MEM_REGION_NOT_FOUND              = 0x15,
    SBE_SEC_MAXIMUM_MEM_REGION_EXCEEDED       = 0x16,
    SBE_SEC_MEM_REGION_AMEND_ATTEMPTED        = 0x17,
    SBE_SEC_INPUT_BUFFER_OVERFLOW             = 0x18,
    SBE_SEC_INVALID_PARAMS                    = 0x19,
    SBE_SEC_BLACKLISTED_CHIPOP_ACCESS         = 0x20,
    SBE_SEC_DMT_TIMEOUT                       = 0x21,
    SBE_SEC_SYSTEM_CHECKSTOP                  = 0x22,
    SBE_SEC_BLACKLISTED_REG_ACCESS_BLOCKED    = 0x23,
    SBE_SEC_S0_START_MPIPL_FAILED             = 0x24,
    SBE_SEC_S0_STOP_CLOCK_FAILED              = 0x25,
    SBE_SEC_S0_CONTINUE_MPIPL_FAILED          = 0x26,
    SBE_SEC_PERIODIC_IO_TOGGLE_FAILED         = 0x27,
    SBE_SEC_SPECIAL_WAKEUP_TIMEOUT            = 0x28,
    SBE_SEC_SPECIAL_WAKEUP_SCOM_FAILURE       = 0x29,
    SBE_SEC_S0_ARCH_REG_DUMP_FAILED           = 0x2A,
    SBE_SEC_LPC_ACCESS_FAILED                 = 0x2B,
    SBE_SEC_HWP_FAILURE                       = 0x2C,
    SBE_SEC_TI_CORE_SCRATCH_READ_FAILED       = 0x2D,
    SBE_SEC_TI_DATA_READ_FAILED               = 0x2E,
    SBE_SEC_PUT_SRAM_FAILED                   = 0x2F,
    SBE_SEC_GET_SRAM_FAILED                   = 0x30,
    SBE_SEC_THREAD_CONTROL_INSTR_FAILED       = 0x31,
    SBE_SEC_RAM_CORE_SETUP_FAILED             = 0x32,
    SBE_SEC_RAM_CORE_ACCESS_FAILED            = 0x33,
    SBE_SEC_RAM_CORE_CLEANUP_FAILED           = 0x34,
    SBE_SEC_SUSPEND_IO_FAILED                 = 0x35,
    SBE_SEC_ENTER_MPIPL_FAILED                = 0X36,
    SBE_SEC_STOP_CLOCK_FAILED                 = 0x37,
    SBE_SEC_OCMB_SCOM_FAILED                  = 0x38,
    SBE_SEC_GET_DUMP_FAILED                   = 0x39,
    SBE_SEC_INVALID_DUMP_TYPE                 = 0x40,
    SBE_SEC_INVALID_OCMB_INSTANCE             = 0x41,
    SBE_SEC_FAILED_TO_CHECKSTOP               = 0x42,
    SBE_SEC_GET_DUMP_STREAM_FAILED            = 0x43,
    SBE_SEC_MPIPL_DUMP_INVALID_PARAMS         = 0x44,
    SBE_SEC_OCMB_TARGET_NOT_PRESENT           = 0x45,
    SBE_SEC_OCMB_TARGET_NOT_FUNCTIONAL        = 0x46,
    SBE_SEC_INVALID_FASTARRAY_COLLECTION_INFO = 0x47,
    SBE_SEC_PMIC_HEALTH_CHECK_FAILED          = 0x48,
    SBE_SEC_INVALID_I2C_CONFIG_VERSION        = 0x49,
    SBE_SEC_INVALID_I2C_TARGET_SUPPORT        = 0x4A,
    SBE_SEC_LPC_ERROR                         = 0x4B,
    SBE_SEC_NOT_ALLOWED_VIA_FIFO_1            = 0x4C,
    SBE_SEC_INVALID_PIPE                      = 0x4E,
    SBE_SEC_HEAP_SPACE_FULL_FAILURE           = 0x4F,
    SBE_SEC_GENERIC_BAD_PAK                   = 0x50,
    SBE_SEC_POP_UNAVAILABE_PAKS               = 0x51,
    SBE_SEC_GETRING_SETUP_FAILED              = 0x52,
    SBE_SEC_GETRING_VERIFY_CLEANUP            = 0x53,
    SBE_SEC_PUTRING_FAILED                    = 0x54,
    SBE_SEC_CU_INVALID_IMAGE_TYPE             = 0x55,
    SBE_SEC_CU_IMAGE_HASH_SIZE_MISMATCH       = 0x56,
    SBE_SEC_CU_FILE_IMAGE_HASH_READ_ERROR     = 0x57,
    SBE_SEC_CU_PARTITION_MAGIC_WORD_MISMATCH  = 0x58,
    SBE_SEC_CU_PARTITION_MAP_INVALID_IMG_TYPE = 0x59,
    SBE_SEC_CU_IMG_NOT_FOUND_IN_PARTITION_TBL = 0x5A,
    SBE_SEC_CU_FAILED_TO_READ_PARTITION_TABLE = 0x5B,
    SBE_SEC_CU_INVALID_PARTITION_ID_PASSED    = 0x5C,
    SBE_SEC_CU_FAIL_TO_RD_PART_FILE_COMPRESSED= 0x5D,
};

/**
 * @brief enums for SBE command timeout values
 *
*/
enum sbeCmdRespTimeout
{
    SBE_CMD_TIMEOUT_SHORT_IN_MSEC = 100,
    SBE_CMD_TIMEOUT_LONG_IN_MSEC  = 30000,
};

/*
 * @brief enums for access modes used in ring access chip op
 */
typedef enum
{
    SBE_RING_MODE_NO_HEADER_CHECK           = 0x0001, ///< Don't check header
    SBE_RING_MODE_SET_PULSE_NO_OPCG_COND    = 0x0002, ///< Set pulse with
                                                     ///  no OPCG conditioning
    SBE_RING_MODE_SET_PULSE_NSL             = 0x0004, ///< Set pulse with NSL
                                                    ///  pulse
    SBE_RING_MODE_SET_PULSE_SL              = 0x0008, ///< Set pulse with SL
                                                    ///  pulse
    SBE_RING_MODE_SET_PULSE_ALL             = 0x0010, ///< Set pulse with pulse
                                                    ///  to all hold types
    SBE_RING_MODE_FASTARRAY                 = 0x0020, //Fast array mode

    SBE_RING_MODE_APPLY_OVERRIDE            = 0x0040, //override mode
} sbeRingAccessModes_t;

/*
* @brief Enum for image types supported
*/
enum class CU_IMAGES : uint16_t
{
    IMG_TYPE_INVALID=0,
    BOOTLOADER,        // Updatable
    RUNTIME,           // Updatable
    BMC_OVRD,          // Updatable
    HOST_OVRD,         // Updatable
    SROM,              // Not-updatable
    CUST,              // Not-updatable
    SPPE_RAM,          // Updatable
    IOPPE_BIN,         // Updatable
    SYNOPSYS_BIN,      // Updatable
    ARC_BIN,           // Updatable
    RSVD,              // Reserved
    END_OF_IMG_LIST
};

#ifdef __cplusplus
}
#endif

#endif /* __SBEFW_SBE_SP_INTF_H */
