/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_utils.C $      */
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

#include <stdint.h>
#include <sbe_sp_intf.H>
#include <attribute_utils.H>
#include <attribute_table.H>
#include <sbetrace.H>
#include <heap.H>

#define ATTR_UPDATE_RESPONSE_BUFF_MIN_PERCENTAGE 10

namespace fapi2
{
namespace ATTR
{

uint32_t HeaderEntry_t::verifyForUpdateRequest() const
{
    #define SBE_FUNC "verifyUpdateRequest"

    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        if ( !this->isUpdateRequest() )
        {
            l_rc = SBE_SEC_ATTR_UPD_INVALID_FILE_TYPE;
            SBE_ERROR(SBE_FUNC "Invalid file type [%d] Expected file type [%d]",
                      iv_fileType, fapi2::ATTR::ATTROVERRIDE_REQ_FILE_TYPE);
            break;
        }

        if ( !this->isValidUpdateVersion() )
        {
            l_rc = SBE_SEC_ATTR_UPD_VER_NOT_SUPPORTED;
            SBE_ERROR(SBE_FUNC "Major [%d] Minor [%d] version not supported",
                      iv_fmtMajor, iv_fmtMinor);
            SBE_ERROR("Supported Major[%d] Minor[%d]",
                    fapi2::ATTR::ATTROVERRIDE_MAJOR_VERSION,
                    fapi2::ATTR::ATTROVERRIDE_MINOR_VERSION);
            break;
        }
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;

    #undef SBE_FUNC
}

AttributeOverrideRc TargetEntry_t::getIndex(uint16_t &o_tgtIdx) const
{
    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_TGT_TYPE_NOT_FOUND;

    o_tgtIdx = 0;

    for (uint16_t i = 0; i < g_tgts_tab_size; i++)
    {
        if (g_targetsTab[i].iv_log_target_type == iv_logTgtType)
        {
            o_tgtIdx = i;
            l_rc = ATTROVERRIDE_RC_SUCCESS;
            break;
        }
    }

    return l_rc;
}

AttributeOverrideRc AttrInfo_t::getIndex(const uint16_t i_startRow,
                                         const uint16_t i_noOfRow,
                                         uint32_t & o_attrIdx) const
{
    AttributeOverrideRc l_rc = ATTROVERRIDE_RC_ATTR_NOT_FOUND;
    o_attrIdx = 0;

    for (uint16_t i=0; i<i_noOfRow; i++)
    {
        if (g_attrsTab[i_startRow+i].iv_attr_id == iv_attrId)
        {
            o_attrIdx = i_startRow+i;
            l_rc = ATTROVERRIDE_RC_SUCCESS;
            break;
        }
    }

    return l_rc;
}

uint32_t getMaxResBuffSize()
{
    uint32_t l_maxSize = 0;

    for (uint8_t idx=0; idx < g_tgts_tab_size; idx++)
    {
        uint32_t l_maxTargetSize = ( sizeof(TargetEntry_t) +
                                        (sizeof(AttrRespEntry_t) * g_targetsTab[idx].iv_attr_row_size) ) * g_targetsTab[idx].iv_max_inst;
        l_maxSize += l_maxTargetSize;
    }

    l_maxSize += sizeof(HeaderEntry_t);

    return l_maxSize;
}


uint32_t getResponseBuffer(void *&o_buffer, uint32_t &o_bufSize)
{
    #define SBE_FUNC "getResponseBuffer"

    SBE_ENTER(SBE_FUNC);

    o_buffer = nullptr;
    o_bufSize = 0;

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        uint32_t l_avlHeap = Heap::get_instance().getFreeHeapSize();

        // Due to memory constraint, we assume that we need only
        //                  ATTR_UPDATE_RESPONSE_BUFF_MIN_PERCENTAGE of the maximum
        // buffer space required for storing the attribute update response. ie. the
        // failure may not cross ATTR_UPDATE_RESPONSE_BUFF_MIN_PERCENTAGE.
        uint32_t l_maxSizeRequired = ((getMaxResBuffSize() *
                                       ATTR_UPDATE_RESPONSE_BUFF_MIN_PERCENTAGE) / 100);

        SBE_INFO(SBE_FUNC"Available heap=%d %d%% of max buffer size required:%d (in bytes)",
                     l_avlHeap, ATTR_UPDATE_RESPONSE_BUFF_MIN_PERCENTAGE, l_maxSizeRequired);

        o_bufSize = ( (l_avlHeap > l_maxSizeRequired) ? l_maxSizeRequired : l_avlHeap );
        if (o_bufSize < sizeof(HeaderEntry_t))
        {
            SBE_ERROR(SBE_FUNC"Min scratch space [%d] is not available",
                    sizeof(HeaderEntry_t));
            l_rc=SBE_SEC_HEAP_SPACE_FULL_FAILURE;
            break;
        }

        o_buffer = Heap::get_instance().scratch_alloc(o_bufSize);
        if (o_buffer == nullptr)
        {
            SBE_ERROR(SBE_FUNC"scratch allocation request for [%d] bytes failed",
                        o_bufSize);
            l_rc=SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
        }
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;

    #undef SBE_FUNC
}

} //ATTR

} //fapi2
