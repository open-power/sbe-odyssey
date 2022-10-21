/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/lib/mc/exp_port.C $ */
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
/// @file exp_port.C
/// @brief Code to support ports
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: HB:FSP


#include <fapi2.H>
#include <algorithm>
#include <lib/mc/exp_port_traits.H>
#include <lib/mc/exp_port.H>
#include <generic/memory/lib/utils/shared/mss_generic_consts.H>
#include <generic/memory/lib/utils/mc/gen_mss_port.H>

namespace mss
{
const std::vector<uint8_t> portTraits< mss::mc_type::EXPLORER >::NON_SPARE_NIBBLES =
{
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    // Byte 5 contains the spares (if they exist) for mc_type
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
};

const std::vector<uint8_t> portTraits< mss::mc_type::EXPLORER >::SPARE_NIBBLES =
{
    // Byte 5 contains the spares (if they exist) for mc_type
    10,
    11
};

///
/// @brief Helper for setting rcd protection time to minimum of DSM0Q_WRDONE_DLY & DSM0Q_RDTAG_DLY
/// @param [in] i_target the fapi2::Target
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS if ok

fapi2::ReturnCode config_exp_rcd_protect_time (const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    using TT = portTraits<mss::mc_type::EXPLORER>;

    uint64_t l_rdtag_dly = 0;
    uint64_t l_wrdone_dly = 0;
    uint64_t l_rcd_prtct_time = 0;

    fapi2::buffer<uint64_t> l_data;
    fapi2::buffer<uint64_t> l_farb0q_data;

    //Get RDTAG and WRDONE dly values from DSM0Q [36:41] and [24:29] respectively
    FAPI_TRY(fapi2::getScom(i_target, EXPLR_SRQ_MBA_DSM0Q, l_data));
    l_data.extractToRight<TT::DSM0Q_RDTAG_DLY, TT::DSM0Q_RDTAG_DLY_LEN>(l_rdtag_dly);
    l_data.extractToRight<TT::DSM0Q_WRDONE_DLY, TT::DSM0Q_WRDONE_DLY_LEN>(l_wrdone_dly);

    // Get previous value of FARB0Q
    FAPI_TRY(fapi2::getScom(i_target, EXPLR_SRQ_MBA_FARB0Q, l_farb0q_data));

    // Find lower of two delay values
    l_rcd_prtct_time = std::min(l_rdtag_dly, l_wrdone_dly);

    // Configure FARB0Q protect time to reflect
    l_farb0q_data.insertFromRight<TT::FARB0Q_RCD_PROTECTION_TIME, TT::FARB0Q_RCD_PROTECTION_TIME_LEN>(l_rcd_prtct_time);
    FAPI_TRY(fapi2::putScom(i_target, EXPLR_SRQ_MBA_FARB0Q, l_farb0q_data));

fapi_try_exit:
    return fapi2::current_err;
}

}// namespace mss
