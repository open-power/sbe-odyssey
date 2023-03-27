/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdsramaaccesswrap.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "ody_getsram.H"
#include "ody_putsram.H"


using namespace fapi2;


// // Function pointer definition for HWP call support
ody_getsram_FP_t odyGetSramFp = ody_getsram;
ody_putsram_FP_t odyPutSramFp = ody_putsram;



ReturnCode sbeSramAccessReqMsgHdr_t::executeHwp( const uint64_t i_offset,
                                                 const uint32_t i_bytes,
                                                 uint8_t* io_data,
                                                 const bool i_isGetFlag )
{
    #define SBE_FUNC " executeHwp "
    uint32_t fapiRc = FAPI2_RC_SUCCESS;

    /* Execute odyssey HWP */
    Target<SBE_ROOT_CHIP_TYPE>l_fapiTarget = g_platTarget->plat_getChipTarget();

    // For IO-PPE SRAM access,caller will pass valid offset/address to be accessed
    // in the lower 32 bits. Procedure expects valid offset/address information
    // to be in upper 32 bits. Updating the address as per the HWP requirement
    uint64_t ody_offset = i_offset << 32;

    if (i_isGetFlag)
    {
        SBE_INFO(SBE_FUNC "Get Sram HWP execution..." );

        // Execute Trace Array HWP
        SBE_EXEC_HWP(fapiRc, odyGetSramFp, l_fapiTarget, ody_offset, i_bytes, io_data);

    }
    else
    {
        SBE_INFO(SBE_FUNC "Put Sram HWP execution..." );

        // Execute Trace Array HWP
        SBE_EXEC_HWP(fapiRc, odyPutSramFp, l_fapiTarget, ody_offset, i_bytes, io_data);

    }

    return fapiRc;
    #undef SBE_FUNC
}