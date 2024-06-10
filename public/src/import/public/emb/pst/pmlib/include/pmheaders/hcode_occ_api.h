/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/include/pmheaders/hcode_occ_api.h $ */
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
/// @file  hcode_occ_api.h
/// @brief Structures used between PGPE HCode and OCC Firmware
///
// *HWP HW Owner        : Prasad Bg Ranganath <prasadbgr@in.ibm.com>
// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 3
// *HWP Consumed by     : PGPE:OCC

#pragma once

#include <pst_ppb_global.H>
#include <hcode_common_api.h>
#include <hcode_tcc_api.h>          // needed for PCE WOF structure
#include <hcode_errl_table.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------
// IPC from 405
//---------------
//
// See ipc_func_ids.h for IPC_MSGID_405 enums used to communicate with GPE2/PGPE
//

///
/// @brief Return Codes
///
enum PGPE_405_IPC_MESSAGE_RCS
{
    PGPE_RC_SUCCESS                        = 0x01,
    PGPE_WOF_RC_NOT_ENABLED                = 0x10,
    PGPE_RC_PSTATES_NOT_STARTED            = 0x11,
    PGPE_RC_OCC_NOT_PMCR_OWNER             = 0x14,
    PGPE_RC_PM_COMPLEX_SUSPEND_SAFE_MODE   = 0x15,
    PGPE_RC_REQ_WHILE_PENDING_ACK          = 0x21,
    PGPE_RC_CEFF_INFO_NOT_PRESENT          = 0x22,
    PGPE_RC_INVALID_PMCR_OWNER             = 0x23,
    PGPE_WOF_RC_INVALID_FIXED_CRATIO_INDEX = 0x24,
};

///
/// @brief PMCR Owner
///
typedef enum
{
    PMCR_OWNER_HOST         = 0,                    ///< PMCR owned by Host
    PMCR_OWNER_OCC          = 1,                    ///< PMCR owned by OCC
    PMCR_OWNER_CHAR         = 2                     ///< PMCR owned by Characterization SCOM
} PMCR_OWNER;

//
// @brief Pstate Control Actions
//
#define PGPE_ACTION_PSTATE_START   0                ///< Pstate Start
#define PGPE_ACTION_PSTATE_STOP    1                ///< Pstate Stop

///
/// @brief Pstate Control IPC
///
typedef struct ipcmsg_pstate_control
{
    ipcmsg_base_t   msg_cb;                         ///< Message control block
    uint32_t        action;                         ///< Pstate action
    PMCR_OWNER      pmcr_owner;                     ///< Power Management Control Request (PNCR) owner
} ipcmsg_pstate_control_t;

///
/// @brief Clip Update IPC
///
typedef struct ipcmsg_clip_update
{
    ipcmsg_base_t   msg_cb;                         ///< Message control block
    uint8_t         pad[4];                         ///< Padding
    uint8_t         ps_val_clip_min[MAX_TAPS];      ///< Pstate clip minimum value per Tap
    uint8_t         ps_val_clip_max[MAX_TAPS];      ///< Pstate clip maximum value per Tap
} ipcmsg_clip_update_t;

///
/// @brief Set PMCR IPC
///
/// Only will be honored if OCC is the owner
///
typedef struct ipcmsg_set_pmcr
{
    ipcmsg_base_t   msg_cb;                         ///< Message control block
    uint8_t         pad[4];                         ///< Padding
    uint64_t        pmcr[MAX_TAPS];                 ///< PMCR value per Tap
} ipcmsg_set_pmcr_t;


//
// WOF Control Actions
//
#define PGPE_ACTION_WOF_ON              1           ///< WOF On
#define PGPE_ACTION_WOF_OFF             2           ///< WOF Off
#define PGPE_OCC_CRATIO_MODE_VARIABLE   0           ///< Variable CRatio mode
#define PGPE_OCC_CRATIO_MODE_FIXED      1           ///< Fixed CRatio mode

///
/// @brief WOF Control IPC
///
typedef struct ipcmsg_wof_control
{
    ipcmsg_base_t   msg_cb;                         ///< Message control block
    uint32_t        action;                         ///< WOF action
} ipcmsg_wof_control_t;

///
/// @brief Effective Capacitance (Ceff) Information Structure
///
typedef struct
{
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint64_t    marker              : 8;    ///< 0xCF
            uint64_t    cfr_index           : 8;    ///< CefFRratio
            uint64_t    io_index            : 8;    ///< IO power
            uint64_t    hs_index            : 8;    ///< HeatSink
            uint64_t    ic_state            : 8;    ///< ICredit state
            uint64_t    nn_index            : 8;    ///< Nearest Neighbor
            uint64_t    cr_mode             : 8;    ///< CoreRatio mode (0:variable, 1: fixed)
            uint64_t    cr_index            : 8;    ///< CoreRatio if CoreRatio mode=fixed
        } fields;       ///< Sub fields
    } dw0;  ///< Doubleword 0
    union
    {
        uint64_t value;                             ///< future expansion
    } dw1;  ///< Doubleword 1
} CeffInfo_t;

///
/// @brief Ceffective Info IPC
///
typedef struct ipcmsg_wof_ceffinfo
{
    ipcmsg_base_t     msg_cb;                       ///< Message control block
    CeffInfo_t*       ceff_info_ptr[MAX_TAPS];      ///< Ceff Info per Tap
} ipcmsg_wof_ci_t;

// -----------------------------------------------------------------------------

///
/// @brief Hub collected data
///
typedef struct
{
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t rdp_limit_10ma         : 16;   ///< RDP current limit (10mA)
            uint64_t io_power_proxy_10w     : 16;   ///< I/O power proxy power (10mW)
            uint64_t io_compute_pwr_10mw    : 16;   ///< I/O computed power (10mW)
            uint64_t reserved0              : 16;   ///< Reserved
        } fields;       ///< Sub fields
    } dw0;  ///< Doubleword 0
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t vdn_avg_mv             : 16;   ///< VDN average voltage (mV)
            uint64_t vio_avg_mv             : 16;   ///< VIO average voltage (mV)
            uint64_t idn_avg_10ma           : 16;   ///< IDN average current (10mA)
            uint64_t iio_avg_10ma           : 16;   ///< IIO average current (10mA)
        } fields;       ///< Sub fields
    } dw1;  ///< Doubleword 1
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t idn_max_100ma          : 16;   ///< IDN maximum current (10mA)
            uint64_t idn_min_100ma          : 16;   ///< IDN minimum current (10mA)
            uint64_t iio_max_100ma          : 16;   ///< IIO maximum current (10mA)
            uint64_t iio_min_100ma          : 16;   ///< IIO minimum current (10mA)
        } fields;       ///< Sub fields
    } dw2;  ///< Doubleword 2
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t vdn_vrm_temp_0p1degC   : 16;   ///< VDN VRM temperature (0.1C)
            uint64_t vio_vrm_temp_0p1degC   : 16;   ///< VIO VRM temperature (0.1C)
            uint64_t reserved3              : 32;   ///< Reserved
        } fields;       ///< Sub fields
    } dw3;  ///< Doubleword 3
} OSS_HUB_t;

// -----------------------------------------------------------------------------

///
/// @brief Tap collected data
///
typedef struct
{
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t idd_avg_10ma           : 16;   ///< IDD average current (10mA)
            uint64_t ics_avg_10ma           : 16;   ///< ICS average current (10mA)
            uint64_t idd_max_100ma          : 16;   ///< IDD maximum current (10mA)
            uint64_t idd_min_100ma          : 16;   ///< IDD minimum current (10mA)
        } fields;       ///< Sub fields
    } dw0;  ///< Doubleword 0
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word;
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;
        struct
        {
            uint64_t ics_max_100ma          : 16;   ///< ICS maximum current (10mA)
            uint64_t ics_min_100ma          : 16;   ///< ICS minimum current (10mA)
            uint64_t ocs_avg_max_10ma       : 16;   ///< OCS average maximum current (10mA)
            uint64_t ocs_avg_max_0p01pct    : 16;   ///< OCS average maximum current (0.01%)
        } fields;       ///< Sub fields
    } dw1;  ///< Doubleword 1
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword
        struct
        {
            uint64_t vdd_vrm_temp_0p1degC   : 16;   ///< VDD VRM temperature (0.1C)
            uint64_t vcs_vrm_temp_0p1degC   : 16;   ///< VCS VRM temperature (0.1C)
            uint64_t reserved3              : 32;   ///< Reserved
        } fields;       ///< Sub fields
    } dw2;  ///< Doubleword 2
} OSS_TAP_t;

// -----------------------------------------------------------------------------

// XGPE created content

#define HCODE_OCC_XGPE_MAGIC_NUMBER     0x584757    ///< XGPE OCC Shared SRAM marker value ("XGW")
#define HCODE_OCC_XGPE_VERSION          1           ///< XGPE OCC Shared SRAM layout version number

///
/// @brief XGPE OCC Shared SRAM header
///
typedef struct
{
    union
    {
        uint32_t value; ///< Raw value
        struct
        {
            uint32_t  magic_value           : 24;   ///< Magic marker value
            uint32_t  version               : 8;    ///< Structure version number
        } fields;       ///< Sub fields
    } signature;        ///< Signature structure
    uint8_t     header_reserved;                    ///< Reserved
    uint8_t     valid_tap_vector;                   ///< Valid vector of Taps
    uint16_t    length;                             ///< OCC Shared SRAM length
    uint16_t    hub_data_offset;                    ///< Hub data offset
    uint16_t    hub_data_length;                    ///< Hub data length
    uint16_t    tap_data_offset;                    ///< Tap data offset
    uint16_t    tap_data_length;                    ///< Tap data length
} OCCShSrXGPEHeader_t;

///
/// @brief XGPE OCC Shared SRAM content
///
typedef struct
{
    OCCShSrXGPEHeader_t     header;                 ///< OCC Shared SRAM header
    OSS_HUB_t               hub;                    ///< Hub data
    OSS_TAP_t               tap[MAX_TAPS];          ///< Tap data per Tap
} XGPE_t;

// -----------------------------------------------------------------------------

// PGPE created content

#define HCODE_OCC_PGPE_MAGIC_NUMBER     0x504757    ///< PGPE OCC Shared SRAM marker value ("PGW")
#define HCODE_OCC_PGPE_VERSION          1           ///< PGPE OCC Shared SRAM layout version number

///
/// @brief PGPE OCC Shared SRAM header
///
typedef struct
{
    union
    {
        uint32_t value; ///< Raw value
        struct
        {
            uint32_t  magic_value           : 24;   ///< Magic marker value
            uint32_t  version               : 8;    ///< Structure version number
        } fields;       ///< Sub fields
    } signature;        ///< Signature structure
    uint8_t     header_reserved;                    ///< Reserved
    uint8_t     valid;                              ///< PGPE data valid
    uint16_t    length;                             ///< PGPE header length
    uint64_t    reserved;                           ///< Reserved
} OCCShSrPGPEHeader_t;

///
/// @brief PGPE OCC Shared SRAM content
///
/// Content is presently undefined
typedef struct
{
    OCCShSrPGPEHeader_t    header;                  ///< OCC Shared SRAM PGPE section header
    uint64_t               reserved;                ///< Reserved
} PGPE_t;

// -----------------------------------------------------------------------------

// Hcode<>OCC Shared Data Structure
//
// Shared data between OCC, PGPE and XGPE
#define HCODE_OCC_SHARED_MAGIC_NUMBER   0x4F5353    ///< OCC Shared SRAM marker value ("TSS")
#define HCODE_OCC_SHARED_VERSION        1           ///< OCC Shared SRAM layout version number

///
/// @brief OCC Shared SRAM Header
///
typedef struct
{
    union
    {
        uint32_t value; ///< Raw value
        struct
        {
            uint32_t  magic_value           : 24;   ///< Magic marker
            uint32_t  version               : 8;    ///< Structure version number
        } fields;       ///< Sub fields
    } signature;        ///< Signature structure
    uint32_t pgpe_beacon;                           ///< PGPE Beacon address
    uint16_t errlog_table_offset;                   ///< Hcode Error Log Table offset (from beginning of Shared region)
    uint16_t errlog_table_length;                   ///< Hcode Error Log Table length
    uint16_t xgpe_data_offset;                      ///< XGPE data region offset (from beginning of Shared region)
    uint16_t xgpe_data_length;                      ///< XGPE data region length
    uint16_t pgpe_data_offset;                      ///< PGPE data region offset (from beginning of Shared region)
    uint16_t pgpe_data_length;                      ///< PGPE data region length
    uint16_t pce_tap0_data_offset;                  ///< Tap0 PCE data region offset (from beginning of Shared region)
    uint16_t pce_tap0_data_length;                  ///< Tap0 PCE data region length
    uint64_t reserved0;                             ///< Reserved
} OCCShSrHeader_t;

///
/// @brief OCC Shared SRAM Region
///
typedef struct
{

    /// Header that points to each of the indicated sections
    OCCShSrHeader_t     header;

    /// Hcode Error Log Index
    hcode_error_table_t errlog_idx;

    /// XGPE Produced WOF Values
    XGPE_t              xgpe_wof_values;

    /// PGPE Produced WOF Values
    PGPE_t              pgpe_wof_values;

    /// PCE Produced WOF Values
    ///
    /// This member is actually a ping/pong buffer that is 2 times the size of
    ///    the structure.
    PCW_t               pce_wof_values[2 * MAX_TAPS];

} HcodeOCCSharedData_t;

#ifdef __cplusplus
} // end extern C
#endif