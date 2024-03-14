/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/stackutils.C $           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
#include "plat_error_scope.H"
#include "hwp_ffdc_classes.H"

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

    SBE_INFO(SBE_FUNC "stack start : 0x%08x, end : 0x%08x",
                        stack_base, stack_limit);

    SBE_INFO(SBE_FUNC "stack size : %d bytes, utilized : %d bytes, "
                      "utilization : %d%%",
                      totalStackSize, utilizedStackSize, percentageUtilized);

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

inline void checkLimitAndAct(uint8_t i_threadId, uint8_t i_utilization)
{
    assert( i_utilization < CRITICAL_STACK_LIMIT_PERCENT );

    PLAT_FAPI_ASSERT_NOEXIT((i_utilization < THRESHOLD_STACK_LIMIT_PERCENT),
                            fapi2::THRESHOLD_STACK_LIMIT_CROSSED(fapi2::FAPI2_ERRL_SEV_RECOVERED).
                            set_THREAD_ID(i_threadId).
                            set_THRESHOLD_PERCENTAGE(THRESHOLD_STACK_LIMIT_PERCENT).
                            set_UTILIZATION_PERCENTAGE(i_utilization),
                            "Thread [%d] stack usage crossed threshold limit", i_threadId);
}

void checkCurThreadStackUsage()
{
    #define SBE_FUNC " checkCurThreadStackUsage"
    SBE_ENTER(SBE_FUNC)

    uint8_t curThreadStackUtilPercent = getCurThreadStackUtilPercent();

    PkThread* thread = (PkThread*)__pk_current_thread;

    checkLimitAndAct(thread->priority, curThreadStackUtilPercent);

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}

void checkKernelStackUsage()
{
    #define SBE_FUNC " checkKernelStackUsage"
    SBE_ENTER(SBE_FUNC)

    uint8_t kernelStackUtilPercent = getKernelStackUtilPercent();

    checkLimitAndAct(0, kernelStackUtilPercent);

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}

void checkThreadStackUsage(const PkThread* i_thread)
{
    uint8_t l_stackUtilPercentage = getStackUtilPercent(i_thread->stack_base,
                                                        i_thread->stack_limit);

    checkLimitAndAct(i_thread->priority, l_stackUtilPercentage);
}

