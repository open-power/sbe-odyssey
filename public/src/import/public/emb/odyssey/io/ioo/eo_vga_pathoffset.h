/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_vga_pathoffset.h $ */
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
// *! FILENAME    : eo_vga_pathoffset.h
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
// mwh23051800 |mwh     | change to void eo_vga_pathoffset
// jjb22112200 |jjb     | added pcie_bist_mode parameters
// mwh21060100 |mwh     | Add recal to inputs for Gen 3 recal
// mwh21052000 |mwh     | Add first_recal,gen3,2,1
// mwh21041600 |mwh     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_VGA_PATHOFFSET_H_
#define _EO_VGA_PATHOFFSET_H_

#include <stdbool.h>
#include "ioo_common.h"

// VGA Gain loop portion of VGA
void eo_vga_pathoffset(t_gcr_addr* gcr_addr, t_bank bank, int saved_Amax_poff, bool pcie_bist_mode);



#endif //_EO_VGA_PATHOFFSET_H_
