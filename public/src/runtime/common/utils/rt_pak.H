/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/utils/rt_pak.H $                    */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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

#pragma once

#include "pakwrapper.H"

/**
 * @brief PakWrapper extensions for runtime code
 */
class RuntimePakWrapper : public PakWrapper
{
public:
    RuntimePakWrapper(void *i_partitionAddress, void* i_partitionEnd) :
        PakWrapper(i_partitionAddress, i_partitionEnd) {}

    /**
     * @brief Read a (possibly compressed) file and return a pointer to its uncompressed data
     *
     * The pointer may be allocated from scratch if the file is compressed, or point
     * right into the embedded archive if not. It must always be freed via free_file.
     *
     * @param[in] i_fileName Pointer to file name to look for.
     * @param[out] i_fileContents Will point to the loaded file contents on success
     * @param[out] o_fileSize Size of the loaded file
     * @param[out] o_hash Pointer to sha3_t, if NULL no hash is returned else hash is returned.
     */
    uint32_t load_file(
        const char* i_fileName, const void* &o_fileContents,
        uint32_t &o_fileSize, sha3_t* o_hash = NULL);

    /**
     * @brief Free a buffer returned by load_file
     */
    void free_file(const void *i_fileContents);
};
