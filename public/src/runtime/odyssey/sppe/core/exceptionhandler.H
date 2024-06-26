/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/exceptionhandler.H $     */
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

#include "stdint.h"

/**
 * @brief Ody platform Register Save off Structure
 */
struct platregistersave_t
{
    // Plat specific Registers to be saved off
    uint32_t LFR0_upper32bits;
    uint32_t LFR0_lower32bits;
    uint32_t LFR1_upper32bits;
    uint32_t LFR1_lower32bits;
    uint32_t OTP_status_upper32bits;
    uint32_t OTP_status_lower32bits;
    uint32_t SPI0_status_upper32bits;
    uint32_t SPI0_status_lower32bits;
    uint32_t SPI0_config_upper32bits;
    uint32_t SPI0_config_lower32bits;
    uint32_t SPI0_clock_config_upper32bits;
    uint32_t SPI0_clock_config_lower32bits;

    // Default Constructor to initialize
    platregistersave_t()
    {
        LFR0_upper32bits = 0;
        LFR0_lower32bits = 0;
        LFR1_upper32bits = 0;
        LFR1_lower32bits = 0;
        OTP_status_upper32bits = 0;
        OTP_status_lower32bits = 0;
        SPI0_status_upper32bits = 0;
        SPI0_status_lower32bits = 0;
        SPI0_config_upper32bits = 0;
        SPI0_config_lower32bits = 0;
        SPI0_clock_config_upper32bits = 0;
        SPI0_clock_config_lower32bits = 0;
    }
};