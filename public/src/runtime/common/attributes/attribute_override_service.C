/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_override_service.C $ */
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
#include <attribute_override.H>
#include <attribute_table.H>
#include <sbetrace.H>
#include <sbe_sp_intf.H>
#include "ppe42_string.h"
#include "assert.h"

namespace fapi2
{
namespace ATTR
{

uint32_t AttributeUpdateBuffer::getNextTarget(TargetEntry_t *&o_targetEntry)
{
    #define SBE_FUNC "getNextTarget "
    o_targetEntry=nullptr;

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        if ( (iv_currPtr + sizeof(TargetEntry_t)) > iv_bufEnd )
        {
            l_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
            SBE_ERROR(SBE_FUNC "input file terminated abruptly, while trying to parse"
                     " target section at %p, and end of input file = %p",
                     iv_currPtr, iv_bufEnd);
            break;
        }

        o_targetEntry = (TargetEntry_t *)iv_currPtr;

        if (o_targetEntry->iv_magicWord != ATTR_FILE_TARGET_MAGIC_WORD)
        {
            l_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
            SBE_ERROR(SBE_FUNC "target section magic word is invalid."
                    " LogTargetType [0x%02X], Instance [0x%02x]"
                    " Magic word [0x%08X], NumAttributes [0x%04x]",
                    o_targetEntry->iv_logTgtType, o_targetEntry->iv_instance,
                    o_targetEntry->iv_magicWord, o_targetEntry->iv_numAttrs);
            break;
        }

        iv_currPtr = iv_currPtr + sizeof(TargetEntry_t);

    } while (false);

    return l_rc;
    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t  AttributeUpdateBuffer::getNextAttr(AttrEntry_t *&o_attrEntry)
{
    #define SBE_FUNC "getNextAttr "
    o_attrEntry = nullptr;

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // See if we have enough bytes in the buffer to read attribute
        // data size
        if ( (iv_currPtr + sizeof(AttrEntry_t)) > iv_bufEnd )
        {
            l_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
            SBE_ERROR(SBE_FUNC "input file terminated abruptly, while trying to parse"
                     " attribute row header at %p, and end of input file = %p",
                     iv_currPtr, iv_bufEnd);
            break;
        }

        AttrEntry_t *l_attrEntry = (AttrEntry_t *)iv_currPtr;

        // Including data, AttrEntry_t is in multiple of 8-bytes;
        uint16_t l_paddedLength =
          uint16_t((sizeof(AttrEntry_t) + l_attrEntry->iv_dataSize) + 7) & uint16_t(~7);
        uint8_t *l_nextEntry = iv_currPtr + l_paddedLength;
        SBE_DEBUG(SBE_FUNC " iv_currPtr=%p", iv_currPtr);
        SBE_DEBUG(SBE_FUNC " sizeof(AttrEntry_t)=%d l_attrEntry->iv_dataSize=%d"
            ",paddedLength=%d",sizeof(AttrEntry_t), l_attrEntry->iv_dataSize,
            l_paddedLength);

        // See if we have enough bytes in the buffer to read attribute
        // data
        if ( l_nextEntry > iv_bufEnd )
        {
            l_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
            SBE_ERROR(SBE_FUNC "input file terminated abruptly, while trying to parse"
                     " attribute row data (size_with_pad = %d) at %p, and end of input file = %p",
                     l_paddedLength, iv_currPtr, iv_bufEnd);
            break;
        }

        o_attrEntry = (AttrEntry_t *)iv_currPtr;
        iv_currPtr = l_nextEntry;
        SBE_DEBUG(SBE_FUNC "l_nextEntry=%p", l_nextEntry);

    } while (false);
    return l_rc;
    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
ResponseBuffer::ResponseBuffer(AttributeUpdateBuffer& i_inputBuffer)
                        : iv_inputBuffer(i_inputBuffer)
{
    iv_currPtr   = (uint8_t *)i_inputBuffer.getHeader() +
                    sizeof(HeaderEntry_t);
    iv_numTargets = 0;
    iv_lastTargetPtr = nullptr;
    i_inputBuffer.setResponseBuf(this);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void ResponseBuffer::addTarget(const TargetEntry_t* i_target)
{
    #define SBE_FUNC "ResponseBuffer::addTarget "

    do
    {
        // If addition of this target results in buffer overflow
        // or overwrites input, then it indicates that some bug
        // in the code.
        //
        if ( (iv_currPtr+sizeof(TargetEntry_t) >
                iv_inputBuffer.getBufferEnd()) ||
             (iv_currPtr+sizeof(TargetEntry_t) >
                iv_inputBuffer.getCurrentPtr()) )
        {
            SBE_ERROR(SBE_FUNC "Trying to write to response to buffer before reading the"
                    " input. iv_currPtr [%p], iv_inputBuffer.getBufferEnd() [%p]"
                    " iv_inputBuffer.getCurrentPtr() [%p]",
                    iv_currPtr,
                    iv_inputBuffer.getBufferEnd(),
                    iv_inputBuffer.getCurrentPtr());
            // This error will happen,
            //      1. if there is a code bug
            //      2. stack corruption.
            // we cannot continue nor chipop caller cannot solve this issue by changing the input.
            // So halting the SBE here.
            assert(false);
        }

        // Number of target sections in the response buffer
        iv_numTargets += 1;

        memcpy(iv_currPtr, (uint8_t *)i_target, sizeof(TargetEntry_t));
        iv_lastTargetPtr = (TargetEntry_t *)iv_currPtr;
        iv_currPtr += sizeof(TargetEntry_t);

    } while (false);

    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t ResponseBuffer::addTarget(const TargetEntry_t* i_target,
                                   const AttributeOverrideRc i_rc)
{
    #define SBE_FUNC "ResponseBuffer::addTarget "
    uint32_t l_rc = ATTROVERRIDE_RC_SUCCESS;

    do
    {
        // This must be called either in case of target type not
        // found or instance number is not valid. On both these
        // cases, RC will not be ATTROVERRIDE_RC_SUCCESS
        // There are no other valid use-case for calling this function.
        if ( i_rc != ATTROVERRIDE_RC_SUCCESS )
        {

            this->addTarget(i_target);

            // Copy all the attribute of this target to the output
            // buffer and update the RC.

            AttrEntry_t *l_attrEntry = nullptr;
            AttrRespEntry_t l_attrRespEntry = {0};

            uint16_t l_numAttrs = i_target->iv_numAttrs;
            for (uint16_t l_idx=0; l_idx < l_numAttrs; l_idx++)
            {
                l_rc = iv_inputBuffer.getNextAttr(l_attrEntry);
                if ( l_rc != SBE_SEC_OPERATION_SUCCESSFUL )
                {
                    SBE_ERROR(SBE_FUNC "getNextTarget(): failed for"
                    " LogTargetType [0x%02X], Instance [0x%04x]"
                    " l_idx [0x%04X], l_rc [%d]",
                    i_target->iv_logTgtType, i_target->iv_instance,
                    l_idx, l_rc);
                    break;
                }

                if ( (iv_currPtr+sizeof(AttrRespEntry_t) >
                        iv_inputBuffer.getBufferEnd()) ||
                     (iv_currPtr+sizeof(AttrRespEntry_t) >
                        iv_inputBuffer.getCurrentPtr()) )
                {
                    SBE_ERROR(SBE_FUNC "Trying to write to response to buffer before reading the"
                            " input. iv_currPtr [%p], iv_inputBuffer.getBufferEnd() [%p]"
                            " iv_inputBuffer.getCurrentPtr() [%p]",
                            iv_currPtr,
                            iv_inputBuffer.getBufferEnd(),
                            iv_inputBuffer.getCurrentPtr());
                    // This error will happen,
                    //      1. if there is a code bug
                    //      2. stack corruption.
                    // we cannot continue nor chipop caller cannot solve this issue by changing the
                    //   input. So halting the SBE here.
                    assert(false);
                }
                l_attrRespEntry.iv_attrId = l_attrEntry->iv_attrId;
                l_attrRespEntry.iv_rc     = i_rc;
                memcpy(iv_currPtr,&l_attrRespEntry,sizeof(AttrRespEntry_t));

                iv_currPtr += sizeof(AttrRespEntry_t);
            }
        }
    } while (false);

    return l_rc;
    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
void ResponseBuffer::addAttr(const TargetEntry_t* i_target,
                                 const AttrEntry_t*  i_attrEntry,
                                 const AttributeOverrideRc i_rc)
{
    #define SBE_FUNC "ResponseBuffer::addAttr "

    do
    {
        // If the target type and/or the instance does not match, then add a
        // new target section in the reponse buffer.
        if ( (iv_lastTargetPtr == nullptr) ||
             (iv_lastTargetPtr->iv_logTgtType != i_target->iv_logTgtType) ||
             (iv_lastTargetPtr->iv_instance   != i_target->iv_instance) )
        {
            this->addTarget(i_target);

            // Set the number of attributes of the newly added target
            // to zero as the number of attributes that will be added
            // is not known at this time. Number of attributes will be
            // incremented as and when an attribute is added.
            iv_lastTargetPtr->iv_numAttrs = 0;
        }

        AttrRespEntry_t l_attrRespEntry = {0};
        // If addition of this target results in buffer overflow
        // or overwrites input, then it indicates that some bug
        // in the code.
        SBE_DEBUG("iv_currPtr+sizeof(AttrRespEntry_t)=%p",
                iv_currPtr+sizeof(AttrRespEntry_t));
        SBE_DEBUG("iv_inputBuffer.getBufferEnd()=%p",
                iv_inputBuffer.getBufferEnd());
        SBE_DEBUG("tgtType 0x%08x, AttrId 0x%08x",
                  i_target->iv_logTgtType, i_attrEntry->iv_attrId);
        SBE_DEBUG("iv_inputBuffer.getCurrentPtr()=%p",
                iv_inputBuffer.getCurrentPtr());
        if ( (iv_currPtr+sizeof(AttrRespEntry_t) >
                iv_inputBuffer.getBufferEnd()) ||
             (iv_currPtr+sizeof(AttrRespEntry_t) >
                iv_inputBuffer.getCurrentPtr()) )
        {
            SBE_ERROR(SBE_FUNC "Trying to write response to buffer before reading the"
                    " input. iv_currPtr [%p], iv_inputBuffer.getBufferEnd() [%p]"
                    " iv_inputBuffer.getCurrentPtr() [%p]",
                    iv_currPtr,
                    iv_inputBuffer.getBufferEnd(),
                    iv_inputBuffer.getCurrentPtr());
            // This error will happen,
            //      1. if there is a code bug
            //      2. stack corruption.
            // we cannot continue nor chipop caller cannot solve this issue by changing the input.
            // So halting the SBE here.
            assert(false);
        }

        // Add the attribute to the response buffer with the RC.
        l_attrRespEntry.iv_attrId = i_attrEntry->iv_attrId;
        l_attrRespEntry.iv_rc     = i_rc;
        memcpy(iv_currPtr,&l_attrRespEntry,sizeof(AttrRespEntry_t));

        iv_currPtr += sizeof(AttrRespEntry_t);

        // Increment the number of attributes for this target + instance
        iv_lastTargetPtr->iv_numAttrs += 1;

    } while (false);

    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
AttributeOverrideRc AttributesTable::applyOverride(
                    const uint8_t       i_tgt_inst,
                    const AttrEntry_t*  i_attrEntry)
{
    #define SBE_FUNC "AttributesTable::applyOverride "

    SBE_DEBUG("iv_max_row=%d, iv_max_col=%d, iv_max_hgt=%d",
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

    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_SUCCESS;

    do
    {
        // Data is present at the end of AttrEntry structure
        uint8_t* l_data = (uint8_t *)i_attrEntry + sizeof(AttrEntry_t);

        // iv_size provides the size of an element in bytes.
        // So, for pointer arithmatic, type cast iv_ptr to uint8_t pointer
        // and then add iv_size times the index to get the actual location.
        if ( (iv_max_row == 0) && (iv_max_col == 0) && (iv_max_hgt == 0) )
        {
            // As per the attribute table, the attribute is a normal attribute.
            // The request is also to update a normal attribute.
            if ((i_attrEntry->iv_row == 0xFF ) &&
                (i_attrEntry->iv_col == 0xFF ) &&
                (i_attrEntry->iv_hgt == 0xFF ))
            {
                if ( i_attrEntry->iv_dataSize != iv_size )
                {
                    l_rc = ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
                    // the target instance can be printed along with target
                    //   in the caller function, for better trace readability.
                    SBE_ERROR(SBE_FUNC "Size is not matching for attribute 0x%08x."
                               " Expected:%d Actual:%d ",
                               i_attrEntry->iv_attrId,
                               iv_size, i_attrEntry->iv_dataSize);
                    break;
                }
                memcpy(
                    (reinterpret_cast<uint8_t *>(iv_ptr))+(iv_size*i_tgt_inst),
                            l_data, i_attrEntry->iv_dataSize);
            }
            else
            {
                // As per the attribute table, the attribute is a normal
                // attribute. The request is to update an array attribute.
                l_rc = ATTROVERRIDE_RC_ATTR_NOT_ARRAY;
                SBE_ERROR(SBE_FUNC "Indices passed for the attribute is not valid for scalar"
                        " attribute. AttributeId=0x%08X, iv_row=0x%02X,"
                        " iv_col=0x%02X, iv_hgt=0x%02X",
                        i_attrEntry->iv_attrId,
                        i_attrEntry->iv_row,
                        i_attrEntry->iv_col,
                        i_attrEntry->iv_hgt);
                break;
            }
        }
        else
        {
            uint32_t index = 0;

            // The request is to update all the elements of the array
            if ( (i_attrEntry->iv_row == 0xFF) &&
                 (i_attrEntry->iv_col == 0xFF) &&
                 (i_attrEntry->iv_hgt == 0xFF) )
            {
                uint32_t arraySize = (iv_max_row * iv_max_col *
                                       iv_max_hgt * iv_size);

                if ( i_attrEntry->iv_dataSize != arraySize )
                {
                    l_rc = ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
                    SBE_ERROR(SBE_FUNC "AttributeId=0x%08X.Data size is not matching."
                            "Expected:%d Actual:%d",
                            i_attrEntry->iv_attrId, arraySize,
                            i_attrEntry->iv_dataSize);
                    break;
                }
                index = (i_tgt_inst * arraySize);
            }
            else if ( (i_attrEntry->iv_row >= iv_max_row) ||
                      (i_attrEntry->iv_col >= iv_max_col) ||
                      (i_attrEntry->iv_hgt >= iv_max_hgt) )
            {
                l_rc = ATTROVERRIDE_RC_ARRAY_INDEX_OUT_RANGE;
                SBE_ERROR(SBE_FUNC "Attribute array index out of range."
                          "AttributeId=0x%08X,Row=%d,Col=%d,Hgt=%d",
                        i_attrEntry->iv_attrId, i_attrEntry->iv_row,
                        i_attrEntry->iv_col, i_attrEntry->iv_hgt);
                SBE_ERROR(SBE_FUNC "Index limit is,"
                          "Row=%d,Col=%d,Hgt=%d",
                        iv_max_row,
                        iv_max_col,
                        iv_max_hgt);
                break;
            }
            else
            {
                // The request is to update an element of an array type
                // attribute.
                if ( i_attrEntry->iv_dataSize != iv_size )
                {
                    l_rc = ATTROVERRIDE_RC_SIZE_NOT_MATCHING;
                    SBE_ERROR("The size is not matching. Expected:%d Actual:%d"
                              "AttributeId=0x%08x",
                            iv_size, i_attrEntry->iv_dataSize,
                            i_attrEntry->iv_attrId);
                    break;
                }
                index = (((i_tgt_inst * iv_max_row + i_attrEntry->iv_row) *
                           iv_max_col + i_attrEntry->iv_col) *
                           iv_max_hgt + i_attrEntry->iv_hgt) * iv_size;
            }
            memcpy((reinterpret_cast<uint8_t *>(iv_ptr))+index, l_data,
                    i_attrEntry->iv_dataSize);
        }
    } while (false);

    return l_rc;
    #undef SBE_FUNC
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

uint32_t TargetsTable::applyOverride(
                    const TargetEntry_t      *i_targetEntry,
                    AttributeUpdateBuffer&    i_buffer)
{
    #define SBE_FUNC "TargetsTable::applyOverride "
    SBE_INFO(SBE_FUNC "iv_max_inst=%d, iv_attr_row_start=%d, iv_attr_row_size=%d",
                iv_max_inst, iv_attr_row_start, iv_attr_row_size);

    uint32_t l_rc = ATTROVERRIDE_RC_SUCCESS;

    AttrEntry_t *l_attrEntry = nullptr;
    uint32_t     l_attrIdx  = 0;
    // iv_instance has been validated by the caller,
    //   so, here its not needed.
    uint8_t l_targ_inst_st  = (i_targetEntry->iv_instance == 0xFF) ? 0 :
                               i_targetEntry->iv_instance;
    uint8_t l_targ_inst_end = (i_targetEntry->iv_instance == 0xFF) ?
                        this->iv_max_inst : i_targetEntry->iv_instance + 1;
    SBE_DEBUG("l_targ_inst_start=%d, l_targ_inst_end=%d",
                        l_targ_inst_st, l_targ_inst_end);

    uint16_t l_numAttrs = i_targetEntry->iv_numAttrs;
    for (uint16_t l_idx=0; l_idx < l_numAttrs; l_idx++)
    {
        l_rc = i_buffer.getNextAttr(l_attrEntry);
        if ( l_rc != ATTROVERRIDE_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC "getNextAttr() failed for"
                    "tgtType : 0x%02X, instance =0x%08x",
                    i_targetEntry->iv_logTgtType,
                    i_targetEntry->iv_instance);
            break;
        }

        SBE_DEBUG("AttributeId=0x%08X", l_attrEntry->iv_attrId);
        l_rc = findAttrIdx(l_attrEntry->iv_attrId, this->iv_attr_row_start,
                        this->iv_attr_row_size, l_attrIdx);
        if (l_rc != ATTROVERRIDE_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "findAttrIdx() failed for target_type=0x%02x instance=%d"
                        " i_attr_id=0x%08X, l_rc = [%d]",
                        i_targetEntry->iv_logTgtType,
                        l_idx,
                        l_attrEntry->iv_attrId,
                        l_rc);

            i_buffer.getResponseBuf().addAttr(i_targetEntry,
                                                    l_attrEntry,
                                        (AttributeOverrideRc)l_rc);
            l_rc = ATTROVERRIDE_RC_SUCCESS;

            // For the current attribute, an attribute entry has been created
            // in the response buffer to indicate an error and proceed to the
            // next attribute.
            continue;
        }

        for (uint8_t l_targ_inst=l_targ_inst_st;
                    l_targ_inst < l_targ_inst_end; l_targ_inst++)
        {
            // TODO: this can return a formate error (secondary rc) or
            //  attribute rc. We should break if its secondary rc.
            //  For this a major rework is required. return value for
            //  all functions should be secondary rc, and attribute rc can
            //  be an output parameter.
            l_rc = g_attrsTab[l_attrIdx].applyOverride(l_targ_inst,
                                                        l_attrEntry);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "g_attrsTab[l_attrIdx].applyOverride failed."
                    "AttributeId : 0x%08X Target Instance : %d RC=0x%08X",
                    l_attrEntry->iv_attrId, l_targ_inst, l_rc);

                i_buffer.getResponseBuf().addAttr(i_targetEntry,
                                                l_attrEntry,
                                                (AttributeOverrideRc)l_rc);
                l_rc = ATTROVERRIDE_RC_SUCCESS;

                // For the current attribute, an attribute entry has been
                // created in the response buffer to indicate an error and
                // proceed to the next attribute.
                continue;
            }
        } // for targ_inst_st to targ_inst_end
        if(l_rc != ATTROVERRIDE_RC_SUCCESS)
        {
            break;
        }
    } // for 0 to N attributes

    return l_rc;
    #undef SBE_FUNC
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
uint32_t applyOverride(const void *io_buffer, const uint32_t i_bufSize,
                       uint32_t & o_resPackSize)
{
    #define SBE_FUNC " applyOverride "
    SBE_ENTER(SBE_FUNC);

    // Initialize the output variable
    o_resPackSize = 0;

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // The input buffer should have minimum of a header. If the
        // buffer size is less than the header size, then the format
        // is invalid.
        if ( i_bufSize < sizeof(HeaderEntry_t) )
        {
            l_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
            SBE_ERROR(SBE_FUNC "The input buffer size [%d] is less than the "
             "expected minimum size [%d], l_rc=[%d]",
              i_bufSize, sizeof(HeaderEntry_t), l_rc);
            break;
        }

        AttributeUpdateBuffer  l_updAttrBuf(io_buffer,i_bufSize);
        if ( (!l_updAttrBuf.majorVerSupported()) ||
             (!l_updAttrBuf.minorVerSupported()) )
        {
            l_rc = SBE_SEC_ATTR_UPD_VER_NOT_SUPPORTED;
            SBE_ERROR(SBE_FUNC "Major [%d] Minor [%d] version not supported",
                      l_updAttrBuf.majorVersion(), l_updAttrBuf.minorVersion());
            SBE_ERROR("Supported Major[%d] Minor[%d]",
                    fapi2::ATTR::ATTROVERRIDE_MAJOR_VERSION,
                    fapi2::ATTR::ATTROVERRIDE_MINOR_VERSION);
            break;
        }

        if (!l_updAttrBuf.validFileType())
        {
            l_rc = SBE_SEC_ATTR_UPD_INVALID_FILE_TYPE;
            SBE_ERROR(SBE_FUNC "Invalid file type [%d] Expected file type [%d]"
                      "l_rc =[%d]",
                      l_updAttrBuf.fileType(),
                      fapi2::ATTR::ATTROVERRIDE_REQ_FILE_TYPE,
                      l_rc);
            break;
        }

        ResponseBuffer l_response(l_updAttrBuf);
        TargetEntry_t *l_targetEntry = nullptr;
        uint16_t l_tgtIdx = 0;

        uint32_t  l_numTargets = l_updAttrBuf.getNumTargets();

        SBE_INFO(SBE_FUNC "Number of targets:%d", l_numTargets);

        for (uint32_t l_idx=0; l_idx < l_numTargets; l_idx++)
        {
            l_rc = l_updAttrBuf.getNextTarget(l_targetEntry);
            if ( l_rc != SBE_SEC_OPERATION_SUCCESSFUL )
            {
                break;
            }

            SBE_INFO(SBE_FUNC " TargetType : 0x%02X Instance: 0x%02X  "
                "Num of attrs : %d", l_targetEntry->iv_logTgtType,
                l_targetEntry->iv_instance, l_targetEntry->iv_numAttrs);

            l_rc = findTargetIdx(l_targetEntry->iv_logTgtType, l_tgtIdx);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " Matching target not found for the"
                           " target_type : %d", l_targetEntry->iv_logTgtType);

                l_rc = l_response.addTarget(l_targetEntry,(AttributeOverrideRc)l_rc);
                if ( l_rc != ATTROVERRIDE_RC_SUCCESS )
                {
                    SBE_ERROR(SBE_FUNC " l_response.addTarget() failed for the "
                              "tgtType : 0x%02X, instance =0x%08x, l_rc =[%d]",
                                l_targetEntry->iv_logTgtType,
                                l_targetEntry->iv_instance, l_rc);
                    break;
                }
                // l_response.addTarget() will skip all the remaining attributes
                // for the target. Proceed to the next target
                continue;
            }

            SBE_INFO("Found matching target at %d", l_tgtIdx);
            if ( (l_targetEntry->iv_instance != 0xFF) &&
                 (l_targetEntry->iv_instance >=
                    g_targetsTab[l_tgtIdx].iv_max_inst) )
            {
                SBE_ERROR(SBE_FUNC " Target instance out of range. TargetType : "
                        "0x%02X Instance : %d", l_targetEntry->iv_logTgtType,
                        l_targetEntry->iv_instance);

                l_rc = l_response.addTarget(l_targetEntry,
                                    ATTROVERRIDE_RC_TARGET_INST_OUT_RANGE);
                if ( l_rc != ATTROVERRIDE_RC_SUCCESS )
                {
                    SBE_ERROR(SBE_FUNC " l_response.addTarget() failed for the "
                              "tgtType : 0x%02X, instance =0x%08x, l_rc =[%d]",
                                l_targetEntry->iv_logTgtType,
                                l_targetEntry->iv_instance, l_rc);
                    break;
                }
                // l_response.addTarget() will skip all the remaining attributes
                // for the target. Proceed to the next target
                continue;
            }

            l_rc = g_targetsTab[l_tgtIdx].applyOverride(
                                            l_targetEntry, l_updAttrBuf);
            if (l_rc != ATTROVERRIDE_RC_SUCCESS)
            {
                // detailed response built into the target table
                // applyOverride so not needed to handle it here
                SBE_ERROR(SBE_FUNC" g_targetsTab[l_tgtIdx].applyOverride failed"
                            " TargetType : 0x%02X Instance : %d RC=0x%08X",
                            l_targetEntry->iv_logTgtType,
                            l_targetEntry->iv_instance, l_rc);
            }
        }

        l_response.updateHeader();
        o_resPackSize = l_response.getOutputPackSize();

    } while(false);

    SBE_INFO("Response packet size in bytes : %d", o_resPackSize);

    SBE_EXIT(SBE_FUNC);
    return l_rc;

    #undef SBE_FUNC
}

} //ATTR

} //fapi2

