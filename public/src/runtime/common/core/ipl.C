/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/ipl.C $                        */
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

#include <stdint.h>
#include "sbe_sp_intf.H"
#include "ipl.H"
#include "sbeglobals.H"
#include "sberegaccess.H"
#include "sbetrace.H"
#include "sbestates.H"
#include "sbestatesutils.H"
#include "sbeffdc.H"
#include "istepIplUtils.H"


using namespace fapi2;
extern "C" void __sbe_register_saveoff();
extern uint32_t __g_isParityError;

void ipl::sbeAutoBoot()
{
    #define SBE_FUNC " sbeAutoBoot "
    SBE_ENTER(SBE_FUNC);
    ReturnCode rc = FAPI2_RC_SUCCESS;

    do{
        // Set SBE state as IPLing
        stateTransition(SBE_EVENT_CMN_PLCK);

        // Run isteps
        for(size_t entry = 0; entry < g_istepTable.len; entry++)
        {
            auto istepTableEntry = &g_istepTable.istepMajorArr[entry];
            for(size_t step = 1; step <= istepTableEntry->len; step++)
            {
                auto istepMap = &istepTableEntry->istepMinorArr[step-1];
                if(istepMap->istepWrapper != NULL)
                {
                    // TODO: PFSBE-316 Enable SBE Console messaging.
                    // SBE_MSG_CONSOLE("istep ", istepTableEntry->istepMajorNum, ".", step);
                    SBE_DEBUG("istep %d.%d", istepTableEntry->istepMajorNum, step);
                    rc = istepMap->istepWrapper(istepMap->istepHwp);
                }

                if (isSpiParityError()) // If true call saveoff and halt
                {
                    __g_isParityError = true;
                    __sbe_register_saveoff();
                }

                if(rc != FAPI2_RC_SUCCESS)
                {
                    SBE_ERROR(SBE_FUNC "Failed istep execution in sbeAutoBoot mode: "
                            "Major: %d, Minor: %d",
                            istepTableEntry->istepMajorNum, step);

                    uint32_t secRc = SBE_SEC_GENERIC_FAILURE_IN_EXECUTION;
                    captureAsyncFFDC(SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                     secRc);
                    // exit outer loop as well
                    entry = g_istepTable.len;
                    break;
                }
                else
                {
                    (void)SbeRegAccess::theSbeRegAccess()
                    .updateSbeStep(istepTableEntry->istepMajorNum,
                                    step);

                    // Checking once we reached istepEndMajor number and successfully executed
                    // All minor number then transition to runtime state .
                    if ( istepTableEntry->istepMajorNum == g_pSbeIstepIplUtils->getIstepEndMajorNumber())
                    {
                        // Transition to runtime state on autoboot success
                        stateTransition(SBE_EVENT_CMN_RUNTIME);
                    }
                }
            }

        }
    }while(false);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}
