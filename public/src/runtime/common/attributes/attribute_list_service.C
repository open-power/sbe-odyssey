/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_list_service.C $ */
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
#include <attribute_list.H>
#include <attribute_table.H>
#include <sbetrace.H>
#include <attribute.H>
#include <target_types.H>
#include "ppe42_string.h"
#include <attrutils.H>
#include <sbe_chip_type.H>

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
        plat_target_sbe_handle l_targetHandle;

        l_rc = g_platTarget->getSbePlatTargetHandle(i_logTargetType,
                                                    i_instanceId,
                                                    l_targetHandle);

        // In case of pervasive targets, the valid instances are
        // 1 and 8. If any other instance is passed, then this
        // function will return the RC:
        //                   SBE_SEC_INVALID_INSTANCE_ID_PASSED.
        // This is not an error per se. This can be ignored.
        if ( l_rc == SBE_SEC_INVALID_INSTANCE_ID_PASSED ) {
            SBE_INFO(SBE_FUNC "Log target type: %d, instance ID: %d"
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
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t ListResponseBuffer::getTargetInstancesCount(const uint8_t i_tgtIdx) const
{
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t l_instancesCount = 0;

    for(uint8_t l_inst = 0;l_inst<g_targetsTab[i_tgtIdx].iv_max_inst;l_inst++)
    {
        bool     l_isPresent;
        l_rc = checkTargetPresent(g_targetsTab[i_tgtIdx].iv_log_target_type,
                                  l_inst, l_isPresent);

        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "checkTargetPresent returned error. l_rc=0x%08X",
                    l_rc);
            l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
            continue;
        }

        if (l_isPresent == false)
        {
            SBE_INFO(SBE_FUNC "Log target type: %d, instance ID: %d"
            " is not present. ", g_targetsTab[i_tgtIdx].iv_log_target_type, l_inst);
            continue;
        }

        ++l_instancesCount;
    }

    return l_instancesCount;
}

uint32_t ListResponseBuffer::getNumberOfTargets() const
{
    uint32_t l_numberOfTargets = 0;

    for(uint8_t l_tgtIdx=0;l_tgtIdx<g_tgts_tab_size;l_tgtIdx++)
    {
        // If there is no attribute for this target type, then
        // no need to check for the presence of this target.
        if (g_targetsTab[l_tgtIdx].iv_attr_row_size == 0)
        {
            SBE_INFO(SBE_FUNC "Log target type : %d has no attributes. Skipping it",
                        g_targetsTab[l_tgtIdx].iv_log_target_type);
            continue;
        }

        l_numberOfTargets += this->getTargetInstancesCount(l_tgtIdx);
    }

    return l_numberOfTargets;
}

uint32_t ListResponseBuffer::streamHeader()
{
    HeaderEntry_t l_header;
    l_header.iv_fmtMajor    = fapi2::ATTR::ATTRLIST_MAJOR_VERSION;
    l_header.iv_fmtMinor    = fapi2::ATTR::ATTRLIST_MINOR_VERSION;
    l_header.iv_chipType    = platGetChipType();

    // For the platforms that use attribute list chip-op
    // response as a payload of attribute update chip-op,
    // platGetChipType() must return CHIP_TYPE_ANY.
    // Generally, it is done to save and restore attributes.
    // In such cases, the file type will be set as
    // ATTROVERRIDE_REQ_FILE_TYPE so that such platforms can
    // directly use the attribute list response as a payload
    // of attribute update chip-op.
    if (platGetChipType() == sbeutil::CHIP_TYPE_ANY)
    {
        l_header.iv_fileType = ATTROVERRIDE_REQ_FILE_TYPE;
    }
    else
    {
        l_header.iv_fileType = ATTRLIST_RESP_FILE_TYPE;
    }

    l_header.iv_numTargets  = this->getNumberOfTargets();

    SBE_DEBUG(SBE_FUNC "Number of targets : %d", l_header.iv_numTargets);

    return iv_oStream->put(sizeof(HeaderEntry_t)/4,(uint32_t *)&l_header);
}

uint32_t ListResponseBuffer::streamTarget(const TargetEntry_t* i_target)
{
    return iv_oStream->put(sizeof(TargetEntry_t)/4, (uint32_t *)i_target);
}

uint32_t ListResponseBuffer::streamAttribute(const uint32_t i_gindex, const uint8_t i_tgt_inst)
{
    AttrEntry_t l_attrEntry;

    uint16_t l_totSize = g_attrsTab[i_gindex].iv_size;

    // In case of array attributes, the row size will be
    // greater than zero. In that case, total size of the
    // attribute will be size of an array element multiplied
    // by the array dimension.
    if ( g_attrsTab[i_gindex].iv_max_row > 0 )
    {
        l_totSize = (l_totSize *
                    g_attrsTab[i_gindex].iv_max_row *
                    g_attrsTab[i_gindex].iv_max_col *
                    g_attrsTab[i_gindex].iv_max_hgt);
    }
    // attrid
    l_attrEntry.iv_attrId   = g_attrsTab[i_gindex].iv_attr_id;
    l_attrEntry.iv_dataSize = l_totSize;

    // In case of array attribute, if the dimension of the array is specified as
    // 0xFF x 0xFF x 0xFF, then it indicates that the data contains all the
    // elements of the array.
    // In case of normal attribute, the dimension will be ignored.
    l_attrEntry.iv_row      = 0xFF;
    l_attrEntry.iv_col      = 0xFF;
    l_attrEntry.iv_hgt      = 0xFF;
    l_attrEntry.iv_res      = 0x0;

    // Including data, AttrEntry_t is in multiple of 8-bytes;
    uint16_t l_paddedLength =
      uint16_t((sizeof(AttrEntry_t) + l_attrEntry.iv_dataSize) + 7) & uint16_t(~7);

    uint8_t l_data[l_paddedLength];

    memset(l_data,0,l_paddedLength);

    memcpy(l_data,&l_attrEntry,sizeof(l_attrEntry));
    memcpy(l_data+sizeof(l_attrEntry), (uint8_t*)g_attrsTab[i_gindex].iv_ptr+(i_tgt_inst)*l_attrEntry.iv_dataSize,
                    l_attrEntry.iv_dataSize);

    return iv_oStream->put(l_paddedLength/4, (uint32_t *)l_data);
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
uint32_t listAttribute(fapi2::sbefifo_hwp_data_ostream* o_outStream)
{
    #define SBE_FUNC " listAttribute "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ListResponseBuffer   l_respBuffer(o_outStream);
    do
    {
        bool l_isPresent = false;

        l_rc = l_respBuffer.streamHeader();
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "streamHeader returned error. l_rc=0x%08X", l_rc);
            break;
        }

        // read target
        for(uint8_t l_tgtIdx=0;l_tgtIdx<g_tgts_tab_size;l_tgtIdx++)
        {
            // If there is no attribute for this target type, then
            // no need to check for the presence of this target.
            if (g_targetsTab[l_tgtIdx].iv_attr_row_size == 0)
            {
                SBE_INFO(SBE_FUNC "Log target type : %d has no attributes. Skipping it",
                            g_targetsTab[l_tgtIdx].iv_log_target_type);
                continue;
            }
            for(uint8_t l_inst = 0;l_inst<g_targetsTab[l_tgtIdx].iv_max_inst;l_inst++)
            {
                l_rc = checkTargetPresent(g_targetsTab[l_tgtIdx].iv_log_target_type,
                                          l_inst, l_isPresent);

                // For attribute generation, the supported target types are picked up
                // from the targetlist.json. For targetting, the supported target types
                // are listed in the target map. If there is a discrepancy between the
                // two, then it results in error. At this time, it is clear that
                // TARGET_TYPE_TEMP_SENSOR is supported only for the attribute generation
                // on certain platforms.So, the chip-op failed. That is correct. As an
                // exception, this error is being skipped.
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "checkTargetPresent returned error. l_rc=0x%08X",
                            l_rc);
                    l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
                    continue;
                }

                if (l_isPresent == false)
                {
                    SBE_INFO(SBE_FUNC "Log target type: %d, instance ID: %d"
                    " is not present. ", g_targetsTab[l_tgtIdx].iv_log_target_type, l_inst);
                    continue;
                }

                // read TargetEntry_t
                TargetEntry_t l_target = {
                    .iv_logTgtType = g_targetsTab[l_tgtIdx].iv_log_target_type,
                    .iv_instance = l_inst,
                    .iv_numAttrs = g_targetsTab[l_tgtIdx].iv_attr_row_size,
                    .iv_magicWord = ATTR_FILE_TARGET_MAGIC_WORD
                };

                SBE_DEBUG(SBE_FUNC "Log target type: %d, instance ID: %d", l_target.iv_logTgtType,
                            l_target.iv_instance);

                l_rc = l_respBuffer.streamTarget(&l_target);
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    SBE_ERROR(SBE_FUNC "streamTarget returned error. l_rc=0x%08X", l_rc);
                    break;
                }

                for(uint32_t i = 0;i<g_targetsTab[l_tgtIdx].iv_attr_row_size; i++)
                {
                    uint32_t g_index = g_targetsTab[l_tgtIdx].iv_attr_row_start + i;

                    l_rc = l_respBuffer.streamAttribute(g_index, l_inst);
                    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                    {
                        SBE_ERROR(SBE_FUNC "streamAttribute returned error. l_rc=0x%08X", l_rc);
                        break;
                    }
                }

                // If SBE FIFO fails, then streamAttribute() will return RC. In that case,
                // we don't want to proceed with the next target instance as SBE FIFO errors
                // are not recoverable.
                if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
                {
                    break;
                }
            }

            // If SBE FIFO fails, streamTarget() or streamAttribute() will return RC.
            // In that case, we don't want to proceed with the next target as SBE FIFO
            // errors are not recoverable.
            if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                break;
            }
        }
    }while(false);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

} //ATTR

} //fapi2
