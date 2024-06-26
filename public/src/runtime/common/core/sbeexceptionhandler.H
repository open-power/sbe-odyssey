/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeexceptionhandler.H $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2024                        */
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

#ifdef PLAT_REGISTER_SAVEOFF
    #include "exceptionhandler.H"
#endif

/**
 * @brief Register save off structure header
 */
struct registersaveoffhdr_t
{
    // Structure Versioning, won't change unless,there is change in this structure
    uint32_t  version:16;

    // Magic byte for address validation (0xA5) in SPRG0
    uint32_t  magicbyte:8;

    //Valid Byte, if the Register below could be saved off
    uint32_t  validbyte:8;

    // Default Constructor to initialize
    registersaveoffhdr_t()
    {
        version = 0;
        magicbyte = 0;
        validbyte = 0;
    }
};

/**
 * @brief Common registers to be collected across all platforms
 */
struct cmnregistersave_t
{
    // Common Registers to be saved off
    uint32_t SRR0;
    uint32_t SRR1;
    uint32_t ISR;
    uint32_t LR;
    uint32_t EIMR_upper32bits;
    uint32_t EIMR_lower32bits;
    uint32_t EISR_upper32bits;
    uint32_t EISR_lower32bits;
    uint32_t EISTR_upper32bits;
    uint32_t EISTR_lower32bits;

    // Default Constructor to initialize
    cmnregistersave_t()
    {
        SRR0 = 0;
        SRR1 = 0;
        ISR = 0;
        LR = 0;
        EIMR_upper32bits = 0;
        EIMR_lower32bits = 0;
        EISR_upper32bits = 0;
        EISR_lower32bits = 0;
        EISTR_upper32bits = 0;
        EISTR_lower32bits = 0;
    }
};

/**
 * @brief Structure for registers to be saved off
 */
struct registersave_t
{
    // Register save off header
    registersaveoffhdr_t registersaveoffhdr;

    // Common Registers to be saved off
    cmnregistersave_t cmnregistersave;

    #ifdef PLAT_REGISTER_SAVEOFF
        // Plat Registers to be saved off
        platregistersave_t platregistersave;
    #endif

};
