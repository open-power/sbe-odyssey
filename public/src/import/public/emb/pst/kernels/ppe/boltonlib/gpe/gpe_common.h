/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/boltonlib/gpe/gpe_common.h $ */
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
#ifndef __GPE_COMMON_H__
#define __GPE_COMMON_H__


/// \file gpe_common.h
/// \brief Common header for GPE

#ifndef __ASSEMBLER__
    #include <stdint.h>
#endif

#include "pmhw_common.h"

/// Each GPE instance has it's own interrupt status register these macros
/// are added for convenience in accessing the correct register
#ifndef UNIFIED_IRQ_HANDLER_GPE
    #define GPE_GISR0(instance_id) (OCB_G0ISR0 + (instance_id * 8))
    #define GPE_GISR1(instance_id) (OCB_G0ISR1 + (instance_id * 8))
#endif

#ifdef __ASSEMBLER__
    #ifdef __PK__
        #include "pk.h"
// *INDENT-OFF*
/// This macro contains GPE specific code for determining what IRQ caused the
/// external exception handler to be invoked by the PPE

/// Check for interrupts pending in status register 0 while the IRQ is
/// computed.  The IRQ is expected to be stored in r4.  If no IRQ is
/// pending then load the phantom irq # (EXTERNAL_IRQS).
///
/// r1, r2, r3, and r13 must not be modified.  All other registers may be used.
///
/// The pk_unified_irq_prty_mask_handler routine MUST return the task priority
/// interrupt vector in d5.
///
    .macro hwmacro_get_ext_irq

#ifdef UNIFIED_IRQ_HANDLER_GPE
        // Unified approach.
        _liw        r5, pk_unified_irq_prty_mask_handler
        mtlr        r5
        blrl            // On return, d5 contains task prty irq vec.
        mfsprg      r3, 0 // In case r3 is modified by unified handler, restore to sprg0
#else
        _lwzi       %r5, %r5, GPE_GISR0(APPCFG_PM_INSTANCE_ID)
#endif
        cntlzw      %r4, %r5
        cmpwible    %r4, 31, call_external_irq_handler   #branch if irq is lt or eq to 31

        ## No IRQ pending in interrupt set 0.  Try set 1.
        ## Note: irq # will be 64 (EXTERNAL_IRQS) if no bits were set in either register

#ifndef UNIFIED_IRQ_HANDLER_GPE
        _lwzi       %r6, %r6, GPE_GISR1(APPCFG_PM_INSTANCE_ID)
#endif
        cntlzw      %r4, %r6
        addi        %r4, %r4, 32

    .endm

#endif /* __PK__ */
/// Redirect the .hwmacro_irq_cfg_bitmaps macro to call our macro that is common for both
/// GPE's and the 405 inside the OCC complex.  This is called from the ppe42_exceptions.S
/// file.
    .macro .hwmacro_irq_cfg_bitmaps
#if defined(__OCC_PLAT)
        .occhw_irq_cfg_bitmaps
#elif defined(__TCC_PLAT)
        .tcchw_irq_cfg_bitmaps
#else
#error "Either __OCC_PLAT or __TCC_PLAT must be defined by the application"
#endif
    .endm
// *INDENT-ON*
#endif /* __ASSEMBLER__ */

#endif  /* __GPE_COMMON_H__ */
