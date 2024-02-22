/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/occhw_interrupts.h $ */
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
#ifndef __OCCHW_INTERRUPTS_H__
#define __OCCHW_INTERRUPTS_H__

/// \file occhw_interrupts.h
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
// Note that the bits named *SPARE* are actually implemented in the
// controller, but the interrupt input is tied low. That means they can also
// be used as IPI targets. Logical bits 32..63 are not implemented.
#define OCCHW_IRQ_DEBUGGER                       0 /*  0 */
#define OCCHW_IRQ_TRACE_TRIGGER_1                1 /*  1 */
#define OCCHW_IRQ_SPARE_2                        2 /*  2 */
#define OCCHW_IRQ_PBA_ERROR                      3 /*  3 */
#define OCCHW_IRQ_GPE0_ERROR                     4 /*  4 */
#define OCCHW_IRQ_GPE1_ERROR                     5 /*  5 */
#define OCCHW_IRQ_CHECK_STOP_PPC405              6 /*  6 */
#define OCCHW_IRQ_EXTERNAL_TRAP                  7 /*  7 */
#define OCCHW_IRQ_OCC_TIMER0                     8 /*  8 */
#define OCCHW_IRQ_OCC_TIMER1                     9 /*  9 */
#define OCCHW_IRQ_IPI0_HI_PRIORITY              10 /* 10 */
#define OCCHW_IRQ_IPI1_HI_PRIORITY              11 /* 11 */
#define OCCHW_IRQ_IPI4_HI_PRIORITY              12 /* 12 */
#define OCCHW_IRQ_I2CM_INTR                     13 /* 13 */
#define OCCHW_IRQ_IPI_SCOM                      14 /* 14 */
#define OCCHW_IRQ_SPARE_15                      15 /* 15 */
#define OCCHW_IRQ_PBAX_OCC_SEND_ATTN            16 /* 16 */
#define OCCHW_IRQ_PBAX_OCC_PUSH0                17 /* 17 */
#define OCCHW_IRQ_PBAX_OCC_PUSH1                18 /* 18 */
#define OCCHW_IRQ_PBA_BCE0_ATTN                 19 /* 19 */
#define OCCHW_IRQ_PBA_BCE1_ATTN                 20 /* 20 */
#define OCCHW_IRQ_OCC_STRM0_PULL                21 /* 21 */
#define OCCHW_IRQ_OCC_STRM0_PUSH                22 /* 22 */
#define OCCHW_IRQ_OCC_STRM1_PULL                23 /* 23 */
#define OCCHW_IRQ_OCC_STRM1_PUSH                24 /* 24 */
#define OCCHW_IRQ_OCC_STRM2_PULL                25 /* 25 */
#define OCCHW_IRQ_OCC_STRM2_PUSH                26 /* 26 */
#define OCCHW_IRQ_OCC_STRM3_PULL                27 /* 27 */
#define OCCHW_IRQ_OCC_STRM3_PUSH                28 /* 28 */
#define OCCHW_IRQ_IPI0_LO_PRIORITY              29 /* 29 */
#define OCCHW_IRQ_IPI1_LO_PRIORITY              30 /* 30 */
#define OCCHW_IRQ_IPI4_LO_PRIORITY              31 /* 31 */
#define OCCHW_IRQ_TRACE_TRIGGER_32              32 /* 32 */
#define OCCHW_IRQ_OCC_ERROR                     33 /* 33 */
#define OCCHW_IRQ_GPE2_ERROR                    34 /* 34 */
#define OCCHW_IRQ_GPE3_ERROR                    35 /* 35 */
#define OCCHW_IRQ_CHECK_STOP_GPE2               36 /* 36 */
#define OCCHW_IRQ_CHECK_STOP_GPE3               37 /* 37 */
#define OCCHW_IRQ_OCC_MALF_ALERT                38 /* 38 */
#define OCCHW_IRQ_PVREF_ERROR                   39 /* 39 */
#define OCCHW_IRQ_IPI2_HI_PRIORITY              40 /* 40 */
#define OCCHW_IRQ_IPI3_HI_PRIORITY              41 /* 41 */
#define OCCHW_IRQ_DEBUG_TRIGGER                 42 /* 42 */
#define OCCHW_IRQ_AVS_PENDING                   43 /* 43 */
#define OCCHW_IRQ_PBAX_PGPE_SEND_ATTN           44 /* 44 */
#define OCCHW_IRQ_PBAX_PGPE_PUSH0               45 /* 45 */
#define OCCHW_IRQ_PBAX_PGPE_PUSH1               46 /* 46 */
#define OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     47 /* 47 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_0            48 /* 48 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_1            49 /* 49 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_2            50 /* 50 */
#define OCCHW_IRQ_PSPI_MSGDOORBELL_3            51 /* 51 */
#define OCCHW_IRQ_PSPI_DOORBELL_0               52 /* 52 */
#define OCCHW_IRQ_PSPI_DOORBELL_1               53 /* 53 */
#define OCCHW_IRQ_PSPI_DOORBELL_2               54 /* 54 */
#define OCCHW_IRQ_PSPI_DOORBELL_3               55 /* 55 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP0        56 /* 56 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP1        57 /* 57 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP2        58 /* 58 */
#define OCCHW_IRQ_PSPI_INTR_PENDING_GRP3        59 /* 59 */
#define OCCHW_IRQ_IPI3_LO_PRIORITY              60 /* 60 */
#define OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     61 /* 61 */
#define OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     62 /* 62 */
#define OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     63 /* 63 */
/// This constant is used to define the size of the table of interrupt handler
/// structures as well as a limit for error checking.  The entire 64-bit
/// vector is now in use.

#define OCCHW_IRQS 64
#define EXTERNAL_IRQS OCCHW_IRQS

// Please keep the string definitions up-to-date as they are used for
// reporting in the Simics simulation.

#define OCCHW_IRQ_STRINGS(var)                      \
    const char* var[OCCHW_IRQS] = {                 \
                                                    "OCCHW_IRQ_DEBUGGER",                       \
                                                    "OCCHW_IRQ_TRACE_TRIGGER_1",                \
                                                    "OCCHW_IRQ_SPARE_2",                        \
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
                                                    "OCCHW_IRQ_SPARE_15",                       \
                                                    "OCCHW_IRQ_PBAX_OCC_SEND",                  \
                                                    "OCCHW_IRQ_PBAX_OCC_PUSH0",                 \
                                                    "OCCHW_IRQ_PBAX_OCC_PUSH1",                 \
                                                    "OCCHW_IRQ_PBA_BCE0_ATTN",                  \
                                                    "OCCHW_IRQ_PBA_BCE1_ATTN",                  \
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
                                                    "OCCHW_IRQ_AVS_PENDING",                    \
                                                    "OCCHW_IRQ_PBAX_PGPE_SEND_ATTN",            \
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
                                                    "OCCHW_IRQ_IPI3_LO_PRIORITY",               \
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

// Note: All standard-product IPI uses are declared here to avoid conflicts
// Validation- and lab-only IPI uses are documented in validation.h

/// The deferred callback queue interrupt
///
/// This IPI is reserved for use of the async deferred callback mechanism.
/// This IPI is used by both critical and noncritical async handlers to
/// activate the deferred callback mechanism.
#define OCCHW_IRQ_ASYNC_IPI OCCHW_IRQ_IPI4_LO_PRIORITY


#ifndef __ASSEMBLER__

// This is a 64-bit mask, with big-endian bit 'irq' set.
#define OCCHW_IRQ_MASK64(irq) (0x8000000000000000ull >> (irq))

/// This expression recognizes only those IRQ numbers that have named
/// (non-reserved) interrupts in the OCB interrupt controller.

// There are so many invalid interrupts now that it's a slight improvement in
// code size to let the compiler optimize the invalid IRQs to a bit mask for
// the comparison.
#define OCCHW_VALID_IRQ_MASK \
    ( ~( OCCHW_IRQ_MASK64(OCCHW_IRQ_SPARE_2)  | \
         OCCHW_IRQ_MASK64(OCCHW_IRQ_SPARE_15) ) )

#define PLATHW_IRQ_VALID(irq) \
    ( { unsigned __irq = (unsigned)(irq); \
        ( OCCHW_IRQ_MASK64(__irq) & OCCHW_VALID_IRQ_MASK ); } )

#endif  /* __ASSEMBLER__ */

#endif  /* __OCCHW_INTERRUPTS_H__ */
