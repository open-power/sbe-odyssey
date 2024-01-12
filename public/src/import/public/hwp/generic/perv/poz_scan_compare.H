/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_scan_compare.H $ */
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
/// @file  poz_scan_compare.H
/// @brief Header definitions for generic scan compare code
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Andrew Singer (andrew.singer@ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <fapi2.H>
#include <hwp_data_stream.H>
#include "crc32.H"
#include "poz_tlv_utils.H"
#include <endian.h>

using namespace fapi2;

/**
 * @brief Output stream that writes data to a hash
 *
 * The final hash value can be acquired by calling digestHash.
 */
class hwp_hash_ostream : public hwp_data_ostream
{
    public:
        /**
         * @brief Initialize the stream by initializing our hash value
         */
        hwp_hash_ostream()
        {
            hash_value = 0;
        };

        virtual ReturnCodes put(hwp_data_unit i_data)
        {
            uint32_t l_be32_data = htobe32(i_data);
            hash_value = crc32_update(hash_value, &l_be32_data, sizeof(l_be32_data));
            return FAPI2_RC_SUCCESS;
        }

        uint32_t getCurrentValue()
        {
            return hash_value;
        }

    private:
        uint32_t hash_value;
};

struct hash_data
{
    uint32_t ring_address;
    uint16_t instance_traits;
    uint8_t care_mask_id_override;
    uint8_t padding;
    uint32_t hash_value;
};

/// @brief Get ring addresses for a hash_data and target if they match
///
/// Given a target and a hash_data, plug the base and exact ring addresses into their respective
/// output parameters, and return a boolean indicating if the hash_data applies to the target.
///
/// @param[in]   i_target                  Target object on which to compare ring content
/// @param[in]   i_hash_data               The hash_data instance to decipher
/// @param[out]  o_base_ring_address       The base ring address that i_hash_data applies to
/// @param[out]  o_ring_address            The ring address that i_hash_data applies to on i_target
///
/// @return  true if i_hash_data applies to i_target, else false
bool get_hash_target_addresses(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    const hash_data& i_hash_data,
    uint32_t& o_base_ring_address,
    uint32_t& o_ring_address);

/// @brief Compare ring content against expect hashes in the provided file
///
/// Given a chiplet target, a base ring address, and an expect hash, sparse scan out the
/// corresponding rings, hash the data with care mask files in the provided directory, and
/// append any flag compare status.
///
/// @param[in]   i_target                  Target object on which to compare ring content
/// @param[in]   i_base_ring_address       The base ring address we want to compare on i_target
/// @param[in]   i_care_mask_dir           Path to directory of care masks hash with
/// @param[in]   i_expect_hash             Expect hash value for the provided compare case
/// @param[in]   i_care_mask_id_override   Override chiplet ID to use for care mask files (0 for skip)
///
/// @return  FAPI2_RC_SUCCESS if success, else error code.
fapi2::ReturnCode poz_scan_compare(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_CORE > & i_target,
    const uint32_t i_base_ring_address,
    const char* i_care_mask_dir,
    const uint32_t i_expect_hash,
    const uint8_t i_care_mask_id_override = 0);
