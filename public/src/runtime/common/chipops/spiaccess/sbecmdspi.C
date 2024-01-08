/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/spiaccess/sbecmdspi.C $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbescom.H"
#include "sbeutil.H"
#include "sbeFifoMsgUtils.H"
#include "plat_target.H"
#include "ffdc.H"
#include "chipop_struct.H"
#include "plat_hwp_data_stream.H"
#include "sbeglobals.H"
#include "heap.H"
#include "codeupdateutils.H"

#include "sbecmdspi.H"

using namespace fapi2;

/**
  * @brief prepSpiAccess: Allocate scratch space based on the SEEPROM img length in bytes
  * & get memory device handle
  *
  * @param[in]  l_imgBufScratchArea  pointer to allocated scratch space
  * @param[in]  boot_side            which redundant seeprom to write to
  * @param[in]  gMemHandle           memory device handle to set up
  * @param[in]  respHdr              response header message
  *
  * @return    Rc from the FIFO access utility
  */
fapi2::ReturnCode prepSpiAccess(uint32_t*& l_imgBufScratchArea, uint8_t boot_side, spi::AbstractMemoryDevice *& gMemHandle, sbeRespGenHdr_t& respHdr)
{
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    //////////////////////////////////////////////////////
    //Allocate scratch space based on the SEEPROM img length in bytes
    l_imgBufScratchArea =
                (uint32_t*)Heap::get_instance().scratch_alloc(MAX_BUFFER_SIZE);
    if(l_imgBufScratchArea == NULL)
    {
        SBE_ERROR(SBE_FUNC "scratch allocation failed. Not enough scratch area to allocate");
        respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                           SBE_SEC_HEAP_BUFFER_ALLOC_FAILED);
        return SBE_SEC_HEAP_BUFFER_ALLOC_FAILED;
    }

    //////////////////////////////////////////////////////
    // Get memory device handle
    l_rc = createMemoryDevice(boot_side, 0, true, true, gMemHandle);
    if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
    {
        SBE_ERROR(SBE_FUNC " createMemoryDevice unsuccessful. RC[0x%08x] ", l_rc);
        respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, l_rc);
    }
    return l_rc;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeSpiWrite(uint8_t *i_pArg)
{
#define SBE_FUNC " sbeSpiWrite "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbeSpiWriteWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeSpiWriteWrap ( fapi2::sbefifo_hwp_data_istream& i_getStream,
                         fapi2::sbefifo_hwp_data_ostream& i_putStream )
{
    #define SBE_FUNC " spiWrite "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeSpiWriteMsgHdr_t l_reqMsg = sbeSpiWriteMsgHdr_t();
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    uint32_t len = 0;
    uint32_t* l_imgBufScratchArea = NULL;

    spi::AbstractMemoryDevice *gMemHandle;

    do
    {
        // Dequeue seeprom_id and start_addr
        uint32_t len2dequeue = sizeof(l_reqMsg)/sizeof(uint32_t);
        // EoT not expected
        l_rc = i_getStream.get(len2dequeue, (uint32_t *)&l_reqMsg, false);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        // Get the length of img
        // Length is not part of chipop. So take length from total length
        len = SBE_GLOBAL->sbeFifoCmdHdr.len -
                        sizeof(SBE_GLOBAL->sbeFifoCmdHdr)/sizeof(uint32_t);

        // Get the length for SEEPROM img in words
        uint32_t imgLenWords = len -
                        sizeof(sbeSpiWriteMsgHdr_t)/sizeof(uint32_t);
        SBE_INFO(SBE_FUNC" imgLen in words: [0x%08X] in bytes: [0x%08X]",
                        imgLenWords, WORD_TO_BYTES(imgLenWords));

        //////////////////////////////////////////////////////
        //Allocoate scratch space based on the SEEPROM img length in bytes & get mem dev handle
        l_rc = prepSpiAccess(l_imgBufScratchArea, l_reqMsg.boot_side, gMemHandle, respHdr);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // flush the fifo
            l_rc = i_getStream.get(len, NULL, true, true);
            // Let command processor routine to handle the RC.
            break;
        }

        //////////////////////////////////////////////////////
        // Pull and write the SEEPROM img

        // The received image would be updated in chunks of maxSpiWriteInWords
        // or less depending on the image size in a loop
        uint32_t l_writeWordsLen = 0;
        uint32_t l_maxSpiWriteInWords = BYTES_TO_WORDS(MAX_BUFFER_SIZE);
        bool l_eotFlag  = false;

        // Offset at which to begin writing
        uint32_t l_imageStartAddr = l_reqMsg.start_addr;

        FAPI_TRY(gMemHandle->write_begin(l_imageStartAddr, WORD_TO_BYTES(imgLenWords)));

        for (uint32_t l_len = imgLenWords; l_len > 0; l_len -= l_writeWordsLen)
        {
            l_writeWordsLen = (l_len > l_maxSpiWriteInWords ? l_maxSpiWriteInWords : l_len);

            // Will attempt to dequeue entries based on the size passed above plus
            // the expected EOT entry at the end
            SBE_INFO(SBE_FUNC "Loop: WriteLen(bytes):[0x%08X] withECC(bytes):[0x%08X]",
                WORD_TO_BYTES(l_writeWordsLen), WITH_ECC(WORD_TO_BYTES(l_writeWordsLen)));

            // For next write to fifo to get next set of data
            if (l_len == l_writeWordsLen)
            {
                // last set of data with EOT set
                l_eotFlag = true;
            }

            // Dequeing fifo along with EOT ack, set to true in last loop
            // false otherwise
            l_rc = i_getStream.get(l_writeWordsLen, (uint32_t *)l_imgBufScratchArea, l_eotFlag);

            // If FIFO access failure
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

            // Perform device write
            SBE_INFO(SBE_FUNC "Write data...");
            l_rc = gMemHandle->write_data(l_imgBufScratchArea, WORD_TO_BYTES(l_writeWordsLen));

            if(l_rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "Write to device failed, Addr:0x%08X Size:0x%08X",\
                          WITH_ECC(l_imageStartAddr),\
                          WORD_TO_BYTES(l_writeWordsLen));

                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                  SBE_SEC_CU_WRITE_DATA_IMAGE_FAILURE );
                break;
            }

            l_imageStartAddr += WORD_TO_BYTES(l_writeWordsLen);

        } //end of for-loop

        FAPI_TRY(gMemHandle->write_end());

    } while(false);

fapi_try_exit:

    //////////////////////////////////////////////////////
    //Free the scratch area
    Heap::get_instance().scratch_free(l_imgBufScratchArea);
    freeMemoryDevice(gMemHandle);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType());
    // will let command processor routine handle the failure

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeSpiRead(uint8_t *i_pArg)
{
#define SBE_FUNC " sbeSpiRead "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbeSpiReadWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeSpiReadWrap ( fapi2::sbefifo_hwp_data_istream& i_getStream,
                         fapi2::sbefifo_hwp_data_ostream& i_putStream )
{
    #define SBE_FUNC " spiRead "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeSpiReadMsgHdr_t l_reqMsg = sbeSpiReadMsgHdr_t();
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    uint32_t len = 0;
    uint32_t* l_imgBufScratchArea = NULL;

    spi::AbstractMemoryDevice *gMemHandle;

    do
    {
        // Dequeue seeprom_id and start_addr
        uint32_t len2dequeue = sizeof(l_reqMsg)/sizeof(uint32_t);
        // EoT not expected
        l_rc = i_getStream.get(len2dequeue, (uint32_t *)&l_reqMsg, true);

        // If FIFO access failure
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // Let command processor routine to handle the RC.
            break;
        }

        //////////////////////////////////////////////////////
        //Allocoate scratch space based on the SEEPROM img length in bytes & get mem dev handle
        l_rc = prepSpiAccess(l_imgBufScratchArea, l_reqMsg.boot_side, gMemHandle, respHdr);
        if (l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            // flush the fifo
            l_rc = i_getStream.get(len, NULL, true, true);
            // Let command processor routine to handle the RC.
            break;
        }

        //////////////////////////////////////////////////////
        // Read the SEEPROM img
        uint32_t l_readBytesLen = 0;

        // Offset at which to begin reading
        uint32_t l_imageStartAddr = l_reqMsg.start_addr;

        for (uint32_t l_len = l_reqMsg.len; l_len > 0; l_len -= l_readBytesLen)
        {
            l_readBytesLen = (l_len > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : l_len);

            SBE_INFO(SBE_FUNC "Loop: ReadLen(bytes):[0x%08X] withECC(bytes):[0x%08X]",
                l_readBytesLen, WITH_ECC(l_readBytesLen));


            // Perform device read
            l_rc = gMemHandle->read(l_imageStartAddr, l_readBytesLen, l_imgBufScratchArea);

            if(l_rc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "Write to device failed, Addr(w/o ECC):0x%08X Size(in Bytes):0x%08X",\
                          l_imageStartAddr,\
                          l_readBytesLen);

                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                  SBE_SEC_CU_READ_DATA_IMAGE_FAILURE );
                break;
            }

            // Enqueing fifo
            l_rc = i_putStream.put(BYTES_TO_WORDS(l_readBytesLen), (uint32_t *)l_imgBufScratchArea);

            // If FIFO access failure
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

            l_imageStartAddr += l_readBytesLen;

        } //end of for-loop

    } while(false);
  
fapi_try_exit:

    //////////////////////////////////////////////////////
    //Free the scratch area
    Heap::get_instance().scratch_free(l_imgBufScratchArea);
    freeMemoryDevice(gMemHandle);

    // Build the response header packet
    l_rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType());
    // will let command processor routine handle the failure

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
