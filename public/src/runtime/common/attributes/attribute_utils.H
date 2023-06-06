/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_utils.H $      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#ifndef __ATTRIBUTE_UTILS_H__
#define __ATTRIBUTE_UTILS_H__

#include <stdint.h>

namespace fapi2
{
namespace ATTR
{

enum AttributeOverrideRc
{
    ATTROVERRIDE_RC_SUCCESS                 = 0,
    ATTROVERRIDE_RC_TGT_TYPE_NOT_FOUND      = 1,
    ATTROVERRIDE_RC_ATTR_NOT_FOUND          = 2,
    ATTROVERRIDE_RC_SIZE_NOT_MATCHING       = 3,
    ATTROVERRIDE_RC_TARGET_INST_OUT_RANGE   = 4,
    ATTROVERRIDE_RC_ARRAY_INDEX_OUT_RANGE   = 5,
    ATTROVERRIDE_RC_ATTR_NOT_ARRAY          = 6,
    ATTROVERRIDE_RC_OVRD_NOT_ALLOWED        = 7,
};

const uint8_t ATTROVERRIDE_MAJOR_VERSION = 0;
const uint8_t ATTROVERRIDE_MINOR_VERSION = 2;
const uint8_t ATTROVERRIDE_REQ_FILE_TYPE = 1;
const uint8_t ATTROVERRIDE_RESP_FILE_TYPE = 2;

// MAGIC WORD is 'T' 'A' 'R' 'G'
const uint32_t ATTR_FILE_TARGET_MAGIC_WORD = 0x54415247;

struct HeaderEntry_t
{
    uint8_t    iv_fmtMajor;     //format major version
    uint8_t    iv_fmtMinor;     //format minor version
    uint8_t    iv_chipType;     //chip type
    uint8_t    iv_fileType;     //file type
    uint32_t   iv_numTargets;   //number of target sections
};

struct TargetEntry_t
{
    uint8_t    iv_logTgtType;   //LOG_TARGET_TYPE
    uint8_t    iv_instance;     //instance number; 0xFF - applicable for all instance
    uint16_t   iv_numAttrs;     //number of attributes for this target type
    uint32_t   iv_magicWord;    //ASCII characters 'T' 'A' 'R' 'G'
};

struct __attribute__((packed)) AttrEntry_t
{
    uint32_t   iv_attrId;       //attribute ID
    uint16_t   iv_dataSize;     //data size
    uint8_t    iv_row;          //row number
    uint8_t    iv_col;          //col number
    uint8_t    iv_hgt;          //height
    uint8_t    iv_res;          //1-byte reserved
    //data is a variable size
};

struct AttrRespEntry_t
{
    uint32_t            iv_attrId; //attribute ID
    AttributeOverrideRc iv_rc;     //RC code for the failure
};

enum ENTRY_TYPE
{
    HEADER_ENTRY,
    TARGET_ENTRY,
    ATTRIBUTE_ENTRY,
    END_ENTRY
};

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_UTILS_H__
