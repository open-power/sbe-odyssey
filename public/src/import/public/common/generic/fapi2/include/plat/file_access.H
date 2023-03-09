/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/include/plat/file_access.H $ */
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
/// @file file_access.H
///
/// @brief File access functions that need to be specialized for platform implementation
///

#pragma once

#include <fapi2_file_access.H>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

namespace fapi2
{

/// @brief Load a file from the embedded archive
/// Attempts to locate a file with given name inside the embedded archive
/// and makes it available in memory.
/// @param[in]  i_target Chip target associated with the access;
///                      platforms may use this to pick an archive to search
/// @param[in]  i_path   Full path name of the file to look for
/// @param[out] o_data   Will point to the file data on success
/// @param[out] o_size   Will receive the file size on success
/// @param[in]  i_flags  Optional flags to affect the operation
/// @return FAPI2_RC_SUCCESS on success,
///         FAPI2_RC_FILE_NOT_FOUND if the file was not found, or another error.
inline ReturnCode loadEmbeddedFile(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                                   const char* i_path,
                                   const void*& o_data, size_t& o_size,
                                   uint32_t i_flags)
{
    std::cout << "   loadEmbeddedFile(" << i_path << ", " << i_flags << ")" << std::endl;

    FILE* f = fopen(i_path, "r");

    if (!f)
    {
        std::cout << "      FILE NOT FOUND" << std::endl;
        o_data = NULL;
        o_size = 0;
        return FAPI2_RC_FILE_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    o_size = ftell(f);
    void* data = malloc(o_size);

    if (!data)
    {
        std::cout << "      OUT OF MEMORY (size: " << o_size << " bytes)" << std::endl;
        return FAPI2_RC_PLAT_ERR_SEE_DATA;
    }

    fseek(f, 0, SEEK_SET);

    if (fread(data, 1, o_size, f) != o_size)
    {
        std::cout << "      READ ERROR (size: " << o_size << " bytes)" << std::endl;
        return FAPI2_RC_PLAT_ERR_SEE_DATA;
    }

    o_data = data;
    std::cout << "      SUCCESS (size: " << o_size << " bytes)" << std::endl;
    return FAPI2_RC_SUCCESS;
}

/// @brief Free memory allocated by loadEmbeddedFile()
/// @param[in] i_data  Pointer to file contents
/// @return FAPI2_RC_SUCCESS on success, else error code
inline ReturnCode freeEmbeddedFile(const void* i_data)
{
    free(const_cast<void*>(i_data));
    return FAPI2_RC_SUCCESS;
}

}
