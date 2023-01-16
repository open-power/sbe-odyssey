/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeirqhandler.C $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
 * @file: ppe/sbe/sbefw/sbeirq.C
 *
 * @brief This sets up and registers SBE ISRs
 *
 */

#include "sbeirqhandler.H"
#include "irqutils.H"
#include "sbeglobals.H"
#include "sbeFifoMsgUtils.H"


/************************ Global variable *************************/

/****************************** Macro *****************************/

/*************************** Functions ****************************/

/**
 * @brief Interrupt handler for SBE FIFO Reset + HFIFO Reset + PIPE Reset
 */
void IrqHandlerFifoReset (void * i_pArg, PkIrqId i_irq)
{
    #define SBE_FUNC "IrqHandler_vIplHreset "

    SBE_INFO(SBE_FUNC "i_irq=[0x%02X]",i_irq);
    CHECK_VALID_IRQ(i_irq, SBE_INT_FIFO_RESET);

    sbeInterfaceSrc_t curInterface = SBE_INTERFACE_UNKNOWN;
    sbeFifoType fifoType = SBE_FIFO_UNKNOWN;

    fifoType = sbeFifoGetSource(true);
    curInterface = sbeFifoGetInstSource(fifoType, true);
    SBE_GLOBAL->sbeIntrSource.setIntrSource(SBE_INTERRUPT_ROUTINE,
                                            curInterface);
    SBE_INFO(SBE_FUNC "reset irq fifotype=0x%08x, curInterface=0x%08x",
                fifoType, curInterface);
    pk_irq_disable(SBE_IRQ_SBEFIFO_DATA);

    if (fifoType != SBE_FIFO_UNKNOWN)
    {
        SBE_GLOBAL->activeUsFifo = fifoType;
        SBE_GLOBAL->activeInterface = curInterface;

         /* CALL CALLBACK should be part of IRQ handler */
        CALL_CALLBACK_FUNC(i_pArg, i_irq, &fifoType);
    }

    #undef SBE_FUNC
}


/**
 * @brief Interrupt handler for SBE FIFO Data + HFIFO Data + PIPE Data
 */
void IrqHandlerFifoData (void * i_pArg, PkIrqId i_irq)
{
    #define SBE_FUNC "IrqHandler_vFifoData "

    SBE_INFO(SBE_FUNC"i_irq=[0x%02X]",i_irq);
    CHECK_VALID_IRQ(i_irq, SBE_INT_FIFO_DATA);

    sbeInterfaceSrc_t curInterface = SBE_INTERFACE_UNKNOWN;
    sbeFifoType fifoType = SBE_FIFO_UNKNOWN;

    fifoType = sbeFifoGetSource(false);
    curInterface = sbeFifoGetInstSource(fifoType, false);

    SBE_GLOBAL->sbeIntrSource.setIntrSource(SBE_INTERRUPT_ROUTINE,
                                            curInterface);
    SBE_INFO(SBE_FUNC "data irq fifotype=0x%08x, curInterface=0x%08x",
                fifoType, curInterface);
    pk_irq_disable(SBE_IRQ_SBEFIFO_RESET);

    if (fifoType != SBE_FIFO_UNKNOWN)
    {
        SBE_GLOBAL->activeUsFifo = fifoType;
        SBE_GLOBAL->activeInterface = curInterface;

        CALL_CALLBACK_FUNC(i_pArg, i_irq, &fifoType);
    }

    #undef SBE_FUNC
}
