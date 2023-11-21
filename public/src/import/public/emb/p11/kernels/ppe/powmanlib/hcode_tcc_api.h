/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/hcode_tcc_api.h $ */
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
/// @file  hcode_tcc_api.h
/// @brief Structures used between the Compute Engines on the TCC
///
// *HWP HW Owner        : Prasad Bg Ranganath <prasadbgr@in.ibm.com>
// *HWP HW Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 3
// *HWP Consumed by     : DCE:OCE:PCE:XCE


#ifndef __HCODE_TCC_API_H__
#define __HCODE_TCC_API_H__

#include <p11_ppb_global.H>
#include <p11_hcode_image_defines.H>
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

/// TCC Shared SRAM Section Header
typedef struct
{
    union
    {
        uint32_t value;
        struct
        {
            uint32_t  magic_value               : 24;
            uint32_t  version                   : 8;
        } fields;
    } signature;;
    uint8_t     block_valid;
    uint8_t     instance_number;
    uint16_t    length;
} TCCShSrScHeader_t;

// -----------------------------------------------------------------------------

/// DCE created content

// "DCV"
#define HCODE_TCC_DCE_MAGIC_NUMBER      0x444356
#define HCODE_TCC_DCE_VERSION           1

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

// "OCV"
#define HCODE_TCC_OCE_MAGIC_NUMBER      0x4F4356
#define HCODE_TCC_OCE_VERSION           1

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

// "PCV"
#define HCODE_TCC_PCE_MAGIC_NUMBER      0x504356
#define HCODE_TCC_PCE_VERSION           1

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
            uint64_t average_pstate             : 8;  // Average of all Pstates including the throttle space
            uint64_t average_frequency_pstate   : 8;  // Average of all Pstates in the frequency region only
            uint64_t wof_clip_pstate            : 8;
            uint64_t wof_clip_throttle          : 8;
            uint64_t uv_avg_0p1pct              : 8;
            uint64_t ov_avg_0p1pct              : 8;
            uint64_t ceff_ratio                 : 8;
            uint64_t dirty                      : 8;
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
            uint64_t average_throttle_idx       : 8;
            uint64_t reserved_2                 : 8;
            uint64_t cratio_vdd_avg             : 16;
            uint64_t cratio_vdd_actual          : 32;
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
            uint64_t vdd_ext_avg_mv             : 16;
            uint64_t vdd_eff_avg_mv             : 16;
            uint64_t idd_avg_10ma               : 16;
            uint64_t vcs_int_avg_mv             : 16;
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
            uint64_t dirty_ttsr;
        } fields;
    } dw4;
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
            uint64_t ceff_ratio_idx             : 8;
            uint64_t cratio_idx                 : 8;
            uint64_t hs_ratio_idx               : 8;
            uint64_t icr_mma_idx                : 8;
            uint64_t io_idx                     : 8;
            uint64_t icr_value                  : 8;
            uint64_t reserved_5                 : 16;
        } fields;
    } dw5;
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
            uint64_t wof_adjust_freq_mhz        : 16;
            uint64_t wof_limit_freq_mhz         : 16;
            uint64_t wof_adjust_throt_idx       : 16;
            uint64_t wof_limit_throt_idx        : 16;
        } fields;
    } dw6;
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
            uint64_t reserved_7;
        } fields;
    } dw7;
} PCW_t;

// -----------------------------------------------------------------------------

/// XCE created content

#define IDDQ_ACTIVITY_SAMPLE_DEPTH   8 // Must be <256

enum ACT_CNT_IDX
{
    ACT_CNT_IDX_CORE_CLK_OFF   = 0,
    ACT_CNT_IDX_CORE_PWR_OFF   = 1,
    ACT_CNT_IDX_MMA_PWR_OFF    = 2,
    ACT_CNT_IDX_CACHE_PWR_OFF  = 3,
    ACT_CNT_IDX_MAX            = 4,
};

// "XCV"
#define HCODE_TCC_XCE_MAGIC_NUMBER      0x584356
#define HCODE_TCC_XCE_VERSION           1

typedef union
{
    uint8_t  act_val[MAX_CORES_PER_TAP][ACT_CNT_IDX_MAX];
    uint32_t act_val_core[MAX_CORES_PER_TAP];
} iddq_activity_t;

typedef struct
{
    TCCShSrScHeader_t  header;
    iddq_activity_t    iddq_activity_values;
} XCW_t;

// -----------------------------------------------------------------------------

/// Hcode TCC Shared Data Structure
///
/// Shared data between DCE, OCE, PCE and XCE

// "TSS"
#define HCODE_TCC_SHARED_MAGIC_NUMBER       0x545353
#define HCODE_TCC_SHARED_VERSION            1

typedef struct
{
    // ASCII value XGV
    uint8_t magic_word[3];
    // Version number of layout
    uint8_t version;
    // reserve
    uint8_t reserve[2];
    //length in bytes o XGV block including its header
    uint16_t length;
} XgvHdr_t;

typedef struct
{
    XgvHdr_t xgv_header;
    //VDD voltage averaged over 500us
    uint16_t vdd_avg_mv;
    // Idd current averaged over 500us
    uint16_t idd_avg_10ma;
    // Ics current averaged over 500us
    uint16_t ics_avg_10mma;
    //reserve
    uint8_t  reserve;
    // status
    uint8_t status;
} XgvValues_t;

typedef union
{
    uint64_t    xvg_data[2];
    XgvValues_t xvg_layout;
} XGW_t;

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
    uint16_t xgpe_data_offset;
    uint16_t xgpe_data_length;
    uint16_t errlog_table_offset;
    uint16_t errlog_table_length;
} TCCShSrHeader_t;


/// Main shared data structure placed at the beginning of the shared
/// Sram region at address TCC_SHARED_SRAM_BASE_ADDR (=0xFFFFC000)
typedef struct
{

    /// Header that points to each of the indicated sections
    TCCShSrHeader_t     header;           //              32B

    /// Hcode Error Log Index
    hcode_error_table_t errlog_idx;       // 8B + 160B = 168B

    /// DCE Produced WOF Values
    DCW_t               dce_wof_values;   // 8B + 8B   =  16B

    /// OCE Produced WOF Values
    OCW_t               oce_wof_values;   // 8B + 8B   =  16B

    /// PCE Produced WOF Values
    PCW_t               pce_wof_values;   // 8B + 56B  =  64B

    /// XCE Produced WOF Values
    XCW_t               xce_wof_values;   // 8B + 32B  =  40B

    /// XGPE Produced WOF Values
    XGW_t               xgpe_wof_values;  //              16B

} HcodeTCCSharedData_t;                   // Total     = 352B


#ifdef __cplusplus
} // end extern C
#endif

#endif    /* __HCODE_TCC_API_H__ */
