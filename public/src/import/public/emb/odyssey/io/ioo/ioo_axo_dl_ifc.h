/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_axo_dl_ifc.h $ */
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
// *! (C) Copyright International Business Machines Corp. 2018
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_axo_dl_ifc.h
// *! TITLE       :
// *! DESCRIPTION : Functions for handling the AXO DL Interface
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr21020800 |vbr     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _IOO_AXO_DL_IFC_H_
#define _IOO_AXO_DL_IFC_H_

#include "io_lib.h"

void dl_init_req (t_gcr_addr* io_gcr_addr, const uint32_t i_num_lanes);
void dl_recal_req(t_gcr_addr* io_gcr_addr, const uint32_t i_num_lanes, const uint32_t i_recal_req_vec);
uint32_t get_dl_recal_req_vec(t_gcr_addr* io_gcr_addr);

#endif //_IOO_AXO_DL_IFC_H_
