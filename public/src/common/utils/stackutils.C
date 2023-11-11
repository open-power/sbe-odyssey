/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/stackutils.C $                        */
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

#include "sbetrace.H"
#include "stackutils.H"
#include "assert.h"

uint8_t getStackUtilPercent(uint32_t stack_base, uint32_t stack_limit)
{
    #define SBE_FUNC " getStackUtilPercent "
    SBE_ENTER(SBE_FUNC)

    PK_STACK_TYPE *ptr = (PK_STACK_TYPE*)stack_limit;

    // stackUnUtilized based on PK_STACK_TYPE (Mostly words)
    uint32_t stackUnUtilized = 0x0;
    // totalStackSize is in bytes
    uint32_t totalStackSize = stack_base -  stack_limit;

    while ((ptr < (PK_STACK_TYPE*)stack_base) && (*ptr == PK_STACK_PATTERN))
    {
        stackUnUtilized++;
        ptr++;
    }

    // convert stackUnUtilized in PK_STACK_TYPE to bytes.
    stackUnUtilized *= sizeof(PK_STACK_TYPE);

    uint32_t utilizedStackSize = totalStackSize - stackUnUtilized;
    uint8_t percentageUtilized =
        ((utilizedStackSize * 100) / totalStackSize);

    SBE_INFO(SBE_FUNC "Stack Start : 0x%08x, Stack end : 0x%08x",
                        stack_base, stack_limit);

    SBE_INFO(SBE_FUNC "Total Stack Size : %d bytes, Stack Utilized : %d bytes, \
                        Stack UnUtilized : %d bytes, Percentage of Utilization : %d%%",
                        totalStackSize, utilizedStackSize, stackUnUtilized, percentageUtilized);

    SBE_EXIT(SBE_FUNC)
    return percentageUtilized;
    #undef SBE_FUNC
}

uint8_t getCurThreadStackUtilPercent()
{
    PkThread* thread = (PkThread*)__pk_current_thread;
    return getStackUtilPercent(thread->stack_base, thread->stack_limit);
}

uint8_t getKernelStackUtilPercent()
{
    return getStackUtilPercent(__pk_kernel_stack, __pk_kernel_stack_limit);
}

void checkCurThreadStackLimit()
{
    #define SBE_FUNC " checkCurThreadStackLimit "
    SBE_ENTER(SBE_FUNC)

    uint8_t curThreadStackUtilPercent = getCurThreadStackUtilPercent();

    assert(curThreadStackUtilPercent < CRITICAL_STACK_LIMIT_PERCENT);

    if(curThreadStackUtilPercent > THRESHOLD_STACK_LIMIT_PERCENT)
    {
        //TODO: PFSBE-799 Create RE if stack utilization percentage has crossed set threshold
        SBE_INFO(SBE_FUNC "**** WARNING : Current Thread Percentage of Utilization : %d%% > Threshold Limit : %d%% ***",
                            curThreadStackUtilPercent, THRESHOLD_STACK_LIMIT_PERCENT);
    }

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}

void checkKernelStackLimit()
{
    #define SBE_FUNC " checkKernelStackLimit "
    SBE_ENTER(SBE_FUNC)

    uint8_t kernelStackUtilPercent = getKernelStackUtilPercent();

    assert(kernelStackUtilPercent < CRITICAL_STACK_LIMIT_PERCENT);

    if(kernelStackUtilPercent > THRESHOLD_STACK_LIMIT_PERCENT)
    {
        //TODO: PFSBE-799 Create RE if stack utilization percentage has crossed set threshold
        SBE_INFO(SBE_FUNC "**** WARNING : Kernel Thread Percentage of Utilization : %d%% > Threshold Limit : %d%% ***",
                            kernelStackUtilPercent, THRESHOLD_STACK_LIMIT_PERCENT);
    }

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}