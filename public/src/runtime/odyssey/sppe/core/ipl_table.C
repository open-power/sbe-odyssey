/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/ipl_table.C $            */
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
#include "ipl.H"
#include "return_code.H"
#include "sbetrace.H"
#include "sbeutil.H"
#include "target.H"
#include <ody_tp_arrayinit.H>
#include <ody_tp_arrayinit_cleanup.H>
#include <ody_tp_initf.H>
#include <ody_tp_startclocks.H>
#include <ody_tp_init.H>

using namespace fapi2;

//----------------------------------------------------------------------------

using  sbeIstepHwpOcmb_t = ReturnCode (*)
                      (const Target<TARGET_TYPE_OCMB_CHIP> & i_target);

ReturnCode istepNoOp( voidfuncptr_t i_hwp)
{
    SBE_INFO("istepNoOp");
    return FAPI2_RC_SUCCESS ;
}

ReturnCode istepWithOcmb( voidfuncptr_t i_hwp)
{
    SBE_INFO("[DBG] : istepWithOcmb : starting");
    ReturnCode rc = FAPI2_RC_SUCCESS;
    Target<TARGET_TYPE_OCMB_CHIP > l_ocmb_chip = g_platTarget->plat_getChipTarget();
    assert( NULL != i_hwp );
    SBE_EXEC_HWP(rc, reinterpret_cast<sbeIstepHwpOcmb_t>( i_hwp ), l_ocmb_chip);
    return rc;
}

static istepMap_t g_istep1PtrTbl[] =
         {
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( istepNoOp, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( NULL, NULL ),
             ISTEP_MAP( istepWithOcmb, ody_tp_arrayinit ),
             ISTEP_MAP( istepWithOcmb, ody_tp_arrayinit_cleanup ),
             ISTEP_MAP( istepWithOcmb, ody_tp_initf ),
             ISTEP_MAP( istepWithOcmb, ody_tp_startclocks ),
             ISTEP_MAP( istepWithOcmb, ody_tp_init ),
         };

istepTableEntry_t istepTableEntries[] = {
    ISTEP_ENTRY(  1, g_istep1PtrTbl),
};

REGISTER_ISTEP_TABLE(istepTableEntries)
