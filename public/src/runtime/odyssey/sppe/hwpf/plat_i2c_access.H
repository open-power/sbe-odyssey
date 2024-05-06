/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/hwpf/plat_i2c_access.H $      */
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
#pragma once

#include "i2c.H"
#include "fapi2.H"

namespace fapi2
{

class plati2c : public i2c {

public:

    /**
     * @brief virtual function to populate platform specific details for
     *        i2c communication
     *
     * @param target FAPI HW target to operate on.
     *
     * @return ReturnCode FAPI RC
     */
    ReturnCode populatei2cdetails( const Target<TARGET_TYPE_ALL>& target) override;

    /**
     * @brief To read from i2c device
     *        This API also performs a i2c reset incase of any i2c fails
     *        and retries based on iv_max_retry_count
     *
     * @param target FAPI HW target to operate on.
     * @param get_size Size that getI2c will read from the HW target.
     * @param cfgData Buffer that holds data to write to the HW target.
     *                May be empty if no address/command is required
     *                before the read.
     * @param o_data Buffer that holds data read from HW target.
     * @return ReturnCode FAPI2_RC_SUCCESS if success, else error code.
     */
    ReturnCode getI2c( const Target<TARGET_TYPE_ALL>& target,
                       const size_t get_size,
                       const std::vector<uint8_t>& cfgData,
                       std::vector<uint8_t>& o_data ) override;

    /**
     * @brief To write to i2c device
     *        This API also performs a i2c reset incase of any i2c fails
     *        and retries based on iv_max_retry_count
     *
     * @param target FAPI HW target to operate on.
     * @param data Buffer that holds data to write to the HW target.
     * @return ReturnCode fapi2::ReturnCode. FAPI2_RC_SUCCESS if success, else error code.
     */
    ReturnCode putI2c( const Target<TARGET_TYPE_ALL>& target,
                       const std::vector<uint8_t>& data ) override;
};

} //namespace fapi2
