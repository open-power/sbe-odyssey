/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_override_service.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2025                        */
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
#include <securityutils.H>
#include "ppe42_string.h"
#include "assert.h"

namespace fapi2
{
namespace ATTR
{

uint32_t AttrUpdateDataStream::readHeader(HeaderEntry_t *o_header)
{
    static_assert((sizeof(HeaderEntry_t) % 4) == 0, "The buffer passed to readHeader() is not in multiples of 4 bytes");

    return iv_stream->get(sizeof(HeaderEntry_t)/sizeof(uint32_t), (uint32_t *)o_header, false);
}

uint32_t AttrUpdateDataStream::getNextTarget(TargetEntry_t *o_targetEntry)
{
    static_assert((sizeof(TargetEntry_t) % 4) == 0, "The buffer passed to getNextTarget() is not in multiples of 4 bytes");

    return iv_stream->get(sizeof(TargetEntry_t)/sizeof(uint32_t), (uint32_t *)o_targetEntry, false);
}

uint32_t  AttrUpdateDataStream::getNextAttrInfo(AttrInfo_t *o_attrInfo)
{
    static_assert((sizeof(AttrInfo_t) % 4) == 0, "The buffer passed to getNextAttrInfo() is not in multiples of 4 bytes");

    return iv_stream->get(sizeof(AttrInfo_t)/sizeof(uint32_t), (uint32_t *)o_attrInfo, false);
}

uint32_t  AttrUpdateDataStream::getNextAttrData(uint8_t *o_buffer, const uint32_t i_bufSize)
{
    return iv_stream->get(i_bufSize/sizeof(uint32_t), (uint32_t *)o_buffer, false);
}

uint32_t  AttrUpdateDataStream::readAndDiscard(const uint16_t i_bytesToDiscard)
{
    uint8_t l_data[i_bytesToDiscard];
    return iv_stream->get(i_bytesToDiscard/sizeof(uint32_t), (uint32_t *)l_data, false);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
AttrUpdateResBuffer::AttrUpdateResBuffer(const uint8_t i_chipType,  uint8_t *i_bufPtr, const uint32_t i_bufSize)
                        : iv_bufPtr(i_bufPtr), iv_bufSize(i_bufSize), iv_lastTargetPtr(nullptr), iv_numTargets(0)
{
    HeaderEntry_t l_header;
    l_header.iv_fmtMajor = ATTROVERRIDE_MAJOR_VERSION;
    l_header.iv_fmtMinor = ATTROVERRIDE_MINOR_VERSION;
    l_header.iv_chipType = i_chipType;
    l_header.iv_fileType = ATTROVERRIDE_RESP_FILE_TYPE;
    l_header.iv_numTargets = 0;

    memset(iv_bufPtr,0,i_bufSize);
    memcpy(iv_bufPtr, &l_header, sizeof(l_header));

    iv_currPtr = iv_bufPtr + sizeof(HeaderEntry_t);
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t AttrUpdateResBuffer::addTarget(const TargetEntry_t* i_targetEntry)
{
    #define SBE_FUNC "AttrUpdateResBuffer::addTarget "

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // If addition of this target results in buffer overflow,
        // then return error
        if ( (iv_currPtr+sizeof(TargetEntry_t) > (iv_bufPtr+iv_bufSize)) )
        {
            SBE_ERROR(SBE_FUNC "Buffer overflow.");
            l_rc = SBE_SEC_ATTR_UPD_OUT_BUFF_OVERFLOW;
            break;
        }
        // Increment the number of target sections in the response buffer
        iv_numTargets += 1;

        memcpy(iv_currPtr, (uint8_t *)i_targetEntry, sizeof(TargetEntry_t));
        iv_lastTargetPtr = (TargetEntry_t *)iv_currPtr;
        iv_currPtr += sizeof(TargetEntry_t);

        // Set the number of attributes of the newly added target
        // to zero as the number of attributes that will be added
        // is not known at this time. Number of attributes will be
        // incremented as and when an attribute is added.
        iv_lastTargetPtr->iv_numAttrs = 0;

    } while (false);

    return l_rc;
    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t AttrUpdateResBuffer::addAttribute(const AttrRespEntry_t &i_attrRespEntry)
{
    #define SBE_FUNC "AttrUpdateResBuffer::addAttribute"

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // If addition of this attribute results in buffer overflow
        // then return error
        if ( (iv_currPtr+sizeof(i_attrRespEntry)) > (iv_bufPtr+iv_bufSize) )
        {
            SBE_ERROR(SBE_FUNC "Buffer overflow.");
            l_rc = SBE_SEC_ATTR_UPD_OUT_BUFF_OVERFLOW;
            break;
        }

        memcpy(iv_currPtr, (uint8_t *)&i_attrRespEntry, sizeof(AttrRespEntry_t));
        iv_currPtr += sizeof(AttrRespEntry_t);

        // Increment the number of attributes for this target and instance
        iv_lastTargetPtr->iv_numAttrs += 1;

    } while (false);

    return l_rc;
    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t AttrUpdateResBuffer::addAttribute(const TargetEntry_t *i_targetEntry,
                                           const AttrEntry_t  *i_attrEntry,
                                           const AttributeOverrideRc i_overrideRc)
{
    #define SBE_FUNC "AttrUpdateResBuffer::addAttribute "

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // If the target type and/or the instance does not match, then add a
        // new target section in the reponse buffer.
        if ( (iv_lastTargetPtr == nullptr) ||
             (iv_lastTargetPtr->iv_logTgtType != i_targetEntry->iv_logTgtType) ||
             (iv_lastTargetPtr->iv_instance   != i_targetEntry->iv_instance) )
        {
            l_rc = this->addTarget(i_targetEntry);
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                break;
            }
        }

        AttrRespEntry_t l_attrRespEntry;

        l_attrRespEntry.iv_attrId = i_attrEntry->iv_attrId;
        l_attrRespEntry.iv_rc     = i_overrideRc;

        l_rc = this->addAttribute(l_attrRespEntry);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            break;
        }


    } while (false);

    return l_rc;
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
        if (iv_deny_update == true)
        {
            if (g_pSbeSecurityUtils->getAllowAttrOverrideCheckLvl() == SOFT_SECURITY_CHECK_ENABLED)
            {
                // denyForSecurityUpdate tag has been specified for this attribute.
                // Hence, this attribute can be updated only if the soft security level is
                // permissive enabled ie. ATTR_SECURITY_LEVEL is PERMISSIVE and
                //                        Scratch 11 Bit 7 is set
                //
                l_rc = ATTROVERRIDE_RC_OVRD_NOT_ALLOWED;

                SBE_INFO(SBE_FUNC "Attribute update is not allowed for the attribute Id : 0x%08X",
                            i_attrEntry->iv_attrId);
                // Dont update the attribute
                break;
            }
            else if (g_pSbeSecurityUtils->getAllowAttrOverrideCheckLvl() ==
                                SOFT_SECURITY_CHECK_PERMISSIVE_ENABLED)
            {
                // Though denyForSecurityUpdate tag has been specified for the attribute, as the soft security
                // is permissive and enabled, it is being updated. In such case, return a different RC.
                l_rc = ATTROVERRIDE_RC_DENIED_UPDATE;
                SBE_INFO(SBE_FUNC "Attribute Id : 0x%08X has been updated as soft security is permissive enabled.",
                            i_attrEntry->iv_attrId);
                // Proceed to update the attribute
            }
        }

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
                    // in the caller function, for better trace readability.
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

uint32_t TargetsTable::applyOverride(
                    const TargetEntry_t*  i_targetEntry,
                    AttrUpdateHelper &    i_attrUpdHelper,
                    uint32_t & o_rc)
{
    #define SBE_FUNC "TargetsTable::applyOverride "
    SBE_INFO(SBE_FUNC "iv_max_inst=%d, iv_attr_row_start=%d, iv_attr_row_size=%d",
                iv_max_inst, iv_attr_row_start, iv_attr_row_size);

    o_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    uint32_t l_fifoRc=SBE_SEC_OPERATION_SUCCESSFUL;

    // iv_instance has been validated by the caller,
    // so, no need to validate here.
    uint8_t l_targ_inst_st  = (i_targetEntry->iv_instance == 0xFF) ? 0 :
                               i_targetEntry->iv_instance;
    uint8_t l_targ_inst_end = (i_targetEntry->iv_instance == 0xFF) ?
                        this->iv_max_inst : i_targetEntry->iv_instance + 1;
    SBE_DEBUG("l_targ_inst_start=%d, l_targ_inst_end=%d",
                        l_targ_inst_st, l_targ_inst_end);

    uint16_t l_numAttrs = i_targetEntry->iv_numAttrs;
    for (uint16_t l_idx=0; l_idx < l_numAttrs; l_idx++)
    {
        AttrInfo_t l_attrInfo;
        l_fifoRc = i_attrUpdHelper.getNextAttrInfo(&l_attrInfo);
        if ( l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC "getNextAttrInfo() failed for"
                    "tgtType : 0x%02X, instance =0x%08x",
                    i_targetEntry->iv_logTgtType,
                    i_targetEntry->iv_instance);
            break;
        }

        SBE_DEBUG("AttributeId=0x%08X", l_attrInfo.iv_attrId);

        // Including data, AttrEntry_t is in multiple of 8-bytes;
        uint16_t l_paddedLength = l_attrInfo.paddedLength();
        uint8_t  l_attrEntryWithData[l_paddedLength];

        memset(l_attrEntryWithData,0,l_paddedLength);
        memcpy(l_attrEntryWithData,&l_attrInfo,sizeof(AttrInfo_t));

        l_fifoRc = i_attrUpdHelper.getNextAttrData(l_attrEntryWithData+sizeof(AttrInfo_t),
                                                        (l_paddedLength - sizeof(AttrInfo_t)));
        if ( l_fifoRc != ATTROVERRIDE_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC "getNextAttrData() failed for"
                    "tgtType : 0x%02X, instance =0x%08x",
                    i_targetEntry->iv_logTgtType,
                    i_targetEntry->iv_instance);
            break;
        }

        uint32_t l_attrIdx  = 0;
        AttributeOverrideRc l_overrideRc = l_attrInfo.getIndex(this->iv_attr_row_start,
                                                               this->iv_attr_row_size,
                                                               l_attrIdx);
        if (l_overrideRc != ATTROVERRIDE_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "getIndex() failed for target_type=0x%02x "
                        " i_attr_id=0x%08X, overrideRc = [%d]",
                        i_targetEntry->iv_logTgtType,
                        l_attrInfo.iv_attrId, l_overrideRc);

            o_rc = i_attrUpdHelper.addAttribute(i_targetEntry,
                                                (AttrEntry_t *)l_attrEntryWithData,
                                                l_overrideRc);
            if (o_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                break;
            }
            // For the current attribute, an attribute entry has been created
            // in the response buffer to indicate an error. Now, proceed to
            // the next attribute.
            continue;
        }

        for (uint8_t l_targ_inst=l_targ_inst_st;
                    l_targ_inst < l_targ_inst_end; l_targ_inst++)
        {
            AttributeOverrideRc l_overrideRc = g_attrsTab[l_attrIdx].applyOverride(l_targ_inst,
                                                                        (AttrEntry_t *)l_attrEntryWithData);
            if (l_overrideRc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "g_attrsTab[l_attrIdx].applyOverride failed."
                    "AttributeId : 0x%08X Target Instance : %d RC=0x%08X",
                    l_attrInfo.iv_attrId, l_targ_inst, l_overrideRc);

                o_rc = i_attrUpdHelper.addAttribute(i_targetEntry,
                                                    (AttrEntry_t *)l_attrEntryWithData,
                                                    l_overrideRc);

                if (o_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    break;
                }
            }
        } // for targ_inst_st to targ_inst_end
        if(o_rc != ATTROVERRIDE_RC_SUCCESS)
        {
            break;
        }
    } // for 0 to N attributes

    return l_fifoRc;
    #undef SBE_FUNC
}

uint32_t AttrUpdateHelper::handleTargetError(const TargetEntry_t & i_targetEntry,
                                             const AttributeOverrideRc i_overrideRc,
                                             uint32_t & o_rc)
{
    #define SBE_FUNC " handleTargetError"
    SBE_ENTER(SBE_FUNC);

    o_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        o_rc = iv_outBuffer.addTarget(&i_targetEntry);
        if ( o_rc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            //The given target is not found in the target table.
            //Adding that information in the response buffer
            //failed. So, stop further processing and return
            //error to the caller.
            SBE_ERROR(SBE_FUNC " l_response.addTarget() failed for the "
                      "tgtType : 0x%02X, instance =0x%08x, l_rc =[%d]",
                        i_targetEntry.iv_logTgtType,
                        i_targetEntry.iv_instance, o_rc);
            break;
        }

        for (uint16_t l_idx=0; l_idx < i_targetEntry.iv_numAttrs; l_idx++)
        {
            // Get the attribute-id and the size from the SBE FIFO
            AttrInfo_t l_attrInfo;
            l_fifoRc = iv_inStream.getNextAttrInfo(&l_attrInfo);
            if ( l_fifoRc  != SBE_SEC_OPERATION_SUCCESSFUL )
            {
                break;
            }

            // Add the attribute-id and the RC to the response buffer
            AttrRespEntry_t l_attrRespEntry = {0};
            l_attrRespEntry.iv_attrId   = l_attrInfo.iv_attrId;
            l_attrRespEntry.iv_rc       = i_overrideRc;

            o_rc = iv_outBuffer.addAttribute(l_attrRespEntry);
            if (o_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                break;
            }

            uint16_t l_bytesToDiscard = l_attrInfo.paddedLength() - sizeof(AttrInfo_t);
            l_fifoRc = iv_inStream.readAndDiscard(l_bytesToDiscard);
            if ( l_fifoRc  != SBE_SEC_OPERATION_SUCCESSFUL )
            {
                break;
            }
        }
    } while (false);

    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;

    #undef SBE_FUNC
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t applyOverride(sbefifo_hwp_data_istream &i_stream,
                       void *i_buffer,
                       const uint32_t i_bufSize,
                       uint32_t & o_resPackSize,
                       uint32_t & o_rc)
{
    #define SBE_FUNC " applyOverride "
    SBE_ENTER(SBE_FUNC);

    AttrUpdateDataStream l_updateDataStream(&i_stream);

    // Initialize the output variable
    o_resPackSize = 0;
    o_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    memset(i_buffer,0,i_bufSize);

    uint32_t l_fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {

        HeaderEntry_t l_header;

        l_fifoRc = l_updateDataStream.readHeader(&l_header);
        if ( l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC "readHeader returned error. l_fifoRc=[0x%08X]", l_fifoRc);
            break;
        }

        o_rc = l_header.verifyForUpdateRequest();
        if ( o_rc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC "verifyForUpdateRequest failed");
            break;
        }

        // The buffer must be able to accomodate atleast the header
        if (i_bufSize < sizeof(HeaderEntry_t))
        {
            o_rc = SBE_SEC_ATTR_UPD_OUT_BUFF_OVERFLOW;
            SBE_ERROR(SBE_FUNC "Output buffer size [%d] < minimum required [%d].",
                        i_bufSize, sizeof(HeaderEntry_t));
            break;
        }

        AttrUpdateResBuffer l_resBuffer(l_header.iv_chipType, (uint8_t *)i_buffer, i_bufSize);
        AttrUpdateHelper    l_helper(l_updateDataStream, l_resBuffer);

        SBE_INFO(SBE_FUNC "Number of targets:%d", l_header.iv_numTargets);

        for (uint32_t l_idx=0; l_idx < l_header.iv_numTargets; l_idx++)
        {
            TargetEntry_t   l_targetEntry;
            l_fifoRc = l_updateDataStream.getNextTarget(&l_targetEntry);
            if ( l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL )
            {
                break;
            }

            if (!l_targetEntry.isValid())
            {
                o_rc = SBE_SEC_ATTR_UPD_FORMAT_ERROR;
                SBE_ERROR(SBE_FUNC "target section magic word is invalid."
                        " LogTargetType [0x%02X], Instance [0x%02x]"
                        " Magic word [0x%08X], NumAttributes [0x%04x]",
                        l_targetEntry.iv_logTgtType, l_targetEntry.iv_instance,
                        l_targetEntry.iv_magicWord, l_targetEntry.iv_numAttrs);
                break;
            }

            SBE_INFO(SBE_FUNC " TargetType : 0x%02X Instance: 0x%02X  "
                "Num of attrs : %d", l_targetEntry.iv_logTgtType,
                l_targetEntry.iv_instance, l_targetEntry.iv_numAttrs);

            uint16_t l_tgtIdx = 0;
            AttributeOverrideRc l_overrideRc = l_targetEntry.getIndex(l_tgtIdx);
            if (l_overrideRc != ATTROVERRIDE_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC " Matching target not found for the"
                           " target_type : %d", l_targetEntry.iv_logTgtType);

                l_fifoRc = l_helper.handleTargetError(l_targetEntry, l_overrideRc, o_rc);
                if ( (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL) ||
                     (o_rc     != SBE_SEC_OPERATION_SUCCESSFUL) )
                {
                    break;
                }

                // Proceed with the next target
                continue;
            }

            SBE_INFO("Found matching target at %d", l_tgtIdx);
            if ( (l_targetEntry.iv_instance != 0xFF) &&
                 (l_targetEntry.iv_instance >=
                    g_targetsTab[l_tgtIdx].iv_max_inst) )
            {
                SBE_ERROR(SBE_FUNC " Target instance out of range. TargetType : "
                        "0x%02X Instance : %d", l_targetEntry.iv_logTgtType,
                        l_targetEntry.iv_instance);

                l_fifoRc = l_helper.handleTargetError(l_targetEntry,
                                    ATTROVERRIDE_RC_TARGET_INST_OUT_RANGE, o_rc);
                if ( (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL) ||
                     (o_rc     != SBE_SEC_OPERATION_SUCCESSFUL) )
                {
                    break;
                }

                // Proceed with the next target
                continue;
            }

            l_fifoRc = g_targetsTab[l_tgtIdx].applyOverride(
                                            &l_targetEntry, l_helper, o_rc);
            if ( (l_fifoRc != SBE_SEC_OPERATION_SUCCESSFUL) ||
                 (o_rc     != SBE_SEC_OPERATION_SUCCESSFUL) )

            {
                SBE_ERROR(SBE_FUNC" g_targetsTab[l_tgtIdx].applyOverride failed"
                            " TargetType : 0x%02X Instance : %d fifoRc=0x%08X"
                            " rc = 0x%08X",
                            l_targetEntry.iv_logTgtType,
                            l_targetEntry.iv_instance, l_fifoRc, o_rc);
                break;
            }
        }

        l_resBuffer.updateHeader();
        o_resPackSize = l_resBuffer.getOutputPackSize();

    } while(false);

    SBE_INFO("Response packet size in bytes : %d", o_resPackSize);

    SBE_EXIT(SBE_FUNC);
    return l_fifoRc;

    #undef SBE_FUNC
}

} //ATTR

} //fapi2

