/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/attributes/attribute_table.H $ */
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
#ifndef __ATTRIBUTE_TABLE_H__
#define __ATTRIBUTE_TABLE_H__

#include <attribute_ids.H>
#include <attribute_utils.H>

namespace fapi2
{
namespace ATTR
{

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

// Forward declaration
class AttributeUpdateBuffer;

struct TargetsTable
{
    fapi2::LogTargetType    iv_log_target_type;
    uint8_t                 iv_max_inst;
    uint16_t                iv_attr_row_start;
    uint16_t                iv_attr_row_size;

    uint32_t applyOverride(const TargetEntry_t *i_targetEntry,
                           AttributeUpdateBuffer&  i_buffer);

};

uint32_t    applyOverride(const void *io_buffer, const uint32_t i_bufSize,
                          uint32_t & o_resPackSize);



extern AttributesTable g_attrsTab[];

extern TargetsTable    g_targetsTab[];

extern uint8_t         g_tgts_tab_size;

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_TABLE_H__
