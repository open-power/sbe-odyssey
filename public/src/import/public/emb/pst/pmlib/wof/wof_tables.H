/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/wof/wof_tables.H $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

/**
 * @file  wof_tables.H
 * @brief Access WOF Adjustment/Limit Tables
 */

// *HWP HW Owner        : Greg Still<stillgs@us.ibm.com>
// *HWP FW Owner        : Prasad Bg Ranganath <prasadbgr@in.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 2
// *HWP Consumed by     : PGPE:PCE:HB:OCC

#ifndef __WOF_TABLES_H__
#define __WOF_TABLES_H__

#include <pst_ppb_common.H>

#ifndef __PPE_PLAT
    #include <fapi2.H>
    #include <map>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef __PPE_PLAT

unsigned int trace_ppe_hash(const char*, int);
typedef unsigned int trace_hash_val;
#include "iota.h"
#include "ppe42_string.h"
#include "iota_trace.h"


#define WOF_INF PK_TRACE_INF
#define WOF_DBG PK_TRACE_DBG
#define WOF_ERR PK_TRACE_ERR
#elif __OCC_PLAT
#define WOF_INF printf
#define WOF_DBG printf
#define WOF_ERR printf
#else
#define WOF_INF FAPI_INF
#define WOF_DBG FAPI_DBG
#define WOF_ERR FAPI_ERR
#endif

// --------------------------------------------------------

/// @defgroup wof_adjlimt_tables WOF Adjustment Tables
/// @{

/// @brief Offset/Entries struct
typedef struct
{
    /** @brief Offset (in bytes) from the beginning of the WTH to the start of the indicted record  */
    uint16_t     offset;

    /** @brief Entries of the indicted record.  The length (in bytes) of the record is contained in the record header */
    uint16_t     entries;
} WTH_ofsent_t;

/// @brief WOF Table Header
typedef struct
{
    //  ---- Double word 0 -----

    ///  Magic value to identify this structure
    char    magic_value[3];

    /// Header version in X.Y form (nibble X = major, nibble Y = minor)
    uint8_t     version;

    /// Major chip revision - X in Chip DD Level X.Y
    uint8_t     dd_major;

    /// Minor chip revision - X in Chip DD Level X.Y
    uint8_t     dd_minor;

    /// Indicates the table contains Override information
    uint8_t     override_enable;

    /// Small core count
    uint8_t     mod_sm_core_count;

    //  ---- Double word 1 -----

    /// Configured Module small core count
    uint8_t     mod_sm_core_count_cfg;

    /// Tap module count - number of Tap chips present
    uint8_t     tap_mod_count;

    /// @brief Base record offset - offset (in bytes) from the beginning of this header to the base_offset field of this header. \n
    /// This allows this header to change and have the location of the base_offset to move without consuming code having to be updated.
    uint8_t     base_rec_offset;

    /// Number of Adjustment dimensions
    uint8_t     adjust_num;

    /// Number of Limit dimensions
    uint8_t     limit_num;

    /// Available Table Entries
    uint8_t     avail_table_entries;

    /// Overall Size of the table from the beginning of this header to the end of the last dimension record
    uint16_t    table_size;

    //  ---- Double word 2 -----

    /// VDD CeffRatio Start value represented by index 0 (in 0.5%)
    uint8_t     vddceff_start;

    /// VDD CeffRatio Stride between each CeffVdd index (in 0.5%)
    uint8_t     vddceff_stride;

    /// VDD CeffRatio Steps (indexes)
    uint8_t     vddceff_steps;

    /// CeffVdd data cell size (in bytes)
    uint8_t     vddceff_data_size;

    /// Override Match Frequency (in MHz)
    uint16_t    override_match_freq_mhz;

    /// Override Match Socket Power (in Watts)
    uint16_t    override_match_power_w;

    //  ---- Double word 3 -----

    /// @briefSort WOF Base Frequency (in MHz)
    /// The #V frequency associatedwith the sort power target for this table set.
    uint16_t    sort_wof_base_freq_mhz;

    /// Core Sort Power Save Frequency (in MHz) (eg Fmin @ Vmin)
    uint16_t    sort_power_save_freq_mhz;

    /// Core Sort UltraTurbo Frequency (in MHz)
    uint16_t    sort_ultraturbo_freq_mhz;

    /// Core Sort Fixed Frequency (in MHz)
    uint16_t    sort_fixed_freq_mhz;

    //  ---- Double word 4 -----

    /// Frequency at which throttling is used for further reduction of Ceff Overages
    uint16_t    sort_throttle_freq_mhz;
    uint8_t     reserved4[6];

    //  ---- Double word 5 -----

    /// Total Socket Power (in Watts)
    uint16_t    tot_skt_power_tgt_w;

    /// Total TAP Power (in Watts)
    uint16_t    tot_tap_power_tgt_w;

    ///Total Spinal Power (in Watts)
    uint16_t    tot_spl_power_tgt_w;

    /// Fixed Frqeuency Mode Power Target (in Watts)
    uint16_t    ff_power_tgt_w;

    //  ---- Double word 6 -----

    /// VDN voltage (mv)
    uint16_t    vdn_mv;

    /// VIO voltage (mv)
    uint16_t    vio_mv;

    /// Regulator Design Point Current Capacity (in Amps)
    uint16_t    rdp_current_a;

    /// Boost Current Capacity (in Amps)
    uint16_t    boost_current_a;

    //  ---- Double word 7 -----

    uint8_t     reserved7[8];

    //  ---- Double word 8 -----

    uint8_t     reserved8[8];

    //  ---- Double word 9 -----

    uint8_t     reserved9[4];

    /// Date Timestamp of the table set as established by the generation process.
    uint32_t    table_date_timestamp;

    //  ---- Double word 10 and 11 ----

    /// Table Version - Up to 16 ASCII characters as a Table Version as established by the generation process.
    char    table_version[16];

    //  ---- Double word 12 -----

    /// Part Number - Up to 8 ASCII characters as a part number
    char    part_number[8];

    /** @brief Array of Dimension offset/lengths
        @verbatim
        Base/Adjustment/Limit
            Offset (in bytes) from the beginning of this header to the start of the table record.
            Length (in bytes) of the dimension is in the table record.

        @endverbatim
     */
    WTH_ofsent_t    dim_ofslen[38];
#ifndef DOXYGEN
} __attribute__((packed, aligned(128)))WofTablesHeader_t;
#else
}
WofTablesHeader_t;
#endif

///
/// @section WOF Table Record Header
///

#define     VDD_CEFFRATIO   0
#define     CRATIO          1
#define     IO              2
#define     HEATSINK        3
#define     ICREDIT_MMA     4
#define     NOISY_NEIGHBOR  5
#define     NUM_DIMS        6

/// @cond WFT_DIMENSIONS
extern const char* WFT_DIMENSION_NAMES[];
#define WFT_DIMENSION_NAMES_STRINGS \
    { \
        "VDD CeffRatio", \
        "CRatio", \
        "IO", \
        "Heatsink", \
        "ICredit MMA", \
        "Noisy Neighbor" \
    }
#define WFT_DIMENSION_NAMES_VAR WFT_DIMENSION_NAMES[] = WFT_DIMENSION_NAMES_STRINGS
#define WFT_DIMENSION_NAME(_m_view)(WFT_DIMENSION_NAMES[_m_view])
/// @endcond

/// WOF Table Record Header Record Types
#define     BASE            1
#define     ADJUST          2
#define     LIMIT           3

// @cond WFT_REC_TYPE
extern const char* WFT_REC_TYPE_NAMES[];
#define WFT_REC_TYPE_NAMES_STRINGS \
    { \
        "NULL", \
        "Base", \
        "Adjust", \
        "Limit" \
    }
#define WFT_REC_TYPE_NAMES_VAR WFT_REC_TYPE_NAMES[] = WFT_REC_TYPE_NAMES_STRINGS
#define WFT_REC_TYPE_NAME(_m_view)(WFT_REC_TYPE_NAMES[_m_view])
/// @endcond

/// WOF Table Record Header Speed Types
#define     ABS_FREQ        0
#define     ABS_PSTATE      1
#define     DELTA_FREQ      2
#define     DELTA_PSTATE    3

// @cond WFT_SPEED_TYPE
extern const char* WFT_SPEED_TYPE_NAMES[];
#define WFT_SPEED_TYPE_NAMES_STRINGS \
    { \
        "Absolute Frequency", \
        "Absolute Pstate", \
        "Delta Frequency", \
        "Delta Pstate" \
    }
#define WFT_SPEED_TYPE_NAMES_VAR WFT_SPEED_TYPE_NAMES[] = WFT_SPEED_TYPE_NAMES_STRINGS
#define WFT_SPEED_TYPE_NAME(_m_view)(WFT_SPEED_TYPE_NAMES[_m_view])
/// @endcond

/// WOF Table Record Header Throt Types
#define     SIGNED_OVG      1
#define     THROT_INDEX     2

// @cond WFT_THROT_TYPE
extern const char* WFT_THROT_TYPE_NAMES[];
#define WFT_THROT_TYPE_NAMES_STRINGS \
    { \
        "Signed Ceff Overage", \
        "Throttle Index" \
    }
#define WFT_THROT_TYPE_NAMES_VAR WFT_THROT_TYPE_NAMES[] = WFT_THROT_TYPE_NAMES_STRINGS
#define WFT_THROT_TYPE_NAME(_m_view)(WFT_THROT_TYPE_NAMES[_m_view])
/// @endcond

/// @struct WOFTableRecordHeader_t
/// @brief WOF Table Record Header
/// This structure identifies the content of a dimension record
typedef struct
{
    /// @brief Magic value to identify this structure
    char        magic_value[3];  //< Magic value

    // Deal with a Doxygen formating bug
    /// @cond
    union
    {
        /// @endcond
        /// Header version in X.Y form (nibble X = major, nibble Y = minor)
        uint8_t     version;
        /// @cond
        struct
        {
            uint8_t version_major : 4;
            uint8_t version_minor : 4;
        };

    };
    /// @endcond

    /// Size of the record in bytes from charid_ver to the end of the WTR entries
    uint16_t    rec_size;

    /**
     * @brief Record Type and Encoding
       @verbatim
       bit      0 - Reserved
       bits 1:3
         Encoding
           0    = Reserved
           1    = base  (type encoding 0 or 1)
           2    = adjustment (type encoding 2)
           3    = limit  (type encoding 0 or 1)
           4-7 = Reserved
       bits 4:5 – Freq/PS encodings
         Encoding
           0    =  Absolute Frequency (SEEPROM)
           1    =  Absolute Pstate (HOMER)
           2    =  Signed Delta Frequency
           3    =  Signed Delta Pstate
       bits 6:7 – Throttle  encodings
         Encoding
           0    =  Reserved
           1    =  Signed Ceff Overage %  (0.5%)
           2    =  throttle index (1.5625%)
           3    =  Reserved
       @endverbatim
     */
    uint8_t     rec_type;  ///< RecType

    /**
     *  @brief Dimension Type
        @verbatim
        0 = Vdd CeffRatio
        1 = Cratio
        2 = Heatsink
        3 = I/O
        4 = Icredit/MMA
        5 = Noisy Neighbor
        @endverbatim
     */
    uint8_t     dim_type;

    /**
     *  @brief Dimension Start value (units dependent @ref dim_type)
        @verbatim
        Vdd CeffRatio - 0 (N/A) as this is carried in the WOF Table Header
        Cratio - Minimum Ratio (0.5%)
        Heatsink - Minimum Temp (degC)
        I/O - Minimum Power(W)
        Icredit/MMA - Undefined
        Noisy Neighbor - Undefined
        @endverbatim
     */
    uint16_t    dim_start;

    /**
     *  @brief Dimension Stride (value depends @ref dim_type)
        @verbatim
        Vdd CeffRatio - 0 (N/A) as this is carried in the WOF Table Header
        Cratio - Step Ratio (0.5%)
        Heatsink - Step Temp (degC)
        I/O - Step Power(W)
        Icredit/MMA - Undefined
        Noisy Neighbor - Undefined
        @endverbatim
     */
    int16_t     dim_stride;

    /// Number of steps in this dimension
    uint16_t    dim_steps;

    /**
     * @brief Fractional decimial digits of dim_start and dim_stride values
     *
     * Maximums:  0 -> 65535, 1 -> 6553.5, 2 -> 655.35, 3 -> 65.535
     * @see dim_start
     * @see dim_stride
     */
    uint8_t     dim_precision;

    /// Index representing the Thermal Design Point (TDP) conditions
    uint8_t     tdp_idx;

#ifndef DOXYGEN
} __attribute__((packed, aligned(2)))WOFTableRecordHeader_t;
#else
}  WOFTableRecordHeader_t;
#endif

/// @struct WOFTableRecordEntry_t
/// @brief WOF Table Record Entry
typedef struct
{
    uint8_t  freq_ps;       // Frequency or Pstate depending on the rec_type.
    int8_t   throt;         // Amount of Ceff Overage (in 0.5%) that should be removed via throttling
} WOFTableRecordEntry_t;

#define WOF_TABLE_MAX_SIZE  10*1024  // For SRAM and HOMER allocation checking purposes

/// @brief Get dimension WTR address
/// @param[in]  i_wft   Pointer to WOF tables
/// @param[in]  i_dim   Dimension of interest
/// @return     Pointer to WOF Table Record
WOFTableRecordHeader_t* wft_get_dim_address(
    uint8_t* i_wft,
    const uint32_t i_dim);

/// @brief Get dimension WTR address
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Character pointer of the WFR magic value
char* wft_get_dim_magic(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension WFR version
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Pointer to WFR header version
uint32_t wft_get_dim_version(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get dimension record size
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Pointer to WFR total record size
uint32_t wft_get_dim_rec_size(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension type
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension type value
uint32_t wft_get_dim_type(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension start value whose units is dependent on the dimension
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension start value
uint32_t wft_get_dim_start(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension stride value whose units is dependent on the dimension
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension stride value

int32_t wft_get_dim_stride(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the number of dimension step
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension start value
uint32_t wft_get_dim_steps(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension start and stride precision divisor in power of 10
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension precision value
uint32_t wft_get_dim_precision(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension record type
/// @param[in]  i_wtr   Pointer to WOF Table Record
/// @return     Dimension record type ENUM value
/// @see wft_rec_type_t
uint32_t wft_get_dim_rec_type(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension speed type
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension speed type ENUM value
/// @see wft_speed_type_t
uint32_t wft_get_dim_speed_type(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension throttle type
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     Dimension throttle type ENUM value
/// @see wft_throt_type_t
uint32_t wft_get_dim_throt_type(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get the dimension TDP index
/// @param[in]  i_wtr   Pointer to WOF dimension Record
/// @return     TDP index value
uint32_t wft_get_dim_tdp_idx(
    WOFTableRecordHeader_t* i_wtr);

/// @brief Get a record entry speed value
/// @param[in]  i_wtr        Pointer to WOF dimension Record
/// @param[in]  i_idx        Dimension index
/// @param[in]  i_ceff_steps Number of CeffRatio steps
/// @param[in]  i_ceff_idx   CeffRatio index
/// @return     Dimension speed (frequency or Pstate) dependent on encoding
/// @see wft_speed_type_t
uint32_t wft_get_dim_speed(
    WOFTableRecordHeader_t* i_wtr,
    uint32_t i_dim_idx,
    uint32_t i_ceff_steps,
    uint32_t i_ceff_idx);

/// @brief Get a record entry throttle value
/// @param[in]  i_wtr        Pointer to WOF dimension Record
/// @param[in]  i_idx        Dimension index
/// @param[in]  i_ceff_steps Number of CeffRatio steps
/// @param[in]  i_ceff_idx   CeffRatio index
/// @return     Dimension throttle dependent on encoding
/// @see wft_throt_type_t
uint32_t wft_get_dim_throt(
    WOFTableRecordHeader_t* i_wtr,
    uint32_t i_dim_idx,
    uint32_t i_ceff_steps,
    uint32_t i_ceff_idx);

/// @struct WOFIndexes_t
/// @brief WOF indexes necessary to compute a WOF Clip
/// This structure holds the indexes necessary to compute a WOF Clip.
typedef struct
{
    uint32_t    ceff_idx;
    uint32_t    cratio_idx;
    uint32_t    ambient_dimm_idx;
    uint32_t    io_idx;
    uint32_t    icredit_mma_idx;
    uint32_t    noisy_neighbor_idx;
} WOFIndexes_t;

/// @brief Compute the WOF Clip given the input indexes
/// @param[in]  i_wtr       Pointer to WOF dimension Record
/// @param[in]  i_indexes   Reference to index structure
/// @return     WOF clips for both frequency and throttle
/// @see WOFIndexes_t
WOFTableRecordEntry_t wft_compute_clip(
    uint8_t* i_wtr,
    uint32_t i_indexes[]);

/// @}  // End of WOF Adjustment Tables Group

#if !defined(DFT) && !defined(__PPE_PLAT)

/// @brief Print the WOF Table Header
/// @param[in]  i_wof_table_data Pointer to WOF Adjustment/Limit table
/// @param[in]  i_hub_target_str String to identify Hub Chip Target
void wfth_print(
    uint8_t*    i_wft,
    char*       i_hub_target_str);

/// @brief Views of the WOF Tables to print
enum wft_print_view_t
{
    WFT_PRINT_RAW,
    WFT_PRINT_DECODED
};

/// @cond WFT_NAMES
extern const char* WFT_VIEW_NAMES[];
#define WFT_VIEW_NAMES_STRINGS \
    { \
        "Raw", \
        "Decoded" \
    }
#define WFT_VIEW_NAMES_VAR WFT_VIEW_NAMES[] = WFT_VIEW_NAMES_STRINGS
#define WFT_VIEW_NAME(_m_view)(WFT_VIEW_NAMES[_m_view])
/// @endcond

/// @brief Print the WOF Table Data
/// @param[in]  i_wof_table_data Pointer to WOF Adjustment/Limit table
/// @param[in]  i_hub_target_str String to identify Hub Chip Target
/// @param[in]  i_view    String to identify Hub Chip Target
void wftd_print(
    uint8_t*                i_wft,
    char*                   i_hub_target_str,
    const wft_print_view_t  i_view);

#endif // not DFT or PPE

/// @}  // End of WOF PPB Group

/// @brief Internal API test
/// @param[in]  i_wfr   Pointer to WOF dimension Record
void wft_api_test(uint8_t* i_wft);


#ifdef __cplusplus
} // end extern C
#endif
#endif    /* __WOF_TABLES_H__ */