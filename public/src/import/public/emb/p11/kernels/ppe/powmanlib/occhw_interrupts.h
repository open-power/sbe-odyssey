/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/occhw_interrupts.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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
#ifndef __OCCHW_INTERRUPTS_H__
#define __OCCHW_INTERRUPTS_H__

/// \file occ_interrupts.h
/// \brief Interrupt assignments and macros for the OCC
///

#ifndef __ASSEMBLER__
    #include <stdint.h>
#endif

////////////////////////////////////////////////////////////////////////////
// IRQ
////////////////////////////////////////////////////////////////////////////

// The OCB interrupt controller consists of 2 x 32-bit controllers.  Unlike
// PPC ASICs, the OCB controllers are _not_ cascaded.  The combined
// controllers are presented to the application as if there were a single
// 64-bit interrupt controller, while the code underlying the abstraction
// manipulates the 2 x 32-bit controllers independently.
//
// Note that the bits named *RESERVED* are actually implemented in the
// controller, but the interrupt input is tied low. That means they can also
// be used as IPI targets. Logical bits 32..63 are not implemented.
#define OCCHW_IRQ_DEBUGGER                       0 /* 0x0 */
#define OCCHW_IRQ_TRACE_TRIGGER_1                1 /* 0x1 */
#define OCCHW_IRQ_RESERVED_2                     2 /* 0x2 */
#define OCCHW_IRQ_PBA_ERROR                      3 /* 0x3 */
#define OCCHW_IRQ_GPE0_ERROR                     4 /* 0x4 */
#define OCCHW_IRQ_GPE1_ERROR                     5 /* 0x5 */
#define OCCHW_IRQ_CHECK_STOP_PPC405              6 /* 0x6 */
#define OCCHW_IRQ_EXTERNAL_TRAP                  7 /* 0x7 */
#define OCCHW_IRQ_OCC_TIMER0                     8 /* 0x8 */
#define OCCHW_IRQ_OCC_TIMER1                     9 /* 0x9 */
#define OCCHW_IRQ_IPI0_HI_PRIORITY              10 /* 0xa */
#define OCCHW_IRQ_IPI1_HI_PRIORITY              11 /* 0xb */
#define OCCHW_IRQ_IPI4_HI_PRIORITY              12 /* 0xc */
#define OCCHW_IRQ_I2CM_INTR                     13 /* 0xd */
#define OCCHW_IRQ_IPI_SCOM                      14 /* 0xe */
#define OCCHW_IRQ_RESERVED_15                   15 /* 0xf */
#define OCCHW_IRQ_PBAX_OCC_SEND                 16 /* 0x10 */
#define OCCHW_IRQ_PBAX_OCC_PUSH0                17 /* 0x11 */
#define OCCHW_IRQ_PBAX_OCC_PUSH1                18 /* 0x12 */
#define OCCHW_IRQ_PBA_BCDE_ATTN                 19 /* 0x13 */
#define OCCHW_IRQ_PBA_BCUE_ATTN                 20 /* 0x14 */
#define OCCHW_IRQ_STRM0_PULL                    21 /* 0x15 */
#define OCCHW_IRQ_STRM0_PUSH                    22 /* 0x16 */
#define OCCHW_IRQ_STRM1_PULL                    23 /* 0x17 */
#define OCCHW_IRQ_STRM1_PUSH                    24 /* 0x18 */
#define OCCHW_IRQ_STRM2_PULL                    25 /* 0x19 */
#define OCCHW_IRQ_STRM2_PUSH                    26 /* 0x1a */
#define OCCHW_IRQ_STRM3_PULL                    27 /* 0x1b */
#define OCCHW_IRQ_STRM3_PUSH                    28 /* 0x1c */
#define OCCHW_IRQ_IPI0_LO_PRIORITY              29 /* 0x1d */
#define OCCHW_IRQ_IPI1_LO_PRIORITY              30 /* 0x1e */
#define OCCHW_IRQ_IPI4_LO_PRIORITY              31 /* 0x1f */
#define OCCHW_IRQ_TRACE_TRIGGER_32              32 /* 0x32 */
#define OCCHW_IRQ_OCC_ERROR                     33 /* 0x33 */
#define OCCHW_IRQ_GPE2_ERROR                    34 /* 0x34 */
#define OCCHW_IRQ_GPE3_ERROR                    35 /* 0x35 */
#define OCCHW_IRQ_CHECK_STOP_GPE2               36 /* 0x36 */
#define OCCHW_IRQ_CHECK_STOP_GPE3               37 /* 0x37 */
#define OCCHW_IRQ_OCC_MALF_ALERT                38 /* 0x38 */
#define OCCHW_IRQ_PVREF_ERROR                   39 /* 0x39 */
#define OCCHW_IRQ_IPI2_HI_PRIORITY              40 /* 0x40 */
#define OCCHW_IRQ_IPI3_HI_PRIORITY              41 /* 0x41 */
#define OCCHW_IRQ_DEBUG_TRIGGER                 42 /* 0x42 */
#define OCCHW_IRQ_GPE3_FUNCTION_TRIGGER         43 /* 0x43 */
#define OCCHW_IRQ_PBAX_PGPE_ATTN                44 /* 0x44 */
#define OCCHW_IRQ_PBAX_PGPE_PUSH0               45 /* 0x45 */
#define OCCHW_IRQ_PBAX_PGPE_PUSH1               46 /* 0x46 */
#define OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     47 /* 0x47 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_0            48 /* 0x48 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_1            49 /* 0x49 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_2            50 /* 0x50 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_3            51 /* 0x51 */
#define OCCHW_IRQ_PSPI_DOORBELL_0               52 /* 0x52 */
#define OCCHW_IRQ_PSPI_DOORBELL_1               53 /* 0x53 */
#define OCCHW_IRQ_PSPI_DOORBELL_2               54 /* 0x54 */
#define OCCHW_IRQ_PSPI_DOORBELL_3               55 /* 0x55 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP0        56 /* 0x56 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP1        57 /* 0x57 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP2        58 /* 0x58 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP3        59 /* 0x59 */
#define OCCHW_IRQ_RESERVED_60                   60 /* 0x60 */
#define OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     61 /* 0x61 */
#define OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     62 /* 0x62 */
#define OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     63 /* 0x63 */
/// This constant is used to define the size of the table of interrupt handler
/// structures as well as a limit for error checking.  The entire 64-bit
/// vector is now in use.

#define OCCHW_IRQS 64

// Please keep the string definitions up-to-date as they are used for
// reporting in the Simics simulation.

#define OCCHW_IRQ_STRINGS(var)                      \
    const char* var[OCCHW_IRQS] = {                 \
                                                    "OCCHW_IRQ_DEBUGGER",                       \
                                                    "OCCHW_IRQ_TRACE_TRIGGER_1",                \
                                                    "OCCHW_IRQ_RESERVED_2",                     \
                                                    "OCCHW_IRQ_PBA_ERROR",                      \
                                                    "OCCHW_IRQ_GPE0_ERROR",                     \
                                                    "OCCHW_IRQ_GPE1_ERROR",                     \
                                                    "OCCHW_IRQ_CHECK_STOP_PPC405",              \
                                                    "OCCHW_IRQ_EXTERNAL_TRAP",                  \
                                                    "OCCHW_IRQ_OCC_TIMER0",                     \
                                                    "OCCHW_IRQ_OCC_TIMER1",                     \
                                                    "OCCHW_IRQ_IPI0_HI_PRIORITY",               \
                                                    "OCCHW_IRQ_IPI1_HI_PRIORITY",               \
                                                    "OCCHW_IRQ_IPI4_HI_PRIORITY",               \
                                                    "OCCHW_IRQ_I2CM_INTR",                      \
                                                    "OCCHW_IRQ_IPI_SCOM",                       \
                                                    "OCCHW_IRQ_RESERVED_15",                    \
                                                    "OCCHW_IRQ_PBAX_OCC_SEND",                  \
                                                    "OCCHW_IRQ_PBAX_OCC_PUSH0",                 \
                                                    "OCCHW_IRQ_PBAX_OCC_PUSH1",                 \
                                                    "OCCHW_IRQ_PBA_BCDE_ATTN",                  \
                                                    "OCCHW_IRQ_PBA_BCUE_ATTN",                  \
                                                    "OCCHW_IRQ_STRM0_PULL",                     \
                                                    "OCCHW_IRQ_STRM0_PUSH",                     \
                                                    "OCCHW_IRQ_STRM1_PULL",                     \
                                                    "OCCHW_IRQ_STRM1_PUSH",                     \
                                                    "OCCHW_IRQ_STRM2_PULL",                     \
                                                    "OCCHW_IRQ_STRM2_PUSH",                     \
                                                    "OCCHW_IRQ_STRM3_PULL",                     \
                                                    "OCCHW_IRQ_STRM3_PUSH",                     \
                                                    "OCCHW_IRQ_IPI0_LO_PRIORITY",               \
                                                    "OCCHW_IRQ_IPI1_LO_PRIORITY",               \
                                                    "OCCHW_IRQ_IPI4_LO_PRIORITY",               \
                                                    "OCCHW_IRQ_TRACE_TRIGGER_32",               \
                                                    "OCCHW_IRQ_OCC_ERROR",                      \
                                                    "OCCHW_IRQ_GPE2_ERROR",                     \
                                                    "OCCHW_IRQ_GPE3_ERROR",                     \
                                                    "OCCHW_IRQ_CHECK_STOP_GPE2",                \
                                                    "OCCHW_IRQ_CHECK_STOP_GPE3",                \
                                                    "OCCHW_IRQ_OCC_MALF_ALERT",                 \
                                                    "OCCHW_IRQ_PVREF_ERROR",                    \
                                                    "OCCHW_IRQ_IPI2_HI_PRIORITY",               \
                                                    "OCCHW_IRQ_IPI3_HI_PRIORITY",               \
                                                    "OCCHW_IRQ_DEBUG_TRIGGER",                  \
                                                    "OCCHW_IRQ_GPE3_FUNCTION_TRIGGER",          \
                                                    "OCCHW_IRQ_PBAX_PGPE_ATTN",                 \
                                                    "OCCHW_IRQ_PBAX_PGPE_PUSH0",                \
                                                    "OCCHW_IRQ_PBAX_PGPE_PUSH1",                \
                                                    "OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR",      \
                                                    "OCCHW_IRQ_PSPI_MSGDOORBELL_0",             \
                                                    "OCCHW_IRQ_PSPI_MSGDOORBELL_1",             \
                                                    "OCCHW_IRQ_PSPI_MSGDOORBELL_2",             \
                                                    "OCCHW_IRQ_PSPI_MSGDOORBELL_3",             \
                                                    "OCCHW_IRQ_DOORBELL_0",                     \
                                                    "OCCHW_IRQ_DOORBELL_1",                     \
                                                    "OCCHW_IRQ_DOORBELL_2",                     \
                                                    "OCCHW_IRQ_DOORBELL_3",                     \
                                                    "OCCHW_IRQ_PSPI_INTR_PENDING_GRP0",         \
                                                    "OCCHW_IRQ_PSPI_INTR_PENDING_GRP1",         \
                                                    "OCCHW_IRQ_PSPI_INTR_PENDING_GRP2",         \
                                                    "OCCHW_IRQ_PSPI_INTR_PENDING_GRP3",         \
                                                    "OCCHW_IRQ_RESERVED_28",                    \
                                                    "OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE",      \
                                                    "OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE",      \
                                                    "OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE",      \
                                  };


/// Routing codes for OCB interrupts
#define OCCHW_IRQ_TARGET_ID_405_NONCRIT     0
#define OCCHW_IRQ_TARGET_ID_405_CRIT        1
#define OCCHW_IRQ_TARGET_ID_405_UNCOND      2
#define OCCHW_IRQ_TARGET_ID_405_DEBUG       3
#define OCCHW_IRQ_TARGET_ID_GPE0            4
#define OCCHW_IRQ_TARGET_ID_GPE1            5
#define OCCHW_IRQ_TARGET_ID_GPE2            6
#define OCCHW_IRQ_TARGET_ID_GPE3            7

// OCB interrupt type values (level or edge)
#define OCCHW_IRQ_TYPE_LEVEL            0
#define OCCHW_IRQ_TYPE_EDGE             1

// OCB interrupt polarity values (high or low, rising falling)
#define OCCHW_IRQ_POLARITY_LO           0
#define OCCHW_IRQ_POLARITY_FALLING      0
#define OCCHW_IRQ_POLARITY_HI           1
#define OCCHW_IRQ_POLARITY_RISING       1

// OCB interrupt mask values (masked or enabled)
#define OCCHW_IRQ_MASKED                0
#define OCCHW_IRQ_ENABLED               1

// Note: All standard-product IPI uses are declared here to avoid conflicts
// Validation- and lab-only IPI uses are documented in validation.h

/// The deferred callback queue interrupt
///
/// This IPI is reserved for use of the async deferred callback mechanism.
/// This IPI is used by both critical and noncritical async handlers to
/// activate the deferred callback mechanism.
#define OCCHW_IRQ_ASYNC_IPI OCCHW_IRQ_IPI4_LO_PRIORITY


#ifndef __ASSEMBLER__

/// This expression recognizes only those IRQ numbers that have named
/// (non-reserved) interrupts in the OCB interrupt controller.

// There are so many invalid interrupts now that it's a slight improvement in
// code size to let the compiler optimize the invalid IRQs to a bit mask for
// the comparison.

#define OCCHW_IRQ_VALID(irq) \
    ({unsigned __irq = (unsigned)(irq); \
        ((__irq < OCCHW_IRQS) &&                                  \
         ((OCCHW_IRQ_MASK64(__irq) &                              \
           (OCCHW_IRQ_MASK64(OCCHW_IRQ_RESERVED_60))) == 0));})

/// This is a 32-bit mask, with big-endian bit (irq % 32) set.
#define OCCHW_IRQ_MASK32(irq) (((uint32_t)0x80000000) >> ((irq) % 32))

/// This is a 64-bit mask, with big-endian bit 'irq' set.
#define OCCHW_IRQ_MASK64(irq) (0x8000000000000000ull >> (irq))

#else

//assembler version of OCCHW_IRQ_MASK32
#define OCCHW_IRQ_MASK32(irq) ((0x80000000) >> ((irq) % 32))

#endif  /* __ASSEMBLER__ */

#ifndef __ASSEMBLER__

    // These macros select OCB interrupt controller registers based on the IRQ
    // number.

    #define OCCHW_OIMR_CLR(irq) (((irq) & 0x20) ? OCB_OIMR1_CLR : OCB_OIMR0_CLR)
    #define OCCHW_OIMR_OR(irq)  (((irq) & 0x20) ? OCB_OIMR1_OR  : OCB_OIMR0_OR)

    #define OCCHW_OISR(irq)     (((irq) & 0x20) ? OCB_OISR1     : OCB_OISR0)
    #define OCCHW_OISR_CLR(irq) (((irq) & 0x20) ? OCB_OISR1_CLR : OCB_OISR0_CLR)
    #define OCCHW_OISR_OR(irq)  (((irq) & 0x20) ? OCB_OISR1_OR  : OCB_OISR0_OR)

    #define OCCHW_OIEPR(irq)     (((irq) & 0x20) ? OCB_OIEPR1     : OCB_OIEPR0)
    #define OCCHW_OIEPR_OR(irq)  (((irq) & 0x20) ? OCB_OIEPR1_OR  : OCB_OIEPR0_OR)
    #define OCCHW_OIEPR_CLR(irq) (((irq) & 0x20) ? OCB_OIEPR1_CLR : OCB_OIEPR0_CLR)
    #define OCCHW_OITR(irq)      (((irq) & 0x20) ? OCB_OITR1      : OCB_OITR0)
    #define OCCHW_OITR_OR(irq)   (((irq) & 0x20) ? OCB_OITR1_OR   : OCB_OITR0_OR)
    #define OCCHW_OITR_CLR(irq)  (((irq) & 0x20) ? OCB_OITR1_CLR  : OCB_OITR0_CLR)

    #define OCCHW_OIRRA(irq)     (((irq) & 0x20) ? OCB_OIRR1A     : OCB_OIRR0A)
    #define OCCHW_OIRRA_OR(irq)  (((irq) & 0x20) ? OCB_OIRR1A_OR  : OCB_OIRR0A_OR)
    #define OCCHW_OIRRA_CLR(irq) (((irq) & 0x20) ? OCB_OIRR1A_CLR : OCB_OIRR0A_CLR)
    #define OCCHW_OIRRB(irq)     (((irq) & 0x20) ? OCB_OIRR1B     : OCB_OIRR0B)
    #define OCCHW_OIRRB_OR(irq)  (((irq) & 0x20) ? OCB_OIRR1B_OR  : OCB_OIRR0B_OR)
    #define OCCHW_OIRRB_CLR(irq) (((irq) & 0x20) ? OCB_OIRR1B_CLR : OCB_OIRR0B_CLR)
    #define OCCHW_OIRRC(irq)     (((irq) & 0x20) ? OCB_OIRR1C     : OCB_OIRR0C)
    #define OCCHW_OIRRC_OR(irq)  (((irq) & 0x20) ? OCB_OIRR1C_OR  : OCB_OIRR0C_OR)
    #define OCCHW_OIRRC_CLR(irq) (((irq) & 0x20) ? OCB_OIRR1C_CLR : OCB_OIRR0C_CLR)
#endif  /* __ASSEMBLER__ */

#endif  /* __OCCHW_INTERRUPTS_H__ */
