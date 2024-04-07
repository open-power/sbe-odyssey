/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/sramaccess/sbecmdsramaccess.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "sbecmdsramaccess.H"
#include "sbeFifoMsgUtils.H"
#include "chipop_handler.H"
#include "sbeutil.H"


using namespace fapi2;


uint32_t sbeGetSram (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetSram "
    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);

    SBE_DEBUG(SBE_FUNC "##### GET SRAM HWP #####");

    return sbeSramAccessWrap (istream, ostream, true);
    #undef SBE_FUNC
}


uint32_t sbePutSram (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbePutSram "
    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);

    SBE_DEBUG(SBE_FUNC "##### PUT SRAM HWP #####");

    return sbeSramAccessWrap (istream, ostream, false);
    #undef SBE_FUNC
}


uint32_t sbeSramAccessWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                            fapi2::sbefifo_hwp_data_ostream& i_putStream,
                            const bool i_isGetFlag )
{
    #define SBE_FUNC " sbeSramAccessWrap "

    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc          = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t fapiRc        = FAPI2_RC_SUCCESS;

    // Create the req struct for the SRAM Chip-op
    sbeSramAccessReqMsgHdr_t l_req = {0};
    sbeResponseFfdc_t        ffdc;
    sbeRespGenHdr_t          respHdr;
    respHdr.init();

    // Total Returned length from the procedure
    uint32_t totalReturnLen = 0;

    do
    {
        // Get the TargetType and Instance Id from the command message
        // Get the Req Struct Size Data from upstream Fifo
        // Get the header length
        SBE_TRY ( i_getStream.get( (sizeof(l_req) / SBE_32BIT_ALIGN_FACTOR),
                                   (uint32_t *)&l_req, i_isGetFlag, false ) );

        SBE_INFO(SBE_FUNC "AddressWord0 [0x%04X] AddressWord1 [0x%04X]"
                          " Len [0x%04X]",
                          l_req.addressWord0,
                          l_req.addressWord1,
                          l_req.length );

        uint64_t sramOffsetAddr =
                    ( (uint64_t)l_req.addressWord0 << 32) | l_req.addressWord1;

        // Validating params
        auto secRc = l_req.validateParams();
        if ( secRc )
        {
            SBE_ERROR(SBE_FUNC "Input Len: 0x%.8x/Addr: 0x%.8x%.8x is not aligned, RC: 0x%.8x",
                                l_req.length, l_req.addressWord0, l_req.addressWord1, secRc);
            respHdr.setStatus(SBE_PRI_INVALID_DATA, secRc);
            break;
        }

        // Create a 128 Byte Buffer -  16 64-Bit buffer
        // 128 (SBE_SRAM_GRANULE) / 4 (SBE_32BIT_ALIGN_FACTOR) = 32 32-bit buffer = 16 64-bit buffer
        uint32_t dataBuffer[SBE_SRAM_GRANULE / SBE_32BIT_ALIGN_FACTOR] = {};
        uint32_t remainingLen = l_req.length; // Initialize with Total Len
        uint32_t lenPassedToHwp = 0;

        //Write or Read in the chunks of 128 bytes
        while(remainingLen)
        {
            // algo for part by part get/put the SRAM chipop
            lenPassedToHwp = (remainingLen <= SBE_SRAM_GRANULE)?
                                                remainingLen : SBE_SRAM_GRANULE;

            SBE_INFO(SBE_FUNC "SRAM Offset addr: 0x%08X%08X, lenPassedToHwp : 0x%08X",
                                            SBE::higher32BWord(sramOffsetAddr),
                                            SBE::lower32BWord(sramOffsetAddr),
                                            lenPassedToHwp );

            if (!i_isGetFlag)
            {
                SBE_TRY( i_getStream.get( lenPassedToHwp/SBE_32BIT_ALIGN_FACTOR,
                                          dataBuffer, false, false ) );
            }

            // Execute HWP
            fapiRc = l_req.executeHwp( sramOffsetAddr, lenPassedToHwp,
                                       (uint8_t *)dataBuffer, i_isGetFlag );
            if(fapiRc != FAPI2_RC_SUCCESS)
            {
                SBE_ERROR("Address:0x%.8x%.8x Length=0x%.8x",
                                            l_req.addressWord0,
                                            l_req.addressWord1,
                                            lenPassedToHwp );
                // Respond with HWP FFDC
                respHdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                    SBE_SEC_GET_SRAM_FAILED);
                ffdc.setRc(fapiRc);
                break;
            }

            // Calculate the Total len passed to HWP
            totalReturnLen += lenPassedToHwp;
            // Update the address offset
            sramOffsetAddr += (static_cast<uint64_t>(lenPassedToHwp)) ;
            // Calculate the remaining len
            remainingLen = remainingLen - lenPassedToHwp;

            if(i_isGetFlag) // Get Sram send buffer data to down fifo
            {
                uint32_t len2dequeue = (lenPassedToHwp/SBE_32BIT_ALIGN_FACTOR);
                // Push this into the downstream FIFO
                SBE_TRY ( i_putStream.put( len2dequeue, dataBuffer ) );
            }
        } // End of while Put/Get from Hwp

        // Put SRAM
        if( !i_isGetFlag )
        {
            if ( respHdr.primaryStatus() != SBE_PRI_OPERATION_SUCCESSFUL )
            {
                // If there was a HWP failure for put sram  request,
                // need to Flush out upstream FIFO, until EOT arrives
                SBE_TRY( i_getStream.get( 0, NULL, true, true ) );
            }
            else
            {
                // For other success paths, just attempt to offload
                // the next entry, which is supposed to be the EOT entry
                SBE_TRY( i_getStream.get( 0, NULL, true, false ) );
            }
        }

    }while(false);

    // In case of trace array chipop SBE will send the response
    if ( i_putStream.isStreamRespHeader(respHdr.rcStatus(), ffdc.getRc()) &&
         SBE_SEC_OPERATION_SUCCESSFUL == l_rc )
    {
        // Get SRAM
        if(i_isGetFlag)
        {
            uint32_t l_bytesWritten = i_putStream.words_written() * 4;
            l_rc = i_putStream.put(1, &l_bytesWritten);
        }
        // Put SRAM
        else
        {
            l_rc = i_putStream.put(1, &totalReturnLen);
        }

        // If there was a FIFO error, will skip sending the response,
        // instead give the control back to the command processor thread
        if (SBE_SEC_OPERATION_SUCCESSFUL == l_rc)
        {
            // Create the Response to caller
            l_rc = sbeDsSendRespHdr(respHdr, &ffdc, i_getStream.getFifoType());
        }
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

