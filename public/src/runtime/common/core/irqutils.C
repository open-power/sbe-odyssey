/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/irqutils.C $                   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "irqutils.H"
#include "assert.h"
#include "ppe42_scom.h"
#include "sbeirqhandler.H"
#include "sbeglobals.H"
#include "sbeFifoMsgUtils.H"


/**
 * @brief Common IRQ register data (common across all plat).
 *        Add here common IRQ handler with callback, This is strut
 *        array of @ref sIrqRegData_t
 *
 * This is COMMON IRQ REGISTER DATA, which is common across all platform
 * This will be used in plat code for IRQ enable (register).
 *
 * Before modify the COMMON IRQ file please read below steps.
 * NOTE:
 * 1. Callback function can be declared in plat/common
 * 2. Given enough flexible to register irq handler and callback,
 *    so need maintain commonality by following below guideline:
 *    - Function are common across more than two plat, put it in
 *      @file irqutils.C be treated as utils functions
 *    - Given Override option for common callback handler in plat, in case
 *      callback functionality change specific to plat
 *    - Sometime irq utils functions can be treated as callback handler
 * 3. IRQ's which are specific to plat will be declared in plat and registered
 *    as part of plat
 * 4. @ref g_IrqRegList can be override in plat code for plat related irq's
 */
sIrqRegData_t g_IrqRegList[ ] =
{
    {
        .eSbeIntNo        = SBE_INT_FIFO_RESET,      /// Interrupt Number
        .IrqHandler       = IrqHandlerFifoReset,     /// IRQ default handler define in @file sbeirqhandler.C
        .CallbackHandler  = SbeIrqCommonCallbackFifo,/// IRQ callback function
    },
    {
        .eSbeIntNo        = SBE_INT_FIFO_DATA,       /// Interrupt Number
        .IrqHandler       = IrqHandlerFifoData,      /// IRQ default handler define in @file sbeirqhandler.C
        .CallbackHandler  = SbeIrqCommonCallbackFifo,/// IRQ callback function
    },
};

/********************** Function ****************************/

/**
 * @brief Get global COMMON IRQ register data size
 */
uint8_t SbeIrqGetCmnIrqDataSize(void)
{
    return (sizeof(g_IrqRegList)/sizeof(sIrqRegData_t));
}


/**
 * @brief Function of enable SBE interrupts
 */
eSbeIrqStatus_t SbeIrqEnable (eSbeIntNo_t i_SbeIntNo,
                                IrqHandlerFunc_t i_pIrqHandler,
                                IrqCallbackFunc_t i_pCallbackHandler)
{
    #define SBE_FUNC "SbeIrqEnable: "
    eSbeIrqStatus_t status = SBE_IRQ_STATUS_SUCCESS;
    int rc = 0;

    do
    {
        /* Check Valid Irq */
        if ((SBE_IRQ_CHECK_VALID_IRQ(i_SbeIntNo) != true) ||
            (i_pIrqHandler == NULL))
        {
            SBE_ERROR (SBE_FUNC "SBE_IRQ_CHECK_VALID_IRQ failed, IRQ=[0x%02X]",
                                                                    i_SbeIntNo);
            status = SBE_IRQ_STATUS_ARG_INVALID;
            break;
        }

        // Disable the relevant IRQs while we set them up
        pk_irq_disable(i_SbeIntNo);

        SBE_DEBUG(SBE_FUNC "Registering the interrupt handler IRQ=[0x%02X]", i_SbeIntNo);
        /* Registering the Handler and callback to PK */
        rc = pk_irq_handler_set(i_SbeIntNo, (PkIrqHandler) i_pIrqHandler, (void *) i_pCallbackHandler);
        if (rc != PK_OK)
        {
            SBE_ERROR (SBE_FUNC "pk_irq_handler_set failed, IRQ=[0x%02X]"
                "rc=[%d]", i_SbeIntNo, rc);
            status = SBE_IRQ_STATUS_ERROR;
            break;
        }

        // Enable the IRQ
        pk_irq_enable(i_SbeIntNo);
    }while(0);

    return status;
    #undef SBE_FUNC
}


/**
 * @brief Function to setup the SBE IRQ for given array
 */
eSbeIrqStatus_t SbeIrqEnable(sIrqRegData_t i_irqRegData[], uint32_t i_size)
{
    #define SBE_FUNC "SbeIrqEnable: "
    eSbeIrqStatus_t status = SBE_IRQ_STATUS_SUCCESS;

    do
    {
        if ((i_irqRegData == NULL) || i_size == 0)
        {
            status = SBE_IRQ_STATUS_ARG_INVALID;
            break;
        }

        /* IRQ register */
        for (uint16_t i = 0; i < i_size; i++)
        {
            SBE_INFO (SBE_FUNC "Cmn IRQ Setup No: %d", i_irqRegData[i].eSbeIntNo);
            status = SbeIrqEnable ( i_irqRegData[i].eSbeIntNo,
                                    i_irqRegData[i].IrqHandler,
                                    i_irqRegData[i].CallbackHandler);
            if (status != SBE_IRQ_STATUS_SUCCESS)
            {
                SBE_ERROR(SBE_FUNC "IRQ setup failed,IRQ no: %d, RC: %d",
                                                i_irqRegData[i].eSbeIntNo, status);
                status = SBE_IRQ_STATUS_ERROR;
                break;
            }
        }
    }while(0);

    return status;
    #undef SBE_FUNC
}


/**
 * @brief Common IRQ setup function
 */
eSbeIrqStatus_t sbeIrqSetup(void)
{
    #define SBE_FUNC "sbeIrqSetup: "

    eSbeIrqStatus_t status = SBE_IRQ_STATUS_SUCCESS;

    /* Common data register */
    status = SbeIrqEnable( g_IrqRegList,
                          (sizeof(g_IrqRegList)/sizeof(sIrqRegData_t)) );

    return status;
    #undef SBE_FUNC
}

/*********************** plat related functions *******************************/
/**
 * @brief Platform IRQ setup function
 */
eSbeIrqStatus_t sbeIrqSetup( sIrqRegData_t i_irqRegData[ ],
                             uint32_t i_regDataSize)
{
    #define SBE_FUNC "sbeIrqSetup: "
    eSbeIrqStatus_t status = SBE_IRQ_STATUS_SUCCESS;

    do
    {
        status = sbeIrqSetup();
        if (status != SBE_IRQ_STATUS_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "Common sbeIrqSetup failure, RC; [0x%08X]",
                                                                        status);
            status = SBE_IRQ_STATUS_ERROR;
            break;
        }

        status = SbeIrqEnable (i_irqRegData, i_regDataSize);
        if (status != SBE_IRQ_STATUS_SUCCESS)
        {
            SBE_ERROR(SBE_FUNC "Plat sbeIrqSetup failure, RC; [0x%08X]",
                                                                        status);
            status = SBE_IRQ_STATUS_ERROR;
            break;
        }
    }while(false);

    return status;
    #undef SBE_FUNC
}



/***************************** utils functions ************************/

/**
 * @brief IRQ until function for post sema to receiver task
 */
void IrqUtilsPostReceiverSema(uint8_t i_irq)
{
    // Mask the interrupt
    pk_irq_disable(i_irq);

    // Unblock the command receiver thread
    int l_rc = pk_semaphore_post(&SBE_GLOBAL->semphores.sbeSemCmdRecv);
    if (l_rc)
    {
        // If we received an error while posting the semaphore,
        // unmask the interrupt back and assert
        SBE_ERROR("IrqUtils_iPostReceiverSema: pk_semaphore_post failed,"
                                                            "rc=[%d]", l_rc);
        pk_irq_enable((PkIrqId) i_irq);
        assert(!l_rc);
    }
}


/***************************** Common callback ************************/

/**
 * @brief SBE FIFO Common specific irq callback function
 */
void SbeIrqCommonCallbackFifo(PkIrqId i_irq, void * i_pArg)
{
    #define SBE_FUNC "SbeIrqCommonCallbackFifo: "

    SBE_INFO(SBE_FUNC "Common IRQ Callback Func, IRQ no: %d", i_irq);
    IrqUtilsPostReceiverSema(i_irq);

    #undef SBE_FUNC
}
