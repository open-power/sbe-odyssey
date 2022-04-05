/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/chipop_table.C $         */
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

#include "sbecmdscomaccess.H"
#include "istep.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "chipop_handler.H"
#include "chipop_table.H"
#include "sbecmdringaccess.H"

static const uint16_t HARDWARE_FENCED_STATE =
     SBE_FENCE_AT_CONTINUOUS_IPL|SBE_FENCE_AT_DMT;

static const uint16_t PUT_HARDWARE_FENCED_STATE =
     HARDWARE_FENCED_STATE;

////////////////////////////////////////////////////////////////
// @brief g_sbeIplControlCmdArray
//
////////////////////////////////////////////////////////////////
CMD_ARR(
   A1,
   {sbeHandleIstep,
    SBE_CMD_EXECUTE_ISTEP,
    HARDWARE_FENCED_STATE|SBE_FENCE_AT_DUMPING,
   },
)

CMD_ARR(
    A2,
    {sbeGetScom,
     SBE_CMD_GETSCOM,
     HARDWARE_FENCED_STATE,
    },
    {sbePutScom,
     SBE_CMD_PUTSCOM,
     HARDWARE_FENCED_STATE,
    }
)

CMD_ARR(
    A3,
    {sbeGetRing,
     SBE_CMD_GETRING,
     SBE_FENCE_AT_CONTINUOUS_IPL|SBE_FENCE_AT_QUIESCE,
    },

    {sbePutRing,
     SBE_CMD_PUTRING,
     HARDWARE_FENCED_STATE|SBE_FENCE_AT_QUIESCE,
    }
)

// Mandatory macro inclusion
CMD_CLASS_DEFAULT_INTIALISATION
