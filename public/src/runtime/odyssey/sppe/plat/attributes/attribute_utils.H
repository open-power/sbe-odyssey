/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attribute_utils.H $ */
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

#include <attribute_ids.H>
#include <target_types.H>

namespace fapi2
{
namespace ATTR
{

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
    uint32_t   iv_padding;
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