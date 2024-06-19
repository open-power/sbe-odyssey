/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_eoff.h $       */
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
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_loff.h
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
// mwh20100800 |mwh     | Removed vga_loop and replaced with first_recal -- vga_loop not need anymore
// mwh20100800 |mwh     | Add in bist_check to turn on or off the bist checks
// -----------------------------------------------------------------------------
// mwh19032100 |mwh     | Add in vga_llop_count
// -----------------------------------------------------------------------------
// mwh18062100 |mwh     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_EOFF_H_
#define _EO_EOFF_H_

#include <stdbool.h>
#include "ioo_common.h"

// Live data edge offset

int eo_eoff(t_gcr_addr* gcr_addr, bool first_recal, t_bank bank);



#endif //_EO_EOFF_H_
