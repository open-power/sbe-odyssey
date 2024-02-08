/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_efuse_program.H $ */
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
#pragma once
#include <fapi2.H>

/**
 * @brief Programing efuse bits
 *
 * Takes a databuffer and programs efuse bits with it.
 * Data is given byte-wise, while the range is given bit-wise.
 * Ecc is calculated automatically when enabled and written to the
 * corresponding efuse bits written.
 *
 * @param[in]    i_target        The chiplet or core containing the efuses.
 * @param[in]    i_buffer        Buffer with data to be written to the fuses.
 * @param[in]    i_startBit      First bit to be written.
 * @param[in]    i_endBit          Last bit to be written.
 *
 * @return the usual FAPI2 return codes
 */
DECLARE_HWP(poz_efuse_program,
            const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
            const uint8_t* i_buffer, uint32_t i_startBit, uint32_t i_endBit);
