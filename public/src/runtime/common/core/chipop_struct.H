/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/chipop_struct.H $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2022                        */
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

#ifndef __CHIOP_STRUCT_H__
#define __CHIOP_STRUCT_H__

#include <stdint.h>
#include <stddef.h>
#include "sbe_sp_intf.H"

// Chip-op response codes
typedef struct sbeChipOpRc
{
    sbePrimResponse primStatus;
    sbeSecondaryResponse secStatus;

    sbeChipOpRc():primStatus(SBE_PRI_OPERATION_SUCCESSFUL),
                  secStatus(SBE_SEC_OPERATION_SUCCESSFUL)
    {}

    bool success()
    {
        return (primStatus == SBE_PRI_OPERATION_SUCCESSFUL) &&
               (secStatus == SBE_SEC_OPERATION_SUCCESSFUL);
    }
} sbeChipOpRc_t;

/**
 * @brief SBE chip-op parameter definition
 *
 */
typedef struct chipOpParam
{
    uint8_t  fifoType;          // Refer to enum sbeFifoType
    uint16_t reserved;          // Reserved
    uint8_t* pkThreadParam;     // Private data to be passed as the argument to
                                // the thread routine when it begins execution
} chipOpParam_t;

/**
 *  @brief SBE Command structure associating an opcode of a command
 *         to the processing function as well as the allowed states
 *
 */
typedef uint32_t (*sbeChipOpFunc_t) (uint8_t *i_pArg);

typedef struct sbeCmdStruct {
    sbeChipOpFunc_t  cmd_func;        /* Command function pointer */
    uint8_t          cmd_opcode;      /* Command opcode */
    uint16_t         cmd_state_fence; /* Command fencing based on SBE state */
} sbeCmdStruct_t;

#endif
