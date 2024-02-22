/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/pk/test/pk_app_irq_table.c $ */
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
#include "pk.h"

EXTERNAL_IRQ_TABLE_START
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_DEBUGGER
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_TRACE_TRIGGER_1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_SPARE_2
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBA_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_GPE0_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_GPE1_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_CHECK_STOP_PPC405
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_EXTERNAL_TRAP
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_TIMER0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_TIMER1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI0_HI_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI1_HI_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI4_HI_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_I2CM_INTR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI_SCOM
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_SPARE_15
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_OCC_SEND_ATTN
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_OCC_PUSH0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_OCC_PUSH1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBA_BCDE_ATTN
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBA_BCUE_ATTN
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM0_PULL
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM0_PUSH
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM1_PULL
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM1_PUSH
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM2_PULL
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM2_PUSH
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM3_PULL
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_STRM3_PUSH
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI0_LO_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI1_LO_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI4_LO_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_TRACE_TRIGGER_32
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_GPE2_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_GPE3_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_CHECK_STOP_GPE2
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_CHECK_STOP_GPE3
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_OCC_MALF_ALERT
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PVREF_ERROR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI2_HI_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI3_HI_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_DEBUG_TRIGGER
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_AVS_PENDING
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_PGPE_SEND_ATTN
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_PGPE_PUSH0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBAX_PGPE_PUSH1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PBA_OVERCURRENT_INDICATOR
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_MSGDOORBELL_0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_MSGDOORBELL_1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_MSGDOORBELL_2
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_MSGDOORBELL_3
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_DOORBELL_0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_DOORBELL_1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_DOORBELL_2
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_DOORBELL_3
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_INTR_PENDING_GRP0
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_INTR_PENDING_GRP1
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_INTR_PENDING_GRP2
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_PSPI_INTR_PENDING_GRP3
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_IPI3_LO_PRIORITY
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_VC1_PUSH_RECEIVE_COMPLETE
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_VC2_PUSH_RECEIVE_COMPLETE
IRQ_HANDLER_DEFAULT            //OCCHW_IRQ_VC3_PUSH_RECEIVE_COMPLETE
EXTERNAL_IRQ_TABLE_END
