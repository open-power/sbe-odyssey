/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_ppe_load.C $    */
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
/// @file poz_ppe_load.C
/// @brief Load PPE images in to SPPE/PME RAM
///
/// *HWP HW Maintainer: Daniel Kiss <kiss@de.ibm.com>
/// *HWP FW Maintainer:
/// *HWP Consumed by: SPPE
///

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <poz_ppe_load.H>
#include <fapi2_subroutine_executor.H>
#include <poz_common_image_load.H>

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

enum PPERegisters
{
    PPE_XIXCR    = 0x0,
    PPE_XIRAMRA  = 0x1,
    PPE_XIRAMGA  = 0x2,
    PPE_XIRAMDBG = 0x3,
    PPE_XIRAMEDR = 0x4,
    PPE_XIDBGPRO = 0x5,
    PPE_XISIB    = 0x6,
    PPE_XIMEM    = 0x7,
    PPE_XISGB    = 0x8,
    PPE_XIICAC   = 0x9,
    PPE_XIDBGINF = 0xF,
};

enum PPEXCROperations
{
    PPE_CLEAR      = 0ULL << 60,
    PPE_HALT       = 1ULL << 60,
    PPE_RESUME     = 2ULL << 60,
    PPE_STEP       = 3ULL << 60,
    PPE_TOGGLE_TRH = 4ULL << 60,
    PPE_SOFT_RESET = 5ULL << 60,
    PPE_HARD_RESET = 6ULL << 60,
    PPE_FORCE_HALT = 7ULL << 60,
};

enum PIBMEMRegisters
{
    PIBMEM_CTRL     = 0x0,
    PIBMEM_ADDR     = 0x1,
    PIBMEM_DATA     = 0x2,
    PIBMEM_DATA_INC = 0x3,
    PIBMEM_DATA_DEC = 0x4,
    PIBMEM_STATUS   = 0x5,
    PIBMEM_RESET    = 0x6,
    PIBMEM_FIR_MASK = 0x8,
    PIBMEM_ECC_CAPT = 0xB,
};

enum IOPPEMEMRegisters
{
    IOPPE_CSCR      = 0xA,
    IOPPE_CSCR_WCLR = 0xB,
    IOPPE_CSCR_WOR  = 0xC,
    IOPPE_CSAR      = 0xD,
    IOPPE_CSDR      = 0xE,

    IOPPE_CSCR_AUTOINC = 0x8000000000000000ULL,
};

fapi2::ReturnCode poz_ppe_load(
    const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint8_t* i_img_data,
    const uint32_t i_img_size,
    const uint32_t i_offset,
    poz_image_type i_type)
{
    const uint32_t PPE_BASE_ADDRESS = (i_type == poz_image_type::SPPE_IMAGE) ? 0xD0000 :
                                      (i_type == poz_image_type::SBE_IMAGE ) ? 0xE0000 :
                                      (i_type == poz_image_type::PME_IMAGE ) ? 0xC0000 :
                                      0xC0000;
    const uint32_t RAM_OFFSET = 0x10;
    uint64_t* ptr = (uint64_t*)i_img_data;
    const uint32_t nwords = (i_img_size + 7) / 8;

    if (i_offset == 0)
    {
        poz_ppe_stop(i_target, PPE_BASE_ADDRESS);
    }

    FAPI_TRY(fapi2::putScom(i_target, PPE_BASE_ADDRESS + RAM_OFFSET + PIBMEM_CTRL, 0));
    FAPI_TRY(fapi2::putScom(i_target, PPE_BASE_ADDRESS + RAM_OFFSET + PIBMEM_ADDR, i_offset / sizeof(uint64_t)));

    for (uint32_t i = 0; i < nwords; i++)
    {
        FAPI_TRY(fapi2::putScom(i_target, PPE_BASE_ADDRESS + RAM_OFFSET + PIBMEM_DATA_INC, *ptr++));
    }

    FAPI_INF("%d bytes written", nwords * 8);

    if (i_img_data == (uint8_t*)NULL && i_img_size == 0)
    {
        poz_ppe_start(i_target, PPE_BASE_ADDRESS);
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

fapi2::ReturnCode poz_ioppe_load(
    const fapi2::Target < fapi2::TARGET_TYPE_PERV | fapi2::TARGET_TYPE_MULTICAST > & i_target,
    const uint8_t* i_img_data,
    const uint32_t i_img_size,
    const uint32_t i_offset,
    poz_image_type i_type,
    const uint32_t i_ppe_base_address,
    const uint32_t i_ioppe_ram_offset,
    const uint32_t i_ioppe_ram_repeat_size,
    const uint32_t i_ioppe_ram_repeat_num)
{
    const int32_t RAM_OFFSET = -16;
    uint64_t* ptr = (uint64_t*)i_img_data;
    const uint32_t nwords = (i_img_size + 7) / 8;

    if (i_offset == 0)
    {
        poz_ppe_stop(i_target, i_ppe_base_address);
    }

    // Enable auto increment
    FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + RAM_OFFSET + IOPPE_CSCR_WOR, IOPPE_CSCR_AUTOINC));

    for (uint32_t thread = 0; thread < i_ioppe_ram_repeat_num; thread++)
    {
        // Load start address
        FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + RAM_OFFSET + IOPPE_CSAR,
                                ((uint64_t)i_ioppe_ram_offset + (i_ioppe_ram_repeat_size * thread)) << 32));

        for (uint32_t i = 0; i < nwords; i++)
        {
            FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + RAM_OFFSET + IOPPE_CSDR, ptr[i]));
        }
    }

    FAPI_INF("%d bytes written", nwords * 8);

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


fapi2::ReturnCode poz_ppe_stop(
    const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
    const uint32_t i_ppe_base_address)
{
    FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + PPE_XIXCR, PPE_FORCE_HALT));
    FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + PPE_XIXCR, PPE_HARD_RESET));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


fapi2::ReturnCode poz_ppe_start(
    const fapi2::Target<fapi2::TARGET_TYPE_ALL_MC>& i_target,
    const uint32_t i_ppe_base_address)
{
    FAPI_TRY(fapi2::putScom(i_target, i_ppe_base_address + PPE_XIXCR, PPE_RESUME));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
