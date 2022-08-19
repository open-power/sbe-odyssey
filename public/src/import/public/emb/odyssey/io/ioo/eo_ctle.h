/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_ctle.h $       */
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
// *! FILENAME    : eo_ctle.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : John Gullickson     Email: gullicks@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// vbr21090800 |vbr     | Updated for P11
// vbr21040700 |vbr     | Added cal_mode input
// jfg20030900 |jfg     | HW525009 add a rough_only mode to set initial coarse peak 1&2
// jfg19091100 |jfg     | Add first_run parm to eo_ctle
// vbr19011600 |vbr     | HW473413: Do not use CDR quad mode tracking.
// jfg16042300 |jfg     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef _EO_CTLE_H_
#define _EO_CTLE_H_

#include <stdbool.h>
#include "ioo_common.h"

// eo_ctle():
// cal_mode       Input  Specify AXO or PCIe Gen X (for determining which peak controls are calibrated)
// cal_bank       Input  Specify the (Alt) bank being calibrated (bank_a or bank_b)
// copy_to_main   Input  Specify copying results to the Main bank (meant for copying from A to B in initial cal)
// start_at_zero  Input  Specify that servo ops should start from 0 rather than previous value; this should be set only on the first run on a channel (which changes with TXEQ, PCIe Speed, etc).
// recal          Input  Specify if this is an initial training or a recal (1st recal on bank B is still a recal)
// hysteresis_en  Input  Enable the hysteresis (during recal, after the first pass on bank B)
// peak_changed   Output Indicate the main loop should repeat due to a new peak value
int eo_ctle(t_gcr_addr* gcr_addr, t_init_cal_mode cal_mode, t_bank cal_bank, bool copy_to_main, bool start_at_zero,
            bool recal, bool hysteresis_en, bool* peak_changed);

#endif //_EO_CTLE_H_
