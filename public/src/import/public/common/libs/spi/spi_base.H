/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_base.H $         */
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

/// @brief Base driver support for generic SPI transactions
/// @author Joachim Fenkes <fenkes@de.ibm.com>

#pragma once

#include <fapi2.H>

namespace spi
{

/**
 * @brief Selection of ECC operation mode
 *
 * This mode will control the behavior of both the SPI port driver as well as the
 * SEEPROM/Flash drivers sitting on top of the port, so it is defined here and specified
 * at port instantiation.
 *
 * It controls how ECC data is handled during read and write operations, and it also
 * controls the meaning of addresses handed to the memory device drivers:
 *
 * ECC_ENABLED and ECC_DISCARD imply logical addressing, where memory addresses look like
 * the memory is comprised of 8-byte words and ECC doesn't exist. The drivers will
 * internally perform the multiplication by 9/8ths to generate raw Flash device addresses.
 *
 * ECC_DISABLED implies physical/raw addressing, where the addresses correspond 1:1 to
 * raw Flash memory addresses.
 *
 * Writes in ECC_ENABLED and ECC_DISCARD modes will implicitly generate ECC for the write
 * data. Writes in ECC_DISABLED mode do not change the user data in any way, they are raw
 * writes.
 *
 * Reads in ECC_ENABLED mode perform ECC checking and correction; they will yield a
 * SPI_ERROR_STATUS error if an ECC UE is detected. Reads in ECC_DISCARD mode ignore ECC
 * and discard the ECC byte on writes. Reads in ECC_DISABLED mode return raw Flash data.
 */
enum ecc_mode
{
    ECC_ENABLED = 0,
    ECC_DISABLED = 1,
    ECC_DISCARD = 2,
};

/**
 * @brief Abstract base class for a generic external interface port (could be SPI or I2C)
 *
 * This base class provides a common API for user code to rely on. Specific driver implementations
 * need to override the virtual methods.
 */
class AbstractPort
{
    public:
        /**
         * @brief Execute a full transaction - command and response
         *
         * This will execute a single transaction consisting of a data write followed by an optional data read.
         * For usage simplicity, write data can be supplied in the form of a command which may be up to 8 bytes,
         * and a request payload which may be any size, including zero. The write data will be the concatenation
         * of command and payload.
         * Optionally, zero or more bytes of response data may be received back from the device.
         *
         * @param[in]  i_cmd     The command to send to the device, right-aligned within the value
         * @param[in]  i_cmd_len Length of the command in bytes; allowed values are 0..8
         * @param[in]  i_req     The command payload to send to the device; must be NULL if no payload; must be 8-byte aligned
         * @param[in]  i_req_len The length of the command payload; must be zero if no payload; maximum length 1024
         * @param[out] o_rsp     Buffer for the response received from the device; must be NULL if no response is expected; must be 8-byte aligned
         * @param[in]  i_rsp_len Length of the response in bytes; must be zero if no response is expected; maximum 1024
         * @param[in]  i_use_ecc If true, the ECC mode of the port is used (data transfer). If false, ECC is disabled (command/status transfer).
         */
        virtual fapi2::ReturnCode transaction(const uint64_t i_cmd, const uint32_t i_cmd_len,
                                              const void* i_req, const uint32_t i_req_len,
                                              void* o_rsp, const uint32_t i_rsp_len,
                                              const bool i_use_ecc = false) const = 0;

        /**
         * @return the ECC mode this port is configured to
         */
        virtual ecc_mode get_ecc_mode() const = 0;
};

/**
 * @brief Driver for a SPI port
 */
class SPIPort : public AbstractPort
{
    public:
        /**
         * @brief Construct a port object
         *
         * @param[in] i_target        Target chip containing the SPI controller logic
         * @param[in] i_base_address  SCOM base address of the targeted SPI controller port;
         *                            i.e. address of its _ERROR_INJECT register
         * @param[in] i_resp_select   Responder select value, range 0..3
         * @param[in] i_ecc_mode      ECC mode to use for all transactions
         */
        SPIPort(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                const uint32_t i_base_address, const uint8_t i_resp_select,
                const ecc_mode i_ecc_mode) :
            iv_target(i_target), iv_base_address(i_base_address),
            iv_resp_select(i_resp_select), iv_ecc_mode(i_ecc_mode)
        {}

        /**
         * @brief Reset the SPI controller logic
         */
        fapi2::ReturnCode reset_controller() const;

        /**
         * @brief Conduct a transaction; see AbstractPort::transaction() for details
         */
        fapi2::ReturnCode transaction(const uint64_t i_cmd, uint32_t i_cmd_len,
                                      const void* i_req, uint32_t i_req_len,
                                      void* o_rsp, uint32_t i_rsp_len,
                                      bool i_use_ecc = false) const override;

        /**
         * @return the ECC mode this port is configured to
         */
        ecc_mode get_ecc_mode() const override
        {
            return iv_ecc_mode;
        }

    private:
        static const uint32_t TDR_FULL_STATUS_BIT = 4;
        static const uint32_t RDR_FULL_STATUS_BIT = 0;
        static const uint32_t SEQUENCER_FSM_IDLE_BIT = 15;
        static const uint32_t SHIFTER_FSM_IDLE_BIT = 27;

        const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& iv_target;
        const uint32_t iv_base_address;
        const uint32_t iv_resp_select;
        const ecc_mode iv_ecc_mode;

        /**
         * @brief Wrapper around SCOM write accesses to simplify driver code
         * @param[in] i_reg   Register address relative to the SCOM base address
         * @param[in] i_data  Data to write
         */
        fapi2::ReturnCode putscom(uint32_t i_reg, uint64_t i_data) const
        {
            return fapi2::putScom(iv_target, iv_base_address + i_reg, i_data);
        }

        /**
         * @brief Wrapper around SCOM read accesses to simplify driver code
         * @param[in]  i_reg   Register address relative to the SCOM base address
         * @param[out] o_data  Read data goes here
         */
        fapi2::ReturnCode getscom(uint32_t i_reg, uint64_t& o_data) const
        {
            fapi2::buffer<uint64_t> l_data;
            auto l_rc = fapi2::getScom(iv_target, iv_base_address + i_reg, l_data);
            o_data = l_data;
            return l_rc;
        }

        /**
         * @brief Generic status polling function; waits for a single bit in the status reg to turn a specific value
         *
         * @param[in] i_bit       The bit index to poll on
         * @param[in] i_wait_for  Wait for the bit to turn 0 or 1 depending on this value
         */
        fapi2::ReturnCode poll_status(uint32_t i_bit, bool i_wait_for) const;

        /**
         * @brief Inline wrappers which poll for specific status bits
         */
        fapi2::ReturnCode wait_for_tdr_empty() const
        {
            FAPI_TRY(poll_status(TDR_FULL_STATUS_BIT, false),
                     "Error while waiting for TDR empty");
        fapi_try_exit:
            return fapi2::current_err;
        }

        fapi2::ReturnCode wait_for_rdr_full() const
        {
            FAPI_TRY(poll_status(RDR_FULL_STATUS_BIT, true),
                     "Error while waiting for RDR full");
        fapi_try_exit:
            return fapi2::current_err;
        }

        fapi2::ReturnCode wait_for_idle() const
        {
            FAPI_TRY(poll_status(SEQUENCER_FSM_IDLE_BIT, true),
                     "Error while waiting for SPI idle");
        fapi_try_exit:
            return fapi2::current_err;
        }

        /**
         * @brief Restore SPI controller registers saved by SPIPort::transaction.
         */
        fapi2::ReturnCode restore_spi_regs(
            uint64_t i_saved_seq,
            uint64_t i_saved_ctr,
            uint64_t i_saved_clk) const;

        /**
         * @brief Set up sequence and counter registers
         */
        fapi2::ReturnCode setup_seq_cnt(
            uint8_t resp_select,
            uint8_t ecc_mode,
            uint16_t req_len,
            uint16_t rsp_len) const;

        /**
         * @brief Transmit the command + request of a transaction
         */
        fapi2::ReturnCode transmit_req(const uint64_t cmd, const uint32_t cmd_len,
                                       const void* req, const uint32_t req_len,
                                       const uint32_t total_req_len) const;

        /**
         * @brief Receive the response of a transaction
         */
        fapi2::ReturnCode receive_rsp(void* rsp, const uint32_t rsp_len) const;
};

}
