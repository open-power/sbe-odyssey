/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/irqutils.H $                   */
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

#ifndef _IRQUTILS_H_
#define _IRQUTILS_H_

#include "sbeirqhandler.H"

/****************************** Macro *****************************/

/**
 * @brief SBE IRQ Return Code
 */
typedef enum
{
    SBE_IRQ_STATUS_SUCCESS = 0x00,
    SBE_IRQ_STATUS_ERROR,
    SBE_IRQ_STATUS_ARG_INVALID,
}eSbeIrqStatus_t;


/**
 * @brief SBE application interrupt numbers
 */
typedef enum
{
    SBE_INT_IPL_HRESET      = SBE_IRQ_START0,
    SBE_INT_RUNTIME_HRESET  = SBE_IRQ_START1,
    SBE_INT_INTR0           = SBE_IRQ_INTR0,
    SBE_INT_INTR1           = SBE_IRQ_INTR1,
    SBE_INT_SPARE           = SBE_IRQ_RESERVED_4, /* not using */
    SBE_INT_FIFO_RESET      = SBE_IRQ_SBEFIFO_RESET,
    SBE_INT_FIFO_DATA       = SBE_IRQ_SBEFIFO_DATA,
    SBE_INT_PSU             = SBE_IRQ_SBEPSU,
    SBE_INT_TRIGGER         = SBE_IRQ_SBETRIG,
    SBE_INT_XSTOP           = SBE_IRQ_SBEXSTOP,
    SBE_INT_TAP_ATTENTION   = SBE_IRQ_TAP_ATTN,
    SB_INT_MAX,
}eSbeIntNo_t;


/**
 * @brief SBE interrupt function pointer typedef
 */
typedef void (* IrqHandlerFunc_t) ( void *, uint8_t );


/**
 * @brief SBE interrupt callback function pointer typedef
 */
typedef void (* IrqCallbackFunc_t) ( uint8_t, void * );

/**
 * @brief Call the callback function if callback is register to interrupt
 */
#define CALL_CALLBACK_FUNC(callback, irq_no, i_arg) \
        if (callback != NULL){ \
            IrqCallbackFunc_t callbackFunc = (IrqCallbackFunc_t) callback; \
            callbackFunc(irq_no, i_arg); \
        }

/**
 * @brief SBE Irq check whether valid IRQ/Invalid Irq
 *
 * return true: Valid Irq
 *        false: Invalie Irq
 *
 */
#define SBE_IRQ_CHECK_VALID_IRQ(i_irq) \
    ((i_irq < SB_INT_MAX)?((i_irq != SBE_INT_SPARE)? true : false) : false)


/**
 * @brief For validate the interrupt number with respect to irq handler
 */
#define CHECK_VALID_IRQ(pk_irq, int_irq) \
if (pk_irq != int_irq){ \
    SBE_ERROR(SBE_FUNC "Irq Number Mismatch"); \
    return; \
}

/**
 * @brief Irq register Data
 */
typedef struct
{
    eSbeIntNo_t eSbeIntNo;
    IrqHandlerFunc_t IrqHandler;
    IrqCallbackFunc_t CallbackHandler;
}sIrqRegData_t;


/**
 * @brief extern the Common IRQ list
 *
 * @note This is common across all platform. IRQ's which are specific to plat
 *       will be declared in plat and registered as part of plat
 */
extern sIrqRegData_t g_IrqRegList[ ];

/********************** Function Prototype ****************************/

/**
 * @brief Get global COMMON IRQ register data size
 *
 * @return uint8_t size of @ref g_IrqRegList
 */
uint8_t SbeIrqGetCmnIrqDataSize(void);


/**
 * @brief Function of enable SBE interrupts
 *
 * @param[in] i_SbeIntNo @ref eSbeIntNo_t SBE interrupt number
 * @param[in] i_pIrqHandler Interrupt Handler
 * @param[in] i_pCallbackHandler interrupt callback function
 * @return eSbeIrqStatus_t SBE_IRQ_STATUS_SUCCESS for enable the IRQ
 */
eSbeIrqStatus_t SbeIrqEnable (eSbeIntNo_t i_SbeIntNo,
                                IrqHandlerFunc_t i_pIrqHandler,
                                IrqCallbackFunc_t i_pCallbackHandler);


/**
 * @brief Function to setup the SBE IRQ for given array
 * @param[in] irqRegData array of @ref sIrqRegData_t
 * @param[in] size size for the array
 *
 * @return eSbeIrqStatus_t enum return
 */
eSbeIrqStatus_t SbeIrqEnable(sIrqRegData_t i_irqRegData[], uint32_t i_size);


/**
 * @brief Function to initialize the common irq's
 *        This function will initialize the @ref g_IrqRegList irq list.
 * @return eSbeIrqStatus_t enum return
 */
eSbeIrqStatus_t sbeIrqSetup(void);



/************************* Platform functions *********************************/
/**
 * @brief Function to initialize Platform irq's
 *
 * @param i_irqRegData  pointer to Drive IRQ's
 * @param i_regDataSize size of the irq reg data
 * @return eSbeIrqStatus_t enum return
 */
eSbeIrqStatus_t sbeIrqSetup( sIrqRegData_t i_irqRegData[ ],
                             uint32_t i_regDataSize );



/************************* utility functions **********************************/
/**
 * @brief IRQ until function for post sema to receiver task
 *
 * @param[in] i_irq IRQ number as defined in sbeirq.h
 */
void IrqUtilsPostReceiverSema(uint8_t i_irq);

/**
 * @brief SBE FIFO Common specific irq callback function
 *
 * @param[in] i_irq Irq number
 * @param[in] i_pArg input arg
 */
void SbeIrqCommonCallbackFifo(PkIrqId i_irq, void * i_pArg);

#endif /* _IRQUTILS_H_ */