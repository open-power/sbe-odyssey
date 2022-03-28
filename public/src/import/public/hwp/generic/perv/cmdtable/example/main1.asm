/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/cmdtable/example/main1.asm $ */
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
/*
 * ROM FSM command table assembler example
 * main1.asm - first part of the main table
 *
 * This and the second part in main2.asm will be combined into the main binary
 * that goes into the SEEPROM image and is identical across all chips.
 *
 * As you can see, C-style comments are allowed.
 */

// You can define labels as needed, though this one will not be used
start:
        /*
         * Our first putscom! Putscoms always have an address, mask and data.
         *
         * A 1 bit in the mask will cause the bit from data to replace the
         * register bit, a 0 in the mask will preserve the register bit.
         *
         * If the mask is all-F, only a putscom will be performed, otherwise
         * a read-modify-write sequence. -1 is a shortcut for all-F.
         */
        putscom     0x50009, -1, 0x0000800300000000

        /*
         * The same but with a left-aligned hex value. These can be used for
         * mask and data to save typing.
         */
        putscom     0x50009, -1, 0xl00008003

        /*
         * If a single line would become too unwieldy, you can insert
         * line breaks after commas:
         */
        putscom     0x50009,
                    0xFFFFFFFFFFFFFFFF,
                    0x0000800300000000

        /*
         * Now for some more symbolic programming, let's define some named constants:
         */

#define SB_MSG 0x50009
#define ALL -1

        /*
         * You can use more complex macros too, but be careful, the expression parser
         * in the assembler is based on Python and may not understand all C-isms.
         */
#define SB_MSG_VALUE(running, boot_phase) (((0x8000 * running) | boot_phase) << 32)

        putscom  SB_MSG, ALL, SB_MSG_VALUE(1, 3)

        /*
         * Macros are cool, so let's include a few more.
         */

#include "defines.inc"

        /*
         * And now let's do some real work and scan in the PIB repairs.
         */

        putscom  SCAN_REGION_TYPE, ALL, REGION_PIB | SCANTYPE_REPR
        putscom  SCAN64(0), ALL, SCAN_HEADER

        /*
         * The actual repair values are custom per chip so we defer them to
         * the customized command table. The CALL instruction jumps into the
         * customized table at a given offset.
         * Once the customized table RETURNs the main table will continue at
         * the instruction following the CALL.
         */

        call 0      // Jump to offset 0

        /*
         * Now check the header and clean up
         * The TEST instruction compares the register read value to the instruction data
         * after applying the mask. Only bits where the mask is 1 will be compared.
         * If the comparison fails the FSM will set SB_MSG to the fourth argument and halt.
         */
        test     SCAN64(0), ALL, SCAN_HEADER, ERR_HEADER_CHECK
        putscom  SCAN_REGION_TYPE, ALL, 0

        /*
         * We're done here but execution will continue in main2.asm
         */
