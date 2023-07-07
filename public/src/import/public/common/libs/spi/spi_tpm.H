/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_tpm.H $          */
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

#include <fapi2.H>
#include "spi_base.H"

namespace spi
{

class TPM
{
    public:
        TPM(const AbstractPort& i_port, bool i_secure_mode = false) :
            iv_port(i_port), iv_secure_mode(i_secure_mode)
        {}

        void set_locality(int i_locality)
        {
            iv_requested_locality = i_locality;
        }

        fapi2::ReturnCode relinquish_locality();

        fapi2::ReturnCode command(
            const void* i_req, uint32_t i_req_len,
            void* o_rsp, uint32_t& o_rsp_len, uint32_t i_max_rsp_len,
            uint32_t i_timeout_ms);

    private:
        const AbstractPort& iv_port;
        const bool iv_secure_mode;
        int iv_requested_locality = 0;
        int iv_locality = -1;

        class TPM_STS_t;

        fapi2::ReturnCode write(uint32_t i_address, const void* i_req, uint32_t i_req_len) const;
        fapi2::ReturnCode read(uint32_t i_address, void* o_rsp, uint32_t i_rsp_len) const;

        fapi2::ReturnCode write_status(const TPM_STS_t i_status) const;
        fapi2::ReturnCode read_status(TPM_STS_t& o_status) const;

        fapi2::ReturnCode request_locality() const;
        fapi2::ReturnCode ensure_locality();

        fapi2::ReturnCode wait_for_fifo(
            TPM_STS_t& o_status, bool i_read_not_write,
            uint32_t i_total_length, uint32_t i_remaining_length) const;
        fapi2::ReturnCode write_fifo(const void* i_data, const uint32_t i_length) const;
        fapi2::ReturnCode read_fifo(void* o_data, uint32_t& o_length, const uint32_t i_max_length) const;
};

}
