/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdreceiver.C $       */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
/*
 * @file: ppe/sbe/sbefw/sbecmdreceiver.C
 *
 * @brief This file contains the SBE Command Receiver Thread Routine
 *
 */

#include "sbeexeintf.H"
#include "sbefifo.H"
#include "sbeirq.H"
#include "sbetrace.H"
#include "sbe_sp_intf.H"
#include "sbeFifoMsgUtils.H"
#include "sbeglobals.H"
#include "sbeerrorcodes.H"
#include "chipop_handler.H"
#include "sberegaccess.H"
#include "sbehandleresponse.H"

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void sbeCommandReceiver_routine(void *i_pArg)
{
    #define SBE_FUNC " sbeCommandReceiver_routine "
    SBE_INFO(SBE_FUNC "thread started");
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    sbeInterfaceSrc_t curInterface = SBE_INTERFACE_UNKNOWN;
    sbeFifoType fifoType = SBE_FIFO;

    do
    {
        // Wait on intr handler to hand-off a FIFO New Data/Reset request
        int l_rcPk = pk_semaphore_pend (&SBE_GLOBAL->sbeSemCmdRecv, PK_WAIT_FOREVER);

        do
        {
            uint8_t l_cmdClass = SBE_CMD_CLASS_UNKNOWN;
            uint8_t l_command  = 0xFF;

            // pk API failure
            if (l_rcPk != PK_OK)
            {
                break;
            }

            SBE_INFO(SBE_FUNC"Receiver unblocked");

            fifoType = static_cast<sbeFifoType>(SBE_GLOBAL->activeUsFifo);
            curInterface = static_cast<sbeInterfaceSrc_t>(SBE_GLOBAL->activeInterface);

            if (fifoType != SBE_FIFO)
            {   // for Pipes, setup the downstream pipe
                // Based on how pipe reset will be handled, the downstream pipe
                // needs to be known and setup upfront
                l_rc = sbeHandleDsPipeCnfg(fifoType);

                if (l_rc)
                {
                    SBE_ERROR(SBE_FUNC "Failed setting DS pipe for fifoType %d",
                              fifoType);
                    break;
                }
            }

            // The responsibility of this thread is limited to reading off
            // the FIFO or PIPE interfaces to be able to decode the command
            // class and the command opcode parameters.

            // Received Reset Interrupt on FIFO/PIPE interface
            if ( SBE_GLOBAL->sbeIntrSource.isSet(SBE_INTERRUPT_ROUTINE,
                       (sbeInterfaceSrc_t) ( SBE_INTERFACE_MASK_RESET_ALL )))
            {
                SBE_INFO(SBE_FUNC "FIFO/PIPE Reset received");
                l_rc = SBE_FIFO_RESET_RECEIVED;
                break;
            }

            // Received FIFO New Data interrupt
            if ( SBE_GLOBAL->sbeIntrSource.isSet(SBE_INTERRUPT_ROUTINE,
                     (sbeInterfaceSrc_t)   ( SBE_INTERFACE_MASK_DATA_ALL )))
            {
                SBE_INFO(SBE_FUNC" Data on curInterface=0x%08x,fifoType=0x%08x",
                                   curInterface, fifoType);
                //Clear the Interrupt Source bit for FIFO
                SBE_GLOBAL->sbeIntrSource.clearIntrSource(SBE_INTERRUPT_ROUTINE,
                                                          curInterface);
                SBE_GLOBAL->sbeFifoCmdHdr.cmdClass = SBE_CMD_CLASS_UNKNOWN;
                SBE_GLOBAL->sbeCmdRespHdr.init();
                uint32_t len = sizeof(SBE_GLOBAL->sbeFifoCmdHdr)/sizeof(uint32_t);
                l_rc = sbeUpFifoDeq_mult ( len,
                (uint32_t *)&SBE_GLOBAL->sbeFifoCmdHdr, false, false, fifoType);
                // EOT is not expected, Don't flush, SBE_FIFO type
                // If FIFO reset is requested,
                if (l_rc == SBE_FIFO_RESET_RECEIVED)
                {
                    SBE_ERROR(SBE_FUNC "SBE Interface = 0x%08x reset received",
                                       curInterface);
                    break;
                }

                // If we received EOT out-of-sequence
                if ( (l_rc == SBE_SEC_UNEXPECTED_EOT_INSUFFICIENT_DATA)  ||
                     (l_rc == SBE_SEC_UNEXPECTED_EOT_EXCESS_DATA) )
                {
                    SBE_ERROR(SBE_FUNC"sbeUpFifoDeq_mult failure with SBE FIFO,"
                        " l_rc=[0x%08X]", l_rc);
                    SBE_GLOBAL->sbeCmdRespHdr.setStatus(SBE_PRI_INVALID_DATA, l_rc);

                    // Reassign l_rc to Success to Unblock command processor
                    // thread and let that take the necessary action.
                    l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
                    break;
                }
                l_cmdClass  = SBE_GLOBAL->sbeFifoCmdHdr.cmdClass;
                l_command   = SBE_GLOBAL->sbeFifoCmdHdr.command;
                SBE_INFO(SBE_FUNC" cmdclass = 0x%08x, command = 0x%08x",
                                 l_cmdClass, l_command);
            }// end else if loop for FIFO interface chipOp handling
            // Any other FIFO access issue
            if ( l_rc != SBE_SEC_OPERATION_SUCCESSFUL)
            {
                SBE_INFO(SBE_FUNC" some fifo access issue 0x%08x",l_rc);
                break;
            }
            // validate the command class and sub-class opcodes
            l_rc = sbeValidateCmdClass (l_cmdClass, l_command);

            if (l_rc)
            {
                // Command Validation failed;
                SBE_ERROR(SBE_FUNC"Command validation failed");
                SBE_GLOBAL->sbeCmdRespHdr.setStatus(SBE_PRI_INVALID_COMMAND, l_rc);

                // Reassign l_rc to Success to Unblock command processor
                // thread and let that take the necessary action.
                l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
                break;
            }
            // Need to return from receiver thread itself for fenced rejection
            // of command, but there might be contention on the response sent
            // over FIFO/Mailbox usage.
            sbeChipOpRc_t cmdAllowedStatus = sbeIsCmdAllowed(l_cmdClass, l_command);
            if( !cmdAllowedStatus.success() )
            {
                SBE_GLOBAL->sbeCmdRespHdr.setStatus(cmdAllowedStatus.primStatus,
                            cmdAllowedStatus.secStatus);
                l_rc = cmdAllowedStatus.secStatus;
                break;
            }
        } while (false); // Inner do..while ends

        SBE_GLOBAL->sbeIntrSource.setIntrSource(SBE_RX_ROUTINE, curInterface );

        // If there was a FIFO reset request,
        if (l_rc == SBE_FIFO_RESET_RECEIVED)
        {
            // Perform FIFO Reset
            l_rc = sbeUpFifoPerformReset(fifoType);
            if (l_rc)
            {
                // Perform FIFO Reset failed
                SBE_ERROR(SBE_FUNC"Perform FIFO Reset failed, "
                                "l_rc=[0x%08X]", l_rc);
                // Collect FFDC?
            }

            SBE_INFO(SBE_FUNC "clearing all interrupt sources ..");
            SBE_GLOBAL->sbeIntrSource.clearIntrSource(SBE_ALL_HANDLER,
                       (sbeInterfaceSrc_t) ( SBE_INTERFACE_MASK_RESET_ALL |
                                             SBE_INTERFACE_MASK_DATA_ALL ));

            SBE_GLOBAL->activeUsFifo = SBE_FIFO_UNKNOWN;
            SBE_GLOBAL->activeInterface = SBE_INTERFACE_UNKNOWN;

            pk_irq_enable(SBE_IRQ_SBEFIFO_DATA);
            pk_irq_enable(SBE_IRQ_SBEFIFO_RESET);
            continue;
        } // FIFO reset handling ends

        // Unblock the command processor thread
        // if we could dequeue the header successfully,
        if ((l_rcPk == PK_OK) && (l_rc == SBE_SEC_OPERATION_SUCCESSFUL))
        {
            l_rcPk = pk_semaphore_post(&SBE_GLOBAL->sbeSemCmdProcess);
        }

        // Handle Cmd not in a valid state here

        if ((l_rcPk != PK_OK) || (l_rc != SBE_SEC_OPERATION_SUCCESSFUL))
        {
            if( (l_rc != SBE_SEC_COMMAND_NOT_ALLOWED_IN_THIS_STATE) &&
                (l_rc != SBE_SEC_BLACKLISTED_CHIPOP_ACCESS ))
            {
                // It's likely a code bug or PK failure,
                // or any other PSU/FIFO access (scom) failure.

                // Add Error trace, collect FFDC and
                // continue wait for the next interrupt
                SBE_ERROR(SBE_FUNC"Unexpected failure, "
                    "l_rcPk=[%d], SBE_GLOBAL->sbeSemCmdProcess.count=[%d], l_rc=[%d]",
                    l_rcPk, SBE_GLOBAL->sbeSemCmdProcess.count, l_rc);
                pk_halt();
            }

            sbeHandleFifoResponse(l_rc, fifoType);
            SBE_GLOBAL->sbeIntrSource.clearIntrSource(SBE_ALL_HANDLER,
                                                curInterface);
            SBE_GLOBAL->activeUsFifo = SBE_FIFO_UNKNOWN;
            SBE_GLOBAL->activeInterface = SBE_INTERFACE_UNKNOWN;

            pk_irq_enable(SBE_IRQ_SBEFIFO_DATA);
            pk_irq_enable(SBE_IRQ_SBEFIFO_RESET);
            continue;
        }

        SBE_DEBUG(SBE_FUNC"Posted SBE_GLOBAL->sbeSemCmdProcess, "
               "SBE_GLOBAL->sbeSemCmdProcess.count=[%d]", SBE_GLOBAL->sbeSemCmdProcess.count);

    } while (true); // thread always exists
    #undef SBE_FUNC
}
