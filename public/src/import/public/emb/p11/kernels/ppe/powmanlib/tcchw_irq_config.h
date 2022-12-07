/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/tcchw_irq_config.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#ifndef __TCCHW_IRQ_CONFIG_H__
#define __TCCHW_IRQ_CONFIG_H__

/// \file tcchw_irq_config.h
/// \brief Contains data and macros pertaining to external interrupt routing
///        and configuration for processors running in the TCC complex.
///


/// Number of TCC instance ID's that can be read from the PPE42 PIR and used in IPCs.
#define PLATHW_MAX_INSTANCES 4


#ifndef __ASSEMBLER__
    /// These globals are statically initialized elsewhere
    extern uint64_t g_ext_irqs_routeA;
    extern uint64_t g_ext_irqs_routeB;
    extern uint64_t g_ext_irqs_routeC;
    extern uint64_t g_ext_irqs_type;
    extern uint64_t g_ext_irqs_owned;
    extern uint64_t g_ext_irqs_polarity;
    extern uint64_t g_ext_irqs_enable;

    #define PLATHW_IRQ_OWNED(irq) ((TCCHW_IRQ_MASK64(irq) & g_ext_irqs_owned) != 0)

#endif

/// Add a pseudo target to indicate that an interrupt is not owned by anyone
#define TCCHW_IRQ_TARGET_ID_NONE    8

/// Default interrupt routing table
#ifndef TCCHW_IRQ_ROUTES
#define TCCHW_IRQ_ROUTES \
    TCCHW_IRQ_DEBUGGER                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_TRACE_TRIGGER_1               TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_CE0_ERROR                     TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_CE1_ERROR                     TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_CHECK_STOP_CE0                TCCHW_IRQ_TARGET_ID_GPE0 \
    TCCHW_IRQ_CHECK_STOP_CE1                TCCHW_IRQ_TARGET_ID_GPE1 \
    TCCHW_IRQ_PBA_ERROR                     TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_SPARE_7                       TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_TCC_TIMER0                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_TCC_TIMER1                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_IPI0_HI_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE0 \
    TCCHW_IRQ_IPI1_HI_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE1 \
    TCCHW_IRQ_SPARE_12                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_SPARE_13                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_SPARE_14                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_SPARE_15                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBAX_TCC_SEND_ATTN            TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBAX_TCC_PUSH0                TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBAX_TCC_PUSH1                TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBA_BCDE_ATTN                 TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_PBA_BCUE_ATTN                 TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_STRM0_PULL                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM0_PUSH                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM1_PULL                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM1_PUSH                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM2_PULL                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM2_PUSH                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM3_PULL                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_STRM3_PUSH                    TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_IPI0_LO_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE0 \
    TCCHW_IRQ_IPI1_LO_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE1 \
    TCCHW_IRQ_SPARE_31                      TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_TRACE_TRIGGER_32              TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_TCC_ERROR                     TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_CE2_ERROR                     TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_CE3_ERROR                     TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_CHECK_STOP_CE2                TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_CHECK_STOP_CE3                TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_TCC_MALF_ALERT                TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PVREF_ERROR                   TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_IPI2_HI_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_IPI3_HI_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_DEBUG_TRIGGER                 TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_IPI2_LO_PRIORITY              TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_PBAX_HCODE_SEND_ATTN          TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBAX_HCODE_PUSH0              TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBAX_HCODE_PUSH1              TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PBA_OVERCURRENT_INDICATOR     TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_PSPI_MSGDOORBELL_0            TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PSPI_MSGDOORBELL_1            TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PSPI_MSGDOORBELL_2            TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_PSPI_MSGDOORBELL_3            TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_PSPI_DOORBELL_0               TCCHW_IRQ_TARGET_ID_GPE0 \
    TCCHW_IRQ_PSPI_DOORBELL_1               TCCHW_IRQ_TARGET_ID_GPE1 \
    TCCHW_IRQ_PSPI_DOORBELL_2               TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PSPI_DOORBELL_3               TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_PCB_INTR_PENDING_GRP0         TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PCB_INTR_PENDING_GRP1         TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_PCB_INTR_PENDING_GRP2         TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_PCB_INTR_PENDING_GRP3         TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_XCE_FUNCTION_TRIGGER          TCCHW_IRQ_TARGET_ID_GPE3 \
    TCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE     TCCHW_IRQ_TARGET_ID_NONE \
    TCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE     TCCHW_IRQ_TARGET_ID_GPE2 \
    TCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE     TCCHW_IRQ_TARGET_ID_NONE \

#endif


#ifdef __ASSEMBLER__
// *INDENT-OFF*
/// These macros aid in the initialization of the external interrupt globals.  I would
/// prefer to use CPP macros, but they don't support recursive macros which I use to
/// convert the variable number of interrupts that a processor can control into static
/// bitmaps used by __hwmacro_setup() at runtime.


    //helper macro for setting up the irq configuration bitmaps for all entities in the occ complex
    .macro .tcchw_irq_config irq_num=-1 irq_type=-1 irq_polarity=-1 irq_mask=-1 parms:vararg
        .if (( \irq_num == -1 ) && ( \irq_type == -1 ) && ( \irq_polarity == -1 ) && ( \irq_mask == -1 ))
            .if ( .ext_irqs_defd != .ext_irqs_owned )
                .error "###### .tcchw_irq_config: Missing configuration for one or more interrupts ######"
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
            .if (( \irq_num < 0 ) || ( \irq_num > (TCCHW_IRQS - 1)))
                .error "###### .tcchw_irq_config: invalid irq number \irq_num ######"
            .elseif ((.ext_irqs_owned & (1 << ( TCCHW_IRQS - 1 - \irq_num ))) == 0 )
                .error "###### .tcchw_irq_config: Attempt to configure unowned irq number \irq_num ######"
            .elseif (.ext_irqs_defd & (1 << ( TCCHW_IRQS - 1 - \irq_num )))
                .error "###### .tcchw_irq_config: duplicate definition for irq \irq_num ######"
            .else
                .ext_irqs_defd = .ext_irqs_defd | (1 << ( TCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_type < 0 ) || ( \irq_type > 1 ))
                .error "###### .tcchw_irq_config: invalid/unspecified irq type \irq_type for irq \irq_num ######"
            .else
                .ext_irqs_type = .ext_irqs_type | ( \irq_type << ( TCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_polarity < 0 ) || ( \irq_polarity > 1 ))
                .error "###### .tcchw_irq_config: invalid/unspecified irq polarity ( \irq_polarity ) for irq \irq_num ######"
            .else
                .ext_irqs_polarity = .ext_irqs_polarity | ( \irq_polarity << ( TCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_mask < 0 ) || ( \irq_mask > 1 ))
                .error "###### .tcchw_irq_config: invalid/unspecified irq mask ( \irq_mask ) for irq \irq_num ######"
            .else
                .ext_irqs_enable = .ext_irqs_enable | ( \irq_mask << ( TCCHW_IRQS - 1 - \irq_num ))
            .endif

            .tcchw_irq_config \parms
        .endif
    .endm

    //Recursive helper macro for setting up the irq route bitmaps for entities in the TCC complex
    //
    //Once completed, g_ext_irqs_route(A,B,C) will hold the correct initialization values and
    //g_ext_irqs_owned will hold a bitmap of interrupts owned by this TCC instance
    .macro .tcchw_irq_route irq_num=-1 irq_route=-1 parms:vararg
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
            .if (( \irq_num < 0) || ( \irq_num > (TCCHW_IRQS - 1)))
                .error "###### .tcchw_irq_route: invalid irq number \irq_num ######"
            .elseif .ext_irqs_defd & (1 << ( TCCHW_IRQS - 1 - \irq_num ))
                .error "###### .tcchw_irq_route: Route for irq \irq_num is already defined ######"
            .else
                .ext_irqs_defd = .ext_irqs_defd | (1 << ( TCCHW_IRQS - 1 - \irq_num ))
            .endif

            .if (( \irq_route < 4) || ( \irq_route > 8 ))
                .error "###### .tcchw_irq_route: route # \irq_route is invalid for irq # \irq_num ######"
            .endif

            .irq_mask = 1 << ( TCCHW_IRQS - 1 - \irq_num)
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
                //do nothing, this irq is not owned by any TCC processor
            .elseif ( \irq_route == (TCCHW_IRQ_TARGET_ID_GPE0 + APPCFG_PM_INSTANCE_ID))
                .ext_irqs_owned = .ext_irqs_owned | .irq_mask
            .endif

            .tcchw_irq_route \parms
        .endif
    .endm

    //Top level macro for generating interrupt routing/configuration globals for all entities in the occ complex
    .macro .tcchw_irq_cfg_bitmaps
        .ext_irqs_routeA = 0
        .ext_irqs_routeB = 0
        .ext_irqs_routeC = 0
        .ext_irqs_owned = 0
        .ext_irqs_type = 0
        .ext_irqs_polarity = 0
        .ext_irqs_enable = 0
        .ext_irqs_defd = 0
        .irq_mask = 0
        .tcchw_irq_route TCCHW_IRQ_ROUTES
        .ext_irqs_defd = 0
        .tcchw_irq_config APPCFG_EXT_IRQS_CONFIG
    .endm

// *INDENT-ON*
#endif /*__ASSEMBLER__*/

#endif /*__TCCHW_IRQ_CONFIG_H__*/
