/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_base.C $         */
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
#include "spi_base.H"
#include <endian.h>

using namespace fapi2;

enum spi_base_constants
{
    POLL_COUNT              = 100,
    POLL_DELAY_NS           = 500,
    POLL_DELAY_CYCLES       = 1000,

    SPIC_COUNTER_REG        = 1,
    SPIC_CONFIG_REG         = 2,
    SPIC_CLOCK_CONFIG_REG   = 3,
    RESET_CONTROL           = 24,
    RESET_CONTROL_LEN       = 4,
    RESET_PART1             = 0x5,
    RESET_PART2             = 0xA,
    ECC_CONTROL             = 29,
    ECC_CONTROL_LEN         = 2,
    SPIC_MEMORY_MAPPING_REG = 4,
    SPIC_TRANSMIT_DATA_REG  = 5,
    SPIC_RECEIVE_DATA_REG   = 6,
    SPIC_SEQUENCER_OP_REG   = 7,
    SPIC_STATUS_REG         = 8,
};

#define _SPIOP(op, arg) (uint64_t)((op) | ((arg) & 0xF))
#define SPIOP_SELECT_RESP(resp) _SPIOP(0x10, resp)
#define SPIOP_SHIFT_N1(nbytes)  _SPIOP(0x30, nbytes)
#define SPIOP_SHIFT_N2(nbytes)  _SPIOP(0x40, nbytes)
#define SPIOP_B_RDR_NE(dest)    _SPIOP(0x60, dest)
#define SPIOP_TRANSFERTDR()     _SPIOP(0xC0, 0)
#define SPIOP_BNEI1(dest)       _SPIOP(0xE0, dest)
#define SPIOP_BNEI2(dest)       _SPIOP(0xF0, dest)

#define _SPI_SEQUENCE(c0, c1, c2, c3, c4, c5, c6, c7, ...) (((c0) << 56) | ((c1) << 48) | ((c2) << 40) | ((c3) << 32) | ((c4) << 24) | ((c5) << 16) | ((c6) << 8) | ((c7) << 0))
#define SPI_SEQUENCE(...) _SPI_SEQUENCE(_VA_ARGS_, 0, 0, 0, 0, 0, 0, 0)

ReturnCode spi::SPIPort::transaction(const uint64_t cmd, const uint32_t cmd_len,
                                     const void* req, const uint32_t req_len,
                                     void* rsp, const uint32_t rsp_len,
                                     const bool i_use_ecc) const
{
    /* Combine command and request payload since otherwise we
     * might end up with more than 8 ops in the sequence */
    const uint32_t total_req_len = cmd_len + req_len;
    const uint64_t* req64 = (uint64_t*)req;
    const ecc_mode l_ecc_mode = i_use_ecc ? iv_ecc_mode : ECC_DISABLED;
    uint64_t* rsp64 = (uint64_t*)rsp;

    FAPI_DBG("SPI transaction: cmd=0x%08x%08x ecc|cmd_len=0x%04x req_len|rsp_len=0x%08x",
             cmd >> 32, cmd & 0xFFFFFFFF, (l_ecc_mode << 8) | cmd_len,
             (req_len << 16) | rsp_len);

    /* -----------------------------------------------------------------------------------------
     * Construct sequence and set up counters
     * ----------------------------------------------------------------------------------------- */

    /* Base setting: N1 used implicitly and for transmit,
     * N2 used implicitly, with reload and for transmit+receive
     * for receive pacing.
     *
     * The transmit+receive+reload setting for N2 is required so that
     * the controller logic correctly performs receive pacing. With any
     * other setting the controller would not wait for us to pick up
     * data from RDR and run into RDR overflow.
     *
     * It is not intuitive at all but it's what works ¯\_(ツ)_/¯
     */
    uint64_t CNT = 0x6F00;

    /* Start with a responder select op */
    uint64_t SEQ = SPIOP_SELECT_RESP(1 << iv_resp_select);
    uint32_t n_ops = 1;

    /* Transmit entire words if necessary */
    if (total_req_len >= 8)
    {
        CNT |= (uint64_t)((total_req_len / 8) - 1) << 32;
        SEQ = (SEQ << 16) |
              SPIOP_SHIFT_N1(8) << 8 |
              SPIOP_BNEI1(n_ops);
        n_ops += 2;
    }

    /* Transmit remainder if there is one */
    if (total_req_len & 7)
    {
        SEQ = (SEQ << 8) |
              SPIOP_SHIFT_N1(total_req_len & 7);
        n_ops++;
    }

    /* Receive response, entire words first */
    if (rsp_len >= 8)
    {
        CNT |= (uint64_t)((rsp_len / 8) - 1) << 24;
        SEQ = (SEQ << 16) |
              /* If we're looking at or discarding ECC, receive 9 bytes */
              SPIOP_SHIFT_N2((l_ecc_mode != ECC_DISABLED) ? 9 : 8) << 8 |
              SPIOP_BNEI2(n_ops);
        n_ops += 2;
    }

    /* Receive response remainder */
    if (rsp_len & 7)
    {
        SEQ = (SEQ << 8) |
              SPIOP_SHIFT_N2(rsp_len & 7);
        n_ops++;
    }

    /* Finally deselect */
    SEQ = (SEQ << 8) | SPIOP_SELECT_RESP(0);
    n_ops++;

    /* Shift sequence into final position */
    SEQ <<= 8 * (8 - n_ops);

    /* -----------------------------------------------------------------------------------------
     * Transmit command and request payload
     * ----------------------------------------------------------------------------------------- */

    /* Prepare TDR for command and first bits of data */
    uint64_t TDR = cmd << (8 * (8 - cmd_len));
    uint64_t read_buf = 0;
    uint32_t read_buf_shift = cmd_len * 8;
    uint32_t read_buf_remain = 64 - read_buf_shift;

    if (req_len && read_buf_remain)
    {
        read_buf = be64toh(*req64);
        req64++;
        TDR |= (read_buf >> read_buf_shift);
    }

    /* Doing this check very late since we're declaring a lot of
     * variables before this point and don't want the implicit goto to
     * cause compiler fails. */
    FAPI_ASSERT(cmd_len <= 8 && req_len < 1024 && rsp_len < 1024,
                SPI_UNSUPPORTED_LENGTH()
                .set_CMD_LEN(cmd_len)
                .set_REQ_LEN(req_len)
                .set_RSP_LEN(rsp_len),
                "Unsupported length: cmd_len=%d (max 8) req_len=%d (max 1024) rsp_len=%d (max 1024)",
                cmd_len, req_len, rsp_len);

    /* Check SPI controller health before we begin */
    FAPI_TRY(wait_for_tdr_empty());

    /* Set up ECC mode */
    {
        buffer<uint64_t> CLK;
        FAPI_TRY(getscom(SPIC_CLOCK_CONFIG_REG, CLK));

        if (CLK.getBits<ECC_CONTROL, ECC_CONTROL_LEN>() != l_ecc_mode)
        {
            CLK.insertFromRight<ECC_CONTROL, ECC_CONTROL_LEN>(l_ecc_mode);
            FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, CLK));
        }
    }

    /* Send first chunk of data */
    FAPI_TRY(putscom(SPIC_SEQUENCER_OP_REG,  SEQ));
    FAPI_TRY(putscom(SPIC_COUNTER_REG,       CNT));
    FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));

    /* Send remaining chunks of data */
    for (uint32_t i = 0; i < ((total_req_len + 7) / 8) - 1; i++)
    {
        FAPI_TRY(wait_for_tdr_empty());
        TDR = read_buf << read_buf_remain;
        read_buf = be64toh(*req64);
        req64++;
        TDR |= read_buf >> read_buf_shift;
        FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));
    }

    /* -----------------------------------------------------------------------------------------
     * Receive response data
     * ----------------------------------------------------------------------------------------- */

    if (rsp_len)
    {
        // We set the SPI controller up to shift out and in at the same time
        // so we can use the transmit side pacing for pacing the receive data.
        // This means that the SPI controller will stop at the transition from
        // write to read, waiting for dummy write data. We write TDR to zero
        // tp provide that dummy data at which point the receive sequence will
        // start and self-pace; all we have to do is pick up data from RDR.
        FAPI_TRY(wait_for_tdr_empty());
        FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, 0));
    }

    /* Receive the bulk of receive data word by word */
    for (uint32_t i = 0; i < rsp_len / 8; i++)
    {
        uint64_t RDR;
        FAPI_TRY(wait_for_rdr_full());
        FAPI_TRY(getscom(SPIC_RECEIVE_DATA_REG, RDR));
        *rsp64 = htobe64(RDR);
        rsp64++;
    }

    /* If there is an incomplete last piece, read that but store it bytewise */
    if (rsp_len & 7)
    {
        uint64_t RDR;
        uint8_t* rsp8 = (uint8_t*)rsp64;

        FAPI_TRY(wait_for_rdr_full());
        FAPI_TRY(getscom(SPIC_RECEIVE_DATA_REG, RDR));
        RDR <<= (8 * (8 - (rsp_len & 7)));

        for (uint32_t i = 0; i < (rsp_len & 7); i++)
        {
            *rsp8 = RDR >> 56;
            rsp8++;
            RDR <<= 8;
        }
    }

    FAPI_TRY(wait_for_idle());

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SPIPort::poll_status(uint32_t i_bit, bool i_wait_for) const
{
    buffer<uint64_t> l_buf;
    uint32_t l_timeout = POLL_COUNT;

    static const uint64_t ERROR_MASK = 0x64000000FFE1FF80;

    while (l_timeout)
    {
        FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_STATUS_REG, l_buf));

        if ((l_buf & ERROR_MASK) != 0)
        {
            buffer<uint64_t> l_seq, l_counter, l_clk_config;
            FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_SEQUENCER_OP_REG, l_seq));
            FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_COUNTER_REG, l_counter));
            FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_CLOCK_CONFIG_REG, l_clk_config));
            FAPI_ASSERT(false,
                        SPI_ERROR_STATUS()
                        .set_TARGET(iv_target)
                        .set_BASE_ADDRESS(iv_base_address)
                        .set_STATUS_REG(l_buf)
                        .set_SEQUENCE_REG(l_seq)
                        .set_COUNTER_REG(l_counter)
                        .set_CLOCK_CONFIG_REG(l_clk_config),
                        "SPI controller signaled an error: status_reg=0x%08X%08X",
                        l_buf >> 32, l_buf & 0xFFFFFFFF);
        }

        if (l_buf.getBit(i_bit) == i_wait_for)
        {
            return FAPI2_RC_SUCCESS;
        }

        delay(POLL_DELAY_NS, POLL_DELAY_CYCLES);
        l_timeout--;
    }

    {
        buffer<uint64_t> l_seq, l_counter, l_clk_config;
        FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_SEQUENCER_OP_REG, l_seq));
        FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_COUNTER_REG, l_counter));
        FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_CLOCK_CONFIG_REG, l_clk_config));
        FAPI_ASSERT(false,
                    SPI_POLL_TIMEOUT()
                    .set_TARGET(iv_target)
                    .set_BASE_ADDRESS(iv_base_address)
                    .set_POLL_BIT(i_bit)
                    .set_POLL_VALUE(i_wait_for)
                    .set_SEQUENCE_REG(l_seq)
                    .set_COUNTER_REG(l_counter)
                    .set_CLOCK_CONFIG_REG(l_clk_config),
                    "Timed out polling for bit %d to turn to %d",
                    i_bit, i_wait_for);
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SPIPort::reset_controller() const
{
    buffer<uint64_t> CLK;

    FAPI_TRY(getscom(SPIC_CLOCK_CONFIG_REG, CLK));

    CLK.insertFromRight<RESET_CONTROL, RESET_CONTROL_LEN>(RESET_PART1);
    FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, CLK));

    CLK.insertFromRight<RESET_CONTROL, RESET_CONTROL_LEN>(RESET_PART2);
    FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, CLK));

    CLK.insertFromRight<RESET_CONTROL, RESET_CONTROL_LEN>(0);
    FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, CLK));
    FAPI_TRY(putscom(SPIC_STATUS_REG, 0));

fapi_try_exit:
    return current_err;
}
