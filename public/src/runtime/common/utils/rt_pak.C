/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/utils/rt_pak.C $                    */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#include "rt_pak.H"
#include "heap.H"

uint32_t RuntimePakWrapper::load_file(
    const char* i_fileName, const void* &o_fileContents,
    uint32_t &o_fileSize, sha3_t* o_hash, uint32_t i_flags)
{
    // Load safe values into output parameters until we're done
    o_fileContents = NULL;
    o_fileSize = 0;

    if (i_fileName == nullptr)
        return ARC_INVALID_PARAMS;

    uint32_t rc = locate_file(i_fileName, fileArchiveEntry, i_flags);
    if (rc != ARC_OPERATION_SUCCESSFUL)
        return rc;

    // For uncompressed files, just return a pointer right into the pak
    if (!fileArchiveEntry.is_compressed())
    {
        rc = fileArchiveEntry.get_stored_data_ptr(o_fileContents);
        if (rc != ARC_OPERATION_SUCCESSFUL)
            return rc;

        o_fileSize = fileArchiveEntry.get_size();
        return ARC_OPERATION_SUCCESSFUL;
    }

    // For compressed files, allocate scratch space and decompress
    void *buffer = Heap::get_instance().scratch_alloc(fileArchiveEntry.get_size());
    if (!buffer)
        return SBE_SEC_HEAP_SPACE_FULL_FAILURE;

    rc = fileArchiveEntry.decompress(buffer, fileArchiveEntry.get_size(), o_hash);
    if (rc != ARC_OPERATION_SUCCESSFUL)
    {
        Heap::get_instance().scratch_free(buffer);
        return rc;
    }

    o_fileContents = buffer;
    o_fileSize = fileArchiveEntry.get_size();
    return ARC_OPERATION_SUCCESSFUL;
}

void RuntimePakWrapper::free_file(const void *i_fileContents)
{
    if (Heap::get_instance().is_scratch_pointer(i_fileContents))
        Heap::get_instance().scratch_free(i_fileContents);
}
