/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/fastarray/sbecmdfastarray.C $ */
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
                                  fapi2::sbefifo_hwp_data_ostream& o_putStream )
{
    #define SBE_FUNC " sbeControlFastArrayWrap "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;

    sbeControlFastArrayCMD_t req = {};
    sbeResponseFfdc_t ffdc;
    sbeRespGenHdr_t respHdr;
    respHdr.init();

    const void * controlData = NULL;

    do
    {
        SBE_TRY( i_getStream.get(sizeof(req)/sizeof(uint32_t), (uint32_t *)&req, false) );
        SBE_INFO(SBE_FUNC "logTargetType [0x%02X] instanceId [0x%02X] flags [0x%02X]",
                 req.logTargetType, static_cast<uint8_t>(req.instanceId),
                 static_cast<uint8_t>(req.flags));

        // Construct a Target from Instance ID and Target Type
        plat_target_handle_t tgtHndl;
        uint32_t l_secRc = g_platTarget->getSbePlatTargetHandle(req.logTargetType,
                                                       req.instanceId, tgtHndl);
        if (l_secRc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "getSbePlatTargetHandle failure"
                      "logTargetType [0x%02X]",
                      "chipletId [0x%02X]",
                      static_cast<uint8_t>(req.logTargetType),
                      static_cast<uint8_t>(req.instanceId));
            respHdr.setStatus( SBE_PRI_INVALID_DATA, l_secRc);
            break;
        }

        if (req.flags & FASTARRAY_FLAG_CUSTOM_CONTROL_DATA)
        {
            // custom control data -> grab instructions right from the FIFO
            SBE_EXEC_HWP(fapiRc, poz_fastarray, tgtHndl, i_getStream, o_putStream);
        }
        else
        {
            // predefined control data -> load and decompress instructions,
            // then grab via array stream
            uint32_t extra_len = i_getStream.getFifoWordLeft();
            SBE_ASSERT(extra_len < (FAST_ARRAY_FILE_NAME_MAX_LEN / sizeof(uint32_t)),
                       true, SBE_PRI_INVALID_DATA,
                       SBE_SEC_INPUT_BUFFER_OVERFLOW,
                       "Fastarray request name is too long."
                       "request name length: %d, max length: %d",
                       extra_len * 4, FAST_ARRAY_FILE_NAME_MAX_LEN);

            char fname[FAST_ARRAY_FILE_NAME_MAX_LEN] = {0};
            SBE_TRY( i_getStream.get (extra_len, (uint32_t *)fname) );
            fname[FAST_ARRAY_FILE_NAME_MAX_LEN - 1] = 0;

            uint32_t controlSize = 0;
            auto secRc = sbeCtrlFaUtilsGetCtrlData( fname,
                                                    controlData,
                                                    controlSize );
            if (secRc)
            {
                SBE_ERROR(SBE_FUNC "sbeCtrlFaUtilsGetCtrlData failure: "
                        "logTargetType [0x%02X]",
                        "chipletId [0x%02X] secRc [0x%02X] ",
                        static_cast<uint8_t>(req.logTargetType),
                        static_cast<uint8_t>(req.instanceId),
                        static_cast<uint32_t>(secRc));
                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, secRc);
                break;
            }

            hwp_array_istream controlStream((uint32_t *)controlData,
                                          (controlSize + 3) / sizeof(uint32_t));
            // Work-around for simics environment for UT test-case
            // Simics not supporting Fast-array, we decided send dummy data as
            // fast-array data to supporting UT-Test data for simics
            // TODO:
            // JIRA:PFSBE-537 Remove fastarray workaround after simics modelling
            if( SBE::isSimicsRunning() )
            {
                uint32_t faBlobSize = 0;
                fapiRc = sbeCtrlFaUtilsGetCtrlDataBlobSize( controlData,
                                                            controlSize,
                                                            faBlobSize );
                if (fapiRc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC "sbeCtrlFaUtilsGetCtrlDataBlobSize failure");
                    break;
                }

                SBE_INFO(SBE_FUNC "sbeCtrlFaUtilsGetCtrlDataBlobSize: "
                                  "faBlobSize: 0x%8X", faBlobSize);
                while (faBlobSize--)
                {
                    l_rc = o_putStream.put(faBlobSize);
                    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
                }
            }
            else
            {
                SBE_EXEC_HWP(fapiRc, poz_fastarray, tgtHndl, controlStream,
                                                                   o_putStream);
            }
        }

        if (controlData)
        {
            SBE_GLOBAL->embeddedArchive.free_file(controlData);
        }

        if(fapiRc != FAPI2_RC_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC" HWP failure: logTargetType [0x%02X] "
                      "chipletId [0x%02X] flags [0x%02X] ",
                      static_cast<uint8_t>(req.logTargetType),
                      static_cast<uint8_t>(req.instanceId),
                      static_cast<uint32_t>(req.flags));
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_FAST_ARRAY_HWP_FAILURE);
            ffdc.setRc(fapiRc);
        }

    } while(false);

    // In case of fastarray chipop SBE will send the response
    if ((( SBE_SEC_OPERATION_SUCCESSFUL == l_rc ) ||
         ( fapiRc != FAPI2_RC_PLAT_ERR_SEE_DATA ))  &&
        ( o_putStream.isStreamRespHeader(respHdr.rcStatus(),ffdc.getRc())) )
    {
        l_rc  = o_putStream.put(o_putStream.words_written());
        // If there was a FIFO error, will skip sending the response,
        // instead give the control back to the command processor thread
        if (l_rc == SBE_SEC_OPERATION_SUCCESSFUL)
        {
            l_rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType() );
        }
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


////////////////////////////////////////////////////////////////////////////////
/// SBE Fast-array utils functions
////////////////////////////////////////////////////////////////////////////////
uint32_t sbeCtrlFaUtilsGetCtrlData( const char * i_fname,
                                    const void* &o_controlData,
                                    uint32_t& o_controlSize)
{
    #define SBE_FUNC "sbeCtrlFaUtilsGetCtrlData "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_secRc = SBE_SEC_OPERATION_SUCCESSFUL;

    do
    {
        // assert checking
        if (i_fname == NULL)
        {
            SBE_ERROR(SBE_FUNC "File name invalid param");
            l_secRc = SBE_SEC_INVALID_PARAMS;
            break;
        }

        // File name formation
        char final_fname[ strlen(FAST_ARRAY_FILE_DIRECTORY) +
                          FAST_ARRAY_FILE_NAME_MAX_LEN] = FAST_ARRAY_FILE_DIRECTORY ;
        strcat(final_fname, i_fname);
        SBE_INFO_BIN(SBE_FUNC "File name final_fname", final_fname, strlen(final_fname));

        // Loading the given file
        l_secRc = SBE_GLOBAL->embeddedArchive.load_file(final_fname, o_controlData, o_controlSize);
        if (l_secRc)
        {
            // Error handling for load file
            SBE_ERROR(SBE_FUNC "Given file load failure, RC: 0x%x", l_secRc);
        }
    }while (0);

    SBE_EXIT(SBE_FUNC);
    return l_secRc;
    #undef SBE_FUNC
}


fapi2::ReturnCode sbeCtrlFaUtilsGetCtrlDataBlobSize ( const void * i_controlData,
                                                  uint32_t i_controlSize,
                                                  uint32_t& o_blobSizeInWords )
{
    #define SBE_FUNC "sbeCtrlFaUtilsGetCtrlDataBlobSize using control data "
    SBE_ENTER(SBE_FUNC);
    fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    o_blobSizeInWords = 0;

    do
    {
        // Calculating control blob size int terms of words
        const uint32_t controlLen =  (i_controlSize + 3) / sizeof(uint32_t);
        // Structure which will give the fast-array blob info
        poz_fastarray_control_info controlInfo __attribute__((aligned(8)))={0};
        // Getting fast-array blob info
        l_fapiRc = poz_fastarray_get_control_info( (const uint32_t *)i_controlData,
                                                   controlLen, controlInfo);
        if (l_fapiRc)
        {
            SBE_ERROR(SBE_FUNC "Fail to execute poz_fastarray_get_control_info"
                                ", RC: 0x%x", l_fapiRc);
            break;
        }

        o_blobSizeInWords = controlInfo.return_data_nwords;
        SBE_INFO(SBE_FUNC "poz_fastarray control blob size: 0x%x words",
                                            controlInfo.return_data_nwords);

    }while(0);

    SBE_EXIT(SBE_FUNC);
    return l_fapiRc;
    #undef SBE_FUNC
}


fapi2::ReturnCode sbeCtrlFaUtilsGetCtrlBlobSize ( const char * i_filename,
                                                  uint32_t& o_blobSizeInWords,
                                                  sbeSecondaryResponse& o_rc )
{
    #define SBE_FUNC "sbeCtrlFaUtilsGetCtrlBlobSize using filename "
    SBE_ENTER(SBE_FUNC);
    fapi2::ReturnCode l_fapiRc = FAPI2_RC_SUCCESS;
    sbeSecondaryResponse l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    const void *controlData   = NULL;

    do
    {
        SBE_INFO_BIN(SBE_FUNC "loading File name ",
                                                i_filename, strlen(i_filename));
        uint32_t controlSize = 0;
        l_rc = (sbeSecondaryResponse) sbeCtrlFaUtilsGetCtrlData( i_filename,
                                                                 controlData,
                                                                 controlSize);
        if (l_rc)
        {
            // Error handling for load file
            SBE_ERROR(SBE_FUNC "sbeCtrlFaUtilsGetCtrlData failure, "
                                "RC: 0x%x", l_rc);
            break;
        }

        l_fapiRc = sbeCtrlFaUtilsGetCtrlDataBlobSize( controlData,
                                                      controlSize,
                                                      o_blobSizeInWords);
    }while(0);

    if (controlData)
    {
        // free loaded file
        SBE_GLOBAL->embeddedArchive.free_file(controlData);
    }

    SBE_EXIT(SBE_FUNC);
    o_rc = l_rc;
    return l_fapiRc;
    #undef SBE_FUNC
}
