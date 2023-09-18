/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeFifoMsgUtils.H $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
 * @file: ppe/sbe/sbefw/sbeFifoMsgUtils.H
 *
 * @brief  This file contains the SBE FIFO Access Common Utility Functions
 *
 */

#ifndef __SBEFW_SBEFIFOMSGUTILS_H
#define __SBEFW_SBEFIFOMSGUTILS_H

#include <stdint.h>
#include "sbefifo.H"

/**********************************************************************/
// SBE Utilities
/**********************************************************************/

/**
  * @brief sbeUpFifoDeq_mult : Dequeue multiple entries from upstream FIFO
  *
  * @param[in/out] io_len
  *    On input: Non-zero number of entries (excluding EOT) to dequeue.
  *        Ignored when i_flush == true.
  *    On output: Number of entries dequeued (excluding EOT).
  * @param[out]    o_pData  Entries dequeued into the buffer
  * @param[in]     i_isEotExpected true / false
  *    true - Default case.
  *        Caller expects an EOT entry after io_len entries are
  *        dequeued. Accordingly, this function will attempt to dequeue
  *        the EOT entry after io_len entries are dequeued.
  *    false - Caller doesn't expect an EOT after io_len entries are
  *        dequeued. Accordingly, this function will not attempt to
  *        dequeue the EOT entry after io_len entries are dequeued.
  * @param[in]     i_flush true / false
  *    true - caller requested FIFO flush
  *        Usually caller marks this flag as true to handle error scenario.
  *        All entries written in the US fifo (until an EOT is encountered),
  *        would be dequeued and discarded (not processed). Note that io_len
  *        and i_isEotExpected inputs are ignored in this case.
  *        However, flag i_isEotExpected is always interpreted as true in
  *        case.
  *    false - Default good path.
  *        US Fifo entries will be dequeued and processed per inputs io_len
  *        and i_isEotExpected.
  * @param[in]   i_type FIFO type.
  *              The default fifo type is FIFO:0
  *
  * @return  Return Code  SUCCESS or a secondary response code
  *                SBE_SEC_OPERATION_SUCCESSFUL
  *                SBE_SEC_FIFO_ACCESS_FAILURE
  *                SBE_SEC_UNEXPECTED_EOT_INSUFFICIENT_DATA
  *                SBE_SEC_UNEXPECTED_EOT_EXCESS_DATA
  *                SBE_FIFO_RESET_RECEIVED
  *
  */
extern uint32_t sbeUpFifoDeq_mult (uint32_t    &io_len,
                                   uint32_t    *o_pData,
                                   const bool  i_isEotExpected = true,
                                   const bool  i_flush = false,
                                   sbeFifoType i_type = SBE_FIFO);


/**
  * @brief sbeDownFifoEnq_mult : Enqueue into downstream FIFO
  *
  * @param[in/out] io_len   number of entries to enqueue as input,
  *                         number of entries enqueued as output
  * @param[in]     i_pData  buffer containting data to be enqueued
  * @param[in]     i_type FIFO type.
  *                The default fifo type is FIFO:0
  *
  * @return        Rc  SUCCESS or a secondary response code
  *                SBE_SEC_OPERATION_SUCCESSFUL
  *                SBE_SEC_FIFO_ACCESS_FAILURE
  *                SBE_FIFO_RESET_RECEIVED
  *
  */
extern uint32_t sbeDownFifoEnq_mult (uint32_t        &io_len,
                                     const uint32_t *i_pData,
                                      sbeFifoType i_type = SBE_FIFO) ;

/**
  * @brief sbeBuildRespHeaderStatusWordGlobal
  *          Builds the status header word from global variables
  *
  * @return     Returns the status word in the response header
  *
  */
extern inline uint32_t sbeBuildRespHeaderStatusWordGlobal (void);

/**
  * @brief sbeBuildRespHeaderStatusWordLocal
  *          Builds the status header word from passed in parameters
  *
  * @param[in]  const uint16_t i_primStatus Primary Response Status Code
  * @param[in]  const uint16_t i_secStatus  Secondary Response Status Code
  *
  * @return     Returns the status word in the response header
  *
  */
extern inline uint32_t sbeBuildRespHeaderStatusWordLocal (
                                const uint16_t i_primStatus,
                                const uint16_t i_secStatus)
{
    return ( (((uint32_t)i_primStatus)<<16) | (i_secStatus) );
}

/**
  * @brief sbeDownFifoSignalEot : Signal EOT in Downstream FIFO
  * @param[in]  i_type FIFO type.
  *             The default fifo type is FIFO:0
  *
  * @return     Rc from the underlying scom utility
  *
  * @note       This is a blocking call. If FIFO is full, it will wait
  *             in loop ( sleep ) till the time there is some space in
  *             FIFO.
  */
uint32_t sbeDownFifoSignalEot ( sbeFifoType i_type = SBE_FIFO);

/**
 * @brief sbeDsSendRespHdr : Send response header to DS FIFO
 *           - This also sends the FFDC if exist.
 *
 * @param[in]    i_hdr     Response Header
 * @param[in]    i_ffdc    Pointer to FFDC object, if NULL FFDC package
 *                         is not sent in the chip op response
 * @param[in]    i_type    FIFO type
 *                         The default fifo type is FIFO:0
 *
 * @return                 Rc from the underlying scom utility
 */
uint32_t sbeDsSendRespHdr(const sbeRespGenHdr_t &i_hdr,
                          sbeResponseFfdc_t *i_ffdc=NULL,
                          sbeFifoType i_type = SBE_FIFO );

/**
 * @brief sbeFifoGetSource : Find where the request originated from
 *
 * @param[in]    reset     Reset interrupt.
 *
 * @return                 FIFO type
 */
sbeFifoType sbeFifoGetSource (bool reset);

/**
 * @brief sbeFifoGetInstSource : Find SBE Interface source
 *
 * @param[in]    upFifoType   Upstream Fifo type
 * @param[in]    reset        Reset interrupt
 *
 * @return                    SBE interface source.
 */
sbeInterfaceSrc_t sbeFifoGetInstSource (sbeFifoType upFifoType, bool reset);


/**
 * @brief sbePipeGetSource: Find which pipe the request originated from
 * @return                  PIPE type
 */
sbeFifoType sbePipeGetSource ( );

/**
 * @brief sbeHandleDsPipeCnfg: Check and update the downstream pipe config
 *        based on the upstream pipe
 *
 * @param[in]    i_usPipe  Upstream pipe
 *
 * @return                 Return code. 0 on success
 * @note
 * Contract (pre-RIT) until product Pipe Pair Assignments are frozen:
 * 1. Chip-Op command will not be accepted on the last Pipe 8
 * 2. Chip-Op response will be sent on command(upstream) pipe+1 (next pipe)
 * 3. Caller of the chip-op is expected to set up access ctrl and ctlr id regs
 * 4. SBE will set up downstream (response) pipe if #3 above was not done,
 *    provided it was interrupted (upstream was set up correctly)
 */
uint32_t sbeHandleDsPipeCnfg (const sbeFifoType  i_usPipe);

#endif // __SBEFW_SBEFIFOMSGUTILS_H
