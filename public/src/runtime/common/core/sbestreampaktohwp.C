/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbestreampaktohwp.C $          */
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
#include "globals.H"
#include "sbeglobals.H"
#include "archive.H"
#include "metadata.H"

using namespace fapi2;

ARC_RET_t HwpStreamReceiver::consume(const void* i_data, uint32_t i_size)
{
    SBE_INFO("PakStreamReceiver::consume: i_size is 0x%08X and iv_offset is 0x%08X", i_size, iv_offset);

    // Execute the HWP.
    if(iv_hwp != NULL)
    {
        iv_fapiRc = sbeexecutehwponpak(iv_hwp, (uint8_t *)i_data,
                                       i_size, iv_offset, iv_image);
        if (iv_fapiRc)
        {
            SBE_ERROR("PakStreamReceiver::consume: sbeexecutehwponpak failed with RC[%08X]", iv_fapiRc);
        }
    }
    iv_offset += i_size;

    return iv_fapiRc;
}

ARC_RET_t PpeImageReceiver::consume(const void* i_data, uint32_t i_size)
{
    if (iv_offset == 0)
    {
        const uint8_t *meta_start = ((uint8_t *)i_data) + 0x200;
        const META_HEA_t *hea = GET_META_HEA(meta_start);
        const META_GIT_t *git = GET_META_GIT(meta_start);

        if (!(hea && git))
        {
            SBE_ERROR("PpeImageReceiver::consume: Required metadata missing in image");
            iv_fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            return ARC_INVALID_PARAMS;
        }

        iv_heapStart = hea->heapAddr;
        iv_gitId = git->commitId;
    }

    return HwpStreamReceiver::consume(i_data, i_size);
}

ReturnCode sbestreampaktohwp(
    PakWrapper *i_pak, const char * i_pakname,
    HwpStreamReceiver &i_receiver)
{
    #define SBE_FUNC " sbestreampaktohwp "
    SBE_ENTER(SBE_FUNC);

    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    ARC_RET_t pakRc = ARC_OPERATION_SUCCESSFUL;
    uint32_t *scratchArea = NULL;
    sha3_t hashData;

    // load the entirety of the stream, build actual hash value
    do
    {
        sha3_t hashData;

        // Locate the File.
        FileArchive::Entry entry;
        pakRc = i_pak->locate_file(i_pakname, entry);
        if (pakRc)
        {
            SBE_ERROR(SBE_FUNC "Locate file failed with pakRc 0x%08X", pakRc);
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }

        SBE_INFO(SBE_FUNC "Size of the binary is 0x%08X", entry.get_size());

        // Allocating scratch
        scratchArea =
            (uint32_t*)Heap::get_instance().scratch_alloc(FileArchive::STREAM_SCRATCH_SIZE);
        if(scratchArea == NULL)
        {
            SBE_ERROR(SBE_FUNC "scratch allocation failed.");
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }

        // Call stream_decompress
        pakRc = entry.stream_decompress(i_receiver, scratchArea, (sha3_t *)hashData);
	
	// Deallocate the scratch space.
        Heap::get_instance().scratch_free(scratchArea);
	
        if(pakRc)
        {
            SBE_ERROR(SBE_FUNC "stream_decompress failed with RC[%08X]", pakRc);
            fapiRc = i_receiver.getFapiRc();
            break;
        }
    } while(false);
    
    // validate that hash value is as expected
    if (fapiRc == FAPI2_RC_SUCCESS)
    {
        /* In case hash mismatch from pak img, check_file_hash will return the miss-match hash*/
        uint64_t *ptrMismatchHash;
        uint64_t *ptrGenHash = (uint64_t *) hashData;

        /* Checking the image hash */
        uint32_t hashListRc = SBE::check_file_hash( i_pakname, hashData,
                                                    &g_hash_list,
                                                    (sha3_t **)&ptrMismatchHash);
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
    }

fapi_try_exit:
    
    // Assign fapiRc for failure case, Assign current_err for PLAT_FAPI_ASSERT
    fapiRc = (fapiRc != FAPI2_RC_SUCCESS)? fapiRc : fapi2::current_err;

    SBE_EXIT(SBE_FUNC);
    return fapiRc;
    #undef SBE_FUNC
}
