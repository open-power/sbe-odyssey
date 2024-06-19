/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_dfe.h $        */
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
// *! FILENAME    : rx_eo_dfe.h
// *! TITLE       :
// *! DESCRIPTION :
// *!
// *! OWNER NAME  : Chris Steffen       Email: cwsteffen@us.ibm.com
// *! BACKUP NAME :
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr21040700 |vbr     | Added way to select to run H1 or H1-H3 for DFE fast
// vbr20021300 |vbr     | Added record_min_eye_height flag to dfe_full
// mbs19041001 |mbs     | Added recal flag to dfe_full function (HW486784)
// cws18071000 |cws     | Initial Rev
// -----------------------------------------------------------------------------

#ifndef __RX_EO_DFE_H__
#define __RX_EO_DFE_H__

#include "ioo_common.h"


/**
 * @brief The procedure calculates and sets proper latch dac values. The dac
 *   values represent latch offset, path offset, and dfe.
 *
 * @param[in] i_gcr_addr   References the unit target
 * @param[in] i_cal_mode   Used to determine GenX and whether to run H1 or H1-H3
 *
 * @return uint32_t. rc_pass if success, else error code.
 *
 */
uint32_t rx_eo_dfe_fast(t_gcr_addr* i_gcr_addr, t_init_cal_mode i_cal_mode);

uint32_t rx_eo_dfe_force(t_gcr_addr* i_gcr_addr);

/**
 * @brief The procedure calculates and sets proper latch dac values. The dac
 *   values represent latch offset, path offset, and dfe.
 *
 * @param[in] i_gcr_addr               References the unit target
 * @param[in] i_bank                   References the bank to be calibrated
 * @param[in] i_run_all_quads          True to force running all 4 quadrants; otherwise follow rx_dfe_full_mode
 * @param[in] i_hyst_en                True when hysteresis is being applied
 * @param[in] i_enable_min_eye_height  True when the min eye height checking and logging are enabled
 * @param[in] i_disable_pattern_filter True to set H3-H1 as don't care in servos, False to match H3-H1 to the latch
 *
 * @return uint32_t. rc_pass if success, else error code.
 *
 */
uint32_t rx_eo_dfe_full(t_gcr_addr* i_gcr_addr, const t_bank i_bank, bool i_run_all_quads, bool i_hyst_en,
                        bool i_enable_min_eye_height, bool i_disable_pattern_filter);

#endif // __RX_EO_DFE_H__
