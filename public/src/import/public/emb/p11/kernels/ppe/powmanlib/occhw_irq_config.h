/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/occhw_irq_config.h $ */
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
#ifndef __OCCHW_IRQ_CONFIG_H__
#define __OCCHW_IRQ_CONFIG_H__

/// \file occhw_irq_config.h
/// \brief Contains data and macros pertaining to external interrupt routing
///        and configuration for processors running in the OCC complex.
///


/// Number of OCC instance ID's that can be read from the PPE42 PIR and used in IPCs.
#define PLATHW_MAX_INSTANCES 5


#ifndef __ASSEMBLER__
    /// These globals are statically initialized elsewhere
    extern uint64_t g_ext_irqs_routeA;
    extern uint64_t g_ext_irqs_routeB;
    extern uint64_t g_ext_irqs_routeC;
    extern uint64_t g_ext_irqs_type;
    extern uint64_t g_ext_irqs_owned;
    extern uint64_t g_ext_irqs_polarity;
    extern uint64_t g_ext_irqs_enable;

    #define PLATHW_IRQ_OWNED(irq) ((OCCHW_IRQ_MASK64(irq) & g_ext_irqs_owned) != 0)

#endif

/// Add a pseudo target to indicate that an interrupt is not owned by anyone
#define OCCHW_IRQ_TARGET_ID_NONE    8

/// Default interrupt routing table

#ifdef WDC_ACTING_OCC

#ifndef OCCHW_IRQ_ROUTES
#define OCCHW_IRQ_ROUTES \
    OCCHW_IRQ_DEBUGGER                      OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_TRACE_TRIGGER_1               OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_SPARE_2                       OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PBA_ERROR                     OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_GPE0_ERROR                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_GPE1_ERROR                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_CHECK_STOP_PPC405             OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_EXTERNAL_TRAP                 OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_TIMER0                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_TIMER1                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_IPI0_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_IPI1_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE1 \
    OCCHW_IRQ_IPI4_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_405_CRIT \
    OCCHW_IRQ_I2CM_INTR                     OCCHW_IRQ_TARGET_ID_405_CRIT \
    OCCHW_IRQ_IPI_SCOM                      OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_SPARE_15                      OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PBAX_OCC_SEND_ATTN            OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBAX_OCC_PUSH0                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBAX_OCC_PUSH1                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBA_BCE0_ATTN                 OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBA_BCE1_ATTN                 OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_OCC_STRM0_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM0_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM1_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM1_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM2_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM2_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM3_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM3_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_IPI0_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_IPI1_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE1 \
    OCCHW_IRQ_IPI4_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_TRACE_TRIGGER_32              OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_OCC_ERROR                     OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_GPE2_ERROR                    OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_GPE3_ERROR                    OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_CHECK_STOP_GPE2               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_CHECK_STOP_GPE3               OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_OCC_MALF_ALERT                OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PVREF_ERROR                   OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_IPI2_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_IPI3_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_DEBUG_TRIGGER                 OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_AVS_PENDING                   OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_SEND_ATTN           OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_PUSH0               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_PUSH1               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_PSPI_MSGDOORBELL_0            OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_MSGDOORBELL_1            OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PSPI_MSGDOORBELL_2            OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PSPI_MSGDOORBELL_3            OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_PSPI_DOORBELL_0               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_DOORBELL_1               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_DOORBELL_2               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PSPI_DOORBELL_3               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP0        OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP1        OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP2        OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP3        OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_IPI3_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_GPE0 \

#endif

#else

#ifndef OCCHW_IRQ_ROUTES
#define OCCHW_IRQ_ROUTES \
    OCCHW_IRQ_DEBUGGER                      OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_TRACE_TRIGGER_1               OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_SPARE_2                       OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PBA_ERROR                     OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_GPE0_ERROR                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_GPE1_ERROR                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_CHECK_STOP_PPC405             OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_EXTERNAL_TRAP                 OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_TIMER0                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_TIMER1                    OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_IPI0_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_IPI1_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE1 \
    OCCHW_IRQ_IPI4_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_405_CRIT \
    OCCHW_IRQ_I2CM_INTR                     OCCHW_IRQ_TARGET_ID_405_CRIT \
    OCCHW_IRQ_IPI_SCOM                      OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_SPARE_15                      OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PBAX_OCC_SEND_ATTN            OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBAX_OCC_PUSH0                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBAX_OCC_PUSH1                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBA_BCE0_ATTN                 OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PBA_BCE1_ATTN                 OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_OCC_STRM0_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM0_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM1_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM1_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM2_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM2_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM3_PULL                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_OCC_STRM3_PUSH                OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_IPI0_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE0 \
    OCCHW_IRQ_IPI1_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE1 \
    OCCHW_IRQ_IPI4_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_TRACE_TRIGGER_32              OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_OCC_ERROR                     OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_GPE2_ERROR                    OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_GPE3_ERROR                    OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_CHECK_STOP_GPE2               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_CHECK_STOP_GPE3               OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_OCC_MALF_ALERT                OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PVREF_ERROR                   OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_IPI2_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_IPI3_HI_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_DEBUG_TRIGGER                 OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_AVS_PENDING                   OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_SEND_ATTN           OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_PUSH0               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBAX_PGPE_PUSH1               OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_PSPI_MSGDOORBELL_0            OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_MSGDOORBELL_1            OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_PSPI_MSGDOORBELL_2            OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PSPI_MSGDOORBELL_3            OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_PSPI_DOORBELL_0               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_DOORBELL_1               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_DOORBELL_2               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_DOORBELL_3               OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP0        OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP1        OCCHW_IRQ_TARGET_ID_NONE \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP2        OCCHW_IRQ_TARGET_ID_GPE2 \
    OCCHW_IRQ_PSPI_INTR_PENDING_GRP3        OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_IPI3_LO_PRIORITY              OCCHW_IRQ_TARGET_ID_GPE3 \
    OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_405_NONCRIT \
    OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     OCCHW_IRQ_TARGET_ID_405_NONCRIT \

#endif

#endif


#ifdef __ASSEMBLER__
// *INDENT-OFF*
/// These macros aid in the initialization of the external interrupt globals.  I would
/// prefer to use CPP macros, but they don't support recursive macros which I use to
/// convert the variable number of interrupts that a processor can control into static
/// bitmaps used by __hwmacro_setup() at runtime.


    //helper macro for setting up the irq configuration bitmaps for all entities in the occ complex
    .macro .occhw_irq_config irq_num=-1 irq_type=-1 irq_polarity=-1 irq_mask=-1 parms:vararg
        .if (( \irq_num == -1 ) && ( \irq_type == -1 ) && ( \irq_polarity == -1 ) && ( \irq_mask == -1 ))
            .if ( .ext_irqs_defd != .ext_irqs_owned )
                .error "###### .occhw_irq_config: Missing configuration for one or more interrupts ######"
            .endif

            .section .sdata
            .align 3
            .global g_ext_irqs_type
            .global g_ext_irqs_polarity
            .global g_ext_irqs_enable
            g_ext_irqs_polarity:
            .quad .ext_irqs_polarity
            g_ext_irqs_type:
            .quad .ext_irqs_type
            g_ext_irqs_enable:
            .quad .ext_irqs_enable
        .else
            .if (( \irq_num < 0 ) || ( \irq_num > (OCCHW_IRQS - 1)))
                .error "###### .occhw_irq_config: invalid irq number \irq_num ######"
            .elseif ((.ext_irqs_owned & (1 << ( OCCHW_IRQS - 1 - \irq_num ))) == 0 )
                .error "###### .occhw_irq_config: Attempt to configure unowned irq number \irq_num ######"
            .elseif (.ext_irqs_defd & (1 << ( OCCHW_IRQS - 1 - \irq_num )))
                .error "###### .occhw_irq_config: duplicate definition for irq \irq_num ######"
            .else
                .ext_irqs_defd = .ext_irqs_defd | (1 << ( OCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_type < 0 ) || ( \irq_type > 1 ))
                .error "###### .occhw_irq_config: invalid/unspecified irq type \irq_type for irq \irq_num ######"
            .else
                .ext_irqs_type = .ext_irqs_type | ( \irq_type << ( OCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_polarity < 0 ) || ( \irq_polarity > 1 ))
                .error "###### .occhw_irq_config: invalid/unspecified irq polarity ( \irq_polarity ) for irq \irq_num ######"
            .else
                .ext_irqs_polarity = .ext_irqs_polarity | ( \irq_polarity << ( OCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_mask < 0 ) || ( \irq_mask > 1 ))
                .error "###### .occhw_irq_config: invalid/unspecified irq mask ( \irq_mask ) for irq \irq_num ######"
            .else
                .ext_irqs_enable = .ext_irqs_enable | ( \irq_mask << ( OCCHW_IRQS - 1 - \irq_num ))
            .endif

            .occhw_irq_config \parms
        .endif
    .endm

    //recursive helper macro for setting up the irq route bitmaps for all entities in the occ complex
    //
    //Once completed, g_ext_irqs_route(A,B,C) will hold the correct initialization values and
    //g_ext_irqs_owned will hold a bitmap of interrupts owned by this OCC instance
    .macro .occhw_irq_route irq_num=-1 irq_route=-1 parms:vararg
        .if (( \irq_num == -1 ) && ( \irq_route == -1 ))
            .section .sdata
            .align 3
            .global g_ext_irqs_routeA
            .global g_ext_irqs_routeB
            .global g_ext_irqs_routeC
            .global g_ext_irqs_owned
            g_ext_irqs_routeA:
            .quad .ext_irqs_routeA
            g_ext_irqs_routeB:
            .quad .ext_irqs_routeB
            g_ext_irqs_routeC:
            .quad .ext_irqs_routeC
            g_ext_irqs_owned:
            .quad .ext_irqs_owned
        .else
            .if (( \irq_num < 0) || ( \irq_num > (OCCHW_IRQS - 1)))
                   .error "###### .occhw_irq_route: invalid irq number \irq_num ######"
            .elseif .ext_irqs_defd & (1 << ( OCCHW_IRQS - 1 - \irq_num ))
                .error "###### .occhw_irq_route: Route for irq \irq_num is already defined ######"
            .else
                .ext_irqs_defd = .ext_irqs_defd | (1 << ( OCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_route < 0) || ( \irq_route > 8 ))
                .error "###### .occhw_irq_route: route # \irq_route is invalid for irq # \irq_num ######"
            .endif

            .irq_mask = 1 << ( OCCHW_IRQS - 1 - \irq_num)
            .if \irq_route & 4
                .ext_irqs_routeA = .ext_irqs_routeA | .irq_mask
            .endif

            .if \irq_route & 2
                .ext_irqs_routeB = .ext_irqs_routeB | .irq_mask
            .endif

            .if \irq_route & 1
                .ext_irqs_routeC = .ext_irqs_routeC | .irq_mask
            .endif

            .if ( \irq_route == 8 )
                //do nothing, this irq is not owned by any OCC processor
            .elseif (( \irq_route < 4 ) && ( APPCFG_PM_INSTANCE_ID == 4 ))
                .ext_irqs_owned = .ext_irqs_owned | .irq_mask
            .elseif ( \irq_route == (OCCHW_IRQ_TARGET_ID_GPE0 + APPCFG_PM_INSTANCE_ID))
                .ext_irqs_owned = .ext_irqs_owned | .irq_mask
            .endif

            .occhw_irq_route \parms
        .endif
    .endm

    //Top level macro for generating interrupt routing/configuration globals for all entities in the occ complex
    .macro .occhw_irq_cfg_bitmaps
        .ext_irqs_routeA = 0
        .ext_irqs_routeB = 0
        .ext_irqs_routeC = 0
        .ext_irqs_owned = 0
        .ext_irqs_type = 0
        .ext_irqs_polarity = 0
        .ext_irqs_enable = 0
        .ext_irqs_defd = 0
        .irq_mask = 0
        .occhw_irq_route OCCHW_IRQ_ROUTES
        .ext_irqs_defd = 0
        .occhw_irq_config APPCFG_EXT_IRQS_CONFIG
    .endm

// *INDENT-ON*
#endif /*__ASSEMBLER__*/

#endif /*__OCCHW_IRQ_CONFIG_H__*/