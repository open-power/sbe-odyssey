/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/exceptionhandler.C $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "exceptionhandler.H"
#include "sbeexceptionhandler.H"

/**
 * @brief SBE handler to save off plat specific registers
 *
 */
extern "C" void __plat_sbe_register_saveoff()
{
    asm(

    "# Read LFR0 into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x2040(%r6)\n"
    "stw %r4, __g_register_ffdc+44@sda21(0)\n"
    "stw %r5, __g_register_ffdc+48@sda21(0)\n"

    "# Read LFR1 into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x2060(%r6)\n"
    "stw %r4, __g_register_ffdc+52@sda21(0)\n"
    "stw %r5, __g_register_ffdc+56@sda21(0)\n"

    "# Read OTP Status Register into r4 and r5\n"
    "lis %r6, 0x1\n"
    "lvd %d4, 0x2(%r6)\n"
    "stw %r4, __g_register_ffdc+60@sda21(0)\n"
    "stw %r5, __g_register_ffdc+64@sda21(0)\n"

    "# Read Spi0 Status Register into r4 and r5\n"
    "lis %r6, 0x7\n"
    "lvd %d4, 0x8(%r6)\n"
    "stw %r4, __g_register_ffdc+68@sda21(0)\n"
    "stw %r5, __g_register_ffdc+72@sda21(0)\n"

    "# Read Spi0 Config Register into r4 and r5\n"
    "lvd %d4, 0x2(%r6)\n"
    "stw %r4, __g_register_ffdc+76@sda21(0)\n"
    "stw %r5, __g_register_ffdc+80@sda21(0)\n"

    "# Read Spi0 Clock Config Register into r4 and r5\n"
    "lvd %d4, 0x3(%r6)\n"
    "stw %r4, __g_register_ffdc+84@sda21(0)\n"
    "stw %r5, __g_register_ffdc+88@sda21(0)\n"
    );
}