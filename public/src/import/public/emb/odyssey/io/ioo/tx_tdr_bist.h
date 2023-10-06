/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/tx_tdr_bist.h $   */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : tx_tdr_bist.c
// *! TITLE       :
// *! DESCRIPTION : TDR (Time Domain Reflectometry) BIST test PPE code.
// *!             :
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : John Bergkvist      Email: john.bergkvist.jr@ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mwh23071200 |mwh     | Initial Code
//------------------------------------------------------------------------------

#ifndef _TX_TDR_BIST_H_
#define _TX_TDR_BIST_H_

#include <stdbool.h>
#include "ioo_common.h"

// tx_tdr_bist

int tx_tdr_bist(t_gcr_addr* gcr_addr);

#endif //_TX_TDR_BIST_H_
