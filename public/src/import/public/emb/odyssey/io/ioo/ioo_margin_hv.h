/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_margin_hv.h $ */
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
// *! FILENAME    : ioo_margin_hv.h
// *! TITLE       :
// *! DESCRIPTION : loff_offset_wrap for vertical offset manual and auto-margining capability
// *|               pr_offset_wrap for horizontal offset manual and auto-margining capability
// *!
// *! OWNER NAME  : John Gullickson     Email: gullicks@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// jfg23022400 |jfg     | Companion to ioo_margin_hv.c
// -----------------------------------------------------------------------------


#ifndef _IOO_MARGIN_HV_
#define _IOO_MARGIN_HV_

// Software State Machine State Variables

#define offset_st_H_on  0b0000
#define offset_st_H_off 0b0100
#define offset_st_H_pos 0b0000
#define offset_st_H_neg 0b0010
//efine offset_st_H_end 0b0011
#define offset_st_H_end 0b0101

#define offset_st_V_on  0b1000
#define offset_st_V_off 0b1100
#define offset_st_V_pos 0b1000
#define offset_st_V_neg 0b1010
#define offset_st_V_end 0b1101

#define offset_mask_on  0b0100
#define offset_mask_HV  0b1100
#define offset_mask_PN  0b0010
#define offset_mask_Bnk 0b0001


PK_STATIC_ASSERT(ppe_margin_offset_overlay_width == 4);
PK_STATIC_ASSERT(ppe_pr_offset_applied_a_startbit - ppe_margin_offset_overlay_startbit == 0);
PK_STATIC_ASSERT(ppe_pr_offset_applied_b_startbit - ppe_margin_offset_overlay_startbit == 1);
PK_STATIC_ASSERT(ppe_loff_offset_applied_a_startbit - ppe_margin_offset_overlay_startbit == 2);
PK_STATIC_ASSERT(ppe_loff_offset_applied_b_startbit - ppe_margin_offset_overlay_startbit == 3);

void loff_offset_wrap (t_gcr_addr* gcr_addr, t_bank bank, bool apply) ;
void pr_offset_wrap (t_gcr_addr* gcr_addr, t_bank bank, bool apply);


#endif //_IOO_MARGIN_HV_
