/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/irqutils.C $                   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#include "sbeirqregistersave.H"
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



///////////////////////////////////////////////////////////////////
// SBE handler to save off specific registers
///////////////////////////////////////////////////////////////////
registersave_t __g_register_ffdc __attribute__((aligned(8), section (".sbss")));
uint64_t __g_address_for_register_ffdc = (uint32_t)&__g_register_ffdc;
uint32_t __g_isParityError __attribute__((section (".sbss"))) = 0;

extern "C" void __sbe_register_saveoff()
{
    asm(
    "# Save r4, r5, r6 to stack, since it is going to be used\n"
    "# inside here, move the stack by 12\n"
    "stwu %r1, -12(%r1)\n"
    "# Store r4 into stack\n"
    "stw  %r4, 0(%r1)\n"
    "# Store r5 into stack\n"
    "stw  %r5, 4(%r1)\n"
    "# Store r6 into stack\n"
    "stw  %r6, 8(%r1)\n"
    "# Write zero to r4,r5,r6 to clear off the content\n"
    "lis %r4, 0\n"
    "lis %r5, 0\n"
    "lis %r6, 0\n"

    "# Check for loop condition if this branch is happening repetitively\n"
    "# load first 4bytes of __g_register_ffdc, it should be initialized\n"
    "# with zero, if not zero than this is repetitive entry, exit now\n"
    "lvd %d5, __g_register_ffdc@sda21(0)\n"
    "# if r5 and r4 not equal, then it's loop inside loop, exit condition\n"
    "cmplwbne %r4, %r5, __goto_exit\n"

    "# Start gathering registers into FFDC\n"
    "# Read SRR0 into r4\n"
    "mfsrr0  %r4\n"
    "stw %r4, __g_register_ffdc+4@sda21(0)\n"

    "# Read SRR1 into r4\n"
    "mfsrr1  %r4\n"
    "stw %r4, __g_register_ffdc+8@sda21(0)\n"

    "# Read ISR into r4\n"
    "mfisr  %r4\n"
    "stw %r4, __g_register_ffdc+12@sda21(0)\n"

    "# Read LR into r4\n"
    "mflr  %r4\n"
    "stw %r4, __g_register_ffdc+16@sda21(0)\n"

    "# Read LFR into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x2040(%r6)\n"
    "stw %r4, __g_register_ffdc+20@sda21(0)\n"

    "# Read Spi0 Status Register into r4 and r5\n"
    "lis %r6, 0xc\n"
    "lvd %d4, 0x8(%r6)\n"
    "stw %r4, __g_register_ffdc+24@sda21(0)\n"
    "stw %r5, __g_register_ffdc+28@sda21(0)\n"

    "# Read Spi0 Config Register into r4 and r5\n"
    "lvd %d4, 0x2(%r6)\n"
    "stw %r4, __g_register_ffdc+32@sda21(0)\n"

    "# Read Spi0 Clock Config Register into r4 and r5\n"
    "lvd %d4, 0x3(%r6)\n"
    "stw %r4, __g_register_ffdc+36@sda21(0)\n"

    "# Read Spi1 Status Register into r4 and r5\n"
    "lis %r6, 0xc\n"
    "lvd %d4, 0x28(%r6)\n"
    "stw %r4, __g_register_ffdc+40@sda21(0)\n"
    "stw %r5, __g_register_ffdc+44@sda21(0)\n"

    "# Read Spi1 Config Register into r4 and r5\n"
    "lvd %d4, 0x22(%r6)\n"
    "stw %r4, __g_register_ffdc+48@sda21(0)\n"

    "# Read Spi1 Clock Config Register into r4 and r5\n"
    "lvd %d4, 0x23(%r6)\n"
    "stw %r4, __g_register_ffdc+52@sda21(0)\n"

    "# Read Spi2 Status Register into r4 and r5\n"
    "lis %r6, 0xc\n"
    "lvd %d4, 0x68(%r6)\n"
    "stw %r4, __g_register_ffdc+56@sda21(0)\n"
    "stw %r5, __g_register_ffdc+60@sda21(0)\n"

    "# Read Spi2 Config Register into r4 and r5\n"
    "lvd %d4, 0x62(%r6)\n"
    "stw %r4, __g_register_ffdc+64@sda21(0)\n"

    "# Read Spi2 Clock Config Register into r4 and r5\n"
    "lvd %d4, 0x63(%r6)\n"
    "stw %r4, __g_register_ffdc+68@sda21(0)\n"

    "# Read Spi3 Status Register into r4 and r5\n"
    "lis %r6, 0xc\n"
    "lvd %d4, 0x48(%r6)\n"
    "stw %r4, __g_register_ffdc+72@sda21(0)\n"
    "stw %r5, __g_register_ffdc+76@sda21(0)\n"

    "# Read Spi3 Config Register into r4 and r5\n"
    "lvd %d4, 0x42(%r6)\n"
    "stw %r4, __g_register_ffdc+80@sda21(0)\n"

    "# Read Spi3 Clock Config Register into r4 and r5\n"
    "lvd %d4, 0x43(%r6)\n"
    "stw %r4, __g_register_ffdc+84@sda21(0)\n"

    "# TODO\n"
    "# Read Spi parity errors\n"
    "lvd %d4, __g_isParityError@sda21(0)\n"
    "stw %r4, __g_register_ffdc+88@sda21(0)\n"

    "# Write versioning into the lower 32bit of d5 i.e. r6\n"
    "#lvd %d5, __g_sbe_register_save_version@sda21(0)\n"
    "# Directly load Versioninfo into upper 16bits in r4 \n"
    "# RTC:183752, Remove this hard-coding of versioning\n"
    "lis %r4, 0x11\n"
    "#rlwinm %r4, %r6, 16, 0, 0x1F\n"
    "# Write magicbyte and validbyte in the lower 16bits\n"
    "ori %r4, %r4, 0xA501\n"
    "stw %r4, __g_register_ffdc@sda21(0)\n"

    "# Get the address of the register ffdc struct and store it in sprg0\n"
    "lvd %d5, __g_address_for_register_ffdc@sda21(0)\n"
    "# Store the content of r6 read above into sprg0\n"
    "mtsprg0 %r6\n"

    "__goto_exit:\n"
    "# Get back the original value of the registers stored in stack\n"
    "lwz %r6, 8(%r1)\n"
    "lwz %r5, 4(%r1)\n"
    "lwz %r4, 0(%r1)\n"
    "addi %r1, %r1, 12\n"
    "b pk_halt\n"
    );
}

////////////////////////////////////////////////////////////////
// SBE handler for the PPE machine check interrupt
// This is valid for both measurement fw and boot fw
////////////////////////////////////////////////////////////////
extern "C" void __sbe_machine_check_handler()
{
    asm
    (
        "# Save r4\n"
        "# lr to stack, since it is going to be used by\n"
        "# this handler\n"
        "stwu %r1, -80(%r1)\n"
        "stw  %r0, 0(%r1)\n"
        "stw  %r4, 4(%r1)\n"
        "mflr %r0\n"
        "stw  %r0, 8(%r1)\n"

        "# need to check whether the machine check caused by a scom error or not\n"
        "#   if it is scom error, we have to continue the sbe firmware, other wise halt\n"

        "# Check the MCS bits (ISR[29:31]) in the ISR to determine the cause for the machine check\n"
        "# For a data machine (which will happen in case of scom error) check, the MCS should be 0x001 to 0x011\n"
        "# 0x01 -> Data Load MC\n"
        "# 0x02 -> Data Store MC\n"
        "# 0x03 -> Imprecise Data Store MC\n"
        "mfisr %r4\n"
        "andi. %r4, %r4, 0x0007\n"

        "# halt if MCS not in between 0b01 and 0b11"
        "bwz %r4, __jump_to_halt\n"
        "cmpwibgt %r4, 0x0003, __jump_to_halt\n"

        "# This is the Data_MC path, EDR contains the Data Addr causing the MC\n"
        "mfedr %r4\n"
        "srawi %r4, %r4, 16\n"
        "# If the address is in the range 0x00000000 - 0x7f000000, we treat it as a\n"
        "# failed scom and jump to __scom_error\n"
        "cmplwi %r4, 0x8000\n"
        "blt __scom_error\n"

        "# Else, halt the SBE\n"
        "# relative jump to pk_halt from this code is not possible since address\n"
        "#   difference is more than supported.\n"
        "__jump_to_halt:\n"
        "b pk_halt\n"

        "__scom_error:\n"
        "# The srr0 contains the address of the instruction that caused the machine\n"
        "# check (since the the interrupt is raised *before* the instruction\n"
        "# completed execution). Since we want the code to continue with the next\n"
        "# instruction, we increment srr0 by 4, restore r4, and rfi to branch to srr0\n"
        "mfsrr0 %r4\n"
        "addi %r4, %r4, 4\n"
        "mtsrr0 %r4\n"

        "lwz %r4, 4(%r1)\n"
        "lwz %r0, 8(%r1)\n"
        "mtlr %r0\n"
        "lwz %r0, 0(%r1)\n"
        "addi %r1, %r1, 80\n"

        "rfi\n"
    );
}
