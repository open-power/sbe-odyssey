/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_scan_compare.C $ */
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
//------------------------------------------------------------------------------
/// @file  poz_scan_compare.C
/// @brief Implementations for generic scan compare code
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#include <poz_scan_via_scom.H>
#include <poz_scan_compare.H>
#include <poz_perv_utils.H>
#include <file_access.H>

using namespace fapi2;

bool get_hash_target_addresses(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const hash_data& i_hash_data,
    uint32_t& o_base_ring_address,
    uint32_t& o_ring_address)
{
    o_base_ring_address = be32toh(i_hash_data.ring_address);

    const uint16_t l_chiplets_in_group = (be16toh(i_hash_data.instance_traits) & 0x1F) + 1;
    const uint8_t l_base_chiplet_id = o_base_ring_address >> 24;
    const uint8_t l_chiplet_id = i_target.getChipletNumber();

    // Skip if this ring address doesn't apply to the input target
    if ((l_chiplet_id < l_base_chiplet_id) || (l_chiplet_id >= l_base_chiplet_id + l_chiplets_in_group))
    {
        return false;
    }

    o_ring_address = o_base_ring_address + ((l_chiplet_id - l_base_chiplet_id) << 24);

    return true;
}

ReturnCode poz_scan_compare(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const uint32_t i_base_ring_address,
    const char* i_care_mask_dir,
    const uint32_t i_expect_hash,
    const uint8_t i_care_mask_id_override)
{
    FAPI_DBG("Entering poz_scan_compare...");

    const void* l_mask_file_data = NULL;
    const void* l_care_mask_ptr = NULL;
    size_t l_mask_file_size;

    // Format is <a/l>bist/<m/s>/cm/<1 char cm set key>/<8 char hex ring address>
    char l_fpath[22] = {};
    char* l_fpath_write_ptr = NULL;

    uint32_t l_ring_address = (i_target.getChipletNumber() << 24) |
                              (i_base_ring_address & 0x00FFFFFF);
    hwp_hash_ostream l_hash;

    // Construct care mask file path with ring address
    l_fpath_write_ptr = stpcpy(l_fpath, i_care_mask_dir);
    strhex(l_fpath_write_ptr,
           i_care_mask_id_override ?
           (i_care_mask_id_override << 24) | (i_base_ring_address & 0x00FFFFFF) :
           i_base_ring_address,
           8);
    // Write trailing null ptr since strhex doesn't do that automatically
    l_fpath_write_ptr[8] = 0;

    // Load the care mask file
    FAPI_DBG("Loading care mask file: %s", l_fpath);
    FAPI_TRY(loadEmbeddedFile(i_target.getParent<TARGET_TYPE_ANY_POZ_CHIP>(),
                              l_fpath,
                              l_mask_file_data,
                              l_mask_file_size));

    // Need new scope here to fix initialization crossing of istream below
    {
        // The care mask starts after the first 32 bits of the care mask file
        l_care_mask_ptr = (void*)(static_cast<const uint32_t*>(l_mask_file_data) + 1);
        hwp_be_array_istream l_mask((hwp_data_unit*)l_care_mask_ptr, l_mask_file_size - 4);

        // Perform a sparse getring with an hwp_hash_ostream to hash output data
        FAPI_DBG("Running poz_sparse_getring on ring 0x%08x", l_ring_address);
        ReturnCode l_rc = poz_sparse_getring(i_target, l_ring_address, l_mask, l_hash);
        FAPI_TRY(freeEmbeddedFile(l_mask_file_data));
        FAPI_TRY(l_rc, "poz_sparse_getring failed");

        if (l_hash.getCurrentValue() != i_expect_hash)
        {
            FAPI_INF("COMPARE FAILURE: Scan content mismatch for ring 0x%08x", l_ring_address);
            FAPI_DBG("Expected hash = 0x%08x; actual hash = 0x%08x",
                     i_expect_hash,
                     l_hash.getCurrentValue());
            current_err = FAPI2_RC_FALSE;
        }
        else
        {
            FAPI_INF("COMPARE SUCCESS: Scan content equality for ring 0x%08x", l_ring_address);
        }
    }

fapi_try_exit:
    FAPI_DBG("Exiting poz_scan_compare...");
    return current_err;
}
