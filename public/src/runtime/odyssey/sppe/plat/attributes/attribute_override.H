/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attribute_override.H $ */
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
// JIRA: PFSBE-254
// TODO : Attribute override is common for odyssey and p11. As p11 attribute has not
//        been setup yet(addition of p11 SBE filter XML, etc...), for the time being,
//        attribute update related changes have been checked-in under runtime Odyssey
//        sppe code. Once p11 SBE attribute XML has been updated, move these files to
//        runtime common code.

#ifndef __ATTRIBUTE_OVERRIDE_H__
#define __ATTRIBUTE_OVERRIDE_H__

#include <attribute_utils.H>

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

const uint8_t ATTROVERRIDE_MAJOR_VERSION = 0;
const uint8_t ATTROVERRIDE_MINOR_VERSION = 2;
const uint8_t ATTROVERRIDE_REQ_FILE_TYPE = 1;
const uint8_t ATTROVERRIDE_RESP_FILE_TYPE = 2;

// MAGIC WORD is 'T' 'A' 'R' 'G'
const uint32_t ATTR_FILE_TARGE_MAGIC_WORD = 0x54415247;

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

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_OVERRIDE_H__
