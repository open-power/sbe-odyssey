/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/fastarray/sbecmdfastarray.C $ */
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
#include "sbecmdfastarray.H"
#include "sbeFifoMsgUtils.H"
#include "sbeglobals.H"
#include "chipop_handler.H"
#include "poz_fastarray.H"

using namespace fapi2;


static inline uint32_t requestPayloadLength()
{
    return SBE_GLOBAL->sbeFifoCmdHdr.len - sizeof(SBE_GLOBAL->sbeFifoCmdHdr)/sizeof(uint32_t);
}


uint32_t sbeControlFastArrayWrap( fapi2::sbefifo_hwp_data_istream& i_getStream,
                                  fapi2::sbefifo_hwp_data_ostream& i_putStream )
{
    #define SBE_FUNC " sbeControlFastArrayWrap "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeControlFastArrayCMD_t req = {};
    sbeResponseFfdc_t ffdc;
    ReturnCode fapiRc;
    sbeRespGenHdr_t respHdr;
    respHdr.init();

    do
    {
        SBE_TRY( i_getStream.get(sizeof(req)/sizeof(uint32_t), (uint32_t *)&req, false) );
        SBE_INFO(SBE_FUNC "logTargetType [0x%02X] instanceId [0x%02X] flags [0x%02X]",
                 req.logTargetType, static_cast<uint8_t>(req.instanceId),
                 static_cast<uint8_t>(req.flags));

        // Construct a Target from Instance ID and Target Type
        plat_target_handle_t tgtHndl;
        l_rc = g_platTarget->getSbePlatTargetHandle(req.logTargetType,
                                                        req.instanceId,
                                                        tgtHndl);
        SBE_ASSERT( !l_rc,
                    false,
                    SBE_PRI_INVALID_DATA,
                    l_rc,
                    "Invalid target - Type [0x%02x] Instance ID [0x%02X]",
                    static_cast<uint8_t>(req.logTargetType),
                    static_cast<uint8_t>(req.instanceId) );

        if (req.flags & FASTARRAY_FLAG_CUSTOM_CONTROL_DATA)
        {
            // custom control data -> grab instructions right from the FIFO
            SBE_EXEC_HWP(fapiRc, poz_fastarray, tgtHndl, i_getStream, i_putStream);
        }
        else
        {
            // predefined control data -> load and decompress instructions,
            // then grab via array stream
            const uint32_t extra_len = requestPayloadLength() - sizeof(req)/sizeof(uint32_t);
            static const uint32_t MAX_FNAME_LEN = 64;
            char fname[MAX_FNAME_LEN + 1];

            SBE_ASSERT(extra_len < (MAX_FNAME_LEN / sizeof(uint32_t)),
                       true, SBE_PRI_INVALID_DATA,
                       SBE_SEC_INPUT_BUFFER_OVERFLOW,
                       "Fastarray request name is too long."
                       "request name length: %d, max length: %d",
                       extra_len * 4, MAX_FNAME_LEN);

            char final_fname[MAX_FNAME_LEN + 4] = "factl/";
            SBE_TRY(i_getStream.get(extra_len, (uint32_t *)fname));
            fname[MAX_FNAME_LEN] = 0;
            strcat(final_fname, fname);

            SBE_INFO_BIN(SBE_FUNC "File name final_fname", final_fname, strlen(final_fname));

            const void *control = NULL;
            uint32_t control_size = 0;
            SBE_TRY(SBE_GLOBAL->embeddedArchive.load_file(final_fname, control, control_size));

            hwp_array_istream control_stream((uint32_t *)control, (control_size + 3) / sizeof(uint32_t));
            SBE_EXEC_HWP(fapiRc, poz_fastarray, tgtHndl, control_stream, i_putStream);

            SBE_GLOBAL->embeddedArchive.free_file(control);
        }

        if(fapiRc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC" HWP failure:logTargetType [0x%02X] "
                      "chipletId [0x%02X] flags [0x%02X] ",
                      static_cast<uint8_t>(req.logTargetType),
                      static_cast<uint8_t>(req.instanceId),
                      static_cast<uint32_t>(req.flags));
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_HWP_FAILURE);
            ffdc.setRc(fapiRc);
        }

        l_rc  = i_putStream.put(i_putStream.words_written());

    } while(false);

    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if( ( SBE_SEC_OPERATION_SUCCESSFUL == l_rc ) &&
        ( i_putStream.isStreamRespHeader(respHdr.rcStatus(),ffdc.getRc())) )
    {
        l_rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType() );
    }

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

uint32_t sbeControlFastArray(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeControlFastArray "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    fapi2::sbefifo_hwp_data_ostream ostream(type);
    fapi2::sbefifo_hwp_data_istream istream(type);

    l_rc = sbeControlFastArrayWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}
