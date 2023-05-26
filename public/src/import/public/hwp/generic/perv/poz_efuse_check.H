/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_efuse_check.H $ */
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
#pragma once
#include <fapi2.H>


typedef fapi2::ReturnCode (*poz_efuse_check_FP_t)(
    const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target_chip,
    const uint8_t* i_buffer, uint32_t i_startBit, uint32_t i_endBit);


extern "C"
{
    /**
     * @brief Checking efuse bits
     *
     * Takes a buffer of data written to the efuses.
     * Checks if the expected data was written and if ECC is correct.
     *
     * @param[in]    i_target        The chiplet or core containing the efuses.
     * @param[in]    i_buffer        Buffer with data written to the fuses.
     * @param[in]    i_startBit      First bit written.
     * @param[in]    i_endBit        Last bit written.
     *
     * @return the usual FAPI2 return codes
     */
    fapi2::ReturnCode poz_efuse_check(const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>& i_target,
                                      const uint8_t* i_buffer, uint32_t i_startBit, uint32_t i_endBit);
}
