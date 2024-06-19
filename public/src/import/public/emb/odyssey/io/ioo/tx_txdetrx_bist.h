/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_txdetrx_bist.h $ */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : tx_txdetrx_bist.h
// *! TITLE       :
// *! DESCRIPTION : TX Signal Detect Circuit BIST .
// *!             :
// *!
// *! OWNER  NAME : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh22032000 |mwh     | Initial Code
//------------------------------------------------------------------------------

#ifndef _TX_TXDETRX_BIST_H_
#define _TX_TXDETRX_BIST_H_

#include <stdbool.h>
#include "ioo_common.h"

// tx_txdetrx_bist

int tx_txdetrx_bist(t_gcr_addr* gcr_addr, int tx_bist_enable_ls, int tx_bist_enable_hs);

#endif // _TX_TXDETRX_BIST_H_
