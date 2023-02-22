/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/odyssey/include/ody_frequency_buckets.H $ */
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
//------------------------------------------------------------------------------
/// @file  ody_frequency_buckets.H
/// @brief Odyssey PLL/frequency bucket information
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Joe McGill (jmcgill@us.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#ifndef _ODY_FREQUENCY_BUCKETS_H_
#define _ODY_FREQUENCY_BUCKETS_H_


//
// REFCLOCKS
//

// internal/external reference clock frequencies
// values represented in KHz
enum ody_refclock_freq_t
{
    REFCLOCK_FREQ_133 = 133333,
};


///
/// Odyssey
///

// constants defining number of PLL frequency options ('buckets')
// documented in bucket descriptor structure (currently used/max)
const uint8_t ODY_NUM_PLL_BUCKETS = 16;
const uint8_t ODY_MAX_PLL_BUCKETS = 16;

// PLL bucket descriptor, defines bucket properties in terms of:
// - required input reference clock freqeuncy (KHz)
// - resultant output grid frequency (MHz)
// - resultant output link frequency (MHz)
struct ody_pll_bucket_descriptor_t
{
    ody_refclock_freq_t refclock_freq_khz;  // PLL input frequency (KHz)
    uint32_t            freq_grid_mhz;      // chip grid frequency (MHz)
    uint32_t            freq_link_mhz;      // OMI link frequency (MHz)
};

// array of bucket descriptors, index is bucket number
const ody_pll_bucket_descriptor_t ODY_PLL_BUCKETS[ODY_NUM_PLL_BUCKETS] =
{
    { REFCLOCK_FREQ_133, 1600, 25600 }, // bucket 0  DDR5-3200
    { REFCLOCK_FREQ_133, 2000, 32000 }, // bucket 1  DDR5-4000
    { REFCLOCK_FREQ_133, 2400, 38400 }, // bucket 2  DDR5-4800
    { REFCLOCK_FREQ_133, 2400, 14400 }, // bucket 3  Test-only (Ring Mode)

    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 4  Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 5  Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 6  Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 7  Reserved

    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 8  Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 9  Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 10 Reserved
    { REFCLOCK_FREQ_133, 0,    0     }, // bucket 11 Reserved

    { REFCLOCK_FREQ_133, 0,    0,    }, // bucket 12 Reserved
    { REFCLOCK_FREQ_133, 0,    0,    }, // bucket 13 Reserved
    { REFCLOCK_FREQ_133, 1600, 25600 }, // bucket 14 Sim-only (VBU)
    { REFCLOCK_FREQ_133, 2000, 32000 }, // bucket 15 Sim-only (DFT)
};


#endif
