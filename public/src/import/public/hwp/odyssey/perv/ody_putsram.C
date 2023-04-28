/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_putsram.C $     */
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
///
/// @file ody_putsram.C
/// @brief Write data to SRAM
/// *HWP HW Maintainer: Thi Tran <thi@us.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: HB, Cronus, SBE
///

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <ody_putsram.H>
#include <poz_writesram.H>
#include <ody_scom_omi_ioo.H>

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------
// OCMB
SCOMT_OMI_USE_PHY_PPE_WRAP0_ARB_CSAR

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------
fapi2::ReturnCode ody_putsram(const fapi2::Target <fapi2::TARGET_TYPE_OCMB_CHIP>& i_target,
                              const uint64_t i_offset,
                              const uint32_t i_bytes,
                              const uint8_t* i_data)
{
    FAPI_DBG("Start");
    using namespace scomt::omi;
    PHY_PPE_WRAP0_ARB_CSAR_t  WRAP0_ARB_CSAR;

    FAPI_DBG("i_offset [0x%.8X%.8X], i_bytes %u.",
             ((i_offset >> 32) & 0xFFFFFFFF), (i_offset & 0xFFFFFFFF), i_bytes);

    // Set SRAM address
    WRAP0_ARB_CSAR = i_offset;
    FAPI_TRY(WRAP0_ARB_CSAR.putScom(i_target),
             "Error putscom to WRAP0_ARB_CSAR (SRAM address).");

    // Write SRAM
    FAPI_TRY(poz_writesram(i_target, PHY_PPE_WRAP0_ARB_CSCR_RW, PHY_PPE_WRAP0_ARB_CSDR, i_bytes, i_data),
             "Error from poz_writesram (Odyssey).");

    FAPI_DBG("poz_writesram completes.");

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
