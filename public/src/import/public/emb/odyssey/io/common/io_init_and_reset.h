/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_init_and_reset.h $ */
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
// *! (C) Copyright International Business Machines Corp. 2021
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_init_and_reset.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr21020500 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _IO_INIT_AND_RESET_H_
#define _IO_INIT_AND_RESET_H_

#include "io_lib.h"


// Reset and Re-initialize per-lane HW Regs. Also clear per-lane mem_regs.
int io_reset_lane(t_gcr_addr* gcr_addr);

// Power up a lane (both RX and TX)
int io_lane_power_on(t_gcr_addr* gcr_addr, bool enable_dl_clk);

// Power down a lane (both RX and TX)
int io_lane_power_off(t_gcr_addr* gcr_addr);


#ifdef IOO
    #include "ioo_init_and_reset.h"
#endif
#ifdef IOT
    #include "iot_init_and_reset.h"
#endif

#endif //_IO_INIT_AND_RESET_H_
