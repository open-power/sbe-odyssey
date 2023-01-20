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

using namespace fapi2;


ReturnCode sbestreampaktohwp(PakWrapper *i_pak, const char * i_pakname, voidfuncptr_t i_hwp, uint8_t i_Image)
{
    #define SBE_FUNC " sbestreampaktohwp "
    SBE_ENTER(SBE_FUNC);

    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    ARC_RET_t pakRc = ARC_OPERATION_SUCCESSFUL;
    uint32_t *scratchArea = NULL;
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
                                        NULL, &size);
        if( pakRc )
        {
            SBE_ERROR(SBE_FUNC "read_file failed with RC[%08X]", pakRc);
            fapiRc = FAPI2_RC_PLAT_ERR_SEE_DATA;
            break;
        }
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

    // Deallocate the scratch space.
    Heap::get_instance().scratch_free(scratchArea);

    SBE_EXIT(SBE_FUNC);
    return fapiRc;
    #undef SBE_FUNC
}
