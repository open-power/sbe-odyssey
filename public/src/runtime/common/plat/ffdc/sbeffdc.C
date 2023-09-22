/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdc.C $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
#include "sbeFifoMsgUtils.H"
#include "sbeffdc.H"
#include "heap.H"
#include "plat_hwp_data_stream.H"
#include "sbestates.H"
#include "sberegaccess.H"

extern fapi2::pozFfdcData_t g_FfdcData;

#define FIFO_BLOB_DATA_UPDATE( length, headerptr) \
    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(ffdcPakageStream.put(length, headerptr)) \

void SbeFFDCPackage::updateHWpackageDataHeader(void)
{
    #define SBE_FUNC "updateHWpackageDataHeader"
    SBE_ENTER(SBE_FUNC);

    //As part of constructor ffdc package data header is updated with dump
    //fileds  and now update the length in ffdc package header base on required
    //fields
    iv_ffdcPackageHeader.lenInWords += HW_HEADER_SIZE_IN_WORDS;

    if( iv_hwDataHeader.dumpFields.get() & SBE_FFDC_HW_DATA )
    {
        // Package HW Data Blob with Field id and length
        iv_hwLocalBlob.field.fieldId = (uint16_t)SBE_FFDC_HW_DATA;
        iv_hwLocalBlob.field.fieldLen = fapi2::g_FfdcData.ffdcLength;
        iv_ffdcPackageHeader.lenInWords +=
                    ( BLOB_HEADER_SIZE_IN_WORDS +
                    (iv_hwLocalBlob.field.fieldLen/sizeof(uint32_t)));

        SBE_INFO(SBE_FUNC" Enable SBE_FFDC_HW_DATA with lenInWords[%d]",
                              (uint32_t)iv_ffdcPackageHeader.lenInWords);
    }
    if( iv_hwDataHeader.dumpFields.get()  & SBE_FFDC_REG_DATA )
    {
        iv_hwRegBlob.field.fieldId = (uint16_t)SBE_FFDC_REG_DATA;
        iv_hwRegBlob.field.fieldLen = 0;
        iv_ffdcPackageHeader.lenInWords += BLOB_HEADER_SIZE_IN_WORDS;
        SBE_INFO(SBE_FUNC" Enable SBE_FFDC_REG_DATA with lenInWords[%d]",
                              (uint32_t)iv_ffdcPackageHeader.lenInWords);
        // @TODO add length for HW Reg data blob
    }

    SBE_INFO(SBE_FUNC" All Hw local and Reg data in words[%d] ",
                                    uint32_t(iv_ffdcPackageHeader.lenInWords));
    return;
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

void SbeFFDCPackage::updateSBEpackageDataHeader(void)
{
    #define SBE_FUNC "updateSBEpackageDataHeader"
    SBE_ENTER(SBE_FUNC);

    //Update the pakage data header with dump fields configuration
    iv_ffdcPackageHeader.lenInWords += PLAT_HEADER_SIZE_IN_WORDS;
    if( iv_sbeDataHeader.dumpFields.get() & SBE_FFDC_TRACE_DATA )
    {
        // Package SBE Trace Data Blob with Field id and length
        iv_sbeTraceBlob.field.fieldId = (uint16_t)SBE_FFDC_TRACE_DATA;
        iv_sbeTraceBlob.field.fieldLen = SBE_FFDC_TRACE_DUMP_LENGTH;
        iv_ffdcPackageHeader.lenInWords += ( sizeof(packageBlobField_t) +
                        iv_sbeTraceBlob.field.fieldLen ) / sizeof(uint32_t);
        SBE_INFO(SBE_FUNC" Enable SBE_FFDC_TRACE_DATA with lenInWords[%d]",
                                      (uint32_t)iv_sbeTraceBlob.field.fieldLen);
    }
    if( iv_sbeDataHeader.dumpFields.get() & SBE_FFDC_ATTR_DATA )
    {
        // Package SBE Attr Data Blob with Field id and length
        uint32_t *startAddr = &_attrs_start_;
        uint32_t *endAddr = &_attrs_end_;
        uint32_t len = ALIGN_8_BYTES((uint32_t)endAddr-(uint32_t)startAddr);
        SBE_INFO(SBE_FUNC " Attr Blob Length is 0x%08x", len);
        iv_sbeAttrBlob.field.fieldId = (uint16_t)SBE_FFDC_ATTR_DATA;
        iv_sbeAttrBlob.field.fieldLen = len;
        iv_ffdcPackageHeader.lenInWords += ( sizeof(packageBlobField_t) +
                             iv_sbeAttrBlob.field.fieldLen ) / sizeof(uint32_t);
        SBE_INFO(SBE_FUNC" Enable SBE_FFDC_ATTR_DATA with lenInWords[%d]",
                                       (uint32_t)iv_sbeAttrBlob.field.fieldLen);
    }
    SBE_INFO(SBE_FUNC" SBE Trace and Attr  data in words[%d] ",
                                     uint32_t(iv_ffdcPackageHeader.lenInWords));

    return;
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

uint32_t SbeFFDCPackage::createHwPackage( uint32_t &o_wordsSent, const bool isFifoData, sbeFifoType i_type )
{
    #define SBE_FUNC "createHwPackage "
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t length = 0;
    //reset sent wordsSent
    o_wordsSent = 0;
    do
    {
        //Update package headers with dump fields configuration
        updateHWpackageDataHeader();

        // Create the fifo ostream class pointing to the scratch space values
        fapi2::sbefifo_hwp_data_ostream ffdcPakageStream(i_type);

        //Send FFDC package header
        length = sizeof(iv_ffdcPackageHeader) / sizeof(uint32_t);
        FIFO_BLOB_DATA_UPDATE(length, (uint32_t*)&iv_ffdcPackageHeader);
        o_wordsSent += length;

        // package fields word update
        uint32_t blobHeader = (uint32_t)iv_hwDataHeader.dumpFields.get();
        FIFO_BLOB_DATA_UPDATE( HW_HEADER_SIZE_IN_WORDS,
                               (uint32_t*)&blobHeader);
        o_wordsSent += HW_HEADER_SIZE_IN_WORDS;
        SBE_INFO(" packageFields Data: 0x%08X", iv_hwDataHeader.dumpFields.get());

        if( iv_hwLocalBlob.field.fieldId & SBE_FFDC_HW_DATA)
        {
            FIFO_BLOB_DATA_UPDATE( HW_HEADER_SIZE_IN_WORDS,
                                  (uint32_t*)&iv_hwLocalBlob.field);
            o_wordsSent += BLOB_HEADER_SIZE_IN_WORDS;
            SBE_INFO(SBE_FUNC" fields and PackageDataId.fieldLen [ 0x%08X ]",
                                                    iv_hwLocalBlob.field.get());
            if(iv_hwLocalBlob.field.fieldLen != 0)
            {
                length = iv_hwLocalBlob.field.fieldLen / sizeof(uint32_t);
                SBE_INFO(SBE_FUNC" SBE_FFDC_HW_DATA data length[%d]", length);
                FIFO_BLOB_DATA_UPDATE( length,
                                    (uint32_t*)(fapi2::g_FfdcData.ffdcDataPtr));
            }
        }
        o_wordsSent += length;

        if( iv_hwRegBlob.field.fieldId & SBE_FFDC_REG_DATA)
        {
            FIFO_BLOB_DATA_UPDATE( HW_HEADER_SIZE_IN_WORDS,
                                  (uint32_t*)&iv_hwRegBlob.field);
            o_wordsSent += BLOB_HEADER_SIZE_IN_WORDS;
            SBE_INFO(SBE_FUNC " SBE_FFDC_REG_DATA data length[0x%04X]",
                                         (uint16_t)iv_hwRegBlob.field.fieldLen);
            // @TODO Enable HW Reg data
        }
        SBE_INFO(SBE_FUNC " Number of words sent [%d], lenInWords[%d]",
                 o_wordsSent, iv_ffdcPackageHeader.lenInWords);

    } while(false);
    // Clean up all cratch data
    Heap::get_instance().scratch_free_all();
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

uint32_t SbeFFDCPackage::createSbePackage( uint32_t &o_bytesSent, const bool isFifoData, sbeFifoType i_type )
{
    #define SBE_FUNC "createSbePakage "
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t length = 0;
    //reset sent bytes
    o_bytesSent = 0;
    do
    {
        //Update package headers with dump fields configuration
        updateSBEpackageDataHeader();

        // Create the fifo ostream class pointing to the scratch space values
        fapi2::sbefifo_hwp_data_ostream ffdcPakageStream(i_type);

        //Send FFDC package header
        length = sizeof(iv_ffdcPackageHeader) / sizeof(uint32_t);
        FIFO_BLOB_DATA_UPDATE( length, (uint32_t*)&iv_ffdcPackageHeader );
        o_bytesSent += length;

        // package fields word update
        length = PLAT_HEADER_SIZE_IN_WORDS - BLOB_HEADER_SIZE_IN_WORDS;
        FIFO_BLOB_DATA_UPDATE( length, (uint32_t*)&iv_sbeDataHeader );
        SBE_INFO("packageFields Data: 0x%08X",iv_sbeDataHeader.dumpFields.get());
        o_bytesSent += length;

        uint32_t blobHeader = (uint32_t)iv_sbeDataHeader.dumpFields.get();
        FIFO_BLOB_DATA_UPDATE( BLOB_HEADER_SIZE_IN_WORDS,
                               (uint32_t*)&blobHeader);
        o_bytesSent += BLOB_HEADER_SIZE_IN_WORDS;

        if( iv_sbeTraceBlob.field.fieldId & SBE_FFDC_TRACE_DATA)
        {
            FIFO_BLOB_DATA_UPDATE( BLOB_HEADER_SIZE_IN_WORDS,
                               (uint32_t*)&iv_sbeTraceBlob.field );
            o_bytesSent += BLOB_HEADER_SIZE_IN_WORDS;
            SBE_INFO(SBE_FUNC " trace fields and PackageData length[0x%08X]",
                                                   iv_sbeTraceBlob.field.get());
            length = iv_sbeTraceBlob.field.fieldLen / sizeof(uint32_t);
            SBE_INFO(SBE_FUNC " SBE_FFDC_TRACE_DATA data length[%d]", length);
            FIFO_BLOB_DATA_UPDATE( length, (uint32_t*)(G_PK_TRACE_BUF));
        }
        o_bytesSent += length;

        if( iv_sbeAttrBlob.field.fieldId & SBE_FFDC_ATTR_DATA)
        {
            FIFO_BLOB_DATA_UPDATE( BLOB_HEADER_SIZE_IN_WORDS,
                               (uint32_t*)&iv_sbeAttrBlob.field );
            o_bytesSent += BLOB_HEADER_SIZE_IN_WORDS;

            SBE_INFO(SBE_FUNC "Attr field and PackageData length [0x%08X]",
                                                   iv_sbeAttrBlob.field.get());
            length = iv_sbeAttrBlob.field.fieldLen / sizeof(uint32_t);
            SBE_INFO(" SBE_FFDC_ATTR_DATA data length[%d]", length);
            FIFO_BLOB_DATA_UPDATE( length, (uint32_t *)&_attrs_start_);
        }
        SBE_INFO(SBE_FUNC " Number of words sent [%d], lenInWords[%d]",
                 o_bytesSent, iv_ffdcPackageHeader.lenInWords);
        o_bytesSent += length;
        SBE_INFO(" packageFields size[%d] ", o_bytesSent);
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

// TODO: This needs to be updated as part of FFDC story
// PFSBE-407
void captureAsyncFFDC(uint32_t primRc, uint32_t secRc)
{
    SBE_GLOBAL->failedPrimStatus = primRc;
    SBE_GLOBAL->failedSecStatus  = secRc;

    // Transition to dump state
    stateTransition(SBE_EVENT_CMN_DUMP_FAILURE);

    // Set async ffdc bit
    (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
}

uint32_t sendFFDCOverFIFO( const uint32_t i_fieldsConfig,
                           uint32_t &o_wordsSent,
                           const bool i_isFifoData, sbeFifoType i_type )
{
    #define SBE_FUNC "sendFFDCOverFIFO"
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        SBE_INFO(SBE_FUNC " Input FFDC Hw/Sbe Packages : 0x%08X", i_fieldsConfig);
        uint32_t wordsCount = 0;
        o_wordsSent = 0;
        if( i_fieldsConfig & SBE_FFDC_ALL_HW_DATA)
        {
            SBE_INFO(SBE_FUNC " Creating  ALL HW FFDC Package[%d]",
                     i_fieldsConfig & SBE_FFDC_ALL_HW_DATA);
            SbeFFDCPackage objFFDC(i_fieldsConfig & SBE_FFDC_ALL_HW_DATA);
            objFFDC.createHwPackage( wordsCount, i_isFifoData, i_type );
            o_wordsSent += wordsCount;
        }

        if( i_fieldsConfig & SBE_FFDC_ALL_PLAT_DATA)
        {
            SbeFFDCPackage objFFDC(i_fieldsConfig & SBE_FFDC_ALL_PLAT_DATA);
            SBE_INFO(SBE_FUNC " Creating  ALL SBE FFDC Pakage[%d]",
                     i_fieldsConfig & SBE_FFDC_ALL_PLAT_DATA);
            objFFDC.createSbePackage( wordsCount, i_isFifoData, i_type );
            o_wordsSent += wordsCount;
        }

    } while(false);
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

