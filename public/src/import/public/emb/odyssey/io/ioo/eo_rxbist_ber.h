/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_rxbist_ber.h $ */
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
// *! FILENAME    : eo_rxbist_ber.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Mike Harper         Email: harperm@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// bja20011500 |bja     | Add lane_mask to eo_rxbist_ber
// mwh1903220  |mwh     | Add another input
// mwh1902190  |mwh     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_rxbist_ber_H_
#define _EO_rxbist_ber_H_

#include <stdbool.h>
#include "ioo_common.h"

// vga ctle lte qpa checks
int eo_rxbist_ber(t_gcr_addr* gcr_addr, const uint32_t lane_mask, const t_bank bank);
int min_pr_shift(t_gcr_addr* gcr_addr, int min_shift, int stop_value, const uint32_t lane_mask, t_bank bank);

void step_pr_to_zero( t_gcr_addr*, t_bank );


//int check_value(t_gcr_addr *gcr_addr, t_bank bank, int check_value, char addr_a,char startbit_a,char endbit_a, char addr_b,char startbit_b,char endbit_b);
#endif //_EO_rxbist_ber_H_
