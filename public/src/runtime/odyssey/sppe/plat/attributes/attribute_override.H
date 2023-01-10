/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attribute_override.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
// JIRA: PFSBE-254
// TODO : Attribute override is common for odyssey and p11. As p11 attribute has not
//        been setup yet(addition of p11 SBE filter XML, etc...), for the time being,
//        attribute update related changes have been checked-in under runtime Odyssey
//        sppe code. Once p11 SBE attribute XML has been updated, move these files to
//        runtime common code.

#ifndef __ATTRIBUTE_OVERRIDE_H__
#define __ATTRIBUTE_OVERRIDE_H__

#include <attribute_ids.H>
#include <target_types.H>

namespace fapi2
{
namespace ATTR
{

enum AttributeOverrideRc
{
    ATTROVERRIDE_RC_SUCCESS,
    ATTROVERRIDE_RC_TGT_TYPE_NOT_FOUND,
    ATTROVERRIDE_RC_ATTR_NOT_FOUND,
    ATTROVERRIDE_RC_SIZE_NOT_MATCHING,
    ATTROVERRIDE_RC_TARGET_INST_OUT_RANGE,
    ATTROVERRIDE_RC_ARRAY_INDEX_OUT_RANGE,
    ATTROVERRIDE_RC_ATTR_NOT_ARRAY,
    ATTROVERRIDE_RC_OVRD_NOT_ALLOWED
};

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

class AttributeUpdateBuffer
{
    public:
        HeaderEntry_t   *iv_headerPtr;
        uint8_t         *iv_currPtr;
        uint32_t        iv_tgtIter;
        uint16_t        iv_attrIter;
        uint16_t        iv_attrEnd;
        ENTRY_TYPE      iv_nextEntryType;

        AttributeUpdateBuffer() = delete;
        AttributeUpdateBuffer(const AttributeUpdateBuffer& ) = delete;
        AttributeUpdateBuffer&
            operator = (const AttributeUpdateBuffer& ) = delete;

        AttributeUpdateBuffer(const void *i_ptr)
        {
            iv_headerPtr = (HeaderEntry_t *)i_ptr;
            iv_currPtr   = (uint8_t *)i_ptr + sizeof(HeaderEntry_t);
            iv_tgtIter   = 0;
            iv_attrIter  = 0;
            iv_nextEntryType = TARGET_ENTRY;
        }

        HeaderEntry_t* getHeader()
        {
            return iv_headerPtr;
        }

        uint32_t getNumTargets()
        {
            return iv_headerPtr->iv_numTargets;
        }

        TargetEntry_t* getNextTarget();

        AttrEntry_t*   getNextAttr();
};

class ResponseBuffer
{
    public:
        uint32_t         iv_outputPackSize;
        HeaderEntry_t   *iv_headerPtr;
        uint8_t         *iv_currPtr;
        ENTRY_TYPE      iv_nextEntryType;

        ResponseBuffer() = delete;
        ResponseBuffer(const ResponseBuffer& ) = delete;
        ResponseBuffer&
            operator = (const ResponseBuffer& ) = delete;

        ResponseBuffer(void *i_ptr)
        {
            iv_currPtr   = (uint8_t *)i_ptr;
            iv_headerPtr = (HeaderEntry_t *)i_ptr;
            iv_nextEntryType = HEADER_ENTRY;
            iv_outputPackSize = 0;
        }

        void  setHeader(HeaderEntry_t* i_header);

        uint32_t getOutputPackSize()
        {
            return iv_outputPackSize;
        }
};


struct AttributesTable
{
    fapi2::AttributeId      iv_attr_id;
    uint8_t                 iv_max_row; //Row
    uint8_t                 iv_max_col; //Column
    uint8_t                 iv_max_hgt; //Height
    uint8_t                 iv_size;
    void                   *iv_ptr;

    AttributeOverrideRc applyOverride(const uint8_t       i_tgt_inst,
                                      const AttrEntry_t*  i_attrEntry);
};

struct TargetsTable
{
    fapi2::LogTargetType    iv_log_target_type;
    uint8_t                 iv_max_inst;
    uint16_t                iv_attr_row_start;
    uint16_t                iv_attr_row_size;

    AttributeOverrideRc applyOverride(const uint8_t           i_targ_inst,
                                      AttributeUpdateBuffer&  i_buffer);
};

uint32_t    applyOverride(const void *i_buffer,void *o_buffer,
                          uint32_t & o_resPackSize);

extern AttributesTable g_attrsTab[];

extern TargetsTable    g_targetsTab[];

extern uint8_t         g_tgts_tab_size;

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_OVERRIDE_H__
