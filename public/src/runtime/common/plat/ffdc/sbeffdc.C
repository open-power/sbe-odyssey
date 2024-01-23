/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdc.C $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
#include "sbestatesutils.H"
#include "sberegaccess.H"

extern fapi2::pozFfdcData_t g_FfdcData;

namespace fapi2
{
    extern pozFfdcCtrl_t g_ffdcCtrlSingleton;
}

#define FIFO_BLOB_DATA_UPDATE( length, headerptr) \
    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(ffdcPakageStream.put(length, headerptr))

/**
 * @brief FFDC plat truncated trace size
 *        SBE_FFDC_TRUNCATED_TRACE_LENGTH for truncated trace len, and extra logic
 *        includes the trace header size @ref PkTraceBuffer
 *        plat_truncated_trace_size = sizeof (PkTraceBuffer) + truncatedTraceSize
*/
#define PLAT_FFDC_TRUNCATED_TRACE_SIZE  ((sizeof(*G_PK_TRACE_BUF) -        \
                                          sizeof(G_PK_TRACE_BUF->cb)) +    \
                                          SBE_FFDC_TRUNCATED_TRACE_LENGTH)

/**
 * @brief FFDC plat full size trace size (8k)
 *        which is include trace_header + trace_size(8K)
*/
#define FFDC_TRACE_FULL_SIZE ( sizeof(*G_PK_TRACE_BUF) -       \
                                sizeof(G_PK_TRACE_BUF->cb) +   \
                                G_PK_TRACE_BUF->size )


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
        iv_sbeTraceBlob.field.fieldLen = sizeof(*G_PK_TRACE_BUF) - sizeof(G_PK_TRACE_BUF->cb) + G_PK_TRACE_BUF->size;
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
    #define SBE_FUNC "sendFFDCOverFIFO "
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


/********************** FFDC utils functions *************************/
/**
 * @brief FFDC utils function for validate given address is with in scratch
 *        space. In case given address not with in scratch space executing
 *        pk_halt()
 *
 * @param i_addr pointer to the address
 */
static void ffdcUtils_checkScratchPtrCorruptionAndHalt (const void * i_addr)
{
    if ( !Heap::get_instance().is_scratch_pointer( i_addr ) )
    {
        SBE_ERROR ("ffdcUtils_checkScratchPtrCorruptionAndHalt Scratch space corrupted...!");
        pk_halt();
    }
}


#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Commit the given error and mask as commit.
 *        For null iv_data validating given RC and create ffdc for genuine RC
 *
 * @param io_rc  FAPI Return Code object
 */
void ffdcUtils_commitError ( fapi2::ReturnCode& io_rc )
{

    if (io_rc.getDataPtr() == 0)
    {
        uint32_t l_fapiRc = io_rc.getRC();
        if ( (l_fapiRc != fapi2::FAPI2_RC_SUCCESS)      &&
            ((l_fapiRc & fapi2::FAPI2_RC_PHAL_MASK)     ||
             (l_fapiRc & fapi2::FAPI2_RC_FAPI2_MASK)    ||
             (l_fapiRc & fapi2::FAPI2_RC_PLAT_MASK))
           )
        {
            void * ptr = NULL;
            io_rc.setDataPtr( ffdcConstructor( io_rc.getRC(), 0, ptr, 0, ptr) );
        }
        else
        {
            SBE_ERROR (SBE_FUNC "Lost the iv_dataPtr for given rc : 0x08X, "
                                "unable to log the error, executing PK_HALT()", l_fapiRc);
            pk_halt();
        }
    }

    pozFfdcNode_t * node = reinterpret_cast<pozFfdcNode_t*>(io_rc.getDataPtr());

    // marks as node is committed
    node->iv_isCommited = true;

    // Add node
    fapi2::g_ffdcCtrlSingleton.addNextNode (node);

    // Set async ffdc bit
    (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
}


void pozFfdcCtrl_t::addNextNode( const pozFfdcNode_t  *  i_node )
{
    pozFfdcNode_t * node = (pozFfdcNode_t *) iv_firstCommitted;
    if (node != nullptr)
    {
        while (node->next != nullptr)
        {
            ffdcUtils_checkScratchPtrCorruptionAndHalt ((const void *) node);
            node = node->next;
        }
        node->next = const_cast<pozFfdcNode_t *>(i_node);
    }
    else
    {
        iv_firstCommitted = const_cast<pozFfdcNode_t *>(i_node);
    }
}
#endif


/**
 * @brief Function to send plat ffdc with full trace
 *
 * @param[out] o_byteSent number of byte sent via fifo
 * @param[in] i_slidId slid id (sbe log identifier)
 * @param[in] io_putStream ostream reference
 *
 * @return secondary RC
*/
static uint32_t ffdcPlatCreateAndSendWithFullTrace (
                                    uint32_t &o_byteSent,
                                    uint16_t i_slidId,
                                    fapi2::errlSeverity_t i_sev,
                                    fapi2::sbefifo_hwp_data_ostream& io_putStream )
{
    #define SBE_FUNC "ffdcPlatCreateAndSendWithFullTrace "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    o_byteSent = 0;

    do
    {
        pozPlatFfdcPackageFormat_t platFfdc;

        /* Calculating the PLAT FFDC len */
        uint32_t ffdcLen = sizeof(pozPlatFfdcPackageFormat_t) +
                           sizeof(packageBlobField_t)         + /* FFDC fields size */
                           FFDC_TRACE_FULL_SIZE;                /* PK FULL trace size */

        // Updating the byte sent
        o_byteSent = ffdcLen;

        platFfdc.header.setLenInWord ( BYTES_TO_WORDS(ffdcLen) );
        platFfdc.header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        platFfdc.header.setSlid      ( i_slidId );
        platFfdc.header.setSeverity  ( i_sev );
        platFfdc.header.setChipId    ( 0 );
        platFfdc.header.setRc        ( fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA );

        platFfdc.platHeader.setRc         ( SBE_GLOBAL->failedPrimStatus, SBE_GLOBAL->failedSecStatus );
        platFfdc.platHeader.setfwCommitId ( SBE_COMMIT_ID );
        platFfdc.platHeader.setDdlevel    ( 0, 0 );
        platFfdc.platHeader.setThreadId   ( 0 );

        platFfdc.dumpFields        = SBE_FFDC_TRACE_DATA;

        l_rc = io_putStream.put( BYTES_TO_WORDS(sizeof(pozPlatFfdcPackageFormat_t)),
                                     (uint32_t *)&platFfdc );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        packageBlobField_t blobField;
        blobField.fieldId = SBE_FFDC_TRACE_DATA;
        blobField.fieldLen = FFDC_TRACE_FULL_SIZE;
        /* Streaming the Blob filed */
        l_rc = io_putStream.put( BYTES_TO_WORDS(sizeof(packageBlobField_t)),
                                        (uint32_t *) &blobField );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        /* Streaming the SBE trace */
        l_rc = io_putStream.put( BYTES_TO_WORDS(FFDC_TRACE_FULL_SIZE),
                                        (uint32_t*)(G_PK_TRACE_BUF));
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t sendFFDCOverFIFO( uint32_t &o_wordsSent,
                           sbeFifoType i_type,
                           bool i_forceFullTracePackage )
{
    #define SBE_FUNC "sendFFDCOverFIFO "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t byteSent = 0;

    do
    {
        // Create the fifo ostream class
        fapi2::sbefifo_hwp_data_ostream ffdcPakageStream(i_type);

        /* Check the first committed FFDC */
        pozFfdcNode_t * node = fapi2::g_ffdcCtrlSingleton.getHead();
        if (node != nullptr)
        {
            /* Get the address of first committed FFDC */
            pozFfdcNode_t * currentNode = (pozFfdcNode_t *) node;
            pozFfdcNode_t * nextNode = nullptr;
            do
            {
                SBE_DEBUG(SBE_FUNC " currentNode: 0x%08X", currentNode);
                ffdcUtils_checkScratchPtrCorruptionAndHalt((const void *)currentNode);

                uint32_t len = currentNode->iv_ffdcLen;
                l_rc = ffdcPakageStream.put((uint32_t) BYTES_TO_WORDS( len ),
                                        (uint32_t*) (((uint8_t *)currentNode) +
                                                    sizeof(pozFfdcNode_t)) );
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

                byteSent += len;

                /* Check FFDC is fatal, In case is fatal send the plat ffdc
                package with FULL trace with full ATTR dump */
                if (currentNode->iv_isFatal)
                {
                    SBE_DEBUG (SBE_FUNC "current node is fatal, streaming full trace");
                    i_forceFullTracePackage = false;

                    uint16_t slid                = fapi2::g_ffdcCtrlSingleton.iv_localSlid;
                    fapi2::errlSeverity_t sev    = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE;

                    // Plat data is available in current node which is RE enabled or
                    //  SBE internal error. the Apply slid and severity from plat
                    //  data to full trace
                    if (currentNode->iv_platSize)
                    {
                        pozPlatFfdcPackageFormat_t * platFfdc =
                                            (pozPlatFfdcPackageFormat_t *)
                                                (((uint8_t *)currentNode)   +
                                                    sizeof(pozFfdcNode_t)   +
                                                    currentNode->iv_hwpSize
                                                );

                        slid = platFfdc->header.slid;
                        sev  = static_cast<fapi2::errlSeverity_t>(platFfdc->header.severity);
                    }
                    // Only HWP local data available in current node which is
                    //  related to RE not enabled, then Apply slid and severity
                    //  from HWP local data to full trace
                    else if (currentNode->iv_hwpSize)
                    {
                        pozHwpFfdcPackageFormat_t * hwpFfdc =
                                            (pozHwpFfdcPackageFormat_t *)
                                                (((uint8_t *)currentNode)   +
                                                    sizeof(pozFfdcNode_t)
                                                );
                        slid = hwpFfdc->header.slid;
                        sev  = static_cast<fapi2::errlSeverity_t>(hwpFfdc->header.severity);
                    }

                    uint32_t tempByteSent = 0;
                    l_rc = ffdcPlatCreateAndSendWithFullTrace ( tempByteSent,
                                                                slid,
                                                                sev,
                                                                ffdcPakageStream );
                    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                    byteSent += tempByteSent;
                }

                /* Logic to get the next FFDC */
                nextNode = currentNode->next;
                SBE_INFO (SBE_FUNC "Scratch free addr: 0x%08X, Changing head to 0x%08X", currentNode, nextNode);
                Heap::get_instance().scratch_free((const void*) currentNode);
                currentNode = nextNode;

            }while (currentNode != nullptr);
        }
        /* Check RC and Break */
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // This is for Get FFDC chipop, in case no FFDC are present in scratch
        if ( i_forceFullTracePackage )
        {
            uint32_t tempByteSent = 0;
            l_rc = ffdcPlatCreateAndSendWithFullTrace ( tempByteSent,
                                                ++fapi2::g_ffdcCtrlSingleton.iv_localSlid, // applying incremented slid id
                                                fapi2::FAPI2_ERRL_SEV_UNDEFINED,           // severity undefined
                                                ffdcPakageStream );
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            byteSent += tempByteSent;
        }
        o_wordsSent += BYTES_TO_WORDS(byteSent);

    } while(false);

    /* clearing the first commited FFDC */
    fapi2::g_ffdcCtrlSingleton.setHead((pozFfdcNode_t *) NULL);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief FFDC HWP data initialization
 *        This function used to update the hwp Respose Header except hwp local
 *        data and hwp reg data. HWP local data will set by set method in
 *        generated RC and HWP reg data updated by auto gen code
 *
 * Below figure is HWP FFDC data frame format and mapped to param of the function
 * +----------+----------+----------+----------+
 * | Byte 0   |  Byte 1  |  Byte 2  |  Bytes 3 |
 * +----------+----------+----------+----------+ --> @ref param i_hwpAddr
 * | Magic Bytes 0xFBAD  |  Len in words       |          start address of the HWP FFDC
 * +----------+----------+----------+----------+
 * |       Seq ID        | Cmd class|    Cmd   |
 * +----------+----------+----------+----------+
 * |        SLID         | Severity |  Chip ID | --> @ref param i_slidId & i_sev
 * +----------+----------+----------+----------+
 * |             FAPI RC ( HWP )               | --> @ref param i_rc
 * +----------+----------+----------+----------+
 * |           HWP FFDC Dump Fields            |
 * +----------+----------+----------+----------+
 * |     Filed ID 0      |   Field ID 0 Length | --> @ref param i_hwpLocalDataLen
 * +----------+----------+----------+----------+ --> @ref param o_hwpLocalDataStartAddr (start address of hwp local data)
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                  .......                  |  |--> HWP local data len
 * +----------+----------+----------+----------+  |
 * |         Field Data N (size 1, data 2)     | -
 * +----------+----------+----------+----------+
 * |      Filed ID 1     |  Field ID 1 Length  | --> @ref param i_hwpRegDataLen
 * +----------+----------+----------+----------+ --> @ref param o_hwpRegDataStartAddr (start address of hwp reg data)
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                   .......                 |  |-> HWP reg data len
 * +----------+----------+----------+----------+  |
 * |        Field Data N (size 1, data 2)      | -
 * +----------+----------+----------+----------+
 *
 * @param[in] i_hwpAddr HWP local and HWP reg FFDC data start address including
 *                      Respose Header, caller should allocate full frame before
 *                      calling this function and pass the pointer here.
 * @param[in] i_ffdcLen FFDC length in bytes
 * @param[in] i_rc      fapi RC which is assign to the FFDC Respose Header
 * @param[in] i_slidId  slid ID (sbe log identifier)
 * @param[in] i_dumpFields HWP FFDC dump field
 * @param[in] i_hwpLocalDataLen HWP local ffdc data length in bytes, which
 *                      is passed from auto gen code (hwp_ffdc_classes.H). which
 *                      is used to allocate the scratch space.
 * @param[out] o_hwpLocalDataStartAddr HWP local ffdc data start address, were
 *                      using address auto gen code set method will update the
 *                      HWP local data.
 * @param[in] i_hwpRegDataLen HWP reg ffdc data length in bytes, which is passed
 *                      from auto gen code (hwp_ffdc_classes.H). which is used
 *                      to allocate the scratch space.
 * @param[out] o_hwpRegDataStartAddr HWP Ref ffdc data start address, were
 *                      using address auto gen code set method will update the
 *                      HWP local data.
 * @param[in] i_sev     severity of the error,
 *                      default FAPI2_ERRL_SEV_UNRECOVERABLE
 *
*/
static void ffdcInitHwpData( const pozHwpFfdcPackageFormat_t * i_hwpAddr,
                             const uint16_t i_ffdcLen,
                             uint32_t i_rc,
                             uint16_t i_slidId,
                             uint16_t i_hwpLocalDataLen,
                             void *&  o_hwpLocalDataStartAddr,
                             uint16_t i_hwpRegDataLen,
                             void *&  o_hwpRegDataStartAddr,
                             fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE)
{
    #define SBE_FUNC "ffdcInitHwpData "
    SBE_ENTER(SBE_FUNC);

    pozHwpFfdcPackageFormat_t * hwpAddr = (pozHwpFfdcPackageFormat_t *) i_hwpAddr;

    if (hwpAddr)
    {
        hwpAddr->header.setMagicbytes(SBE_FFDC_MAGIC_BYTES);
        hwpAddr->header.setLenInWord ( BYTES_TO_WORDS(i_ffdcLen) );
        hwpAddr->header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        hwpAddr->header.setSlid      ( i_slidId );
        hwpAddr->header.setSeverity  ( i_sev );
        hwpAddr->header.setChipId    ( 0 );
        hwpAddr->header.setRc        ( i_rc );

        hwpAddr->dumpFields = 0;

        if (i_hwpLocalDataLen)
        {
            hwpAddr->dumpFields        = SBE_FFDC_HW_DATA;

            // calculating the field data ptr
            packageBlobField_t * packetPtr = (packageBlobField_t *)
                        (((uint8_t *)i_hwpAddr) + sizeof(pozHwpFfdcPackageFormat_t));

            packetPtr->setFields( (uint16_t) SBE_FFDC_HW_DATA,
                                (uint16_t) i_hwpLocalDataLen );
            // calculating the hwp local data start ptr
            o_hwpLocalDataStartAddr = (void *) ( ((uint8_t *)i_hwpAddr)    +
                                            sizeof(pozHwpFfdcPackageFormat_t) +
                                            sizeof(packageBlobField_t) );
        }

        // Checking the HWP reg data dump fields
        if ( i_hwpRegDataLen )
        {
            hwpAddr->dumpFields |= SBE_FFDC_REG_DATA;
            packageBlobField_t * packetPtr = (packageBlobField_t *) ( ((uint8_t *)i_hwpAddr)           +
                                                 sizeof(pozHwpFfdcPackageFormat_t)+
                                                 sizeof(packageBlobField_t)       +
                                                 i_hwpLocalDataLen );
            packetPtr->setFields( (uint16_t) SBE_FFDC_REG_DATA,
                                  (uint16_t) i_hwpRegDataLen );

            // calculating the hwp reg data start ptr
            o_hwpRegDataStartAddr = (void *)( ((uint8_t *)i_hwpAddr)           +
                                              sizeof(pozHwpFfdcPackageFormat_t)+
                                              sizeof(packageBlobField_t)       +
                                              i_hwpLocalDataLen                +
                                              sizeof(packageBlobField_t) );
        }
    }
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

#if defined( MINIMUM_FFDC_RE )
/**
 * @brief FFDC initialization plat ffdc data
 *        This function used to update the plat Response Header including the
 *        truncated trace to given space.
 *
 * Below figure is PLAT FFDC data frame format and mapped to param of the function
 * +----------+----------+----------+----------+
 * | Byte 0   |  Byte 1  |  Byte 2  |  Bytes 3 |
 * +----------+----------+----------+----------+ --> @ref param i_platAddr (start address of the PLAT FFDC)
 * | Magic Bytes 0xFBAD  |  Len in words       |
 * +----------+----------+----------+----------+
 * |       Seq ID        | Cmd class|    Cmd   |
 * +----------+----------+----------+----------+
 * |        SLID         | Severity |  Chip ID | --> @ref param i_slidId & i_sev
 * +----------+----------+----------+----------+
 * |   FAPI RC ( FAPI2_RC_PLAT_ERR_SEE_DATA )  |
 * +----------+----------+----------+----------+
 * |    Primary Status   |  Secondary Status   | --> @ref param i_primRc & i_secRc
 * +----------+----------+----------+----------+
 * |              FW Commit ID                 |
 * +----------+----------+----------+----------+
 * | Reserved | DD Major | DD Minor | Thread ID|
 * +----------+----------+----------+----------+
 * |           HWP FFDC Dump Fields            |
 * +----------+----------+----------+----------+
 * |     Filed ID 0      |   Field ID 0 Length |
 * +----------+----------+----------+----------+
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                  .......                  |  |--> PLAT ffdc truncated trace data
 * +----------+----------+----------+----------+  |
 * |         Field Data N (size 1, data 2)     | -
 * +----------+----------+----------+----------+
 * |      Filed ID 1     |  Field ID 1 Length  |
 * +----------+----------+----------+----------+
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                   .......                 |  |-> PLAT ffdc attr dump
 * +----------+----------+----------+----------+  |
 * |        Field Data N (size 1, data 2)      | -
 * +----------+----------+----------+----------+
 *
 * @param[in] i_platAddr Start address of the PLAT FFDC, caller should allocate
 *                       full frame before calling this function and pass the
 *                       pointer here.
 * @param[in] i_ffdcLen  FFDC length in bytes
 * @param[in] i_slidId   slid ID (sbe log identifier)
 * @param[in] i_primRc   primary RC
 * @param[in] i_secRc    secondary RC
 * @param[in] i_sev      severity of the error,
 *                       default FAPI2_ERRL_SEV_UNRECOVERABLE
 *
*/
static void ffdcInitPlatData( const pozPlatFfdcPackageFormat_t * i_platAddr,
                              const uint16_t i_ffdcLen,
                              uint16_t i_slidId,
                              uint16_t i_primRc,
                              uint16_t i_secRc,
                              fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE )
{
    pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *) i_platAddr;
    if (platAddr)
    {
        // Assigning the PLAT ffdc response header
        platAddr->header.setMagicbytes( SBE_FFDC_MAGIC_BYTES );
        platAddr->header.setLenInWord ( BYTES_TO_WORDS(i_ffdcLen) );
        platAddr->header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        platAddr->header.setSlid      ( i_slidId );
        platAddr->header.setSeverity  ( i_sev );
        platAddr->header.setChipId    ( 0 );
        platAddr->header.setRc        ( fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA );

        platAddr->platHeader.setRc         ( i_primRc, i_secRc );
        platAddr->platHeader.setfwCommitId ( SBE_COMMIT_ID );
        platAddr->platHeader.setDdlevel    ( 0, 0 );
        platAddr->platHeader.setThreadId   ( 0 );
        platAddr->dumpFields = 0;

        if ( i_ffdcLen > sizeof(pozPlatFfdcPackageFormat_t) )
        {
            platAddr->dumpFields = SBE_FFDC_TRACE_DATA;

            // Calculating the address to trace blobfield
            packageBlobField_t * blobField = (packageBlobField_t *)
                                            ( ((uint8_t *)i_platAddr)  +
                                                sizeof(pozPlatFfdcPackageFormat_t) );
            blobField->setFields( (uint16_t) SBE_FFDC_TRACE_DATA,
                                    (uint16_t) PLAT_FFDC_TRUNCATED_TRACE_SIZE );
        }
    }
}
#endif


uint32_t ffdcConstructor ( uint32_t i_rc,
                           uint16_t i_hwpLocalDataLen,
                           void *&  o_hwpLocalDataStartAddr,
                           uint16_t i_hwpRegDataLen,
                           void *&  o_hwpRegDataStartAddr
                         )
{
    #define SBE_FUNC "ffdcConstructor "
    SBE_ENTER(SBE_FUNC);
    uint32_t ffdcPlatSize = 0;
    uint32_t ffdcHwpSize  = 0;

    /**
     * Calculating the FFDC size to allocate the heap
     * refer diagram in @file sbeffdctype.H (figure 1 and 2)
    */
    ffdcHwpSize = sizeof(pozHwpFfdcPackageFormat_t)+ /* hwp ffdc Frame size */
                  sizeof(packageBlobField_t)       + /* HWP local data fields size */
                  i_hwpLocalDataLen                + /* HWP local data length */
                  ((i_hwpRegDataLen)? sizeof(packageBlobField_t) : 0) + /* HWP Reg data field size */
                  i_hwpRegDataLen                  ; /* HWP reg data length */

#if defined( MINIMUM_FFDC_RE )

    ffdcPlatSize = sizeof(pozPlatFfdcPackageFormat_t); /* Plat FFDC size (plat data started) */
                //    sizeof(packageBlobField_t)        + /* FFDC fields size */ // TODO: need to enable once truncated trace enabled
                //    PLAT_FFDC_TRUNCATED_TRACE_SIZE;     /* Truncated Trace Len */
                   /* size of (PkTraceBuffer) - size of trace buffer = size of PkTraceBuffer except trace buffer size*/

#endif

    uint32_t ffdcLen = ffdcHwpSize           + /* HWP size */
                       ffdcPlatSize;           /* Plat size */

#if !defined( MINIMUM_FFDC_RE )
    // Note: For MINIMUM_FFDC, MINIMUM_FFDC_RE not defined by default Unrecoverable
    //         error enable.
    if (fapi2::g_ffdcCtrlSingleton.getHead() != nullptr)
    {
        Heap::get_instance().scratch_free((const void*) fapi2::g_ffdcCtrlSingleton.getHead());

    }
#endif

    /* Allocation scratch space */
    pozFfdcNode_t * currentNode  = (pozFfdcNode_t *) Heap::get_instance().
                                scratch_alloc( ffdcLen + sizeof(pozFfdcNode_t) );
    // Check for Scratch is allocation
    if (currentNode)
    {
        SBE_DEBUG (SBE_FUNC "currentNode: 0x%08X", currentNode);
        // Updating FFDC node status
        currentNode->set ((uint16_t)ffdcLen, false, false, ffdcHwpSize, ffdcPlatSize);
        currentNode->next = NULL;

#if defined( MINIMUM_FFDC_RE )
        // Incrementing SBE log ID, Identical slid ID for particular FFDC
        //  this may roll-over after 2^16, but its ok, since we can safely
        //  assume by that time the first ffdc with slid-id = 0, might have
        //  streamed back
        fapi2::g_ffdcCtrlSingleton.incrementSlid();
#else
        // Note: For not defined MINIMUM_FFDC_RE, Directly assign allocated scratch
        //        address to iv_firstCommitedFfdc and mark error as commited and
        //        fatal error
        currentNode->iv_isCommited = true;
        currentNode->iv_isFatal    = true;

        /* Add node at last */
        fapi2::g_ffdcCtrlSingleton.setHead( currentNode );
#endif

        // HWP FFDC data pointer
        pozHwpFfdcPackageFormat_t * hwpFfdcPtr = ( pozHwpFfdcPackageFormat_t * )
                               (((uint8_t *)currentNode) + sizeof(pozFfdcNode_t));

#if defined( MINIMUM_FFDC_RE )
        // Plat FFDC data pointer
        pozPlatFfdcPackageFormat_t * platFfdcPtr = (pozPlatFfdcPackageFormat_t * )
                                                      ( ((uint8_t *)hwpFfdcPtr) +
                                                        ffdcHwpSize
                                                      ) ;
#endif
        // Calling HWP data initialization function
        ffdcInitHwpData( hwpFfdcPtr,
                         ffdcHwpSize,
                         i_rc,
                         fapi2::g_ffdcCtrlSingleton.iv_localSlid,
                         i_hwpLocalDataLen,
                         o_hwpLocalDataStartAddr,
                         i_hwpRegDataLen,
                         o_hwpRegDataStartAddr );

#if defined( MINIMUM_FFDC_RE )
        // Calling PLAT FFDC initialization function
        ffdcInitPlatData( platFfdcPtr,
                          ffdcPlatSize,
                          fapi2::g_ffdcCtrlSingleton.iv_localSlid,
                          SBE_PRI_GENERIC_EXECUTION_FAILURE,
                          SBE_SEC_HWP_FAILURE,
                          fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE
                        );
#endif
    }
    else
    {
        // failed to allocate scratch space
        SBE_ERROR (SBE_FUNC "failed to allocate scratch space, executing PK_HALT()");
        pk_halt();
    }

    SBE_EXIT(SBE_FUNC);
    return reinterpret_cast<uint32_t>(currentNode);
    #undef SBE_FUNC
}

#if defined( MINIMUM_FFDC_RE )
void logSbeError( const uint16_t i_primRc,
                  const uint16_t i_secRc,
                  fapi2::errlSeverity_t i_sev,
                  bool i_isFatal
                )
{
    uint32_t ffdcLen = sizeof(pozPlatFfdcPackageFormat_t); /* Plat ffdc header + plat header(commit ID + DD level) + Dump field size */
    //             sizeof(packageBlobField_t)      + /* FFDC fields size */ // TODO: need to enable once truncated trace enabled
    //             PLAT_FFDC_TRUNCATED_TRACE_SIZE ;  /* Trace data size  */

    pozFfdcNode_t * currentNode = (pozFfdcNode_t *) Heap::get_instance().
                               scratch_alloc( ffdcLen + sizeof(pozFfdcNode_t) );
    if (currentNode)
    {
        // Updating the FFDC node status, Committing the created node
        currentNode->set ((uint16_t)ffdcLen, true, i_isFatal, 0, ffdcLen);
        currentNode->next = NULL;

        // Incrementing SBE log ID, Identical slid ID for particular FFDC
        fapi2::g_ffdcCtrlSingleton.incrementSlid();

        pozPlatFfdcPackageFormat_t * platFfdcPtr = (pozPlatFfdcPackageFormat_t *)
                                                      ( ((uint8_t *)currentNode) +
                                                        sizeof(pozFfdcNode_t)
                                                      );

        ffdcInitPlatData( platFfdcPtr,
                          ffdcLen,
                          fapi2::g_ffdcCtrlSingleton.iv_localSlid,
                          i_primRc,
                          i_secRc,
                          i_sev
                        );

        /* Add node at last */
        fapi2::g_ffdcCtrlSingleton.addNextNode(currentNode);

        // Set async ffdc bit
        (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
    }
    else
    {
        // Failed to allocate scratch
        SBE_ERROR ("logSbeError failed to allocate scratch space, executing PK_HALT()");
        pk_halt();
    }
}
#endif


void logFatalError( fapi2::ReturnCode& i_rc )
{
#if defined(MINIMUM_FFDC_RE)

    ffdcUtils_commitError( i_rc );

    pozFfdcNode_t * currentNode = (pozFfdcNode_t *) i_rc.getDataPtr();

    // Mark Error as a FATAL
    currentNode->iv_isFatal    = true;

    // Note: Ideally iv_rc also need to be cleared. but not clearing since,
    //       it will break some of the existing flow where sbe functions
    //       which is calling SBE_EXEC_HWP is returning fapi-rc.
    i_rc.setDataPtr ( NULL );
#endif
}

#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Update FFDC severity to created FFDC node. Will identify the package
 *        (HWP/PLAT) and update severity
 *
 * @param i_nodeAddr Node address
 * @param i_sev      severity of the error, default FAPI2_ERRL_SEV_UNRECOVERABLE
 */
static void ffdcUpdateSeverity ( const pozFfdcNode_t * i_nodeAddr,
                          fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE)
{
    pozFfdcNode_t * node = const_cast<pozFfdcNode_t*>(i_nodeAddr);

    /* Check the node contain HWP local data */
    if (node->iv_hwpSize)
    {
        // Get the HWP ffdc package start address
        pozHwpFfdcPackageFormat_t * hwpAddr = (pozHwpFfdcPackageFormat_t *)
                                ( ((uint8_t *)node) + sizeof (pozFfdcNode_t));
        // Update severity
        hwpAddr->header.setSeverity( i_sev );
    }

    /* Check the node contain Plat data */
    if (node->iv_platSize)
    {
        // Get the plat ffdc package start address
        pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *)
                                                     ( ((uint8_t *)node)      +
                                                       sizeof (pozFfdcNode_t) +
                                                       node->iv_hwpSize
                                                     );
        // Update severity
        platAddr->header.setSeverity( i_sev );
    }
}
#endif

namespace fapi2
{

void logError( fapi2::ReturnCode& io_rc,
               fapi2::errlSeverity_t i_sev,
               bool i_unitTestError )
{

#if defined(MINIMUM_FFDC_RE)

    ffdcUtils_commitError( io_rc );

    pozFfdcNode_t * node = reinterpret_cast<pozFfdcNode_t*>(io_rc.getDataPtr());
    if (node != nullptr)
    {
        // update severity
        ffdcUpdateSeverity ( node, i_sev );

        // clear rc
        io_rc.setDataPtr ( NULL );
        io_rc.setRC ( FAPI2_RC_SUCCESS );
    }
    else
    {
        SBE_ERROR (SBE_FUNC "Lost the iv_dataPtr unable to log the error, "
                            "executing PK_HALT()");
        pk_halt();
    }
#else
    SBE_ERROR ("logError Logging RC=0x%08X, with severity=%d", io_rc.getRC(), i_sev);
    io_rc.setRC ( FAPI2_RC_SUCCESS );
#endif

}

};
