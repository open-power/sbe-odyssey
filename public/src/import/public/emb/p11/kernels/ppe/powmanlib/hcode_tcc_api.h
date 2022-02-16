/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/hcode_tcc_api.h $ */
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
/// @file  hcode_tcc_api.h
/// @brief Structures used between the Compute Engines on the TCC
///
// *HWP HW Owner        : Rahul Batra <rbatra@us.ibm.com>
// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 1
// *HWP Consumed by     : DCE:OCE:PCE:XCE


#ifndef __HCODE_TCC_API_H__
#define __HCODE_TCC_API_H__

#include <p11_gppb.H>
#include <hcode_errl_table.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// TCC Message IDs
//
enum TCC_IPC_MESSAGE_IDS
{
    MSGID_TCC_INVALID       = 0,
    MSGID_DCE_ERRLOG        = 0x10,
    MSGID_OCE_ERRLOG        = 0x12,
    MSGID_PCE_ERRLOG        = 0x13,
    MSGID_PCE_DOPT_OFLD     = 0x20,
    MSGID_TCC_PMSUSPEND     = 0x30,
};

// -----------------------------------------------------------------------------

/// Shared SRAM Section Header
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
    } signature;;
    uint8_t     header_reserved;
    uint8_t     instance_number;
    uint16_t    length;
} TCCShSrScHeader_t;

// -----------------------------------------------------------------------------

/// DCE created content
///   This is a placeholder structure for DCE created elements.  This
///   may well only amount to an address where the structure resides.
typedef struct
{
    TCCShSrScHeader_t header;

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
            uint64_t reserved1                  : 64;
        } fields;
    } dw1;
} DCW_t;

// -----------------------------------------------------------------------------

/// OCE created content
///   This is a placeholder structure for OCE consumable temperatures.  This
///   may well only amount to an address where the structure resides.
typedef struct
{
    TCCShSrScHeader_t header;

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
            uint64_t reserved1                  : 64;
        } fields;
    } dw1;
} OCW_t;

// -----------------------------------------------------------------------------

/// PCE created content
///   This content is created by PCE and shipped to the OCC shared SRAM.
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
    } signature;;
    uint8_t     header_reserved;
    uint8_t     instance_number;
    uint16_t    length;
} OCCShSrScHeader_t;

/// PCE created content

typedef struct
{
    OCCShSrScHeader_t header;

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
            uint64_t average_pstate             : 8;
            uint64_t average_frequency_pstate   : 8;
            uint64_t wof_clip_pstate            : 8;
            uint64_t average_throttle_idx       : 8;
            uint64_t cratio_vdd_roundup_avg     : 16;
            uint64_t reserved1                  : 16;
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
            uint64_t uv_avg_0p1pct              :  8;
            uint64_t ov_avg_0p1pct              :  8;
            uint64_t reserved_2                 : 48;
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
            uint64_t dirty_ttsr;
        } fields;
    } dw3;
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
            uint64_t reserved;
        } fields;
    } dw47[4];    // 4 reserved double words
} PCW_t;

// -----------------------------------------------------------------------------

/// XCE created content

enum ACT_CNT_IDX
{
    ACT_CNT_IDX_CORECLK_OFF    = 0,
    ACT_CNT_IDX_CORE_VMIN      = 1,
    ACT_CNT_IDX_MMA_OFF        = 2,
    ACT_CNT_IDX_CORECACHE_OFF  = 3,
    ACT_CNT_IDX_MAX            = 4,
};

typedef union
{
    uint8_t act_val[MAX_TAPS][ACT_CNT_IDX_MAX];
    uint32_t act_val_core[32];
} iddq_activity_t;

typedef struct
{
    TCCShSrScHeader_t header;

    iddq_activity_t     iddq_activity_values;

} XCW_t;

// -----------------------------------------------------------------------------

/// Hcode TCC Shared Data Structure
///
/// Shared data between DCE, OCE, PCE and XCE

// "TSS"
#define HCODE_TCC_SHARED_MAGIC_NUMBER       0x545353
#define HCODE_TCC_SHARED_VERSION            1
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
    uint32_t pce_beacon;
    uint16_t dce_data_offset;
    uint16_t dce_data_length;
    uint16_t oce_data_offset;
    uint16_t oce_data_length;
    uint16_t pce_data_offset;
    uint16_t pce_data_length;
    uint16_t xce_data_offset;
    uint16_t xce_data_length;
    uint16_t errlog_table_offset;
    uint16_t errlog_table_length;
    uint8_t  reserved0[3];
    uint8_t  iddq_active_sample_depth;
} TCCShSrHeader_t;


typedef struct
{

    /// Header that points to each of the indicated sections
    TCCShSrHeader_t     header;

    /// Hcode Error Log Index
    hcode_error_table_t errlog_idx;

    /// DCE Produced WOF Values
    DCW_t               dce_wof_values;

    /// OCE Produced WOF Values
    OCW_t               oce_wof_values;

    /// PCE Produced WOF Values
    PCW_t               pce_wof_values;

    /// XCE Produced WOF Values
    XCW_t               xce_wof_values;

} HcodeTCCSharedData_t;

#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __HCODE_TCC_API_H__ */
