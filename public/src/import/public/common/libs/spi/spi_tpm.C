/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_tpm.C $          */
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
#include <algorithm>
#include <endian.h>
#include "spi_tpm.H"

using std::min;
using namespace fapi2;

enum spi_tpm_constants
{
    TPM_ACCESS    = 0x0000,
    TPM_STS       = 0x0018,
    TPM_DATA_FIFO = 0x0024,

    FIFO_TIMEOUT_COUNT  = 100,
    FIFO_TIMEOUT_NS     = 100000,
    FIFO_TIMEOUT_CYCLES = 10000,

    ACCESS_TIMEOUT_COUNT  = 100,
    ACCESS_TIMEOUT_NS     = 100000,
    ACCESS_TIMEOUT_CYCLES = 10000,

    READY_TIMEOUT_COUNT  = 100,
    READY_TIMEOUT_NS     = 100000,
    READY_TIMEOUT_CYCLES = 10000,

    COMMAND_TIMEOUT_COUNT_1MS = 10,
    COMMAND_TIMEOUT_NS        = 100000,
    COMMAND_TIMEOUT_CYCLES    = 10000,
};

class spi::TPM::TPM_STS_t : public buffer<uint32_t>
{
    public:
        uint32_t get_tpm_family() const
        {
            return getBits<4, 2>();
        }
        uint32_t get_burst_count() const
        {
            return getBits<8, 16>();
        }
        bool get_valid() const
        {
            return getBit<24>();
        }
        bool get_command_ready() const
        {
            return getBit<25>();
        }
        TPM_STS_t& set_command_ready()
        {
            setBit<25>();
            return *this;
        }
        TPM_STS_t& set_tpm_go()
        {
            setBit<26>();
            return *this;
        }
        bool get_data_avail() const
        {
            return getBit<27>();
        }
        bool get_expect() const
        {
            return getBit<28>();
        }
        bool get_self_test_done() const
        {
            return getBit<29>();
        }
        TPM_STS_t& set_response_retry()
        {
            setBit<30>();
            return *this;
        }
};

class TPM_ACCESS_t : public buffer<uint8_t>
{
    public:
        bool get_valid() const
        {
            return getBit<0>();
        }
        bool get_active() const
        {
            return getBit<2>();
        }
        TPM_ACCESS_t& set_relinquish()
        {
            setBit<2>();
            return *this;
        }
        bool get_seized() const
        {
            return getBit<3>();
        }
        TPM_ACCESS_t& set_clear_seized()
        {
            setBit<3>();
            return *this;
        }
        TPM_ACCESS_t& set_seize()
        {
            setBit<4>();
            return *this;
        }
        bool get_pending() const
        {
            return getBit<5>();
        }
        bool get_request() const
        {
            return getBit<6>();
        }
        TPM_ACCESS_t& set_request()
        {
            setBit<6>();
            return *this;
        }
        bool get_establishment() const
        {
            return getBit<7>();
        }
};

ReturnCode spi::TPM::write(uint32_t i_address, const void* i_req, uint32_t i_req_len) const
{
    FAPI_ASSERT(i_req_len != 0, SPI_TPM_ZERO_LENGTH_RW().set_READ_NOT_WRITE(false),
                "Zero length write detected, this is a bug");
    return iv_port.transaction(0x00D40000 | ((i_req_len - 1) << 24) | (iv_locality << 12) | i_address,
                               4, i_req, i_req_len, NULL, 0);
fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::read(uint32_t i_address, void* o_rsp, uint32_t i_rsp_len) const
{
    FAPI_ASSERT(i_rsp_len != 0, SPI_TPM_ZERO_LENGTH_RW().set_READ_NOT_WRITE(true),
                "Zero length read detected, this is a bug");
    return iv_port.transaction(0x80D40000 | ((i_rsp_len - 1) << 24) | (iv_locality << 12) | i_address,
                               4, NULL, 0, o_rsp, i_rsp_len);
fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::write_status(const TPM_STS_t i_status) const
{
    uint32_t tmp = htole32(i_status);
    return write(TPM_STS, &tmp, 4);
}

ReturnCode spi::TPM::read_status(TPM_STS_t& o_status) const
{
    uint32_t tmp;
    FAPI_TRY(read(TPM_STS, &tmp, 4));

    FAPI_ASSERT(tmp != 0xFFFFFFFF,
                SPI_TPM_READ_ABORT().set_LOCALITY(iv_locality).set_ADDRESS(TPM_STS),
                "A TPM status read resulted in the TPM aborting the command");

    o_status() = le32toh(tmp);
    FAPI_DBG("Read locality %d status: 0x%08x", iv_locality, o_status);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::request_locality() const
{
    TPM_ACCESS_t access;
    uint32_t timeout = ACCESS_TIMEOUT_COUNT;

    while (true)
    {
        FAPI_TRY(read(TPM_ACCESS, &access, 1));
        FAPI_DBG("Read locality %d access: 0x%02x", iv_locality, access);

        FAPI_ASSERT(access != 0xFF,
                    SPI_TPM_READ_ABORT().set_LOCALITY(iv_locality).set_ADDRESS(TPM_ACCESS),
                    "A TPM access register read resulted in the TPM aborting the command");

        if (!access.get_valid())
        {
            FAPI_DBG("Invalid access register value 0x%02x - retrying", access);
        }
        else if (access.get_active())
        {
            FAPI_DBG("Locality %d active", iv_locality);
            break;
        }
        else if (!access.get_request())
        {
            FAPI_DBG("Requesting locality %d", iv_locality);
            access.flush<0>();
            access.set_request();
            FAPI_TRY(write(TPM_ACCESS, &access, 1));
        }

        FAPI_ASSERT(timeout, SPI_TPM_ACCESS_TIMEOUT()
                    .set_LOCALITY(iv_locality),
                    "Timed out waiting for locality %d to be granted",
                    iv_locality);

        // wait and retry
        delay(ACCESS_TIMEOUT_NS, ACCESS_TIMEOUT_CYCLES);
        timeout--;
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::relinquish_locality()
{
    TPM_ACCESS_t access;
    access.flush<0>();
    access.set_relinquish();

    if (iv_locality < 0)
    {
        return FAPI2_RC_SUCCESS;
    }

    FAPI_TRY(write(TPM_ACCESS, &access, 1));

    iv_locality = -1;

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::ensure_locality()
{
    ReturnCode rc;

    if (iv_requested_locality != iv_locality)
    {
        // First relinquish the current locality if one is active
        FAPI_TRY(relinquish_locality());

        // Then attempt to request the new locality;
        // we have to speculatively set iv_locality since it flows into every access
        iv_locality = iv_requested_locality;
        rc = request_locality();

        if (rc != FAPI2_RC_SUCCESS)
        {
            // and if the request fails we unset the locality again
            iv_locality = -1;
        }

        FAPI_TRY(rc);
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::wait_for_fifo(TPM_STS_t& o_status, bool i_read_not_write, uint32_t i_total_length,
                                   uint32_t i_remaining_length) const
{
    uint32_t timeout = FIFO_TIMEOUT_COUNT;

    while (timeout)
    {
        FAPI_TRY(read_status(o_status));

        if (o_status.get_valid() && (                               // If the status is not valid, poll again
                (o_status.get_burst_count() > 0) ||                 // Exit if the FIFO is available...
                (i_read_not_write && !o_status.get_data_avail()) || // ...or if we're reading and no more data is available...
                (!i_read_not_write &&                               // ...or if we're writing and the TPM doesn't expect any more data
                 !(o_status.get_command_ready() || o_status.get_expect()))
            )
           )
        {
            return FAPI2_RC_SUCCESS;
        }

        delay(FIFO_TIMEOUT_NS, FIFO_TIMEOUT_CYCLES);
        timeout--;
    }

    FAPI_ASSERT(false, SPI_TPM_FIFO_TIMEOUT()
                .set_TOTAL_LENGTH(i_total_length)
                .set_REMAINING_LENGTH(i_remaining_length)
                .set_STATUS(o_status),
                "Timed out waiting for the TPM FIFO total_length=%d remaining_length=%d",
                i_total_length, i_remaining_length);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::write_fifo(const void* i_data, const uint32_t i_length) const
{
    TPM_STS_t status;
    uint32_t l_remain = i_length;
    auto data8 = static_cast<const uint8_t*>(i_data);

    while (l_remain)
    {
        FAPI_TRY(wait_for_fifo(status, false, i_length, l_remain));

        FAPI_ASSERT(status.get_command_ready() || status.get_expect(),
                    SPI_TPM_REQUEST_TOO_LONG()
                    .set_REQUEST_LENGTH(i_length)
                    .set_REMAINING_LENGTH(l_remain),
                    "TPM no longer expecting data before complete request transmitted - "
                    "request_length=%d remaining_length=%d",
                    i_length, l_remain);

        {
            const uint32_t burst_count = min(status.get_burst_count(), 4U);  // TODO can't we use the full burst count?
            const uint32_t chunk = min(l_remain, burst_count);
            FAPI_TRY(write(TPM_DATA_FIFO, data8, chunk));
            data8 += chunk;
            l_remain -= chunk;
        }
    }

    FAPI_TRY(read_status(status));
    FAPI_ASSERT(!status.get_expect(),
                SPI_TPM_REQUEST_TOO_SHORT()
                .set_REQUEST_LENGTH(i_length),
                "TPM still expecting data after request - request_length=%d",
                i_length);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::read_fifo(void* o_data, uint32_t& o_length, uint32_t i_max_length) const
{
    auto data8 = static_cast<uint8_t*>(o_data);
    uint32_t l_received = 0;
    uint32_t l_expect = i_max_length;
    TPM_STS_t status;
    bool l_got_length = false;

    while (true)
    {
        // Once we have enough data to decode the length field, update our expected size
        if (!l_got_length && l_received >= 6)
        {
            l_expect = (data8[2] << 24) | (data8[3] << 16) | (data8[4] << 8) | data8[5];
            FAPI_ASSERT(l_expect <= i_max_length,
                        SPI_TPM_RESPONSE_TOO_LONG()
                        .set_RESPONSE_LENGTH(l_expect)
                        .set_MAX_LENGTH(i_max_length),
                        "TPM response does not fit buffer response_length=%d max_length=%d",
                        l_expect, i_max_length);
            l_got_length = true;
        }

        FAPI_TRY(wait_for_fifo(status, true, l_expect, l_expect - l_received));

        if (!status.get_data_avail())
        {
            // We're done, return the final receive length
            o_length = l_received;

            FAPI_ASSERT(l_got_length,
                        SPI_TPM_RESPONSE_TOO_LONG()
                        .set_RESPONSE_LENGTH(l_received)
                        .set_MAX_LENGTH(i_max_length),
                        "TPM response too short, no length field seen - received %d bytes",
                        l_received);

            FAPI_ASSERT(l_received == l_expect,
                        SPI_TPM_RESPONSE_SIZE_MISMATCH()
                        .set_RECEIVED_LENGTH(l_received)
                        .set_HEADER_LENGTH(l_expect),
                        "TPM response length does not match length from header field - "
                        "received_size=%d expected_size=%d", l_received, l_expect);

            return FAPI2_RC_SUCCESS;
        }

        {
            const uint32_t burst_count = min(status.get_burst_count(), 4U);
            const uint32_t chunk = min(l_expect - l_received, burst_count);
            FAPI_TRY(read(TPM_DATA_FIFO, data8 + l_received, chunk));
            l_received += chunk;
        }
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::TPM::command(
    const void* i_req, uint32_t i_req_len,
    void* o_rsp, uint32_t& o_rsp_len, uint32_t i_max_rsp_len,
    uint32_t i_timeout_ms)
{
    TPM_STS_t status;
    uint32_t timeout;

    FAPI_ASSERT(i_max_rsp_len > 6,
                SPI_TPM_RESPONSE_BUF_TOO_SHORT()
                .set_MAX_LENGTH(i_max_rsp_len),
                "Invalid max response length %d, must be above 6",
                i_max_rsp_len);

    FAPI_TRY(ensure_locality());

    // Wait for TPM to be ready to receive commands
    timeout = READY_TIMEOUT_COUNT;

    while (true)
    {
        FAPI_TRY(read_status(status));

        if (status.get_valid() && status.get_command_ready())
        {
            break;
        }

        FAPI_ASSERT(timeout > 0,
                    SPI_TPM_READY_TIMEOUT(),
                    "Timed out waiting for TPM to become ready");
        timeout--;

        status.flush<0>();
        status.set_command_ready();
        FAPI_TRY(write_status(status));

        delay(READY_TIMEOUT_NS, READY_TIMEOUT_CYCLES);
    }

    // Send command to FIFO
    FAPI_TRY(write_fifo(i_req, i_req_len));

    // Trigger TPM_GO
    status.flush<0>();
    status.set_tpm_go();
    FAPI_TRY(write_status(status));

    // Wait for TPM to complete the command
    timeout = COMMAND_TIMEOUT_COUNT_1MS * i_timeout_ms;

    while (true)
    {
        FAPI_TRY(read_status(status));

        if (status.get_valid() && status.get_data_avail())
        {
            break;
        }

        FAPI_ASSERT(timeout > 0,
                    SPI_TPM_COMMAND_TIMEOUT()
                    .set_TIMEOUT_MS(i_timeout_ms),
                    "Timed out waiting for TPM command to complete");
        timeout--;

        delay(COMMAND_TIMEOUT_NS, COMMAND_TIMEOUT_CYCLES);
    }

    // Receive response
    FAPI_TRY(read_fifo(o_rsp, o_rsp_len, i_max_rsp_len));

    // Acknowledge receipt
    status.flush<0>();
    status.set_command_ready();
    FAPI_TRY(write_status(status));

fapi_try_exit:
    return current_err;
}
