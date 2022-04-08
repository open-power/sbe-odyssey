/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/tcc/occhw_interrupts.h $ */
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
#ifndef __OCCHW_INTERRUPTS_H__
#define __OCCHW_INTERRUPTS_H__

/// \file occhw_interrupts.h
/// \brief Interrupt assignments and macros for the TCC
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
// Note that the bits named *SPARE* are actually implemented in the
// controller, but the interrupt input is tied low. That means they can also
// be used as IPI targets. Logical bits 32..63 are not implemented.
#define TCCHW_IRQ_DEBUGGER                       0 /*  0 */
#define TCCHW_IRQ_TRACE_TRIGGER_1                1 /*  1 */
#define TCCHW_IRQ_CE0_ERROR                      2 /*  2 */
#define TCCHW_IRQ_CE1_ERROR                      3 /*  3 */
#define TCCHW_IRQ_CHECK_STOP_CE0                 4 /*  4 */
#define TCCHW_IRQ_CHECK_STOP_CE1                 5 /*  5 */
#define TCCHW_IRQ_PBA_ERROR                      6 /*  6 */
#define TCCHW_IRQ_SPARE_7                        7 /*  7 */
#define TCCHW_IRQ_TCC_TIMER0                     8 /*  8 */
#define TCCHW_IRQ_TCC_TIMER1                     9 /*  9 */
#define TCCHW_IRQ_IPI0_HI_PRIORITY              10 /* 10 */
#define TCCHW_IRQ_IPI1_HI_PRIORITY              11 /* 11 */
#define TCCHW_IRQ_SPARE_12                      12 /* 12 */
#define TCCHW_IRQ_SPARE_13                      13 /* 13 */
#define TCCHW_IRQ_SPARE_14                      14 /* 14 */
#define TCCHW_IRQ_SPARE_15                      15 /* 15 */
#define TCCHW_IRQ_PBAX_TCC_SEND_ATTN            16 /* 16 */
#define TCCHW_IRQ_PBAX_TCC_PUSH0                17 /* 17 */
#define TCCHW_IRQ_PBAX_TCC_PUSH1                18 /* 18 */
#define TCCHW_IRQ_PBA_BCDE_ATTN                 19 /* 19 */
#define TCCHW_IRQ_PBA_BCUE_ATTN                 20 /* 20 */
#define TCCHW_IRQ_STRM0_PULL                    21 /* 21 */
#define TCCHW_IRQ_STRM0_PUSH                    22 /* 22 */
#define TCCHW_IRQ_STRM1_PULL                    23 /* 23 */
#define TCCHW_IRQ_STRM1_PUSH                    24 /* 24 */
#define TCCHW_IRQ_STRM2_PULL                    25 /* 25 */
#define TCCHW_IRQ_STRM2_PUSH                    26 /* 26 */
#define TCCHW_IRQ_STRM3_PULL                    27 /* 27 */
#define TCCHW_IRQ_STRM3_PUSH                    28 /* 28 */
#define TCCHW_IRQ_IPI0_LO_PRIORITY              29 /* 29 */
#define TCCHW_IRQ_IPI1_LO_PRIORITY              30 /* 30 */
#define TCCHW_IRQ_SPARE_31                      31 /* 31 */
#define TCCHW_IRQ_TRACE_TRIGGER_32              32 /* 32 */
#define TCCHW_IRQ_TCC_ERROR                     33 /* 33 */
#define TCCHW_IRQ_CE2_ERROR                     34 /* 34 */
#define TCCHW_IRQ_CE3_ERROR                     35 /* 35 */
#define TCCHW_IRQ_CHECK_STOP_CE2                36 /* 36 */
#define TCCHW_IRQ_CHECK_STOP_CE3                37 /* 37 */
#define TCCHW_IRQ_TCC_MALF_ALERT                38 /* 38 */
#define TCCHW_IRQ_PVREF_ERROR                   39 /* 39 */
#define TCCHW_IRQ_IPI2_HI_PRIORITY              40 /* 40 */
#define TCCHW_IRQ_IPI3_HI_PRIORITY              41 /* 41 */
#define TCCHW_IRQ_DEBUG_TRIGGER                 42 /* 42 */
#define TCCHW_IRQ_IPI2_LO_PRIORITY              43 /* 43 */
#define TCCHW_IRQ_PBAX_HCODE_SEND_ATTN          44 /* 44 */
#define TCCHW_IRQ_PBAX_HCODE_PUSH0              45 /* 45 */
#define TCCHW_IRQ_PBAX_HCODE_PUSH1              46 /* 46 */
#define TCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     47 /* 47 */
#define TCCHW_IRQ_PSPI_MSGDOORBELL_0            48 /* 48 */
#define TCCHW_IRQ_PSPI_MSGDOORBELL_1            49 /* 49 */
#define TCCHW_IRQ_PSPI_MSGDOORBELL_2            50 /* 50 */
#define TCCHW_IRQ_PSPI_MSGDOORBELL_3            51 /* 51 */
#define TCCHW_IRQ_PSPI_DOORBELL_0               52 /* 52 */
#define TCCHW_IRQ_PSPI_DOORBELL_1               53 /* 53 */
#define TCCHW_IRQ_PSPI_DOORBELL_2               54 /* 54 */
#define TCCHW_IRQ_PSPI_DOORBELL_3               55 /* 55 */
#define TCCHW_IRQ_PCB_INTR_PENDING_GRP0         56 /* 56 */
#define TCCHW_IRQ_PCB_INTR_PENDING_GRP1         57 /* 57 */
#define TCCHW_IRQ_PCB_INTR_PENDING_GRP2         58 /* 58 */
#define TCCHW_IRQ_PCB_INTR_PENDING_GRP3         59 /* 59 */
#define TCCHW_IRQ_IPI3_LO_PRIORITY              60 /* 60 */
#define TCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     61 /* 61 */
#define TCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     62 /* 62 */
#define TCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     63 /* 63 */
/// This constant is used to define the size of the table of interrupt handler
/// structures as well as a limit for error checking.  The entire 64-bit
/// vector is now in use.

#define OCCHW_IRQS 64

// Please keep the string definitions up-to-date as they are used for
// reporting in the Simics simulation.

#define OCCHW_IRQ_STRINGS(var)                      \
    const char* var[OCCHW_IRQS] = {                 \
                                                    "TCCHW_IRQ_DEBUGGER",                       \
                                                    "TCCHW_IRQ_TRACE_TRIGGER_1",                \
                                                    "TCCHW_IRQ_CE0_ERROR",                      \
                                                    "TCCHW_IRQ_CE1_ERROR",                      \
                                                    "TCCHW_IRQ_CHECK_STOP_CE0",                 \
                                                    "TCCHW_IRQ_CHECK_STOP_CE1",                 \
                                                    "TCCHW_IRQ_PBA_ERROR",                      \
                                                    "TCCHW_IRQ_SPARE_7",                        \
                                                    "TCCHW_IRQ_TCC_TIMER0",                     \
                                                    "TCCHW_IRQ_TCC_TIMER1",                     \
                                                    "TCCHW_IRQ_IPI0_HI_PRIORITY",               \
                                                    "TCCHW_IRQ_IPI1_HI_PRIORITY",               \
                                                    "TCCHW_IRQ_SPARE_12",                       \
                                                    "TCCHW_IRQ_SPARE_13",                       \
                                                    "TCCHW_IRQ_SPARE_14",                       \
                                                    "TCCHW_IRQ_SPARE_15",                       \
                                                    "TCCHW_IRQ_TCC_SEND_ATTN",                  \
                                                    "TCCHW_IRQ_TCC_PUSH0",                      \
                                                    "TCCHW_IRQ_TCC_PUSH1"                       \
                                                    "TCCHW_IRQ_PBA_BCDE_ATTN",                  \
                                                    "TCCHW_IRQ_PBA_BCUE_ATTN",                  \
                                                    "TCCHW_IRQ_STRM0_PULL",                     \
                                                    "TCCHW_IRQ_STRM0_PUSH",                     \
                                                    "TCCHW_IRQ_STRM1_PULL",                     \
                                                    "TCCHW_IRQ_STRM1_PUSH",                     \
                                                    "TCCHW_IRQ_STRM2_PULL",                     \
                                                    "TCCHW_IRQ_STRM2_PUSH",                     \
                                                    "TCCHW_IRQ_STRM3_PULL",                     \
                                                    "TCCHW_IRQ_STRM3_PUSH",                     \
                                                    "TCCHW_IRQ_IPI0_LO_PRIORITY",               \
                                                    "TCCHW_IRQ_IPI1_LO_PRIORITY",               \
                                                    "TCCHW_IRQ_SPARE_31",                       \
                                                    "TCCHW_IRQ_TRACE_TRIGGER_32",               \
                                                    "TCCHW_IRQ_TCC_ERROR",                      \
                                                    "TCCHW_IRQ_CE2_ERROR",                      \
                                                    "TCCHW_IRQ_CE3_ERROR",                      \
                                                    "TCCHW_IRQ_CHECK_STOP_CE2",                 \
                                                    "TCCHW_IRQ_CHECK_STOP_CE3",                 \
                                                    "TCCHW_IRQ_TCC_MALF_ALERT",                 \
                                                    "TCCHW_IRQ_PVREF_ERROR",                    \
                                                    "TCCHW_IRQ_IPI2_HI_PRIORITY",               \
                                                    "TCCHW_IRQ_IPI3_HI_PRIORITY",               \
                                                    "TCCHW_IRQ_DEBUG_TRIGGER",                  \
                                                    "TCCHW_IRQ_IPI2_LO_PRIORITY",               \
                                                    "TCCHW_IRQ_PBAX_HCODE_SEND_ATTN",           \
                                                    "TCCHW_IRQ_PBAX_HCODE_PUSH0",               \
                                                    "TCCHW_IRQ_PBAX_HCODE_PUSH1",               \
                                                    "TCCHW_IRQ_PBA_OVERCURRENT_INDICATOR",      \
                                                    "TCCHW_IRQ_PSPI_MSGDOORBELL_0",             \
                                                    "TCCHW_IRQ_PSPI_MSGDOORBELL_1",             \
                                                    "TCCHW_IRQ_PSPI_MSGDOORBELL_2",             \
                                                    "TCCHW_IRQ_PSPI_MSGDOORBELL_3",             \
                                                    "TCCHW_IRQ_PSPI_DOORBELL_0",                \
                                                    "TCCHW_IRQ_PSPI_DOORBELL_1",                \
                                                    "TCCHW_IRQ_PSPI_DOORBELL_2",                \
                                                    "TCCHW_IRQ_PSPI_DOORBELL_3",                \
                                                    "TCCHW_IRQ_PCB_INTR_PENDING_GRP0",          \
                                                    "TCCHW_IRQ_PCB_INTR_PENDING_GRP1",          \
                                                    "TCCHW_IRQ_PCB_INTR_PENDING_GRP2",          \
                                                    "TCCHW_IRQ_PCB_INTR_PENDING_GRP3",          \
                                                    "TCCHW_IRQ_IPI3_LO_PRIORITY",               \
                                                    "TCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE",      \
                                                    "TCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE",      \
                                                    "TCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE",      \
                                  };


/// Routing codes for OCB interrupts (starting at 0x4 to not mess up irq route macro assumptions)
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
/// (OCC side only, but keeping here for future reference.)
///
/// This IPI is reserved for use by the async deferred callback mechanism.
/// It is used by both critical and noncritical async handlers to
/// activate the deferred callback mechanism.
//#define OCCHW_IRQ_ASYNC_IPI OCCHW_IRQ_IPI4_LO_PRIORITY


//
// CMO: Since we'll never use the below 64-bit MASK defs for IOTA/CEs/TCC,
//      it's probably OK to delete:
//      - I had initially modified it, but now I think we should just delete.
//      - Commenting out for now.
//      - Please advise...
#if 0

#ifndef __ASSEMBLER__

/// This expression recognizes only those IRQ numbers that have named
/// (non-reserved) interrupts in the OCB interrupt controller.

// There are so many invalid interrupts now that it's a slight improvement in
// code size to let the compiler optimize the invalid IRQs to a bit mask for
// the comparison.
#define TCCHW_VALID_IRQ_MASK \
    ( ~( OCCHW_IRQ_MASK64(TCCHW_IRQ_SPARE_7)  | \
         OCCHW_IRQ_MASK64(TCCHW_IRQ_SPARE_12) | \
         OCCHW_IRQ_MASK64(TCCHW_IRQ_SPARE_13) | \
         OCCHW_IRQ_MASK64(TCCHW_IRQ_SPARE_14) | \
         OCCHW_IRQ_MASK64(TCCHW_IRQ_SPARE_15) ) )

#define OCCHW_IRQ_VALID(irq) \
    ( { unsigned __irq = (unsigned)(irq); \
        ( OCCHW_IRQ_MASK64(__irq) & TCCHW_VALID_IRQ_MASK); } )

/// This is a 64-bit mask, with big-endian bit 'irq' set.
#define OCCHW_IRQ_MASK64(irq) (0x8000000000000000ull >> (irq))

#endif  /* __ASSEMBLER__ */

#endif // CMO: End of proposed delete "MASK" section we will never use.

/// This is a 32-bit mask, with big-endian bit (irq % 32) set.
#define OCCHW_IRQ_MASK32(irq) (0x80000000 >> ((irq) % 32))

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
