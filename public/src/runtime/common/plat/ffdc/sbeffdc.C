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
#include "sbeutil.H"
#include "plat_hwp_data_stream.H"
#include <ffdc.H>
#include <error_info.H>

extern fapi2::pozFfdcData_t g_FfdcData;

void SbeFFDCPackage::updateHWpackageDataHeader(void)
{
    #define SBE_FUNC "updateHWpakageDataHeader"
    SBE_ENTER(SBE_FUNC);

    //As part of constructor ffdc package data header is updated with dump
    //fileds  and now update the length in ffdc package header base on required
    //fields
    iv_ffdcPackageHeader.lenInWords += HW_HEADER_SIZE_IN_WORDS;

    if( iv_hwDataHeader.dumpFields.get() & SBE_FFDC_HW_DATA )
    {
        // Package HW Data Blob with Field id and length
        iv_hwLocalBlob.field.fieldId = (uint16_t)SBE_FFDC_HW_DATA;
        iv_hwLocalBlob.field.fieldLen =
              ( fapi2::g_FfdcData.ffdcLength * (sizeof(fapi2::sbeFfdc_t)));
        iv_hwLocalBlob.blobPtr = NULL;
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

uint32_t SbeFFDCPackage::createHwPackage( uint32_t &o_wordsSent, const bool isFifoData )
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
        fapi2::sbefifo_hwp_data_ostream ffdcPakageStream;

        //Send FFDC package header
        length = sizeof(iv_ffdcPackageHeader) / sizeof(uint32_t);
        uint32_t *headerptr = (uint32_t*)&iv_ffdcPackageHeader;
        rc = ffdcPakageStream.put(length, headerptr);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
        o_wordsSent += length;

        // package fields word update
        uint32_t fields = iv_hwDataHeader.dumpFields.get();
        headerptr = (uint32_t*)&fields;
        rc = ffdcPakageStream.put(HW_HEADER_SIZE_IN_WORDS,headerptr);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
        o_wordsSent += HW_HEADER_SIZE_IN_WORDS;
        SBE_INFO(" packageFields Data: 0x%08X", iv_hwDataHeader.dumpFields.get());

        if( iv_hwLocalBlob.field.fieldId & SBE_FFDC_HW_DATA)
        {
            fields = iv_hwLocalBlob.field.get();
            headerptr = (uint32_t*)&fields;;
            rc = ffdcPakageStream.put(HW_HEADER_SIZE_IN_WORDS, headerptr);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
            o_wordsSent += BLOB_HEADER_SIZE_IN_WORDS;

            SBE_INFO(" fields and PackageDataId.fieldLen [ 0x%08X ]", fields);
            if(iv_hwLocalBlob.field.fieldLen != 0)
            {
                headerptr = (uint32_t*)(fapi2::g_FfdcData.ffdcDataPtr);
                length = iv_hwLocalBlob.field.fieldLen / sizeof(uint32_t);
                SBE_INFO(" SBE_FFDC_HW_DATA data length[%d]", length);
                rc = ffdcPakageStream.put(length, headerptr);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
            }
        }
        o_wordsSent += length;

        if( iv_hwRegBlob.field.fieldId & SBE_FFDC_REG_DATA)
        {
            fields = (uint32_t)iv_hwRegBlob.field.get();
            headerptr = (uint32_t*)&fields;
            rc = ffdcPakageStream.put(HW_HEADER_SIZE_IN_WORDS, headerptr);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
            o_wordsSent += BLOB_HEADER_SIZE_IN_WORDS;
            SBE_INFO(" SBE_FFDC_REG_DATA:PackageDataId.fieldLen [0x%08X ]", fields);
            // @TODO Enable HW Reg data
        }
        SBE_INFO(SBE_FUNC " Number of words sent [%d], lenInWords[%d]",
                 o_wordsSent, iv_ffdcPackageHeader.lenInWords);

    } while(false);
    // Clean up scratch data
    Heap::get_instance().scratch_free(fapi2::g_FfdcData.ffdcDataPtr);
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

uint32_t sendFFDCOverFIFO( const uint32_t i_fieldsConfig,
                           uint32_t &o_wordsSent,
                           const bool i_isFifoData )
{
    #define SBE_FUNC "sendOverFfdePackageToFifo"
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
            objFFDC.createHwPackage( wordsCount, i_isFifoData);
            o_wordsSent += wordsCount;;
        }

        if( i_fieldsConfig & SBE_FFDC_ALL_PLAT_DATA)
        {
            // @TODO Enable Sbe Ffdc Package 
            // JIRA: PFSBE-122
        }
        o_wordsSent = wordsCount;

    } while(false);

    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

