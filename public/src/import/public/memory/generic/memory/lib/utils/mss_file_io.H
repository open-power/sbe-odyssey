/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/generic/memory/lib/utils/mss_file_io.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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

///
/// @file mss_file_io.H
/// @brief Tools to handle file I/O to a wrapper or tool
/// @note DO NOT MIRROR TO HOSTBOOT - this function is only used in HWP wrappers
///
// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
// *HWP HWP Backup: Stephen Glancy <sglancy@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: CI

#ifndef _MSS_FILEIO_H_
#define _MSS_FILEIO_H_

#include <fapi2.H>
#include <fstream>
#include <sys/stat.h>

namespace mss
{

///
/// @brief Read data from file
/// @param[in] i_file_name The file name
/// @param[out] o_image_data The data
/// @param[out] o_image_size The size
/// @return ECMD_SUCCESS if successful
///
inline uint32_t read_file(const char* i_file_name, uint8_t*& o_image_data, size_t& o_image_size)
{
    struct stat filestat;
    char outstr[256];

    if (i_file_name == NULL)
    {
        snprintf(outstr, sizeof(outstr), "Filename is null!\n");
        ecmdOutputError(outstr);
        return ECMD_FATAL_FAILURE;
    }

    if (stat(i_file_name, &filestat) != 0)
    {
        snprintf(outstr, sizeof(outstr), "Can't open %s\n", i_file_name);
        ecmdOutputError(outstr);
        return ECMD_FATAL_FAILURE;
    }

    // Allocate memory to contain the whole file:
    o_image_size = filestat.st_size;
    o_image_data = (uint8_t*) malloc (sizeof(uint8_t) * o_image_size);

    std::ifstream l_ifs (i_file_name, std::ios::in | std::ios::binary);

    if (!l_ifs.read(reinterpret_cast<char*>(o_image_data), o_image_size))
    {
        snprintf(outstr, sizeof(outstr), "Can't read from %s\n", i_file_name);
        ecmdOutputError(outstr);
        return ECMD_FATAL_FAILURE;
    }

    l_ifs.close();

    return ECMD_SUCCESS;
}

}
#endif
