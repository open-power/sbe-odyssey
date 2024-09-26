/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdcodeupdateall.C $  */
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
#include "sbecmdcodeupdate.H"
#include "imgcustomize.H"
#include "sbeupdateimage.H"
#include "codeupdateutils.H"
#include "chipop_struct.H"
#include "sbeutil.H"
#include "plat_hwp_data_stream.H"

using namespace fapi2;

/*
 * @brief updateImageGolden : Get image updated from golden to both side-0/1
 *
 * @param[in] updateImageCmdMsg_t msg : structure for update image
 * @param[in] sbeFifoType type : fifo type
 * @param[out] uint32_t fifoRc : rc for fifo access failure
 * @param[out] bool ackEOT : flag for EOT received or not
 *
 * @return    fapiRc
 */
static
uint32_t updateImageGolden(const CU::updateImageCmdMsg_t *i_msg,
                           const sbeFifoType i_type,
                           uint32_t &o_fifoRc,
                           bool &o_ackEOT)
{
    #define SBE_FUNC " updateImageGolden "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    //////// FILL-UP CODE FOR GOLDEN

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}

////////////////////////////////////////////////////////
//////// Update Image Chip-op //////////////////////////
////////////////////////////////////////////////////////
uint32_t sbeUpdateImageAll (uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeUpdateImageAll "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    CU::updateImageCmdMsg_t msg;
    sbeRespGenHdr_t hdr;
    hdr.init();
    sbeResponseFfdc_t ffdc;
    sbeFifoType type;
    bool ackEOT = false;

    do
    {
        uint8_t l_runningSide = 0, l_nonrunningSide = 0;
        chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
        type = static_cast<sbeFifoType>(configStr->fifoType);
        SBE_DEBUG(SBE_FUNC "Fifo Type is:[%02X]",type);

        // Will attempt to dequeue two entries for the update image
        // plus the expected EOT entry at the end
        uint32_t len2dequeue = sizeof(msg)/sizeof(uint32_t);
        fifoRc = sbeUpFifoDeq_mult (len2dequeue, (uint32_t *)&msg, false, false, type);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        // Get image type and its size to update in nor
        SBE_INFO(SBE_FUNC "ImageType: 0x%04x , ImageSize(bytes): 0x%08x",
                 msg.imageType, WORD_TO_BYTES(msg.imageSizeInWords));

        // Check for validity of input parameters
        rc = msg.checkInputParamsValid();
        if (rc)
        {
            hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, rc );
            break;
        }

        // Get booted side
        getSideInfo(l_runningSide, l_nonrunningSide);

        if (l_runningSide == GOLDEN_SIDE_INDEX)
        {
            // call update image chip-op function (golden)
            rc = updateImageGolden(&msg, type, fifoRc, ackEOT);
            if (rc)
            {
                hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE, rc );
            }
        }
        else
        {
            // call update image chip-op function
            fapiRc = updateImage(&msg, type, &hdr, fifoRc, ackEOT);

            if (hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                if(fapiRc != FAPI2_RC_SUCCESS)
                {
                    ffdc.setRc(fapiRc);
                }
                break;
            }
            else
            {
                if(fapiRc != FAPI2_RC_SUCCESS)
                {
                    ffdc.setRc(fapiRc);
                    hdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                            SBE_SEC_CU_UPDATE_IMAGE_FAILURE );
                    break;
                }
            }
        }
    } while(false);

    do
    {
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        if ((hdr.secondaryStatus() != SBE_SEC_OPERATION_SUCCESSFUL) &&
            (ackEOT == false))
        {
            // flush out fifo
            uint32_t len2dequeue = 0;
            fifoRc = sbeUpFifoDeq_mult (len2dequeue, NULL, true, true, type);
        }
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);
        // Build the response header packet
        fifoRc = sbeDsSendRespHdr(hdr, &ffdc, type);
    } while(false);

    SBE_EXIT(SBE_FUNC);
    return fifoRc;
    #undef SBE_FUNC
}
