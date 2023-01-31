/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/hwp_includes.H $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017,2023                        */
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

#ifndef __HWP_INCLUDES_H
#define __HWP_INCLUDES_H

// MEM HWP Includes
#include <ody_scominit.H>
#include <ody_ddrphyinit.H>
#include <ody_load_imem.H>
#include <ody_load_dmem.H>
#include <ody_sppe_draminit.H>
#include <ody_load_pie.H>
#include <ody_draminit_mc.H>
#include <ody_enable_ecc.H>
#include <ody_thermal_init.H>

// IO HWP Includes
#include <ody_omi_hss_ppe_load.H>
#include <ody_omi_hss_config.H>
#include <ody_omi_hss_ppe_start.H>
#include <ody_omi_hss_bist_init.H>
#include <ody_omi_hss_bist_start.H>
#include <ody_omi_hss_bist_poll.H>
#include <ody_omi_hss_bist_cleanup.H>
#include <ody_omi_hss_init.H>
#include <ody_omi_hss_dccal_start.H>
#include <ody_omi_hss_dccal_poll.H>
#include <ody_omi_hss_tx_zcal.H>
#include <ody_omi_pretrain_adv.H>
#include <ody_omi_setup.H>
#include <ody_omi_train.H>
#include <ody_omi_train_check.H>
#include <ody_omi_posttrain_adv.H>

#endif
