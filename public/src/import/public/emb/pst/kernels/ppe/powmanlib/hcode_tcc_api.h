/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/hcode_tcc_api.h $ */
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

#include <pst_ppb_global.H>
#include <pst_hcode_image_defines.H>
#include <hcode_errl_table.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// @brief TCC Message IDs
///
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

///
/// @brief TCC Shared SRAM Section Header
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
    uint8_t     block_valid;                        ///< Block valid indicator
    uint8_t     instance_number;                    ///< Tap instance number
    uint16_t    length;                             ///< Block length
} TCCShSrScHeader_t;

// -----------------------------------------------------------------------------

// DCE created content

#define HCODE_TCC_DCE_MAGIC_NUMBER      0x444356    ///< DCE TCC Shared SRAM marker value ("OCV")
#define HCODE_TCC_DCE_VERSION           1           ///< DCE TCC Shared SRAM layout version number

///
/// @brief DCE created WOF elements
///
/// This is a placeholder structure for DCE created elements.  This
/// may well only amount to an address where the structure resides.
typedef struct
{
    TCCShSrScHeader_t header;                       ///< Region Header
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
            uint64_t reserved1                : 64; ///< Reserved
        } fields;       ///< Sub fields
    } dw1;  ///< Doubleword 1
} DCW_t;

// -----------------------------------------------------------------------------

// OCE created content

#define HCODE_TCC_OCE_MAGIC_NUMBER      0x4F4356    ///< OCE TCC Shared SRAM marker value ("OCV")
#define HCODE_TCC_OCE_VERSION           1           ///< OCE TCC Shared SRAM layout version number

///
/// @brief OCE consummable WOF elements
///
/// This is a placeholder structure for OCE consumable temperatures.  This
/// may well only amount to an address where the structure resides.
typedef struct
{
    TCCShSrScHeader_t header;                       ///< Region Header
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
            uint64_t reserved1                : 64; ///< Reserved
        } fields;       ///< Sub fields
    } dw1;  ///< Doubleword 1
} OCW_t;

// -----------------------------------------------------------------------------

// PCE created content

#define HCODE_TCC_PCE_MAGIC_NUMBER      0x504356    ///< PCE TCC Shared SRAM marker value ("PCV")
#define HCODE_TCC_PCE_VERSION           1           ///< PCE TCC Shared SRAM layout version number

///
/// @brief PCE created WOF elements
///
typedef struct
{
    TCCShSrScHeader_t header;                       ///< Region Header

    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword;
        struct
        {
            uint64_t average_pstate           : 8;  ///< Average of all Pstates including the throttle space
            uint64_t average_frequency_pstate : 8;  ///< Average of all Pstates in the frequency region only
            uint64_t wof_clip_pstate          : 8;  ///< WOF Clipped Pstate
            uint64_t wof_clip_throttle        : 8;  ///< WOF Clipped Throttle value
            uint64_t uv_avg_0p1pct            : 8;  ///< Undervolting average (0.1%)
            uint64_t ov_avg_0p1pct            : 8;  ///< Overvolting average (0.1%)
            uint64_t ceff_ratio               : 8;  ///< CeffRatio used
            uint64_t dirty                    : 8;  ///< Dirty (eg droop or overrcurrent) indicator
        } fields;       ///< Sub fields
    } dw1;  ///< Doubleword 1
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword;
        struct
        {
            uint64_t average_throttle_idx     : 8;  ///< Average throttle index
            uint64_t reserved_2               : 8;  ///< Reserved
            uint64_t cratio_vdd_avg           : 16; ///< VDD average core ratio
            uint64_t cratio_vdd_actual        : 32; ///< VDD actual (instantaneous) core ratio
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
            uint64_t vdd_ext_avg_mv           : 16; ///< VDD external average voltage (mV)
            uint64_t vdd_eff_avg_mv           : 16; ///< VDD effetive average voltage (mV)
            uint64_t idd_avg_10ma             : 16; ///< IDD average current (10mA)
            uint64_t vcs_int_avg_mv           : 16; ///< VCS internal average voltage (mV)
        } fields;       ///< Sub fields
    } dw3;  ///< Doubleword 3
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doubleword;
        struct
        {
            uint64_t dirty_ttsr;
        } fields;       ///< Sub fields
    } dw4;  ///< Doubleword 14
    union
    {
        uint64_t value; ///< Raw value
        struct
        {
            uint32_t high_order;                    ///< High order (MSB) word
            uint32_t low_order;                     ///< Low  order (MSB) word
        } words;        ///< Words within doublewords;
        struct
        {
            uint64_t ceff_ratio_idx           : 8;  ///< Ceff ratio index
            uint64_t cratio_idx               : 8;  ///< Core ratio index
            uint64_t hs_ratio_idx             : 8;  ///< Heatsink ratio index
            uint64_t icr_mma_idx              : 8;  ///< Input current credit / MMA index
            uint64_t io_idx                   : 8;  ///< I/O index
            uint64_t icr_value                : 8;  ///< Input current value
            uint64_t reserved_5               : 16; ///< Reserved
        } fields;       ///< Sub fields
    } dw5;  ///< Doubleword 5
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
            uint64_t wof_adjust_freq_mhz      : 16; ///< WOF adjustment frequency (MHz)
            uint64_t wof_limit_freq_mhz       : 16; ///< WOF limit frequency (MHz)
            uint64_t wof_adjust_throt_idx     : 16; ///< WOF adjustment throttle index
            uint64_t wof_limit_throt_idx      : 16; ///< WOF limit throttle index
        } fields;       ///< Sub fields
    } dw6;  ///< Doubleword 6
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
            uint64_t reserved_7;                    ///< Reserved
        } fields;       ///< Sub fields
    } dw7;  ///< Doubleword 7
} PCW_t;

// -----------------------------------------------------------------------------

// XCE created content

#define IDDQ_ACTIVITY_SAMPLE_DEPTH   8              ///< IDDQ activity sampling depth.  Must be <256

///
/// @brief Activity count indexes
enum ACT_CNT_IDX
{
    ACT_CNT_IDX_CORE_CLK_OFF   = 0,
    ACT_CNT_IDX_CORE_PWR_OFF   = 1,
    ACT_CNT_IDX_MMA_PWR_OFF    = 2,
    ACT_CNT_IDX_CACHE_PWR_OFF  = 3,
    ACT_CNT_IDX_MAX            = 4,
};

#define HCODE_TCC_XCE_MAGIC_NUMBER      0x584356    ///< XCE TCC Shared SRAM marker value ("XCV")
#define HCODE_TCC_XCE_VERSION           1           ///< XCE TCC Shared SRAM layout version number

///
/// @brief IDDQ activity counts
///
typedef union
{
    uint8_t  act_val[MAX_CORES_PER_TAP][ACT_CNT_IDX_MAX];  ///< Activity values per Tap per ACT_CNT_IDX element
    uint32_t act_val_core[MAX_CORES_PER_TAP];       ///< Core Activity values per Tap
} iddq_activity_t;

///
/// @brief XCE created WOF elements
///
typedef struct
{
    TCCShSrScHeader_t  header;                      ///< Region Header
    iddq_activity_t    iddq_activity_values;        ///< Core activity value for IDDQ computations
} XCW_t;

// -----------------------------------------------------------------------------

// Hcode TCC Shared Data Structure
//
// Shared data between DCE, OCE, PCE and XCE

#define HCODE_TCC_SHARED_MAGIC_NUMBER   0x545353    ///< TCC Shared SRAM marker value ("TSS")
#define HCODE_TCC_SHARED_VERSION        1           ///< TCC Shared SRAM layout version number

///
/// @brief TCC XGPE created values Header
///
typedef struct
{

    uint8_t magic_word[3];                          ///< ASCII value XGV
    uint8_t version;                                ///< Version number of layout
    uint8_t reserve[2];                             ///< Reserved
    uint16_t length;                                ///< length in bytes o XGV block including its header
} XgvHdr_t;

///
/// @brief TCC XGPE created values
///
typedef struct
{
    XgvHdr_t xgv_header;                            ///< Region header
    uint16_t vdd_avg_mv;                            ///< VDD voltage averaged over 500u
    uint16_t idd_avg_10ma;                          ///< Idd current averaged over 500us
    uint16_t ics_avg_10mma;                         ///< Ics current averaged over 500us
    uint8_t  reserve;                               ///< Reserved
    uint8_t status;                                 ///< Status
} XgvValues_t;

///
/// @brief TCC XGPE created values region
///
typedef union
{
    uint64_t    xvg_data[2];                        ///< XGPE deposited raw data
    XgvValues_t xvg_layout;                         ///< XGPE deposited mapped data
} XGW_t;

///
/// @brief TCC Shared SRAM Header
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
    uint32_t pce_beacon;                            ///< PGPE Beacon address
    uint16_t dce_data_offset;                       ///< DCE data region offset (from beginning of Shared region)
    uint16_t dce_data_length;                       ///< DCE data region length
    uint16_t oce_data_offset;                       ///< OCE data region offset (from beginning of Shared region)
    uint16_t oce_data_length;                       ///< OCE data region length
    uint16_t pce_data_offset;                       ///< PCE data region offset (from beginning of Shared region)
    uint16_t pce_data_length;                       ///< PCE data region length
    uint16_t xce_data_offset;                       ///< XCE data region offset (from beginning of Shared region)
    uint16_t xce_data_length;                       ///< XCE data region length
    uint16_t xgpe_data_offset;                      ///< XGPE data region offset (from beginning of Shared region)
    uint16_t xgpe_data_length;                      ///< XGPE data region length
    uint16_t errlog_table_offset;                   ///< Hcode Error Log Table offset (from beginning of Shared region)
    uint16_t errlog_table_length;                   ///< Hcode Error Log Table length
} TCCShSrHeader_t;

///
/// @brief TCC Shared SRAM Region
///
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
