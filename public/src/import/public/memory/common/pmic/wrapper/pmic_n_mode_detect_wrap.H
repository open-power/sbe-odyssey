/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/wrapper/pmic_n_mode_detect_wrap.H $ */
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
/// @file pmic_n_mode_detect_wrap.H
/// @brief Wrapper for calling pmic_n_mode_detect
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: FSP:HB

#ifndef __PMIC_N_MODE_DETECT_WRAP__
#define __PMIC_N_MODE_DETECT_WRAP__

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <pmic_n_mode_detect.H>

///
/// @brief ostream implementation for usage with this wrapper
///
class wrapper_ostream : public fapi2::hwp_data_ostream
{
    public:
        std::vector<fapi2::hwp_data_unit> iv_data;

        wrapper_ostream() = default;

        ///
        /// @brief Put function implementation for cronus usage
        ///
        /// @param[in] i_data hwp data unit
        ///
        virtual fapi2::ReturnCodes put(fapi2::hwp_data_unit i_data)
        {
            // Data unit is a uint32_t currently
            iv_data.push_back(i_data);
            return fapi2::FAPI2_RC_SUCCESS;
        }

        ///
        /// @brief Get the raw data vector
        ///
        /// @return std::vector<fapi2::hwp_data_unit> Vector of raw data
        ///
        std::vector<fapi2::hwp_data_unit> get_raw()
        {
            return iv_data;
        }

        ///
        /// @brief Return the data formatted as a struct
        ///
        /// @return runtime_n_mode_telem_info info struct
        ///
        runtime_n_mode_telem_info get_struct()
        {
            runtime_n_mode_telem_info l_info;
            std::copy(iv_data.begin(), iv_data.end(), reinterpret_cast<uint32_t*>(&l_info));

            return l_info;
        }
};

#endif
