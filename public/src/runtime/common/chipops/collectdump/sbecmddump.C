/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/collectdump/sbecmddump.C $  */
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
#include "chipop_handler.H"
#include "sbetrace.H"
#include "sbeFifoMsgUtils.H"
#include "sbecollectdump.H"
#include "sbecmddump.H"
#include "sbeCmdGetCapabilities.H"
#include "filenames.H"
#include "sbeffdc.H"
#include "sbeglobals.H"


/**
 * @brief Get EKB commit ID from INFO.txt
 *
 * @param[out] o_commitId 64 Bit commit ID
 * @return Secondary RC
 *
 */
static uint32_t getInfoTxtEkbCommitId(uint64_t &o_commitId)
{
    // Get EKB commit-Id from Info.txt file
    uint32_t buildDate = 0;
    char tag[BUILD_TAG_CHAR_MAX_LENGTH] = {0};
    return (loadAndParseInfoTxt(ekb_info_file_name, CU_IMAGES::EKB, o_commitId, buildDate, tag));
}


uint32_t sbeGetDump( uint8_t *i_pArg )
{
    #define SBE_FUNC "sbeGetDump: "
    SBE_DEBUG(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeGetDumpReq_t req_msg = {};
    sbeResponseFfdc_t ffdc;
    sbeFifoType fifoType;

    const void * hdctLoadFile = nullptr;
    do
    {
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        fifoType = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",fifoType);
        fapi2::sbefifo_hwp_data_istream istream(fifoType);

        // Will attempt to dequeue an entry for the dump Type plus
        // the expected EOT entry at the end.
        uint32_t len2dequeue  = sizeof(req_msg)/sizeof(uint32_t);
        l_rc = istream.get(len2dequeue, (uint32_t *)&req_msg, true, false);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        if(!req_msg.validateDumpType())
        {
            SBE_ERROR(SBE_FUNC "Unsupported/Invalid dump type %x",(uint8_t)req_msg.dumpType);
            respHdr.setStatus(SBE_PRI_INVALID_DATA,SBE_SEC_INVALID_DUMP_TYPE);
            break;
        }

        if(!req_msg.validateClockState())
        {
            SBE_ERROR(SBE_FUNC "Unsupported/Invalid clock state %x",(uint8_t)req_msg.clockState);
            respHdr.setStatus(SBE_PRI_INVALID_DATA,SBE_SEC_INVALID_CLOCK_STATE);
            break;
        }

        if(!req_msg.validateFastArrayCollection())
        {
            SBE_ERROR(SBE_FUNC "Unsupported/Invalid fastarray collecton parameter %x",(uint8_t)req_msg.collectFastArray);
            respHdr.setStatus(SBE_PRI_INVALID_DATA,SBE_SEC_INVALID_FASTARRAY_COLLECTION_INFO);
            break;
        }
        SBE_DEBUG(SBE_FUNC "Validated dump params create DumpObj");

        // Get EKB commit-Id from Info.txt file
        uint64_t commitId = 0;
        uint32_t rc = getInfoTxtEkbCommitId(commitId);
        if (rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "loadAndParseInfoTxt failed for image [%d] from [%s]. "\
                            "RC[0x%08x]", CU_IMAGES::EKB, ekb_info_file_name, rc);

            respHdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE, rc);
            break;
        }

        // Load the HDCT bin file
        uint32_t hdctSize = 0;
        rc = SBE_GLOBAL->embeddedArchive.load_file(hdct_binary_fname, hdctLoadFile, hdctSize);
        if (rc != SBE_SEC_OPERATION_SUCCESSFUL)
        {
            SBE_ERROR(SBE_FUNC "embeddedArchive load_file failed for hdct.bin. "\
                            "SEC_RC[0x%08x]", rc);

            respHdr.setStatus(SBE_PRI_GENERIC_EXECUTION_FAILURE, rc);
            break;
        }

        // Create the sbeCollectDump object
        sbeCollectDump dumpObj( (uint8_t)req_msg.dumpType,
                                (uint8_t)req_msg.clockState,
                                (uint8_t)req_msg.collectFastArray,
                                fifoType,     commitId,
                                hdctLoadFile, hdctSize );
        // Call collectAllEntries
        l_rc = dumpObj.collectAllHDCTEntries();
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
    } while(false);

    if (hdctLoadFile)
    {
        SBE_GLOBAL->embeddedArchive.free_file(hdctLoadFile);
    }

    do
    {
        // Build the response header packet if fifo above is not fail.
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        l_rc = sbeDsSendRespHdr(respHdr, &ffdc, fifoType);
        // will let command processor routine handle the failure.
    }while(0);
    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC

}
