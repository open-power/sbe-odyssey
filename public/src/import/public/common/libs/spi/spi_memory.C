/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_memory.C $       */
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
#include <algorithm>
#include "spi_memory.H"
#include "ibmecc.H"

using namespace fapi2;
using std::min;

enum spi_memory_constants
{
    WRITE_POLL_DELAY_NS = 50000,
    WRITE_POLL_DELAY_CYCLES = 1000,
    WRITE_POLL_COUNT_PER_MS = 20,

    // Max page write times according to datasheets:
    //   AT25M01 SEEPROM: 5ms
    //   MT25Q NOR Flash: 1.8ms
    //   MX66 NOR Flash:  3ms
    // Generously round up to 10ms for good measure.
    PAGE_WRITE_TIMEOUT_MS = 10,
};

ReturnCode spi::SEEPROMDevice::check_start_and_length(uint32_t i_start, uint32_t i_length) const
{
    const uint32_t raw_start = ecc_correct(i_start);
    const uint32_t raw_length = ecc_correct(i_length);

    FAPI_ASSERT((iv_ecc_mode == ECC_DISABLED) || (!(i_start & 7) && !(i_length & 7)),
                SPI_MEMORY_INVALID_ADDR_LEN()
                .set_START(i_start)
                .set_LENGTH(i_length),
                "Invalid start/length for ECC mode: i_start=0x%08x i_length=0x%08x",
                i_start, i_length);

    FAPI_ASSERT(raw_start + raw_length <= iv_raw_size,
                SPI_MEMORY_ADDRESS_OVERRUN()
                .set_START(i_start)
                .set_LENGTH(i_length)
                .set_RAW_START(raw_start)
                .set_RAW_LENGTH(raw_length)
                .set_RAW_DEVICE_SIZE(iv_raw_size),
                // Not tracing i_length since PPE only supports up to 4 values
                "Request exceeds device size: start=0x%08x raw_start=0x%08x raw_length=%08x raw_device_size=0x%08x",
                i_start, raw_start, raw_length, iv_raw_size);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::check_write_active(operation_t i_operation, bool i_expect)
{
    FAPI_ASSERT(iv_write_active == i_expect,
                SPI_MEMORY_INVALID_WRITE_SEQUENCE()
                .set_OPERATION(i_operation)
                .set_WRITE_ACTIVE(iv_write_active),
                "SPI write operations called in incorrect sequence: operation=%d write_active=%d",
                i_operation, iv_write_active);

fapi_try_exit:
    return current_err;
}

// The SPI controller cannot transfer more than 256 8byte words in one go,
// so we have to split larger blocks up into chunks.
constexpr uint32_t MAX_READ_SIZE = 0x800;

ReturnCode spi::SEEPROMDevice::read(uint32_t i_start, uint32_t i_length, void* o_buffer) const
{
    uint8_t* buf8 = (uint8_t*)o_buffer;
    uint32_t address = ecc_correct(i_start);
    // When reading with ECC (either correcting or discarding), the length increase
    // caused by the ECC byte will be handled inside the transaction so we must not
    // apply ECC correction here.
    uint32_t length = i_length;

    FAPI_INF("Memory read i_start=0x%08x (ECC corrected 0x%08x) i_length=0x%08x",
             i_start, address, i_length);
    FAPI_TRY(check_start_and_length(i_start, i_length));

    while (length)
    {
        const uint32_t nbytes = min(length, MAX_READ_SIZE);

        FAPI_TRY(iv_port.transaction(X25_CMD_READ | (address & X25_ADDR_MASK), 4,
                                     NULL, 0, buf8, nbytes, true));
        length -= nbytes;
        buf8 += nbytes;
        address += ecc_correct(nbytes);
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::write_begin(uint32_t i_start, uint32_t i_length)
{
    FAPI_TRY(check_write_active(WRITE_BEGIN, false));
    FAPI_TRY(check_start_and_length(i_start, i_length));

    iv_write_start = iv_write_pos = ecc_correct(i_start);
    iv_write_end = iv_write_start + ecc_correct(i_length);
    iv_buf_pos = 0;
    iv_write_active = true;

    FAPI_INF("SEEPROM write begin i_start=0x%08x i_length=0x%08x "
             "iv_write_start=0x%08x iv_write_end=0x%08x",
             i_start, i_length, iv_write_start, iv_write_end);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::wait_for_write_complete(
    int i_timeout_ms, int i_operation, int i_address, int i_length) const
{
    uint8_t status;
    uint32_t timeout = i_timeout_ms * WRITE_POLL_COUNT_PER_MS;

    while (true)
    {
        FAPI_TRY(read_status(status));

        if (!(status & SR_WIP))
        {
            break;
        }

        timeout--;

        FAPI_ASSERT(timeout > 0,
                    SPI_MEMORY_WRITE_TIMEOUT()
                    .set_OPERATION(i_operation)
                    .set_ADDRESS(i_address)
                    .set_LENGTH(i_length),
                    "Timed out waiting for operation %d to complete: "
                    "i_address=0x%08x i_length=0x%08x",
                    i_address, i_length);

        delay(WRITE_POLL_DELAY_NS, WRITE_POLL_DELAY_CYCLES);
    }

    if (status & SR_WEL)
    {
        FAPI_TRY(write_disable());
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::write_page(uint32_t i_start, uint32_t i_length, const uint8_t* i_buffer) const
{
    FAPI_TRY(write_enable());
    FAPI_TRY(iv_port.transaction(X25_CMD_PAGE_PROGRAM | (i_start & X25_ADDR_MASK), 4,
                                 i_buffer, i_length, NULL, 0, true));

    FAPI_TRY(wait_for_write_complete(PAGE_WRITE_TIMEOUT_MS, OP_PAGE_WRITE, i_start, i_length));

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::flush_page_buf()
{
    // iv_write_pos points to the _next_ byte that will be written, so
    // (iv_write_pos - 1) is the _last_ byte written. We want to know that
    // byte's page start address.
    const uint32_t page_start = (iv_write_pos - 1) & ~WRITE_PAGE_MASK;

    // The first page we're writing might be written only partially, so
    // figure out the offset inside the page where we should start.
    const uint32_t offset = page_start < iv_write_start ? iv_write_start & WRITE_PAGE_MASK : 0;

    // The write length depends on the offset (which may indicate starting
    // in the middle of a page) and the write position (which may indicate ending
    // in the middle of a page). The difference (modulo the page size) is the write
    // length.
    // Subtract one before modulo and add it back afterwards so that the output range
    // goes from 1..WRITE_PAGE_SIZE instead of 0..(WRITE_PAGE_SIZE-1).
    const uint32_t length = ((iv_write_pos - offset - 1) & WRITE_PAGE_MASK) + 1;

    FAPI_DBG("flush_page_buf iv_write_pos=0x%08x page_start=0x%08x offset=0x%03x length=0x%03x",
             iv_write_pos, page_start, offset, length);

    iv_buf_pos = 0;
    return write_page(page_start + offset, length, iv_page_buffer);
}

inline ReturnCode spi::SEEPROMDevice::write_byte(uint8_t i_byte)
{
    // This function is kept minimal (little code, no traces, no FAPI_TRY)
    // so that the compiler may inline it into write_data().

    iv_page_buffer[iv_buf_pos] = i_byte;
    iv_write_pos++;
    iv_buf_pos++;

    if ((iv_write_pos & WRITE_PAGE_MASK) == 0)
    {
        return flush_page_buf();
    }

    return FAPI2_RC_SUCCESS;
}

ReturnCode spi::SEEPROMDevice::write_data(const void* i_buffer, uint32_t i_length)
{
    auto buf8 = static_cast<const uint8_t*>(i_buffer);

    FAPI_INF("SEEPROM write data iv_write_pos=0x%08x iv_write_end=0x%08x i_length=0x%08x",
             iv_write_pos, iv_write_end, i_length);

    FAPI_TRY(check_write_active(WRITE_DATA, true));
    FAPI_TRY(check_start_and_length(0, i_length));

    for (uint32_t i = 0; i < i_length;)
    {
        uint64_t word = 0;

        // As long as we have data available, try to write 8 bytes at a time...
        for (uint32_t j = 0; (j < 8) && (i < i_length); j++, i++)
        {
            uint8_t byte = buf8[i];
            FAPI_TRY(write_byte(byte));
            word = word << 8 | byte;
        }

        // ...followed by an ECC byte if ECC is enabled
        if (iv_ecc_mode != ECC_DISABLED)
        {
            FAPI_TRY(write_byte(ecc64_gen(word)));
        }
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::write_end()
{
    FAPI_INF("SEEPROM write end iv_write_pos=0x%08x iv_write_end=0x%08x",
             iv_write_pos, iv_write_end);

    FAPI_TRY(check_write_active(WRITE_END, true));

    if ((iv_write_pos & WRITE_PAGE_MASK) != 0)
    {
        FAPI_TRY(flush_page_buf());
    }

    iv_write_active = false;

    FAPI_ASSERT(iv_write_pos == iv_write_end,
                SPI_MEMORY_INCOMPLETE_WRITE()
                .set_ECC_MODE(iv_ecc_mode)
                .set_START_ADDRESS(iv_write_start)
                .set_END_ADDRESS(iv_write_end)
                .set_FINAL_WRITE_POINTER(iv_write_pos),
                "Incomplete write operation: ecc_mode=%d start_address=0x%08x "
                "end_address=0x%08x write_pointer=0x%08x",
                iv_ecc_mode, iv_write_start, iv_write_end, iv_write_pos);

fapi_try_exit:
    return current_err;
}
