/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/chipophandlerutils.H $         */
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

#ifndef __CHIOP_HANDLER_UTILS_H__
#define __CHIOP_HANDLER_UTILS_H__

#include <stdint.h>
#include <stddef.h>
#include "sbe_sp_intf.H"
#include "chipop_struct.H"

////////////////////////////CHIP-OP table helper////////////////////////////
typedef struct {
    sbeChipOpFunc_t  cmd_func;         /* Command function pointer */
    uint8_t          cmd_opcode;       /* Command opcode */
    uint16_t         cmd_state_fence;  /* Command fencing based on SBE state */
} cmdStruct_t;

typedef struct {
    cmdStruct_t* cmdArr;
    size_t len;
} cmdStructTable_t;

typedef struct
{
    uint8_t cmdClass;
    cmdStructTable_t* cmdTable;
} cmdClass_t;

typedef struct
{
    cmdClass_t* cmdClassArr;
    size_t len;
} cmdClassTable_t;

extern cmdClassTable_t cmdClassTable;
extern cmdStructTable_t cmdTable;

///////////////Auto population of chip-op table in project files///////////////
#define CMD_CLASS(cmdClass) \
        { 0x##cmdClass, cmdStructP_t<0x##cmdClass>::value }

template <int T>
struct cmdStructP_t
{
    static constexpr cmdStructTable_t* value = NULL;
};

#define CMD_ARR(cmdclass, ...) \
    cmdStruct_t cmdClassArr_##cmdclass[] = \
    { \
        __VA_ARGS__ \
    }; \
    \
    cmdStructTable_t cmdClassArr_table_##cmdclass = { \
        cmdClassArr_##cmdclass, \
        sizeof(cmdClassArr_##cmdclass)/sizeof(cmdClassArr_##cmdclass[0]) \
    }; \
    template <> \
    struct cmdStructP_t<0x##cmdclass> \
    { \
        static constexpr cmdStructTable_t* value = &cmdClassArr_table_##cmdclass; \
    };

#define CMD_CLASS_COMMON \
cmdClassTable_t cmdClassTable = { \
    const_cast<cmdClass_t*>(cmdClassArr), \
    sizeof(cmdClassArr)/sizeof(cmdClassArr[0]) \
}; \
\
constexpr bool validateCmdClassHash(uint8_t index) { \
    return (index >= sizeof(cmdClassArr)/sizeof(cmdClassArr[0])) || \
            ((HASH_KEY(cmdClassArr[index].cmdClass)) == index && \
             validateCmdClassHash(index + 1)); \
} \
\
static_assert(validateCmdClassHash(0), "Bad chipop_table, check cmdclass hashes");

////////////////////////////////////////////////////////////////////////////////////

uint8_t sbeGetCmdStructAttr (const uint8_t  i_cmdClass,
                                   cmdStruct_t **o_ppCmd);

/**
  * @brief sbeValidateCmdClass  Validates the command class and opcode
  *
  * @param[in]     i_cmdClass    Command class code
  * @param[in]     i_cmdOpcode   Command opcode
  *
  * @return uint8_t  return code
  *               SBE_SEC_OPERATION_SUCCESSFUL - Command found
  *               SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED
  *               SBE_SEC_COMMAND_NOT_SUPPORTED
  */
uint8_t sbeValidateCmdClass (const uint8_t i_cmdClass,
                             const uint8_t i_cmdOpcode);

/**
  * @brief sbeCmdFunc_t Typical signature for any SBE ChipOp back-end function
  *
  * @param[in]  uint8_t *i_pArg  Pointer to the argument to be passed to
  *                              the chipOp function
  *
  * @return     uint32_t Return code from the chipOp function
  */
typedef uint32_t ( *sbeCmdFunc_t ) (uint8_t *i_pArg);

/**
  * @brief sbeFindCmdFunc  Finds the function corresponding to the command
  *
  * @param[in]     i_cmdClass    Command class code
  * @param[in]     i_cmdOpcode   Command opcode
  *
  * @return  sbeCmdFunc_t A pointer to the corresponding ChipOps function
  */
sbeCmdFunc_t sbeFindCmdFunc (const uint8_t i_cmdClass,
                             const uint8_t i_cmdOpcode);

#endif
