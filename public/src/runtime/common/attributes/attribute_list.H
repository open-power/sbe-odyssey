/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_list.H $       */
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
#ifndef __ATTRIBUTE_LIST_H__
#define __ATTRIBUTE_LIST_H__

#include <attribute_utils.H>
#include "plat_hwp_data_stream.H"

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
    fapi2::sbefifo_hwp_data_ostream* iv_oStream;

    /**
     * @brief Helper function to find the number of target
     *        instances that are present
     *
     * @return uint32_t Number of instances
    */
    uint32_t getTargetInstancesCount(const uint8_t i_tgtIdx) const;

    /**
     * @brief Calculate the number of target section upfront
     *
     * @return uint32_t Number of targets
    */
    uint32_t getNumberOfTargets() const;

    // Attribute List Response
    public:

        ListResponseBuffer(const ListResponseBuffer& ) = delete;
        ListResponseBuffer&
            operator = (const ListResponseBuffer& ) = delete;

        explicit ListResponseBuffer(fapi2::sbefifo_hwp_data_ostream* i_outStream) :
                                                                iv_oStream(i_outStream)
        { };

        /**
         * @brief Set Header in the list response buffer
         *
         * @return uint32_t SBE_SEC_OPERATION_SUCCESSFUL if it is success, otherwise error
        */
        uint32_t streamHeader();

        /**
         * @brief Set Target in the list response buffer
         *
         * @param[in] TargetEntry_t ptr
         *
         * @return uint32_t SBE_SEC_OPERATION_SUCCESSFUL if it is success, otherwise error
        */
        uint32_t streamTarget(const TargetEntry_t* i_target);

        /**
         * @brief Set Attribute in the list response buffer
         *
         * @param[in] uint32_t index for attribute Table (g_attrsTab)
         * @param[in] uint8_t tgt_inst for which we are setting attribute
         *
         * @return uint32_t SBE_SEC_OPERATION_SUCCESSFUL if it is success, otherwise error
        */
        uint32_t streamAttribute(const uint32_t i_gindex,
                                  const uint8_t i_tgt_inst);


};

/**
 * @brief iterate through targets table
 *        and for every target
 *        iterate through attribute table for target
 *        send response back in the buffer
 *
 * @param[out] fapi2::sbefifo_hwp_data_ostream o_outStream
 *
 * @return uint32_t SBE_SEC_OPERATION_SUCCESSFUL if it is success, otherwise error
*/
uint32_t listAttribute(fapi2::sbefifo_hwp_data_ostream* o_outStream);

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_LIST_H__
