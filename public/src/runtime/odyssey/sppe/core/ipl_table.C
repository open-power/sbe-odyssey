/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/ipl_table.C $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#include "ipl.H"
#include "return_code.H"
#include "sbetrace.H"

using namespace fapi2;

//----------------------------------------------------------------------------

ReturnCode istepNoOp( voidfuncptr_t i_hwp)
{
    SBE_INFO("istepNoOp");
    return FAPI2_RC_SUCCESS ;
}

// File static data
static istepMap_t g_istep2PtrTbl[] =
         {
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP(istepNoOp, NULL),
         };

istepTableEntry_t istepTableEntries[] = {
    ISTEP_ENTRY(  2, g_istep2PtrTbl),
};

REGISTER_ISTEP_TABLE(istepTableEntries)
