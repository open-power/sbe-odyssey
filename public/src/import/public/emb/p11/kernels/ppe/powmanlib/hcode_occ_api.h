/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/hcode_occ_api.h $ */
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
/// @file  hcode_occ_api.h
/// @brief Structures used between PGPE HCode and OCC Firmware
///
// *HWP HW Owner        : Prasad Bg Ranganath <prasadbgr@in.ibm.com>
// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 1
// *HWP Consumed by     : PGPE:OCC


#ifndef __HCODE_OCC_API_H__
#define __HCODE_OCC_API_H__

#include <p11_ppb_global.H>
#include <hcode_common_api.h>
#include <hcode_tcc_api.h>          // needed for PCE WOF structure
#include <hcode_errl_table.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------
// IPC from 405
//---------------
//Note: These are really not used. They are just for documentation purposes
enum MESSAGE_ID_IPI2HI
{
    MSGID_405_INVALID       = 0,
    MSGID_405_START_SUSPEND = 1,
    MSGID_405_CLIPS         = 2,
    MSGID_405_SET_PMCR      = 3,
    MSGID_405_WOF_CONTROL   = 4,
    MSGID_405_WOF_CRT       = 5
};

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
    PGPE_RC_NULL_VRT_POINTER               = 0x22,
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
// Start Suspend Actions
//
#define PGPE_ACTION_PSTATE_START   0
#define PGPE_ACTION_PSTATE_STOP    1

typedef struct ipcmsg_start_stop
{
    ipcmsg_base_t   msg_cb;
    uint32_t         action;
    PMCR_OWNER      pmcr_owner;
} ipcmsg_start_stop_t;

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


typedef struct ipcmsg_wof_crt
{
    ipcmsg_base_t   msg_cb;
    uint8_t         cratio_mode;        // 0 = variable; 1 = fixed
    uint8_t         fixed_cratio_index; // if cratio_mode = fixed, index to use
    uint8_t         pad[2];
    CRT_t*          crt_ptr[MAX_TAPS];  // Core Ratio Table per domain
    uint32_t        vdd_ceff_ratio;     // Used for CRT - debug only
} ipcmsg_wof_crt_t;


// -----------------------------------------------------------------------------

/// Hub collected data

/// Shared SRAM Section Header with Tap Number

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
            uint64_t rdp_limit_10ma             : 16;
            uint64_t io_power_proxy_10w         : 16;
            uint64_t compute_pwr_10mw           : 16;
            uint64_t reserved0                  : 16;
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
            uint64_t vdd_avg_mv                 : 16;
            uint64_t vcs_avg_mv                 : 16;
            uint64_t vdn_avg_mv                 : 16;
            uint64_t vio_avg_mv                 : 16;
        } fields;
    } dw1;
} OSS_HUB_t;

// -----------------------------------------------------------------------------

/// Per TAP collected data
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
            uint64_t vdd_avg_mv                 : 16;
            uint64_t vcs_avg_mv                 : 16;
            uint64_t idd_avg_10ma              : 16;
            uint64_t ics_avg_10ma               : 16;
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
            uint64_t idd_max_100ma              : 16;
            uint64_t ocs_avg_max_10ma           : 16;
            uint64_t ocs_avg_0p01pct            : 16;
            uint64_t dirty_current_10ma         : 16;
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
            uint64_t ics_max_10ma               : 16;
            uint64_t reserved                   : 48;
        } fields;
    } dw2;
} OSS_TAP_t;

// -----------------------------------------------------------------------------

/// XGPE created content
typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t  magic     :   24;
            uint32_t  version   :    8;
        } fields;
    } signature;
    uint8_t     header_reserved;
    uint8_t     num_of_taps;
    uint16_t    length;
    uint16_t    hub_data_offset;
    uint16_t    hub_data_length;
    uint16_t    tap_data_offset;
    uint16_t    tap_data_length;
} OCCShSrScTapHeader_t;

typedef struct
{
    OCCShSrScTapHeader_t    header;
    OSS_HUB_t               hub;
    OSS_TAP_t               tap[MAX_TAPS];
} OSS_xgpe_values_t;

// -----------------------------------------------------------------------------

/// Hcode<>OCC Shared Data Structure
///
/// Shared data between OCC, PGPE and XGPE

// "OSS"
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
            uint32_t  magic     :   24;
            uint32_t  version   :    8;
        } fields;
    } signature;
    uint32_t pgpe_beacon;
    uint16_t pce_data_offset;
    uint16_t pce_data_length;
    uint16_t xgpe_data_offset;
    uint16_t xgpe_data_length;
    uint16_t errlog_table_offset;
    uint16_t errlog_table_length;
    uint32_t reserved0;
    uint64_t reserved1;
} OCCShSrHeader_t;


typedef struct
{

    /// Header that points to each of the indicated sections
    OCCShSrHeader_t     header;

    /// Hcode Error Log Index
    hcode_error_table_t errlog_idx;

    /// PCE Produced WOF Values
    PCW_t               pce_wof_values[MAX_TAPS];

    /// XGPE Produced WOF Values
    OSS_xgpe_values_t   xgpe_wof_values;

} HcodeOCCSharedData_t;

#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __HCODE_OCC_API_H__ */
