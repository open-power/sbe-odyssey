/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_override.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
// JIRA: PFSBE-254
// TODO : Attribute override is common for odyssey and p11. As p11 attribute has not
//        been setup yet(addition of p11 SBE filter XML, etc...), for the time being,
//        attribute update related changes have been checked-in under runtime Odyssey
//        sppe code. Once p11 SBE attribute XML has been updated, move these files to
//        runtime common code.

#ifndef __ATTRIBUTE_OVERRIDE_H__
#define __ATTRIBUTE_OVERRIDE_H__

#include <attribute_utils.H>
#include <fapi2.H>

namespace fapi2
{
namespace ATTR
{

// Forward declaration
class ResponseBuffer;

/**
 *  @brief  Utility class for handling incoming attribute update buffer
 *
 */
class AttributeUpdateBuffer
{
    private:
        HeaderEntry_t   *iv_headerPtr;
        uint8_t         *iv_bufEnd;
        uint8_t         *iv_currPtr;
        ResponseBuffer *iv_response;

    public:
        AttributeUpdateBuffer() = delete;
        AttributeUpdateBuffer(const AttributeUpdateBuffer& ) = delete;
        AttributeUpdateBuffer&
            operator = (const AttributeUpdateBuffer& ) = delete;

        /**
         * @brief  Constructor
         *
         * @param[in]   i_buf       Buffer containing attribute update data
         *
         * @param[in]   i_bufSize   The size of the buffer
         */
        AttributeUpdateBuffer(const void *i_buf, const uint32_t i_bufSize)
        {
            iv_headerPtr = (HeaderEntry_t *)i_buf;
            iv_bufEnd    = (uint8_t *)i_buf + i_bufSize;
            iv_currPtr   = (uint8_t *)i_buf + sizeof(HeaderEntry_t);
            iv_response  = nullptr;
        }

        /**
         * @brief  Method to read the header of the attribute update data
         *
         * @return Returns a pointer to the header row
         */
        HeaderEntry_t* getHeader()
        {
            return iv_headerPtr;
        }

        /**
         * @brief  Checks if the major version is supported
         *
         * @return true if the major version is supported
         */
        bool majorVerSupported() const
        {
            return (iv_headerPtr->iv_fmtMajor == ATTROVERRIDE_MAJOR_VERSION);
        }

        /**
         * @brief  Method to get the major version
         */
        uint8_t majorVersion() const
        {
            return iv_headerPtr->iv_fmtMajor;
        }

        /**
         * @brief  Checks if the minor version is supported
         *
         * @return true if the minor version is supported
         */
        bool minorVerSupported() const
        {
            return (iv_headerPtr->iv_fmtMinor == ATTROVERRIDE_MINOR_VERSION);
        }

        /**
         * @brief  Method to get the minor version
         */
        uint8_t minorVersion() const
        {
            return iv_headerPtr->iv_fmtMinor;
        }

        /**
         * @brief  Checks if the file type is valid
         *
         * @return true if the file type is valid
         */
        bool validFileType() const
        {
            return (iv_headerPtr->iv_fileType == ATTROVERRIDE_REQ_FILE_TYPE);
        }

        /**
         * @brief  Method to get the file type
         */
        uint8_t fileType() const
        {
            return iv_headerPtr->iv_fileType;
        }

        /**
         * @brief  Method to get the number of target sections in the
         *         attribute updata data
         *
         * @return Returns number of target sections
         */
        uint32_t getNumTargets() const
        {
            return iv_headerPtr->iv_numTargets;
        }

        /**
         * @brief  Method to read the next target section
         *
         * @param[out]  o_targetEntry   A pointer to the target section
         *
         * @return  ATTROVERRIDE_RC_SUCCESS if successful
         *          non-zero in case of error
         */
        uint32_t getNextTarget(TargetEntry_t *&o_targetEntry);

        /**
         * @brief  Method to read the next attribute entry
         *
         * @param[out]  &o_attrEntry    A pointer to an attribute entry
         *
         * @return  ATTROVERRIDE_RC_SUCCESS if successful
         *          non-zero in case of error
         */
        uint32_t getNextAttr(AttrEntry_t *&o_attrEntry);

        /**
         * @brief  Method to read the current position of the buffer pointer
         *
         * @return  Returns the current position of the buffer pointer
         */
        const uint8_t* getCurrentPtr() const
        {
            return iv_currPtr;
        }

        /**
         * @brief  Method to set the response buffer object
         */
        void setResponseBuf(ResponseBuffer *i_response)
        {
            iv_response = i_response;
        }

        /**
         * @brief  Method to get the response buffer object
         *
         * @return  Returns the response buffer object
         */
        ResponseBuffer & getResponseBuf()
        {
            return *iv_response;
        }

        /**
         * @brief  Method to know the last position of the buffer
         *
         * @return  Returns the last position of the buffer
         */
        const uint8_t* getBufferEnd() const
        {
            return iv_bufEnd;
        }
};

/**
 *  @brief  Utility class for handling attribute update response buffer
 *
 */
class ResponseBuffer
{
    private:
        uint8_t                 *iv_currPtr;
        TargetEntry_t           *iv_lastTargetPtr;
        uint32_t                 iv_numTargets;
        AttributeUpdateBuffer&   iv_inputBuffer;

    public:
        ResponseBuffer() = delete;
        ResponseBuffer(const ResponseBuffer& ) = delete;
        ResponseBuffer&
            operator = (const ResponseBuffer& ) = delete;

        ResponseBuffer(AttributeUpdateBuffer& i_inputBuffer);

        /**
         * @brief  Method to add a target section
         *
         * @param[in]  i_target    A pointer to the target section
         */
        void addTarget(const TargetEntry_t* i_target);

        /**
         * @brief  Method to add a target section in the response
         *
         * @param[in]  i_target    A pointer to the target section
         *
         * @param[in]  i_rc        RC to be set
         *
         * @return  ATTROVERRIDE_RC_SUCCESS if successful
         *          non-zero in case of error
         */
        uint32_t addTarget(const TargetEntry_t* i_target,
                           const AttributeOverrideRc i_rc);

        /**
         * @brief  Method to add an attribute row in the response
         *
         * @param[in]  i_target    A pointer to the target section
         *
         * @param[in]  i_attrEntry A pointer to the attribute row
         *
         * @param[in]  i_rc        RC to be set
         */
        void addAttr(const TargetEntry_t* i_target,
                         const AttrEntry_t*  i_attrEntry,
                         const AttributeOverrideRc i_rc);

        // Both input and output buffer point to the same header.
        // So, the number of target sections in the output buffer
        // will be updated just before sending the response.
        // Otherwise, the number of target sections in the input
        // buffer will be overwritten.
        void updateHeader()
        {
            iv_inputBuffer.getHeader()->iv_fileType = ATTROVERRIDE_RESP_FILE_TYPE;
            iv_inputBuffer.getHeader()->iv_numTargets = iv_numTargets;
        }

        /**
         * @brief  Method to get the output packet size in bytes
         *
         * @return  Number of bytes of the response buffer
         */
        uint32_t getOutputPackSize() const
        {
            return (iv_currPtr - (uint8_t *)iv_inputBuffer.getHeader());
        }
};

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_OVERRIDE_H__
