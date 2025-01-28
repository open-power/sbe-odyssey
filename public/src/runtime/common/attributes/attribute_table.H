/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_table.H $      */
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
#pragma once

#include <attribute_ids.H>
#include <attribute_utils.H>
#include <target_types.H>
#include <plat_hwp_data_stream.H>

namespace fapi2
{
namespace ATTR
{

struct AttributesTable
{
    uint32_t                iv_memory_shared : 1;
    uint32_t                iv_deny_update   : 1;
    uint32_t                iv_reserved      : 2;
    uint32_t                iv_attr_id       : 28;
    uint8_t                 iv_max_row; //Row
    uint8_t                 iv_max_col; //Column
    uint8_t                 iv_max_hgt; //Height
    uint8_t                 iv_size;
    void                   *iv_ptr;

    /**
     * @brief  Method to update the attribute of the given target instance
     *
     * @param[in]  i_tgt_inst       Target instance to be updated
     *
     * @param[in]  i_attrEntry      The attribute section providing the
     *                              new value of the attribute
     *
     * @return  ATTROVERRIDE_RC_SUCCESS if the attribute has been sucessfully
     *          updated; non-zero in case of error
     */
    AttributeOverrideRc applyOverride(const uint8_t       i_tgt_inst,
                                      const AttrEntry_t*  i_attrEntry);

};

// Forward declaration
class AttrUpdateHelper;

struct TargetsTable
{
    fapi2::LogTargetType    iv_log_target_type;
    uint8_t                 iv_max_inst;
    uint16_t                iv_attr_row_start;
    uint16_t                iv_attr_row_size;
    uint8_t                 iv_gen_override;

    /**
     * @brief  Method to iterate through the attributes of the given target
     *         section and update them
     *
     * @param[in]  i_targetEntry    A pointer to the target section, which
     *                              provide the instance of the target type
     *                              to be updated and the number of attributes
     *                              to be updated
     *
     * @param[in]  i_attrUpdHelper  Object to read the attribute sections from
     *                              the update data and store the response
     *
     * @param[out] o_rc             Software error,if any
     *
     * @return  SBE_SEC_OPERATION_SUCCESSFUL if the SBE FIFO operation is successful,
     *          Otherwise, the error RC
     */
    uint32_t applyOverride(const TargetEntry_t*  i_targetEntry,
                           AttrUpdateHelper &    i_attrUpdHelper,
                           uint32_t & o_rc);
};

/**
 * @brief  Method to update attribute tank. The update data is read
 *         from the SBE FIFO stream.
 *
 * @param[in]  i_stream         SBE FIFO stream providing the attribute
 *                              update data
 *
 * @param[in]  i_buffer         Buffer to store the response
 *
 * @param[out] i_bufSize        The size of the response buffer
 *
 * @param[out] o_resPackSize    The response packet size
 *
 * @param[out] o_rc             Software error,if any
 *
 * @return  SBE_SEC_OPERATION_SUCCESSFUL if the SBE FIFO operation is successful,
 *          Otherwise, the error RC
 */
uint32_t applyOverride(sbefifo_hwp_data_istream &i_stream,
                       void *i_buffer,
                       const uint32_t i_bufSize,
                       uint32_t & o_resPackSize,
                       uint32_t & o_rc);


extern AttributesTable g_attrsTab[];

extern TargetsTable    g_targetsTab[];

extern uint8_t         g_tgts_tab_size;

} //ATTR

} //fapi2
