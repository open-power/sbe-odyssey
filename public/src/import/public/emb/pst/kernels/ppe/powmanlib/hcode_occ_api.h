/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/hcode_occ_api.h $ */
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


#ifndef __HCODE_OCC_API_H__
#define __HCODE_OCC_API_H__

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

//
// Return Codes
//
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

//
// PMCR Owner
//
typedef enum
{
    PMCR_OWNER_HOST         = 0,
    PMCR_OWNER_OCC          = 1,
    PMCR_OWNER_CHAR         = 2
} PMCR_OWNER;

//
// Pstate Control Actions
//
#define PGPE_ACTION_PSTATE_START   0
#define PGPE_ACTION_PSTATE_STOP    1

typedef struct ipcmsg_pstate_control
{
    ipcmsg_base_t   msg_cb;
    uint32_t        action;
    PMCR_OWNER      pmcr_owner;
} ipcmsg_pstate_control_t;

typedef struct ipcmsg_clip_update
{
    ipcmsg_base_t   msg_cb;
    uint8_t         pad[4];
    uint8_t         ps_val_clip_min[MAX_TAPS];
    uint8_t         ps_val_clip_max[MAX_TAPS];
} ipcmsg_clip_update_t;


typedef struct ipcmsg_set_pmcr
{
    ipcmsg_base_t   msg_cb;
    uint8_t         pad[4];
    uint64_t        pmcr[MAX_TAPS];
} ipcmsg_set_pmcr_t;


//
// WOF Control Actions
//
#define PGPE_ACTION_WOF_ON         1
#define PGPE_ACTION_WOF_OFF        2
#define PGPE_OCC_CRATIO_MODE_VARIABLE   0
#define PGPE_OCC_CRATIO_MODE_FIXED      1

typedef struct ipcmsg_wof_control
{
    ipcmsg_base_t   msg_cb;
    uint32_t        action;
} ipcmsg_wof_control_t;


typedef struct
{
    union
    {
        uint64_t value;
        struct
        {
            uint64_t    marker              : 8;  // 0xCF
            uint64_t    cfr_index           : 8;  // CefFRratio
            uint64_t    io_index            : 8;  // IO power
            uint64_t    hs_index            : 8;  // HeatSink
            uint64_t    ic_state            : 8;  // ICredit state
            uint64_t    nn_index            : 8;  // Nearest Neighbor
            uint64_t    cr_mode             : 8;  // CoreRatio mode (0:variable, 1: fixed)
            uint64_t    cr_index            : 8;  // CoreRatio if CoreRatio mode=fixed
        } fields;
    } dw0;
    union
    {
        uint64_t value;                   // future expansion
    } dw1;
} CeffInfo_t;

typedef struct ipcmsg_wof_ceffinfo
{
    ipcmsg_base_t     msg_cb;
    CeffInfo_t*       ceff_info_ptr[MAX_TAPS];     // Ceff Info per Tap
} ipcmsg_wof_ci_t;

// -----------------------------------------------------------------------------

/// Hub collected data
typedef struct
{
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t rdp_limit_10ma         : 16;
            uint64_t io_power_proxy_10w     : 16;
            uint64_t io_compute_pwr_10mw    : 16;
            uint64_t reserved0              : 16;
        } fields;
    } dw0;
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t vdn_avg_mv             : 16;
            uint64_t vio_avg_mv             : 16;
            uint64_t idn_avg_10ma           : 16;
            uint64_t iio_avg_10ma           : 16;
        } fields;
    } dw1;
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t idn_max_100ma          : 16;
            uint64_t idn_min_100ma          : 16;
            uint64_t iio_max_100ma          : 16;
            uint64_t iio_min_100ma          : 16;
        } fields;
    } dw2;
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t vdn_vrm_temp_0p1degC   : 16;
            uint64_t vio_vrm_temp_0p1degC   : 16;
            uint64_t reserved3              : 32;
        } fields;
    } dw3;
} OSS_HUB_t;

// -----------------------------------------------------------------------------

typedef struct
{
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t idd_avg_10ma           : 16;
            uint64_t ics_avg_10ma           : 16;
            uint64_t idd_max_100ma          : 16;
            uint64_t idd_min_100ma          : 16;
        } fields;
    } dw0;
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t ics_max_100ma          : 16;
            uint64_t ics_min_100ma          : 16;
            uint64_t ocs_avg_max_10ma       : 16;
            uint64_t ocs_avg_max_0p01pct    : 16;
        } fields;
    } dw1;
    union
    {
        uint64_t value;
        struct
        {
            uint32_t high_order;
            uint32_t low_order;
        } words;
        struct
        {
            uint64_t vdd_vrm_temp_0p1degC   : 16;
            uint64_t vcs_vrm_temp_0p1degC   : 16;
            uint64_t reserved3              : 32;
        } fields;
    } dw2;
} OSS_TAP_t;

// -----------------------------------------------------------------------------

/// XGPE created content

// "XGW"
#define HCODE_OCC_XGPE_MAGIC_NUMBER         0x584757
#define HCODE_OCC_XGPE_VERSION              1

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t  magic_value           : 24;
            uint32_t  version               : 8;
        } fields;
    } signature;
    uint8_t     header_reserved;
    uint8_t     valid_tap_vector;
    uint16_t    length;
    uint16_t    hub_data_offset;
    uint16_t    hub_data_length;
    uint16_t    tap_data_offset;
    uint16_t    tap_data_length;
} OCCShSrXGPEHeader_t;

typedef struct
{
    OCCShSrXGPEHeader_t     header;
    OSS_HUB_t               hub;
    OSS_TAP_t               tap[MAX_TAPS];
} XGPE_t;

// -----------------------------------------------------------------------------

/// PGPE created content

// "PGW"
#define HCODE_OCC_PGPE_MAGIC_NUMBER         0x504757
#define HCODE_OCC_PGPE_VERSION              1

typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t  magic_value           : 24;
            uint32_t  version               : 8;
        } fields;
    } signature;
    uint8_t     header_reserved;
    uint8_t     valid;
    uint16_t    length;
    uint64_t    reserved;
} OCCShSrPGPEHeader_t;

// Content is presently undefined
typedef struct
{
    OCCShSrPGPEHeader_t    header;
    uint64_t               reserved;
} PGPE_t;

// -----------------------------------------------------------------------------

/// Hcode<>OCC Shared Data Structure
///
/// Shared data between OCC, PGPE and XGPE
// "TSS"
#define HCODE_OCC_SHARED_MAGIC_NUMBER       0x4F5353
#define HCODE_OCC_SHARED_VERSION            1

/// Shared SRAM Header
typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t  magic_value           : 24;
            uint32_t  version               : 8;
        } fields;
    } signature;
    uint32_t pgpe_beacon;
    uint16_t errlog_table_offset;
    uint16_t errlog_table_length;
    uint16_t xgpe_data_offset;
    uint16_t xgpe_data_length;
    uint16_t pgpe_data_offset;
    uint16_t pgpe_data_length;
    uint16_t pce_tap0_data_offset;
    uint16_t pce_tap0_data_length;
    uint64_t reserved0;
} OCCShSrHeader_t;

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
    /// This member is actually a ping/pong buffer that is 2 times the size of
    ///    the structure.
    PCW_t               pce_wof_values[2 * MAX_TAPS];

} HcodeOCCSharedData_t;

#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __HCODE_OCC_API_H__ */
