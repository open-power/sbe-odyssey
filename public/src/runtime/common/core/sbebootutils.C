/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbebootutils.C $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "sbebootutils.H"
#include "sberegaccess.H"
#include "sbestates.H"
#include "sbetrace.H"
#include "ipl.H"
#include "istepIplUtils.H"

void setSBEBootState(uint8_t i_bootMode)
{
    #define SBE_FUNC "setSBEBootState"
    SBE_ENTER(SBE_FUNC);
    switch(i_bootMode)
    {
        case SbeRegAccess::BootMode::AUTOBOOT:
            SBE_INFO(SBE_FUNC "AutoBoot Mode set... IPLing");
            (void)SbeRegAccess::theSbeRegAccess().
                updateSbeState(SBE_STATE_CMN_IPLING);
            g_pSbeIstepIplUtils->sbeAutoBoot();
            break;
        case SbeRegAccess::BootMode::JUMP_TO_RUNTIME:
            // jumpToRuntime
            SBE_INFO(SBE_FUNC "Jump To Runtime Mode ...");
            (void)SbeRegAccess::theSbeRegAccess().
                updateSbeState(SBE_STATE_CMN_RUNTIME);
            break;
        case SbeRegAccess::BootMode::ISTEP:
            SBE_INFO(SBE_FUNC " Istep Mode ...");
            (void)SbeRegAccess::theSbeRegAccess().
                updateSbeState(SBE_STATE_CMN_ISTEP);
            break;
        case SbeRegAccess::BootMode::PAUSE_AND_BOOT:
            // PauseAndBoot
            // TODO: JIRA: PFSBE-315
            SBE_ERROR(SBE_FUNC "Unsupported Pause and Boot mode ");
        default:
            SBE_ERROR(SBE_FUNC " Not a supported Boot mode 0x%02X", i_bootMode);
            (void)SbeRegAccess::theSbeRegAccess().
                updateSbeState(SBE_STATE_CMN_FAILURE);
            break;
    }
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}