/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_scan_compare.C $ */
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
//------------------------------------------------------------------------------
/// @file  poz_scan_compare.C
/// @brief Implementations for generic scan compare code
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include <poz_scan_compare.H>
#include <poz_perv_utils.H>
#include <file_access.H>
#include <poz_fastarray.H>

using namespace fapi2;

ReturnCode poz_compare(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const char* i_compare_hash_fname,
    const char* i_care_mask_dir,
    std::vector<uint32_t>& o_failing_rings)
{
    FAPI_INF("Entering ...");

    const void* l_hash_file_data = NULL;
    const void* l_mask_file_data = NULL;
    size_t l_hash_file_size, l_mask_file_size;
    const hash_data* l_hash_data = NULL;

    char l_fpath[48];
    char* l_fpath_write_ptr = NULL;

    auto l_chip = i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>();
    FAPI_DBG("Loading compare hash file: %s", i_compare_hash_fname);
    ReturnCode l_rc = loadEmbeddedFile(l_chip, i_compare_hash_fname, l_hash_file_data, l_hash_file_size);

    if (l_rc != FAPI2_RC_SUCCESS)
    {
        // Not using FAPI_TRY here because we don't want to call freeEmbeddedFile in fapi_try_exit
        FAPI_INF("Exiting ...");
        return l_rc;
    }

    l_hash_data = (hash_data*)l_hash_file_data;

    for (uint8_t i = 0; i < l_hash_file_size / sizeof(hash_data); i++)
    {
        hwp_hash_ostream l_hash;
        uint32_t l_ring_address = be32toh(l_hash_data[i].ring_address);

        // Construct care mask file path with ring address
        l_fpath_write_ptr = stpcpy(l_fpath, i_care_mask_dir);
        strhex(l_fpath_write_ptr, l_ring_address, 8);
        // Write trailing null ptr since strhex doesn't do that automatically
        l_fpath_write_ptr[8] = 0;

        // Load the care mask file
        FAPI_DBG("Loading care mask file: %s", l_fpath);
        FAPI_TRY(loadEmbeddedFile(l_chip, l_fpath, l_mask_file_data, l_mask_file_size));
        hwp_be_array_istream l_mask((hwp_data_unit*)l_mask_file_data, l_mask_file_size);

        // Perform a sparse getring with an hwp_hash_ostream to hash output data
        FAPI_DBG("Running poz_sparse_getring on ring 0x%08x", l_ring_address);
        l_rc = poz_sparse_getring(i_target, l_ring_address, l_mask, l_hash);
        freeEmbeddedFile(l_mask_file_data);
        FAPI_TRY(l_rc, "poz_sparse_getring failed");

        if (l_hash.getCurrentValue() != l_hash_data[i].hash_value)
        {
            FAPI_INF("COMPARE FAILURE: Scan content mismatch for ring 0x%08x", l_ring_address);
            o_failing_rings.push_back(l_ring_address);
        }
        else
        {
            FAPI_INF("COMPARE SUCCESS: Scan content equality for ring 0x%08x", l_ring_address);
        }
    }

fapi_try_exit:
    freeEmbeddedFile(l_hash_file_data);
    FAPI_INF("Exiting ...");
    return current_err;
}
