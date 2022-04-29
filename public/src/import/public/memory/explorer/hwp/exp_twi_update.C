/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/exp_twi_update.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
/// @file exp_fw_update.C
/// @brief Procedure definition to update explorer firmware
///
// *HWP HWP Owner: Glenn Miles <milesg@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: FSP:HB

#include <fapi2.H>
#include <exp_twi_update.H>
#include <lib/inband/exp_inband.H>
#include <i2c_access.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/pos.H>
#include <lib/i2c/exp_i2c.H>


namespace mss
{
namespace exp
{
namespace twi
{

#define TWI_BYTES_TO_WORDS(_BYTES) ((_BYTES + (MIN_WRITE_BYTES-1))/MIN_WRITE_BYTES)
#define TWI_ALIGN4(_BYTES) ((_BYTES + 3) & ~3)

///
/// @brief Maximum number of bytes to write at a time
///
constexpr uint32_t MAX_WRITE_BYTES = 8;

///
/// @brief minimum number of bytes to write at a time
///
constexpr uint32_t MIN_WRITE_BYTES = 4;

///
/// @brief Maximum allowed image size
///
constexpr uint32_t MAX_IMG_BYTES = (256 * 1024);

///
/// @brief Check that target status is as expected
///
/// @note This is the only TWI operation that reads data from the target
///
/// @param[in] i_target Which OCMB chip to target
/// @param[in] i_status_buf The status returned over i2c
/// @param[in] i_expected_status The expected status
/// @param[in] i_expected_offset The expected 32-bit word write offset
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode check_status(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const std::vector<uint8_t>& i_status_buf,
    const uint8_t i_expected_status,
    const uint16_t i_expected_offset)
{
    status_word_t l_status;
    uint32_t l_status_word = 0;

    // Parse status
    l_status.status_id = i_status_buf[0];
    l_status.status    = i_status_buf[1];
    l_status.write_offset = (i_status_buf[3] << 8) | i_status_buf[2];
    l_status_word = (i_status_buf[0] << 24) |
                    (i_status_buf[1] << 16) |
                    (i_status_buf[2] << 8)  |
                    i_status_buf[3];
    // Check that status ID is correct
    FAPI_ASSERT((l_status.status_id == STATUS_ID),
                fapi2::EXP_TWI_INVALID_STATUS_ID().
                set_TARGET(i_target).
                set_STATUS_ID(l_status.status_id).
                set_STATUS_WORD32(0),
                "twi::check_status(%s): Invalid status ID[0x%02x] in status "
                "word[0x%08x]. Expected[0x%02x]",
                c_str(i_target), l_status.status_id,
                l_status_word, STATUS_ID);

    // Check that status is what caller expected
    FAPI_ASSERT((l_status.status == i_expected_status),
                fapi2::EXP_TWI_UNEXPECTED_STATUS().
                set_TARGET(i_target).
                set_EXPECTED_STATUS(i_expected_status).
                set_STATUS(l_status.status).
                set_STATUS_WORD32(0),
                "twi::check_status(%s): Invalid status[0x%02x] in status "
                "word[0x%08x]. Expected[0x%02x]",
                c_str(i_target), l_status.status,
                l_status_word, i_expected_status);

    // Check that write offset is what caller expected
    FAPI_ASSERT((l_status.write_offset == i_expected_offset),
                fapi2::EXP_TWI_UNEXPECTED_WRITE_OFFSET().
                set_TARGET(i_target).
                set_EXPECTED_OFFSET(i_expected_offset).
                set_WRITE_OFFSET(l_status.write_offset).
                set_STATUS_WORD32(0),
                "twi::check_status(%s): Invalid offset[0x%04x] in status "
                "word[0x%08x]. Expected[0x%04x]",
                c_str(i_target), l_status.write_offset,
                l_status_word, i_expected_offset);

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Places explorer into the IDLE state and sets the write offset to 0
///
/// @param[in] i_target Which OCMB chip to target
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode send_reset(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    std::vector<uint8_t> l_no_command;
    std::vector<uint8_t> l_status_buf;
    std::vector<uint8_t> l_cmd;
    l_cmd.push_back(CMD_RESET);

    // Send the command over i2c
    FAPI_TRY(fapi2::putI2c(i_target, l_cmd),
             "twi::send_reset: i2c failure sending reset command to %s",
             c_str(i_target));

    // Check that target reached IDLE state and write offset was set to 0
    FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                           l_no_command, l_status_buf));
    FAPI_TRY(check_status(i_target, l_status_buf, STATUS_IDLE, 0),
             "twi::send_reset: Failed entering idle state on %s",
             c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Places explorer into the WRITE state
///
/// @note Target must be in the IDLE state before sending WRITE command
///
/// @param[in] i_target Which OCMB chip to target
/// @param[in] i_length The total number of 32-bit words that will be written by
//                      subsequent calls to send_data
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode send_write(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint16_t i_length)
{
    std::vector<uint8_t> l_no_command;
    std::vector<uint8_t> l_status_buf;
    std::vector<uint8_t> l_cmd;
    l_cmd.push_back(CMD_IMGWRITE);

    // Append the length to the end (byte swapping if necessary)
    FAPI_TRY(ib::forceCrctEndian(i_length, l_cmd));

    // Send the command over i2c
    FAPI_TRY(fapi2::putI2c(i_target, l_cmd),
             "twi::send_write: i2c failure sending write command to %s",
             c_str(i_target));

    // Check that target reached WRITE state and write offset is still 0
    FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                           l_no_command, l_status_buf));
    FAPI_TRY(check_status(i_target, l_status_buf, STATUS_WRITE, 0),
             "twi::send_write: Failed entering write state on %s",
             c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Writes image data to the target
///
/// @note Target must be in the WRITE state before sending data
///
/// @param[in] i_target Which OCMB chip to target
/// @param[in] i_offset 32-bit word offset (bytes / 4) to start writing the data
/// @param[in] i_data The data to be written to the target
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode send_data(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint16_t i_offset,
    const std::vector<uint8_t>& i_data)
{
    // Note, we only check the i2c response every 256th transfer to reduce run time
    // (256 transfers * 8B per transfer = offset to check)
    constexpr uint16_t OFFSET_TO_CHECK_STATUS = 256 * 8;

    // This is actually a continuation of the write command.  No command
    // byte is needed here.

    // write the image data over i2c
    FAPI_TRY(fapi2::putI2c(i_target, i_data),
             "twi::send_data: i2c failure writing image data to %s",
             c_str(i_target));

    // Check that target is still in the WRITE state and write offset was
    // increased to the correct amount of 32-bit words.
    // Note, only checking every 256th transfer (2K offset / 8B per transfer)
    // to reduce run time
    if ((i_offset % OFFSET_TO_CHECK_STATUS) == 0)
    {
        std::vector<uint8_t> l_no_command;
        std::vector<uint8_t> l_status_buf;

        FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                               l_no_command, l_status_buf));
        FAPI_TRY(check_status(
                     i_target, l_status_buf, STATUS_WRITE,
                     i_offset + (i_data.size() / (sizeof(uint32_t)))),
                 "twi::send_data: Failed sending %u bytes"
                 " to %s with starting word offset 0x%08x",
                 i_data.size(), c_str(i_target), i_offset);
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Request OCMB target to verify data that has been sent
///
/// @note All data must be sent before sending verify command
///
/// @param[in] i_target Which OCMB chip to target
/// @param[in] i_offset Expected 32-bit word offset
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode send_verify(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
    const uint16_t i_offset)
{
    std::vector<uint8_t> l_no_command;
    std::vector<uint8_t> l_status_buf;
    std::vector<uint8_t> l_cmd;
    l_cmd.push_back(CMD_VERIFY);

    // Send the command over i2c
    FAPI_TRY(fapi2::putI2c(i_target, l_cmd),
             "twi::send_verify: i2c failure sending verify command to %s",
             c_str(i_target));

    // Check that target reached VERIFY_SUCCESS state and write offset
    // is equal to the number of 32-bit words that were sent.
    FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                           l_no_command, l_status_buf));
    FAPI_TRY(check_status(i_target, l_status_buf, STATUS_VERIFY_SUCCESS, i_offset),
             "twi::send_verify: Failed to verify image on %s",
             c_str(i_target));

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Request OCMB target to start executing new image
///
/// @note Target must be in the VERIFIED state before sending the EXEC command
///
/// @param[in] i_target Which OCMB chip to target
/// @param[in] i_offset Offset to start writing the data
/// @param[in] i_data The data to be written to the target
/// @return FAPI2_RC_SUCCESS iff successful
///
fapi2::ReturnCode send_exec(
    const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
{
    std::vector<uint8_t> l_cmd;
    l_cmd.push_back(CMD_EXEC);

    // Send the command over i2c
    FAPI_TRY(fapi2::putI2c(i_target, l_cmd),
             "twi::send_exec: i2c failure sending verify command to %s",
             c_str(i_target));

    // NOTE: Not possible to check status after sending this command
    //       since it likely is not running TWI code anymore.

fapi_try_exit:
    return fapi2::current_err;
}

extern "C"
{
    ///
    /// @brief Updates explorer firmware using low-level TWI interfaces
    ///
    /// The caller must place the explorer chip in TWI mode before this function
    /// is called.  This is done by calling the exp_twi_enable function.
    ///
    /// @param[in] i_target the controller
    /// @param[in] i_image_ptr pointer to the binary image
    /// @param[in] i_image_sz size of the binary image
    /// @return FAPI2_RC_SUCCESS if ok
    ///
    fapi2::ReturnCode exp_twi_update(
        const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
        const uint8_t* i_image_ptr, const size_t i_image_sz)
    {
        std::vector<uint8_t> l_no_command;
        std::vector<uint8_t> l_status_buf;
        std::vector<uint8_t> l_buffer;
        // TWI writes must be multiples of 4 (this rounds up)
        const size_t l_padded_sz = TWI_ALIGN4(i_image_sz);

        FAPI_INF("Entering exp_twi_update(%s). image_sz[0x%08x]",
                 c_str(i_target), i_image_sz);

        // Verify size of image is within max size
        FAPI_ASSERT((i_image_sz <= MAX_IMG_BYTES),
                    fapi2::EXP_TWI_INVALID_IMAGE_SIZE().
                    set_TARGET(i_target).
                    set_MAX_SIZE(MAX_IMG_BYTES).
                    set_ACTUAL_SIZE(i_image_sz),
                    "exp_twi_update: image size[%u] must be less than %u bytes!",
                    i_image_sz, MAX_IMG_BYTES);

        // Reset TWI state machine
        FAPI_TRY(send_reset(i_target));

        // Enter Write state
        FAPI_TRY(send_write(i_target, l_padded_sz / sizeof(uint32_t)));

        FAPI_INF("exp_twi_update(%s): writing image data...", c_str(i_target));

        // Write data in MAX_WRITE_BYTES byte blocks (or smaller)
        //
        // NOTE: MAX_WRITE_BYTES byte writes are not required by TWI spec, but
        //       using this limit to reduce stack foot print.
        l_buffer.reserve(MAX_WRITE_BYTES);

        for(uint32_t l_byte_offset = 0;
            l_byte_offset < i_image_sz;
            l_byte_offset += MAX_WRITE_BYTES)
        {
            const uint8_t* l_start_ptr = &i_image_ptr[l_byte_offset];
            uint32_t l_copy_sz = std::min(MAX_WRITE_BYTES, static_cast<uint32_t>(i_image_sz - l_byte_offset));

            // size rounded up to 4 byte boundary
            uint32_t l_align_sz = TWI_ALIGN4(l_copy_sz);

            // copy data into vector for sending over i2c
            l_buffer.assign(l_start_ptr, l_start_ptr + l_copy_sz);
            FAPI_DBG("exp_twi_update(%s): writing %d bytes (offset %d / total %d bytes)",
                     c_str(i_target), l_copy_sz, l_byte_offset, i_image_sz);

            // pad to 4 byte boundary
            l_buffer.resize(l_align_sz, 0xFF);

            // write up to MAX_WRITE_BYTES of data
            FAPI_TRY(send_data(i_target,
                               l_byte_offset / sizeof(uint32_t),
                               l_buffer));
        }

        FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                               l_no_command, l_status_buf));
        FAPI_TRY(check_status(
                     i_target, l_status_buf, STATUS_WRITE,
                     l_padded_sz / sizeof(uint32_t)),
                 "twi::send_data: Failed sending %u bytes",
                 i_image_sz);

        FAPI_INF("exp_twi_update(%s): Finished writing image data.",
                 c_str(i_target));

        // Verify image
        FAPI_TRY(send_verify(i_target, l_padded_sz / sizeof(uint32_t)));

        FAPI_INF("exp_twi_update(%s): Executing flashed image.",
                 c_str(i_target));

        // Execute image
        FAPI_TRY(send_exec(i_target));

    fapi_try_exit:
        FAPI_INF("Exiting exp_twi_update(%s) with return code : 0x%08x...",
                 c_str(i_target), (uint64_t) fapi2::current_err);
        return fapi2::current_err;
    }

    ///
    /// @brief Places explorer chip into TWI mode
    ///
    /// Explorer must already be in the Boot ROM stage or FW upgrade mode before
    /// calling this function.  Explorer will be in the Boot ROM stage for 4
    /// seconds after reset.  It will enter FW upgrade mode if no valid images
    /// are found on its flash after the 4 seconds have expired.
    ///
    /// @param[in] i_target the controller
    /// @return FAPI2_RC_SUCCESS if ok
    ///
    fapi2::ReturnCode exp_twi_enable(
        const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target)
    {
        std::vector<uint8_t> l_no_command;
        std::vector<uint8_t> l_status_buf;

        FAPI_INF("Entering exp_twi_enable(%s)", c_str(i_target));

        // Check for correct boot stage and send FW_DOWNLOAD command if
        // possible
        FAPI_TRY(i2c::fw_download(i_target));

        // Check that target reached IDLE state and write offset was set to 0
        FAPI_TRY(fapi2::getI2c(i_target, sizeof(status_word_t),
                               l_no_command, l_status_buf));
        FAPI_TRY(check_status(i_target, l_status_buf, STATUS_IDLE, 0),
                 "exp_twi_enable: Failed enabling TWI mode on %s",
                 c_str(i_target));

    fapi_try_exit:
        FAPI_INF("Exiting exp_twi_enable(%s) with return code : 0x%08x...",
                 c_str(i_target), (uint64_t) fapi2::current_err);
        return fapi2::current_err;
    }
}

}//ns twi
}//ns exp
}//ns mss
