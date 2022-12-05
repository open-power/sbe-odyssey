/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/chipop_handler.H $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2023                        */
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

#ifndef __CHIOP_HANDLER_H__
#define __CHIOP_HANDLER_H__

#include <stdint.h>
#include <stddef.h>
#include "sbe_sp_intf.H"
#include "chipop_struct.H"
#include "chipophandlerutils.H"

/**
  * @brief sbeIsCmdAllowed  Validates if the command is allowed
  *        at the current SBE state
  *
  * @param[in]    i_cmdClass    Command class code
  * @param[in]    i_cmdOpcode   Command opcode
  *
  * @return       sbeChipOpRc_t Indicating primary and secondary status of
  *                             chip-op
  */
sbeChipOpRc_t sbeIsCmdAllowed (const uint8_t i_cmdClass,
                             const uint8_t i_cmdOpcode);


#endif
