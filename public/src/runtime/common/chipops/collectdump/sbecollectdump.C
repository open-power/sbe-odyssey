/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/collectdump/sbecollectdump.C $ */
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

#include "sbecollectdump.H"
#include "sbedumpconstants.H"
#include "sbecollectdumpwrap.H"
#include "poz_gettracearray.H" // For Trace-array dump support header
#include "sbecmdtracearray.H"  // For Trace-array dump support header
#include "sbecmdstopclocks.H"  // For Stopclock dump support header
#include "sbecmdringaccess.H"  // For Rings dump support header
#include "sbecmdfastarray.H"   // For Fast-array support header
#include "poz_fastarray.H"
#include "sbeglobals.H"


#define PRI_SEC_RC_UNION(primary_rc, secondary_rc) primary_rc<<16 | secondary_rc


inline bool sbeCollectDump::isDumpTypeMapped()
{
    return (iv_hdctRow->genericHdr.dumpContent & iv_hdctDumpTypeMap);
}


////////////////////////////////////////////////////////////////////////////////
/// SBE get DUMP - HDCT PAK section details functions
////////////////////////////////////////////////////////////////////////////////
// Constructor
hdctPakSecDetails::hdctPakSecDetails()
{
    size_t imageSize = 0;

    // allocate  scratch area for hdct binary
    // TODO rc failure case not handled here need to handle
    //      Jira: PFSBE-533 Handle the Error condition in
    //            hdctPakSecDetails::hdctPakSecDetails() constructor
    fapi2::ReturnCode rc = fapi2::plat_loadEmbeddedFile(hdct_binary_fname,
                                                        (const void *&) iv_startAddr, imageSize);
    if(rc == fapi2::FAPI2_RC_SUCCESS)
    {
        // get the start address using loadEmbeddedFile api
        iv_endAddr = iv_startAddr + imageSize;
        //Set currAddr to start of HDCT row
        iv_currAddr = iv_startAddr + HDCT_BIN_COMMIT_ID_SIZE;
        //Dump start after EKB CommitID
        SBE_INFO("hdctPakSecDetails:: Start Offset: [0x%08X] Size: [0x%08X] End offset: [0x%08X]",
                                          iv_startAddr, imageSize, iv_endAddr );
    }
}

// Destructor
hdctPakSecDetails::~hdctPakSecDetails()
{
    // releasing allocated space for hdct binary
    if (iv_startAddr)
    {
        fapi2::freeEmbeddedFile((void *)iv_startAddr);
    }
    iv_startAddr = NULL;
}


/********************* Types of Dump Packets to FIFO START ****************************/

////////////////////////////////////////////////////////////////////////////////
/// SBE get DUMP - Write trace-array data to FIFO
////////////////////////////////////////////////////////////////////////////////
uint32_t sbeCollectDump::writeGetTracearrayPacketToFifo()
{
    #define SBE_FUNC " writeGetTracearrayPacketToFifo: "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Set FFDC failed command information and
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo( 0, SBE_CMD_CLASS_ARRAY_ACCESS, SBE_CMD_CONTROL_TRACE_ARRAY );

        // Update address, length and stream header data via FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdTraceArray.strEqvHash32;
        uint32_t len = sizeof( iv_tocRow.hdctHeader ) / sizeof( uint32_t );
        if( !iv_tocRow.tgtHndl.getFunctional() )
        {
            // Update non functional state DUMP header
            iv_tocRow.hdctHeader.preReq     = PRE_REQ_NON_FUNCTIONAL;
            iv_tocRow.hdctHeader.dataLength = 0x00;
            l_rc = iv_oStream.put( len, (uint32_t*)&iv_tocRow.hdctHeader );
            SBE_DEBUG( "DUMP GETTRACEARRAY: NonFunctional Target UnitNum[0x%08X]",
                    (uint32_t)iv_tocRow.hdctHeader.chipUnitNum );
            break;
        }

        // The size of data streamed from SBE is irrespective of trace ID
        // and it is 128*16*8 bits. [ PROC_TRACEARRAY_MAX_SIZE ]
        iv_tocRow.hdctHeader.dataLength = PROC_TRACEARRAY_MAX_SIZE * 8;
        l_rc = iv_oStream.put( len, (uint32_t*)&iv_tocRow.hdctHeader );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // Trace array request message packet
        sbeControlTraceArrayCmd_t reqMsg = {};
        len  = sizeof( sbeControlTraceArrayCmd_t )/sizeof( uint32_t );

        reqMsg.iv_traceArrayId = iv_hdctRow->cmdTraceArray.traceArrayID;
        reqMsg.iv_operation    =    SBE_TA_OPERATION_RESTART            |
                                    SBE_TA_OPERATION_STOP               |
                                    SBE_TA_OPERATION_COLLECT_DUMP;

        SBE_INFO(SBE_FUNC "traceArrayId [0x%04X] operation [0x%04X]",
                        reqMsg.iv_traceArrayId, reqMsg.iv_operation);

        sbefifo_hwp_data_istream istreamDump( iv_fifoType, len,
                                        (uint32_t*)&reqMsg, false );
        uint32_t startCount = iv_oStream.words_written();

        // Calling trace array HWP
        l_rc = sbeControlTraceArrayWrap( istreamDump, iv_oStream );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // Calculate the word written to fifo and len
        uint32_t endCount          = iv_oStream.words_written();
        uint32_t totalCountInBytes = iv_tocRow.hdctHeader.dataLength / 8;
        uint32_t totalCount        = totalCountInBytes / (sizeof(uint32_t));
        uint32_t dummyData         = 0x00;

        /// If endCount = startCount means chip-op failed. We will write dummy data.
        if(endCount == startCount || ((endCount - startCount) != totalCount))
        {
            totalCount = totalCount - (endCount - startCount);
            while(totalCount !=0)
            {
                l_rc = iv_oStream.put(dummyData);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

                totalCount = totalCount - 1;
            }
        }
    }while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t sbeCollectDump::writeGetRingPacketToFifo()
{
    #define SBE_FUNC " writeGetRingPacketToFifo "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do{
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo(0, SBE_CMD_CLASS_RING_ACCESS, SBE_CMD_GETRING);

        // Update address, length and stream header data via FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdGetRing.strEqvHash32;
        uint32_t len = sizeof(iv_tocRow.hdctHeader) / sizeof(uint32_t);
        if( !iv_tocRow.tgtHndl.getFunctional() )
        {
            // Update non functional state DUMP header
            iv_tocRow.hdctHeader.preReq     = PRE_REQ_NON_FUNCTIONAL;
            iv_tocRow.hdctHeader.dataLength = 0x00;
            l_rc = iv_oStream.put( len, (uint32_t*)&iv_tocRow.hdctHeader );
            SBE_DEBUG( "DUMP GETRING: NonFunctional Target UnitNum[0x%08X]",
                    (uint32_t)iv_tocRow.hdctHeader.chipUnitNum );
            break;
        }

        uint32_t bitlength = iv_hdctRow->cmdGetRing.ringLen;
        //Stream out the actual ring length.
        iv_tocRow.hdctHeader.dataLength = bitlength;
        //Dummy data length to be streamed out in case of FFDC.Keep it 8byte
        //alligned based on ring length as ring chip-op streams out data 8byte
        //aligned.
        uint32_t dummyDataLengthInBits = 64 * (((uint32_t)(bitlength / 64)) +
                                        ((uint32_t)(bitlength % 64) ? 1:0 ));
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        sbeGetRingAccessMsgHdr_t l_reqMsg;
        len  = sizeof(sbeGetRingAccessMsgHdr_t)/sizeof(uint32_t);
        uint32_t translatedAddress = 0;
        getAbsoluteAddressForRing(iv_tocRow.tgtHndl,
                                iv_hdctRow->cmdGetRing.ringAddr,
                                translatedAddress);
        l_reqMsg.ringAddr = translatedAddress;
        l_reqMsg.ringMode = 0x0001;
        l_reqMsg.ringLenInBits = bitlength;
        SBE_DEBUG(SBE_FUNC "Ring Address 0x%08X User Ring Mode 0x%04X "
                "Length in Bits 0x%08X Length in Bits(8 Byte aligned) 0x%08X",
                    l_reqMsg.ringAddr,
                    l_reqMsg.ringMode,
                    l_reqMsg.ringLenInBits,
                    iv_tocRow.hdctHeader.dataLength);
        // Verify ring data length in FIFO as per length size
        uint32_t startCount = iv_oStream.words_written();
        uint32_t totalCountInBytes = dummyDataLengthInBits / 8;
        uint32_t totalCount = totalCountInBytes / (sizeof(uint32_t));
        uint32_t dummyData = 0x00;

        fapi2::sbefifo_hwp_data_istream istream( iv_fifoType, len,
                                        (uint32_t*)&l_reqMsg, false );
        l_rc = sbeGetRingWrap( istream, iv_oStream );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        uint32_t endCount = iv_oStream.words_written();
        //If endCount = startCount means chip-op failed. We will write dummy data.
        if(endCount == startCount || ((endCount - startCount) != totalCount))
        {
            totalCount = totalCount - (endCount - startCount);
            l_rc = SBE_SEC_PCB_PIB_ERR;
            while(totalCount !=0)
            {
                l_rc = iv_oStream.put(dummyData);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                totalCount = totalCount - 1;
            }
        }
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
    }while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


////////////////////////////////////////////////////////////////////////////////
/// SBE get DUMP - Write fast-array data to FIFO
////////////////////////////////////////////////////////////////////////////////
uint32_t sbeCollectDump::writeGetFastArrayPacketToFifo()
{
    #define SBE_FUNC "writeGetFastArrayPacketToFifo "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode l_fapiRc;

    do
    {
        // Set FFDC failed command information and
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo(0, SBE_CMD_CLASS_ARRAY_ACCESS,
                                    SBE_CMD_CONTROL_FAST_ARRAY);

        // Update address, length and stream header data vai FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdFastArray.strEqvHash32;
        uint32_t len = sizeof(iv_tocRow.hdctHeader) / sizeof(uint32_t);
        if(!iv_tocRow.tgtHndl.getFunctional())
        {
            SBE_ERROR(SBE_FUNC "GETFASTARRAY: NonFunctional Target UnitNum[0x%08X]",
                                    (uint32_t)iv_tocRow.hdctHeader.chipUnitNum);
            // Update non functional state DUMP header
            iv_tocRow.hdctHeader.preReq = PRE_REQ_NON_FUNCTIONAL;
            iv_tocRow.hdctHeader.dataLength = 0x00;
            l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            break;
        }

        // assigning to local varibale
        const char * faFileName = (const char *)
           &const_fastarrayFileNameMapping[iv_hdctRow->cmdFastArray.controlSet];
        uint32_t strSize        = strlen(faFileName);
        SBE_INFO_BIN(SBE_FUNC "Fast array file name ",(char *)faFileName, strSize);

        // Create the request msg array
        uint8_t requestMsg[sizeof(sbeControlFastArrayCMD_t) +
                        FAST_ARRAY_FILE_NAME_MAX_LEN ] = {0};

        // Create the req struct for the sbeFastArray Chip-op
        sbeControlFastArrayCMD_t dumpFastArrayReq = {0};
        // Size of fastarray request msg
        uint32_t fastArrayReqMsgSize = 0;

        // Initialize the fast-array request msg
        dumpFastArrayReq.flags        = FASTARRAY_FLAG_NONE;
        dumpFastArrayReq.logTargetType= (uint32_t) getLogTargetFromHdctCustomChipTarget(
                                          (chipUnitTypes) iv_tocRow.hdctHeader.chipUnitType);
        dumpFastArrayReq.instanceId   = iv_hdctRow->genericHdr.chipletStart;

        // copying req struct to request message
        memcpy ((uint8_t *)requestMsg, (uint8_t *) &dumpFastArrayReq,
                                             sizeof(dumpFastArrayReq));
        fastArrayReqMsgSize = sizeof(dumpFastArrayReq)/sizeof(uint32_t);

        // copying fa file name (as per chipop spec) to request message
        memcpy ((char *) &requestMsg[ sizeof( dumpFastArrayReq ) ],
                                            (char *) faFileName, strSize );
        // string size aliging with 4 bytes
        fastArrayReqMsgSize += (strSize + 3)/sizeof(uint32_t);

        SBE_INFO(SBE_FUNC "Chipop data - Flags %x, logtarget %x, instanceid %x",
                            FASTARRAY_FLAG_NONE,
                            dumpFastArrayReq.logTargetType,
                            iv_hdctRow->genericHdr.chipletStart);

        // Getting fast-array control blob size
        uint32_t fastarrayBlobSize = 0x00;
        l_fapiRc = sbeCtrlFaUtilsGetCtrlBlobSize( faFileName,
                                                  fastarrayBlobSize,
                                                  (sbeSecondaryResponse&) l_rc);
        if((l_fapiRc != fapi2::FAPI2_RC_SUCCESS) ||
                                        (l_rc != SBE_SEC_OPERATION_SUCCESSFUL))
        {
            // sending the dummy dump header
            iv_tocRow.hdctHeader.dataLength = 0x00;
            l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

            SBE_ERROR(SBE_FUNC "getFastarrayControlBlobSize failure");
            iv_oStream.setFifoRc(l_fapiRc);
            iv_oStream.setPriSecRc(PRI_SEC_RC_UNION(
                        SBE_PRI_GENERIC_EXECUTION_FAILURE,
                        l_rc));
            break;
        }

        SBE_INFO(SBE_FUNC "fastarray blob size: 0x%x", fastarrayBlobSize);
        // fast array length in bytes
        iv_tocRow.hdctHeader.dataLength = (fastarrayBlobSize * 8);
        // Update TOC Header
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        uint32_t startCount = iv_oStream.words_written();
        uint32_t totalCount = fastarrayBlobSize;
        uint32_t dummyData = 0x00;

        // Create the iostream obj
        sbefifo_hwp_data_istream istream( iv_fifoType, fastArrayReqMsgSize,
                (uint32_t*)&requestMsg, false );

        l_rc = sbeControlFastArrayWrap( istream, iv_oStream );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        uint32_t endCount = iv_oStream.words_written();

        SBE_INFO(SBE_FUNC "startCount: 0x%x, endCount: 0x%x, totalCount: 0x%x,"
                          " (endCount - startCount): 0x%x",
                            startCount, endCount, totalCount,
                            (endCount - startCount));

        // If endCount = startCount means chip-op failed. We will write dummy data
        // All data streamed out need's to be 8 byte aligned.
        if(endCount == startCount || ((endCount - startCount) != totalCount))
        {
            totalCount = totalCount - (endCount - startCount);
            SBE_INFO(SBE_FUNC "Fast array data total count mismatch,"
                              " Writing dummy data len: 0x%x", totalCount);
            while(totalCount !=0)
            {
                l_rc = iv_oStream.put(dummyData);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                totalCount = totalCount - 1;
            }
        }
    }
    while(0);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


uint32_t sbeCollectDump::stopClocksOff()
{
    #define SBE_FUNC " stopClocksOff "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Set FFDC failed command information and
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo(0, SBE_CMD_CLASS_MPIPL_COMMANDS,
                                    SBE_CMD_MPIPL_STOPCLOCKS);

        // Update address, length and stream header data via FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdStopClocks.strEqvHash32;
        iv_tocRow.hdctHeader.dataLength = 0x00;
        uint32_t len = sizeof(iv_tocRow.hdctHeader) / sizeof(uint32_t);
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // Create the req struct for the sbeStopClocks Chip-op
        sbeStopClocksReqMsgHdr_t dumpStopClockReq = {0};
        len = sizeof(dumpStopClockReq)/sizeof(uint32_t);
        dumpStopClockReq.reserved1 = 0x00;
        dumpStopClockReq.iv_logTargetType = iv_hdctRow->cmdStopClocks.logTgtType;
        dumpStopClockReq.reserved2 = 0x00;
        dumpStopClockReq.iv_instanceId = iv_hdctRow->cmdStopClocks.chipletStart;
        fapi2::sbefifo_hwp_data_istream istream( iv_fifoType, len,
                                         (uint32_t*)&dumpStopClockReq, false );
        l_rc = sbeStopClocksWrap( istream, iv_oStream );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        SBE_DEBUG(SBE_FUNC" clockTypeTgt[0x%04X], chipUnitNum[0x%08X] ",
                    dumpStopClockReq.iv_logTargetType,
                    iv_tocRow.hdctHeader.chipUnitNum);
    }
    while(0);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t sbeCollectDump::writePutScomPacketToFifo()
{
    #define SBE_FUNC "writePutScomPacketToFifo"
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    fapi2::ReturnCode fapiRc = fapi2::FAPI2_RC_SUCCESS;
    uint64_t dumpData = 0;
    do
    {
        // Set FFDC failed command information and
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo(0, SBE_CMD_CLASS_SCOM_ACCESS, SBE_CMD_PUTSCOM);

        // Update address, length and stream header data via FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdPutScom.addr;
        iv_tocRow.hdctHeader.dataLength = 0x00;
        uint32_t len = sizeof(iv_tocRow.hdctHeader) / sizeof(uint32_t);
        if(!iv_tocRow.tgtHndl.getFunctional())
        {
            iv_tocRow.hdctHeader.preReq = PRE_REQ_NON_FUNCTIONAL;
            l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            SBE_DEBUG("DUMP PUTSCOM: NonFunctional Target UnitNum[0x%08X]",
                     (uint32_t)iv_tocRow.hdctHeader.chipUnitNum);
            break;
        }
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        uint32_t addr = iv_tocRow.hdctHeader.address;
        uint32_t maskType = iv_hdctRow->cmdPutScom.extGenericHdr.bitModifier;
        fapi2::Target<fapi2::TARGET_TYPE_CHIPS> dumpRowTgt(iv_tocRow.tgtHndl);
        fapi2::seeprom_hwp_data_istream stream((uint32_t*)&iv_hdctRow->cmdPutScom.value,
                                         sizeof(uint64_t));
        uint32_t msbValue, lsbValue;
        stream.get(msbValue), stream.get(lsbValue);
        uint64_t mask = (((uint64_t)msbValue << 32 ) | ((uint64_t)lsbValue));
        dumpData = mask; // maskType is nnone then putScom data is mask value.

        SBE_DEBUG("putscom address:[0x%08X], maskType:[0x%08X], mask:[0x%08X%08X]",
                  addr, maskType, SBE::higher32BWord(mask), SBE::lower32BWord(mask));

        if( B_NONE != maskType )
        {
            uint64_t readData = 0;
            fapiRc = getscom_abs_wrap(&dumpRowTgt, addr, &readData);
            if(fapiRc != fapi2::FAPI2_RC_SUCCESS)
            {
                iv_oStream.setFifoRc(fapiRc);
                iv_oStream.setPriSecRc(PRI_SEC_RC_UNION(
                            SBE_PRI_GENERIC_EXECUTION_FAILURE,
                            SBE_SEC_GENERIC_FAILURE_IN_EXECUTION));
                break;
            }
            SBE_INFO(SBE_FUNC " putscom scom value: 0x%.8X%.8X ",
                     SBE::higher32BWord(readData),SBE::lower32BWord(readData));
            if( B_OR == maskType )
            {
                dumpData = (readData | mask);
            }
            if( B_AND == maskType )
            {
                dumpData = (readData & mask);
            }
        }
        SBE_DEBUG(SBE_FUNC " maskType[0x%02X], data [0x%08X %08X] ", maskType,
                      SBE::higher32BWord(dumpData),SBE::lower32BWord(dumpData));
        fapiRc = putscom_abs_wrap(&dumpRowTgt, addr, dumpData);
        if(fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            iv_oStream.setFifoRc(fapiRc);
            iv_oStream.setPriSecRc(PRI_SEC_RC_UNION(
                        SBE_PRI_GENERIC_EXECUTION_FAILURE,
                        SBE_SEC_GENERIC_FAILURE_IN_EXECUTION));
            break;
        }
    }
    while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t sbeCollectDump::writeGetScomPacketToFifo()
{
    #define SBE_FUNC "writeGetScomPacketToFifo"
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    fapi2::ReturnCode fapiRc = fapi2::FAPI2_RC_SUCCESS;

    do{
        // Set FFDC failed command information and
        // Sequence Id is 0 by default for Fifo interface
        iv_chipOpffdc.setCmdInfo(0, SBE_CMD_CLASS_SCOM_ACCESS, SBE_CMD_GETSCOM );

        // Update address, length and stream header data vai FIFO
        iv_tocRow.hdctHeader.address = iv_hdctRow->cmdGetScom.addr;
        uint32_t len = sizeof(iv_tocRow.hdctHeader) / sizeof(uint32_t);
        if(!iv_tocRow.tgtHndl.getFunctional())
        {
            // Update non functional state DUMP header
            iv_tocRow.hdctHeader.preReq = PRE_REQ_NON_FUNCTIONAL;
            iv_tocRow.hdctHeader.dataLength = 0x00;
            SBE_DEBUG("DUMP GETSCOM: NonFunctional Target UnitNum[0x%08X]",
                    (uint32_t)iv_tocRow.hdctHeader.chipUnitNum);
            l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        }
        iv_tocRow.hdctHeader.dataLength = 0x40; // 64 bits -or- 2 words
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_tocRow.hdctHeader);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        uint64_t dumpData;
        // Proc Scoms
        fapi2::Target<fapi2::TARGET_TYPE_CHIPS> dumpRowTgt(iv_tocRow.tgtHndl);
        fapiRc = getscom_abs_wrap(&dumpRowTgt, iv_tocRow.hdctHeader.address, &dumpData);
        if(fapiRc != fapi2::FAPI2_RC_SUCCESS)
        {
            iv_oStream.setFifoRc(fapiRc);
            iv_oStream.setPriSecRc(PRI_SEC_RC_UNION(
                            SBE_PRI_GENERIC_EXECUTION_FAILURE,
                            SBE_SEC_GENERIC_FAILURE_IN_EXECUTION));
            break;
        }
        SBE_DEBUG("getScom: address: 0x%08X, data HI: 0x%08X, data LO: 0x%08X ",
                iv_tocRow.hdctHeader.address, SBE::higher32BWord(dumpData),
                SBE::lower32BWord(dumpData));
        l_rc = iv_oStream.put(FIFO_DOUBLEWORD_LEN, (uint32_t*)&dumpData);
    }while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/******************** Types of Dump Packets to FIFO END **********************/


// ------------------------------------------------------------------------------ //


/****************  Dump Packet to FIFO based on type ********************/
uint32_t sbeCollectDump::writeDumpPacketRowToFifo()
{
    #define SBE_FUNC "writeDumpPacketRowToFifo"
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // Get update row values by using HDCT bin data
        iv_tocRow.hdctHeaderInit(iv_hdctRow);

        // Filtering HDCT Cmd types
        if( !( (iv_tocRow.hdctHeader.cmdType == CMD_GETSCOM)       ||
               (iv_tocRow.hdctHeader.cmdType == CMD_PUTSCOM)       ||
               (iv_tocRow.hdctHeader.cmdType == CMD_GETTRACEARRAY) ||
               (iv_tocRow.hdctHeader.cmdType == CMD_STOPCLOCKS)    ||
               (iv_tocRow.hdctHeader.cmdType == CMD_GETRING)       ||
               (iv_tocRow.hdctHeader.cmdType == CMD_GETFASTARRAY)
               )
          )
        {
            SBE_ERROR(SBE_FUNC "Unsupported command types %d", (uint32_t)iv_tocRow.hdctHeader.cmdType);
            break;
        }

        // Map Dump target id with plat target list
        std::vector<fapi2::plat_target_handle_t> targetList;
        getTargetList(targetList);

        for( auto &target : targetList )
        {
            //CPU cycles to complete the chip-op.
            iv_tocRow.cpuCycles = pk_timebase_get();

            // write dump row header contents using FIFO
            fapi2::Target<fapi2::TARGET_TYPE_CHIPS> dumpRowTgtHnd(target);
            iv_tocRow.tgtHndl = target;
            iv_tocRow.hdctHeader.preReq = PRE_REQ_PASSED;

            if(iv_tocRow.hdctHeader.chipUnitType == CHIP_UNIT_TYPE_PERV)
            {
                iv_tocRow.hdctHeader.chipUnitNum = dumpRowTgtHnd.getChipletNumber();
            }
            else
            {
                iv_tocRow.hdctHeader.chipUnitNum = dumpRowTgtHnd.get().getTargetInstance();
            }

            // Clear FifoRc and Clear Primary Secondary RC
            iv_oStream.setFifoRc(fapi2::FAPI2_RC_SUCCESS);
            iv_oStream.setPriSecRc(SBE_PRI_OPERATION_SUCCESSFUL);
            // Add HWP specific ffdc data length
            iv_chipOpffdc.lenInWords = 0;

            switch(iv_tocRow.hdctHeader.cmdType)
            {
                case CMD_GETSCOM:
                {
                    l_rc = writeGetScomPacketToFifo();
                    break;
                }

                case CMD_PUTSCOM:
                {
                    l_rc = writePutScomPacketToFifo();
                    break;
                }

                case CMD_GETTRACEARRAY:
                {
                    l_rc = writeGetTracearrayPacketToFifo();
                    break;
                }

                case CMD_STOPCLOCKS:
                {
                    l_rc = stopClocksOff();
                    break;
                }

                case CMD_GETRING:
                {
                    l_rc = writeGetRingPacketToFifo();
                    break;
                }

                case CMD_GETFASTARRAY:
                {
                    l_rc = writeGetFastArrayPacketToFifo();
                    break;
                }

                default:
                {
                    // Print the error message and continue the dumping
                    SBE_ERROR(SBE_FUNC " command Id [0x%08X] is not supported.",
                                        (uint8_t)iv_tocRow.hdctHeader.cmdType);
                    break;
                }
            } // End switch
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            if( (iv_oStream.getFifoRc() != fapi2::FAPI2_RC_SUCCESS) ||
                (iv_oStream.getPriSecRc() != SBE_SEC_OPERATION_SUCCESSFUL) )
            {
                iv_chipOpffdc.setRc(iv_oStream.getFifoRc());
                // Update FFDC lenth + PrimarySecondary(32 bits) RC lenth
                iv_tocRow.ffdcLen = sizeof(sbeResponseFfdc_t) + sizeof(uint32_t);
                // write FFDC data on failed case using FIFO
                l_rc = iv_oStream.put(iv_tocRow.ffdcLen);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                // Set Primary Secondary RC
                l_rc = iv_oStream.put(iv_oStream.getPriSecRc());
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                l_rc = iv_oStream.put(sizeof(sbeResponseFfdc_t)/sizeof(uint32_t)
                                        ,(uint32_t*)&iv_chipOpffdc);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            }
            else
            {
                uint32_t ffdcDataLength = 0x00;
                // write FFDC data as a zero for success using FIFO
                l_rc = iv_oStream.put(ffdcDataLength);
                CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            }
            // FIFO the cpuCycles value
            iv_tocRow.cpuCycles = pk_timebase_get() - iv_tocRow.cpuCycles; // Delay time
            l_rc = iv_oStream.put(FIFO_DOUBLEWORD_LEN,
                                    (uint32_t*)&iv_tocRow.cpuCycles);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        } // End For loop
    }while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

/******************** Parse Single HDCT Entry **********************/
uint32_t sbeCollectDump::parserSingleHDCTEntry()
{
    #define SBE_FUNC " parserSingleHDCTEntry "
    SBE_ENTER(SBE_FUNC);

    //Return status
    bool status = true;
    do
    {
        //Check if all HDCT entries are parsed
        if(!(iv_hdctPakSecDetails.iv_currAddr < iv_hdctPakSecDetails.iv_endAddr))
        {
            SBE_INFO("All HDCT entries parsed for Clock State: %d", iv_clockState);
            status = false;
            break;
        }

        //Parse single HDCT row and populate genericHdctRow_t struct
        iv_hdctRow = (genericHdctRow_t*)iv_hdctPakSecDetails.iv_currAddr;

        //Increment the current address to point to the next HDCT row
        iv_hdctPakSecDetails.iv_currAddr = iv_hdctPakSecDetails.iv_currAddr +
                                        genericHdctRowSize_table[(uint8_t)(iv_hdctRow->genericHdr.command)];

        //Error Check
        if(genericHdctRowSize_table[(uint8_t)(iv_hdctRow->genericHdr.command)] == CMD_TYPE_NOT_USED)
        {
            SBE_ERROR("Unknown command type: %X, Error in parsing HDCT.bin",
                      (uint8_t) iv_hdctRow->genericHdr.command);
            status = false;
            break;
        }

        // Clock off state filter
        if (iv_clockState == SBE_DUMP_CLOCK_OFF &&
            iv_clockOffEntryFlag == true &&
            iv_hdctRow->genericHdr.command == CMD_STOPCLOCKS)
        {
            iv_clockOffEntryFlag = false;
        }

        //Clock On state filter
        if (iv_clockState == SBE_DUMP_CLOCK_ON && iv_hdctRow->genericHdr.command == CMD_STOPCLOCKS)
        {
            SBE_INFO("All HDCT entries parsed for Clock State: %d", iv_clockState);
            status = false;
            break;
        }

    }while(iv_clockOffEntryFlag ? true : (!(sbeCollectDump::isDumpTypeMapped())));

    SBE_EXIT(SBE_FUNC);
    return status;
    #undef SBE_FUNC
}

// ------------------------------------------------------------------------------ //


/******************** MAIN ENTRY POINT TO COLLECT HDCT ENTRIES
 * Loop For HDCT Entries and Dump them based on Types
 * 1. parseSingleHDCTEntry
 * 2. Dump packet to FIFO based on
 * 3. Dump Type
 * **********************/
uint32_t sbeCollectDump::collectAllHDCTEntries()
{
#define SBE_FUNC " collectAllHDCTEntries "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    do
    {
        // Write the dump header to FIFO
        uint32_t len = sizeof(dumpHeader_t)/ sizeof(uint32_t);
        l_rc = iv_oStream.put(len, (uint32_t*)&iv_dumpHeader);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        while(sbeCollectDump::parserSingleHDCTEntry())
        {
            l_rc = writeDumpPacketRowToFifo();
            // If FIFO access failure
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        }
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        //Dump chip-op Footer - DONE
        l_rc = iv_oStream.put(DUMP_CHIP_OP_FOOTER);
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
    }
    while(0);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}