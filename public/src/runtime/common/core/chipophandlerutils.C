/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/chipophandlerutils.C $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2023                        */
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

#include "chipophandlerutils.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbeglobals.H"
#include "cmd_class_init.H"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
uint8_t sbeGetCmdStructAttr (const uint8_t  i_cmdClass,
                                   cmdStruct_t **o_ppCmd)
{
    #define SBE_FUNC " sbeGetCmdStructAttr "
    uint8_t l_numCmds  = 0;
    *o_ppCmd = NULL;

    size_t cmdClassIndex = HASH_KEY(i_cmdClass);
    if(cmdClassIndex < cmdClassTable.len &&
       cmdClassTable.cmdClassArr[cmdClassIndex].cmdTable)
    {
        l_numCmds = cmdClassTable.cmdClassArr[cmdClassIndex].cmdTable->len;
        *o_ppCmd = cmdClassTable.cmdClassArr[cmdClassIndex].cmdTable->cmdArr;
    }

    return l_numCmds;
    #undef SBE_FUNC
}

uint8_t sbeValidateCmdClass (const uint8_t i_cmdClass,
                             const uint8_t i_cmdOpcode)
{
    #define SBE_FUNC " sbeValidateCmdClass "
    uint8_t l_rc           = SBE_SEC_COMMAND_NOT_SUPPORTED;

    SBE_INFO(SBE_FUNC"i_cmdClass[0x%02X], "
              "i_cmdOpcode[0x%02X]", i_cmdClass, i_cmdOpcode);

    do
    {
        uint8_t l_numCmds      = 0;
        cmdStruct_t *l_pCmd = NULL;

        l_numCmds = sbeGetCmdStructAttr (i_cmdClass, &l_pCmd);
        if (!l_numCmds)
        {
            SBE_ERROR(SBE_FUNC"SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED "
                "i_cmdClass[0x%02X], i_cmdOpcode[0x%02X]",
                i_cmdClass, i_cmdOpcode);
            // Command class not supported
            l_rc = SBE_SEC_COMMAND_CLASS_NOT_SUPPORTED;
            break;
        }

        // @TODO via RTC : 128654
        //       Analyze on merging the validation functions into one
        //       and also on using loop vs switch case performance
        for (uint8_t l_cnt = 0; l_cnt < l_numCmds; ++l_cnt, ++l_pCmd)
        {
            if (i_cmdOpcode == l_pCmd->cmd_opcode)
            {
                // Command found
                l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
                break;
            }
        }
    } while (false);

    return l_rc;
    #undef SBE_FUNC
}

sbeCmdFunc_t sbeFindCmdFunc (const uint8_t i_cmdClass,
                             const uint8_t i_cmdOpcode)

{
    #define SBE_FUNC " sbeFindCmdFunc "
    uint8_t l_numCmds      = 0;
    cmdStruct_t *l_pCmd = NULL;

    l_numCmds = sbeGetCmdStructAttr (i_cmdClass, &l_pCmd);

    for (uint8_t l_cnt = 0; l_cnt < l_numCmds; ++l_cnt, ++l_pCmd)
    {
        if (i_cmdOpcode == l_pCmd->cmd_opcode)
        {
            break;
        }
    }

    return l_pCmd ? (l_pCmd->cmd_func) : NULL;
    #undef SBE_FUNC
}
