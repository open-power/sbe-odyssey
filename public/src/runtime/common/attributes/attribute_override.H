/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/attributes/attribute_override.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2025                        */
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
// TODO : Attribute override is common for odyssey and pst. As pst attribute has not
//        been setup yet(addition of pst SBE filter XML, etc...), for the time being,
//        attribute update related changes have been checked-in under runtime Odyssey
//        sppe code. Once pst SBE attribute XML has been updated, move these files to
//        runtime common code.

#ifndef __ATTRIBUTE_OVERRIDE_H__
#define __ATTRIBUTE_OVERRIDE_H__

#include <attribute_utils.H>
#include <fapi2.H>
#include <plat_hwp_data_stream.H>

namespace fapi2
{
namespace ATTR
{

/**
 *  @brief  Class for handling incoming attribute update data
 *
 */
class AttrUpdateDataStream
{
    friend class AttrUpdateHelper;

    sbefifo_hwp_data_istream* iv_stream;

    /**
     * @brief  Internal method to discard the given number of bytes
     *         from the SBE FIFO
     *
     * @param[in]  i_bytesToDiscard The number of bytes to discard
     *
     * @return  SBE_SEC_OPERATION_SUCCESSFUL if successful,
     *          non-zero in caes of SBE FIFO operation failure
     */
    uint32_t readAndDiscard(const uint16_t i_bytesToDiscard);

    public:
        /**
         * @brief  Constructor
         *
         * @param[in]   i_stream    SBE FIFO stream providing the attribute update
         *                          data
         *
         */
        explicit AttrUpdateDataStream(sbefifo_hwp_data_istream *i_stream) : iv_stream(i_stream)
        { }

        /**
         * @brief  Method to read the header of the attribute update data
         *
         * @param[out] o_header      A pointer to the buffer to store the
         *                           header
         *
         * @return SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *        non-zero in caes of SBE FIFO operation failure
         */
        uint32_t readHeader(HeaderEntry_t *o_header);

        /**
         * @brief  Method to read the next target section
         *
         * @param[out]  o_targetEntry   A pointer to the buffer to store the target
         *
         * @return SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *        non-zero in caes of SBE FIFO operation failure
         */
        uint32_t getNextTarget(TargetEntry_t *o_targetEntry);

        /**
         * @brief  Method to read the next attribute's details
         *
         * @param[out]  o_attrSize    A pointer to the buffer to store the attribute's
         *                            details, which provides attribute-id,
         *                            attribute size, and the first two dimension.
         *
         * @return SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *        non-zero in caes of SBE FIFO operation failure
         *
         */
        uint32_t getNextAttrInfo(AttrInfo_t *o_attrInfo);

        /**
         * @brief  Method to read the next attribute's data portion
         *
         * @param[io]  o_buffer         A pointer to the buffer to store the
         *                              attribute data
         *
         * @param[out]  i_bufferSize    The size of the buffer
         *
         * @return  SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *        non-zero in caes of SBE FIFO operation failure
         */
        uint32_t getNextAttrData(uint8_t *o_buffer, const uint32_t i_bufferSize);
};

/**
 *  @brief  Class for handling attribute update response
 *
 */
class AttrUpdateResBuffer
{

    friend class AttrUpdateHelper;

    uint8_t         *iv_bufPtr;
    uint32_t         iv_bufSize;
    uint8_t         *iv_currPtr;
    TargetEntry_t   *iv_lastTargetPtr;
    uint8_t          iv_numTargets;

    /**
     * @brief  Internal method to add an attribute in the response
     *
     * @param[in]  i_attrRespEntry      Attribute-id and the override RC
     *                                  indicating the reason for the
     *                                  update failure
     *
     * @return  SBE_SEC_OPERATION_SUCCESSFUL if successful,
     *          non-zero in case of error
     */
    uint32_t addAttribute(const AttrRespEntry_t &i_attrRespEntry);

    public:

        /**
         * @brief  Constructor
         *
         * @param[in]   i_chipType    SBE chip type to identify the chip
         *                            requesting attribute update chip-op
         *
         * @param[in]   i_butPtr      Pointer to the buffer to store the
         *                            response
         *
         * @param[in]   i_butPtr      The size of the buffer
         *
         */
        AttrUpdateResBuffer(const uint8_t i_chipType, uint8_t *i_bufPtr, const uint32_t i_bufSize);

        /**
         * @brief  Method to add a target section in the response
         *
         * @param[in]  i_targetEntry    A pointer to the target section,
         *                              providing the target type and the
         *                              instance
         *
         * @return  SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *          non-zero in case of error
         */
        uint32_t addTarget(const TargetEntry_t* i_target);

        /**
         * @brief  Method to add an attribute in the response
         *
         * @param[in]  i_targetEntry        A pointer to the target section,
         *                                  providing the target type and the
         *                                  instance
         *
         * @param[in]  i_attrEntry          A pointer to the attribute row,
         *                                  providing the attribute-id
         *
         * @param[in]  AttributeOverrideRc  RC indicating the the reason
         *                                  for the update failure
         *
         * @return  SBE_SEC_OPERATION_SUCCESSFUL if successful,
         *          non-zero in case of error
         */
        uint32_t addAttribute(const TargetEntry_t* i_target,
                              const AttrEntry_t*  i_attrEntry,
                              const AttributeOverrideRc i_rc);

        /**
         * @brief  Update the number of targets in the response header
         */
        void updateHeader()
        {
            HeaderEntry_t  *l_header = (HeaderEntry_t *)(iv_bufPtr);
            l_header->iv_numTargets = iv_numTargets;
        }

        /**
         * @brief  Method to get the output packet size in bytes
         *
         * @return  Size of the response buffer in bytes
         */
        uint32_t getOutputPackSize() const
        {
            return (iv_currPtr - iv_bufPtr);
        }
};

/**
 *  @brief  Helper class providing methods that require access to
 *          input stream and response buffer
 *
 */
class AttrUpdateHelper
{
    AttrUpdateDataStream&   iv_inStream;
    AttrUpdateResBuffer&    iv_outBuffer;

    public:
        AttrUpdateHelper(AttrUpdateDataStream & i_inStream, AttrUpdateResBuffer & i_outBuffer) :
                                                                            iv_inStream(i_inStream),
                                                                            iv_outBuffer(i_outBuffer)
        { }

        /**
         * @brief  Method to handle invalid target type or target instance
         *         in the attribute update data
         *
         * @param[in]  i_targetEntry    A pointer to the target section, which
         *                              provides the target type and the target
         *                              instance
         *
         * @param[in]  i_overrideRc     AttributeOverrideRc indicating the error
         *
         *
         * @param[out] o_rc             Software error,if any
         *
         * @return  SBE_SEC_OPERATION_SUCCESSFUL if the SBE FIFO operation is successful,
         *          Otherwise, the error RC
         */
        uint32_t handleTargetError(const TargetEntry_t & i_targetEntry,
                                   const AttributeOverrideRc i_overrideRc,
                                   uint32_t & o_rc);

        uint32_t getNextAttrInfo(AttrInfo_t *o_attrInfo)
        {
            return iv_inStream.getNextAttrInfo(o_attrInfo);
        }

        uint32_t getNextAttrData(uint8_t *o_buffer, const uint32_t i_bufSize)
        {
            return iv_inStream.getNextAttrData(o_buffer,i_bufSize);
        }

        uint32_t addAttribute(const TargetEntry_t *i_targetEntry,
                              const AttrEntry_t  *i_attrEntry,
                              const AttributeOverrideRc i_overrideRc)
        {
            return iv_outBuffer.addAttribute(i_targetEntry, i_attrEntry, i_overrideRc);
        }
};

} //ATTR

} //fapi2

#endif // __ATTRIBUTE_OVERRIDE_H__
