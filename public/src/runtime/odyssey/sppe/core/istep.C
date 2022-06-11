/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/istep.C $                */
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
#include <stdint.h>

#include "sbefifo.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbeFifoMsgUtils.H"
#include "assert.h"
#include "sberegaccess.H"
#include "sbestates.H"
#include "sbeglobals.H"
#include "ipl.H"
#include "istep.H"

#define ISTEP_START_MINOR_NUMBER 13

using namespace fapi2;
//----------------------------------------------------------------------------
static void findNextIstep(uint8_t& o_nextMajor, uint8_t& o_nextMinor)
{
    #define SBE_FUNC "findNextIstep "
    uint8_t prevMajorNumber =
                SbeRegAccess::theSbeRegAccess().getSbeMajorIstepNumber();
    uint8_t prevMinorNumber =
                SbeRegAccess::theSbeRegAccess().getSbeMinorIstepNumber();
    SBE_INFO(SBE_FUNC"prevMajorNumber:%u prevMinorNumber:%u ",
                        prevMajorNumber, prevMinorNumber );
    if( 0 == prevMajorNumber )
    {
        prevMajorNumber = istepTable.istepMajorArr[0].istepMajorNum;
        prevMinorNumber = ISTEP_START_MINOR_NUMBER - 1;
    }

    for(size_t entry = 0; entry < istepTable.len; entry++)
    {
        auto istepTableEntry = &istepTable.istepMajorArr[entry];
        if( istepTableEntry->istepMajorNum == prevMajorNumber)
        {
            if( prevMinorNumber == istepTableEntry->len )
            {
                o_nextMajor = prevMajorNumber + 1;
                o_nextMinor =  1;
            }
            else
            {
                o_nextMajor = prevMajorNumber;
                o_nextMinor = prevMinorNumber + 1;
            }
            break;
        }
    }
    #undef SBE_FUNC
}

static bool validateIstepRange(const uint8_t i_major, const uint8_t i_minor)
{
    bool inRange = false;
    do
    {
        if( 0 == i_minor )
        {
            break;
        }
        if((istepTable.istepMajorArr[0].istepMajorNum == i_major) &&
            (i_minor < ISTEP_START_MINOR_NUMBER))
        {
            break;
        }
        for(size_t entry = 0; entry < istepTable.len; entry++)
        {
            auto istepTableEntry = &istepTable.istepMajorArr[entry];
            if( i_major == istepTableEntry->istepMajorNum )
            {
                if( i_minor <= istepTableEntry->len )
                {
                    inRange = true;
                }
                break;
            }
        }
    }
    while(0);

    return inRange;
}

bool validateIstep (const uint8_t i_major, const uint8_t i_minor)
{
    #define SBE_FUNC "validateIstep "
    bool valid = false;
    do
    {
        if(false == validateIstepRange(i_major, i_minor))
        {
            SBE_ERROR(SBE_FUNC" Invalid Istep range. major:%u minor:%u",
                        i_major, i_minor);
            break;
        }
// TODO: P11SBE Porting
#if 0
        // If secuirty is not enabled, no further chacks asre required
        if(!SBE_GLOBAL->sbeFWSecurityEnabled)
        {
            SBE_INFO(SBE_FUNC" Security is disabled. Skipping istep order check");
            valid = true;
            break;
        }
#endif

        uint8_t nextMajorIstep = 0;
        uint8_t nextMinorIstep = 0;
        findNextIstep(nextMajorIstep, nextMinorIstep);

        if( ( i_major != nextMajorIstep) ||
            ( i_minor != nextMinorIstep) )
        {
            SBE_ERROR("Secuity validation failed for executing istep "
                    "Skipping istep not allowed "
                    "in secure mode. nextMajorIstep:%u "
                    "nextMinorIstep:%u", nextMajorIstep, nextMinorIstep);
            // TODO: remove this workaround after odyssey rit
            valid = true;
            break;
        }
        else
        {
            valid = true;
        }
    } while(0);

    return valid;
    #undef SBE_FUNC
}

//----------------------------------------------------------------------------
uint32_t sbeHandleIstep (uint8_t *i_pArg)
{
    #define SBE_FUNC "sbeHandleIstep "
    SBE_ENTER(SBE_FUNC);
    uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t len = 0;
    sbeIstepReqMsg_t req;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;

    // NOTE: In this function we will have two loops
    // First loop will deque data and prepare the response
    // Second loop will enque the data on DS FIFO
    //loop 1
    do
    {
        len = sizeof( req )/sizeof(uint32_t);
        rc = sbeUpFifoDeq_mult ( len, (uint32_t *)&req);
        if (rc != SBE_SEC_OPERATION_SUCCESSFUL) //FIFO access issue
        {
            SBE_ERROR(SBE_FUNC"FIFO dequeue failed, rc[0x%X]", rc);
            break;
        }

        if( false == validateIstep( req.major, req.minor ) )
        {
            SBE_ERROR(SBE_FUNC" Invalid Istep. major:%u"
                      " minor:%u",
                      req.major, req.minor);
            // @TODO via RTC 132295.
            // Need to change code asper better error handling.
            respHdr.setStatus( SBE_PRI_INVALID_DATA,
                               SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
            break;
        }

        fapiRc = sbeExecuteIstep( req.major, req.minor );
        // TODO - F001A is not available till istep 2.3, which is driven by the
        // nest clock, so we can enable this only after 2.3, For time being
        // commenting this out.
        //bool checkstop = isSystemCheckstop();
        if( fapiRc != FAPI2_RC_SUCCESS )
        //if(( fapiRc != FAPI2_RC_SUCCESS ) || (checkstop))
        {
            SBE_ERROR(SBE_FUNC" sbeExecuteIstep() Failed. major:%u"
                " minor:%u", req.major, req.minor);
            //if(checkstop)
            //{
            //    respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
            //                       SBE_SEC_SYSTEM_CHECKSTOP );
            //}
            //else
            //{
                respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                   SBE_SEC_GENERIC_FAILURE_IN_EXECUTION );
                ffdc.setRc(fapiRc);
            //}

            break;
        }

    }while(0);

    //loop 2
    do
    {
        // FIFO error
        if ( rc )
        {
            break;
        }

        rc = sbeDsSendRespHdr(respHdr, &ffdc);
    }while(0);

    if( rc )
    {
        SBE_ERROR( SBE_FUNC"Failed. rc[0x%X]", rc);
    }
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}
