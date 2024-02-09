/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/istep.C $                      */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2024                        */
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
#include "chipop_struct.H"
#include "sbefifo.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbeFifoMsgUtils.H"
#include "assert.h"
#include "sberegaccess.H"
#include "sbestates.H"
#include "sbestatesutils.H"
#include "istep.H"
#include "istepIplUtils.H"
#include "securityutils.H"

using namespace fapi2;

extern "C" void __sbe_register_saveoff();
extern uint32_t __g_isParityError;

void istep::findNextIstep(uint8_t& o_nextMajor, uint8_t& o_nextMinor)
{
    #define SBE_FUNC " findNextIstep "
    SBE_ENTER(SBE_FUNC);

    uint8_t prevMajorNumber =
                SbeRegAccess::theSbeRegAccess().getSbeMajorIstepNumber();
    uint8_t prevMinorNumber =
                SbeRegAccess::theSbeRegAccess().getSbeMinorIstepNumber();

    SBE_INFO(SBE_FUNC"prevMajorNumber:%u prevMinorNumber:%u ",
                        prevMajorNumber, prevMinorNumber );

    if( 0 == prevMajorNumber )
    {
        prevMajorNumber = g_istepTable.istepMajorArr[0].istepMajorNum;
        prevMinorNumber = iv_istepStartMinorNumber - 1;
    }

    for(size_t entry = 0; entry < g_istepTable.len; entry++)
    {
        auto istepTableEntry = &g_istepTable.istepMajorArr[entry];
        if( istepTableEntry->istepMajorNum == prevMajorNumber)
        {
            if( prevMinorNumber == istepTableEntry->len )
            {
                istepTableEntry = &g_istepTable.istepMajorArr[entry+1];
                o_nextMajor = istepTableEntry->istepMajorNum;
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

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

bool istep::validateIstepRange(const uint8_t i_major, const uint8_t i_minor)
{
    #define SBE_FUNC " validateIstepRange "
    SBE_ENTER(SBE_FUNC);

    bool inRange = false;

    do
    {
        if( 0 == i_minor )
        {
            break;
        }

        if((g_istepTable.istepMajorArr[0].istepMajorNum == i_major) &&
            (i_minor < iv_istepStartMinorNumber))
        {
            break;
        }

        for(size_t entry = 0; entry < g_istepTable.len; entry++)
        {
            auto istepTableEntry = &g_istepTable.istepMajorArr[entry];
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

    SBE_EXIT(SBE_FUNC);
    return inRange;
    #undef SBE_FUNC
}

bool istep::validateIstep (const uint8_t i_major, const uint8_t i_minor)
{
    #define SBE_FUNC " validateIstep "
    SBE_ENTER(SBE_FUNC);

    bool valid = false;
    do
    {
        if(false == validateIstepRange(i_major, i_minor))
        {
            SBE_ERROR(SBE_FUNC" Invalid Istep range. major:%u minor:%u",
                        i_major, i_minor);
            break;
        }

        if(g_pSbeSecurityUtils->getScomFilteringCheckLvl() != SOFT_SECURITY_CHECK_ENABLED)
        {
            SBE_INFO(SBE_FUNC" Security is disabled. Skipping istep order check");
            valid = true;
            break;
        }

        uint8_t nextMajorIstep = 0;
        uint8_t nextMinorIstep = 0;
        findNextIstep(nextMajorIstep, nextMinorIstep);

        if( ( i_major != nextMajorIstep) ||
            ( i_minor != nextMinorIstep) )
        {
            SBE_ERROR("Security validation failed for executing istep "
                    "Skipping istep not allowed "
                    "in secure mode. nextMajorIstep:%u "
                    "nextMinorIstep:%u", nextMajorIstep, nextMinorIstep);
            break;
        }

        valid = true;

    } while(0);

    SBE_EXIT(SBE_FUNC);
    return valid;
    #undef SBE_FUNC
}

ReturnCode istep::sbeExecuteIstep(const uint8_t i_major, const uint8_t i_minor)
{
    #define SBE_FUNC " sbeExecuteIstep "
    SBE_ENTER(SBE_FUNC)

    ReturnCode rc = FAPI2_RC_SUCCESS;
    uint8_t l_lengthOfMinorNo = 0;

    SBE_INFO(SBE_FUNC "Major number:%d Minor number:%d", i_major, i_minor);

    for(size_t entry = 0; entry < g_istepTable.len; entry++)
    {
        auto istepTableEntry = &g_istepTable.istepMajorArr[entry];

        l_lengthOfMinorNo = istepTableEntry->len;

        if(( i_major == istepTableEntry->istepMajorNum ) && ( i_minor <=  l_lengthOfMinorNo))
        {
            auto istepMap = &istepTableEntry->istepMinorArr[i_minor-1];
            if(istepMap->istepWrapper != NULL)
            {
                rc = istepMap->istepWrapper(istepMap->istepHwp);
            }
            break;
        }
    }

    if (isSpiParityError()) // If true call saveoff and halt
    {
        __sbe_register_saveoff();
    }

    if(rc != FAPI2_RC_SUCCESS)
    {
        SBE_ERROR(SBE_FUNC "FAPI RC:0x%08X", rc);
        stateTransition(SBE_EVENT_CMN_DUMP_FAILURE);
    }
    else
    {
        (void)SbeRegAccess::theSbeRegAccess().updateSbeStep(i_major, i_minor);

        // Setting SBE to runtime after executing end major and minor number
        if ((i_major == iv_istepEndMajorNumber) && (l_lengthOfMinorNo == i_minor))
        {
            stateTransition(SBE_EVENT_CMN_RUNTIME);
        }
    }

    SBE_EXIT(SBE_FUNC)
    return rc;
    #undef SBE_FUNC
}

uint32_t sbeHandleIstep(uint8_t *i_pArg)
{
    #define SBE_FUNC " sbeHandleIstep "
    SBE_ENTER(SBE_FUNC);

    uint32_t fifoRc = SBE_SEC_OPERATION_SUCCESSFUL;
    ReturnCode fapiRc = FAPI2_RC_SUCCESS;
    uint32_t len = 0;
    sbeIstepReqMsg_t req;
    sbeRespGenHdr_t respHdr;
    respHdr.init();
    sbeResponseFfdc_t ffdc;

    chipOpParam_t* configStr = (struct chipOpParam*)i_pArg;
    sbeFifoType type = static_cast<sbeFifoType>(configStr->fifoType);

    do
    {
        len = sizeof( req )/sizeof(uint32_t);
        fifoRc = sbeUpFifoDeq_mult ( len, (uint32_t *)&req, true, false, type);
        // If FIFO access failure
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(fifoRc);

        if(g_pSbeIstepIplUtils->validateIstep(req.major, req.minor) == false)
        {
            SBE_ERROR(SBE_FUNC" Invalid Istep. major:%u minor:%u",
                      req.major, req.minor);
            respHdr.setStatus( SBE_PRI_INVALID_DATA,
                               SBE_SEC_GENERIC_FAILURE_IN_EXECUTION);
            break;
        }

        fapiRc = g_pSbeIstepIplUtils->sbeExecuteIstep( req.major, req.minor );
        if( fapiRc != FAPI2_RC_SUCCESS )
        {
            SBE_ERROR(SBE_FUNC" sbeExecuteIstep() Failed. major:%u"
                " minor:%u", req.major, req.minor);
            respHdr.setStatus( SBE_PRI_GENERIC_EXECUTION_FAILURE,
                                SBE_SEC_GENERIC_FAILURE_IN_EXECUTION );
            ffdc.setRc(fapiRc);
            break;
        }

    }while(false);

    if( fifoRc == SBE_SEC_OPERATION_SUCCESSFUL )
    {
        fifoRc = sbeDsSendRespHdr(respHdr, &ffdc, type);
    }

    SBE_EXIT(SBE_FUNC);
    return fifoRc;
    #undef SBE_FUNC
}