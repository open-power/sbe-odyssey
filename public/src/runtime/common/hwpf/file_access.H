/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/file_access.H $                */
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

/// @file file_access.H
///
/// @brief File access functions that need to be specialized for platform implementation
///

#pragma once

#include "plat_file_access.H"

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
/// @return FAPI2_RC_SUCCESS on success,
///         FAPI2_RC_FILE_NOT_FOUND if the file was not found, or another error.
inline ReturnCode loadEmbeddedFile(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                                   const char* i_path,
                                   void*& o_data, size_t& o_size)
{
    return plat_loadEmbeddedFile(i_path, o_data, o_size);
}

// placeholder variant to prepare for an upcoming fapi2 update
inline ReturnCode loadEmbeddedFile(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                                   const char* i_path,
                                   void*& o_data, size_t& o_size,
                                   uint32_t i_flags)
{
    return plat_loadEmbeddedFile(i_path, o_data, o_size);
}

/// @brief Free memory allocated by loadEmbeddedFile()
/// @param[in] i_data  Pointer to file contents
/// @return FAPI2_RC_SUCCESS on success, else error code
inline ReturnCode freeEmbeddedFile(void* i_data)
{
    return plat_freeEmbeddedFile(i_data);
}

}
