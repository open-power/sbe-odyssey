/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/iota/iota_ppe42.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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
#ifndef __IOTA_PPE42_H__
#define __IOTA_PPE42_H__

#if !defined(__PPE42X__)
    #error P10 IOTA requires PPE42X instruction set.
#endif

#ifndef __ASSEMBLER__
    #include <stdint.h>
#endif

#include "ppe42_asm.h"
#include "ppe42_msr.h"
#include "ppe42_spr.h"
#include "ppe42_mmio.h"
#include "ppe_port.h"

#ifdef __PPE42_CORE_C__
    #define IF__PPE42_CORE_C__(x) x
    #define UNLESS__PPE42_CORE_C__(x)
#else
    #define IF__PPE42_CORE_C__(x)
    #define UNLESS__PPE42_CORE_C__(x) x
#endif

#ifdef __PPE42_IRQ_CORE_C__
    #define IF__PPE42_IRQ_CORE_C__(x) x
    #define UNLESS__PPE42_IRQ_CORE_C__(x)
#else
    #define IF__PPE42_IRQ_CORE_C__(x)
    #define UNLESS__PPE42_IRQ_CORE_C__(x) x
#endif


#ifdef HWMACRO_GPE
    #include "gpe.h"
#elif defined(HWMACRO_STD)
    #include "std.h"
#elif defined(HWMACRO_PPE)
    #include "ppe.h"
#else
    #error "Macro Type not specified.  Are you building from the correct directory?"
#endif


#define _IOTA_SCHEDULE_REASON_FIT 0x0f
#define _IOTA_SCHEDULE_REASON_DEC 0x0d
#define _IOTA_SCHEDULE_REASON_EXT 0x0e
#define _IOTA_SCHEDULE_REASON_WDG 0x0c

#if !defined(__PPE42X__)

    #define _IOTA_SAVE_D0_OFFSET    0
    #define _IOTA_SAVE_D4_OFFSET    8
    #define _IOTA_SAVE_D6_OFFSET   16
    #define _IOTA_SAVE_D8_OFFSET   24
    #define _IOTA_SAVE_D28_OFFSET  32
    #define _IOTA_SAVE_D30_OFFSET  40
    #define _IOTA_SAVE_SRR0_OFFSET 48
    #define _IOTA_SAVE_SRR1_OFFSET 52
    #define _IOTA_SAVE_LR_OFFSET   56
    #define _IOTA_SAVE_CR_OFFSET   60
    #define _IOTA_SAVE_R3_OFFSET   64
    #define _IOTA_SAVE_R10_OFFSET  68
    #define _IOTA_SAVE_CTR_OFFSET  72
    #define _IOTA_SAVE_XER_OFFSET  76

    #define _IOTA_TEMPORARY_R3_STACK_OFFSET -4

#endif

#ifndef PPE42_DBCR_INITIAL
    // Enable 'trap' to cause a halt
    #define PPE42_DBCR_INITIAL (DBCR_TRAP | DBCR_ZACE)
#endif

#ifdef __ASSEMBLER__
// *INDENT-OFF*

    ## ----------------------------------------------------------------------
    ## The Interrupt Stack Conext layout for IOTA interrupts @see stcxcu/lcxu
    ## ----------------------------------------------------------------------
        .set    IOTA_CTX_BACK_CHAIN,  0x00 # Pointer to previous frame
        .set    IOTA_CTX_LINKAGE,     0x04 # LR slot for called function
        .set    IOTA_CTX_SIZE,        0x58 # Context size (88)
    ## Additional IOTA context would go here if ever needed
        .set    IOTA_CTX_CR,          IOTA_CTX_SIZE - 0x50
        .set    IOTA_CTX_KERNEL_CTX,  IOTA_CTX_SIZE - 0x4c // SPRG0
        .set    IOTA_CTX_GPR0,        IOTA_CTX_SIZE - 0x48
        .set    IOTA_CTX_GPR1,        IOTA_CTX_SIZE - 0x44
        .set    IOTA_CTX_GPR3,        IOTA_CTX_SIZE - 0x40
        .set    IOTA_CTX_GPR4,        IOTA_CTX_SIZE - 0x3c
        .set    IOTA_CTX_GPR5,        IOTA_CTX_SIZE - 0x38
        .set    IOTA_CTX_GPR6,        IOTA_CTX_SIZE - 0x34
        .set    IOTA_CTX_GPR7,        IOTA_CTX_SIZE - 0x30
        .set    IOTA_CTX_GPR8,        IOTA_CTX_SIZE - 0x2c
        .set    IOTA_CTX_GPR9,        IOTA_CTX_SIZE - 0x28
        .set    IOTA_CTX_GPR10,       IOTA_CTX_SIZE - 0x24
        .set    IOTA_CTX_XER,         IOTA_CTX_SIZE - 0x20
        .set    IOTA_CTX_CTR,         IOTA_CTX_SIZE - 0x1c
        .set    IOTA_CTX_SRR0,        IOTA_CTX_SIZE - 0x18
        .set    IOTA_CTX_SRR1,        IOTA_CTX_SIZE - 0x14
        .set    IOTA_CTX_GPR28,       IOTA_CTX_SIZE - 0x10
        .set    IOTA_CTX_GPR29,       IOTA_CTX_SIZE - 0x0c
        .set    IOTA_CTX_GPR30,       IOTA_CTX_SIZE - 0x08
        .set    IOTA_CTX_GPR31,       IOTA_CTX_SIZE - 0x04
        .set    IOTA_CTX_PREV_FRAME,  IOTA_CTX_SIZE - 0x00 // Don't touch
        .set    IOTA_CTX_LR,          IOTA_CTX_SIZE + 0x04

# halt ppe with panic code macro
.macro _pk_panic, code
    tw  31,(\code)/256, (\code)%256
.endm

# branch to machine check handler
.macro __m_iota_machine_check_handler
    bl _iota_machine_check_handler
.endm

# EABI set SDATA2(r2) ,SDATA(r13), and stack pointer
# Call to _iota_boot should never return
# 7 instructions MAX!!
.macro __m_iota_boot
    lis     %r13, _SDA_BASE_@h
    ori     %r13, %r13, _SDA_BASE_@l
    lis     %r2, _SDA2_BASE_@h
    ori     %r2, %r2, _SDA2_BASE_@l
    lis     %r1, g_iota_execution_stack_end@h
    ori     %r1, %r1, g_iota_execution_stack_end@l
    bl      _iota_boot
.endm


# This routine is called on any exception or interrupt and saves some state
# prior to setting up the call to and calling __iota_save_interrupt_state_and_schedule
# Note: must be <=8 instructions!

.macro __m_iota_interrupt_and_exception_handler, iota_schedule_reason
# Creates Stack frame and saves CR,SPRG0,R0,R1,R3-R10,XER,CTR,SRR0,SRR1,R28-R31
stcxu   %r1,-IOTA_CTX_SIZE(%r1)
li      %r3, \iota_schedule_reason
bl      _iota_schedule
# modify stack copy of SRR1 (MSR), turn off WE
lwz     %r3, IOTA_CTX_SRR1(%r1)
rlwinm  %r3,%r3,0,14,12
stw     %r3, IOTA_CTX_SRR1(%r1)
lcxu    %r1, IOTA_CTX_SIZE(%r1)
rfi
.endm

// *INDENT-ON*
#else

/// CouNT Leading Zeros Word
#define cntlzw(x) \
    ({uint32_t __x = (x); \
    uint32_t __lzw; \
    asm volatile ("cntlzw %0, %1" : "=r" (__lzw) : "r" (__x)); \
    __lzw;})

/// CouNT Leading Zeros : uint32_t
static inline int
cntlz32(uint32_t x)
{
    return cntlzw(x);
}

/// CouNT Leading Zeros : uint64_t
static inline int
cntlz64(uint64_t x)
{
    if (x > 0xffffffff)
    {
        return cntlz32(x >> 32);
    }
    else
    {
        return 32 + cntlz32(x);
    }
}

// Memory barrier
#define barrier() asm volatile ("" : : : "memory")


#define sync() asm volatile ("sync" : : : "memory")

// machine context is simply the MSR
typedef unsigned long PkMachineContext;

/// Enter a critical section, saving the current machine
/// context.
UNLESS__PPE42_CORE_C__(extern)
inline
void ppe_interrupt_disable(PkMachineContext* context)
{
    *context = mfmsr();
    wrteei(0);
}

UNLESS__PPE42_CORE_C__(extern)
inline
void ppe_machine_context_set(PkMachineContext* context)
{
    mtmsr(*context);
}

// These are needed for pk trace
#define pk_critical_section_enter(pctx) \
    ppe_interrupt_disable(pctx)

/// Exit a critical section by restoring the previous machine context.

#define pk_critical_section_exit(pctx) \
    ppe_machine_context_set(pctx)

// Use these instead of pk version for iota based code
#define ppe_critical_section_enter(pctx) \
    ppe_interrupt_disable(pctx)

#define ppe_critical_section_exit(pctx) \
    ppe_machine_context_set(pctx)


// TODO What do we want the context to be for IOTA? if any.
typedef union
{

    uint32_t value;

    struct
    {

        /// A flag indicating that PK is in thread mode after a call of
        /// pk_start_threads().
        unsigned thread_mode : 1;

        /// If this field is non-zero then PK is processing an interrupt
        /// and the \c irq field will contain the PkIrqId of the interrupt
        /// that kicked off interrupt processing.
        unsigned processing_interrupt : 1;

        /// The priority of the currently running thread.  In an interrupt
        /// context, this is the priority of the thread that was interrupted.
        unsigned thread_priority : 6;

        /// This bit tracks whether the current context can be discarded or
        /// if the context must be saved.  If the processor takes an interrupt
        /// and this bit is set, then the current context will be discarded.
        /// This bit is set at the end of handling an interrupt and prior
        /// to entering the wait enabled state.
        unsigned discard_ctx : 1;

        /// The PkIrqId of the currently running (or last run) handler.  If
        /// \c processing_interrupt is set, then this is the
        /// PkIrqId of the IRQ that is currently executing.
        unsigned irq : 7;

        /// Each PPE application will define (or not) the interpretation of
        /// this field.  Since SPRG0 is saved and restored during during thread
        /// context switches, this field can be used to record the progress of
        /// individual threads.  The kernel and/or application will provide
        /// APIs or macros to read and write this field.
        unsigned app_specific : 16;

    } fields;

} __KernelContext;

static inline void ppe42_app_ctx_set(uint16_t app_ctx)
{
    uint32_t    mctx;
    __KernelContext   __ctx;
    mctx = mfmsr();
    wrteei(0);
    __ctx.value = mfspr(SPRN_SPRG0);
    __ctx.fields.app_specific = app_ctx;
    mtspr(SPRN_SPRG0, __ctx.value);
    mtmsr(mctx);
}

#endif // __ASSEMBLER__

#endif // __IOTA_PPE42_H__
