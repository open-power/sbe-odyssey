/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/exp_twi_update.H $ */
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
/// @file exp_twi_update.H
/// @brief Procedure declaration to update explorer firmware over TWI
///
// *HWP HWP Owner: Glenn Miles <milesg@ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: FSP:HB

#ifndef __MSS_EXP_TWI_UPDATE__
#define __MSS_EXP_TWI_UPDATE__

#include <fapi2.H>

// Required for Cronus
typedef fapi2::ReturnCode (*exp_twi_update_FP_t) (const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>&,
        const uint8_t*, const size_t);

extern "C"
{

///
/// @brief Updates explorer firmware using low-level TWI interface
///
/// The caller must place the explorer chip in TWI mode before this function
/// is called.  This is done by calling the exp_twi_enable function.
///
/// @param[in] i_target the controller
/// @param[in] i_image_ptr pointer to the binary image
/// @param[in] i_image_sz size of the binary image
/// @return FAPI2_RC_SUCCESS iff ok
///
    fapi2::ReturnCode exp_twi_update(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                                     const uint8_t* i_image_ptr, const size_t i_image_sz);

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
    fapi2::ReturnCode exp_twi_enable(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_target);

}// extern C

namespace mss
{
namespace exp
{
namespace twi
{

///
/// @brief Defines the TWI commands available
///
typedef enum cmd_id
{
    CMD_IMGWRITE          = 0xA1,
    CMD_VERIFY            = 0xA2,
    CMD_EXEC              = 0xA3,
    CMD_RESET             = 0xFF
} cmd_id_t;

///
/// @brief Constants used in the status word
///
typedef enum status_consts
{
    STATUS_ID                   = 0x3D,
    STATUS_IDLE                 = 0xB1,
    STATUS_WRITE                = 0xB2,
    STATUS_VERIFY_SUCCESS       = 0xB3,
    STATUS_HARDWARE_ERROR       = 0xFB,
    STATUS_PROTOCOL_ERROR       = 0xFC,
    STATUS_AUTHENTICATION_ERROR = 0xFD,
    STATUS_TIMEOUT              = 0xFE
} status_consts_t;

///
/// @brief structure of data returned for status read
///
typedef struct status_word
{
    uint8_t status_id;
    uint8_t status;
    uint16_t write_offset;
} status_word_t;

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
    const uint16_t i_expected_offset);

}//ns twi
}//ns exp
}//ns mss


#endif
