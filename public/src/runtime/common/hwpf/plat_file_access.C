/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/plat_file_access.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
#include "fapi2_file_access.H"
#include "sbeglobals.H"
#include "heap.H"
#include "archive_config.H"

namespace fapi2
{

ReturnCode plat_loadEmbeddedFile(const char* i_path,
                                 const void*&o_data, size_t &o_size, uint32_t i_flags)
{
#define SBE_FUNC " plat_loadEmbeddedFile "
    SBE_ENTER(SBE_FUNC);

    RuntimePakWrapper * const archive = (i_flags & fapi2::LEFF_ALLOW_UNTRUSTED) ?
        &SBE_GLOBAL->embeddedArchive : &SBE_GLOBAL->trustedEmbeddedArchive;
    const uint32_t l_flags = (i_flags & fapi2::LEFF_QUIET_IF_NOT_FOUND) ?
        ARCHIVE_FLAGS_NOT_FOUND_QUIET : 0;

    uint32_t l_size;
    const ARC_RET_t pakRc = archive->load_file(i_path, o_data, l_size, NULL, l_flags);
    o_size = l_size;
    const ReturnCode l_rc =
        (pakRc == ARC_FILE_NOT_FOUND) ? FAPI2_RC_FILE_NOT_FOUND :
        ((pakRc == ARC_OPERATION_SUCCESSFUL) ? FAPI2_RC_SUCCESS :
         FAPI2_RC_PLAT_ERR_SEE_DATA);

    //TODO: JIRA  PFSBE-251: Need to look into error xml data collection

    SBE_EXIT(SBE_FUNC);
    return l_rc;
#undef SBE_FUNC
}

ReturnCode plat_freeEmbeddedFile(const void* i_data)
{
    SBE_GLOBAL->embeddedArchive.free_file(i_data);
    return FAPI2_RC_SUCCESS;
}

}
