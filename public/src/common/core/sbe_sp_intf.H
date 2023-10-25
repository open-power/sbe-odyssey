/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/core/sbe_sp_intf.H $                        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2024                        */
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * $Version: Confirms to SP-SBE Interface Spec v0.2e
 * Constants for SBE FW major and minor number
 */
static const uint16_t SBE_FW_MAJOR_VERSION = 1;
static const uint16_t SBE_FW_MINOR_VERSION = 2;

static const uint8_t INFO_TXT_MAJOR_VERSION = 0;
static const uint8_t INFO_TXT_MINOR_VERSION = 1;

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

// JIRA: PFSBE-439 -> Cmd Class and Cmd Opcode should be generated
/**
 * @brief enums for SCOM Access Messages
 *
*/
enum sbeScomAccessCommands
{
    SBE_CMD_GETSCOM          = 0x01, /* Get SCOM */
    SBE_CMD_PUTSCOM          = 0x02, /* Put SCOM */
};

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
 *         Discuss on SBE_SEC_INVALID_CHIPLET_ID_PASSED
 *
 */
enum sbeSecondaryResponse
{
    SBE_SEC_OPERATION_SUCCESSFUL              = 0x00,
    SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED       = 0x01,
    SBE_SEC_COMMAND_NOT_SUPPORTED             = 0x02,
    SBE_SEC_INVALID_ADDRESS_PASSED            = 0x03,
    SBE_SEC_INVALID_TARGET_TYPE_PASSED        = 0x04,
    SBE_SEC_INVALID_INSTANCE_ID_PASSED        = 0x05,
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
    SBE_SEC_CU_INVALID_IMAGE_SIZE             = 0x5C,
    SBE_SEC_CU_IMAGE_NOT_UPDATABLE            = 0x5D,
    SBE_SEC_CU_IMAGE_START_POINTER_FAILED     = 0x5E,
    SBE_SEC_CU_IMAGE_SIZE_MORE_THAN_MAX_SIZE  = 0x5F,
    SBE_SEC_CU_INVALID_PARTITION_ADDR_RXVD    = 0x60,
    SBE_SEC_CU_UPDATE_IMAGE_FAILURE           = 0x61,
    SBE_SEC_CU_IMAGE_SIZE_NOT_8BYTE_ALIGNED   = 0x62,
    SBE_SEC_CU_INVALID_SIDE_NUMBER_PASSED     = 0x63,
    SBE_SEC_CU_PARTITION_TABLE_IS_EMPTY       = 0x64,
    SBE_SEC_CU_FAILED_TO_READ_COMPRESSED_FILE = 0x65,
    SBE_SEC_CU_PTBL_TOTAL_IMG_SIZE_INVALID    = 0x66,
    SBE_SEC_CU_PAK_END_MARKER_NOT_FOUND       = 0x67,
    SBE_SEC_CU_FILE_SIZE_NOT_MATCHING         = 0x68,
    SBE_SEC_ODY_DTS_READ_FAILED               = 0x69,
    SBE_SEC_CU_SECTION_START_ADDR_MISMATCH    = 0x6A,
    SBE_SEC_HEAP_BUFFER_ALLOC_FAILED          = 0x6B,
    SBE_SEC_CU_IMAGE_HIGH_ORDER_ADDR_MISMATCH = 0x6C,
    SBE_SEC_CU_IMG_ADDR_N_SZ_EXCEEDS_SIDE_SZ  = 0x6D,
    SBE_SEC_CU_WRITE_BEGIN_IMAGE_FAILURE      = 0x6E,
    SBE_SEC_CU_WRITE_END_IMAGE_FAILURE        = 0x6F,
    SBE_SEC_FILE_CORRUPTED                    = 0x70,
    SBE_SEC_FILE_NOT_FOUND                    = 0x71,
    SBE_SEC_CU_BOTH_SIDES_UPDATE_FAILED       = 0x72,
    SBE_SEC_CU_SIDE_1_UPDATE_FAILED           = 0x73,
    SBE_SEC_CU_SIDE_0_UPDATE_FAILED           = 0x74,
    SBE_SEC_CHIPLET_ID_NOT_PRESENT_IN_SBE     = 0x75,
    SBE_SEC_CU_INVALID_PAD_SIZE               = 0X76,
    SBE_SEC_CU_SYNC_CHIPOP_FAILURE            = 0x77,
    SBE_SEC_CU_READ_IMAGE_FAILURE             = 0x78,
    SBE_SEC_ATTR_UPD_FORMAT_ERROR             = 0x79,
    SBE_SEC_ATTR_UPD_VER_NOT_SUPPORTED        = 0x7A,
    SBE_SEC_ATTR_UPD_INVALID_FILE_TYPE        = 0x7B,
    SBE_SEC_UNALIGNED_ADDRESS_PASSED          = 0x7C,
    SBE_SEC_INVALID_LENGTH_PASSED             = 0x7D,
    SBE_SEC_GIT_COMMIT_ID_FAILURE             = 0x7E,
    SBE_SEC_GIT_TIME_STAMP_FAILURE            = 0x7F,
    SBE_SEC_INVALID_CLOCK_STATE               = 0x80,
    SBE_SEC_INFO_TXT_FORMAT_INVALID           = 0x81,
    SBE_SEC_INFO_TXT_FORMAT_VERSION_MISMATCH  = 0x82,
    SBE_SEC_FILE_SIZE_IS_ZERO                 = 0x83,
    SBE_SEC_BUFFER_SIZE_SMALL                 = 0x84,
    SBE_SEC_NON_ALPHA_NUMERIC_CHAR_FOUND      = 0x85,
    SBE_SEC_NO_MEM_TRAIN_DATA                 = 0x86,
    SBE_SEC_FAST_ARRAY_HWP_FAILURE            = 0x87,
    SBE_SEC_CU_MEM_DEV_ID_NOT_SUPPORTED       = 0x88,
    SBE_SEC_CU_UNKNOWN_DEVICE                 = 0x89,
    SBE_SEC_CU_INVALID_BOOT_SIDE              = 0x8A,
    SBE_SEC_CU_ECC_USE_DISABLED               = 0x8B,
    SBE_SEC_CU_WRITE_DATA_IMAGE_FAILURE       = 0x8C,
    SBE_SEC_CU_READ_DATA_IMAGE_FAILURE        = 0x8D,
    SBE_SEC_ITS_A_BUG                         = 0x8E,
    SBE_SEC_ATTR_UPD_BLOB_GEN_FAILED          = 0x8F,
    SBE_SEC_CHK_UPDATE_BLOB_GEN_TARGET_TYPES  = 0x90,
    SBE_SEC_TEST_FFDC_SUCCESSFUL              = 0x91,
    SBE_SEC_TEST_FFDC_INVALIDMODE             = 0x92,
    SBE_SEC_BAD_TESTFFDC_ERR_RECORD           = 0x93,
    SBE_SEC_TESTFFDC_FAILED                   = 0x94,
    SBE_TESTFFDC_INVALID_FFDC_RC              = 0x95,
    SBE_SEC_RAS_INVALID_SCOPE_CHECK_VALUE     = 0x96,
    SBE_SEC_RAS_INVALID_SIDE_VALUE            = 0x97,
    SBE_SEC_RAS_INVALID_DEVICE_ID             = 0x98,
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
    CUSTOM,            // Updatable
    CUSTOM_TBL,
    MISC,
    PAD,
    RUNTIME2,          // Updatable
    CUSTOM_BACKUP,
    END_OF_CU_IMAGES,

    // Virtual images, only used for getCapability
    // Start at a high number to leave space for expanding the CodeUpdate images
    SROM = 0x1000,
    EKB,
    END_OF_VIRTUAL_IMAGES,
};

/**
 * @brief Capabilities index (aka, command class) values.
 */
enum
{
    GENERIC_CAPABILITY_START_IDX = 0,
    IPL_CAPABILITY_START_IDX,
    SCOM_CAPABILITY_START_IDX,
    RING_CAPABILITY_START_IDX ,
    ARRAY_CAPABILITY_START_IDX ,
    GENERIC_CHIPOP_CAPABILITY_START_IDX ,
    STOP_CLOCKS_CAPABILITY_START_IDX ,
    ATTRIBUTE_CAPABILITY_START_IDX ,
    CODEUPDATE_CAPABILITY_START_IDX,
};

/**
 * @brief Capabilities enum values.
 *
 * - Each enum value needs to be decided based on the supported SBE
 *   features and chip-ops bit position in the respective capabilities word.
 *
 * - In a single capability word, we can represent 24 features or chip-ops
 *   Format: 0x<Class_1byte><BitPosition_3byte>.
 *
 * - The chip-op bit position needs to be decided by the chip-op op-code.
 *   For example, the execute hwp chip-op (0xA104) bit position will be 4th bit
 *   in the capability word based on the execute hwp chip-op op-code "04".
 */
enum
{
    // SBE supported features list

    HWP_FFDC_COLLECTION_SUPPPORTED      = 0xC0000001,
    SBE_FFDC_COLLECTION_SUPPPORTED      = 0xC0000002,
    FIFO_RESET_SUPPPORTED               = 0xC0000008,
    PIPES_SUPPPORTED                    = 0xC0000040,

    // SBE supported chip-ops list
    // 0xA101
    EXECUTE_ISTEP_SUPPPORTED            = 0xA1000001,
    // 0xA104
    EXECUTE_HWP_SUPPPORTED              = 0xA1000008,

    // 0xA201
    GET_SCOM_SUPPPORTED                 = 0xA2000001,
    // 0xA202
    PUT_SCOM_SUPPPORTED                 = 0xA2000002,

    // 0xA301
    GET_RING_SUPPPORTED                 = 0xA3000001,
    // 0xA302
    PUT_RING_SUPPPORTED                 = 0xA3000002,
    // 0xA303
    PUT_RING_FROM_IMAGE_SUPPPORTED      = 0xA3000004,

    // 0xA601
    CONTROL_FAST_ARRAY_SUPPPORTED       = 0xA6000001,
    // 0xA602
    CONTROL_TRACE_ARRAY_SUPPPORTED      = 0xA6000002,

    // 0xA801
    GET_SBE_FFDC_SUPPPORTED             = 0xA8000001,
    // 0xA802
    GET_CAPABILITIES_SUPPORTED          = 0xA8000002,

    // 0xA903
    STOP_CLOCKS_SUPPORTED               = 0xA9000004,

    // 0xAD01
    GET_RAWATTR_DUMP_SUPPORTED          = 0xAD000001,
    // 0xAD02
    LIST_ATTR_SUPPORTED                 = 0xAD000002,
    // 0xAD03
    UPDATE_ATTR_SUPPORTED               = 0xAD000004,

    // 0xAE01
    GET_CODE_LEVELS_SUPPORTED           = 0xAE000001,
    // 0xAE02
    UPDATE_IMAGE_SUPPORTED              = 0xAE000002,
    // 0xAE03
    SYNC_SIDE_SUPPORTED                 = 0xAE000004,
};

/**
 * @brief enum for sbe supported dump types
 *
*/
enum sbeSupportedDumpTypes
{
    SBE_DUMP_TYPE_SCS        = 0x01, /* System Check Stop */
    SBE_DUMP_TYPE_MPIPL      = 0x02, /* Memory Preserving IPl */
    SBE_DUMP_TYPE_PERF       = 0x03, /* Performance */
    SBE_DUMP_TYPE_RESERVED   = 0x04, /* Reserved */
    SBE_DUMP_TYPE_HB         = 0x05, /* Host Boot */
};

/**
 * @brief enum for memory scrub scope
 *
*/
enum SCOPE
{
    SCOPE_IMAGE_ONLY = 0x01,
    SCOPE_FULL_SIDE  = 0x02,
};

/**
 * @brief enum for memory side types
 *
*/
enum MEMORY_SIDES
{
    INVALID_SIDE    = 0,
    PRIMARY_SIDE    = 0x01,
    SECONDARY_SIDE  = 0x02,
    GOLDEN_SIDE     = 0x04,
    END_OF_SIDE_LIST= 0x08,
};

/**
 * @brief enum for device Id per side
 *
*/
enum DEVICE_ID
{
    INVALID_DEVICE_ID    = 0,
    PHY_DEVICE_ID_0      = 0x01,
    PHY_DEVICE_ID_1      = 0x02,
    PHY_DEVICE_ID_2      = 0x04,
    PHY_DEVICE_ID_3      = 0x08,
    END_OF_DEVICE_LIST   = 0x10,
};

#ifdef __cplusplus
}
#endif
