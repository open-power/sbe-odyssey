/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_ddc.h $        */
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
// *! FILENAME    : eo_ddc.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : John Gullickson     Email: gullicks@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// jfg23033100 |jfg     | Coverage-based obsolete code cleanup: pr_seek_ber remove all edge support. Only data is manipulated.
// jfg22111701 |jfg     | Remove PR defines in lieu of common read_active_pr
// jfg21120100 |jfg     | Add constants and function defs for new eo_ddc.c
// jfg20090100 |jfg     | HW532333 Move offset changes to eo_main
// jfg18081000 |jfg     | Modified for new function def
// vbr16021600 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_DDC_H_
#define _EO_DDC_H_

#include "ioo_common.h"

#define max_eye 0xFFFF
#define seek_error -1

int  pr_recenter_werr(t_gcr_addr* gcr_addr, t_bank bank, int* pr_vals, uint32_t* Esave, uint32_t* Dsave, int* Doffset,
                      int Eoffset, bool ret_err);
static inline int  pr_recenter(t_gcr_addr* gcr_addr, t_bank bank, int* pr_vals, uint32_t* Esave, uint32_t* Dsave,
                               int* Doffset, int Eoffset)
{
    return pr_recenter_werr(gcr_addr, bank, pr_vals, Esave, Dsave, Doffset, Eoffset, false);
}
int pr_seek_ber (t_gcr_addr* gcr_addr, t_bank bank, unsigned int Dstep, bool dirL1R0, t_seek seek_edge, int* pr_vals);
// Run DDC on a lane and update historical width
int eo_ddc(t_gcr_addr* gcr_addr, t_bank bank, bool recal);

#endif //_EO_DDC_H_
