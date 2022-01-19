/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/cmdtable/example/cust.asm $ */
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
 * cust.asm - The customized table
 *
 * This table will be compiled into a separate binary that will be custom for each
 * chip and put in a separate section of SEEPROM. It can be accessed via the CALL
 * instruction in the main table.
 */
#include "defines.inc"

        // We're supposed to scan in repair data now but we're lazy so we'll just rotate the
        // ring a bunch.
        putscom  LONG_ROTATE, ALL, 0xl00000123

        /*
         * New instruction!
         * POLL address, mask, data, errorcode
         * Will poll a register until all bits where mask is 1 match the bits in data.
         * After a hardcoded poll timeout the FSM will write the error code into SB_MSG and halt.
         */
        poll     CPLT_STAT, OPCG_DONE, OPCG_DONE, ERR_ROTATE_TIMEOUT

        // Return to sender.
        return
