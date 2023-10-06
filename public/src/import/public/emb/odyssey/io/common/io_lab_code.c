/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_lab_code.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_lab_code.c
// *! TITLE       :
// *! DESCRIPTION : Lab specific code
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr22060200 |vbr     | Initial Rev (space holder)
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "io_lab_code.h"

#include "ppe_img_reg_const_pkg.h"
#include "ppe_fw_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


////////////////////////////////////////////////////////////////
// Per-Group
////////////////////////////////////////////////////////////////
int io_lab_code_pg(t_gcr_addr* io_gcr_addr)
{
    set_debug_state(0xF500); // DEBUG - Lab Code PG

    return rc_no_error;
} //io_lab_code_pg()


////////////////////////////////////////////////////////////////
// Per-Lane (RX)
////////////////////////////////////////////////////////////////
int io_lab_code_pl_rx(t_gcr_addr* io_gcr_addr)
{
    set_debug_state(0xF510); // DEBUG - Lab Code PL RX

    // TODO - Space reservation that should be deleted
    reserve_256B;
    reserve_256B;
    reserve_256B;
    reserve_256B;

    return rc_no_error;
} //io_lab_code_pl_rx()


////////////////////////////////////////////////////////////////
// Per-Lane (TX)
////////////////////////////////////////////////////////////////
int io_lab_code_pl_tx(t_gcr_addr* io_gcr_addr)
{
    set_debug_state(0xF520); // DEBUG - Lab Code PL TX

    // Switch to the TX group
    set_gcr_addr_reg_id(io_gcr_addr, tx_group);

    // TODO - Space reservation that should be deleted
    reserve_256B;
    reserve_256B;
    reserve_256B;
    reserve_256B;

    // Return to RX group on exit
    set_gcr_addr_reg_id(io_gcr_addr, rx_group);
    return rc_no_error;
} //io_lab_code_pl_tx()
