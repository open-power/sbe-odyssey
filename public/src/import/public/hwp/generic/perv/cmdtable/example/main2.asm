/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/cmdtable/example/main2.asm $ */
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
 * main2.asm - second part of the main table
 *
 * This and the first part in main1.asm will be combined into the main binary
 * that goes into the SEEPROM image and is identical across all chips.
 */
#include "defines.inc"

        /*
         * This file is responsible for starting the PIB clocks, but with a twist:
         * If SCRATCH_REG_1 bits 0:3 are equal to 0xA, it will skip starting the PIB clocks.
         */
        /*
         * CMPBEQ means CoMPare and Branch if EQual.
         * It compares the register value to the data under mask. If the value matches
         * execution continues at the label in the fourth parameter, otherwise it continues
         * with the next instruction.
         *
         * There is also a CMPBNE for CoMPare and Branch if Not Equal.
         */
        cmpbeq   SCRATCH_REG_1, 0xlF, 0xl3, skip

        /*
         * If we got here we can start the clocks
         */
        putscom  CLK_REGION, ALL, START_CLOCKS | REGION_PIB
        test     CLOCK_STAT_SL,  REGION_PIB, 0, ERR_CLOCK_STAT
        test     CLOCK_STAT_NSL, REGION_PIB, 0, ERR_CLOCK_STAT
        test     CLOCK_STAT_ARY, REGION_PIB, 0, ERR_CLOCK_STAT

// This is the label we might jump to from above. Only jumps forward are allowed.
skip:
        // RETURN from the main command table will end command table execution and
        // the ROM code will continue.
        return
