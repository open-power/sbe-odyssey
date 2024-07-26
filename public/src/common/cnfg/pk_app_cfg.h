/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/cnfg/pk_app_cfg.h $                         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2024                        */
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
/*
 * @file: ppe/sbe/sbefw/pk_app_cfg.h
 *
 * @brief Application specific overrides go here.
 *
 */

#pragma once

#include "sbeirq.H"

/*
 * @brief  Static configuration data for external interrupts:
 *     IRQ#, TYPE, POLARITY, ENABLE
 *
 * For SBE platform, hardware will setup the interrupt configuration, hence PK
 *  doesnt have to setup the interrupts. But keeping these macros, otherwise static
 *  checks in PK will fail.
 *
 */
#define APPCFG_EXT_IRQS_CONFIG

#ifndef __ASSEMBLER__

    /*
     * @brief 64-bit mask specifies which of the interrupts are not to be used.
     *         Refer below for details.
     */
    extern uint64_t g_irq_invalid_mask;
    /*
     * @brief This 64 bit mask specifies which of the interrupts are not to be used
     *         or unavailable to this platform. To make the PK compilation independent
     *         of different projects, implementing this macro as a global variable.
     *         And each SBE project has to define this global variable.
     *
     */
    #define APPCFG_IRQ_INVALID_MASK g_irq_invalid_mask
#else
    /*
    * @brief This 64 bit mask specifies which of the interrupts are not to be used
    *         or unavailable to this platform.
    *
    *        A dummy APPCFG_IRQ_INVALID_MASK is used here to pass the compilation
    *         of assembly code in PK. But these assembly code is never executed
    *         in SBE platform.
    *
    */
    #define APPCFG_IRQ_INVALID_MASK 0xFFFFFFFFFFFFFFFF
#endif



/*
 * @brief  Override the default behavior of the PK API error handling.
 *          Force PK to send the return code back to the application,
 *          instead of a kernel panic.
 *
 */
#ifndef PK_ERROR_PANIC
#define PK_ERROR_PANIC 0
#endif

/*
 * @brief  Override the default behavior of idle timer trace.
 *         As SBE trace buffer is small, we do not want idle timer traces.
 *
 */
#define PK_TRACE_TIMER_OUTPUT 0

#ifdef PPE42_MACHINE_CHECK_HANDLER
#undef PPE42_MACHINE_CHECK_HANDLER
#endif
#define PPE42_MACHINE_CHECK_HANDLER SBE_MACHINE_CHECK_HANDLER

// Added default register-save handler to the interrupt
#ifdef PPE_CAPTURE_INTERRUPT_FFDC
#undef PPE_CAPTURE_INTERRUPT_FFDC
#endif
#define PPE_CAPTURE_INTERRUPT_FFDC SBE_INTERRUPT_REGISTER_SAVEOFF
