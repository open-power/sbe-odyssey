/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/plat_file_access.C $           */
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

#include "plat_file_access.H"
#include "sbeglobals.H"
#include "heap.H"
#include "archive_config.H"

namespace fapi2
{

ReturnCode plat_loadEmbeddedFile(const char* i_path,
                                void*&o_data, size_t &o_size)
{
#define SBE_FUNC " plat_loadEmbeddedFile "
    SBE_ENTER(SBE_FUNC)

    ReturnCode l_rc = FAPI2_RC_SUCCESS;
    uint32_t uncompressedFileSize = 0x00;
    ARC_RET_t pakRc = ARC_OPERATION_SUCCESSFUL;
    o_data = NULL;
    o_size = 0x0;

    do
    {
        //TODO: JIRA:PFSBE-248 : We could optimize by skipping copying of image if its uncompressed.
        //      But HWP will always call freeembedded API . But we have not allocoated Heap.
        //Locate the file in sppe.ram.pak or pak stack area and fetch the uncompressed file size
        pakRc = SBE_GLOBAL->embeddedArchive.get_image_start_ptr_and_size(
                                        i_path, NULL, &uncompressedFileSize);
        if(pakRc ==  ARC_FILE_NOT_FOUND)
        {
           l_rc = FAPI2_RC_FILE_NOT_FOUND;
           break;
        }
        else if(pakRc != ARC_OPERATION_SUCCESSFUL)
        {
            //TODO: JIRA  PFSBE-251: Need to look into error xml data collection
           l_rc = FAPI2_RC_PLAT_ERR_SEE_DATA;
           break;
        }

        // File size is already returned above .
        // Use the same for allocoating scratch area
        uint32_t *decompressionScratchArea =
                    (uint32_t*)Heap::get_instance().scratch_alloc(uncompressedFileSize);
        if(decompressionScratchArea == NULL)
        {
            FAPI_ERR(SBE_FUNC "scratch allocation failed. Not enough scratch area to allocate");
            l_rc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }

        // Uncompresses/copy the file into the scratch area
        pakRc = SBE_GLOBAL->embeddedArchive.read_file(i_path, decompressionScratchArea,
                                uncompressedFileSize, NULL, NULL);
        if( pakRc != ARC_OPERATION_SUCCESSFUL )
        {
            FAPI_ERR(SBE_FUNC "File read failed with RC[%08X]", pakRc);
            l_rc = FAPI2_RC_PLAT_ERR_SEE_DATA;

            //Free the scratch are as we failed.
            Heap::get_instance().scratch_free(decompressionScratchArea);

            break;
        }

        o_data = decompressionScratchArea;
        o_size = uncompressedFileSize;
    }while(false);

    SBE_EXIT(SBE_FUNC)
    return l_rc;
#undef SBE_FUNC
}

ReturnCode plat_freeEmbeddedFile(void* i_data)
{
#define SBE_FUNC " plat_freeEmbeddedFile "
    SBE_ENTER(SBE_FUNC)

    ReturnCode l_rc = FAPI2_RC_SUCCESS;

    do
    {
        //If HWP has passed a null ptr , dont try to free.
        if(i_data == NULL)
            break;

        Heap::get_instance().scratch_free(i_data);

    }while(false);

    return l_rc;

    SBE_EXIT(SBE_FUNC)
#undef SBE_FUNC
}

}