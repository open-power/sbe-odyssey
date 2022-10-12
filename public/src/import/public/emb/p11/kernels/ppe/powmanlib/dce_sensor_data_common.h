/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/dce_sensor_data_common.h $ */
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
#ifndef __DCE_SENSOR_DATA_COMMON_H__
#define __DCE_SENSOR_DATA_COMMON_H__

#include <stdint.h>
#include <p11_hcd_memmap_tcc_sram.H>
#include <p11_hcode_image_defines.H>


// Quad and Core defs (CMO-TBD: These must be defined somewhere else. Where?)
#define QUADS_PER_TCC           (2)  // 2 Quads per TCC
#define CORES_PER_QUAD          (4)  // 4 Cores per Quad
#define CORES_PER_TCC           (QUADS_PER_TCC * CORES_PER_QUAD) // Cores per TCC

//
// DTS structs
//
typedef union DtsSensorReg
{
    uint64_t value;
    struct
    {
        uint16_t  reading[4];
    } half_words;
} DtsSensorReg_t;

typedef union DtsSensorReading
{
    uint16_t reading;
    struct
    {
        uint16_t result   : 12;
        uint16_t id       :  2;
        uint16_t overflow :  1;
        uint16_t valid    :  1;
    } fields;
} DtsSensorReading_t;

typedef struct
{
    DtsSensorReading_t core[2];
    DtsSensorReading_t mma;
    DtsSensorReading_t cache;
} CoreDataDts_t;

typedef struct
{
    DtsSensorReading_t dragstrip;
} QuadDataDts_t;

//
// Core activity structs
//
typedef struct
{
    uint32_t raw_cycles;        // 0x200
    uint32_t run_cycles;        // 0x208
    uint32_t complete;          // 0x210  Core instruction complete Utilization
    uint32_t freq_sens_busy;    // 0x218  Core workrate busy counter
    uint32_t freq_sens_finish;  // 0x220  Core workrate finish counter
    uint32_t mem_latency_a;     // 0x228
    uint32_t mem_latency_b;     // 0x230
    uint32_t mem_access_c;      // 0x238
} CoreDataEmpath_t;

typedef struct
{
    uint32_t ifu_throttle;      // 0x240  IFU Throttle Block Fetch
    uint32_t ifu_active;        // 0x248  IFU Throttle Active
    uint32_t pwr_proxy;         // 0x250  power proxy
    uint32_t fine_throttle;     // 0x258  Fine Throttle blocked
    uint32_t harsh_throttle;    // 0x260  Harsh Trottle blocked
} CoreDataThrottle_t;

typedef struct
{
    uint32_t v_droop_small;     // 0x268  Small Droop present
    uint32_t v_droop_large;     // 0x270  Large Droop present
    uint32_t mma_active;        // 0x278  MMA Active
} CoreDataDroop_t;

//
// DDS
//
typedef union CpmsSdsr
{
    uint64_t value;
    struct
    {
        uint64_t data : 5;
        uint64_t data_min : 5;
        uint64_t data_max : 5;
        uint64_t not_used : 49;
    } fields;
} CpmsSdsr_t;

typedef union
{
    uint64_t value;
    struct
    {
        uint64_t dds_valid : 1;
        uint64_t dds_reading : 15;
        uint64_t dds_min_valid : 1;
        uint64_t dds_min : 15;
        uint64_t dds_max_valid : 1;
        uint64_t dds_max : 15;
        uint64_t not_used : 16;
    } fields;
} DdsData_t;


//
// The instance of this CoreData_t data object must be 8 byte aligned and size must be a
// muliple of 8. We will use this assumption when initializing the data content in
// core_get_data().
// Notes (in view of the P10 implementation of the CoreData struct):
// - The dts result for dragstrip is unnecessarily measured and repeated four time (per quad) and
//   should ideally be in a quad-level struct in a higher level struct, eg TapData_t.
// - The tod is unnecessarily calculated and repeated eight times (per tap) and should ideally be
//   in a single shared data point in a higher level struct, eg TapData_t.
// - The following CoreData_t struct reflects the above corrections for better and more
//   efficient data organization.
//
typedef struct
{
    CoreDataEmpath_t           empath;          //32
    CoreDataThrottle_t         throttle;        //20
    CoreDataDroop_t            droop;           //12
    CoreDataDts_t              dts;             // 8
    uint32_t                   empath_valid;    // 4
    uint32_t                   stop_state_hist; // 4
    DdsData_t                  dds;             // 8
} CoreData_t;
//
// P10 version (for reference)
//
//typedef struct
//{
//    CoreDataEmpath             empath;          //32
//    CoreDataThrottle           throttle;        //20
//    CoreDataDroop              droop;           //12
//    CoreDataDts                dts;             // 8 <- Changed in new p11 struct
//    uint64_t                   stop_state_hist; // 8
//    uint32_t                   tod_2mhz;        // 4 <- Changed in new p11 struct
//    uint32_t                   empathValid;     // 4 <- Changed in new p11 struct
//    DdsData                    dds;             // 8
//} CoreData;

typedef struct
{
    QuadDataDts_t              dts;             // 2
} QuadData_t;

//
// High-level DCE sensor data struct for DCE, OCC(405) and OCE
//
// Notes:
// - The underlying assumption here is that the DCE data is *live* and can only be updated
//   by the DCE after both the OCE *and* the 405 have used the data.
// - Compared to the P10 implementation approach, which would have consumed a total of
//   8 x 96B = 768B, the below struct reduces this to 8 x 88B + 24B = 728B.
// - The data block is managed in link.ld to guarantee 8-byte alignment.
//
typedef struct
{
    CoreData_t  core_data[CORES_PER_TCC];  // 8 x 88B
    QuadData_t  quad_data[QUADS_PER_TCC];  // 2 x 2B
    uint32_t    tbr_notif_period;          // 4B - Most recent notification period (DB or FIT)
    uint32_t    tbr_notif_rcvd;            // 4B - Most recent notif reception time (DB or FIT)
    uint32_t    tbr_data_collect_duration; // 4B - Data collection duration (since notif_rcvd)
    uint32_t    tod_data_rcvd;             // 4B - For optional use by 405 using local 2MHz TOD
    uint16_t    dce_status_flag;           // 2B - Status vector. See DCE_STATUS_FLAGS enum below.
    uint16_t    undefined;                 // 2B - Pad to 4B
} DCESensorData_t;

extern DCESensorData_t*  G_dce_sensor_data; //Big "G_" because it's shared across CEs.

//
// Enum of DCE status flags (used by DCESensorData_t.dce_status_flags)
//
// Notes:
// - Used for indicating protocol violations to the 405 (and possibly the OCE too).
// - These violations are detected at the time of DB0 IRQ reception and at commencement
//   of data collection.
//
enum DCE_STATUS_FLAGS
{
    DCE_STATUS_RESET              = 0b0000000000000000, //Reset init value
    DCE_STATUS_DB_VIOLATION       = 0b0000000000000001, //DB rcvd before prev data collected.
    DCE_STATUS_OCE_IS_CALCULATING = 0b0000000000000010, //OCE is still calculating IDDQ
    DCE_STATUS_OCE_NOT_RESPONDING = 0b0000000000000100, //OCE did not even use the prev data
    DCE_STATUS_AUTO_MODE          = 0b0000000000001000, //DCE is in AUTO lab mode
};


//
// Variable used by DCE and OCE to signal eachother about their usage of the DCESensorData data.
// - Uses the DCE_DATA_USAGE_STATUS enum below
// - This variable is initially set by DCE to STATUS_NEW upon the first new sensor data reads.
// - OCE will be polling this variable for ==STATUS_NEW upon which it will set this variable
//   to STATUS_USING and start calculating IDDQ.
// - When OCE done calculating IDDQ, it will set this variable to STATUS_USED.
//
typedef struct
{
    uint32_t  status;
} DCESensorDataUsage_t;

extern DCESensorDataUsage_t*  G_dce_sensor_data_usage;

//
// Enum indicating the DCE-OCE usage status of the most recently collected DCESensorData data set.
//
// Notes:
// - The enum contains signal values used to communicate *back and forth*, between the DCE and
//   OCE, the status of the data usage between them via the g_dce_data_usage_status signal
//   variable visible to both DCE and OCE.
//
enum DCE_DATA_USAGE_STATUS
{
    DCE_DATA_USAGE_STATUS_UNDEFINED = 0,
    DCE_DATA_USAGE_STATUS_NEW       = 1, // New data for OCE (set by DCE, cleared by OCE)
    DCE_DATA_USAGE_STATUS_USING     = 2, // Data is being used by OCE (set by OCE, cleared by DCE)
    DCE_DATA_USAGE_STATUS_USED      = 3, // Data has been used by OCE (set by OCE, cleared by DCE)
};

#endif /* __DCE_SENSOR_DATA_COMMON_H__ */
