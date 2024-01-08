/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/lib/ecc/ody_mbs_error_vector_trap.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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
/// @file mbs_error_vector_trap.C
/// @brief  Main functions for ecc decode
///
// *HWP HWP Owner: Geetha Pisapati <geetha.pisapati@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: Memory

#include <lib/mcbist/ody_mcbist_traits.H>
#include <lib/ecc/ecc_traits_odyssey.H>
#include <ody_scom_ody_odc.H>
#include <generic/memory/lib/ecc/ecc.H>
#include <generic/memory/lib/ecc/mbs_error_vector_trap.H>
#include <lib/dimm/ody_rank.H>

namespace mss
{
namespace ecc
{
namespace mbs_error_vector_trap
{

///
/// @brief get_nce_galois
/// @param[in] i_target the ocmb target
/// @param[in] i_data the register value
/// @param[out] o_value the value of the field
///
template<>
void get_nce_galois<mss::mc_type::ODYSSEY>( const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target,
        const fapi2::buffer<uint64_t>& i_data,
        uint64_t& o_value)
{
    using TT = eccTraits<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_MEM_PORT>;
    const auto& l_rel_port_pos = mss::relative_pos<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_OCMB_CHIP>(i_target);

    l_rel_port_pos ==  1 ? i_data.extractToRight<TT::P1_NCE_GALOIS, TT::P1_NCE_GALOIS_LEN>(o_value) :
    i_data.extractToRight<TT::P0_NCE_GALOIS, TT::P0_NCE_GALOIS_LEN>(o_value);
}

//
/// @brief get_nce_magnitude
/// @param[in] i_target the ocmb target
/// @param[in] i_data the register value
/// @param[out] o_value the value of the field
///
template<>
void get_nce_magnitude<mss::mc_type::ODYSSEY>( const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target,
        const fapi2::buffer<uint64_t>& i_data,
        uint64_t& o_value)
{
    using TT = eccTraits<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_MEM_PORT>;
    const auto& l_rel_port_pos = mss::relative_pos<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_OCMB_CHIP>(i_target);

    l_rel_port_pos ==  1 ? i_data.extractToRight<TT::P1_NCE_MAGNITUDE, TT::P1_NCE_MAGNITUDE_LEN>(o_value) :
    i_data.extractToRight<TT::P0_NCE_MAGNITUDE, TT::P0_NCE_MAGNITUDE_LEN>(o_value);
}

///
/// @brief get_tce_galois
/// @param[in] i_target the ocmb target
/// @param[in] i_data the register value
/// @param[out] o_value the value of the field
///
template<>
void get_tce_galois<mss::mc_type::ODYSSEY>( const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target,
        const fapi2::buffer<uint64_t>& i_data,
        uint64_t& o_value)
{
    using TT = eccTraits<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_MEM_PORT>;
    const auto& l_rel_port_pos = mss::relative_pos<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_OCMB_CHIP>(i_target);

    l_rel_port_pos ==  1 ? i_data.extractToRight<TT::P1_TCE_GALOIS, TT::P1_TCE_GALOIS_LEN>(o_value) :
    i_data.extractToRight<TT::P0_TCE_GALOIS, TT::P0_TCE_GALOIS_LEN>(o_value);
}


///
/// @brief get_tce_magnitude
/// @param[in] i_target the ocmb target
/// @param[in] i_data the register value
/// @param[out] o_value the value of the field
///
template<>
void get_tce_magnitude<mss::mc_type::ODYSSEY>( const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_target,
        const fapi2::buffer<uint64_t>& i_data,
        uint64_t& o_value)
{

    using TT = eccTraits<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_MEM_PORT>;
    const auto& l_rel_port_pos = mss::relative_pos<mss::mc_type::ODYSSEY, fapi2::TARGET_TYPE_OCMB_CHIP>(i_target);

    l_rel_port_pos ==  1 ? i_data.extractToRight<TT::P1_TCE_MAGNITUDE, TT::P1_TCE_MAGNITUDE_LEN>(o_value) :
    i_data.extractToRight<TT::P0_TCE_MAGNITUDE, TT::P0_TCE_MAGNITUDE_LEN>(o_value);
}

}// ns mbs_error_vector_trap
}// ns ecc
}// ns mss
