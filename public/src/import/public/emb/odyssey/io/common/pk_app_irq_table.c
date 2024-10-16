/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/pk_app_irq_table.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : pk_app_irq_table.c
// *! TITLE       :
// *! DESCRIPTION : IRQ tables
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr19100300 |vbr     | Removing old P9 IRQ handlers.
// vbr17051800 |vbr     | Remove IOF code.
// vbr17020800 |vbr     | Disabled GCR interrupt since registers no loner exist.
// vbr16061000 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "pk.h"
#include "io_irq_handlers.h"


//Example parameter list for passing to an IRQ handler
//uint32_t params[2] = {0, 1};


/////////////////////////////////
// P11 PHY PPE IRQ Map
/////////////////////////////////
// 00     group0_ioppe_servo_intr
// 01     group1_ioppe_servo_intr
// 02     group2_ioppe_servo_intr
// 03     group3_ioppe_servo_intr
// 23     Unconnected (P11), P10: dl2phy_ppe_data_valid (incoming message request from dl to phy)
// 24     scom_ppe_func ppe_interrupt register
// Other  Unconnected

EXTERNAL_IRQ_TABLE_START
//IRQ_HANDLER(io_example_irq_handler, &params[0])  // 00 Example IRQ Handler - replaces the appropriate default handler (by position)
IRQ_HANDLER_DEFAULT                              // 00
IRQ_HANDLER_DEFAULT                              // 01
IRQ_HANDLER_DEFAULT                              // 02
IRQ_HANDLER_DEFAULT                              // 03
IRQ_HANDLER_DEFAULT                              // 04
IRQ_HANDLER_DEFAULT                              // 05
IRQ_HANDLER_DEFAULT                              // 06
IRQ_HANDLER_DEFAULT                              // 07
IRQ_HANDLER_DEFAULT                              // 08
IRQ_HANDLER_DEFAULT                              // 09
IRQ_HANDLER_DEFAULT                              // 10
IRQ_HANDLER_DEFAULT                              // 11
IRQ_HANDLER_DEFAULT                              // 12
IRQ_HANDLER_DEFAULT                              // 13
IRQ_HANDLER_DEFAULT                              // 14
IRQ_HANDLER_DEFAULT                              // 15
IRQ_HANDLER_DEFAULT                              // 16
IRQ_HANDLER_DEFAULT                              // 17
IRQ_HANDLER_DEFAULT                              // 18
IRQ_HANDLER_DEFAULT                              // 19
IRQ_HANDLER_DEFAULT                              // 20
IRQ_HANDLER_DEFAULT                              // 21
IRQ_HANDLER_DEFAULT                              // 22
IRQ_HANDLER_DEFAULT                              // 23
IRQ_HANDLER_DEFAULT                              // 24
IRQ_HANDLER_DEFAULT                              // 25
IRQ_HANDLER_DEFAULT                              // 26
IRQ_HANDLER_DEFAULT                              // 27
IRQ_HANDLER_DEFAULT                              // 28
IRQ_HANDLER_DEFAULT                              // 29
IRQ_HANDLER_DEFAULT                              // 30
IRQ_HANDLER_DEFAULT                              // 31
EXTERNAL_IRQ_TABLE_END
