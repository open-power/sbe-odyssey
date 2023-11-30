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
#define PLAT_FFDC_TRUNCATED_TRACE_SIZE  ((sizeof(*G_PK_TRACE_BUF) - \
                                          sizeof(G_PK_TRACE_BUF->cb)) + \
                                          SBE_FFDC_TRUNCATED_TRACE_LENGTH)


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
static void ffdcUtils_isAddrWithinScratchSpace (const void * i_addr)
{
    if ( !Heap::get_instance().is_scratch_pointer( i_addr ) )
    {
        SBE_ERROR (SBE_FUNC "Scratch space corrupted...!" );
        pk_halt();
    }
}


pozFfdcNode_t * pozFfdcCtrl_t::getLastNode( void )
{
    pozFfdcNode_t * node = iv_firstCommitted;
    ffdcUtils_isAddrWithinScratchSpace ( (const void *) node);

    if (node != nullptr)
    {
        while (node->next != nullptr)
        {
            node = node->next;
            ffdcUtils_isAddrWithinScratchSpace ( (const void *) node);
        }
    }
    return node;
}


void pozFfdcCtrl_t::addNextNode( const pozFfdcNode_t  *  i_node )
{
    #define SBE_FUNC "pozFfdcCtrl_t::addNextNode "
    SBE_ENTER(SBE_FUNC);

    pozFfdcNode_t * node = (pozFfdcNode_t *) iv_firstCommitted;
    if (node != nullptr)
    {
        node = getLastNode ( );
        node->next = const_cast<pozFfdcNode_t *>(i_node);
    }
    else
    {
        iv_firstCommitted = const_cast<pozFfdcNode_t *>(i_node);
    }

    SBE_EXIT(SBE_FUNC);
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

        // Checking the HWP reg data dump fields
        if ( i_hwpRegDataLen )
        {
            hwpAddr->dumpFields |= SBE_FFDC_REG_DATA;
            packetPtr = (packageBlobField_t *) ( ((uint8_t *)i_hwpAddr)           +
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
    #define SBE_FUNC "ffdcInitPlatData "
    SBE_ENTER(SBE_FUNC);

    pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *) i_platAddr;
    if (platAddr)
    {
        // Assigning the PLAT ffdc response header
        platAddr->header.setMagicbytes(SBE_FFDC_MAGIC_BYTES);
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

        platAddr->dumpFields = SBE_FFDC_TRACE_DATA;

        // Calculating the address to trace blobfield
        packageBlobField_t * blobField = (packageBlobField_t *)
                                        ( ((uint8_t *)i_platAddr)  +
                                            sizeof(pozPlatFfdcPackageFormat_t) );
        blobField->setFields( (uint16_t) SBE_FFDC_TRACE_DATA,
                                (uint16_t) PLAT_FFDC_TRUNCATED_TRACE_SIZE );

        // Calculating the truncated trace start address
        uint8_t * traceBufferPtr = (uint8_t *) ((uint8_t *)i_platAddr)         +
                                            sizeof(pozPlatFfdcPackageFormat_t) +
                                            sizeof(packageBlobField_t) ;
        /* TODO: Need to check this is correct way to copy trace */
        memcpy ( (uint8_t *) traceBufferPtr,
                    (uint8_t *) G_PK_TRACE_BUF,
                    PLAT_FFDC_TRUNCATED_TRACE_SIZE  );
    }
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
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
                  sizeof(packageBlobField_t)      + /* HWP local data fields size */
                  i_hwpLocalDataLen               + /* HWP local data length */
                  ((i_hwpRegDataLen)? sizeof(packageBlobField_t) : 0) + /* HWP Reg data field size */
                  i_hwpRegDataLen                 ; /* HWP reg data length */

#if defined( MINIMUM_FFDC_RE )

    ffdcPlatSize = sizeof(pozPlatFfdcPackageFormat_t)+ /* Plat FFDC size (plat data started) */
                   sizeof(packageBlobField_t)        + /* FFDC fields size */
                   PLAT_FFDC_TRUNCATED_TRACE_SIZE;   /* Truncated Trace Len */
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
                               ((uint8_t *)currentNode) + sizeof(pozFfdcNode_t);

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
    #define SBE_FUNC "logError "
    SBE_ENTER(SBE_FUNC);

#if defined(MINIMUM_FFDC_RE)

    pozFfdcNode_t * node = reinterpret_cast<pozFfdcNode_t*>(io_rc.getDataPtr());
    if (node != NULL)
    {
        // marks as node is committed
        node->iv_isCommited = true;

        // update severity
        ffdcUpdateSeverity ( node, i_sev );

        // Add node
        fapi2::g_ffdcCtrlSingleton.addNextNode (node);

        // Set async ffdc bit
        (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);

        // clear rc
        io_rc.setDataPtr ( NULL );
        io_rc.setRC ( FAPI2_RC_SUCCESS );
    }
    else
    {
        // In case on error pk_halt()
        SBE_ERROR (SBE_FUNC "Lost the iv_dataPtr can't able to log the error, "
                            "executing PK_HALT()");
        pk_halt();
    }
#else
    SBE_ERROR (SBE_FUNC "Recoverable error not support");
#endif

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

};
