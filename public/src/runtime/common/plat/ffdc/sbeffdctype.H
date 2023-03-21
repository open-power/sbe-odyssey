/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdctype.H $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
#ifndef __SBE_FFDC_TYPE_H
#define __SBE_FFDC_TYPE_H

#include <endian.h>

/**
 * Bit mapped identifiers
 */
#define SBE_FFDC_HW_DATA        0x0001
#define SBE_FFDC_REG_DATA       0x0002
#define SBE_FFDC_TRACE_DATA     0x0004
#define SBE_FFDC_ATTR_DATA      0x0008
#define SBE_FFDC_ALL_HW_DATA    0x0003  // Get FFDC_HW + FFDC_REG
#define SBE_FFDC_ALL_PLAT_DATA  0x000C  // Get FFDC_ATTR + FFDC_TRACE
#define SBE_FFDC_ALL_DATA       0x000F
#define SBE_FFDC_INVALID        0xFFF0

/**
 *  Header size in words as per FFDC package
 */
#define HW_HEADER_SIZE_IN_WORDS     0x0001 // HW FFDC Fields
#define PLAT_HEADER_SIZE_IN_WORDS   0x0004 // PLAT Header values
#define BLOB_HEADER_SIZE_IN_WORDS   0x0001 // Blob Header size

/// @TODO JIRA: PFSBE-351
/// bellow trace length will be removed once SPPE_PK_TRACE_SIZE_WITH_HEADER
/// can start using in common code by linker.cmd changes
#define SBE_FFDC_TRACE_DUMP_LENGTH  0x2020 // Package SBE Trace Data length

enum SBE_FFDC_DD_LEVEL
{
    SBE_FFDC_DD1 = 1,
};

/**
 * Structure indicating the contents of FFDC package
 * value 'true' - field present;value 'false' - field not present
 * bit_0 - Local Hw dump
 * bit_1 - Hw Reg dump
 * bit_2 - Trace buffer dump
 * bit_3 - Attribute dump
 * bit 4-31 - reserved
 */
typedef struct __attribute__((packed, aligned(8)))
{
#if __BYTE_ORDER == __BIG_ENDIAN
    uint32_t hwField:1;
    uint32_t regField:1;
    uint32_t traceField:1;
    uint32_t attrField:1;
    uint32_t reserved:28;
#else
    uint32_t reserved:28;
    uint32_t attrField:1;
    uint32_t traceField:1;
    uint32_t regField:1;
    uint32_t hwField:1;
#endif
    /**
     *  @breif - set dump fields
     *
     * @param[in] - uint32_t value to be updated
     */
    void set(uint32_t val)
    {
        if(val & SBE_FFDC_HW_DATA)
        {
            hwField = true;
        }
        if(val & SBE_FFDC_REG_DATA)
        {
            regField = true;
        }
        if(val & SBE_FFDC_TRACE_DATA)
        {
            traceField = true;
        }
        if(val & SBE_FFDC_ATTR_DATA)
        {
            attrField = true;
        }
    }
    /**
     *  @brief - get dump fields as uint32_t
     *
     * @return - uint32_t value
     */
    uint32_t get()
    {
        uint32_t l_val = 0;
        if(hwField)
        {
            l_val |= SBE_FFDC_HW_DATA;
        }
        if(regField)
        {
            l_val |= SBE_FFDC_REG_DATA;
        }
        if(traceField)
        {
            l_val |= SBE_FFDC_TRACE_DATA;
        }
        if(attrField)
        {
            l_val |= SBE_FFDC_ATTR_DATA;
        }
        return l_val;
    }
} dumpFields_t;

/**
 * @brief Sturcture indicating the type of ffdc Package [ HW or  User data ] blob
 * and its length in bytes
 */
typedef struct __attribute__((packed, aligned(8)))
{
    uint32_t fieldId:16;
    uint32_t fieldLen:16;
    uint32_t get()
    {
        uint32_t field = (fieldId << 16) + fieldLen;
        return field;
    }
} packageBlobField_t;

/**
 * @brief Structure of ffdc Package blob
 */
typedef struct __attribute__((packed, aligned(8)))
{
    packageBlobField_t  field;
    const void *blobPtr;
} ffdcPackageBlob_t;

/**
 * @brief FFDC HW Data Package Header
 */
typedef struct __attribute__((packed, aligned(8)))
{
    dumpFields_t dumpFields;//bitmapped dumpFields [ HW / REG ]
} hwDataHeader_t;

/**
 * @brief FFDC User Data Package Header
 */
typedef struct __attribute__((packed, aligned(8)))
{
    uint32_t primaryStatus:16;//Chip Op Primary status
    uint32_t secondaryStatus:16;//Chip Op Secondary status
    uint32_t fwCommitID;// FW commit ID
    uint32_t ddLevel;// DD level of the SBE
    dumpFields_t dumpFields; //bitmapped dumpFields [ TRACE / ATTR ]
} sbeDataHeader_t;

#endif //__SBE_FFDC_TYPE_H
