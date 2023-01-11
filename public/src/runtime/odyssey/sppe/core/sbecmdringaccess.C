/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdringaccess.C $     */
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
/*
 * @file: ppe/sbe/sbefw/sbecmdringaccess.C
 *
 * @brief This file contains the SBE Ring Access chipOps
 *
 */

#include "sbecmdringaccess.H"
#include "sbefifo.H"
#include "sbeSpMsg.H"
#include "sbe_sp_intf.H"
#include "sbetrace.H"
#include "sbeFifoMsgUtils.H"
#include "sbeutil.H"
#include "target.H"
#include "plat_hw_access.H"
#include "plat_hwp_data_stream.H"
#include "sbeglobals.H"
#include "chipop_handler.H"
#include "scan_compression.H"
#include "base_utils.H"
#include "poz_putRingUtils.H"
#include "plat_ring_utils.H"
#include "poz_putRingBackend.H"
#include "heap.H"

using namespace fapi2;

static const uint32_t SIZE_OF_LENGTH_INWORDS = 1;
static const uint32_t NUM_WORDS_PER_GRANULE = 2;
static const uint32_t GETRING_GRANULE_SIZE_IN_BITS = 64;

uint16_t sbeToFapiRingMode(uint16_t i_ringMode)
{
    uint16_t l_fapiRingMode = RING_MODE_HEADER_CHECK;

    if(i_ringMode & SBE_RING_MODE_SET_PULSE_NO_OPCG_COND)
    {
        l_fapiRingMode |= RING_MODE_SET_PULSE_NO_OPCG_COND;
    }
    if(i_ringMode & SBE_RING_MODE_NO_HEADER_CHECK)
    {
        l_fapiRingMode |= RING_MODE_NO_HEADER_CHECK;
    }
    if(i_ringMode & SBE_RING_MODE_SET_PULSE_NSL)
    {
        l_fapiRingMode |= RING_MODE_SET_PULSE_NSL;
    }
    if(i_ringMode & SBE_RING_MODE_SET_PULSE_SL)
    {
        l_fapiRingMode |= RING_MODE_SET_PULSE_SL;
    }
    if(i_ringMode & SBE_RING_MODE_SET_PULSE_ALL)
    {
        l_fapiRingMode |= RING_MODE_SET_PULSE_ALL;
    }
    if(i_ringMode & SBE_RING_MODE_FASTARRAY)
    {
        l_fapiRingMode |= RING_MODE_FASTARRAY;
    }
    return l_fapiRingMode;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeGetRingWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                        fapi2::sbefifo_hwp_data_ostream& i_putStream)
{
    #define SBE_FUNC " sbeGetRingWrap "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeGetRingAccessMsgHdr_t l_reqMsg;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t l_ffdc;
    ReturnCode l_fapiRc;
    uint32_t l_len = 0;
    // Note-Read operation flow is SHIFT and then READ.
    // First time , the shift count will be 0.. because of the way it
    // works. if we shift 64bits in the very first iteration then we
    // loose first 64 bit. But still we should update l_bitSentCnt
    // because we are sending back this data
    uint32_t l_bitSentCnt = 64;
    const uint32_t LONG_ROTATE_ADDRESS = 0x00039000;
    do
    {
        // Get the ring access header
        l_len  = sizeof(sbeGetRingAccessMsgHdr_t)/sizeof(uint32_t);
        l_rc = i_getStream.get(l_len, (uint32_t *)&l_reqMsg);

        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        SBE_INFO(SBE_FUNC "Ring Address 0x%08X User Ring Mode 0x%04X "
            "Length in Bits 0x%08X",
            (uint32_t)l_reqMsg.ringAddr,
            (uint32_t)l_reqMsg.ringMode,
            (uint32_t)l_reqMsg.ringLenInBits);

        uint16_t l_ringMode = sbeToFapiRingMode(l_reqMsg.ringMode);

        // Call getRing_setup - loads the scan region data for the given ring
        // address and updates the check word data
        l_fapiRc = fapi2::getRing_setup(l_reqMsg.ringAddr,
                                       (fapi2::RingMode)l_ringMode);
        if( l_fapiRc != FAPI2_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC" getRing_setup failed. RingAddress: 0x%08X "
                        "RingMode: 0x%04X, fapiRc: 0x%08X", l_reqMsg.ringAddr,
                         l_ringMode, l_fapiRc);
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_GETRING_SETUP_FAILED);
            l_ffdc.setRc(l_fapiRc);
            break;
         }
         // Calculate the iteration length
         uint32_t l_loopCnt =
                  (l_reqMsg.ringLenInBits / GETRING_GRANULE_SIZE_IN_BITS);
         // Check for modulus - remainder
         uint8_t l_mod = (l_reqMsg.ringLenInBits % GETRING_GRANULE_SIZE_IN_BITS);
         if(l_mod)
         {
             ++l_loopCnt;
         }
        // fix for the alignment issue
        uint32_t l_buf[NUM_WORDS_PER_GRANULE]__attribute__ ((aligned (8))) ={0};
        uint32_t l_bitShift = 0;
        l_len = NUM_WORDS_PER_GRANULE;
        Target<SBE_ROOT_CHIP_TYPE> l_hndl =  g_platTarget->plat_getChipTarget();
        uint32_t l_chipletId = (uint32_t)(l_reqMsg.ringAddr) & 0xFF000000;
        uint32_t l_scomAddress = 0;

        // Fetch the ring data in bits, each iteration will give you 64bits
        for(uint32_t l_cnt=0; l_cnt < l_loopCnt; l_cnt++)
        {
            l_scomAddress = LONG_ROTATE_ADDRESS | l_chipletId;
            l_scomAddress |= l_bitShift;

            l_fapiRc = getscom_abs_wrap (&l_hndl,
                                         l_scomAddress,
                                         (uint64_t*)&l_buf);
            if( l_fapiRc != FAPI2_RC_SUCCESS )
            {
                SBE_ERROR(SBE_FUNC" getRing_granule_data failed. "
                    "RingAddress:0x%08X RingMode:0x%04X fapiRc:0x%08X",
                    l_reqMsg.ringAddr, l_ringMode, l_fapiRc);
                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                        SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
                l_ffdc.setRc(l_fapiRc);
                break;
            }
            // if the length of ring is not divisible by 64 then mod value
            // should be considered which will match with the length in bits
            // that passed
            if((l_cnt == (l_loopCnt -1)) && (l_mod))
            {
                l_bitShift = l_mod;
            }
            // Send it to DS Fifo
            // If this is the last iteration in the loop, let the full 64bit
            // go, even for 1bit of remaining length. The length passed to
            // the user will take care of actual number of bits.
            l_rc = i_putStream.put(l_len, (uint32_t *)&l_buf);
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            l_bitSentCnt = l_bitSentCnt + l_bitShift;
            l_bitShift = GETRING_GRANULE_SIZE_IN_BITS;
        }
        if ( (l_fapiRc == FAPI2_RC_SUCCESS) &&
             (l_rc == SBE_SEC_OPERATION_SUCCESSFUL) )
        {
            if (!l_mod)
            {
                l_mod = GETRING_GRANULE_SIZE_IN_BITS;
            }
            //Here we need to shift with the mod value to enter into the
            //starting position of the ring.But the data is already read in the
            //above for loop.. so here we ignore the data
            l_scomAddress = LONG_ROTATE_ADDRESS | l_chipletId;
            l_scomAddress |= l_mod;
            l_fapiRc = getscom_abs_wrap (&l_hndl,
                                         l_scomAddress,
                                         (uint64_t*)&l_buf);
            if( l_fapiRc != FAPI2_RC_SUCCESS )
            {
                SBE_ERROR(SBE_FUNC" getRing_granule_data failed. "
                       "RingAddress:0x%08X RingMode:0x%04X fapiRc:0x%08X",
                        l_reqMsg.ringAddr, l_ringMode, l_fapiRc);
                    respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                        SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
               l_ffdc.setRc(l_fapiRc);
               break;
            }

            // Call getRing_verifyAndcleanup - verify the check word data is
            // matching or not and will clean up the scan region data
            l_fapiRc = getRing_verifyAndcleanup((uint32_t)(l_reqMsg.ringAddr),
                                        (fapi2::RingMode)l_ringMode);
            if( l_fapiRc != FAPI2_RC_SUCCESS )
            {
                SBE_ERROR(SBE_FUNC" getRing_verifyAndcleanup failed. "
                    "RingAddress:0x%08X RingMode:0x%04X fapiRc:0x%08X",
                    l_reqMsg.ringAddr, l_ringMode, l_fapiRc);
                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                   SBE_SEC_GETRING_VERIFY_CLEANUP);
                l_ffdc.setRc(l_fapiRc);
            }
        }
    }while(false);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( (SBE_SEC_OPERATION_SUCCESSFUL == l_rc) &&
         (i_putStream.isStreamRespHeader( respHdr.rcStatus(),l_ffdc.getRc())) )
    {
        l_rc  = i_putStream.put(l_bitSentCnt);
        if( (SBE_SEC_OPERATION_SUCCESSFUL == l_rc) )
        {
            l_rc = sbeDsSendRespHdr( respHdr, &l_ffdc,
                                     i_getStream.getFifoType() );
        }
    }
    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbeGetRing(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeGetRing "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbeGetRingWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbePutRingWrap( fapi2::sbefifo_hwp_data_istream& i_getStream,
                         fapi2::sbefifo_hwp_data_ostream& i_putStream )
{
#define SBE_FUNC " sbePutRingWrap "
    SBE_ENTER(SBE_FUNC);

    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    ReturnCode fapiRc;
    sbePutRingMsgHdr_t hdr;
    uint32_t len = 0;
    uint32_t *rs4ScratchArea = NULL;
    do
    {
        // Get the length of payload
        // Length is not part of chipop. So take length from total length
        len = SBE_GLOBAL->sbeFifoCmdHdr.len -
                        sizeof(SBE_GLOBAL->sbeFifoCmdHdr)/sizeof(uint32_t);

        // Get the length for RS4 payload in words
        uint32_t rs4PayloadSize = len -
                        sizeof(sbePutRingMsgHdr_t)/sizeof(uint32_t);

        // Get the length for RS4 payload in bytes
        uint32_t rs4PayloadSizeBytes = rs4PayloadSize * sizeof(uint32_t);
        SBE_INFO(SBE_FUNC" rs4PayloadSize in words: [0x%08X] in bytes: [0x%08X]",
                        rs4PayloadSize, rs4PayloadSizeBytes);

        //Allocoate heap area based on the rs4 payload length in bytes
        rs4ScratchArea =
                    (uint32_t*)Heap::get_instance().scratch_alloc(rs4PayloadSizeBytes);
        if(rs4ScratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "scratch allocation failed. Not enough scratch area to allocate");
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_HEAP_SPACE_FULL_FAILURE);

            // flush the fifo
            rc = i_getStream.get(len, NULL,true, true);
            break;
        }

        // Get the message header
        len = sizeof(sbePutRingMsgHdr_t)/sizeof(uint32_t);
        rc = i_getStream.get(len, (uint32_t *)&hdr, false);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);
        // Initialize with HEADER CHECK mode
        uint16_t ringMode = sbeToFapiRingMode(hdr.ringMode);

        // Get the rs4 payload
        rc = i_getStream.get(rs4PayloadSize, rs4ScratchArea);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        SBE_INFO(SBE_FUNC" ringMode: [0x%04X]", ringMode);

        Target<SBE_ROOT_CHIP_TYPE> l_hndl = g_platTarget->plat_getChipTarget();
        auto l_allgood_mc =
              l_hndl.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD);

        fapiRc = poz_applyCompositeImage(l_allgood_mc, rs4ScratchArea,
                                         rs4PayloadSizeBytes, (fapi2::RingMode)ringMode);
        if( fapiRc != FAPI2_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC" plat_putringutil failed."
                "fapiRc:0x%04X",  fapiRc);
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
            ffdc.setRc(fapiRc);
            break;
        }
    }while(false);

    //Free the scratch area
    Heap::get_instance().scratch_free(rs4ScratchArea);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( SBE_SEC_OPERATION_SUCCESSFUL == rc )
    {
        rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType());
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
#undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbePutRing(uint8_t *i_pArg)
{
#define SBE_FUNC " sbePutRing "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbePutRingWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
uint32_t sbePutRingFromImgWrap(fapi2::sbefifo_hwp_data_istream& i_getStream,
                               fapi2::sbefifo_hwp_data_ostream& i_putStream)
{
#define SBE_FUNC " sbePutRingFromImgWrap "
    SBE_ENTER(SBE_FUNC);

    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;
    ReturnCode fapiRc;
    sbePutRingFromImgCMD_t hdr;
    uint32_t len = 0;

    do{
        len = sizeof(sbePutRingFromImgCMD_t)/sizeof(uint32_t);
        rc = i_getStream.get(len, (uint32_t *)&hdr);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(rc);

        // Initialize with HEADER CHECK mode
        uint16_t ringMode = sbeToFapiRingMode(hdr.ringMode);

        Target<SBE_ROOT_CHIP_TYPE> l_hndl = g_platTarget->plat_getChipTarget();
        auto l_allgood_mc =
              l_hndl.getMulticast<fapi2::TARGET_TYPE_PERV>(fapi2::MCGROUP_GOOD);

        SBE_EXEC_HWP(fapiRc,
                     putRing,
                     l_allgood_mc, hdr.ringIdString,
                     (fapi2::RingMode)ringMode);

        if( fapiRc != FAPI2_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC"  poz_putRingBackend failed."
                "fapiRc:0x%04x",  fapiRc);
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                               SBE_SEC_PUTRING_FAILED);
            ffdc.setRc(fapiRc);
            break;
        }
    }while(false);

    // Now build and enqueue response into downstream FIFO
    // If there was a FIFO error, will skip sending the response,
    // instead give the control back to the command processor thread
    if ( SBE_SEC_OPERATION_SUCCESSFUL == rc )
    {
        rc = sbeDsSendRespHdr( respHdr, &ffdc, i_getStream.getFifoType());
    }

    SBE_EXIT(SBE_FUNC);
    return rc;
#undef SBE_FUNC
}

uint32_t sbePutRingFromImg(uint8_t *i_pArg)
{
#define SBE_FUNC " sbePutRingFromImg "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    sbefifo_hwp_data_ostream ostream(type);
    sbefifo_hwp_data_istream istream(type);
    SBE_INFO(SBE_FUNC" hwp streams created");
    l_rc = sbePutRingFromImgWrap( istream, ostream );

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}
