/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attribute_override_service.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include <attribute_table.H>
#include <sbetrace.H>
#include "ppe42_string.h"

namespace fapi2
{
namespace ATTR
{

TargetEntry_t* AttributeUpdateBuffer::getNextTarget()
{
    if ((iv_nextEntryType != TARGET_ENTRY) ||
        (iv_tgtIter >= this->getNumTargets()))
    {
        return nullptr;
    }

    // Update the current pointer
    TargetEntry_t* ptr = (TargetEntry_t *)iv_currPtr;
    iv_currPtr = (uint8_t *)ptr + sizeof(TargetEntry_t);
    iv_nextEntryType = ATTRIBUTE_ENTRY;

    // Set the attribute iterators
    iv_attrIter = 0;
    iv_attrEnd  = ptr->iv_numAttrs;

    // Increment the target iterator
    iv_tgtIter++;
    return ptr;
}

AttrEntry_t* AttributeUpdateBuffer::getNextAttr()
{
    if ((iv_nextEntryType != ATTRIBUTE_ENTRY) ||
        (iv_attrIter >= iv_attrEnd))
    {
        return nullptr;
    }

    AttrEntry_t* ptr = (AttrEntry_t *)iv_currPtr;
    // Including data, AttrEntry_t is in multiple of 8-bytes;
    uint8_t padding = (8 - ((sizeof(AttrEntry_t) + ptr->iv_dataSize) % 8));
    iv_currPtr = (uint8_t *)ptr + sizeof(AttrEntry_t) + ptr->iv_dataSize + padding;

    iv_attrIter++;

    if (iv_attrIter >= iv_attrEnd)
    {
        if (iv_tgtIter >= this->getNumTargets())
        {
            iv_nextEntryType = END_ENTRY;
        }
        else
        {
            iv_nextEntryType = TARGET_ENTRY;
        }
    }
    return ptr;
}

void  ResponseBuffer::setHeader(HeaderEntry_t* i_header)
{
    if ( (i_header == nullptr) || (iv_nextEntryType != HEADER_ENTRY) )
    {
        return;
    }
    memcpy((void *)iv_headerPtr, (uint8_t *)i_header, sizeof(HeaderEntry_t));
    iv_currPtr += sizeof(HeaderEntry_t);
    iv_outputPackSize += sizeof(HeaderEntry_t);
}

AttributeOverrideRc AttributesTable::applyOverride(
                    const uint8_t       i_tgt_inst,
                    const AttrEntry_t*  i_attrEntry)
{

    SBE_INFO("iv_max_row=%d, iv_max_col=%d, iv_max_hgt=%d",
                iv_max_row, iv_max_col, iv_max_hgt);

    // Attribute override supports 4-dimensional array ie. instance, row,
    // col, height. The "instance" is used internally to store instance
    // specific values. So, effectively, 3-dimensional arrays are supported.
    // If an attribute type is 1-dimensional array, then the size of the
    // remaining dimensions will be 1. Similarly, for a 2-dimensional array,
    // the size of the 3rd dimension will be 1.
    // If the target supports only one instance, then the size of the instance
    // is 1.  For a normal attribute (not an array type), the size of the row,
    // col, and height will be zero.
    //

    // Data is preset at the end of AttrEntry structure
    uint8_t* l_data = (uint8_t *)i_attrEntry + sizeof(AttrEntry_t);

    // iv_size provides the size of an element in bytes.
    // So, for pointer arithmatic, type cast iv_ptr to uint8_t pointer
    // and then add iv_size times the index to get the actual location.
    if ( (iv_max_row == 0) && (iv_max_col == 0) && (iv_max_hgt == 0) )
    {
        // As per the attribute table, the attribute is a normal attribute. The
        // request is also to update a normal attribute.
        if ((i_attrEntry->iv_row == 0xFF) && (i_attrEntry->iv_col == 0xFF) &&
            (i_attrEntry->iv_hgt == 0xFF))
        {
            if ( i_attrEntry->iv_dataSize != iv_size )
            {
                SBE_ERROR("The size is not matching. Expected:%d Actual:%d",
                                i_attrEntry->iv_dataSize, iv_size);
                return ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
            }
            memcpy((reinterpret_cast<uint8_t *>(iv_ptr))+(iv_size*i_tgt_inst),
                        l_data, i_attrEntry->iv_dataSize);
        }
        else
        {
            // As per the attribute table, the attribute is a normal attribute. The
            // request is to update an array attribute.
            SBE_ERROR("Not an array attribute");
            return ATTROVERRIDE_RC_ATTR_NOT_ARRAY;
        }
    }
    else
    {
        uint32_t index = 0;

        // The request is to update all the elements of the array
        if ( (i_attrEntry->iv_row == 0xFF) && (i_attrEntry->iv_col == 0xFF) &&
             (i_attrEntry->iv_hgt == 0xFF) )
        {
            uint32_t arraySize = (iv_max_row * iv_max_col * iv_max_hgt * iv_size);

            if ( i_attrEntry->iv_dataSize != arraySize )
            {
                SBE_ERROR("The size is not matching. Expected:%d Actual:%d",
                                i_attrEntry->iv_dataSize, iv_size);
                return ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
            }
            index = (i_tgt_inst * arraySize);
        }
        else if ( (i_attrEntry->iv_row >= iv_max_row) ||
                  (i_attrEntry->iv_col >= iv_max_col) ||
                  (i_attrEntry->iv_hgt >= iv_max_hgt) )
        {
            SBE_ERROR("Attribute array index out of range");
            return ATTROVERRIDE_RC_ARRAY_INDEX_OUT_RANGE;
        }
        else
        {
            // The request is to update an element of an array type attribute
            if ( i_attrEntry->iv_dataSize != iv_size )
            {
                SBE_ERROR("The size is not matching. Expected:%d Actual:%d",
                                i_attrEntry->iv_dataSize, iv_size);
                return ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
            }
            index = (((i_tgt_inst * iv_max_row + i_attrEntry->iv_row) *
                       iv_max_col + i_attrEntry->iv_col) *
                       iv_max_hgt + i_attrEntry->iv_hgt) * iv_size;
        }
        memcpy((reinterpret_cast<uint8_t *>(iv_ptr))+index, l_data,
                i_attrEntry->iv_dataSize);
    }
    return ATTROVERRIDE_RC_SUCCESS;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
inline AttributeOverrideRc findAttrIdx(const uint32_t i_attrId,
                                       const uint16_t i_start,
                                       const uint16_t i_size,
                                       uint32_t & o_attrIdx)
{
    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_ATTR_NOT_FOUND;
    o_attrIdx = 0;

    for (uint16_t i=0; i<i_size; i++)
    {
        if (g_attrsTab[i_start+i].iv_attr_id == i_attrId)
        {
            o_attrIdx = i_start+i;
            l_rc = ATTROVERRIDE_RC_SUCCESS;
            break;
        }
    }
    return l_rc;
}

AttributeOverrideRc TargetsTable::applyOverride(
                    const uint8_t             i_targ_inst,
                    AttributeUpdateBuffer&    i_buffer)
{
    SBE_INFO("iv_max_inst=%d, iv_attr_row_start=%d, iv_attr_row_size=%d",
                iv_max_inst, iv_attr_row_start, iv_attr_row_size);

    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_SUCCESS;

    AttrEntry_t *l_attrEntry = nullptr;
    uint32_t     l_attrIdx  = 0;
    uint8_t l_targ_inst_st  = (i_targ_inst == 0xFF) ? 0 : i_targ_inst;
    uint8_t l_targ_inst_end = (i_targ_inst == 0xFF) ? this->iv_max_inst :
                                                      i_targ_inst + 1;

    SBE_INFO("l_targ_inst_start=%d, l_targ_inst_end=%d",
                        l_targ_inst_st, l_targ_inst_end);

    while ( ((l_attrEntry=i_buffer.getNextAttr()) != nullptr) &&
            (l_rc == ATTROVERRIDE_RC_SUCCESS) )
    {
        l_rc = findAttrIdx(l_attrEntry->iv_attrId, this->iv_attr_row_start,
                           this->iv_attr_row_size, l_attrIdx);
        if (l_rc != ATTROVERRIDE_RC_SUCCESS)
        {
            SBE_ERROR("Match not found for i_attr_id=0x%08X",
                            l_attrEntry->iv_attrId);
            break;
        }
        for (uint8_t l_targ_inst=l_targ_inst_st;
                    l_targ_inst < l_targ_inst_end; l_targ_inst++)
        {
            l_rc = g_attrsTab[l_attrIdx].applyOverride(l_targ_inst,
                                                        l_attrEntry);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR("g_attrsTab[l_attrIdx].applyOverride failed."
                    "AttributeId : 0x%08X Target Instance : %d RC=0x%08X",
                    l_attrEntry->iv_attrId, l_targ_inst, l_rc);
                break;
            }
        }
    }

    return l_rc;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
inline AttributeOverrideRc findTargetIdx(const uint8_t i_logTgtType,
                                         uint16_t & o_tgtIdx)
{
    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_TGT_TYPE_NOT_FOUND;

    o_tgtIdx = 0;

    for (uint16_t i = 0; i < g_tgts_tab_size; i++)
    {
        if (g_targetsTab[i].iv_log_target_type == i_logTgtType)
        {
            o_tgtIdx = i;
            l_rc = ATTROVERRIDE_RC_SUCCESS;
            break;
        }
    }

    return l_rc;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t applyOverride(const void *i_buffer,void *o_buffer,
                       uint32_t & o_resPackSize)
{
    #define SBE_FUNC " applyOverride "
    SBE_ENTER(SBE_FUNC);

    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_SUCCESS;

    AttributeUpdateBuffer  l_buffer(i_buffer);
    ResponseBuffer         l_resBuffer(o_buffer);
    o_resPackSize = 0;

    do
    {

        // JIRA: PFSBE-276
        // TODO : The ResponseBuffer must be fully populated. For the
        //        time being, only header is populated with number of
        //        target as zero. This must be fixed.
        HeaderEntry_t l_respHeader;
        memcpy((void *)&l_respHeader, (void *)l_buffer.getHeader(), sizeof(HeaderEntry_t));

        l_respHeader.iv_numTargets = 0;
        l_respHeader.iv_fileType = ATTROVERRIDE_RESP_FILE_TYPE;
        l_resBuffer.setHeader(&l_respHeader);

        SBE_INFO(SBE_FUNC "Number of targets:%d", l_buffer.getNumTargets());

        TargetEntry_t *l_targetEntry = nullptr;
        uint16_t l_tgtIdx = 0;

        while ((l_targetEntry=l_buffer.getNextTarget()) != nullptr)
        {
            SBE_INFO(SBE_FUNC "TargetType : 0x%02X Instance: 0x%02X  "
                "Num of attrs : %d", l_targetEntry->iv_logTgtType,
                l_targetEntry->iv_instance, l_targetEntry->iv_numAttrs);

            l_rc = findTargetIdx(l_targetEntry->iv_logTgtType, l_tgtIdx);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR("Matching target not found for the target_type : %d",
                            l_targetEntry->iv_logTgtType);
                break;
            }

            SBE_INFO("Found matching target at %d", l_tgtIdx);
            if ( (l_targetEntry->iv_instance != 0xFF) &&
                 (l_targetEntry->iv_instance >=
                    g_targetsTab[l_tgtIdx].iv_max_inst) )
            {
                SBE_ERROR("Target instance out of range. TargetType : 0x%02X"
                        " Instance : %d", l_targetEntry->iv_logTgtType,
                        l_targetEntry->iv_instance);
                l_rc = ATTROVERRIDE_RC_TARGET_INST_OUT_RANGE;
                break;
            }

            l_rc = g_targetsTab[l_tgtIdx].applyOverride(
                        l_targetEntry->iv_instance, l_buffer);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR("g_targetsTab[l_tgtIdx].applyOverride failed."
                            "TargetType : 0x%02X Instance : %d RC=0x%08X",
                            l_targetEntry->iv_logTgtType,
                            l_targetEntry->iv_instance, l_rc);
                break;
            }
        }

    } while(false);

    SBE_EXIT(SBE_FUNC);
    o_resPackSize = l_resBuffer.getOutputPackSize();
    return l_rc;
    #undef SBE_FUNC
}

} //ATTR

} //fapi2

