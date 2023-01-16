/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeirqhandler.H $              */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#ifndef _SBEIRQHANDLER_H_
#define _SBEIRQHANDLER_H_

#include "sbeirq.H"

#ifdef __cplusplus
extern "C" {
#endif

#include "pk.h"

#ifdef __cplusplus
}
#endif


/********************** Function Prototype ****************************/
/* Default IRQ Handler */

/**
 * @brief Interrupt handler for SBE FIFO Reset + HFIFO Reset + PIPE Reset
 *
 * @param[in] i_pArg Pointer Arg
 * @param[in] i_irq IRQ number as defined in sbeirq.h
 */
void IrqHandlerFifoReset (void * i_pArg, PkIrqId i_irq);

/**
 * @brief Interrupt handler for SBE FIFO Data + HFIFO Data + PIPE Data
 *
 * @param[in] i_pArg Pointer Arg
 * @param[in] i_irq IRQ number as defined in sbeirq.h
 */
void IrqHandlerFifoData (void * i_pArg, PkIrqId i_irq);



#endif /* _SBEIRQHANDLER_H_ */