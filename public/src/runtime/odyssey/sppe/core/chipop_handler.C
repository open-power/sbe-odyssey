/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/chipop_handler.C $       */
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
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbestates.H"
#include "sberegaccess.H"
#include "sbeglobals.H"
#include "cmd_class_init.H"
#include "chipop_handler.H"
#include "fences.H"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

sbeChipOpRc_t sbeIsCmdAllowed (const uint8_t i_cmdClass,
                               const uint8_t i_cmdOpcode)
{
    #define SBE_FUNC " sbeIsCmdAllowedAtState "
    bool l_ret = true;
    sbeChipOpRc_t retRc;
    uint8_t l_numCmds = 0;
    cmdStruct_t *l_pCmd = NULL;
    l_numCmds = sbeGetCmdStructAttr (i_cmdClass, &l_pCmd);

    for (uint8_t l_cnt = 0; l_cnt < l_numCmds; ++l_cnt, ++l_pCmd)
    {
        if (i_cmdOpcode == l_pCmd->cmd_opcode)
        {
            // Get the Present State
            uint64_t l_state =
                SbeRegAccess::theSbeRegAccess().getSbeState();
            SBE_INFO(SBE_FUNC "SBE State [0x%08X] Fence State[0x%04X]",
                (uint32_t)(l_state & 0xFFFFFFFF),l_pCmd->cmd_state_fence);

            switch(l_state)
            {
                case SBE_STATE_UNKNOWN:
                case SBE_STATE_FAILURE:
                    // All operations are fenced here, return false
                    // Reset is the only Option available
                    break;

                case SBE_STATE_IPLING:
                {
                    l_ret = ((l_pCmd->cmd_state_fence &
                             SBE_FENCE_AT_CONTINUOUS_IPL)? false:true);
                    break;
                }

                case SBE_STATE_ISTEP:
                {
                    l_ret = ((l_pCmd->cmd_state_fence &
                             SBE_FENCE_AT_ISTEP)? false:true);
                    break;
                }

                case SBE_STATE_RUNTIME:
                {
                    l_ret = ((l_pCmd->cmd_state_fence &
                             SBE_FENCE_AT_RUNTIME)? false:true);
                    break;
                }

                case SBE_STATE_DUMP:
                {
                    l_ret = ((l_pCmd->cmd_state_fence &
                             SBE_FENCE_AT_DUMPING)? false:true);
                    break;
                }

                default:
                    l_ret = false;
                    break;
            }
            if(false == l_ret)
            {
                retRc.primStatus = SBE_PRI_INVALID_COMMAND;
                retRc.secStatus  = SBE_SEC_COMMAND_NOT_ALLOWED_IN_THIS_STATE;
                break;
            }
            break;
        }
    }
    // For any other state, which is not handled above, return from here
    return retRc;
    #undef SBE_FUNC
}
