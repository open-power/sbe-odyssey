/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spi_memory.C $       */
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
#include <string.h>
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

    // Max erase times according to datasheets:
    //   MT25Q: 1s
    //   MX66:  2.2s   (yes SECONDS)
    ERASE_TIMEOUT_MS = 3000,
};

enum flash_device_commands
{
    X25_CMD_READ_ID = 0x9F,
    X25_CMD_RESET1 = 0x66,
    X25_CMD_RESET2 = 0x99,
    MT25Q_CMD_READ_FLAG_STATUS_REG = 0x70,
    MT25Q_CMD_CLR_FLAG_STATUS_REG = 0x50,
    MX66_CMD_READ_SECURITY_REG = 0x2B,

    // Commands with 24-bit address
    X25_CMD_ERASE_64K = 0xD8000000,
    X25_CMD_ERASE_32K = 0x52000000,
    X25_CMD_ERASE_4K = 0x20000000,
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
    uint32_t address = ecc_correct(i_start);
    // When reading with ECC (either correcting or discarding), the length increase
    // caused by the ECC byte will be handled inside the transaction so we must not
    // apply ECC correction here.
    uint32_t length = i_length;

    FAPI_DBG("Memory read i_start=0x%08x (ECC corrected 0x%08x) i_length=0x%08x",
             i_start, address, i_length);
    FAPI_TRY(check_start_and_length(i_start, i_length));
    FAPI_TRY(read_internal(address, length, true, o_buffer));

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::read_internal(
    uint32_t i_start, uint32_t i_length, bool i_use_ecc, void* o_buffer) const
{
    uint8_t* buf8 = (uint8_t*)o_buffer;

    while (i_length)
    {
        const uint32_t nbytes = min(i_length, MAX_READ_SIZE);

        FAPI_TRY(iv_port.transaction(X25_CMD_READ | (i_start & X25_ADDR_MASK), 4,
                                     NULL, 0, buf8, nbytes, i_use_ecc));
        i_length -= nbytes;
        buf8 += nbytes;
        i_start += ecc_correct(nbytes);
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
    iv_verify_fail = false;
    iv_write_active = true;

    FAPI_INF("Memory write begin i_start=0x%08x i_length=0x%08x "
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
                    i_operation, i_address, i_length);

        delay(WRITE_POLL_DELAY_NS, WRITE_POLL_DELAY_CYCLES);
    }

    if (status & SR_WEL)
    {
        FAPI_TRY(write_disable());
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::write_page(uint32_t i_start, uint32_t i_length, const uint8_t* i_buffer)
{
    FAPI_TRY(write_enable());
    FAPI_TRY(iv_port.transaction(X25_CMD_PAGE_PROGRAM | (i_start & X25_ADDR_MASK), 4,
                                 i_buffer, i_length, NULL, 0, true));

    FAPI_TRY(wait_for_write_complete(PAGE_WRITE_TIMEOUT_MS, OP_PAGE_WRITE, i_start, i_length));

    if (iv_verify)
    {
        FAPI_TRY(verify_write(i_start, i_length, i_buffer));
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::verify_write(uint32_t i_start, uint32_t i_length, const uint8_t* i_buffer)
{
    uint8_t verify_buffer[WRITE_PAGE_SIZE];
    FAPI_TRY(read_internal(i_start, i_length, false, verify_buffer));

    if (memcmp(i_buffer, verify_buffer, i_length) != 0)
    {
        FAPI_ERR("Write verify failed at address=0x%08x length=0x%08x", i_start, i_length);

        // Store the first fail's address and length
        if (!iv_verify_fail)
        {
            iv_verify_fail = true;
            iv_verify_fail_address = i_start;
            iv_verify_fail_length = i_length;
        }

        handle_verification_error();
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::verify_check() const
{
    FAPI_ASSERT(!iv_verify_fail,
                SPI_MEMORY_VERIFY_FAIL()
                .set_ADDRESS(iv_verify_fail_address)
                .set_LENGTH(iv_verify_fail_length),
                "Verify after write failed address=0x%08x length=0x%08x",
                iv_verify_fail_address, iv_verify_fail_length);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::SEEPROMDevice::flush_page_buf(bool i_final_write)
{
    // iv_write_pos points to the _next_ byte that will be written to the buffer, so
    // (iv_write_pos - 1) is the _last_ byte written to the buffer. We want to know that
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
    FAPI_TRY(write_page(page_start + offset, length, iv_page_buffer));
    FAPI_TRY(verify_check());

fapi_try_exit:
    return current_err;
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
        return flush_page_buf(false);
    }

    return FAPI2_RC_SUCCESS;
}

ReturnCode spi::SEEPROMDevice::write_data(const void* i_buffer, uint32_t i_length)
{
    auto buf8 = static_cast<const uint8_t*>(i_buffer);

    FAPI_INF("Memory write data iv_write_pos=0x%08x iv_write_end=0x%08x i_length=0x%08x",
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
    FAPI_INF("Memory write end iv_write_pos=0x%08x iv_write_end=0x%08x",
             iv_write_pos, iv_write_end);

    FAPI_TRY(check_write_active(WRITE_END, true));

    if ((iv_write_pos & WRITE_PAGE_MASK) != 0)
    {
        FAPI_TRY(flush_page_buf(true));
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

void spi::SEEPROMDevice::handle_verification_error()
{
    // No functional code
}

// The Flash driver expands upon the SEEPROM driver like so:
//
//  * The read mechanism is simply reused as-is.
//  * The write mechanism is expanded by overriding flush_page_buf() so that
//    it issues erase commands as needed before writing the next page.
//  * write_begin() takes care of saving off and writing back the beginning of
//    the first erase block and saving the tail end of the last erase block if
//    necessary. It uses the existing read() and write_data() mechanics for this.
//  * write_end() writes back the saved tail end of the last erase block.
//
// This way, it needs very little in the way of additional code.

ReturnCode spi::FlashDevice::write_begin(uint32_t i_start, uint32_t i_length)
{
    // Save the ECC mode now so we can restore it even on an error
    const ecc_mode saved_ecc_mode = iv_ecc_mode;

    // First defer to the superclass for basic initialization & ECC correction
    FAPI_TRY(SEEPROMDevice::write_begin(i_start, i_length));

    // Now, expand the total amount we're writing to cover entire erase blocks
    iv_recovered_from_error = false;
    iv_erase_start = iv_erase_pos = iv_write_start & ~SMALLEST_ERASE_BLOCK_MASK;
    iv_erase_end = (iv_write_end + SMALLEST_ERASE_BLOCK_MASK) & ~SMALLEST_ERASE_BLOCK_MASK;

    // Save off partial erase blocks if necessary
    {
        const uint32_t partial_start = iv_write_pos - iv_erase_pos;
        const uint32_t partial_end = iv_erase_end - iv_write_end;
        const uint32_t erase_length = iv_erase_end - iv_erase_pos;
        const uint32_t saved_erase_end = iv_erase_end;

        // If we write very little new data we may end up staying within a
        // single erase block. This is a special case since the first and last
        // erase block are identical.
        const bool single_erase_block = erase_length == SMALLEST_ERASE_BLOCK_SIZE;

        FAPI_DBG("iv_erase_start=0x%08x iv_erase_end=0x%08x partial_start=0x%08x partial_end=0x%08x",
                 iv_erase_start, iv_erase_end, partial_start, partial_end);

        FAPI_ASSERT(iv_erase_buffer || !(partial_start || partial_end),
                    SPI_FLASH_ERASE_BUFFER_NEEDED()
                    .set_ADDRESS(i_start)
                    .set_LENGTH(i_length),
                    "Write request requires an erase buffer but no buffer was provided: "
                    "i_start=0x%08x i_length=0x%08x", i_start, i_length);

        // Temporarily disable ECC while we move raw data around
        iv_ecc_mode = ECC_DISABLED;

        // Rewind the write pointer to the beginning of the erase area
        iv_write_pos = iv_erase_pos;

        // Since the write process always tries to use the largest fitting erase
        // block size, it's possible that the entire write area will be erased upon
        // the first write - including a potential partially written last block.
        // Since we haven't saved off that last page's data yet we have to make sure
        // this does not happen - so temporarily modify the erase length to exclude
        // the last block.
        if (partial_end && !single_erase_block)
        {
            iv_erase_end -= SMALLEST_ERASE_BLOCK_SIZE;
        }

        // Save off the first erase block and write back its beginning
        if (partial_start)
        {
            // If we only have a single erase block we need to save all of it
            // so we get both the beginning and the end before it's erased.
            // Otherwise we only need to save the beginning.
            const uint32_t read_size = (partial_end && single_erase_block) ?
                                       SMALLEST_ERASE_BLOCK_SIZE : partial_start;

            FAPI_TRY(read_internal(iv_erase_pos, read_size, false, iv_erase_buffer));
            iv_partial_end_restore_buffer = iv_erase_buffer + SMALLEST_ERASE_BLOCK_SIZE - partial_end;

            // write_data will implicitly do the erase
            FAPI_TRY(write_data(iv_erase_buffer, partial_start));
        }

        // Save off the end of the last erase block for later
        if (partial_end && !(partial_start && single_erase_block))
        {
            FAPI_TRY(read_internal(iv_write_end, partial_end, false, iv_erase_buffer));
            iv_partial_end_restore_buffer = iv_erase_buffer;
        }

        // Undo our earlier hacks
        iv_erase_end = saved_erase_end;
    }

fapi_try_exit:
    iv_ecc_mode = saved_ecc_mode;
    return current_err;
}

struct erase_mode_t
{
    uint32_t addr_mask;
    uint32_t block_size;
    uint32_t command;
};

// Erase modes sorted from large to small blocks
static const struct erase_mode_t ERASE_MODES[] =
{
    { 0xFFFF, 65536, X25_CMD_ERASE_64K },
    { 0x7FFF, 32768, X25_CMD_ERASE_32K },
    { 0x0FFF,  4096, X25_CMD_ERASE_4K  },
};

ReturnCode spi::FlashDevice::flush_page_buf(bool i_final_write)
{
    // If SEEPROMDevice::write_end() calls this to "finalize" we have to politely
    // refuse since we're going to add the saved tail end of the last erase block.
    if (i_final_write)
    {
        return FAPI2_RC_SUCCESS;
    }

    // iv_write_pos points to the _next_ byte that will be written to the buffer, so
    // (iv_write_pos - 1) is the _last_ byte written to the buffer. We want to know that
    // byte's page start address.
    const uint32_t page_start = (iv_write_pos - 1) & ~WRITE_PAGE_MASK;

    // Check whether we need to erase before we can write
    if (page_start >= iv_erase_pos)
    {
        // Go through available erase modes from largest to smallest block size
        // and pick the largest one that fits both the current erase address
        // and remaining erase size
        const uint32_t max_size = iv_erase_end - iv_erase_pos;

        // If we had a verify fail before, this is a good point to abort the write
        // since we completed the current erase block (and thus wrote back all the
        // saved data that would otherwise be lost) and have not yet erased the next.
        FAPI_TRY(verify_check());

        for (auto& mode : ERASE_MODES)
        {
            if ((iv_erase_pos & mode.addr_mask) == 0 && max_size >= mode.block_size)
            {
                FAPI_DBG("erase address=0x%08x length=0x%08x", iv_erase_pos, mode.block_size);

                FAPI_TRY(write_enable());
                FAPI_TRY(iv_port.transaction(mode.command | (iv_erase_pos & X25_ADDR_MASK),
                                             4, NULL, 0, NULL, 0));
                FAPI_TRY(wait_for_write_complete(ERASE_TIMEOUT_MS, OP_BLOCK_ERASE,
                                                 iv_erase_pos, mode.block_size));
                FAPI_TRY(check_extended_status(iv_erase_pos, mode.block_size));

                iv_erase_pos += mode.block_size;
                break;
            }
        }

        FAPI_ASSERT(iv_erase_pos > page_start,
                    SPI_FLASH_ERASE_INSUFFICIENT()
                    .set_PAGE_ADDRESS(page_start)
                    .set_ERASE_START(iv_erase_start)
                    .set_ERASE_POS(iv_erase_pos)
                    .set_ERASE_END(iv_erase_end),
                    "Did not erase enough blocks to continue - CODE BUG "
                    "page_start=0x%08x erase_start=0x%08x erase_pos=0x%08x erase_end=0x%08x",
                    page_start, iv_erase_start, iv_erase_pos, iv_erase_end);
    }

    // Now write the page buffer; since we don't do partial pages the address
    // and size calculations become a lot simpler than for SEEPROM.
    {
        FAPI_DBG("flush_page_buf iv_write_pos=0x%08x page_start=0x%08x",
                 iv_write_pos, page_start);

        FAPI_TRY(write_page(page_start, WRITE_PAGE_SIZE, iv_page_buffer));
        FAPI_TRY(check_extended_status(page_start, WRITE_PAGE_SIZE));
        iv_buf_pos = 0;
    }

fapi_try_exit:
    // Save off current_err since cleanup might modify it
    const ReturnCode l_final_rc = current_err;

    if (current_err != FAPI2_RC_SUCCESS)
    {
        // Recover a partial last block if needed;
        // we call this without FAPI_TRY since we're just trying to clean up
        finalize_write(false);
        iv_recovered_from_error = true;
    }

    return l_final_rc;
}

ReturnCode spi::FlashDevice::finalize_write(const bool i_write_end)
{
    const ecc_mode saved_ecc_mode = iv_ecc_mode;

    // This code might be called in the context of write_begin() where
    // we're lying about the real end of the erase area, so we have to
    // re-calculate the actual end of the erase area.
    const uint32_t l_real_erase_end = (iv_write_end + SMALLEST_ERASE_BLOCK_MASK) & ~SMALLEST_ERASE_BLOCK_MASK;
    const uint32_t partial_end = l_real_erase_end - iv_write_end;
    const bool last_block_erased = iv_erase_pos == l_real_erase_end;

    // In the good path, we always restore the partial end if we have one;
    // in the error case we only need to do this if the last block has been erased
    if (partial_end && (i_write_end || last_block_erased))
    {
        // Set up internal state to pretend we wrote data all
        // the way to the end and are ready to write the partial end
        // (in the good case this is already the case)
        iv_buf_pos = iv_write_end & WRITE_PAGE_MASK;
        iv_write_pos = iv_write_end;

        // We need to write the saved block with ECC off
        iv_ecc_mode = ECC_DISABLED;

        // Make sure we don't trip the write sequence check
        iv_write_active = true;

        // Write out saved erase block end
        FAPI_TRY(write_data(iv_partial_end_restore_buffer, partial_end));
    }

fapi_try_exit:
    iv_write_active = false;
    iv_ecc_mode = saved_ecc_mode;
    return current_err;
}

ReturnCode spi::FlashDevice::write_end()
{
    ReturnCode l_rc;

    // If we're no longer in write mode because we just had an error
    // and recovered from it, allow exactly one call to write_end()
    // without raising an error.
    if (iv_recovered_from_error)
    {
        iv_recovered_from_error = false;
        return FAPI2_RC_SUCCESS;
    }

    FAPI_TRY(check_write_active(WRITE_END, true));

    // Always restore the saved last block data even if write_end fails
    l_rc = SEEPROMDevice::write_end();
    FAPI_TRY(finalize_write(true));
    FAPI_TRY(l_rc);

    // One last check for verify fails
    FAPI_TRY(verify_check());

    FAPI_ASSERT(iv_erase_pos == iv_erase_end,
                SPI_MEMORY_ERASE_MISMATCH()
                .set_ECC_MODE(iv_ecc_mode)
                .set_START_ADDRESS(iv_erase_start)
                .set_END_ADDRESS(iv_erase_end)
                .set_FINAL_ERASE_POINTER(iv_erase_pos),
                "Erasing did not go as planned: ecc_mode=%d start_address=0x%08x "
                "end_address=0x%08x erase_pointer=0x%08x",
                iv_ecc_mode, iv_erase_start, iv_erase_end, iv_erase_pos);

fapi_try_exit:
    return current_err;
}

ReturnCode spi::FlashDevice::read_extended_status(uint32_t i_address, extended_status& o_status) const
{
    uint8_t dev_status;

    // Depending on the device type the extended status has a different
    // read command and bit layout.
    if (iv_devtype == DEV_MICRON_MT25Q)
    {
        FAPI_TRY(iv_port.transaction(MT25Q_CMD_READ_FLAG_STATUS_REG, 1, NULL, 0, &dev_status, 1));
        o_status = static_cast<extended_status>(
                       ((dev_status & 0x40) ? ES_ERASE_SUSP : 0) |
                       ((dev_status & 0x20) ? ES_ERASE_FAIL : 0) |
                       ((dev_status & 0x10) ? ES_PROG_FAIL : 0) |
                       ((dev_status & 0x04) ? ES_PROG_SUSP : 0) |
                       ((dev_status & 0x02) ? ES_WRITE_PROT : 0));

        // Clear status if any error bit was set
        if (o_status != ES_NONE)
        {
            FAPI_TRY(iv_port.transaction(MT25Q_CMD_CLR_FLAG_STATUS_REG, 1, NULL, 0, NULL, 0));
        }
    }
    else if (iv_devtype == DEV_MACRONIX_MX66)
    {
        FAPI_TRY(iv_port.transaction(MX66_CMD_READ_SECURITY_REG, 1, NULL, 0, &dev_status, 1));
        o_status = static_cast<extended_status>(
                       ((dev_status & 0x40) ? ES_ERASE_FAIL : 0) |
                       ((dev_status & 0x20) ? ES_PROG_FAIL : 0) |
                       ((dev_status & 0x08) ? ES_ERASE_SUSP : 0) |
                       ((dev_status & 0x04) ? ES_PROG_SUSP : 0));
    }

fapi_try_exit:
    return current_err;
}

ReturnCode spi::FlashDevice::check_extended_status(
    uint32_t i_address, uint32_t i_length) const
{
    extended_status status = ES_NONE;

    // Read the status
    FAPI_TRY(read_extended_status(i_address, status));

    // Now check for errors
    FAPI_ASSERT(!(status & ES_WRITE_PROT),
                SPI_FLASH_WRITE_PROTECTED()
                .set_ADDRESS(i_address)
                .set_LENGTH(i_length),
                "Flash is write protected at address=0x%08x length=0x%08x",
                i_address, i_length);

    FAPI_ASSERT(!(status & ES_PROG_FAIL),
                SPI_FLASH_WRITE_FAIL()
                .set_ADDRESS(i_address)
                .set_LENGTH(i_length),
                "Flash write failed at address=0x%08x length=0x%08x",
                i_address, i_length);

    FAPI_ASSERT(!(status & ES_ERASE_FAIL),
                SPI_FLASH_ERASE_FAIL()
                .set_ADDRESS(i_address)
                .set_LENGTH(i_length),
                "Flash erase failed at address=0x%08x length=0x%08x",
                i_address, i_length);

fapi_try_exit:
    return current_err;
}

// Special value that we don't want to expose to user code
static const auto DEV_UNKNOWN = static_cast<spi::FlashDevice::device_type>(0xFF);

/**
 * @brief Map a device ID read from the Flash chip to a supported device type
 */
static inline spi::FlashDevice::device_type get_mode_from_id(const uint8_t id[3])
{
    if (id[0] == 0xC2)
    {
        if (id[1] == 0x20)
        {
            FAPI_DBG("Detected Macronix MX66 type flash device");
            return spi::FlashDevice::DEV_MACRONIX_MX66;
        }
    }
    else if (id[0] == 0x20)
    {
        if (id[1] == 0xBA || id[1] == 0xBB)
        {
            FAPI_DBG("Detected Micron MT25Q type flash device");
            return spi::FlashDevice::DEV_MICRON_MT25Q;
        }
    }

    return DEV_UNKNOWN;
}

ReturnCode spi::FlashDevice::detect_device(const AbstractPort& i_port, device_type& o_devtype)
{
    union
    {
        uint8_t b[4];
        uint32_t i;
    } id = { 0 };

    FAPI_TRY(i_port.transaction(X25_CMD_READ_ID, 1, NULL, 0, id.b, 3));

    FAPI_ASSERT(id.i != 0,
                SPI_FLASH_NO_DEVICE(),
                "Got zero response to 'get device ID' command, looks like no device attached?");

    o_devtype = get_mode_from_id(id.b);

    FAPI_ASSERT(o_devtype != DEV_UNKNOWN,
                SPI_FLASH_UNKNOWN_DEVICE()
                .set_DEVICE_ID(be32toh(id.i)),
                "Unknown device, id=0x%08x", be32toh(id.i));

fapi_try_exit:
    return current_err;
}

ReturnCode spi::FlashDevice::soft_reset()
{
    FAPI_TRY(iv_port.transaction(X25_CMD_RESET1, 1, NULL, 0, NULL, 0));
    FAPI_TRY(iv_port.transaction(X25_CMD_RESET2, 1, NULL, 0, NULL, 0));

fapi_try_exit:
    return current_err;
}
