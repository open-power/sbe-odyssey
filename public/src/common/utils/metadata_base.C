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
