/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_list.H $       */
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
#ifndef __ATTRIBUTE_LIST_H__
#define __ATTRIBUTE_LIST_H__

#include <attribute_utils.H>

namespace fapi2
{
namespace ATTR
{

// TODO : Instead of creating a constant here, use sbe_attribute_utils.H from
//        ekb which defines an enum for file type.
const uint8_t ATTRLIST_RESP_FILE_TYPE = 3;
const uint8_t ATTRLIST_MAJOR_VERSION = 0;
const uint8_t ATTRLIST_MINOR_VERSION = 2;

// Attribute List response buffer
/**
 * @class ListResponseBuffer
 *
 * @details List Response Buffer
 *          data members
 *          - iv_headerPtr : HeaderEntry_t
 *          - iv_currPtr : void *
 *          member functions
 *          + setHeader
 *          + setTarget
 *          + setAttribute
 *          + getOutputPackSize
 *          + getNumTargetSections
 *          + getExpectedHeapSize
 *
*/
class ListResponseBuffer
{
    // Attribute List Response
    public:
        HeaderEntry_t   *iv_headerPtr;
        uint8_t         *iv_currPtr;

        ListResponseBuffer() = delete;
        ListResponseBuffer(const ListResponseBuffer& ) = delete;
        ListResponseBuffer&
            operator = (const ListResponseBuffer& ) = delete;

        ListResponseBuffer(void *i_ptr)
        {
            iv_currPtr   = (uint8_t *)i_ptr+sizeof(HeaderEntry_t);
            iv_headerPtr = (HeaderEntry_t *)i_ptr;//this if for start
        }

        /**
         * @brief Set Header in the list response buffer
         *
         * @return
        */
        void  setHeader();

        /**
         * @brief Set Target in the list response buffer
         *
         * @param[in] TargetEntry_t ptr
         *
         * @return
        */
        void  setTarget(TargetEntry_t* i_target);

        /**
         * @brief Set Attribute in the list response buffer
         *
         * @param[in] uint32_t index for attribute Table (g_attrsTab)
         * @param[in] uint8_t row for the attribute (i_row)
         * @param[in] uint8_t col for the attribute (i_col)
         * @param[in] uint8_t hgt for the attribute (i_hgt)
         * @param[in] uint8_t tgt_inst for which we are setting attribute
         *
         * @return
        */
        void setAttribute(
                          uint32_t i_gindex,
                          uint8_t i_row,
                          uint8_t i_col,
                          uint8_t i_hgt,
                          uint8_t i_tgt_inst
                        );

        /**
         * @brief get Output packet size in the list response buffer
         *
         * @return uint32_t size of output response packet
        */
        uint32_t getOutputPackSize()
        {
            return iv_currPtr-reinterpret_cast<uint8_t*>(iv_headerPtr);
        }

        /**
         * @brief get the Expected Heap Size to be allocated
         *
         * @return uint32_t heap expected size
        */
        static uint32_t getExpectedHeapSize();

};

/**
 * @brief iterate through targets table
 *        and for every target
 *        iterate through attribute table for target
 *        send response back in the buffer
 *
 * @param[in] void* o_buffer
 *
 * @return uint32_t heap size
*/
uint32_t listAttribute(void *o_buffer);

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_LIST_H__
