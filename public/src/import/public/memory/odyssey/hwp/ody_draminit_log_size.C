/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/odyssey/hwp/ody_draminit_log_size.C $ */
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
///
/// @file ody_draminit_log_sizes.C
/// @brief Returns max size of Synopsys training log
///
// *HWP HWP Owner: Louis Stermole <stermole@us.ibm.com>
// *HWP HWP Backup: David J. Chung <dj.chung@ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: FSP:HB

#include <fapi2.H>

#include <ody_draminit_log_size.H>
#include <lib/phy/ody_draminit_utils.H>

#include <generic/memory/mss_git_data_helper.H>
#include <generic/memory/lib/utils/c_str.H>

extern "C"
{
///
/// @brief Prints out max size of Synopsys training log
/// @param[in] i_port Memory port target
/// @return fapi2::FAPI2_RC_SUCCESS
///
    fapi2::ReturnCode ody_draminit_log_size(const fapi2::Target<fapi2::TARGET_TYPE_MEM_PORT>& i_port)
    {
        uint8_t l_port_verbosity = fapi2::ENUM_ATTR_ODY_DRAMINIT_VERBOSITY_COARSE_DEBUG;

        mss::display_git_commit_info("ody_draminit_log_size");

        FAPI_INF( GENTARGTIDFORMAT " Start ody_draminit_log_size", GENTARGTID(i_port));

        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_ODY_DRAMINIT_VERBOSITY, i_port, l_port_verbosity));

        FAPI_INF(GENTARGTIDFORMAT "is set to verbosity %d of size %d", GENTARGTID(i_port),
                 l_port_verbosity,
                 mss::ody::phy::getBufferSizeFromVerboseAttr(l_port_verbosity));
        return fapi2::FAPI2_RC_SUCCESS;

    fapi_try_exit:
        return fapi2::current_err;
    }
}
