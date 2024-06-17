/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/metadata_base.C $                     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#include "metadata_base.H"
#include "sbetrace.H"

const void *_get_metadata(const void *start, uint32_t tag)
{
    uint32_t *ptr = (uint32_t *)start;
    // Limit the search to 32KiB to prevent overruns
    const uint32_t *limit = ((uint32_t *)start) + 0x2000;

    while (*ptr && ptr < limit) {
        ImageMetadataHeader *hdr = (ImageMetadataHeader *)ptr++;
        if (hdr->tag == tag)
        {
            return ptr;
        }
        ptr += hdr->len;
    }
    return nullptr;
}

void populateMetaData(uint32_t i_metadata_ptr, uint32_t i_metaDataTag,
                      const void *i_metaData, size_t i_metaDataSizeInBytes)
{
    #define SBE_FUNC " populateMetaData "
    SBE_ENTER(SBE_FUNC)

    // Find if the meta data tag is present
    uint8_t *l_metaDataPtr = (uint8_t*)_get_metadata((const void*)i_metadata_ptr,i_metaDataTag);
    if(l_metaDataPtr == NULL)
    {
        SBE_ERROR("Meta Data Tag 0x%08x not found", i_metaDataTag);
        SBE_ERROR("Halting PPE...");
        pk_halt();
    }

    // Check for the meta data size
    uint8_t *l_metaDataSize = l_metaDataPtr-1;
    if(WORD_TO_BYTES(*l_metaDataSize) != i_metaDataSizeInBytes)
    {
        SBE_ERROR("Expected meta data tag 0x%08x len : [%d], Actual len : [%d]",
                    i_metaDataTag,i_metaDataSizeInBytes, WORD_TO_BYTES(*l_metaDataSize));
        SBE_ERROR("Halting PPE...");
        pk_halt();
    }

    // Populate the meta data
    memcpy(l_metaDataPtr, i_metaData, i_metaDataSizeInBytes);

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}
