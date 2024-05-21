/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_base.C $         */
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
#include "spi_base.H"
#include <endian.h>

using namespace fapi2;
using namespace spi;

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

    TPM_RDR_MATCH           = 0x00000000FF01FF00ull,

    // Mask to check for controller errors while polling status.
    // Trips on every defined error except:
    //   bit  0 : RDR full : needed for receive
    //   bit  3 : reserved
    //   bit  4 : TDR full : needed for transmit
    //   bit  6 : TDR underrun : occurs at the end of transmission
    //   bit  7 : reserved
    //   bits 8:31 : various FSM states
    //   bit 43 : ECC CE : corrected and thus benign
    //   bits 45:46 : unused
    //   bit 57 : mux indicator : not an error
    //   bit 58 : PIB write returned RETRY response :
    //            caused by external entity and
    //            should not disrupt ongoing SPI transaction
    //   bits 59:63 : unused
    ERROR_MASK = 0x64000000FFE9FF80ull,

    // Masks to check for proper idle state:
    // No error flags whatsoever, state machines idle, ignore sequence index
    // Masks:
    // bit-3 : RDR_reserved  : Not used
    // bit-6 : TDR_under_run : set when no tx data available
    // bit-7 : TDR_reserved  : Not used
    // bits[28:31] : Sequence index currently being executed
    // bits[58:63] : Unused general status bits
    IDLE_STATUS_MASK = 0xECFFFFF0FFFFFFC0ull,
    IDLE_STATUS_EXP  = 0x0001001000000000ull,
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

ReturnCode _SPIPortBase::transaction(const uint64_t cmd, const uint32_t cmd_len,
                                     const void* req, const uint32_t req_len,
                                     void* rsp, const uint32_t rsp_len,
                                     const bool i_use_ecc) const
{
    // Combine command and request payload since otherwise we
    // might end up with more than 8 ops in the sequence.
    // Exception: In TPM mode we send the command separately
    const uint32_t total_req_len = iv_tpm_mode ? req_len : cmd_len + req_len;
    const ecc_mode l_ecc_mode = i_use_ecc ? iv_ecc_mode : ECC_DISABLED;
    uint64_t saved_seq = -1ULL, saved_ctr = -1ULL, saved_clk = -1ULL;
    uint8_t delay_cs = 0;

    FAPI_DBG("SPI transaction: cmd=0x%08x%08x ecc|cmd_len=0x%04x req_len|rsp_len=0x%08x",
             cmd >> 32, cmd & 0xFFFFFFFF, (l_ecc_mode << 8) | cmd_len,
             (req_len << 16) | rsp_len);

    if (cmd_len > 8 || req_len > 2048 || rsp_len > 2048)
    {
        FAPI_ERR("Unsupported length: cmd_len=%d (max 8) req_len=%d (max 2048) rsp_len=%d (max 2048)",
                 cmd_len, req_len, rsp_len);
        return log_invalid_parms(cmd, cmd_len, req_len, rsp_len, i_use_ecc);
    }

    if (rsp_len == 0)
    {
        /* Workaround time! \o/
         *
         * The SPI controller deasserts CS# too quickly after the last bit has been transferred.
         * During SPI reads nobody cares but during SPI writes some devices will not take this well,
         * so we have to apply a workaround: Omit the CS# deassert operation from the sequence and
         * wait for the sequencer to complete, then reset the entire SPI engine to deassert CS#.
         * This costs us a bit of performance (but not much) and makes sure that CS# is deasserted
         * long after the last bit has been transferred.
         *
         * Make this conditional on an EC attr so that we don't have to pull these shenanigans on
         * a chip with a fixed SPI controller.
         */
        FAPI_TRY(FAPI_ATTR_GET(ATTR_CHIP_EC_FEATURE_SPI_WRITE_NEEDS_CS_DELAY, iv_target, delay_cs));
    }

    /* Check SPI controller health before we begin */
    {
        buffer<uint64_t> status;

        FAPI_TRY(getscom(SPIC_STATUS_REG, status));

        if ((status & IDLE_STATUS_MASK) != IDLE_STATUS_EXP)
        {
            FAPI_INF("SPI controller status not clean (0x%08x%08x), resetting controller",
                     status >> 32, status);
            FAPI_TRY(reset_controller());

            // One more status check, this time with FAPI_ASSERT on bad status
            FAPI_TRY(wait_for_tdr_empty());
        }
    }

    /* Set up ECC mode */
    {
        buffer<uint64_t> CLK;
        FAPI_TRY(getscom(SPIC_CLOCK_CONFIG_REG, CLK));
        saved_clk = CLK;

        if (CLK.getBits<ECC_CONTROL, ECC_CONTROL_LEN>() != l_ecc_mode)
        {
            CLK.insertFromRight<ECC_CONTROL, ECC_CONTROL_LEN>(l_ecc_mode);
            FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, CLK));
        }
    }

    /* Save off the previous sequence and counter */
    FAPI_TRY(getscom(SPIC_SEQUENCER_OP_REG, saved_seq));
    FAPI_TRY(getscom(SPIC_COUNTER_REG, saved_ctr));

    // In TPM mode set up pattern match
    if (iv_tpm_mode)
    {
        FAPI_TRY(putscom(SPIC_MEMORY_MAPPING_REG, TPM_RDR_MATCH));
    }

    /* Game time */
    FAPI_TRY(setup_seq_cnt(iv_resp_select, l_ecc_mode, cmd_len, total_req_len, rsp_len, delay_cs));

    if (iv_tpm_mode)
    {
        FAPI_TRY(transmit_req_tpm(cmd, cmd_len, req, req_len, total_req_len));
    }
    else
    {
        FAPI_TRY(transmit_req(cmd, cmd_len, req, req_len, total_req_len));
    }

    FAPI_TRY(receive_rsp(rsp, rsp_len));
    FAPI_TRY(complete_transaction(delay_cs));

fapi_try_exit:
    /* Save return code and restore all the registers we saved off */
    const ReturnCode rc = current_err;
    const ReturnCode restore_rc =
        restore_spi_regs(saved_seq, saved_ctr, saved_clk);
    /* A bad RC from the main code takes precedence over a bad RC from the restore */
    return (rc != FAPI2_RC_SUCCESS) ? rc : restore_rc;
}

ReturnCode _SPIPortBase::setup_seq_cnt(
    uint8_t resp_select,
    uint8_t ecc_mode,
    uint16_t cmd_len,
    uint16_t req_len,
    uint16_t rsp_len,
    uint8_t& delay_cs) const
{
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
     *
     * In TPM mode we cannot use this mode since it interferes with the
     * self-paced waiting for the TPM ready signal. It is also not
     * necessary since the SPI controller seems to do receive pacing
     * already.
     */
    uint64_t CNT = iv_tpm_mode ? 0x6500 : 0x6F00;

    /* Start with a responder select op */
    uint64_t SEQ = SPIOP_SELECT_RESP(1 << resp_select);
    uint32_t n_ops = 1;

    // In TPM mode, send the initial command and address,
    // then wait for the TPM to report ready.
    if (iv_tpm_mode)
    {
        SEQ = (SEQ << 24) |
              SPIOP_SHIFT_N1(cmd_len) << 16 |
              SPIOP_SHIFT_N2(1) << 8 |
              SPIOP_B_RDR_NE(n_ops + 1);
        n_ops += 3;
    }

    /* Transmit entire words if necessary */
    if (req_len >= 8)
    {
        CNT |= (uint64_t)((req_len / 8) - 1) << 32;
        SEQ = (SEQ << 16) |
              SPIOP_SHIFT_N1(8) << 8 |
              SPIOP_BNEI1(n_ops);
        n_ops += 2;
    }

    /* Transmit remainder if there is one */
    if (req_len & 7)
    {
        SEQ = (SEQ << 8) |
              SPIOP_SHIFT_N1(req_len & 7);
        n_ops++;
    }

    /* Receive response, entire words first */
    if (rsp_len >= 8)
    {
        CNT |= (uint64_t)((rsp_len / 8) - 1) << 24;
        SEQ = (SEQ << 16) |
              /* If we're looking at or discarding ECC, receive 9 bytes */
              SPIOP_SHIFT_N2((ecc_mode != spi::ECC_DISABLED) ? 9 : 8) << 8 |
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

    if (delay_cs)
    {
        /* Omit the deselect op but store the op index where it would have gone */
        delay_cs = n_ops;
    }
    else
    {
        /* Finally deselect */
        SEQ = (SEQ << 8) | SPIOP_SELECT_RESP(0);
        n_ops++;
    }

    /* Shift sequence into final position */
    SEQ <<= 8 * (8 - n_ops);

    FAPI_TRY(putscom(SPIC_SEQUENCER_OP_REG,  SEQ));
    FAPI_TRY(putscom(SPIC_COUNTER_REG,       CNT));

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::transmit_req(const uint64_t cmd, const uint32_t cmd_len,
                                      const void* req, const uint32_t req_len,
                                      const uint32_t total_req_len) const
{
    // The generic variant needs to merge the command and request payload
    // together, so there's some buffering and shifting involved.

    // Prepare TDR for command and first bits of data
    const uint64_t* req64 = (uint64_t*)req;
    const uint32_t req_chunks = (total_req_len + 7) / 8;
    uint64_t TDR = cmd << (8 * (8 - cmd_len));
    uint64_t read_buf = 0;
    uint32_t read_buf_shift = cmd_len * 8;
    uint32_t read_buf_remain = 64 - read_buf_shift;

    if (req_len)
    {
        read_buf = be64toh(*req64);
        req64++;
        // Note that if read_buf_shift == 64 the shift result
        // is zero according to the C++ standard.
        TDR |= (read_buf >> read_buf_shift);
    }

    /* Send first chunk of data */
    FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));

    /* Send remaining chunks of data */
    for (uint32_t i = 1; i < req_chunks; i++)
    {
        FAPI_TRY(wait_for_tdr_empty());
        // Note that if read_buf_remain == 64 the shift result
        // is zero according to the C++ standard.
        TDR = read_buf << read_buf_remain;
        read_buf = be64toh(*req64);
        req64++;
        // Note that if read_buf_shift == 64 the shift result
        // is zero according to the C++ standard.
        TDR |= read_buf >> read_buf_shift;
        FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));
    }

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::transmit_req_tpm(const uint64_t cmd, const uint32_t cmd_len,
        const void* req, const uint32_t req_len,
        const uint32_t total_req_len) const
{
    // The TPM variant has an easier task since there is no merging of command
    // and payload, but it needs to wait for the pattern match instruction
    // after the initial command to complete before sending the payload.

    // Prepare TDR for command and first bits of data
    const uint64_t* req64 = (uint64_t*)req;
    const uint32_t req_chunks = (total_req_len + 7) / 8;

    /* Send command */
    uint64_t TDR = cmd << (8 * (8 - cmd_len));
    FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));

    // Wait until we pass the wait for pattern match
    FAPI_TRY(wait_for_instruction(4));

    /* Send request payload */
    for (uint32_t i = 0; i < req_chunks; i++)
    {
        // workaround for unaligned buffers, which unfortunately
        // is required in TPM mode.
        // TODO implement proper handling of unaligned buffers
        uint64_t dataAlign64;
        memcpy(&dataAlign64, req64, sizeof(uint64_t));
        TDR = be64toh(dataAlign64);
        req64++;
        FAPI_TRY(putscom(SPIC_TRANSMIT_DATA_REG, TDR));
        FAPI_TRY(wait_for_tdr_empty());
    }

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::receive_rsp(void* rsp, const uint32_t rsp_len) const
{
    uint64_t* rsp64 = (uint64_t*)rsp;

    if (rsp_len && !iv_tpm_mode)
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
        uint64_t dataAlign64;
        FAPI_TRY(wait_for_rdr_full());
        FAPI_TRY(getscom(SPIC_RECEIVE_DATA_REG, RDR));
//        *rsp64 = htobe64(RDR);
        // temporary workaround for unaligned buffers
        // TODO implement proper handling of unaligned buffers
        dataAlign64 = htobe64(RDR);
        memcpy(rsp64, &dataAlign64, sizeof(RDR));
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

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::complete_transaction(uint8_t delay_cs) const
{
    if (delay_cs)
    {
        /* Wait until the sequencer is at the end of the instruction sequence and hangs */
        FAPI_TRY(wait_for_instruction(delay_cs));
        /* Then reset it */
        FAPI_TRY(reset_controller());
    }
    else
    {
        FAPI_TRY(wait_for_idle());
    }

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::restore_spi_regs(
    uint64_t i_saved_seq,
    uint64_t i_saved_ctr,
    uint64_t i_saved_clk) const
{
    if (i_saved_seq != -1ULL)
    {
        FAPI_TRY(putscom(SPIC_SEQUENCER_OP_REG, i_saved_seq));
    }

    if (i_saved_ctr != -1ULL)
    {
        FAPI_TRY(putscom(SPIC_COUNTER_REG, i_saved_ctr));
    }

    if (i_saved_clk != -1ULL)
    {
        FAPI_TRY(putscom(SPIC_CLOCK_CONFIG_REG, i_saved_clk));
    }

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::write_status(uint64_t i_status) const
{
    return putscom(SPIC_STATUS_REG, i_status);
}

ReturnCode _SPIPortBase::poll_status(
    const uint32_t i_bitrange,
    const uint32_t i_wait_for,
    comparison i_comparison) const
{
    const uint32_t l_start_bit = i_bitrange >> 8;
    const uint32_t l_nbits = i_bitrange & 0xFF;
    buffer<uint64_t> l_buf;
    uint32_t l_timeout = POLL_COUNT;

    while (l_timeout)
    {
        FAPI_TRY(getScom(iv_target, iv_base_address + SPIC_STATUS_REG, l_buf));

        /* Check on SPI errors */
        FAPI_TRY(handle_spi_errors(l_buf));

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

        const uint32_t l_cur_value = l_buf.getBits(l_start_bit, l_nbits);

        if ((i_comparison == EQUAL && l_cur_value == i_wait_for)
            || (i_comparison == GREATER_OR_EQUAL && l_cur_value >= i_wait_for))
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
                    .set_POLL_BITRANGE(i_bitrange)
                    .set_POLL_VALUE(i_wait_for)
                    .set_SEQUENCE_REG(l_seq)
                    .set_COUNTER_REG(l_counter)
                    .set_CLOCK_CONFIG_REG(l_clk_config),
                    "Timed out polling for bits %d:%d to turn to %d",
                    l_start_bit, l_start_bit + l_nbits - 1, i_wait_for);
    }

fapi_try_exit:
    return current_err;
}

ReturnCode _SPIPortBase::reset_controller() const
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

ReturnCode _SPIPortBase::log_invalid_parms(const uint64_t i_cmd,
        const uint32_t i_cmd_len,
        const uint32_t i_req_len,
        const uint32_t i_rsp_len,
        const bool i_use_ecc) const
{
    FAPI_ASSERT(false,
                SPI_INVALID_TRANSACTION_PARMS()
                .set_TARGET(iv_target)
                .set_BASE_ADDRESS(iv_base_address)
                .set_COMMAND(i_cmd)
                .set_CMD_LENGTH(i_cmd_len)
                .set_REQ_LENGTH(i_req_len)
                .set_RSP_LENGTH(i_rsp_len)
                .set_TPM_MODE(iv_tpm_mode)
                .set_ECC_MODE(i_use_ecc), "");
fapi_try_exit:
    return current_err;
}

ReturnCode SPITPMPort::transaction(const uint64_t i_cmd, uint32_t i_cmd_len,
                                   const void* i_req, uint32_t i_req_len,
                                   void* o_rsp, uint32_t i_rsp_len,
                                   bool i_use_ecc) const
{
    if (i_use_ecc || (i_req_len && i_rsp_len))
    {
        FAPI_ERR("TPM transaction must be only read or only write and does not support ECC");
        return log_invalid_parms(i_cmd, i_cmd_len, i_req_len, i_rsp_len, i_use_ecc);
    }

    return spi::_SPIPortBase::transaction(i_cmd, i_cmd_len, i_req, i_req_len, o_rsp, i_rsp_len);
}
