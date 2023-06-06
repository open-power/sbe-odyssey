/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_list_service.C $ */
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
#include <attribute_list.H>
#include <attribute_table.H>
#include <sbetrace.H>
#include <attribute.H>
#include <target_types.H>
#include "ppe42_string.h"
#include <plat_target_service.H>

namespace fapi2
{
namespace ATTR
{

/**
 * @brief Check if a target is present
 *
 * @param[in] i_logTargetType   Log target type to be checked
 *
 * @param[in] i_instanceId      Instance Id of the target to be checked
 *
 * @param[out] o_present        True if it is present in the target handle
 *                              returned by getSbePlatTargetHandle(). Othewise,
 *                              false.
 *
 * @return RC from getSbePlatTargetHandle()
*/
static sbeSecondaryResponse checkTargetPresent(const uint8_t i_logTargetType,
                                               const uint8_t i_instanceId,
                                               bool & o_present)
{
    #define SBE_FUNC " checkTargetPresent "
    SBE_ENTER(SBE_FUNC);

    sbeSecondaryResponse l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    o_present = false;
    do
    {
        // TARGET_TYPE_SYSTEM will be always present
        // Also, the below logic will not work for TARGET_TYPE_SYSTEM
        // since it is not available in the SBE target vector
        if (i_logTargetType == fapi2::LOG_TARGET_TYPE_SYSTEM)
        {
            o_present = true;
            break;
        }
        fapi2::plat_target_handle_t l_targetHandle;

        l_rc = g_platTarget->getSbePlatTargetHandle(i_logTargetType,
                                                    i_instanceId,
                                                    l_targetHandle);
        if ( l_rc == SBE_SEC_INVALID_INSTANCE_ID_PASSED ) {
            SBE_DEBUG(SBE_FUNC "Log target type: %d, instance ID: %d"
            " is invalid. ", i_logTargetType, i_instanceId);
            l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
            break;
        }

        if ( l_rc != SBE_SEC_OPERATION_SUCCESSFUL )
        {
            SBE_ERROR(SBE_FUNC "getSbePlatTargetHandle() returned error for "
            "Log target type: %d, instance ID: %d", i_logTargetType, i_instanceId);
            break;
        }
        if ( l_targetHandle.getPresent() == true )
        {
            SBE_DEBUG(SBE_FUNC "Log target type: %d, instance ID: %d"
            " is present. ", i_logTargetType, i_instanceId);
            o_present = true;
            break;
        }
    } while (false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

void  ListResponseBuffer::setHeader()
{
    iv_headerPtr->iv_fmtMajor = fapi2::ATTR::ATTRLIST_MAJOR_VERSION;
    iv_headerPtr->iv_fmtMinor = fapi2::ATTR::ATTRLIST_MINOR_VERSION;
    iv_headerPtr->iv_chipType = CHIP_TYPE_ODYSSEY_00 +
                                fapi2::ATTR::TARGET_TYPE_OCMB_CHIP::ATTR_BUS_POS;
    iv_headerPtr->iv_fileType = ATTRLIST_RESP_FILE_TYPE;
    iv_headerPtr->iv_numTargets = 0;
}

void  ListResponseBuffer::setTarget(TargetEntry_t* i_target)
{
    assert(nullptr!=i_target);
    iv_headerPtr->iv_numTargets++;
    memcpy((void *)iv_currPtr, (uint8_t *)i_target, sizeof(TargetEntry_t));
    iv_currPtr += sizeof(TargetEntry_t);
}

void ListResponseBuffer::setAttribute(uint32_t i_gindex,
                                      uint8_t i_row,
                                      uint8_t i_col,
                                      uint8_t i_hgt,
                                      uint8_t i_tgt_inst)
{
    uint16_t data_size = 0;
    uint8_t index[4] = {0xff,0xff,0xff,0};
    if (i_row == 0 && i_col == 0 && i_hgt == 0)
    {
        // for calculating data_size and index
        i_row = 1;
        i_col = 1;
        i_hgt = 1;
    }
    else
    {
        // max_row, max_col and max_hgt
        i_row = g_attrsTab[i_gindex].iv_max_row;
        i_col = g_attrsTab[i_gindex].iv_max_col;
        i_hgt = g_attrsTab[i_gindex].iv_max_hgt;
    }
    // attrid
    *((uint32_t*)iv_currPtr) = static_cast<uint32_t>(g_attrsTab[i_gindex].iv_attr_id);;
    iv_currPtr += sizeof(uint32_t);

    // size
    data_size = static_cast<uint16_t>(g_attrsTab[i_gindex].iv_size)*i_row*i_col*i_hgt;
    *((uint16_t*)iv_currPtr) = data_size;
    iv_currPtr += sizeof(uint16_t);

    // index
    memcpy(iv_currPtr, index, 4*sizeof(uint8_t));
    iv_currPtr += 4*sizeof(uint8_t);

    // data
    memcpy(iv_currPtr,
    (uint8_t*)g_attrsTab[i_gindex].iv_ptr+(i_tgt_inst)*data_size,
    data_size);
    iv_currPtr += data_size;

    // padding needed due to alignment
    if(getOutputPackSize()%8!=0)
    {
        uint8_t padding = 8-(getOutputPackSize()%8);
        iv_currPtr += padding;
    }
}

uint32_t ListResponseBuffer::getExpectedHeapSize()
{
    uint32_t expectedHeapSize = 0;
    expectedHeapSize += sizeof(HeaderEntry_t);
    for(uint8_t l_tgtIdx=0;l_tgtIdx<g_tgts_tab_size;l_tgtIdx++)
    {
        for(uint8_t l_inst = 0;l_inst<g_targetsTab[l_tgtIdx].iv_max_inst;l_inst++)
        {
            expectedHeapSize += sizeof(TargetEntry_t);
            for(uint32_t i = 0;i<g_targetsTab[l_tgtIdx].iv_attr_row_size; i++)
            {

                uint32_t g_index = g_targetsTab[l_tgtIdx].iv_attr_row_start + i;
                // attr id
                expectedHeapSize += 4;
                // size field
                expectedHeapSize += sizeof(uint16_t);
                // index
                expectedHeapSize += 4*sizeof(uint8_t);
                uint8_t row = g_attrsTab[g_index].iv_max_row;
                uint8_t col = g_attrsTab[g_index].iv_max_col;
                uint8_t hgt = g_attrsTab[g_index].iv_max_hgt;
                if ( row == 0 && col == 0 && hgt == 0)
                {
                    // for calculating data_size and index
                    row = 1;
                    col = 1;
                    hgt = 1;
                }
                expectedHeapSize += g_attrsTab[g_index].iv_size*row*col*hgt;
                if(expectedHeapSize%8!=0)
                {
                    expectedHeapSize += 8-expectedHeapSize%8;
                }
            }
        }
    }
    return expectedHeapSize;

}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t listAttribute(void *o_buffer)
{
    #define SBE_FUNC " listAttribute "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = 0;
    ListResponseBuffer   l_respBuffer(o_buffer);
    do
    {
        SBE_INFO(SBE_FUNC " resp buffer after set header = 0x%08x", l_respBuffer.iv_currPtr);
        l_respBuffer.setHeader();

        bool l_isPresent = false;
        // read target
        for(uint8_t l_tgtIdx=0;l_tgtIdx<g_tgts_tab_size;l_tgtIdx++)
        {
            for(uint8_t l_inst = 0;l_inst<g_targetsTab[l_tgtIdx].iv_max_inst;l_inst++)
            {
                l_rc = checkTargetPresent(g_targetsTab[l_tgtIdx].iv_log_target_type,
                                          l_inst, l_isPresent);

                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "checkTargetPresent returned error. l_rc=0x%08X",
                            l_rc);
                    assert(false);
                }

                if (l_isPresent == false)
                {
                    SBE_INFO(SBE_FUNC "Log target type: %d, instance ID: %d"
                    " is not present. ", g_targetsTab[l_tgtIdx].iv_log_target_type, l_inst);
                    continue;
                }

                // read TargetEntry_t
                TargetEntry_t target = {
                    .iv_logTgtType = g_targetsTab[l_tgtIdx].iv_log_target_type,
                    .iv_instance = l_inst,
                    .iv_numAttrs = g_targetsTab[l_tgtIdx].iv_attr_row_size,
                    .iv_magicWord = ATTR_FILE_TARGET_MAGIC_WORD
                };
                l_respBuffer.setTarget(& target);
                for(uint32_t i = 0;i<g_targetsTab[l_tgtIdx].iv_attr_row_size; i++)
                {
                    uint32_t g_index = g_targetsTab[l_tgtIdx].iv_attr_row_start + i;
                    uint8_t row = g_attrsTab[g_index].iv_max_row;
                    uint8_t col = g_attrsTab[g_index].iv_max_col;
                    uint8_t hgt = g_attrsTab[g_index].iv_max_hgt;
                    l_respBuffer.setAttribute( g_index, row, col, hgt, l_inst);
                }
            }
        }
    }while(false);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

} //ATTR

} //fapi2
