/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbestreampaktohwp.C $    */
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

#include "sbestreampaktohwp.H"
#include "heap.H"
#include "hwpWrapper.H"
#include "odysseylink.H"

using namespace fapi2;

ReturnCode sbestreampaktohwp(PakWrapper *i_pak, const char * i_pakname, voidfuncptr_t i_hwp, uint8_t i_Image)
{
    #define SBE_FUNC " sbestreampaktohwp "
    SBE_ENTER(SBE_FUNC);

    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    ARC_RET_t pakRc = ARC_OPERATION_SUCCESSFUL;
    uint32_t *scratchArea = NULL;
    sha3_t hashData;

    do
    {
        uint32_t binSize = 0;
        uint32_t offset = 0;
        uint32_t size = 0;

        pakRc = i_pak->get_image_start_ptr_and_size(i_pakname, NULL, &binSize);
        if (pakRc)
        {
            SBE_ERROR(SBE_FUNC "get_image_start ptr Failed with pakRc 0x%08X", pakRc);
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }
        SBE_INFO(SBE_FUNC "Size of the binary is 0x%08X", binSize);

        // Allocating scratch
        scratchArea =
                (uint32_t*)Heap::get_instance().scratch_alloc(binSize);
        if(scratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "scratch allocation failed.");
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }

        SBE_INFO(SBE_FUNC "fileSize is 0x%08X", binSize);

        /// TODO: JIRA: PFSBE-270 pakwrapper utils functions for stream class
        pakRc = i_pak->read_file(i_pakname, scratchArea, binSize,
                                        &hashData, &size);
        if( pakRc )
        {
            SBE_ERROR(SBE_FUNC "read_file failed with RC[%08X]", pakRc);
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }

        /* In case hash mismatch from pak img, check_file_hash will return the miss-match hash*/
        uint32_t mismatchHashAddr;
        uint64_t * ptrGenHash = (uint64_t *) hashData;
        uint64_t * ptrMismatchHash = (uint64_t *) mismatchHashAddr;

        /* Checking the image hash */
        uint32_t hashListRc = SBE::check_file_hash( i_pakname, hashData,
                                                    (uint8_t *) HASH_LIST_START_OFFSET, (uint8_t *) &mismatchHashAddr);
        if (hashListRc != SBE::HASH_COMPARE_PASS)
        {
            /* ERROR message */
            SBE_ERROR(SBE_FUNC "check_file_hash failure rc: 0x%X", hashListRc);
            SBE_ERROR_BIN(SBE_FUNC "check_file_hash failure file name: ", i_pakname, strlen(i_pakname));
        }

        /* Check the SBE class RC with HASH_COMPARE_FAIL, assert with FAPI */
        PLAT_FAPI_ASSERT( !(hashListRc == SBE::HASH_COMPARE_FAIL),
                    SBE_FILE_HASH_MISMATCH().
                    set_GEN_HASH_0(* (ptrGenHash + 0)).
                    set_GEN_HASH_1(* (ptrGenHash + 1)).
                    set_GEN_HASH_2(* (ptrGenHash + 2)).
                    set_GEN_HASH_3(* (ptrGenHash + 3)).
                    set_GEN_HASH_4(* (ptrGenHash + 4)).
                    set_GEN_HASH_5(* (ptrGenHash + 5)).
                    set_GEN_HASH_6(* (ptrGenHash + 6)).
                    set_GEN_HASH_7(* (ptrGenHash + 7)).
                    set_CMP_HASH_0(* (ptrMismatchHash + 0)).
                    set_CMP_HASH_1(* (ptrMismatchHash + 1)).
                    set_CMP_HASH_2(* (ptrMismatchHash + 2)).
                    set_CMP_HASH_3(* (ptrMismatchHash + 3)).
                    set_CMP_HASH_4(* (ptrMismatchHash + 4)).
                    set_CMP_HASH_5(* (ptrMismatchHash + 5)).
                    set_CMP_HASH_6(* (ptrMismatchHash + 6)).
                    set_CMP_HASH_7(* (ptrMismatchHash + 7)),
                    "sbestreampaktohwp: Pak file hash mismatch");

        /* Check the SBE class RC with FILE_NOT_FOUND, assert with FAPI */
        PLAT_FAPI_ASSERT( !(hashListRc == SBE::FILE_NOT_FOUND),
                    SBE_FILE_NOT_FOUND(),
                    "sbestreampaktohwp: Pak file not found in hash list");

        // Call the HWP
        if(i_hwp != NULL)
        {
            fapiRc = sbeexecutehwponpak(i_hwp, (uint8_t *)scratchArea,
                                        binSize, offset, i_Image);
            if(fapiRc)
            {
                SBE_ERROR(SBE_FUNC "sbeexecutehwponpak failed with RC[%08X]", fapiRc);
                break;
            }
        }

    }while(0);

fapi_try_exit:
    // Assign fapiRc for failure case, Assign current_err for PLAT_FAPI_ASSERT
    fapiRc = (fapiRc != FAPI2_RC_SUCCESS)? fapiRc : fapi2::current_err;

    // Deallocate the scratch space.
    Heap::get_instance().scratch_free(scratchArea);

    SBE_EXIT(SBE_FUNC);
    return fapiRc;
    #undef SBE_FUNC
}
