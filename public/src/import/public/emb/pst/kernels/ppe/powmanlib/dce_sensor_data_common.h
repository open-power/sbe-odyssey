/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/dce_sensor_data_common.h $ */
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
#ifndef __DCE_SENSOR_DATA_COMMON_H__
#define __DCE_SENSOR_DATA_COMMON_H__

#include <stdint.h>
#include <pst_hcode_image_defines.H>

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
    DtsSensorReading_t core[2]; //0:ISU, 1:VSU
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


//-------------------------------------------------------------------------------------------------
// Common notes to CoreData_t, QuadData_t and DCESensorData_t structure definitions:
//
// - The following points explain why the final DCESensorData structure is different from p10:
//   - The p10 high-level sensor data struct was named CoreData. But also contained non-core
//     quad level only data. It is listed below for reference to be compared against the pst
//     high-level sensor data struct, DCESensorData_t.
//   - The DTS result for dragstrip was unnecessarily measured and repeated four time (per quad)
//     in p10. It's been put into it's own function call now in get_quad_data() and has its own
//     data struct as well, QuadData_t. (See further below.)
//   - The TOD was unnecessarily calculated and repeated eight times in p10. In pst we have moved
//     timestamp snapshot to the common calling function, dce_get_data(). (Note that we don't
//     have a TOD on the Tap in pst. Instead we take a TBR snapshot.)
//-------------------------------------------------------------------------------------------------

//
// P10's CoreData listed here for reference (Compare with PST's DCESensorData_t)
//
//typedef struct
//{
//    CoreDataEmpath             empath;          //32
//    CoreDataThrottle           throttle;        //20
//    CoreDataDroop              droop;           //12
//    CoreDataDts                dts;             // 8 <- Changed in new pst struct
//    uint64_t                   stop_state_hist; // 8
//    uint32_t                   tod_2mhz;        // 4 <- Changed in new pst struct
//    uint32_t                   empathValid;     // 4 <- Changed in new pst struct
//    DdsData                    dds;             // 8
//} CoreData;

//
// CoreData_t : Core level data struct containing Empath counts and DTS reads
//
// Notes:
// - The CoreData_t object must be 8 byte aligned and the size must be a muliple of 8. This
//   assumption will be used when initializing the data content in get_core_data().
//
typedef struct
{
    CoreDataEmpath_t           empath;          //32
    CoreDataThrottle_t         throttle;        //20
    CoreDataDroop_t            droop;           //12
    CoreDataDts_t              dts;             // 8
    uint32_t                   empath_valid;    // 4
    uint32_t                   stop_state_hist; // 4 - .dds is 8B aligned after this
    DdsData_t                  dds;             // 8 - This is an 8B entity
} CoreData_t;

//
// QuadData_t : Quad level data struct containing DTS reads
//
// Notes: See previous notes to CoreData_t.
//
typedef struct
{
    QuadDataDts_t              dts;             // 2
} QuadData_t;


//
// DCESensorData_t : High-level DCE sensor data struct for DCE, OCE and 405
//
// Notes:
// - See previous notes to CoreData_t.
// - Size of this data block is 8 x 88B + 3 x 8 x 2B + 12B + 20B = 772B
// - With a future increase to 12 cores and 3 quads, size grows to = 1164 Bytes
// - The data block is managed in link.ld where it is 8-byte aligned for PSPI
//   transfer to the 405 on VC3.
//
typedef struct
{
    CoreData_t  core_data[MAX_CORES_PER_TAP];      // 8 x 88B
    uint16_t    core_temp[MAX_CORES_PER_TAP];      // 8 x 2B  [0.5C] unit
    uint16_t    mma_temp[MAX_CORES_PER_TAP];       // 8 x 2B  [0.5C] unit
    uint16_t    cache_temp[MAX_CORES_PER_TAP];     // 8 x 2B  [0.5C] unit
    QuadData_t  quad_data[MAX_QUADS_PER_TAP];      // 2 x 2B
    uint16_t    dragstrip_temp[MAX_QUADS_PER_TAP]; // 2 x 2B  [0.5C] unit
    uint16_t    dragstrip_temp_avg;                // 2B      [0.5C] unit
    uint16_t    present_cores;                     // 2B - Enabled cores (bit(0) is core 0)
    uint32_t    tbr_notif_period;          // 4B - Most recent notification period (DB or FIT)
    uint32_t    tbr_notif_rcvd;            // 4B - Most recent notif reception time (DB or FIT)
    uint32_t    tbr_data_collect_duration; // 4B - Data collection duration (since notif_rcvd)
    uint32_t    tod_data_rcvd;             // 4B - For optional use by 405 using local 2MHz TOD
    uint16_t    dce_status_flag;           // 2B - Status vector. See DCE_STATUS_FLAGS enum below.
    uint16_t    dce_error_code;            // 2B - Error code. See DCE_ERROR_CODES enum below.
} DCESensorData_t;


//
// Enum of DCE status flags (used by DCESensorData_t.dce_status_flag)
//
enum DCE_STATUS_FLAGS
{
    DCE_STATUS_RESET              = 0b0000000000000000, //Reset init value
    DCE_STATUS_OCC_MODE           = 0b0000000000000001, //DCE is in OCC mode
    DCE_STATUS_AUTO_MODE          = 0b0000000000000010, //DCE is in AUTO lab mode
    DCE_STATUS_DTS_FAILURE        = 0b0000000000000100, //One or more DTSs invalid in some way
    DCE_STATUS_UNEXPECTED_DB      = 0b0000000000001000, //DB rcvd out of order
    DCE_STATUS_OCE_IS_CALCULATING = 0b0000000000010000, //OCE is still calculating IDDQ
    DCE_STATUS_OCE_NOT_RESPONDING = 0b0000000000100000, //OCE did not use the prev data
};

//
// Enum of DCE error coded (used by DCESensorData_t.dce_error_code)
//
// Notes:
// - Used for indicating error status to the 405 and OCE.
//
enum DCE_ERROR_CODES
{
    DCE_RC_TOO_MANY_DTS_FAILS = 0b0000000000000001, //Eg, too many accumulated DTS fails
    DCE_RC_OUT_OF_SYNC        = 0b0000000000000010, //Eg, too many DB rcvd but OCE didn't finish
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
    DCE_DATA_USAGE_STATUS_UNDEFINED = 0xff,
    DCE_DATA_USAGE_STATUS_NEW       = 0x01, // New data for OCE (set by DCE, cleared by OCE)
    DCE_DATA_USAGE_STATUS_USING     = 0x02, // Data is in use by OCE (set by OCE, cleared by DCE)
    DCE_DATA_USAGE_STATUS_USED      = 0x03, // Data was used by OCE (set by OCE, cleared by DCE)
};

#endif /* __DCE_SENSOR_DATA_COMMON_H__ */
